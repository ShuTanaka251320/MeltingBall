// =================================================
// 
// 3Dオブジェクト(マルチテクスチャ)処理[object3DMultiTex.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include"object3Dmultitex.h"
#include"renderer.h"
#include"manager.h"
#include"texture.h"
#include"macro.h"
#include"player.h"
#include"input.h"
#include"debugproc.h"

// =================================================
// コンストラクタ
// =================================================
CObject3DMultiTex::CObject3DMultiTex(int nPriority) :CObject(nPriority)
{
	// --- 各種変数のクリア --- //
	m_pTexture = NULL;
	m_pVtxBuff = NULL;	// 頂点バッファへのポインタ
	m_pos = VEC_NULL;	// 位置
	m_rot = VEC_NULL;	// 向き
	m_fLength = 0.0f;	// 対角線の長さ
	m_fAngle = 0.0f;	// 対角線の角度
	m_fPolHeight = 0.0f;

	for (int nCnt = 0; nCnt < 10; nCnt++)
	{
		m_nIdxTex[nCnt] = -1;
	}
}

// =================================================
// デストラクタ
// =================================================
CObject3DMultiTex::~CObject3DMultiTex()
{

}

// =================================================
// ポリゴンの初期化処理
// =================================================
HRESULT CObject3DMultiTex::Init(void)
{
	return S_OK;
}

// =================================================
// ポリゴンの初期化処理(オーバーロード)
// =================================================
HRESULT CObject3DMultiTex::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, CObject::TYPE type, const char* pFileName[10])
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9	pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	// --- 頂点バッファの生成 --- //
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3DMULTI) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	if (m_pVtxBuff == NULL)
	{// バッファ読み込み失敗
		return E_FAIL;
	}

	VERTEX_3DMULTI* pVtx;		// 頂点情報へのポインタ

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// --- 各種メンバ変数の初期化 --- //
	m_pos = pos;
	m_move = move;
	m_rot = rot;
	m_fLength = sqrtf(fWidth * fWidth + fHeight * fHeight) * 0.5f;	// 対角線の長さ
	m_fAngle = (float)atan2(fWidth, fHeight);						// 対角線の角度
	m_fWidth = fWidth;		// 横幅
	m_fHeight = fHeight;	// 高さ
	SetType(type);

	// --- 頂点座標の設定 --- //
	pVtx[0].pos = D3DXVECTOR3(-m_fWidth, 0.0f, m_fHeight);
	pVtx[1].pos = D3DXVECTOR3(m_fWidth, 0.0f, m_fHeight);
	pVtx[2].pos = D3DXVECTOR3(-m_fWidth, 0.0f, -m_fHeight);
	pVtx[3].pos = D3DXVECTOR3(m_fWidth, 0.0f, -m_fHeight);

	// --- norの設定 --- //
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// --- 法線ベクトル --- //
	D3DXVECTOR3 VecA1 = pVtx[1].pos - pVtx[0].pos;
	D3DXVECTOR3 VecA2 = pVtx[2].pos - pVtx[0].pos;

	D3DXVECTOR3 VecB1 = pVtx[1].pos - pVtx[3].pos;
	D3DXVECTOR3 VecB2 = pVtx[2].pos - pVtx[3].pos;

	D3DXVECTOR3 norA = {};
	D3DXVECTOR3 norB = {};

	// --- 外積を算出 --- //
	D3DXVec3Cross(&norA, &VecA1, &VecA2);
	D3DXVec3Cross(&norB, &VecB2, &VecB1);	// TRIANGLESTRIPの場合は、三角形が逆回りのため、ベクトルを反対で算出

	// --- 正規化 --- //
	D3DXVec3Normalize(&norA, &norA);
	D3DXVec3Normalize(&norB, &norB);

	pVtx[0].nor = norA;
	pVtx[1].nor = (norA + norB) / 2;
	pVtx[2].nor = (norA + norB) / 2;
	pVtx[3].nor = norB;

	// --- 頂点カラーの設定 --- //
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// --- テクスチャ座標の設定 --- //
	pVtx[0].tex0 = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex0 = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex0 = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex0 = D3DXVECTOR2(1.0f, 1.0f);

	pVtx[0].tex1 = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex1 = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex1 = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex1 = D3DXVECTOR2(1.0f, 1.0f);

	// --- 頂点バッファをアンロックする --- //
	m_pVtxBuff->Unlock();

	return S_OK;
}

// =================================================
// ポリゴンの終了処理
// =================================================
void CObject3DMultiTex::Uninit()
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
void CObject3DMultiTex::Update(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得

	VERTEX_3DMULTI* pVtx;		// 頂点情報へのポインタ

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// --- 頂点座標の設定 --- //
	pVtx[0].pos.x = sinf(m_rot.y + m_fAngle + D3DX_PI) * m_fLength;
	pVtx[0].pos.y = 0.0f;
	pVtx[0].pos.z = cosf(m_rot.y + m_fAngle + D3DX_PI) * m_fLength;

	pVtx[1].pos.x = sinf(m_rot.y - m_fAngle) * m_fLength;
	pVtx[1].pos.y = 0.0f;
	pVtx[1].pos.z = cosf(m_rot.y - m_fAngle) * m_fLength;

	pVtx[2].pos.x = sinf(m_rot.y + D3DX_PI - m_fAngle) * m_fLength;
	pVtx[2].pos.y = 0.0f;
	pVtx[2].pos.z = cosf(m_rot.y + D3DX_PI - m_fAngle) * m_fLength;

	pVtx[3].pos.x = sinf(m_rot.y + m_fAngle) * m_fLength;
	pVtx[3].pos.y = 0.0f;
	pVtx[3].pos.z = cosf(m_rot.y + m_fAngle) * m_fLength;

	//static float fMove0 = 0.0f;
	//static float fMove1 = 1.0f;

	//fMove0 += 0.01f;
	//fMove1 += 0.01f;

	//pVtx[0].tex0 = D3DXVECTOR2(fMove0, 0.0f);
	//pVtx[1].tex0 = D3DXVECTOR2(fMove1, 0.0f);
	//pVtx[2].tex0 = D3DXVECTOR2(fMove0, 1.0f);
	//pVtx[3].tex0 = D3DXVECTOR2(fMove1, 1.0f);

	static float fMove2 = 0.0f;
	static float fMove3 = 1.0f;

	fMove2 -= 0.01f;
	fMove3 -= 0.01f;

	pVtx[0].tex1 = D3DXVECTOR2(fMove2, 0.0f);
	pVtx[1].tex1 = D3DXVECTOR2(fMove3, 0.0f);
	pVtx[2].tex1 = D3DXVECTOR2(fMove2, 1.0f);
	pVtx[3].tex1 = D3DXVECTOR2(fMove3, 1.0f);

	// --- 頂点バッファをアンロックする --- //
	m_pVtxBuff->Unlock();
}

// =================================================
// ポリゴンの描画処理
// =================================================
void CObject3DMultiTex::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	//LPDIRECT3DTEXTURE9 pTexMT;	// マルチターゲットテクスチャ

	//// --- テクスチャの取得 --- //
	//pTexMT = CManager::GetRenderer()->GetTextureMT();

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);	// かけ合わせる

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans,
		m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);	// かけ合わせる

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// --- 頂点バッファをデータストリームに設定 --- //
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3DMULTI));

	// --- 頂点フォーマットの設定 --- //
	pDevice->SetFVF(FVF_VERTEX_3DMULTI);

	// --- テクスチャの設定 --- //
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTex[0]));

	// --- 型用のテクスチャのステージステートの設定 --- //
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);

	// --- メインテクスチャの設定 --- //
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pDevice->SetTexture(1, pTexture->GetAddress(m_nIdxTex[1]));

	// --- ポリゴンの描画 --- //
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// --- ステージステートを初期化 --- //
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
}

// =================================================
// ポリゴンの生成
// =================================================
CObject3DMultiTex* CObject3DMultiTex::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, CObject::TYPE type, const char* pFileName[10])
{
	CObject3DMultiTex* pObject3DMulti;	// ポインタ宣言

	// --- オブジェクトの生成 --- //
	pObject3DMulti = new CObject3DMultiTex;

	// --- 初期化 --- //
	pObject3DMulti->Init(pos, rot, move, col, fWidth, fHeight, type, pFileName);

	return pObject3DMulti;
}

// =================================================
// ポリゴンの色設定
// =================================================
void CObject3DMultiTex::SetCol(D3DXCOLOR col)
{
	VERTEX_3D* pVtx;		// 頂点情報へのポインタ

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