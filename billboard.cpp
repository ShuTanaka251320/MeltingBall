// =================================================
// 
// ビルボード処理[billboard.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "billboard.h"
#include "manager.h"
#include "macro.h"
#include "texture.h"
#include "renderer.h"

// =================================================
// コンストラクタ
// =================================================
CBillboard::CBillboard(int nPriority) :CObject(nPriority)
{
	memset(m_mtxWorld, 0, sizeof(m_mtxWorld));	// ワールドマトリックス
	m_pTexture = NULL;	// テクスチャへのポインタ
	m_pVtxBuff = NULL;	// 頂点バッファへのポインタ
	m_pos = VEC_NULL;	// 位置
	m_rot = VEC_NULL;	// 向き
	m_move = VEC_NULL;	// 移動量
	m_col = COLOR_WHITE;	// 色
	m_Alpha = 0.0f;		// 透明度
	m_fLength = 0.0f;	// 対角線の長さ
	m_fAngle = 0.0f;	// 対角線の角度
	m_fWidth = 0.0f;	// 横幅
	m_fHeight = 0.0f;	// 高さ
	m_bDisp = false;	// 表示するかどうか
	m_nIdxTex = 0;	// テクスチャインデックス
}

// =================================================
// デストラクタ
// =================================================
CBillboard::~CBillboard()
{

}

// =================================================
// ビルボードの生成処理
// =================================================
CBillboard* CBillboard::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, CObject::TYPE type, int nPri, const char* pFileName)
{
	CBillboard* pBillboard;

	pBillboard = new CBillboard(nPri);

	pBillboard->Init(pos, rot, move, col, fWidth, fHeight, type, pFileName);

	return pBillboard;
}

// =================================================
// ビルボードの初期化処理
// =================================================
HRESULT CBillboard::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, CObject::TYPE type, const char* pFileName)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	// --- 頂点バッファの生成 --- //
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// --- メンバ変数に代入 --- //
	m_pos = pos;	// 位置
	m_rot = rot;	// 向き
	m_move = move;	// 移動量
	m_col = col;	// 色
	m_fLength = sqrtf(fWidth * fWidth + fHeight * fHeight) * 0.5f;	// 対角線の長さ
	m_fAngle = (float)atan2(fWidth, fHeight);						// 対角線の角度
	m_fWidth = fWidth;		// 横幅
	m_fHeight = fHeight;	// 高さ
	m_bDisp = true;	// 表示フラグ
	SetType(type);	// タイプ設定

	VERTEX_3D* pVtx;		// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-fWidth * 0.5f, fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(fWidth * 0.5f, fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-fWidth * 0.5f, 0.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(fWidth * 0.5f, 0.0f, 0.0f);

	// 法線ベクトルの設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, -0.5f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, -0.5f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, -0.5f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, -0.5f);

	// 頂点カラーの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	return S_OK;
}

// =================================================
// ビルボードの終了処理
// =================================================
void CBillboard::Uninit(void)
{
	// テクスチャの破棄
	if (m_pTexture != NULL)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}
	// 頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	Release();
}

// =================================================
// ビルボードの更新処理
// =================================================
void CBillboard::Update(void)
{

}

// =================================================
// ビルボードの描画処理
// =================================================
void CBillboard::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	D3DXMATRIX mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxView;		// ビューマトリックス取得用

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ビューマトリックスの取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// ポリゴンをカメラに対して正面に向ける
	D3DXMatrixInverse(&m_mtxWorld, NULL, &mtxView);	// 逆行列を求める
	m_mtxWorld._41 = 0.0f;		// 位置情報の初期化
	m_mtxWorld._42 = 0.0f;		// 位置情報の初期化
	m_mtxWorld._43 = 0.0f;		// 位置情報の初期化

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans,
		m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);	// かけ合わせる

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTex));

	// ビルボードの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

// =================================================
// 寿命による透明度の変化
// =================================================
void CBillboard::AlphaDecrease(int nMaxLife, int nCurrentLife)
{
	VERTEX_3D* pVtx;		// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	m_col.a = nCurrentLife / nMaxLife;	// 透明度設定

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(m_col.r, m_col.g, m_col.b, m_col.a);
	pVtx[1].col = D3DXCOLOR(m_col.r, m_col.g, m_col.b, m_col.a);
	pVtx[2].col = D3DXCOLOR(m_col.r, m_col.g, m_col.b, m_col.a);
	pVtx[3].col = D3DXCOLOR(m_col.r, m_col.g, m_col.b, m_col.a);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

