// =================================================
// 
// 数字処理[number.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include"number.h"
#include"renderer.h"
#include"manager.h"
#include"texture.h"

// =================================================
// 静的メンバ変数
LPDIRECT3DTEXTURE9 CNumber::m_pTexture = NULL;	// テクスチャのポインタ

// =================================================
// コンストラクタ
// =================================================
CNumber::CNumber()
{
	m_pos = {};		// 位置
	m_move = {};	// 移動量
	m_rot = {};		// 向き
	m_fLength = 0.0f;	// ポリゴンの対角線の長さ
	m_fAngle = 0.0f;	// ポリゴンの対角線の角度
	m_fWidth = 0.0f;	// 横幅
	m_fHeight = 0.0f;	// 縦幅
	m_nIdxTex = -1;		// テクスチャのインデックス
	m_fAlpha = 1.0f;			// ポリゴンの透明度
	m_nFlashCounter = 0;		// 点滅のカウンター
	m_nFadeCounter = 0;	// フェードのカウンター
}

// =================================================
// デストラクタ
// =================================================
CNumber::~CNumber()
{

}

// =================================================
// 数字の初期化処理
// =================================================
HRESULT CNumber::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9	pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	// --- 頂点バッファの生成 --- //
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	if (m_pVtxBuff == NULL)
	{// バッファ読み込み失敗
		return E_FAIL;
	}

	VERTEX_2D* pVtx;		// 頂点情報へのポインタ

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// --- 各種メンバ変数の初期化 --- //
	m_pos = pos;	// 位置
	m_move = move;	// 移動量
	m_rot = rot;	// 向き
	m_col = col;	// 色
	m_fLength = sqrtf(fWidth * fWidth + fHeight * fHeight) * 0.5f;	// 対角線の長さ
	m_fAngle = (float)atan2(fWidth, fHeight);						// 対角線の角度
	m_fWidth = fWidth;		// 横幅
	m_fHeight = fHeight;	// 高さ

	// --- 頂点座標の設定 --- //
	pVtx[0].pos.x = m_pos.x + sinf(m_rot.z + m_fAngle + D3DX_PI) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(m_rot.z + m_fAngle + D3DX_PI) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + sinf(m_rot.z + D3DX_PI - m_fAngle) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(m_rot.z + D3DX_PI - m_fAngle) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + sinf(m_rot.z - m_fAngle) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(m_rot.z - m_fAngle) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + sinf(m_rot.z + m_fAngle) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_rot.z + m_fAngle) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// --- rhwの設定 --- //
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// --- 頂点カラーの設定 --- //
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// --- テクスチャ座標の設定 --- //
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// --- 頂点バッファをアンロックする --- //
	m_pVtxBuff->Unlock();

	return S_OK;
}

// =================================================
// 数字の終了処理
// =================================================
void CNumber::Uninit()
{
	// --- 頂点バッファの破棄 --- //
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}
}

// =================================================
// 数字の更新処理
// =================================================
void CNumber::Update(void)
{
	VERTEX_2D* pVtx;		// 頂点情報へのポインタ

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// --- 頂点座標の設定 --- //
	pVtx[0].pos.x = m_pos.x + sinf(m_rot.z + m_fAngle + D3DX_PI) * m_fLength;
	pVtx[0].pos.y = m_pos.y + cosf(m_rot.z + m_fAngle + D3DX_PI) * m_fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + sinf(m_rot.z + D3DX_PI - m_fAngle) * m_fLength;
	pVtx[1].pos.y = m_pos.y + cosf(m_rot.z + D3DX_PI - m_fAngle) * m_fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + sinf(m_rot.z - m_fAngle) * m_fLength;
	pVtx[2].pos.y = m_pos.y + cosf(m_rot.z - m_fAngle) * m_fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + sinf(m_rot.z + m_fAngle) * m_fLength;
	pVtx[3].pos.y = m_pos.y + cosf(m_rot.z + m_fAngle) * m_fLength;
	pVtx[3].pos.z = 0.0f;

	// --- 頂点バッファをアンロックする --- //
	m_pVtxBuff->Unlock();
}

// =================================================
// 数字の描画処理
// =================================================
void CNumber::Draw(void)
{
	CTexture* pTexture = CManager::GetTexture();

	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	// --- 頂点バッファをデータストリームに設定 --- //
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// --- 頂点フォーマットの設定 --- //
	pDevice->SetFVF(FVF_VERTEX_2D);

	// --- テクスチャの設定 --- //
	if (m_nIdxTex >= 0)
	{
		pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTex));
	}
	else
	{
		pDevice->SetTexture(0, NULL);
	}

	// --- ポリゴンの描画 --- //
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

// =================================================
// 数字の生成
// =================================================
CNumber* CNumber::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight)
{
	CNumber* pNumber;	// ポインタ宣言

	// --- オブジェクトの生成 --- //
	pNumber = new CNumber;

	// --- 初期化 --- //
	pNumber->Init(pos, rot, move, col, fWidth, fHeight);

	return pNumber;
}

// =================================================
// ポリゴンの数字の変更
// =================================================
void CNumber::SetNumber(int nNumber)
{
	if (m_pVtxBuff != NULL)
	{
		VERTEX_2D* pVtx;		// 頂点情報へのポインタ

		// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		pVtx[0].tex = D3DXVECTOR2(0.1f * (float)nNumber, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(0.1f * (float)nNumber + 0.1f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.1f * (float)nNumber, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(0.1f * (float)nNumber + 0.1f, 1.0f);

		// --- 頂点バッファをアンロックする --- //
		m_pVtxBuff->Unlock();
	}
}

// =================================================
// ポリゴンの色変更
// =================================================
void CNumber::SetCol(D3DXCOLOR col)
{
	VERTEX_2D* pVtx;		// 頂点情報へのポインタ

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// --- 頂点バッファをアンロックする --- //
	m_pVtxBuff->Unlock();
}

// =================================================
// ポリゴンの点滅処理
// =================================================
void CNumber::FlashPol(int nCntFlash)
{
	if (m_pVtxBuff != NULL)
	{
		VERTEX_2D* pVtx;		// 頂点情報へのポインタ

		// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		m_nFlashCounter++;

		if (m_nFlashCounter % nCntFlash >= (nCntFlash / 3))
		{
			// --- 頂点座標の設定 --- //
			pVtx[0].pos = { 0.0f, 0.0f, 0.0f };
			pVtx[1].pos = { 0.0f, 0.0f, 0.0f };
			pVtx[2].pos = { 0.0f, 0.0f, 0.0f };
			pVtx[3].pos = { 0.0f, 0.0f, 0.0f };
		}
		else
		{
			// --- 頂点座標の設定 --- //
			pVtx[0].pos.x = m_pos.x + sinf(m_rot.z + m_fAngle + D3DX_PI) * m_fLength;
			pVtx[0].pos.y = m_pos.y + cosf(m_rot.z + m_fAngle + D3DX_PI) * m_fLength;
			pVtx[0].pos.z = 0.0f;

			pVtx[1].pos.x = m_pos.x + sinf(m_rot.z + D3DX_PI - m_fAngle) * m_fLength;
			pVtx[1].pos.y = m_pos.y + cosf(m_rot.z + D3DX_PI - m_fAngle) * m_fLength;
			pVtx[1].pos.z = 0.0f;

			pVtx[2].pos.x = m_pos.x + sinf(m_rot.z - m_fAngle) * m_fLength;
			pVtx[2].pos.y = m_pos.y + cosf(m_rot.z - m_fAngle) * m_fLength;
			pVtx[2].pos.z = 0.0f;

			pVtx[3].pos.x = m_pos.x + sinf(m_rot.z + m_fAngle) * m_fLength;
			pVtx[3].pos.y = m_pos.y + cosf(m_rot.z + m_fAngle) * m_fLength;
			pVtx[3].pos.z = 0.0f;
		}

		// --- 頂点バッファをアンロックする --- //
		m_pVtxBuff->Unlock();
	}
}


// =================================================
// ポリゴンのフェード消滅
// =================================================
float CNumber::FadePol(int nFadeTime)
{
	if (m_fAlpha >= 0.0f)
	{
		if (m_pVtxBuff != NULL)
		{
			VERTEX_2D* pVtx;		// 頂点情報へのポインタ

			// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
			m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			m_nFadeCounter++;

			if (m_nFadeCounter % nFadeTime >= (nFadeTime / 2))
			{
				m_fAlpha -= 0.1f;
			}

			if (m_fAlpha <= 0.0f)
			{
				m_fAlpha = 0.0f;
			}

			// --- 頂点カラーの設定 --- //
			pVtx[0].col = D3DXCOLOR(m_col.r, m_col.g, m_col.b, m_fAlpha);
			pVtx[1].col = D3DXCOLOR(m_col.r, m_col.g, m_col.b, m_fAlpha);
			pVtx[2].col = D3DXCOLOR(m_col.r, m_col.g, m_col.b, m_fAlpha);
			pVtx[3].col = D3DXCOLOR(m_col.r, m_col.g, m_col.b, m_fAlpha);

			// --- 頂点バッファをアンロックする --- //
			m_pVtxBuff->Unlock();

			if (m_fAlpha <= 0.0f)
			{
				//Uninit();
			}

			return m_fAlpha;
		}
	}

	return 0.0f;
}
