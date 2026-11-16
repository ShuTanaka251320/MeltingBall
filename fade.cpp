// =================================================
// 
// フェード処理[fade.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "fade.h"
#include "input.h"
#include "manager.h"
#include "renderer.h"

// =================================================
// 静的メンバ変数宣言
CFade::FADE CFade::m_fade = FADE_NONE;				// フェードの状態
CScene::MODE CFade::m_modeNext = CScene::MODE_TITLE;	// 次のモード	
D3DXCOLOR CFade::m_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 黒いポリゴンを(不透明)にする

// =================================================
// コンストラクタ
// =================================================
CFade::CFade()
{
}

// =================================================
// デストラクタ
// =================================================
CFade::~CFade()
{

}

// =================================================
// フェードの初期化処理
// =================================================
void CFade::Init(CScene::MODE modeNext)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9	pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	m_fade = FADE_IN;								// フェードイン状態
	m_modeNext = modeNext;							// 次の画面を設定
	m_color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);	// 黒いポリゴンを(不透明)にする

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_2D* pVtx;		// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(1280.0f, 0.0f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f, 720.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(1280.0f, 720.0f, 0.0f);

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	// モードの設定
	CManager::SetMode(m_modeNext);
}

// =================================================
// フェードの終了処理
// =================================================
void CFade::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}
}

// =================================================
// フェードの更新処理
// =================================================
void CFade::Update(void)
{
	VERTEX_2D* pVtx;		// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);
	
	if (m_fade != FADE_NONE)
	{
		if (m_fade == FADE_IN)
		{// フェードイン状態
			m_color.a -= 0.03f;

			if (m_color.a <= 0)
			{
				m_color.a = 0;
				m_fade = FADE_NONE;
			}
		}
		else if (m_fade == FADE_OUT)
		{// フェードアウト状態
			m_color.a += 0.03f;

			if (m_color.a >= 1.0f)
			{
				m_color.a = 1.0f;
				m_fade = FADE_IN;

				// --- モード設定 --- //
				CManager::SetMode(m_modeNext);
			}
		}

		// 頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_color.a);
		pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_color.a);
		pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_color.a);
		pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, m_color.a);
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

// =================================================
// フェードの描画処理
// =================================================
void CFade::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9	pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, NULL);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

// =================================================
// フェードの設定
// =================================================
void CFade::SetFade(CScene::MODE modeNext)
{
	if (m_fade == FADE_NONE)
	{
		m_fade = FADE_OUT;								// フェードアウト状態に
		m_modeNext = modeNext;							// 次の画面を設定
		m_color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);	// 黒ポリゴンに(透明)する
	}
}
