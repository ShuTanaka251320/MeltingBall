// =================================================
// 
// 2Dオブジェクト処理[object2D.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include"object2D.h"
#include"renderer.h"
#include"manager.h"
#include"texture.h"
#include"input.h"
#include"player.h"

// =================================================
// コンストラクタ
// =================================================
CObject2D::CObject2D(int nPriority) :CObject(nPriority)
{
	// --- 各種変数のクリア --- //
	m_pTexture = NULL;	// テクスチャへのポインタ
	m_pVtxBuff = NULL;				// 頂点バッファへのポインタ
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 向き
	m_fLength = 0.0f;	// 対角線の長さ
	m_fDefLength = 0.0f;	// 初期設定の対角線の長さ
	m_fAngle = 0.0f;	// 対角線の角度
	m_nIdxTex = 0;	// テクスチャのインデックス
	m_fAlpha = 1.0f;			// ポリゴンの透明度
	m_nFlashCounter = 0;		// 点滅のカウンター
	m_nFadeCounter = 0;	// フェードのカウンター
	m_fExpand = 0.0f;	// 拡縮量
}

// =================================================
// デストラクタ
// =================================================
CObject2D::~CObject2D()
{

}

// =================================================
// ポリゴンの初期化処理
// =================================================
HRESULT CObject2D::Init(void)
{
	return S_OK;
}

// =================================================
// ポリゴンの初期化処理(オーバーロード)
// =================================================
HRESULT CObject2D::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, CObject::TYPE type, const char* pFileName)
{
	CTexture* pTexture = CManager::GetTexture();

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

	if (pFileName == NULL)
	{
		m_nIdxTex = -1;
	}

	VERTEX_2D* pVtx;		// 頂点情報へのポインタ

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// --- 各種メンバ変数の初期化 --- //
	m_pos = pos;
	m_move = move;
	m_rot = rot;
	m_col = col;
	m_fLength = sqrtf(fWidth * fWidth + fHeight * fHeight) * 0.5f;	// 対角線の長さ
	m_fDefLength = sqrtf(fWidth * fWidth + fHeight * fHeight) * 0.5f;	// 対角線の長さ
	m_fAngle = (float)atan2(fWidth, fHeight);								// 対角線の角度
	m_fWidth = fWidth;		// 横幅
	m_fHeight = fHeight;	// 高さ
	SetType(type);			// タイプ設定

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
// ポリゴンの終了処理
// =================================================
void CObject2D::Uninit()
{
	// --- 頂点バッファの破棄 --- //
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// --- 自身のメモリを破棄 --- //
	CObject::Release();
}

// =================================================
// ポリゴンの更新処理
// =================================================
void CObject2D::Update(void)
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
// ポリゴンの描画処理
// =================================================
void CObject2D::Draw(void)
{
	LPDIRECT3DTEXTURE9 pTexMT;	// マルチターゲットテクスチャ

	// --- テクスチャの取得 --- //
	pTexMT = CManager::GetRenderer()->GetTextureMT();

	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	// --- 頂点バッファをデータストリームに設定 --- //
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// --- 頂点フォーマットの設定 --- //
	pDevice->SetFVF(FVF_VERTEX_2D);

	if (m_nIdxTex >= 0)
	{
		// --- テクスチャの設定 --- //
		pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTex));
	}
	else
	{
		// --- テクスチャの設定 --- //
		pDevice->SetTexture(0, NULL);
	}

	// --- ポリゴンの描画 --- //
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

// =================================================
// ポリゴンの生成
// =================================================
CObject2D* CObject2D::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, CObject::TYPE type, int nPri, const char* pFileName)
{
	CObject2D* pObject2D;	// ポインタ宣言

	// --- オブジェクトの生成 --- //
	pObject2D = new CObject2D(nPri);

	// --- 初期化 --- //
	pObject2D->Init(pos, rot, move, col, fWidth, fHeight, type, pFileName);

	return pObject2D;
}

// =================================================
// ポリゴンの生成
// =================================================
void CObject2D::SetCol(D3DXCOLOR col)
{
	if (m_pVtxBuff != NULL)
	{
		VERTEX_2D* pVtx;		// 頂点情報へのポインタ

		// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// --- 頂点カラーの設定 --- //
		pVtx[0].col = col;
		pVtx[1].col = col;
		pVtx[2].col = col;
		pVtx[3].col = col;

		// --- 頂点バッファをアンロックする --- //
		m_pVtxBuff->Unlock();
	}
}

// =================================================
// ポリゴンの点滅処理
// =================================================
void CObject2D::FlashPol(int nCntFlash)
{
	if (m_pVtxBuff != NULL)
	{
		VERTEX_2D* pVtx;		// 頂点情報へのポインタ

		// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		m_nFlashCounter++;

		if (m_nFlashCounter % nCntFlash >= nCntFlash / 2)
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
float CObject2D::FadePol(int nFadeTime)
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

			// --- 頂点カラーの設定 --- //
			pVtx[0].col = D3DXCOLOR(m_col.r, m_col.g, m_col.b, m_fAlpha);
			pVtx[1].col = D3DXCOLOR(m_col.r, m_col.g, m_col.b, m_fAlpha);
			pVtx[2].col = D3DXCOLOR(m_col.r, m_col.g, m_col.b, m_fAlpha);
			pVtx[3].col = D3DXCOLOR(m_col.r, m_col.g, m_col.b, m_fAlpha);

			// --- 頂点バッファをアンロックする --- //
			m_pVtxBuff->Unlock();

			//if (m_fAlpha <= 0.0f)
			//{
			//	Uninit();
			//}

			return m_fAlpha;
		}
	}

	return 0.0f;
}

// =================================================
// ポリゴンの拡大処理
// =================================================
void CObject2D::Expand(float fSpan, float fValue)
{
	VERTEX_2D* pVtx;		// 頂点情報へのポインタ

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	m_fExpand += fSpan;

	if (m_fExpand > D3DX_PI)
	{
		m_fExpand -= D3DX_PI * 2;
	}

	m_fLength = m_fLength + sinf(m_fExpand) * fValue;

	// --- 頂点カラーの設定 --- //
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

