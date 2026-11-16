// =================================================
// 
// 3Dオブジェクト処理[object3D.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include"object3D.h"
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
CObject3D::CObject3D(int nPriority) :CObject(nPriority)
{
	// --- 各種変数のクリア --- //
	m_pTexture = NULL;
	m_pVtxBuff = NULL;	// 頂点バッファへのポインタ
	m_pos = VEC_NULL;	// 位置
	m_rot = VEC_NULL;	// 向き
	m_fLength = 0.0f;	// 対角線の長さ
	m_fAngle = 0.0f;	// 対角線の角度
	m_nIdxTex = -1;		// テクスチャのインデックス
	m_fPolHeight = 0.0f;
}

// =================================================
// デストラクタ
// =================================================
CObject3D::~CObject3D()
{

}

// =================================================
// ポリゴンの初期化処理
// =================================================
HRESULT CObject3D::Init(void)
{
	return S_OK;
}

// =================================================
// ポリゴンの初期化処理(オーバーロード)
// =================================================
HRESULT CObject3D::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, CObject::TYPE type, const char* pFileName)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9	pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	// --- 頂点バッファの生成 --- //
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	if (m_pVtxBuff == NULL)
	{// バッファ読み込み失敗
		return E_FAIL;
	}

	VERTEX_3D* pVtx;		// 頂点情報へのポインタ

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
	pVtx[0].pos = D3DXVECTOR3(- m_fWidth, 0.0f, m_fHeight);
	pVtx[1].pos = D3DXVECTOR3( m_fWidth, 0.0f, m_fHeight);
	pVtx[2].pos = D3DXVECTOR3(- m_fWidth, 0.0f, - m_fHeight);
	pVtx[3].pos = D3DXVECTOR3( m_fWidth, 0.0f, - m_fHeight);

	// --- norの設定 --- //
	//pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	//pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	//pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	//pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

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
void CObject3D::Uninit()
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
void CObject3D::Update(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得

	VERTEX_3D* pVtx;		// 頂点情報へのポインタ

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// --- 頂点座標の設定 --- //
	pVtx[0].pos.x = sinf(m_rot.y + m_fAngle + D3DX_PI) * m_fLength;
	pVtx[0].pos.y = 0.0f;
	pVtx[0].pos.z = cosf(m_rot.y + m_fAngle + D3DX_PI) * m_fLength;

	pVtx[1].pos.x = sinf(m_rot.y - m_fAngle) * m_fLength;
	pVtx[1].pos.y = 0.0f ;
	pVtx[1].pos.z = cosf(m_rot.y - m_fAngle) * m_fLength;

	pVtx[2].pos.x = sinf(m_rot.y + D3DX_PI - m_fAngle) * m_fLength;
	pVtx[2].pos.y = 0.0f ;
	pVtx[2].pos.z = cosf(m_rot.y + D3DX_PI - m_fAngle) * m_fLength;

	pVtx[3].pos.x = sinf(m_rot.y + m_fAngle) * m_fLength;
	pVtx[3].pos.y = 0.0f;
	pVtx[3].pos.z = cosf(m_rot.y + m_fAngle) * m_fLength;

	// --- 頂点バッファをアンロックする --- //
	m_pVtxBuff->Unlock();
}

// =================================================
// ポリゴンの描画処理
// =================================================
void CObject3D::Draw(void)
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
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// --- 頂点フォーマットの設定 --- //
	pDevice->SetFVF(FVF_VERTEX_3D);

	// --- テクスチャの設定 --- //
	if (m_nIdxTex > -1)
	{
		pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTex));
	}
	else
	{
		pDevice->SetTexture(0, NULL);
	}

	//pDevice->SetTexture(0, pTexMT);

	// --- ポリゴンの描画 --- //
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

// =================================================
// ポリゴンの生成
// =================================================
CObject3D* CObject3D::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, CObject::TYPE type, const char* pFileName)
{
	CObject3D* pObject3D;	// ポインタ宣言

	// --- オブジェクトの生成 --- //
	pObject3D = new CObject3D;

	// --- 初期化 --- //
	pObject3D->Init(pos, rot, move, col, fWidth, fHeight, type, pFileName);

	return pObject3D;
}

// =================================================
// ポリゴンの生成
// =================================================
void CObject3D::SetCol(D3DXCOLOR col)
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

// =================================================
// 高さを返す
// =================================================
//float CObject3D::GetPosHeight(D3DXVECTOR3 pPos, D3DXVECTOR3 pPosOld)
//{
//	CDebugProc* pDebugProc = CManager::GetDrbugProc();				// デバッグ表示のポインタ
//
//	VERTEX_3D* pVtx;		// 頂点情報へのポインタ
//
//	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
//	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);
//
//	// --- それぞれの頂点からベクトルを算出 --- //
//	D3DXVECTOR3 VecLine1 = pVtx[1].pos - pVtx[0].pos;
//	D3DXVECTOR3 VecLine2 = pVtx[2].pos - pVtx[1].pos;
//	D3DXVECTOR3 VecLine3 = pVtx[0].pos - pVtx[2].pos;
//
//	// --- 正規化 --- //
//	D3DXVec3Normalize(&VecLine1, &VecLine1);
//	D3DXVec3Normalize(&VecLine2, &VecLine2);
//	D3DXVec3Normalize(&VecLine3, &VecLine3);
//
//	// --- それぞれの3頂点からプレイヤーの位置までのベクトルを算出 --- //
//	D3DXVECTOR3 VecPos1 = pPos - pVtx[0].pos;
//	D3DXVECTOR3 VecPos2 = pPos - pVtx[1].pos;
//	D3DXVECTOR3 VecPos3 = pPos - pVtx[2].pos;
//
//	// --- 正規化 --- //
//	D3DXVec3Normalize(&VecPos1, &VecPos1);
//	D3DXVec3Normalize(&VecPos2, &VecPos2);
//	D3DXVec3Normalize(&VecPos3, &VecPos3);
//
//	// --- プレイヤーがポリゴンの内側にいるかの判定用 --- //
//	float OnPolygon1 = (VecLine1.z * VecPos1.x) - (VecLine1.x * VecPos1.z);
//	float OnPolygon2 = (VecLine2.z * VecPos2.x) - (VecLine2.x * VecPos2.z);
//	float OnPolygon3 = (VecLine3.z * VecPos3.x) - (VecLine3.x * VecPos3.z);
//
//	// --- デバッグ表示 --- //
//	pDebugProc->PrintDebug("%f %f %f\n", OnPolygon1, OnPolygon2, OnPolygon3);
//
//	// --- ポリゴンの内側にいるとき --- //
//	if (OnPolygon1 > 0.0f && OnPolygon2 > 0.0f && OnPolygon3 > 0.0f)
//	{
//		D3DXVECTOR3 nor, Vec1, Vec2;
//
//		// --- 法線を求める処理 --- // 
//		Vec1 = pVtx[1].pos - pVtx[0].pos;
//		Vec2 = pVtx[2].pos - pVtx[0].pos;
//
//		// --- 外積を算出して法線の決定 --- //
//		D3DXVec3Cross(&nor, &Vec1, &Vec2);
//
//		// --- 正規化 --- //
//		D3DXVec3Normalize(&nor, &nor);
//
//		if (nor.y != 0)
//		{
//			m_fPolHeight = -(((pPos.x - pVtx[1].pos.x) * nor.x) + (-pVtx[0].pos.y * nor.y) + ((pPos.z - pVtx[0].pos.z) * nor.z)) / nor.y;
//
//			// --- 頂点バッファをアンロックする --- //
//			m_pVtxBuff->Unlock();
//
//			return m_fPolHeight;
//		}
//	}
//
//	// --- 2枚目のポリゴン --- //
//	// --- それぞれの頂点から頂点へのベクトルを算出 --- //
//	VecLine1 = pVtx[1].pos - pVtx[2].pos;
//	VecLine2 = pVtx[2].pos - pVtx[3].pos;
//	VecLine3 = pVtx[3].pos - pVtx[1].pos;
//
//	// --- 正規化 --- //
//	D3DXVec3Normalize(&VecLine1, &VecLine1);
//	D3DXVec3Normalize(&VecLine2, &VecLine2);
//	D3DXVec3Normalize(&VecLine3, &VecLine3);
//
//	// --- それぞれの3頂点からプレイヤーの位置までのベクトルを算出 --- //
//	VecPos1 = pPos - pVtx[1].pos;
//	VecPos2 = pPos - pVtx[2].pos;
//	VecPos3 = pPos - pVtx[3].pos;
//
//	// --- 正規化 --- //
//	D3DXVec3Normalize(&VecPos1, &VecPos1);
//	D3DXVec3Normalize(&VecPos2, &VecPos2);
//	D3DXVec3Normalize(&VecPos3, &VecPos3);
//
//	// --- プレイヤーがポリゴンの内側にいるかの判定用 --- //
//	OnPolygon1 = (VecLine1.z * VecPos1.x) - (VecLine1.x * VecPos1.z);
//	OnPolygon2 = (VecLine2.z * VecPos2.x) - (VecLine2.x * VecPos2.z);
//	OnPolygon3 = (VecLine3.z * VecPos3.x) - (VecLine3.x * VecPos3.z);
//
//	// --- デバッグ表示 --- //
//	pDebugProc->PrintDebug("%f %f %f\n", OnPolygon1, OnPolygon2, OnPolygon3);
//
//	// --- ポリゴンの内側にいるとき --- //
//	if (OnPolygon1 > 0.0f && OnPolygon2 > 0.0f && OnPolygon3 > 0.0f)
//	{
//		D3DXVECTOR3 nor, Vec1, Vec2;
//
//		// --- 法線を求める処理 --- // 
//		Vec1 = pVtx[1].pos - pVtx[3].pos;
//		Vec2 = pVtx[2].pos - pVtx[3].pos;
//
//		// --- 外積を算出して法線の決定 --- //
//		D3DXVec3Cross(&nor, &Vec2, &Vec1);
//
//		// --- 正規化 --- //
//		D3DXVec3Normalize(&nor, &nor);
//
//		if (nor.y != 0)
//		{
//			// --- ポリゴンの高さを算出 --- //
//			m_fPolHeight = -(((pPos.x - pVtx[3].pos.x) * nor.x) + (-pVtx[3].pos.y * nor.y) + ((pPos.z - pVtx[3].pos.z) * nor.z)) / nor.y;	// m_fPolHeight = pPos.y
//
//			// --- 頂点バッファをアンロックする --- //
//			m_pVtxBuff->Unlock();
//
//			return m_fPolHeight;
//		}
//	}
//
//	// --- 頂点バッファをアンロックする --- //
//	m_pVtxBuff->Unlock();
//
//	return 0.0f;
//}
