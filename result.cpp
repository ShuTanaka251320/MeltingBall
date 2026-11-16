// =================================================
// 
// タイトル処理[title.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include<winsock2.h>

#include "result.h"
#include "object.h"
#include "object2D.h"
#include "object3D.h"
#include "macro.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"
#include "manager.h"
#include "fade.h"
#include "camera.h"
#include "light.h"
#include "mesh.h"
#include "meshfield.h"
#include "score.h"
#include "ranking.h"
#include "fade.h"
#include "sound.h"
#include "tcp_client.h"

// =================================================
// マクロ定義
#define RANKING_FILE	"data\\scripts\\ranking.txt"	// ランキング保存ファイル
#define PORT_NUMBER	(22333)		// ポート番号

// =================================================
// 静的メンバ変数宣言
CObject2D* CResult::m_pBg = NULL;	// 背景のポリゴン用
CObject2D* CResult::m_pUI = NULL;	// UIのポリゴン用
int CResult::m_nIdxTex = -1;		// テクスチャのインデックス
int CResult::m_nCntFade = 0;		// 自動で画面遷移するまでのカウンター
int CResult::m_nScore = 0;			// 獲得スコア
int CResult::m_nRank = 0;			// 更新順位
CCamera* CResult::m_pCamera = NULL;	// カメラのポインタ
CLight* CResult::m_pLight = NULL;	// ライトのポインタ
CMesh* CResult::m_pMesh = NULL;		// メッシュのポインタ
CField* CResult::m_pField[MAX_FIELD] = {};	// 床のポインタ
CObject3D* CResult::m_pSide = NULL;		// 限界値のビルポリゴン
CScore* CResult::m_pScore = NULL;		// スコアのポインタ

// =================================================
// コンストラクタ
// =================================================
CResult::CResult(CScene::MODE mode) :CScene(mode)
{

}

// =================================================
// デストラクタ
// =================================================
CResult::~CResult()
{

}

// =================================================
// 初期化処理
// =================================================
HRESULT CResult::Init(void)
{
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	CSound* pSound = CManager::GetSound();	// サウンドポインタ

	pSound->PlaySound(CSound::SOUND_LABEL_RESULTBGM);	// BGM再生

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

	CObjectX::Create(VEC_NULL, VEC_NULL, CObject::TYPE_STAGEMODEL, "data\\model\\stage\\stage.x");

	// ===== メッシュフィールドの設置 ===== //
	// --- 下のメッシュフィールド --- //
	m_pField[CField::TYPE_BOTTOM] = CField::Create({ 0.0f, 15.0f, 0.0f }, VEC_NULL, FIELD_MESHSIZE, FIELD_MESHSIZE, FIELD_SIZE, FIELD_SIZE, FIELD_SIZE / FIELD_MESHSIZE, FIELD_SIZE / FIELD_MESHSIZE, CMesh::TYPE_FIELD, true, "data\\TEXTURE\\road.jpg");

	// --- 雪のメッシュフィールド --- //
	m_pField[CField::TYPE_SNOW] = CField::Create({ 0.0f, 28.0f, 0.0f }, VEC_NULL, FIELD_MESHSIZE, FIELD_MESHSIZE, FIELD_SIZE, FIELD_SIZE, FIELD_SIZE / FIELD_MESHSIZE, FIELD_SIZE / FIELD_MESHSIZE, CMesh::TYPE_FIELD, true, "data\\TEXTURE\\snow.jpg");
	m_pField[CField::TYPE_SNOW]->LoadEndSave();	// ゲーム終了時の頂点座標を読み込む

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

	// --- GAMECLEARポリゴン --- //
	m_pUI = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3 - 100, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		1200.0f,
		300.0f,
		CObject::TYPE_BG);

	m_nIdxTex = pTexture->Register("data\\TEXTURE\\finish.png");
	m_pUI->SetTexture(m_nIdxTex);

	// --- %のUI --- //
	m_pUI = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH - 450.0f, 410.0f, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		600.0f,
		200.0f,
		CObject::TYPE_BG);

	m_nIdxTex = pTexture->Register("data\\TEXTURE\\persent.png");
	m_pUI->SetTexture(m_nIdxTex);

	FILE* pFile;
	pFile = fopen("data\\scripts\\score.txt", "r"); // ファイルを開く

	if (pFile != NULL)
	{
		fscanf(pFile, "%d\n", &m_nScore);
	}
	else
	{// ファイルへの読み込みに失敗
		return E_FAIL;
	}

	fclose(pFile);

	// --- スコアの背景 --- //
	CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2 + 10.0f, SCREEN_HEIGHT / 2 + 30.0f, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_HALFWHITE,
		300.0f,
		100.0f,
		CObject::TYPE_BG);

	// --- スコアの表示 --- //
	m_pScore = CScore::Create(D3DXVECTOR3(SCREEN_WIDTH / 2 - 100.0f, 400.0f, 0.0f), VEC_NULL, VEC_NULL, COLOR_BLACK, 70.0f, 100.0f, 4, "data\\TEXTURE\\number.png");
	m_pScore->AddScore(m_nScore);

	// --- 小数点表現ポリゴン --- //
	CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2 + 75.0f, 430.0f, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_BLACK,
		10.0f,
		10.0f,
		CObject::TYPE_BG);

	m_nCntFade = 600;	// カウンターの設定

	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CResult::Uninit(void)
{
	m_pBg = NULL;	// 背景のポリゴン用
	m_pUI = NULL;	// UIのポリゴン用
	m_nIdxTex = -1;		// テクスチャのインデックス
	m_nCntFade = 0;		// 自動で画面遷移するまでのカウンター
	m_pMesh = NULL;		// メッシュのポインタ
	m_pSide = NULL;		// 限界値のビルポリゴン
	m_pScore = NULL;	// スコアのポインタ
	m_nScore = 0;		// 獲得スコア

	if (m_pCamera != NULL)
	{
		m_pCamera = NULL;
		delete m_pCamera;
	}

	if (m_pLight != NULL)
	{
		m_pLight = NULL;	// ライトのポインタ
		delete m_pLight;
	}
}

// =================================================
// 更新処理
// =================================================
void CResult::Update(void)
{
	// --- キーボードの取得 --- //
	CInputKeyboard* pInputkeyboard = CManager::GetInputKeyboard();
	CDebugProc* pDebugProc = CManager::GetDrbugProc();		// デバッグ表示のポインタ
	CJoyPad* pJoyPad = CManager::GetJoyPad();	// ジョイパッドの取得
	CSound* pSound = CManager::GetSound();

	// --- カメラの更新処理 --- //
	m_pCamera->Update();

	// --- ライトの更新処理 --- //
	m_pLight->Update();

	// --- スコアの更新処理 --- //
	m_pScore->Update();

	m_nCntFade--;	// フェードカウンタの減少

	CFade* pFade = CManager::GetFade();
	CFade::FADE fade = pFade->GetFade();

	if (pInputkeyboard->GetTrigger(DIK_RETURN) == true
		|| pJoyPad->GetTrigger(CJoyPad::JOYKEY_START)
		|| pJoyPad->GetTrigger(CJoyPad::JOYKEY_A)
		|| pJoyPad->GetTrigger(CJoyPad::JOYKEY_B)
		|| pJoyPad->GetTrigger(CJoyPad::JOYKEY_X)
		|| pJoyPad->GetTrigger(CJoyPad::JOYKEY_Y))
	{
		if (fade == CFade::FADE_NONE)
		{
			CManager::SetStopSound(false);	// BGM終了フラグを立てない
			UpdateRanking();
			CFade::SetFade(CScene::MODE_RANKING);
		}
	}
	else if (m_nCntFade <= 0)
	{
		if (fade == CFade::FADE_NONE)
		{
			CManager::SetStopSound(false);	// BGM終了フラグを立てない
			UpdateRanking();
			CFade::SetFade(CScene::MODE_RANKING);
		}
	}

	pDebugProc->PrintDebug("result");
}

// =================================================
// 描画処理
// =================================================
void CResult::Draw(void)
{

}

// =================================================
// ランキングの更新
// =================================================
void CResult::UpdateRanking(void)
{
	FILE* pFile;

	int nRank = 10;	// 更新した順位
	int nCurrentScore = 0;	//	今回の獲得スコア

	// --- 外部ファイルに保存していた今回のスコアを取得 --- //
	pFile = fopen("data\\scripts\\score.txt", "r");

	if (pFile != NULL)
	{
		fscanf(pFile, "%d\n", &nCurrentScore);
	}
	else
	{// ファイルへの読み込みに失敗

	}

	fclose(pFile);

	// --- オンラインランキング処理 --- //
	CTcpClient* pClient = new CTcpClient;

	// --- 初期化処理関数を実行 --- //
	printf("[WinSockの初期化を実行]\n");
	WSADATA wsaData;	// 変数宣言

	WSAStartup(WINSOCK_VERSION, &wsaData);

	char aIPAddress[256];	// IPアドレスの保存用
	strcpy(&aIPAddress[0], "127.0.0.1");
	char aData[128] = {};	// 送信用の変数
	int nSendByte;	// 送信するバイト数
	int nRecvByte;	// 受信したバイト数

	if (pClient->Init(&aIPAddress[0], PORT_NUMBER) == true)
	{
		strcpy(&aData[0], "1");	// 1バイト目をランキング更新の合図に設定

		std::string aCurrentScore = std::to_string(nCurrentScore);

		for (int nCnt = 0; nCnt < aCurrentScore.size(); nCnt++)
		{// std型をchar型にキャスト
			aData[nCnt + 1] = aCurrentScore[nCnt];
		}

		for (int nCnt = 0; nCnt < 128; nCnt++)
		{
			if (aData[nCnt] == NULL)
			{
				strcpy(&aData[nCnt], "\0");	// 終端に\0(NULL文字)を入れる
			}
		}

		// --- 送信処理 --- //
		nSendByte = pClient->Send(&aData[0], sizeof(aData));

		// --- 受信処理 --- //
		nRecvByte = pClient->Recv(&nRank, nSendByte);

		// --- エンディアン返還 --- //
		//nRank = ntohl(nRank);

		m_nRank = nRank;	// 受け取った順位を保存

		// --- 切断処理 --- //
		pClient->Uninit();

		// --- メモリの破棄 --- //
		if (pClient != NULL)
		{
			delete pClient;
		}

		// --- Winsock終了処理 --- //
		WSACleanup();
	}

	// --- 外部ファイルに更新順位をを保存 --- //
	pFile = fopen("data\\scripts\\rank.txt", "w");

	if (pFile != NULL)
	{
		fprintf(pFile, "%d\n", m_nRank);
	}
	else
	{// ファイルへの保存に失敗

	}

	fclose(pFile);
}