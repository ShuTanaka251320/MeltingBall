// =================================================
// 
// レンダラー処理[renderer.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include"renderer.h"
#include"object.h"
#include"debugproc.h"
#include"manager.h"

#include"player.h"
#include"camera.h"
#include"light.h"
#include"objectX.h"
#include"fade.h"

// =================================================
// コンストラクタ
// =================================================
CRenderer::CRenderer()
{
	// 各種変数のクリア
	m_pD3D = NULL;			// Direct3Dオブジェクトへのポインタ 
	m_pD3DDevice = NULL;	// Direct3Dデバイスへのポインタ
	m_pTextureMT = NULL;			// レンダリングターゲット用テクスチャ
	m_pRenderMT = NULL;			// テクスチャレンダリング用インターフェース
	m_pZBuffMT = NULL;				// テクスチャレンダリング用Zバッファ
	m_viewportMT = {};			// テクスチャレンダリング用ビューポート
}

// =================================================
// デストラクタ
// =================================================
CRenderer::~CRenderer()
{

}

// =================================================
// 初期化処理
// =================================================
HRESULT CRenderer::Init(HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;			// ディスプレイモード
	D3DPRESENT_PARAMETERS d3dpp;	// プレゼンテーションパラメータ

	// --- Direct3Dオブジェクトの生成 --- //
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pD3D == NULL)
	{
		return E_FAIL;
	}

	// --- 現在のディスプレイモードを取得 --- //
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// --- デバイスのプレゼンテーションパラメータの設定 --- //
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.BackBufferCount = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.Windowed = bWindow;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	// --- Direct3Dデバイスの生成 --- //
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_pD3DDevice)))
	{
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&m_pD3DDevice)))
		{
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&m_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

	// --- レンダーステートの設定 --- //
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// --- サンプラーステートの設定 --- //
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// --- テクスチャステージステートの設定 --- //
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	// ***** マルチターゲットテクスチャ処理 ***** //
	LPDIRECT3DSURFACE9 pRenderDef, pZBuffDef;

	// --- レンダリングターゲットテクスチャの生成 --- //
	m_pD3DDevice->CreateTexture(SCREEN_WIDTH,
		SCREEN_HEIGHT,
		1,
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_pTextureMT,
		NULL);

	// --- テクスチャレンダリング用インターフェースの生成 --- //  
	m_pTextureMT->GetSurfaceLevel(0, &m_pRenderMT);

	// --- テクスチャレンダリング用Zバッファの生成 --- //
	m_pD3DDevice->CreateDepthStencilSurface(SCREEN_WIDTH,
		SCREEN_HEIGHT,
		D3DFMT_D16,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&m_pZBuffMT,
		NULL);

	// --- 現在のレンダリングターゲットの取得 --- //
	m_pD3DDevice->GetRenderTarget(0, &pRenderDef);

	// --- 現在のZバッファを取得 --- //
	m_pD3DDevice->GetDepthStencilSurface(&pZBuffDef);

	// --- レンダリングターゲットを生成したテクスチャに設定 --- //
	m_pD3DDevice->SetRenderTarget(0, m_pRenderMT);

	// --- Zバッファを生成したものに設定 --- //
	m_pD3DDevice->SetDepthStencilSurface(m_pZBuffMT);

	// --- レンダリングターゲット用テクスチャのクリア --- //
	m_pD3DDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	// --- レンダリングターゲットをもとに戻す --- //
	m_pD3DDevice->SetRenderTarget(0, pRenderDef);

	// --- Zバッファをもとに戻す --- //
	m_pD3DDevice->SetDepthStencilSurface(pZBuffDef);

	// --- テクスチャレンダリング用ビューポートの設定 --- //
	m_viewportMT.X = 0;
	m_viewportMT.Y = 0;
	m_viewportMT.Width = SCREEN_WIDTH;
	m_viewportMT.Height = SCREEN_HEIGHT;
	m_viewportMT.MinZ = 0.0f;
	m_viewportMT.MaxZ = 1.0f;

	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CRenderer::Uninit(void)
{
	// --- Direct3Dデバイスの破棄 --- //
	if (m_pD3DDevice != NULL)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	// --- Direct3Dオブジェクトの破棄 --- //
	if (m_pD3D != NULL)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}

	// --- マルチターゲットテクスチャの破棄 --- //
	if (m_pTextureMT != NULL)
	{
		m_pTextureMT->Release();
		m_pTextureMT = NULL;
	}

	// --- マルチターゲットインターフェースの破棄 --- //
	if (m_pRenderMT != NULL)
	{
		m_pRenderMT->Release();
		m_pRenderMT = NULL;
	}

	// --- マルチターゲットZバッファの破棄 --- //
	if (m_pZBuffMT != NULL)
	{
		m_pZBuffMT->Release();
		m_pZBuffMT = NULL;
	}
}

// =================================================
// 更新処理
// =================================================
void CRenderer::Update(void)
{
	CDebugProc* pDebug = CManager::GetDrbugProc();	// デバッグ表示のポインタ
	pDebug->PrintDebug("FPS : [%d]\n", GetFPS());		// FPSの表示

	// --- 全オブジェクトの更新処理 --- //
	CObject::UpdateAll();
}

// =================================================
// 描画処理
// =================================================
void CRenderer::Draw(void)
{
	// --- 画面クリア --- //
	if (CManager::GetMode() == CScene::MODE_GAME)
	{

	m_pD3DDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(128, 128, 128, 0), 1.0f, 0);
	}
	else
	{
		m_pD3DDevice->Clear(0, NULL,
			(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
			D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);
	}


	CDebugProc* pDebug = CManager::GetDrbugProc();	// デバッグ表示のポインタ
	CFade* pFade = CManager::GetFade();

	// 描画開始
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{// 描画開始が成功

		// -- 全オブジェクトの描画処理 --- //
		CObject::DrawAll();

		// --- デバッグ表示の描画処理 --- //
		pDebug->Draw();

		// --- フェードの描画 --- //
		pFade->Draw();

		// --- 描画終了 --- //
		m_pD3DDevice->EndScene();
	}

	// --- バックバッファとフロントバッファの入れ替え --- //
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

// =================================================
// 描画対象の変更
// =================================================
void CRenderer::ChangeTarget(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 posU)
{
	D3DXMATRIX mtxview, mtxProjection;
	float fAspect;

	// --- レンダリングターゲットをテクスチャに設定 --- //
	m_pD3DDevice->SetRenderTarget(0, m_pRenderMT);

	// --- Zバッファを生成したZバッファに設定 --- //
	m_pD3DDevice->SetDepthStencilSurface(m_pZBuffMT);

	// --- テクスチャレンダリング用のビューポートを設定 --- //
	m_pD3DDevice->SetViewport(&m_viewportMT);

	// --- ビューマトリックスの初期化 --- //
	D3DXMatrixIdentity(&mtxview);

	// --- ビューマトリックスの生成 --- //
	D3DXMatrixLookAtLH(&mtxview,
		&posV,
		&posR,
		&posU);

	// --- ビューマトリックスの設定 --- //
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &mtxview);

	// --- プロジェクションマトリックスの初期化 --- //
	D3DXMatrixIdentity(&mtxProjection);

	// --- プロジェクションマトリックスの作成 --- //
	fAspect = (float)m_viewportMT.Width / (float)m_viewportMT.Height;

	D3DXMatrixPerspectiveFovLH(&mtxProjection,
		D3DXToRadian(45.0f),
		fAspect,
		10.0f,
		10000.0f);

	// --- プロジェクションマトリックスの設定 --- //
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &mtxProjection);
}

