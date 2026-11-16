// =================================================
// 
// ゲーム処理[game.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "game.h"
#include "player.h"
#include "meshfield.h"
#include "meshring.h"
#include "meshcylinder.h"
#include "macro.h"
#include "input.h"
#include "debugproc.h"
#include "object2D.h"
#include "texture.h"
#include "manager.h"
#include "object3D.h"
#include "item.h"
#include "timer.h"
#include "camera.h"
#include "light.h"
#include "fade.h"
#include "pause.h"
#include "score.h"
#include "sound.h"
#include "field.h"
#include "wall.h"
#include "stagemodel.h"
#include "particle.h"
#include "3DMultiTex.h"
#include<stdio.h>

// =================================================
// マクロ定義
#define AUDIENCE_FILE	"data\\SCRIPTS\\Setaudience.txt"

// =================================================
// 静的メンバ変数宣言
//CPlayer* CGame::m_pPlayer = NULL;	// プレイヤーのポインタ
//CTimer* CGame::m_pTimer = NULL;		// タイマーのポインタ
//CScore* CGame::m_pScore[2] = {};		// スコアのポインタ
//CItem* CGame::m_pItem = NULL;		// アイテムのポインタ
//CCamera* CGame::m_pCamera = NULL;	// カメラのポインタ
//CLight* CGame::m_pLight = NULL;		// ライトのポインタ
//CPause* CGame::m_pPause = NULL;		// ポーズのポインタ

CObject2D* CGame::m_pUI = NULL;		// UI各種のポインタ
CGame* CGame::m_pGame = NULL;		// 自身のポインタ
CPause* CGame::m_pPause = NULL;		// ポーズのポインタ
CTimer* CGame::m_pTimer = NULL;		// タイマーのポインタ
CPlayer* CGame::m_pPlayer = NULL;	// プレイヤーのポインタ
CScore* CGame::m_pScore = {};		// スコアのポインタ
CCamera* CGame::m_pCamera = NULL;	// カメラのポインタ
CLight* CGame::m_pLight = NULL;		// ライトのポインタ
CMesh* CGame::m_pMesh = NULL;		// メッシュのポインタ
CWall* CGame::m_pWall[4] = {};		// 壁のポインタ
CField* CGame::m_pField[MAX_FIELD] = {};	// 床のポインタ
CMeshRing* CGame::m_pRing[MAX_RING] = {};	// メッシュリングのポインタ
CItem* CGame::m_pItem = NULL;		// アイテムのポインタ
CStageModel* CGame::m_pStageModel = NULL;	// ステージモデルマネージャーのポインタ

int CGame::m_nIdxTex = -1;		// テクスチャのインデックス

// =================================================
// コンストラクタ
// =================================================
CGame::CGame(CScene::MODE mode) :CScene(mode)
{
	m_nIdxTex = -1;		// テクスチャのインデックス
}

// =================================================
// デストラクタ
// =================================================
CGame::~CGame()
{

}

// =================================================
// 初期化処理
// =================================================
HRESULT CGame::Init(void)
{
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ
	CSound* pSound = CManager::GetSound();	// サウンドポインタ

	pSound->PlaySound(CSound::SOUND_LABEL_GAMEBGM);	// BGM再生

	// --- 自身のアドレス --- //
	m_pGame = this;

	if (m_pCamera == NULL)
	{// カメラ
		m_pCamera = new CCamera;
	}

	m_pCamera->Init();

	if (m_pLight == NULL)
	{// ライト
		m_pLight = new CLight;
	}

	m_pLight->Init();

	if (m_pPause == NULL)
	{// ポーズ
		m_pPause = new CPause;
	}

	// --- ポーズの初期化 --- //
	m_pPause->Init();

	if (m_pStageModel == NULL)
	{// ステージモデルマネージャー
		m_pStageModel = new CStageModel;
	}

	// --- ステージモデルマネージャーの初期化 --- //
	m_pStageModel->Init();

	// --- タイマーの設定 --- //
	m_pTimer = CTimer::Create(D3DXVECTOR3(SCREEN_WIDTH / 2 - 50.0f, 50.0f, 0.0f), VEC_NULL, VEC_NULL, COLOR_BLACK, 80, 50.0f, 80.0f, "data\\TEXTURE\\number.png");

	// --- タイマーの背景 --- //
	CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH/ 2, 50.0f, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_HALFWHITE,
		200.0f,
		100.0f,
		CObject::TYPE_BG);

	// --- スコアの設定 --- //
	m_pScore = CScore::Create(D3DXVECTOR3(SCREEN_WIDTH - 280.0f, 50.0f, 0.0f), VEC_NULL, VEC_NULL, COLOR_GREEN, 40.0f, 100.0f, 4, "data\\TEXTURE\\number.png");

	// --- スコアの背景 --- //
	CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH - 200.0f, 50.0f, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_HALFWHITE,
		250.0f,
		100.0f,
		CObject::TYPE_BG);

	// --- %のUI --- //
	m_pUI = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH - 120.0f, 70.0f, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		450.0f,
		150.0f,
		CObject::TYPE_BG);

	m_nIdxTex = pTexture->Register("data\\TEXTURE\\persent.png");
	m_pUI->SetTexture(m_nIdxTex);

	CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH - 180.0f, 80.0f, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		8.0f,
		8.0f,
		CObject::TYPE_BG);

	m_pPlayer = CPlayer::Create(VEC_NULL, VEC_NULL, VEC_NULL);

	// --- 下のメッシュフィールド --- //
	m_pField[CField::TYPE_BOTTOM] = CField::Create({ 0.0f, 0.0f, 0.0f }, VEC_NULL, FIELD_MESHSIZE, FIELD_MESHSIZE, FIELD_SIZE, FIELD_SIZE, FIELD_SIZE / FIELD_MESHSIZE, FIELD_SIZE / FIELD_MESHSIZE, CMesh::TYPE_FIELD, true, "data\\TEXTURE\\road.jpg");

	// --- 雪のメッシュフィールド --- //
	m_pField[CField::TYPE_SNOW] = CField::Create({ 0.0f, 28.0f, 0.0f }, VEC_NULL, FIELD_MESHSIZE, FIELD_MESHSIZE, FIELD_SIZE, FIELD_SIZE, FIELD_SIZE / FIELD_MESHSIZE, FIELD_SIZE / FIELD_MESHSIZE, CMesh::TYPE_FIELD, true, "data\\TEXTURE\\snow.jpg");

	// --- 右のロードヒーティングゾーン --- //
	m_pField[CField::TYPE_RIGHTHEAT] = CField::Create({ (FIELD_SIZE / 2) - (ROADHEAT_SIZEX / 2), 2.0f, 0.0f }, VEC_NULL,
		ROADHEAT_SIZE_MESHSIZE, ROADHEAT_SIZE_MESHSIZE, ROADHEAT_SIZEX, ROADHEAT_SIZEZ,
		ROADHEAT_SIZEX / ROADHEAT_SIZE_MESHSIZE, ROADHEAT_SIZEZ / ROADHEAT_SIZE_MESHSIZE,
		CMesh::TYPE_FIELD, true, "data\\TEXTURE\\roadheat.jpg");

	// --- 左のロードヒーティングゾーン --- //
	m_pField[CField::TYPE_LEFTHEAT] = CField::Create({ -((FIELD_SIZE / 2) - (ROADHEAT_SIZEX / 2)), 2.0f, 0.0f }, VEC_NULL,
		SECRETROADHEAT_SIZE_MESHSIZE, SECRETROADHEAT_SIZE_MESHSIZE, ROADHEAT_SIZEX, ROADHEAT_SIZEZ,
		ROADHEAT_SIZEX / SECRETROADHEAT_SIZE_MESHSIZE, ROADHEAT_SIZEZ / SECRETROADHEAT_SIZE_MESHSIZE,
		CMesh::TYPE_FIELD, true, "data\\TEXTURE\\roadheat.jpg");

	// --- 隠しロードヒーティングゾーン --- //
	m_pField[CField::TYPE_CENTERHEAT] = CField::Create({ 0.0f, 2.0f, -800.0f }, VEC_NULL,
		ROADHEAT_SIZE_MESHSIZE, ROADHEAT_SIZE_MESHSIZE, SECRETROADHEAT_SIZEX, SECRETROADHEAT_SIZEZ,
		SECRETROADHEAT_SIZEX / ROADHEAT_SIZE_MESHSIZE, SECRETROADHEAT_SIZEZ / ROADHEAT_SIZE_MESHSIZE,
		CMesh::TYPE_FIELD, true, "data\\TEXTURE\\roadheat.jpg");

	m_pField[CField::TYPE_SNOW]->Load();	// ロードヒーティングのゾーンはへこます

	//m_pRing[0] = CMeshRing::Create({ 0.0f, 5.0f, -400.0f }, VEC_NULL, 50.0f, 100.0f, 16);
//	m_pRing[0]->SetColor(COLOR_RED);
	//m_pRing = CMeshRing::Create({100.0f, 1.0f, 30.0f}, VEC_NULL, 30.0f, 50.0f, 32);
	//m_pRing = CMeshRing::Create({-30.0f, 1.0f, 0.0f}, VEC_NULL, 50.0f, 100.0f, 32);
	//m_pRing = CMeshRing::Create({-30.0f, 1.0f, -70.0f}, VEC_NULL, 5.0f, 10.0f, 32);
	//m_pRing = CMeshRing::Create({70.0f, 1.0f, 40.0f}, VEC_NULL, 10.0f, 50.0f, 32);

	//CMeshCylinder::Create({0.0f, 0.0f, 0.0f}, VEC_NULL, 2000.0f, 300.0f, 8, 1, CMesh::TYPE_CYLINDER, true, false, "data\\TEXTURE\\sky.png");

	//CItem::ResetItem();	// 静的メンバ変数の初期化

	m_pWall[0] = CWall::Create({ FIELD_SIZE * 0.5f, -100.0f, 0.0f },{ D3DX_PI * 0.5f, 0.0f, -D3DX_PI * 0.5f }, VEC_NULL, COLOR_WHITE, FIELD_SIZE, 200, CObject::TYPE_BG, NULL);
	m_pWall[1] = CWall::Create({ -FIELD_SIZE * 0.5f, -100.0f, 0.0f },  { D3DX_PI * 0.5f , 0.0f , D3DX_PI * 0.5f }, VEC_NULL, COLOR_WHITE, FIELD_SIZE, 200, CObject::TYPE_BG, NULL);
	m_pWall[2] = CWall::Create({ 0.0f, -100.0f, FIELD_SIZE * 0.5f },{ -D3DX_PI * 0.5f, D3DX_PI, 0.0f }, VEC_NULL, COLOR_WHITE, FIELD_SIZE, 200, CObject::TYPE_BG, NULL);
	m_pWall[3] = CWall::Create({ 0.0f, -100.0f, -FIELD_SIZE * 0.5f },  { -D3DX_PI * 0.5f, 0.0f, 0.0f }, VEC_NULL, COLOR_WHITE, FIELD_SIZE, 200, CObject::TYPE_BG, NULL);


	// --- 駐車場の白線 --- //
	CObject3D::Create({ 300.0f, 1.0f, 200.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 400.0f, 30.0f, CObject::TYPE_LINE);
	CObject3D::Create({ 300.0f, 1.0f, 0.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 400.0f, 30.0f, CObject::TYPE_LINE);
	CObject3D::Create({ 300.0f, 1.0f, -200.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 400.0f, 30.0f, CObject::TYPE_LINE);
	CObject3D::Create({ 300.0f, 1.0f, -400.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 400.0f, 30.0f, CObject::TYPE_LINE);
	CObject3D::Create({ 300.0f, 1.0f, -600.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 400.0f, 30.0f, CObject::TYPE_LINE);

	CObject3D::Create({ -500.0f, 1.0f, 0.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 400.0f, 30.0f, CObject::TYPE_LINE);
	CObject3D::Create({ -500.0f, 1.0f, -200.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 400.0f, 30.0f, CObject::TYPE_LINE);
	CObject3D::Create({ -500.0f, 1.0f, -400.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 400.0f, 30.0f, CObject::TYPE_LINE);
	CObject3D::Create({ -500.0f, 1.0f, -600.0f }, VEC_NULL, VEC_NULL, COLOR_WHITE, 400.0f, 30.0f, CObject::TYPE_LINE);

	//const char* pFileName[10];

	//for (int nCnt = 0; nCnt < 10; nCnt++)
	//{
	//	pFileName[nCnt] = NULL;
	//}	

	//pFileName[0] = "data\\TEXTURE\\bossenemy.png";
	//pFileName[1] = "data\\TEXTURE\\block.jpg";

	//C3DMultiTex::Create({ 100.0, 23.0f, 0.0f }, VEC_NULL, VEC_NULL, 100.0f, 100.0f, &pFileName[0]);

	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CGame::Uninit(void)
{
	// --- 各種終了処理 --- //
	m_pCamera->Uninit();
	m_pLight->Uninit();
	m_pPause->Uninit();
	m_pStageModel->Uninit();
	m_pItem->Uninit();

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

	if (m_pPause != NULL)
	{// メモリの破棄
		delete m_pPause;
		m_pPause = NULL;
	}

	if (m_pStageModel != NULL)
	{// メモリの破棄
		delete m_pStageModel;
		m_pStageModel = NULL;
	}

	if (m_pGame != NULL)
	{// メモリの破棄
		delete m_pGame;
		m_pGame = NULL;
	}

	if (m_pItem != NULL)
	{// メモリの破棄
		delete m_pItem;
		m_pItem = NULL;
	}

	m_pPlayer = NULL;	// プレイヤーのポインタ
	m_pMesh = NULL;		// メッシュのポインタ
	m_pTimer = NULL;	// タイマーのポインタ

	m_nIdxTex = -1;		// テクスチャのインデックス
}

// =================================================
// 更新処理
// =================================================
void CGame::Update(void)
{
	// --- キーボードの取得 --- //
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();
	CDebugProc* pDebugProc = CManager::GetDrbugProc();				// デバッグ表示のポインタ
	CJoyPad* pJoyPad = CManager::GetJoyPad();	// ジョイパッドの取得

	// --- カメラの更新処理 --- //
	if (GetMode() == CScene::MODE_GAME)
	{
		m_pCamera->Update();
	}

	// --- ライトの更新処理 --- //
	m_pLight->Update();

	// --- スコアの更新 --- //
	m_pScore->AddScore();

	// --- ポーズの更新処理 --- //
	m_pPause->Update();

	if (CManager::GetPause() == false && CManager::GetDrawParticle() == true)
	{
		// --- ロードヒーティングのパーティクル --- //
		D3DXVECTOR3 Rightpos = m_pField[CField::TYPE_RIGHTHEAT]->GetPosition();
		CParticle::Create(Rightpos, VEC_NULL, { 10.0f, 1.0f, 10.0f }, { 0.0f, 10.0f, 0.0f }, (COLOR_RED + COLOR_ORANGE) * 0.5f, 15.0f, 15.0f, 2, 10, 6, CParticle::TYPE_ROADHEAT);

		D3DXVECTOR3 Leftpos = m_pField[CField::TYPE_LEFTHEAT]->GetPosition();
		CParticle::Create(Leftpos, VEC_NULL, { 10.0f, 1.0f, 10.0f }, { 0.0f, 10.0f, 0.0f }, (COLOR_RED + COLOR_ORANGE) * 0.5f, 15.0f, 15.0f, 2, 10, 6, CParticle::TYPE_ROADHEAT);

		// --- 雪のパーティクル --- //
		CParticle::Create({1500.0f, 800.0f, 0.0f}, VEC_NULL, { 30.0f, 1.0f, 200.0f }, { -50.0f, -20.0f, -100.0f }, COLOR_WHITE, 10.0f, 10.0f, 5, 2, 6, CParticle::TYPE_SNOW);
	}

	// --- ポーズ処理 --- //
	if (CManager::GetMode() == MODE_GAME)
	{
		if (CManager::GetFade()->GetFade() == CFade::FADE_NONE)
		{
			if (pInputKeyboard->GetTrigger(DIK_P) == true || pJoyPad->GetTrigger(CJoyPad::JOYKEY_START))
			{
				// --- ポーズ状況の変更 --- //
				if (CManager::GetPause() == true)
				{
					CManager::SetPause(false);
				}
				else
				{
					CManager::SetPause(true);
				}
			}
		}
	}

	int nPersent = m_pField[CField::TYPE_SNOW]->GetPersentHeight();

	// --- 画面遷移条件 --- //
	if (m_pTimer->IsGameOver() == true || nPersent >= 1000)
	{
		CManager::SetStopSound(true);	// bgm終了フラグを立てる
		m_pField[CField::TYPE_SNOW]->EndSave();
		SaveScore();	// スコアの保存
		CFade::SetFade(CScene::MODE_RESULT);		
		
		if (nPersent >= 1000)
		{// 100%時は強制的に全頂点座標を下げる
			m_pField[CField::TYPE_SNOW]->DownHeight();
		}
	}

	pDebugProc->PrintDebug("game");
}

// =================================================
// 描画処理
// =================================================
void CGame::Draw(void)
{

}

// =================================================
// ステージの生成
// =================================================
void CGame::CreateStage(void)
{

}

// =================================================
// スコアの保存
// =================================================
void CGame::SaveScore(void)
{
	FILE* pFile;
	pFile = fopen("data\\scripts\\score.txt", "w"); // ファイルを開く

	if (pFile != NULL)
	{
		fprintf(pFile, "%d", m_pScore->GetScore());
	}
	else
	{

	}

	fclose(pFile);
}

