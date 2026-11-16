// =================================================
// 
// チュートリアル処理[tutorial.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "tutorial.h"
#include "player.h"
#include "meshfield.h"
#include "macro.h"
#include "input.h"
#include "debugproc.h"
#include "object2D.h"
#include "texture.h"
#include "manager.h"
#include "object3D.h"
#include "item.h"
#include "timer.h"
#include "score.h"
#include "camera.h"
#include "light.h"
#include "fade.h"
#include "field.h"
#include "sound.h"
#include "stagemodel.h"
#include "particle.h"

#include<stdio.h>

// =================================================
// マクロ定義
#define	MODEL_FILE	"data\\Scripts\\player.txt"			// モデルを読み込むファイル名
#define	MOTION_FILE	"data\\Scripts\\motion.txt"			// モーションを読み込むファイル名
#define NUM_START_TUTORIAL	(0)	// チュートリアル開始の番号
#define NUM_SKIP_TUTORIAL	(1)	// チュートリアルスキップの番号

// =================================================
// 静的メンバ変数宣言
CPlayer* CTutorial::m_pPlayer = NULL;	// プレイヤーのポインタ
CMeshField* CTutorial::m_pMesh = NULL;	// メッシュのポインタ
CScore* CTutorial::m_pScore = NULL;			// スコアのポインタ
CItem* CTutorial::m_pItem[MAX_ITEM] = {};	// アイテムのポインタ

CCamera* CTutorial::m_pCamera = NULL;		// カメラのポインタ
CLight* CTutorial::m_pLight = NULL;			// ライトのポインタ
CTutorial* CTutorial::m_pTutorial = NULL;	// チュートリアルシーンのポインタ
CObject2D* CTutorial::m_pUI[32] = {};		// 表示UIポインタ
CObject2D* CTutorial::m_pSkipUI = NULL;		// チュートリアルスキップUIポインタ
CObject2D* CTutorial::m_pStartUI = NULL;		// チュートリアル開始UIポインタ
CObject2D* CTutorial::m_pBG = NULL;		// 選択肢背景ポインタ
CField* CTutorial::m_pField[MAX_FIELD] = {};	// フィールドのポインタ
CStageModel* CTutorial::m_pStageModel = NULL;	// ステージモデルマネージャーのポインタ

// =================================================
// コンストラクタ
// =================================================
CTutorial::CTutorial(CScene::MODE mode) :CScene(mode)
{
	memset(m_bDisp, 0, sizeof(m_bDisp));	// 各ポリゴンの表示フラグ
	memset(m_aAlpha, 0, sizeof(m_aAlpha));	// 各ポリゴンのα値
	memset(m_pField, 0, sizeof(m_pField));	// フィールドのポインタ
	m_nCntMove = 0;		// 移動入力カウント
	m_nCntCamera = 0;	// カメラ移動カウント
	m_nIdxTex = 0;		// テクスチャのインデックス
}

// =================================================
// デストラクタ
// =================================================
CTutorial::~CTutorial()
{

}

// =================================================
// 初期化処理
// =================================================
HRESULT CTutorial::Init(void)
{
	CSound* pSound = CManager::GetSound();	// サウンドポインタ
	CTexture* pTexture = CManager::GetTexture();	// テクスチャの取得

	pSound->PlaySound(CSound::SOUND_LABEL_TUTORIALBGM);	// BGM再生

	// --- 自身のアドレス --- //
	m_pTutorial = this;

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

	//// --- ライトの初期化 --- //
	m_pLight->Init();

	if (m_pStageModel == NULL)
	{// ステージモデルマネージャー
		m_pStageModel = new CStageModel;
	}

	// --- ステージモデルマネージャーの初期化 --- //
	m_pStageModel->Init();

	//// --- プレイヤーの設置 --- //
	CObjectX::Create({ 0.0f, 40.0f, 0.0f }, VEC_NULL, CObject::TYPE_PLAYER, "data\\MODEL\\player.x");
	//m_pPlayer = CPlayer::Create(D3DXVECTOR3(400.0f, 10.0f, -300.0f), VEC_NULL, VEC_NULL);

	// --- メッシュフィールドの設置 --- //
		// --- 下のメッシュフィールド --- //
	m_pField[CField::TYPE_BOTTOM] = CField::Create({ 0.0f, 0.0f, 0.0f }, VEC_NULL, FIELD_MESHSIZE, FIELD_MESHSIZE, FIELD_SIZE, FIELD_SIZE, FIELD_SIZE / FIELD_MESHSIZE, FIELD_SIZE / FIELD_MESHSIZE, CMesh::TYPE_FIELD, true, "data\\TEXTURE\\road.jpg");

	// --- 雪のメッシュフィールド --- //
	m_pField[CField::TYPE_SNOW] = CField::Create({ 0.0f, 25.0f, 0.0f }, VEC_NULL, FIELD_MESHSIZE, FIELD_MESHSIZE, FIELD_SIZE, FIELD_SIZE, FIELD_SIZE / FIELD_MESHSIZE, FIELD_SIZE / FIELD_MESHSIZE, CMesh::TYPE_FIELD, true, "data\\TEXTURE\\snow.jpg");

	// --- 右のロードヒーティングゾーン --- //
	m_pField[CField::TYPE_RIGHTHEAT] = CField::Create({ (FIELD_SIZE / 2) - (ROADHEAT_SIZEX / 2), 2.0f, 0.0f }, VEC_NULL,
		ROADHEAT_SIZE_MESHSIZE, ROADHEAT_SIZE_MESHSIZE, ROADHEAT_SIZEX, ROADHEAT_SIZEZ,
		ROADHEAT_SIZEX / ROADHEAT_SIZE_MESHSIZE, ROADHEAT_SIZEZ / ROADHEAT_SIZE_MESHSIZE,
		CMesh::TYPE_FIELD, true, "data\\TEXTURE\\roadheat.jpg");

	// --- 左のロードヒーティングゾーン --- //
	m_pField[CField::TYPE_LEFTHEAT] = CField::Create({ -((FIELD_SIZE / 2) - (ROADHEAT_SIZEX / 2)), 2.0f, 0.0f }, VEC_NULL,
		ROADHEAT_SIZE_MESHSIZE, ROADHEAT_SIZE_MESHSIZE, ROADHEAT_SIZEX, ROADHEAT_SIZEZ,
		ROADHEAT_SIZEX / ROADHEAT_SIZE_MESHSIZE, ROADHEAT_SIZEZ / ROADHEAT_SIZE_MESHSIZE,
		CMesh::TYPE_FIELD, true, "data\\TEXTURE\\roadheat.jpg");

	m_pField[CField::TYPE_SNOW]->Load();	// ロードヒーティングのゾーンはへこます

	// --- チュートリアルロゴUI --- //
	m_pUI[POL_NONE] = CObject2D::Create({ 1100.0f, 50.0f, 0.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 250, 150, CObject::TYPE_BG);
	m_nIdxTex = pTexture->Register("data\\TEXTURE\\tutorialUI.png");
	m_pUI[POL_NONE]->SetTexture(m_nIdxTex);

	// --- チュートリアル開始選択ポリゴン --- //
	m_pUI[POL_CHOICE] = CObject2D::Create({ SCREEN_WIDTH / 2, 350.0f, 0.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 800, 250, CObject::TYPE_BG, 7);
	m_nIdxTex = pTexture->Register("data\\TEXTURE\\choice.png");
	m_pUI[POL_CHOICE]->SetTexture(m_nIdxTex);

	// --- ゲームスタートUIの生成 --- //
	m_pStartUI = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2 - 200.0f, SCREEN_HEIGHT - 200.0f, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		200.0f,
		100.0f,
		CObject::TYPE_BG);

	int nIdxTex = pTexture->Register("data\\TEXTURE\\yes.png");
	m_pStartUI->SetTexture(nIdxTex);

	// --- ゲーム終了UIの生成 --- //
	m_pSkipUI = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2 + 200.0f, SCREEN_HEIGHT - 200.0f, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		200.0f,
		100.0f,
		CObject::TYPE_BG);

	nIdxTex = pTexture->Register("data\\TEXTURE\\skip.png");
	m_pSkipUI->SetTexture(nIdxTex);

	for (int nCnt = 0; nCnt < 32; nCnt++)
	{
		m_aAlpha[nCnt] = 1.0f;
	}

	m_bSkip = false;	// スキップフラグを消す
	m_bStart = false;	// チュートリアルの開始フラグ
	m_nSelect = 0;		// 選択番号
	m_bSelect = false;

	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CTutorial::Uninit(void)
{
	// --- 各種終了処理 --- //
	m_pCamera->Uninit();
	m_pLight->Uninit();
	m_pStageModel->Uninit();

	if (m_pCamera != NULL)
	{// メモリの破棄
		delete m_pCamera;
		m_pCamera = NULL;
	}

	if (m_pLight != NULL)
	{// メモリの破棄
		delete m_pLight;
		m_pLight = NULL;
	}

	if (m_pTutorial != NULL)
	{// メモリの破棄
		delete m_pTutorial;
		m_pTutorial = NULL;
	}

	if (m_pStageModel != NULL)
	{// メモリの破棄
		delete m_pStageModel;
		m_pStageModel = NULL;
	}

	m_pPlayer = NULL;	// プレイヤーのポインタ
	m_pMesh = NULL;		// メッシュのポインタ
	m_pScore = NULL;	// スコアのポインタ
	m_nCntMove = 0;		// 移動入力カウント
	m_nCntCamera = 0;	// カメラ移動カウント
	memset(m_pItem, 0, sizeof(m_pItem));	// アイテムのポインタ
	memset(m_pUI, 0, sizeof(m_pUI));		// 表示UIのポインタ
	memset(m_aAlpha, 0, sizeof(m_aAlpha));	// 各ポリゴンのα値
}

// =================================================
// 更新処理
// =================================================
void CTutorial::Update(void)
{
	CFade* pFade = CManager::GetFade();			// フェードの取得

	if (pFade->GetFade() == CFade::FADE_NONE)
	{
		// --- 変数宣言 --- //
		D3DXVECTOR2 JoyStick = {};	// スティックの傾き度

		// --- カメラの更新処理 --- //
		m_pCamera->Update();

		// --- ライトの更新処理 --- //
		m_pLight->Update();

		if (m_bSelect == false)
		{
			// --- チュートリアル開始の可否を選択 --- //
			ChoiceStart();
		}
		else if (m_bSelect == true)
		{// 最初の選択肢を決定したか
			// --- チュートリアルの開始 --- //
			PlayTutorial();

			if (m_pUI[POL_NONE] != NULL)
			{
				m_pUI[POL_NONE]->FlashPol(90);
			}
		}
	}

	CDebugProc* pDebugProc = CManager::GetDrbugProc();				// デバッグ表示のポインタ

	pDebugProc->PrintDebug("tutorial\n");
}

// =================================================
// 描画処理
// =================================================
void CTutorial::Draw(void)
{

}

// =================================================
// チュートリアル再生を選択
// =================================================
void CTutorial::ChoiceStart(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard(); // キーボードの取得
	CJoyPad* pJoyPad = CManager::GetJoyPad();	// ジョイパッドの取得
	CSound* pSound = CManager::GetSound();		// サウンドの取得
	CFade* pFade = CManager::GetFade();			// フェードの取得

	if (m_nSelect == NUM_START_TUTORIAL)
	{// スタートが選択
		m_pStartUI->Expand(0.05f, 1.0f);
		m_pSkipUI->ResetLength();	// 対角線の長さをリセット
		m_pSkipUI->ResetExpand();	// 拡縮率をリセット

		if (m_pBG != NULL) {// 背景ポリゴンの破棄
			m_pBG->Uninit();
			m_pBG = NULL;
		}

		if (m_pBG == NULL) {// 背景ポリゴンの生成
			m_pBG = CObject2D::Create(m_pStartUI->GetPosition(), VEC_NULL, VEC_NULL, COLOR_HALFWHITE, 200.0f, 100.0f, CObject::TYPE_BG, 4);
		}
	}
	if (m_nSelect == NUM_SKIP_TUTORIAL)
	{// スキップが選択
		m_pSkipUI->Expand(0.05f, 1.0f);
		m_pStartUI->ResetLength();// 対角線の長さをリセット
		m_pStartUI->ResetExpand();// 拡縮率をリセット

		if (m_pBG != NULL) {// 背景ポリゴンの破棄
			m_pBG->Uninit();
			m_pBG = NULL;
		}

		if (m_pBG == NULL) {// 背景ポリゴンの生成
			m_pBG = CObject2D::Create(m_pSkipUI->GetPosition(), VEC_NULL, VEC_NULL, COLOR_HALFWHITE, 200.0f, 100.0f, CObject::TYPE_BG, 4);
		}
	}

	// --- 選択カーソル移動処理 --- //
	if (pInputKeyboard->GetTrigger(DIK_A) == true || pJoyPad->GetTrigger(CJoyPad::JOYKEY_LEFT) == true)
	{
		pSound->PlaySound(CSound::SOUND_LABEL_SELECT);	// 効果音の再生

		m_nSelect--;

		if (m_nSelect == NUM_START_TUTORIAL - 1)
		{
			m_nSelect = NUM_SKIP_TUTORIAL;
		}
	}
	if (pInputKeyboard->GetTrigger(DIK_D) == true || pJoyPad->GetTrigger(CJoyPad::JOYKEY_RIGHT) == true)
	{
		pSound->PlaySound(CSound::SOUND_LABEL_SELECT);	// 効果音の再生

		m_nSelect++;

		if (m_nSelect == NUM_SKIP_TUTORIAL + 1)
		{
			m_nSelect = NUM_START_TUTORIAL;
		}
	}

	// --- 決定処理 --- //
	if (pInputKeyboard->GetTrigger(DIK_RETURN) == true || pJoyPad->GetTrigger(CJoyPad::JOYKEY_A) == true)
	{
		pSound->PlaySound(CSound::SOUND_LABEL_ENTER);	// 効果音の再生

		if (m_nSelect == NUM_START_TUTORIAL)
		{// スタートを選択
			m_bSelect = true;
		}
		if (m_nSelect == NUM_SKIP_TUTORIAL)
		{// スキップを選択
			m_bSelect = true;

			if (m_pBG != NULL) {// 背景ポリゴンの破棄
				m_pBG->Uninit();
				m_pBG = NULL;
			}

			if (pFade->GetFade() == CFade::FADE_NONE)
			{
				pSound->PlaySound(CSound::SOUND_LABEL_ENTER);	// 効果音の再生
			}

			CManager::SetStopSound(true);	// BGM終了フラグを立てる
			CFade::SetFade(CScene::MODE_GAME);
		}
	}
}

// =================================================
// チュートリアル再生を選択
// =================================================
void CTutorial::PlayTutorial(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard(); // キーボードの取得
	CJoyPad* pJoyPad = CManager::GetJoyPad();	// ジョイパッドの取得
	CSound* pSound = CManager::GetSound();		// サウンドの取得
	CFade* pFade = CManager::GetFade();			// フェードの取得
	CTexture* pTexture = CManager::GetTexture();	// テクスチャの取得
	int nIdxTex[POL_MAX];	// テクスチャインデックス

	if (CManager::GetDrawParticle() == true)
	{
		// --- ロードヒーティングのパーティクル --- //
		D3DXVECTOR3 Rightpos = m_pField[CField::TYPE_RIGHTHEAT]->GetPosition();
		CParticle::Create(Rightpos, VEC_NULL, { 10.0f, 1.0f, 10.0f }, { 0.0f, 10.0f, 0.0f }, (COLOR_RED + COLOR_ORANGE) * 0.5f, 15.0f, 15.0f, 2, 10, 6, CParticle::TYPE_ROADHEAT);

		D3DXVECTOR3 Leftpos = m_pField[CField::TYPE_LEFTHEAT]->GetPosition();
		CParticle::Create(Leftpos, VEC_NULL, { 10.0f, 1.0f, 10.0f }, { 0.0f, 10.0f, 0.0f }, (COLOR_RED + COLOR_ORANGE) * 0.5f, 15.0f, 15.0f, 2, 10, 6, CParticle::TYPE_ROADHEAT);
	}

	if (m_bSkip == false && m_nSelect == NUM_START_TUTORIAL)
	{// チュートリアル再生	
		if (m_pBG != NULL) {// 背景ポリゴンの破棄
			m_pBG->Uninit();
			m_pBG = NULL;
		}

		if (m_pUI[POL_INTRO] == NULL && m_bStart == false)
		{// ゲーム導入チュートリアルが表示中にEnter
			m_pUI[POL_CHOICE]->Uninit();
			m_pStartUI->Uninit();
			m_pSkipUI->Uninit();

			m_pUI[POL_CHOICE] = NULL;
			m_pStartUI = NULL;
			m_pSkipUI = NULL;

			// --- チュートリアル導入ポリゴン --- //
			m_pUI[POL_INTRO] = CObject2D::Create({ SCREEN_WIDTH / 2, 550.0f, 0.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 800, 250, CObject::TYPE_BG, 7);
			m_nIdxTex = pTexture->Register("data\\TEXTURE\\intro.png");
			m_pUI[POL_INTRO]->SetTexture(m_nIdxTex);

			m_pUI[POL_NONE] = CObject2D::Create({ 1160.0f, 680.0f, 0.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 200, 100, CObject::TYPE_BG);
			m_nIdxTex = pTexture->Register("data\\TEXTURE\\next.png");
			m_pUI[POL_NONE]->SetTexture(m_nIdxTex);
		}

		m_bStart = true;	// チュートリアルの開始フラグ

		if (pInputKeyboard->GetTrigger(DIK_RETURN) == true || pJoyPad->GetTrigger(CJoyPad::JOYKEY_A) == true)
		{
			pSound->PlaySound(CSound::SOUND_LABEL_ENTER);	// 効果音の再生

			if (m_pUI[POL_INTRO] != NULL)
			{// ゲーム導入チュートリアルが表示中にEnter
				m_pUI[POL_INTRO]->Uninit();
				m_pUI[POL_INTRO] = NULL;;

				m_pUI[POL_PLAYER] = CObject2D::Create({ SCREEN_WIDTH / 2, 550.0f, 0.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 800.0f, 350.0f, CObject::TYPE_BG, 7);
				nIdxTex[POL_PLAYER] = pTexture->Register("data\\TEXTURE\\player.png");
				m_pUI[POL_PLAYER]->SetTexture(nIdxTex[POL_PLAYER]);
			}
			else if (m_pUI[POL_PLAYER] != NULL)
			{
				m_pUI[POL_PLAYER]->Uninit();
				m_pUI[POL_PLAYER] = NULL;;

				m_pUI[POL_RISK] = CObject2D::Create({ SCREEN_WIDTH / 2, 550.0f, 0.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 800, 350.0f, CObject::TYPE_BG, 7);
				nIdxTex[POL_RISK] = pTexture->Register("data\\TEXTURE\\risk.png");
				m_pUI[POL_RISK]->SetTexture(nIdxTex[POL_RISK]);
			}
			else if (m_pUI[POL_RISK] != NULL)
			{
				m_pUI[POL_RISK]->Uninit();
				m_pUI[POL_RISK] = NULL;;

				m_pUI[POL_ROADHEAT] = CObject2D::Create({ SCREEN_WIDTH / 2, 550.0f, 0.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 800, 350.0f, CObject::TYPE_BG, 7);
				nIdxTex[POL_ROADHEAT] = pTexture->Register("data\\TEXTURE\\roadheat.png");
				m_pUI[POL_ROADHEAT]->SetTexture(nIdxTex[POL_ROADHEAT]);
			}
			else if (m_pUI[POL_ROADHEAT] != NULL)
			{
				m_pUI[POL_ROADHEAT]->Uninit();
				m_pUI[POL_ROADHEAT] = NULL;;

				m_pUI[POL_HEATTIME] = CObject2D::Create({ SCREEN_WIDTH / 2, 550.0f, 0.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 800, 350.0f, CObject::TYPE_BG, 7);
				nIdxTex[POL_HEATTIME] = pTexture->Register("data\\TEXTURE\\heattime.png");
				m_pUI[POL_HEATTIME]->SetTexture(nIdxTex[POL_HEATTIME]);
			}
			else if (m_pUI[POL_HEATTIME] != NULL)
			{
				m_pUI[POL_HEATTIME]->Uninit();
				m_pUI[POL_HEATTIME] = NULL;;

				m_pUI[POL_PURPOSE] = CObject2D::Create({ SCREEN_WIDTH / 2, 550.0f, 0.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 800, 350.0f, CObject::TYPE_BG, 7);
				nIdxTex[POL_PURPOSE] = pTexture->Register("data\\TEXTURE\\purpose.png");
				m_pUI[POL_PURPOSE]->SetTexture(nIdxTex[POL_PURPOSE]);
			}
			else if (m_pUI[POL_PURPOSE] != NULL)
			{
				m_pUI[POL_PURPOSE]->Uninit();
				m_pUI[POL_PURPOSE] = NULL;;

				m_pUI[POL_CONTROLL] = CObject2D::Create({ SCREEN_WIDTH / 2, 550.0f, 0.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 800, 350.0f, CObject::TYPE_BG, 7);
				nIdxTex[POL_CONTROLL] = pTexture->Register("data\\TEXTURE\\controll.png");
				m_pUI[POL_CONTROLL]->SetTexture(nIdxTex[POL_CONTROLL]);
			}
			else if (m_pUI[POL_CONTROLL] != NULL)
			{
				m_pUI[POL_CONTROLL]->Uninit();
				m_pUI[POL_CONTROLL] = NULL;;

				m_pUI[POL_HOWMOVE] = CObject2D::Create({ SCREEN_WIDTH / 2, 550.0f, 0.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 800, 350.0f, CObject::TYPE_BG, 7);
				nIdxTex[POL_HOWMOVE] = pTexture->Register("data\\TEXTURE\\howmove.png");
				m_pUI[POL_HOWMOVE]->SetTexture(nIdxTex[POL_HOWMOVE]);
			}
			else if (m_pUI[POL_HOWMOVE] != NULL)
			{
				m_pUI[POL_HOWMOVE]->Uninit();
				m_pUI[POL_HOWMOVE] = NULL;;

				m_pUI[POL_BREAK] = CObject2D::Create({ SCREEN_WIDTH / 2, 550.0f, 0.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 800, 350.0f, CObject::TYPE_BG, 7);
				nIdxTex[POL_BREAK] = pTexture->Register("data\\TEXTURE\\break.png");
				m_pUI[POL_BREAK]->SetTexture(nIdxTex[POL_BREAK]);
			}
			else if (m_pUI[POL_BREAK] != NULL)
			{
				m_pUI[POL_BREAK]->Uninit();
				m_pUI[POL_BREAK] = NULL;;

				m_pUI[POL_FINISH] = CObject2D::Create({ SCREEN_WIDTH / 2, 550.0f, 0.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 800, 350.0f, CObject::TYPE_BG, 7);
				nIdxTex[POL_FINISH] = pTexture->Register("data\\TEXTURE\\finishtutorial.png");
				m_pUI[POL_FINISH]->SetTexture(nIdxTex[POL_FINISH]);
			}
			else if (m_pUI[POL_FINISH] != NULL)
			{
				m_pUI[POL_FINISH]->Uninit();
				m_pUI[POL_FINISH] = NULL;;

				if (pFade->GetFade() == CFade::FADE_NONE)
				{
					pSound->PlaySound(CSound::SOUND_LABEL_ENTER);	// 効果音の再生
				}

				CManager::SetStopSound(true);	// BGM終了フラグを立てる
				CFade::SetFade(CScene::MODE_GAME);
			}
		}
	}
}