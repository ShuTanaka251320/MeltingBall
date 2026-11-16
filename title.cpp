// =================================================
// 
// タイトル処理[title.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "title.h"
#include "object.h"
#include "object2D.h"
#include "object3D.h"
#include "objectX.h"
#include "macro.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"
#include "manager.h"
#include "fade.h"
#include "camera.h"
#include "light.h"
#include "meshfield.h"
#include "particle.h"
#include "sound.h"
#include "math.h"

// =================================================
// マクロ定義
#define NUM_PLAY	(0)	// ゲームスタートの選択番号
#define NUM_QUIT	(1)	// ゲーム終了の選択番号

// =================================================
// 静的メンバ変数宣言
CObject2D* CTitle::m_pRogo = NULL;	// ロゴのポインタ
CObject2D* CTitle::m_pInfo = NULL;	// pressstartのポインタ
CObject2D* CTitle::m_pStartUI = NULL;	// ゲームスタートのポリゴン用
CObject2D* CTitle::m_pQuitUI = NULL;	// ゲーム終了のポリゴン用
CObject2D* CTitle::m_pBG = NULL;	// 選択肢背景のポリゴン用
int CTitle::m_nIdxTex = -1;		// テクスチャのインデックス
int CTitle::m_nCntFade = 0;		// 自動で画面遷移するまでのカウンター
CCamera* CTitle::m_pCamera = NULL;	// カメラのポインタ
CLight* CTitle::m_pLight = NULL;	// ライトのポインタ
CMeshField* CTitle::m_pMesh = NULL;		// メッシュのポインタ
CObject3D* CTitle::m_pSide = NULL;		// 限界値のビルポリゴン
CObjectX* CTitle::m_pPlayer = NULL;		// プレイヤーのポインタ
int CTitle::m_nSelect = 0;		// 選択番号

// =================================================
// コンストラクタ
// =================================================
CTitle::CTitle(CScene::MODE mode) :CScene(mode)
{

}

// =================================================
// デストラクタ
// =================================================
CTitle::~CTitle()
{

}

// =================================================
// 初期化処理
// =================================================
HRESULT CTitle::Init(void)
{
	CTexture* pTexture = CManager::GetTexture();	// テクスチャの取得
	CSound* pSound = CManager::GetSound();	// サウンドポインタ
	pSound->PlaySound(CSound::SOUND_LABEL_TITLEBGM);	// BGM再生

	if (m_pCamera == NULL)
	{// カメラ
		m_pCamera = new CCamera;
	}

	if (m_pLight == NULL)
	{// ライト
		m_pLight = new CLight;
	}

	// --- カメラの初期化 --- //
	m_pCamera->Init();

	// --- ライトの初期化 --- //
	m_pLight->Init();

	m_pPlayer = CObjectX::Create({ 0.0f, -0.0f, -150.0f }, VEC_NULL, CObject::TYPE_PLAYER, "data\\model\\player.x");

	// --- UIの生成 --- //
	m_pRogo = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		800.0f,
		250.0f,
		CObject::TYPE_BG);

	int nIdxTex = pTexture->Register("data\\TEXTURE\\rogo.png");
	m_pRogo->SetTexture(nIdxTex);

	// --- ゲームスタートUIの生成 --- //
	m_pStartUI = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2 - 200.0f, SCREEN_HEIGHT - 200.0f, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		400.0f,
		200.0f,
		CObject::TYPE_BG);

	nIdxTex = pTexture->Register("data\\TEXTURE\\play.png");
	m_pStartUI->SetTexture(nIdxTex);

	// --- ゲーム終了UIの生成 --- //
	m_pQuitUI = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2 + 200.0f, SCREEN_HEIGHT - 200.0f, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		400.0f,
		200.0f,
		CObject::TYPE_BG);

	nIdxTex = pTexture->Register("data\\TEXTURE\\exequit.png");
	m_pQuitUI->SetTexture(nIdxTex);

	m_nCntFade = 1200;	// フェードカウンタの初期設定
	m_nSelect = NUM_PLAY;	// 初期選択番号

	FILE* pFile;
	int nRank = 10;

	// --- 外部ファイルに更新順位をを保存 --- //
	pFile = fopen("data\\scripts\\rank.txt", "w");

	if (pFile != NULL)
	{
		fprintf(pFile, "%d\n", nRank);
	}
	else
	{// ファイルへの保存に失敗

	}

	fclose(pFile);

	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CTitle::Uninit(void)
{
	m_pRogo = NULL;	// ロゴのポインタ
	m_pInfo = NULL;	// pressstartのポインタ
	m_nIdxTex = -1;		// テクスチャのインデックス
	m_nCntFade = 0;		// 自動で画面遷移するまでのカウンター
	m_pMesh = NULL;		// メッシュのポインタ
	m_pSide = NULL;		// 限界値のビルポリゴン
	m_pBG = NULL;

	delete m_pCamera;
	m_pCamera = NULL;	// カメラのポインタ

	delete m_pLight;
	m_pLight = NULL;	// ライトのポインタ
}

// =================================================
// 更新処理
// =================================================
void CTitle::Update(void)
{
	// --- キーボードの取得 --- //
	CInputKeyboard* pInputkeyboard = CManager::GetInputKeyboard();
	CJoyPad* pJoyPad = CManager::GetJoyPad();
	CSound* pSound = CManager::GetSound();
	CFade* pFade = CManager::GetFade();

	if (pFade->GetFade() == CFade::FADE_NONE)
	{
		// --- カメラの更新処理 --- //
		m_pCamera->Update();

		// --- ライトの更新処理 --- //
		m_pLight->Update();

		// --- タイトルの拡縮 --- //
		m_pRogo->Expand(0.05f, 3.0f);

		// --- 選択処理 --- //
		Select();

		// --- タイトル画面に動きを出す処理 --- //
		D3DXVECTOR3 rot = m_pPlayer->GetRotation();

		rot.y += 0.05f;
		rot = CMath::RepairRot(&rot);	// 正規化

		m_pPlayer->SetRotation(rot);

		CParticle::Create(m_pPlayer->GetPosition(), VEC_NULL, { 5.0f, 2.0f, 5.0f }, { 0.0f, 5.0f, 0.0f }, COLOR_RED, 7.0f, 7.0f, 2, 30);
		CParticle::Create(m_pPlayer->GetPosition(), VEC_NULL, { 2.0f, 2.0f, 2.0f }, { 0.0f, 2.0f, 0.0f }, COLOR_ORANGE, 5.0f, 5.0f, 1, 10, 6);

		m_nCntFade--;	// 遷移カウンタの減少

		if (pInputkeyboard->GetTrigger(DIK_RETURN) == true
			|| pJoyPad->GetTrigger(CJoyPad::JOYKEY_START)
			|| pJoyPad->GetTrigger(CJoyPad::JOYKEY_A))
		{
			if (m_pBG != NULL) {// 背景ポリゴンの破棄
				m_pBG->Uninit();
				m_pBG = NULL;
			}

			if (pFade->GetFade() == CFade::FADE_NONE)
			{
				pSound->PlaySound(CSound::SOUND_LABEL_ENTER);	// 効果音の再生
			}

			if (m_nSelect == NUM_PLAY)
			{
				CManager::SetStopSound(true);	// BGM終了フラグを立てる
				CFade::SetFade(CScene::MODE_TUTORIAL);
			}
			else if (m_nSelect == NUM_QUIT)
			{
				CObject::ReleaseAll();
				//Uninit();
				SetFinish(true);
			}
		}

		if (m_nCntFade <= 0)
		{
			CManager::SetStopSound(false);	// BGM終了フラグを立てない
			CFade::SetFade(CScene::MODE_RANKING);
		}
	}

	CDebugProc* pDebugProc = CManager::GetDrbugProc();				// デバッグ表示のポインタ

	pDebugProc->PrintDebug("title\n");
}

// =================================================
// 描画処理
// =================================================
void CTitle::Draw(void)
{

}

// =================================================
// UIの生成
// =================================================
void CTitle::CreateUI(void)
{
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	m_pRogo = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		800.0f,
		250.0f,
		CObject::TYPE_BG);

	m_pInfo = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 100.0f, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		200.0f,
		200.0f,
		CObject::TYPE_BG);

	m_nIdxTex = pTexture->Register("data\\TEXTURE\\title.png");
	m_pRogo->SetTexture(m_nIdxTex);

	m_nIdxTex = pTexture->Register("data\\TEXTURE\\pressstart.png");
	m_pInfo->SetTexture(m_nIdxTex);
}

// =================================================
// 選択処理
// =================================================
void CTitle::Select()
{
	CInputKeyboard* pInputkeyboard = CManager::GetInputKeyboard();
	CJoyPad* pJoyPad = CManager::GetJoyPad();
	CSound* pSound = CManager::GetSound();

	if (m_nSelect == NUM_PLAY)
	{// PLAYが選択
		if (m_pBG != NULL) {// 背景ポリゴンの破棄
			m_pBG->Uninit();
			m_pBG = NULL;
		}

		if (m_pBG == NULL) {// 背景ポリゴンの生成
			m_pBG = CObject2D::Create(m_pStartUI->GetPosition(), VEC_NULL, VEC_NULL, COLOR_HALFWHITE, 200.0f, 100.0f, CObject::TYPE_BG, 4);
		}

		m_pStartUI->Expand(0.05f, 1.0f);
		m_pQuitUI->ResetLength();
		m_pQuitUI->ResetExpand();
	}
	if (m_nSelect == NUM_QUIT)
	{// QUITが選択
		if (m_pBG != NULL) {// 背景ポリゴンの破棄
			m_pBG->Uninit();
			m_pBG = NULL;
		}

		if (m_pBG == NULL) {// 背景ポリゴンの生成
			m_pBG = CObject2D::Create(m_pQuitUI->GetPosition(), VEC_NULL, VEC_NULL, COLOR_HALFWHITE, 200.0f, 100.0f, CObject::TYPE_BG, 4);
		}

		m_pQuitUI->Expand(0.05f, 1.0f);
		m_pStartUI->ResetLength();
		m_pStartUI->ResetExpand();
	}

	if (pInputkeyboard->GetTrigger(DIK_A) == true || pJoyPad->GetTrigger(CJoyPad::JOYKEY_LEFT) == true)
	{
		pSound->PlaySound(CSound::SOUND_LABEL_SELECT);	// 効果音の再生

		m_nSelect--;

		if (m_nSelect == NUM_PLAY - 1)
		{
			m_nSelect = NUM_QUIT;
		}
	}
	if (pInputkeyboard->GetTrigger(DIK_D) == true || pJoyPad->GetTrigger(CJoyPad::JOYKEY_RIGHT) == true)
	{
		pSound->PlaySound(CSound::SOUND_LABEL_SELECT);	// 効果音の再生

		m_nSelect++;
		
		if (m_nSelect == NUM_QUIT + 1)
		{
			m_nSelect = NUM_PLAY;
		}
	}
}
