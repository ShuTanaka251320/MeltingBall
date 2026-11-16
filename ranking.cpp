// =================================================
// 
// タイトル処理[title.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include<winsock2.h>

#include "ranking.h"
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
#include "tcp_client.h"

// =================================================
// マクロ定義
#define RANK_POS	D3DXVECTOR3(SCREEN_WIDTH / 4 + 200.0f, 200.0f + (nCnt * 90.0f), 0.0f)
#define PORT_NUMBER	(22333)		// ポート番号

// =================================================
// 静的メンバ変数宣言
CCamera* CRanking::m_pCamera = NULL;
CObject2D* CRanking::m_pBg = NULL;	// 背景のポリゴン用
CObject2D* CRanking::m_pUI = NULL;	// UIのポリゴン用
int CRanking::m_nIdxTex = -1;		// テクスチャのインデックス
int CRanking::m_nCntFade = 0;		// 自動で画面遷移するまでのカウンター
CScore* CRanking::m_pScore[MAX_RANKING] = {};		// スコアのポインタ
int CRanking::m_nUpdateRank = 0;

// =================================================
// コンストラクタ
// =================================================
CRanking::CRanking(CScene::MODE mode) :CScene(mode)
{

}

// =================================================
// デストラクタ
// =================================================
CRanking::~CRanking()
{

}

// =================================================
// 初期化処理
// =================================================
HRESULT CRanking::Init(void)
{
	m_pCamera = new CCamera;

	if (m_pCamera != NULL)
	{
		m_pCamera->Init();
	}

	// --- 各種UIの生成 --- //
	CreateUI();

	// --- ランキング情報の読み込み --- //
	LoadRanking();

	m_nCntFade = 600;	// カウンターの設定

	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CRanking::Uninit(void)
{
	if (m_pCamera != NULL)
	{
		m_pCamera->Uninit();

		delete m_pCamera;
		m_pCamera = NULL;
	}

	m_pBg = NULL;	// 背景のポリゴン用
	m_pUI = NULL;	// UIのポリゴン用
	m_nIdxTex = -1;		// テクスチャのインデックス
	m_nCntFade = 0;		// 自動で画面遷移するまでのカウンター
	memset(m_pScore, 0, sizeof(m_pScore));		// スコアのポインタ
	m_nUpdateRank = 0;
}

// =================================================
// 更新処理
// =================================================
void CRanking::Update(void)
{
	// --- キーボードの取得 --- //
	CInputKeyboard* pInputkeyboard = CManager::GetInputKeyboard();
	CJoyPad* pJoyPad = CManager::GetJoyPad();	// ジョイパッドの取得
	CSound* pSound = CManager::GetSound();

	m_pCamera->Update();

	m_nCntFade--;	// フェードカウンタの減少

	if (m_nUpdateRank >= 0 && m_nUpdateRank <= 4)
	{
		m_pScore[m_nUpdateRank]->FlashPol(60);
	}

	if (pInputkeyboard->GetTrigger(DIK_RETURN) == true
		|| pJoyPad->GetTrigger(CJoyPad::JOYKEY_START)
		|| pJoyPad->GetTrigger(CJoyPad::JOYKEY_A)
		|| pJoyPad->GetTrigger(CJoyPad::JOYKEY_B)
		|| pJoyPad->GetTrigger(CJoyPad::JOYKEY_X)
		|| pJoyPad->GetTrigger(CJoyPad::JOYKEY_Y)
		|| m_nCntFade <= 0)
	{
		CManager::SetStopSound(true);	// BGM終了フラグを立てる
		CFade::SetFade(CScene::MODE_TITLE);
	}

	CDebugProc* pDebugProc = CManager::GetDrbugProc();				// デバッグ表示のポインタ
	pDebugProc->PrintDebug("ranking");
}

// =================================================
// 描画処理
// =================================================
void CRanking::Draw(void)
{

}

// =================================================
// UI生成
// =================================================
void CRanking::CreateUI(void)
{
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	// --- RANKINGポリゴン --- //
	m_pUI = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2 + 10.0f, SCREEN_HEIGHT / 7, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		1000.0f,
		200.0f,
		CObject::TYPE_BG);

	// --- テクスチャ設定 --- //
	m_nIdxTex = pTexture->Register("data\\TEXTURE\\ranking.png");
	m_pUI->SetTexture(m_nIdxTex);

	// --- 順位ポリゴン --- //
	m_pUI = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 4 + 10.0f, SCREEN_HEIGHT / 5 + 50.0f, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		150.0f,
		150.0f,
		CObject::TYPE_BG);

	// --- テクスチャ設定 --- //
	m_nIdxTex = pTexture->Register("data\\TEXTURE\\1st.png");
	m_pUI->SetTexture(m_nIdxTex);

	// --- 順位ポリゴン --- //
	m_pUI = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 4 + 10.0f, SCREEN_HEIGHT / 3 + 50, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		130.0f,
		130.0f,
		CObject::TYPE_BG);

	// --- テクスチャ設定 --- //
	m_nIdxTex = pTexture->Register("data\\TEXTURE\\2nd.png");
	m_pUI->SetTexture(m_nIdxTex);

	// --- 順位ポリゴン --- //
	m_pUI = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 4 + 5.0f, SCREEN_HEIGHT / 2 + 20.0f, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		130.0f,
		130.0f,
		CObject::TYPE_BG);

	// --- テクスチャ設定 --- //
	m_nIdxTex = pTexture->Register("data\\TEXTURE\\3rd.png");
	m_pUI->SetTexture(m_nIdxTex);


	// --- 順位ポリゴン --- //
	m_pUI = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2 + 110.0f, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		120.0f,
		120.0f,
		CObject::TYPE_BG);

	// --- テクスチャ設定 --- //
	m_nIdxTex = pTexture->Register("data\\TEXTURE\\4th.png");
	m_pUI->SetTexture(m_nIdxTex);


	// --- 順位ポリゴン --- //
	m_pUI = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2 + 200.0f, 0.0f),
		VEC_NULL,
		VEC_NULL,
		COLOR_WHITE,
		120.0f,
		120.0f,
		CObject::TYPE_BG);

	// --- テクスチャ設定 --- //
	m_nIdxTex = pTexture->Register("data\\TEXTURE\\5th.png");
	m_pUI->SetTexture(m_nIdxTex);

	for (int nCnt = 0; nCnt < MAX_RANKING; nCnt++)
	{
		m_pUI = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2 + 150.0f, SCREEN_HEIGHT / 4 + 30.0f + nCnt * 90.0f, 0.0f),
			VEC_NULL,
			VEC_NULL,
			COLOR_WHITE,
			350.0f,
			150.0f,
			CObject::TYPE_BG);

		// --- テクスチャ設定 --- //
		m_nIdxTex = pTexture->Register("data\\TEXTURE\\persent.png");
		m_pUI->SetTexture(m_nIdxTex);
	}

	for (int nCnt = 0; nCnt < MAX_RANKING; nCnt++)
	{
		CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2 + 55.0f, SCREEN_HEIGHT / 4 + 35.0f + nCnt * 90.0f, 0.0f),
			VEC_NULL,
			VEC_NULL,
			COLOR_WHITE,
			5.0f,
			5.0f,
			CObject::TYPE_BG);
	}
}

// =================================================
// ランキング情報の読み込み
// =================================================
void CRanking::LoadRanking(void)
{
	FILE* pFile;

	int nScore[MAX_RANKING] = {};	// ランキングスコアの保存

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
		strcpy(&aData[0], "2");	// 1バイト目をランキング更新の合図に設定

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
		nRecvByte = pClient->Recv(nScore, nSendByte);

		// --- エンディアン返還 --- //
		for (int nCnt = 0; nCnt < MAX_RANKING; nCnt++)
		{
			nScore[nCnt] = ntohl(nScore[nCnt]);
		}

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

	int nRank = 0;	// 更新した順位の保存

	pFile = fopen("data\\scripts\\rank.txt", "r"); // ファイルを開く

	// --- 更新した順位の取得 --- //
	if (pFile != NULL)
	{
		fscanf(pFile, "%d\n", &nRank);
	}
	else
	{

	}

	fclose(pFile);

	// --- ランキングスコアの表示 --- //
	for (int nCnt = 0; nCnt < MAX_RANKING; nCnt++)
	{
		m_pScore[nCnt] = CScore::Create(RANK_POS, VEC_NULL, VEC_NULL, COLOR_WHITE, 70.0f, 70.0f, 4, "data\\TEXTURE\\number.png");
		m_pScore[nCnt]->AddScore(nScore[nCnt]);
	}

	if (nRank >= 0 && nRank <= 4)
	{// 更新があった順位の色を赤に変更
		m_pScore[nRank]->SetCol(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
		m_nUpdateRank = nRank;
	}
	else
	{
		m_nUpdateRank = 10;
	}
}
