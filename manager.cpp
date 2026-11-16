// =================================================
// 
// マネージャー処理[manager.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include<ctime>

#include"manager.h"
#include"macro.h"
#include"sound.h"
#include"object2D.h"
#include"object3D.h"
#include"objectX.h"
#include"texture.h"
#include"Xmodel.h"
#include"renderer.h"
#include"input.h"
#include"debugproc.h"
#include"player.h"
#include "camera.h"
#include "light.h"
#include "item.h"
#include "wall.h"
#include "field.h"
#include "meshring.h"
#include "meshcylinder.h"
#include "tcp_client.h"

#include "scene.h"
#include "fade.h"

// =================================================
// マクロ定義
#define RANK_FILE	("data\\SCRIPTS\\rank.txt")

// =================================================
// 静的メンバ変数宣言
CRenderer* CManager::m_pRenderer = NULL;			// レンダラーのポインタ
CInputKeyboard* CManager::m_pInputKeyboard = NULL;	// キーボードのポインタ
CInputMouse* CManager::m_pInputMouse = NULL;		// マウスのポインタ
CJoyPad* CManager::m_pJoyPad = NULL;				// ジョイパッドのポインタ
CDebugProc* CManager::m_pDebugProc = NULL;			// デバッグ表示のポインタ
CSound* CManager::m_pSound = NULL;					// サウンドのポインタ
CFade* CManager::m_pFade = NULL;		// フェードのポインタ
CScene* CManager::m_pScene = NULL;		// シーンのポインタ

CTexture* CManager::m_pTexture = NULL;				// テクスチャのポインタ
CXModel* CManager::m_pXModel = NULL;				// モデルマネージャのポインタ

int CManager::m_nRank = 0;	// 順位

bool CManager::m_bPause = false;					// ポーズ判定
bool CManager::m_bFinish = false;					// ゲーム終了判定
bool CManager::m_bTutorial = false;					// チュートリアル終了判定
bool CManager::m_bStopSound = false;				// サウンド終了フラグ
bool CManager::m_bDrawParticle = true;				// パーティクルの描画フラグ

// =================================================
// コンストラクタ
// =================================================
CManager::CManager()
{
	m_bPause = false;
	m_bFinish = false;
	m_bTutorial = false;
}

// =================================================
// デストラクタ
// =================================================
CManager::~CManager()
{

}

// =================================================
// 初期化処理
// =================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	srand(time(0));		// 乱数の種を生成

	// --- 各種メモリの生成 --- //
	if (m_pRenderer == NULL)
	{// レンダラー
		m_pRenderer = new CRenderer;
	}

	if (m_pInputKeyboard == NULL)
	{// キーボード
		m_pInputKeyboard = new CInputKeyboard;
	}

	if (m_pInputMouse == NULL)
	{// マウス
		m_pInputMouse = new CInputMouse;
	}

	if (m_pJoyPad == NULL)
	{// ジョイパッド
		m_pJoyPad = new CJoyPad;
	}

	if (m_pDebugProc == NULL)
	{// デバッグ表示
		m_pDebugProc = new CDebugProc;
	}

	if (m_pSound == NULL)
	{// サウンド
		m_pSound = new CSound;
	}

	if (m_pTexture == NULL)
	{// テクスチャ
		m_pTexture = new CTexture;
	}

	if (m_pXModel == NULL)
	{// Xモデル
		m_pXModel = new CXModel;
	}

	if (m_pFade == NULL)
	{// フェード
		m_pFade = new CFade;
	}

	// --- キーボードの初期化処理 --- //
	if (FAILED(m_pInputKeyboard->Init(hInstance, hWnd)))
	{
		return E_FAIL;
	}

	// --- マウスの初期化処理 --- //
	if (FAILED(m_pInputMouse->Init(hInstance, hWnd)))
	{
		return E_FAIL;
	}

	// --- ジョイパッドの初期化処理 --- //
	if (FAILED(m_pJoyPad->Init()))
	{
		return E_FAIL;
	}

	// --- レンダラーの初期化処理 --- //
	if (FAILED(m_pRenderer->Init(hWnd, bWindow)))
	{
		return E_FAIL;
	}

	// --- デバッグ表示の初期化処理 --- //
	m_pDebugProc->Init();

	// --- サウンドの初期化 --- //
	m_pSound->Init(hWnd);

	// --- メモリのクリア --- //
	CObject::ReleaseAll();

	// --- フェードの初期化 --- //
	m_pFade->Init(CScene::MODE_TITLE);

	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CManager::Uninit(void)
{
	// --- 全オブジェクトの破棄 --- //
	CObject::ReleaseAll();

	// --- 各種終了処理 --- //
	m_pDebugProc->Uninit();
	m_pInputKeyboard->Uninit();
	m_pInputMouse->Uninit();
	m_pJoyPad->Uninit();
	m_pSound->Uninit();
	m_pFade->Uninit();

	// --- Xモデルの全破壊 --- //
	m_pXModel->Unload();

	// --- テクスチャの全破棄 --- //
	m_pTexture->Unload();

	if (m_pRenderer != NULL)
	{// メモリの破棄
		delete m_pRenderer;
		m_pRenderer = NULL;
	}

	if (m_pInputKeyboard != NULL)
	{// メモリの破棄
		delete m_pInputKeyboard;
		m_pInputKeyboard = NULL;
	}

	if (m_pInputMouse != NULL)
	{// メモリの破棄
		delete m_pInputMouse;
		m_pInputMouse = NULL;
	}

	if (m_pJoyPad != NULL)
	{// メモリの破棄
		delete m_pJoyPad;
		m_pJoyPad = NULL;
	}

	if (m_pDebugProc != NULL)
	{// メモリの破棄
		delete m_pDebugProc;
		m_pDebugProc = NULL;
	}

	if (m_pSound != NULL)
	{// メモリの破棄
		delete m_pSound;
		m_pSound = NULL;
	}

	if (m_pTexture != NULL)
	{// メモリの破棄
		delete m_pTexture;
		m_pTexture = NULL;
	}

	if (m_pXModel != NULL)
	{// メモリの破棄
		delete m_pXModel;
		m_pXModel = NULL;
	}


	if (m_pScene != NULL)
	{// メモリの破棄
		delete m_pScene;
		m_pScene = NULL;
	}

	if (m_pFade != NULL)
	{// メモリの破棄
		delete m_pFade;
		m_pFade = NULL;
	}
}

// =================================================
// 更新処理
// =================================================
void CManager::Update(void)
{
	if (m_bPause == false)
	{
		// --- レンダラーの更新処理 --- //
		m_pRenderer->Update();
	}

	// --- キーボードの更新処理 --- //
	m_pInputKeyboard->Update();

	// --- マウスの更新処理 --- //
	m_pInputMouse->Update();

	// --- ジョイパッドの更新処理 --- //
	m_pJoyPad->Update();

	// --- デバッグ表示の更新処理 --- //
	m_pDebugProc->Update();

	// --- シーンの更新 --- //
	m_pScene->Update();

	// --- フェードの更新 --- //
	m_pFade->Update();
}

// =================================================
// 描画処理
// =================================================
void CManager::Draw(void)
{
	// --- レンダラーの描画処理 --- //
	m_pRenderer->Draw();
}

// =================================================
// シーン設定処理
// =================================================
void CManager::SetMode(CScene::MODE mode)
{
	// --- サウンドの停止 --- //
	if (m_pSound != NULL)
	{// NULLチェック
		if (m_bStopSound == true)
		{
			m_pSound->Stop();
		}
	}

	// --- 現在のモードの破棄 --- //
	if (m_pScene != NULL)
	{// NULLチェック
		m_pScene->Uninit();
	}

	// --- 全オブジェクトの破棄 --- //
	CObject::ReleaseAll();

	// --- 新しいシーンの作成 --- //
	m_pScene = CScene::Create(mode);
}

// =================================================
// 現在のシーン情報を渡す
// =================================================
CScene::MODE CManager::GetMode(void)
{
	return m_pScene->GetMode();
}


