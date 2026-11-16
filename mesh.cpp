// =================================================
// 
// メッシュ関連処理[mesh.h]
// Author : Shu Tanaka
// 
// =================================================
#include"mesh.h"
#include"manager.h"
#include"renderer.h"
#include"debugproc.h"
#include"macro.h"
#include"texture.h"
#include"input.h"
#include"meshfield.h"
#include"player.h"
#include"game.h"

#include<cmath>
#include<vector>
#include<iostream>
#include<algorithm>

// =================================================
// マクロ定義
#define COLLSION_FILE	"data\\Scripts\\collisionSave.txt"		// 当たり判定保存ファイル
#define COLLIDER_FILE	"data\\Scripts\\collisionLoad.txt"		// 当たり判定読み込みファイル
#define STAIR_COLLIDER	"data\\Scripts\\collider\\collisionStair.txt"		// 階段の当たり判定ファイル
#define HANDRAIL_COLLIDER	"data\\Scripts\\collider\\collisionHandRail.txt"	// 手すりの当たり判定ファイル

// =================================================
// コンストラクタ
// =================================================
CMesh::CMesh(int nPriority) :CObject(nPriority)
{
	m_pTexture = NULL;	// テクスチャのポインタ
	m_pVtxBuff = NULL;	// 頂点バッファのポインタ	
	m_pIdxBuff = NULL;	// インデックスバッファへのポインタ
	m_mtxWorld = {};	// ワールドマトリックス
	m_nIdxTex = 0;;	// テクスチャのインデックス

	m_nNumVtx = 0;	// 頂点数
	m_nNumIdx = 0;	// インデックス数
	m_nNumPri = 0;	// プリミティブ数

	m_nNumDevideX = 0;	// Xの分割数
	m_nNumDevideY = 0;	// Yの分割数
	m_nNumDevideZ = 0;	// Zの分割数
	m_nNumDeiveSurcle = 0;	// リングの分割数

	m_pos = VEC_NULL;	// 位置
	m_rot = VEC_NULL;	// 向き
	m_fLength = 0.0f;	// ポリゴンの対角線の長さ
	m_fAngle = 0.0f;	// ポリゴンの対角線の角度
	m_fSizeX = 0.0f;	// 横幅
	m_fSizeZ = 0.0f;	// 縦幅
	m_fMaxSizeX = 0.0f;	// 最大横幅
	m_fMaxSizeZ = 0.0f;	// 最大縦幅
	m_fHeight = 0.0f;	// 高さ
	m_col = COLOR_WHITE;	// 色

	m_type = TYPE_FIELD;	// タイプ
	m_bDrawOutSide = true;
	m_bDraw = true;	// 描画フラグ

}

// =================================================
// デストラクタ
// =================================================
CMesh::~CMesh()
{

}

// =================================================
// メッシュの初期化処理
// =================================================
HRESULT CMesh::Init(void)
{
	return S_OK;
}

// =================================================
// メッシュの初期化処理(meshfield)
// =================================================
HRESULT CMesh::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSizeX, float fSizeZ, float fMaxSizeX, float fMaxSizeZ,
	int nDevideX, int nDevideZ, int nNumVtx, int nNumIdx, CMesh::TYPE type, bool bDraw)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	// --- メンバ変数に代入 --- //
	m_pos = pos;
	m_rot = rot;
	m_fSizeX = fSizeX;
	m_fSizeZ = fSizeZ;
	m_fMaxSizeX = fMaxSizeX;
	m_fMaxSizeZ = fMaxSizeZ;
	m_type = type;
	m_fHeight = pos.y;

	m_fLength = sqrtf(fMaxSizeX * fMaxSizeX + fMaxSizeZ * fMaxSizeZ) * 0.5f;	// 対角線の長さ
	m_fAngle = (float)atan2(fMaxSizeX, fMaxSizeZ);								// 対角線の角度

	// --- 頂点関連 --- //
	m_nNumIdx = nNumIdx;
	m_nNumPri = nNumIdx - 2;
	m_nNumVtx = nNumVtx;

	// --- 分割数 --- //
	m_nNumDevideX = nDevideX;
	m_nNumDevideZ = nDevideZ;

	m_bDraw = bDraw;	// 描画の判断
	SetType(TYPE_MESH);

	// --- 頂点バッファの生成 --- //
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * nNumVtx,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_3D* pVtx;		// 頂点情報へのポインタ

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// --- 起伏の生成用 --- //
	float sinCurve = 0.0f;
	float posY = 0.0f;

	// --- 頂点座標の設定 --- //
	for (int nCntLine = 0; nCntLine < nDevideZ + 1; nCntLine++)
	{
		for (int nCntRow = 0; nCntRow < nDevideX + 1; nCntRow++)
		{
			pVtx[0].pos = D3DXVECTOR3((-fMaxSizeX + (float)nCntRow * fSizeX) + (fMaxSizeX * 0.5f),
				0.0f,
				(fMaxSizeZ - (float)nCntLine * fSizeZ) - (fMaxSizeZ * 0.5f));

			pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
			pVtx[0].tex = D3DXVECTOR2((float)nCntRow, (float)nCntLine);

			pVtx++;
		}
	}

	// --- 頂点バッファをアンロックする --- //
	m_pVtxBuff->Unlock();

	// --- 各頂点の法線の設定 --- //
	SetNor();

	// --- インデックスバッファの生成 --- //
	pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIdx,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	WORD* pIdx;	// インデックス情報へのポインタ

	// --- インデックスバッファをロックし、頂点番号データのポインタを取得 --- //
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	WORD nIdx = 0;	// 頂点番号保存

	// --- 頂点番号データの設定 --- //
	for (int nCntLine = 0; nCntLine < nDevideZ; nCntLine++)
	{// Z軸
		for (int nCntRow = 0; nCntRow < nDevideX + 1; nCntRow++)
		{// X軸	
			pIdx[0] = (nDevideX + 1) + nCntRow + (nCntLine * (nDevideX + 1));
			pIdx[1] = nCntRow + (nCntLine * (nDevideX + 1));
			nIdx = pIdx[1];	// 別の変数に保存

			pIdx += 2;
		}

		if (nCntLine != nDevideZ - 1)
		{// 端の処理
			pIdx[0] = nIdx;		// 重ねる
			pIdx[1] = nIdx + nDevideX + 2;

			pIdx += 2;
		}
	}

	// --- インデックスバッファをアンロック --- //
	m_pIdxBuff->Unlock();

	return S_OK;
}

// =================================================
// メッシュの初期化処理(meshring)
// =================================================
HRESULT CMesh::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSizeInside, float fSizeOutside, int nDevide, int nNumVtx, int nNumIdx, CMesh::TYPE type, bool bDraw)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	// --- メンバ変数に代入 --- //
	m_pos = pos;
	m_rot = rot;
	m_type = type;
	m_fHeight = pos.y;
	m_fInRadius = fSizeInside;
	m_fOutRadius = fSizeOutside;

	// --- 頂点関連 --- //
	m_nNumIdx = nNumIdx;
	m_nNumPri = nNumIdx - 2;
	m_nNumVtx = nNumVtx;

	// --- 分割数 --- //
	m_nNumDevideX = nDevide;
	m_nNumDevideZ = nDevide;

	m_nNumDeiveSurcle = nDevide;

	m_bDraw = bDraw;	// 描画の判断
	SetType(TYPE_MESH);

	// --- 頂点バッファの生成 --- //
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * nNumVtx,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_3D* pVtx;		// 頂点情報へのポインタ

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// --- 起伏の生成用 --- //
	float sinCurve = 0.0f;
	float posY = 0.0f;

	m_nNumPri = nNumVtx;

	// --- 頂点座標の設定 --- //
	for (int nCnt = 0; nCnt < nDevide; nCnt++)
	{// 外側頂点の設定
		float fAngle = 2.0f * D3DX_PI * nCnt / nDevide;	// 中心(m_pos)空の角度

		float c = cosf(fAngle);
		float s = sinf(fAngle);

		pVtx[0].pos = { s * fSizeOutside, 0.0f,  c * fSizeOutside };
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[0].tex = D3DXVECTOR2(0.0, 0.0f);

		pVtx++;
	}

	// --- 頂点バッファをアンロックする --- //
	m_pVtxBuff->Unlock();

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < nDevide; nCnt++)
	{// 内側頂点の設定
		float fAngle = 2.0f * D3DX_PI * nCnt / nDevide;	// 中心(m_pos)空の角度

		float c = cos(fAngle);
		float s = sin(fAngle);

		pVtx[nDevide].pos = { s * fSizeInside,0.0f,  c * fSizeInside };
		pVtx[nDevide].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[nDevide].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[nDevide].tex = D3DXVECTOR2(0.0, 0.0f);

		pVtx++;
	}

	// --- 頂点バッファをアンロックする --- //
	m_pVtxBuff->Unlock();

	// --- インデックスバッファの生成 --- //
	pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIdx,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	WORD* pIdx;	// インデックス情報へのポインタ

	// --- インデックスバッファをロックし、頂点番号データのポインタを取得 --- //
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	WORD nIdx = 0;	// 頂点番号保存

	// --- 頂点番号データの設定 --- //
	for (int nCnt = 0; nCnt < nDevide + 1; nCnt++)
	{
		pIdx[0] = nDevide + nCnt;
		pIdx[1] = nCnt;
		nIdx = pIdx[1];	// 別の変数に保存

		if (nCnt == nDevide)
		{
			pIdx[0] = nDevide;
			pIdx[1] = 0;
		}

		pIdx += 2;
	}

	// --- インデックスバッファをアンロック --- //
	m_pIdxBuff->Unlock();


	return S_OK;
}


// =================================================
// メッシュの初期化処理(meshcylinder)
// =================================================
HRESULT CMesh::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fRadius, float fHight, int nDevideSurcle, int nDevideHeight, int nNumVtx, int nNumIdx, CMesh::TYPE type, bool bDraw, bool bDrawOutSide)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	// --- メンバ変数に代入 --- //
	m_pos = pos;
	m_rot = rot;
	m_type = type;
	m_fHeight = pos.y;
	m_fRadius = fRadius;	// 半径
	m_fSizeY = fHight;		// 高さ

	// --- 頂点関連 --- //
	m_nNumIdx = nNumIdx;
	m_nNumVtx = nNumVtx;
	m_nNumPri = (((nDevideSurcle) * 2) * nDevideHeight + (2 * nDevideHeight));

	// --- 分割数 --- //
	m_nNumDevideX = nDevideSurcle;
	m_nNumDevideY = nDevideHeight;

	m_bDrawOutSide = bDrawOutSide;
	m_bDraw = bDraw;	// 描画の判断
	SetType(TYPE_MESH);

	// --- 頂点バッファの生成 --- //
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * nNumVtx,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_3D* pVtx;		// 頂点情報へのポインタ

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// --- 起伏の生成用 --- //
	float sinCurve = 0.0f;
	float posY = 0.0f;

	D3DXVECTOR3 Nor;	// 法線ベクトル

	int nIdxVtx = 0;	// 頂点インデックス

	// --- 頂点座標の設定 --- //
	for (int nCnt = 0; nCnt < nDevideSurcle; nCnt++)
	{//底辺頂点の設定
		float fAngle = 2.0f * D3DX_PI * nCnt / nDevideSurcle;	// 中心(m_pos)からの角度

		float c = cosf(fAngle);
		float s = sinf(fAngle);

		pVtx[nIdxVtx].pos = { s * fRadius, 0.0f,  c * fRadius };
		pVtx[nIdxVtx].nor = { s, 0.0f, c };
		pVtx[nIdxVtx].col = COLOR_WHITE;
		pVtx[nIdxVtx].tex = D3DXVECTOR2(nIdxVtx % 2, 1.0f);

		nIdxVtx++;
	}

	for (int nCntHeight = 0; nCntHeight < nDevideHeight; nCntHeight++)
	{
		for (int nCnt = 0; nCnt < nDevideSurcle; nCnt++)
		{// 内側頂点の設定
			float fAngle = 2.0f * D3DX_PI * nCnt / nDevideSurcle;	// 中心(m_pos)からの角度

			float c = cos(fAngle);
			float s = sin(fAngle);

			pVtx[nIdxVtx].pos = { s * fRadius, (fHight / nDevideHeight) * (nCntHeight + 1), c * fRadius };
			pVtx[nIdxVtx].nor = { s, 0.0f, c };
			pVtx[nIdxVtx].col = COLOR_WHITE;
			pVtx[nIdxVtx].tex = D3DXVECTOR2(nIdxVtx % 2, 0.0f - nCntHeight);

			nIdxVtx++;
		}
	}

	// --- 頂点バッファをアンロックする --- //
	m_pVtxBuff->Unlock();

	// --- インデックスバッファの生成 --- //
	pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIdx,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	WORD* pIdx;	// インデックス情報へのポインタ

	// --- インデックスバッファをロックし、頂点番号データのポインタを取得 --- //
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	WORD Idx = 0;	// 頂点番号保存
	int nIdx = 0;	// インデックス番号
	int aData[512];

	// --- 頂点番号データの設定 --- //
	for (int nCntHeight = 0; nCntHeight < nDevideHeight; nCntHeight++)
	{
		for (int nCnt = 0; nCnt < nDevideSurcle + 1; nCnt++)
		{
			pIdx[nIdx] = nDevideSurcle + nCnt + (nDevideSurcle * nCntHeight);
			pIdx[nIdx + 1] = nCnt + (nDevideSurcle * nCntHeight);
			Idx = pIdx[1];	// 別の変数に保存
			aData[nCnt] = pIdx[nIdx];

			if (nCnt == nDevideSurcle)
			{
				pIdx[nIdx] = nDevideSurcle + (nDevideSurcle * nCntHeight);
				pIdx[nIdx + 1] = (nDevideSurcle * nCntHeight);
			}

			nIdx += 2;
		}
	}

	// --- インデックスバッファをアンロック --- //
	m_pIdxBuff->Unlock();

	return S_OK;
}

// =================================================
// メッシュの終了処理
// =================================================
void CMesh::Uninit(void)
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

	// インデックスバッファの破棄
	if (m_pIdxBuff != NULL)
	{
		m_pIdxBuff->Release();
		m_pIdxBuff = NULL;
	}

	Release();
}

// =================================================
// メッシュの更新処理
// =================================================
void CMesh::Update()
{
	if (m_type == TYPE_FIELD)
	{
		// --- 各頂点の法線の設定 --- //
		SetNor();
	}
}

// =================================================
// メッシュの描画処理
// =================================================
void CMesh::Draw(void)
{
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

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

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	if (m_nIdxTex > -1)
	{
		pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTex));
	}
	else
	{
		pDevice->SetTexture(0, NULL);
	}

	if (m_bDraw == true) {
		// ポリゴンの描画
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
			0,
			0,
			m_nNumVtx,	// 頂点数
			0,
			m_nNumPri);	// プリミティブ数
	}
}

// =================================================
// メッシュポリゴンの高さを渡す
// =================================================
float CMesh::GetPosHeight(D3DXVECTOR3 pPos, D3DXVECTOR3 pPosOld)
{
	D3DXVECTOR3 PlayerPos = {};

	// --- 現在のプレイヤーの位置を、メッシュフィールドの左上を基準で算出 --- //
	PlayerPos.x = pPos.x + (m_fSizeX * m_nNumDevideX) / 2;
	PlayerPos.z = pPos.z - (m_fSizeZ * m_nNumDevideZ) / 2;

	PlayerPos.z *= -1;	// 負の符号の除去

	if (m_pVtxBuff != NULL)
	{
		// --- プレイヤーの位置で判定継続かを決める --- //
		if (pPos.x > m_pos.x - (m_fMaxSizeX / 2) && pPos.x < m_pos.x + (m_fMaxSizeX / 2)
			&& pPos.z > m_pos.z - (m_fMaxSizeZ / 2) && pPos.z < m_pos.z + (m_fMaxSizeZ / 2))
		{
			D3DXVECTOR3 VecLine1, VecLine2, VecLine3;	// ポリゴンの頂点同士のベクトルを保存
			D3DXVECTOR3 VecPos1, VecPos2, VecPos3;		// プレイヤーとポリゴンの頂点とのベクトルを保存
			float OnPolygon1, OnPolygon2, OnPolygon3;	// プレイヤーがポリゴンの外積結果を保存

			// --- 相対値を出して、どのポリゴン上にいるかを特定する --- //
			int nIdxX = (int)PlayerPos.x / (int)m_fSizeX;
			int nIdxZ = (int)PlayerPos.z / (int)m_fSizeZ;

			// --- 求めたいポリゴンの頂点を算出 --- //
			int nIdxVtx = nIdxX + nIdxZ * (m_nNumDevideX + 1);

			VERTEX_3D* pVtx;		// 頂点情報へのポインタ

			// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
			m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// ***** 左のポリゴン(偶数) ***** //
			// --- それぞれの頂点からベクトルを算出 --- //
			VecLine1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;
			VecLine2 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx].pos;
			VecLine3 = pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos - pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecLine1, &VecLine1);
			D3DXVec3Normalize(&VecLine2, &VecLine2);
			D3DXVec3Normalize(&VecLine3, &VecLine3);

			// --- それぞれの3頂点からプレイヤーの位置までのベクトルを算出 --- //
			VecPos1 = pPos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;
			VecPos2 = pPos - pVtx[nIdxVtx].pos;
			VecPos3 = pPos - pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecPos1, &VecPos1);
			D3DXVec3Normalize(&VecPos2, &VecPos2);
			D3DXVec3Normalize(&VecPos3, &VecPos3);

			// --- プレイヤーがポリゴンの内側にいるかの判定用 --- //
			OnPolygon1 = (VecLine1.z * VecPos1.x) - (VecLine1.x * VecPos1.z);
			OnPolygon2 = (VecLine2.z * VecPos2.x) - (VecLine2.x * VecPos2.z);
			OnPolygon3 = (VecLine3.z * VecPos3.x) - (VecLine3.x * VecPos3.z);

			// --- ポリゴンの内側にいるとき --- //
			if (OnPolygon1 > 0.0f && OnPolygon2 > 0.0f && OnPolygon3 > 0.0f)
			{
				D3DXVECTOR3 Vec1, Vec2;

				// --- 法線を求める処理 --- // 
				Vec1 = pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos - pVtx[nIdxVtx].pos;
				Vec2 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx].pos;

				Vec1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;
				Vec2 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;

				// --- 外積を算出して法線の決定 --- //
				D3DXVec3Cross(&pVtx[nIdxVtx].nor, &Vec1, &Vec2);

				// --- 正規化 --- //
				D3DXVec3Normalize(&pVtx[nIdxVtx].nor, &pVtx[nIdxVtx].nor);

				if (pVtx[nIdxVtx].nor.y != 0)
				{
					float fPolHeight = -(((pPos.x - pVtx[nIdxVtx].pos.x) * pVtx[nIdxVtx].nor.x) + (-pVtx[nIdxVtx].pos.y * pVtx[nIdxVtx].nor.y) + ((pPos.z - pVtx[nIdxVtx].pos.z) * pVtx[nIdxVtx].nor.z)) / pVtx[nIdxVtx].nor.y;

					// --- 頂点バッファをアンロックする --- //
					m_pVtxBuff->Unlock();

					return fPolHeight;
				}
			}

			// ***** 右のポリゴン(奇数) ***** //
			// --- それぞれの頂点からベクトルを算出 --- //
			VecLine1 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx + 1].pos;
			VecLine2 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;
			VecLine3 = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecLine1, &VecLine1);
			D3DXVec3Normalize(&VecLine2, &VecLine2);
			D3DXVec3Normalize(&VecLine3, &VecLine3);

			// --- それぞれの3頂点からプレイヤーの位置までのベクトルを算出 --- //
			VecPos1 = pPos - pVtx[nIdxVtx + 1].pos;
			VecPos2 = pPos - pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;
			VecPos3 = pPos - pVtx[nIdxVtx].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecPos1, &VecPos1);
			D3DXVec3Normalize(&VecPos2, &VecPos2);
			D3DXVec3Normalize(&VecPos3, &VecPos3);

			// --- プレイヤーがポリゴンの内側にいるかの判定用 --- //
			OnPolygon1 = (VecLine1.z * VecPos1.x) - (VecLine1.x * VecPos1.z);
			OnPolygon2 = (VecLine2.z * VecPos2.x) - (VecLine2.x * VecPos2.z);
			OnPolygon3 = (VecLine3.z * VecPos3.x) - (VecLine3.x * VecPos3.z);

			// --- ポリゴンの内側にいるとき --- //
			if (OnPolygon1 > 0.0f && OnPolygon2 > 0.0f && OnPolygon3 > 0.0f)
			{
				D3DXVECTOR3 Vec1, Vec2;

				// --- 法線を求める処理 --- // 
				Vec1 = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;
				Vec2 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx].pos;

				// --- 外積を算出して法線の決定 --- //
				D3DXVec3Cross(&pVtx[nIdxVtx].nor, &Vec1, &Vec2);

				// --- 正規化 --- //
				D3DXVec3Normalize(&pVtx[nIdxVtx].nor, &pVtx[nIdxVtx].nor);

				if (pVtx[nIdxVtx].nor.y != 0)
				{
					float fPolHeight = -(((pPos.x - pVtx[nIdxVtx].pos.x) * pVtx[nIdxVtx].nor.x) + (-pVtx[nIdxVtx].pos.y * pVtx[nIdxVtx].nor.y) + ((pPos.z - pVtx[nIdxVtx].pos.z) * pVtx[nIdxVtx].nor.z)) / pVtx[nIdxVtx].nor.y;

					// --- 頂点バッファをアンロックする --- //
					m_pVtxBuff->Unlock();

					return fPolHeight;
				}
			}

			// --- 頂点バッファをアンロックする --- //
			m_pVtxBuff->Unlock();
		}
	}

	return 0.0f;
}

// =================================================
// メッシュポリゴンの高さを渡す(コライダー用)
// =================================================
float CMesh::Collision(D3DXVECTOR3 Pos, D3DXVECTOR3 SizeMax, D3DXMATRIX Worldmtx, D3DXVECTOR3 posVtx[4])
{
	if (this != NULL)
	{
		CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得

		CDebugProc* pDebugProc = CManager::GetDrbugProc();				// デバッグ表示のポインタ

		D3DXVECTOR3 PlayerPos = {};
		D3DXVECTOR3 VEC = VEC_NULL;

		D3DXVECTOR3 PlayerPosCenter = Pos - m_pos;
		D3DXVECTOR3 PlayerPosLeftUp;

		// --- 現在のプレイヤーの位置を、メッシュフィールドの左上を基準で算出 --- //
		PlayerPosLeftUp.x = Pos.x - (m_pos.x - (m_fMaxSizeX * 0.5f));
		PlayerPosLeftUp.z = Pos.z - (m_pos.z + (m_fMaxSizeZ * 0.5f));

		PlayerPosLeftUp.z *= -1;	// 負の符号の除去

		// --- プレイヤーの位置で判定継続かを決める --- //
	//	if (Pos.x > m_pos.x - (m_fMaxSizeX * 0.5f) && Pos.x < m_pos.x + (m_fMaxSizeX * 0.5f)
	//		&& Pos.z > m_pos.z - (m_fMaxSizeZ * 0.5f) && Pos.z < m_pos.z + (m_fMaxSizeZ * 0.5f))
		{
			D3DXVECTOR3 VecLine1, VecLine2, VecLine3;	// ポリゴンの頂点同士のベクトルを保存
			D3DXVECTOR3 VecPos1, VecPos2, VecPos3;		// プレイヤーとポリゴンの頂点とのベクトルを保存
			float OnPolygon1, OnPolygon2, OnPolygon3;	// プレイヤーがポリゴンの外積結果を保存

			VERTEX_3D* pVtx;		// 頂点情報へのポインタ

			// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
			m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// --- 相対値を出して、どのポリゴン上にいるかを特定する --- //
			int nIdxX = (int)PlayerPosLeftUp.x / (int)m_fSizeX;
			int nIdxZ = (int)PlayerPosLeftUp.z / (int)m_fSizeZ;

			// --- 求めたいポリゴンの頂点番号を算出 --- //
			int nIdxVtx = nIdxX + nIdxZ * (m_nNumDevideX + 1);

			// --- 当たり判定の外部ファイル保存 --- //
			if (pInputKeyboard->GetTrigger(DIK_NUMPAD1) == true)
			{// 地形保存
				FILE* pFile;

				pFile = fopen(COLLSION_FILE, "w"); // ファイルを開く

				if (pFile != NULL)
				{// ファイルオープン成功時
					for (int nCnt = 0; nCnt < m_nNumVtx; nCnt++) {
						fprintf(pFile, "%.1f %.1f %.1f\n", pVtx[nCnt].pos.x, pVtx[nCnt].pos.y, pVtx[nCnt].pos.z);
					}

					fclose(pFile);
				}
				else
				{// ファイルが存在しない
					return -1;
				}
			}

			// --- ワールド座標にある頂点 --- //
			D3DXVECTOR3 pVtxCol[4];
			D3DXVECTOR3 POS[4];
			D3DXVECTOR3 Size = { 0.0f, posVtx[0].y, 0.0f };

			// --- 外積当たり判定用 --- //
			D3DXVECTOR3 PosVtx[4] = {};
			D3DXVECTOR3 LinetoVec[4] = {};
			D3DXVECTOR3 PostoVec[4] = {};

			float OnPolygon[4] = {};

			// --- 判定用のために別変数に保存 --- //
			pVtxCol[0] = pVtx[nIdxVtx].pos;
			pVtxCol[1] = pVtx[nIdxVtx + 1].pos;
			pVtxCol[2] = pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;
			pVtxCol[3] = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;

			//// --- ワールド座標にそろえる --- //
			D3DXVec3TransformCoord(&pVtxCol[0], &Size, &m_mtxWorld);
			D3DXVec3TransformCoord(&pVtxCol[1], &Size, &m_mtxWorld);
			D3DXVec3TransformCoord(&pVtxCol[2], &Size, &m_mtxWorld);
			D3DXVec3TransformCoord(&pVtxCol[3], &Size, &m_mtxWorld);

			// --- プレイヤーとのベクトルを算出 --- //
			for (int nCnt = 0; nCnt < 4; nCnt++) {

				PostoVec[nCnt] = Pos - posVtx[nCnt];
			}

			// --- モデルの境界線ベクトルを算出 --- //
			LinetoVec[0] = posVtx[1] - posVtx[0];
			LinetoVec[1] = posVtx[2] - posVtx[1];
			LinetoVec[2] = posVtx[3] - posVtx[2];
			LinetoVec[3] = posVtx[0] - posVtx[3];

			// --- プレイヤーが境界線ベクトル内にいるかの判定 --- //
			for (int nCnt = 0; nCnt < 4; nCnt++) {

				OnPolygon[nCnt] = (LinetoVec[nCnt].z * PostoVec[nCnt].x) - (LinetoVec[nCnt].x * PostoVec[nCnt].z);
			}

			if (OnPolygon[0] >= 0.0f && OnPolygon[1] >= 0.0f && OnPolygon[2] >= 0.0f && OnPolygon[3] >= 0.0f)
			{
				return posVtx[0].y;
			}

			// ***** 左のポリゴン(偶数) ***** //
			// --- それぞれの頂点からベクトルを算出 --- //
			VecLine1 = pVtxCol[0] - pVtxCol[2];
			VecLine2 = pVtxCol[3] - pVtxCol[0];
			VecLine3 = pVtxCol[2] - pVtxCol[3];

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecLine1, &VecLine1);
			D3DXVec3Normalize(&VecLine2, &VecLine2);
			D3DXVec3Normalize(&VecLine3, &VecLine3);

			// --- それぞれの3頂点からプレイヤーの位置までのベクトルを算出 --- //
			VecPos1 = PlayerPosCenter - pVtxCol[2];
			VecPos2 = PlayerPosCenter - pVtxCol[0];
			VecPos3 = PlayerPosCenter - pVtxCol[3];

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecPos1, &VecPos1);
			D3DXVec3Normalize(&VecPos2, &VecPos2);
			D3DXVec3Normalize(&VecPos3, &VecPos3);

			// --- プレイヤーがポリゴンの内側にいるかの判定用 --- //
			OnPolygon1 = (VecLine1.z * VecPos1.x) - (VecLine1.x * VecPos1.z);
			OnPolygon2 = (VecLine2.z * VecPos2.x) - (VecLine2.x * VecPos2.z);
			OnPolygon3 = (VecLine3.z * VecPos3.x) - (VecLine3.x * VecPos3.z);

			// --- ポリゴンの内側にいるとき --- //
			if (OnPolygon1 >= 0.0f && OnPolygon2 >= 0.0f && OnPolygon3 >= 0.0f)
			{
				D3DXVECTOR3 Vec1, Vec2, nor;

				// --- 法線を求める処理 --- // 
				Vec1 = pVtxCol[0] - pVtxCol[2];
				Vec2 = pVtxCol[3] - pVtxCol[2];

				D3DXVec3Normalize(&Vec1, &Vec1);
				D3DXVec3Normalize(&Vec2, &Vec2);

				// --- 外積を算出して法線の決定 --- //
				D3DXVec3Cross(&nor, &Vec1, &Vec2);

				// --- 正規化 --- //
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0)
				{
					float fPolHeight = -(((PlayerPosCenter.x - pVtxCol[0].x) * nor.x)
						+ (-pVtxCol[0].y * nor.y) +
						((PlayerPosCenter.z - pVtxCol[0].z) * nor.z))
						/ nor.y;

					// --- 頂点バッファをアンロックする --- //
					m_pVtxBuff->Unlock();

					m_fHeight = pVtxCol[0].y;
					return pVtxCol[0].y;
				}
			}

			// ***** 右のポリゴン(奇数) ***** //
			// --- それぞれの頂点からベクトルを算出 --- //
			VecLine1 = pVtxCol[3] - pVtxCol[1];
			VecLine2 = pVtxCol[0] - pVtxCol[3];
			VecLine3 = pVtxCol[1] - pVtxCol[0];

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecLine1, &VecLine1);
			D3DXVec3Normalize(&VecLine2, &VecLine2);
			D3DXVec3Normalize(&VecLine3, &VecLine3);

			// --- それぞれの3頂点からプレイヤーの位置までのベクトルを算出 --- //
			VecPos1 = PlayerPosCenter - pVtxCol[1];
			VecPos2 = PlayerPosCenter - pVtxCol[3];
			VecPos3 = PlayerPosCenter - pVtxCol[0];

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecPos1, &VecPos1);
			D3DXVec3Normalize(&VecPos2, &VecPos2);
			D3DXVec3Normalize(&VecPos3, &VecPos3);

			// --- プレイヤーがポリゴンの内側にいるかの判定用 --- //
			OnPolygon1 = (VecLine1.z * VecPos1.x) - (VecLine1.x * VecPos1.z);
			OnPolygon2 = (VecLine2.z * VecPos2.x) - (VecLine2.x * VecPos2.z);
			OnPolygon3 = (VecLine3.z * VecPos3.x) - (VecLine3.x * VecPos3.z);

			// --- ポリゴンの内側にいるとき --- //
			if (OnPolygon1 >= 0.0f && OnPolygon2 >= 0.0f && OnPolygon3 >= 0.0f)
			{
				D3DXVECTOR3 Vec1, Vec2, nor;

				// --- 法線を求める処理 --- // 
				Vec1 = pVtxCol[0] - pVtxCol[1];
				Vec2 = pVtxCol[3] - pVtxCol[1];

				// --- 正規化 --- //
				D3DXVec3Normalize(&Vec1, &Vec1);
				D3DXVec3Normalize(&Vec2, &Vec2);

				// --- 外積を算出して法線の決定 --- //
				D3DXVec3Cross(&nor, &Vec2, &Vec1);

				// --- 正規化 --- //
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0)
				{
					float fPolHeight = -(((PlayerPosCenter.x - pVtxCol[0].x) * nor.x) +
						(-pVtxCol[0].y * nor.y) +
						((PlayerPosCenter.z - pVtxCol[0].z) * nor.z))
						/ nor.y;

					// --- 頂点バッファをアンロックする --- //
					m_pVtxBuff->Unlock();

					m_fHeight = pVtxCol[0].y;
					return pVtxCol[0].y;
				}
			}
		}
	}

	return 0.0f;
}

// =================================================
// メッシュポリゴンの高さを渡す(アイテム用)
// =================================================
float CMesh::Collision(D3DXVECTOR3 Pos, D3DXVECTOR3 pPosOld)
{
	if (this != NULL)
	{
		CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得

		CDebugProc* pDebugProc = CManager::GetDrbugProc();				// デバッグ表示のポインタ

		D3DXVECTOR3 PlayerPos = {};

		D3DXVECTOR3 PlayerPosCenter = Pos - m_pos;
		D3DXVECTOR3 PlayerPosLeftUp;

		// --- 現在のプレイヤーの位置を、メッシュフィールドの左上を基準で算出 --- //
		PlayerPosLeftUp.x = Pos.x - (m_pos.x - (m_fMaxSizeX * 0.5f));
		PlayerPosLeftUp.z = Pos.z - (m_pos.z + (m_fMaxSizeZ * 0.5f));

		PlayerPosLeftUp.z *= -1;	// 負の符号の除去

		// --- プレイヤーの位置で判定継続かを決める --- //
		if (Pos.x > m_pos.x - (m_fMaxSizeX * 0.5f) && Pos.x < m_pos.x + (m_fMaxSizeX * 0.5f)
			&& Pos.z > m_pos.z - (m_fMaxSizeZ * 0.5f) && Pos.z < m_pos.z + (m_fMaxSizeZ * 0.5f))
		{
			D3DXVECTOR3 VecLine1, VecLine2, VecLine3;	// ポリゴンの頂点同士のベクトルを保存
			D3DXVECTOR3 VecPos1, VecPos2, VecPos3;		// プレイヤーとポリゴンの頂点とのベクトルを保存
			float OnPolygon1, OnPolygon2, OnPolygon3;	// プレイヤーがポリゴンの外積結果を保存

			VERTEX_3D* pVtx;		// 頂点情報へのポインタ

			// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
			m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// --- 相対値を出して、どのポリゴン上にいるかを特定する --- //
			int nIdxX = (int)PlayerPosLeftUp.x / (int)m_fSizeX;
			int nIdxZ = (int)PlayerPosLeftUp.z / (int)m_fSizeZ;

			// --- 求めたいポリゴンの頂点番号を算出 --- //
			int nIdxVtx = nIdxX + nIdxZ * (m_nNumDevideX + 1);

			// --- 当たり判定の外部ファイル保存 --- //
			if (pInputKeyboard->GetTrigger(DIK_NUMPAD1) == true)
			{// 地形保存
				FILE* pFile;

				pFile = fopen(COLLSION_FILE, "w"); // ファイルを開く

				if (pFile != NULL)
				{// ファイルオープン成功時
					for (int nCnt = 0; nCnt < m_nNumVtx; nCnt++) {
						fprintf(pFile, "%.1f %.1f %.1f\n", pVtx[nCnt].pos.x, pVtx[nCnt].pos.y, pVtx[nCnt].pos.z);
					}

					fclose(pFile);
				}
				else
				{// ファイルが存在しない
					return -1;
				}
			}

			// ***** 左のポリゴン(偶数) ***** //
			// --- それぞれの頂点からベクトルを算出 --- //
			VecLine1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;
			VecLine2 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx].pos;
			VecLine3 = pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos - pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecLine1, &VecLine1);
			D3DXVec3Normalize(&VecLine2, &VecLine2);
			D3DXVec3Normalize(&VecLine3, &VecLine3);

			// --- それぞれの3頂点からプレイヤーの位置までのベクトルを算出 --- //
			VecPos1 = PlayerPosCenter - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;
			VecPos2 = PlayerPosCenter - pVtx[nIdxVtx].pos;
			VecPos3 = PlayerPosCenter - pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecPos1, &VecPos1);
			D3DXVec3Normalize(&VecPos2, &VecPos2);
			D3DXVec3Normalize(&VecPos3, &VecPos3);

			// --- プレイヤーがポリゴンの内側にいるかの判定用 --- //
			OnPolygon1 = (VecLine1.z * VecPos1.x) - (VecLine1.x * VecPos1.z);
			OnPolygon2 = (VecLine2.z * VecPos2.x) - (VecLine2.x * VecPos2.z);
			OnPolygon3 = (VecLine3.z * VecPos3.x) - (VecLine3.x * VecPos3.z);

			// --- ポリゴンの内側にいるとき --- //
			if (OnPolygon1 >= 0.0f && OnPolygon2 >= 0.0f && OnPolygon3 >= 0.0f)
			{
				// --- メッシュフィールドに起伏をつける処理 --- //
				if (pInputKeyboard->GetPress(DIK_0) == true)
				{
					pVtx[nIdxVtx].pos.y += 1.0f;
					pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos.y += 1.0f;
					pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos.y += 1.0f;
					pVtx[nIdxVtx + 1].pos.y += 1.0f;
				}
				else if (pInputKeyboard->GetPress(DIK_9) == true)
				{
					pVtx[nIdxVtx].pos.y -= 1.0f;
					pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos.y -= 1.0f;
					pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos.y -= 1.0f;
					pVtx[nIdxVtx + 1].pos.y -= 1.0f;
				}

				D3DXVECTOR3 Vec1, Vec2, nor;

				// --- 法線を求める処理 --- // 
				Vec1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;
				Vec2 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;

				D3DXVec3Normalize(&Vec1, &Vec1);
				D3DXVec3Normalize(&Vec2, &Vec2);

				// --- 外積を算出して法線の決定 --- //
				D3DXVec3Cross(&nor, &Vec1, &Vec2);

				// --- 正規化 --- //
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0)
				{
					float fPolHeight = -(((PlayerPosCenter.x - pVtx[nIdxVtx].pos.x) * nor.x) + (-pVtx[nIdxVtx].pos.y * nor.y) + ((PlayerPosCenter.z - pVtx[nIdxVtx].pos.z) * nor.z)) / nor.y;

					// --- 頂点バッファをアンロックする --- //
					m_pVtxBuff->Unlock();

					m_fHeight = fPolHeight;
					return fPolHeight + m_pos.y;
				}
			}

			// ***** 右のポリゴン(奇数) ***** //
			// --- それぞれの頂点からベクトルを算出 --- //
			VecLine1 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx + 1].pos;
			VecLine2 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;
			VecLine3 = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecLine1, &VecLine1);
			D3DXVec3Normalize(&VecLine2, &VecLine2);
			D3DXVec3Normalize(&VecLine3, &VecLine3);

			// --- それぞれの3頂点からプレイヤーの位置までのベクトルを算出 --- //
			VecPos1 = PlayerPosCenter - pVtx[nIdxVtx + 1].pos;
			VecPos2 = PlayerPosCenter - pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;
			VecPos3 = PlayerPosCenter - pVtx[nIdxVtx].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecPos1, &VecPos1);
			D3DXVec3Normalize(&VecPos2, &VecPos2);
			D3DXVec3Normalize(&VecPos3, &VecPos3);

			// --- プレイヤーがポリゴンの内側にいるかの判定用 --- //
			OnPolygon1 = (VecLine1.z * VecPos1.x) - (VecLine1.x * VecPos1.z);
			OnPolygon2 = (VecLine2.z * VecPos2.x) - (VecLine2.x * VecPos2.z);
			OnPolygon3 = (VecLine3.z * VecPos3.x) - (VecLine3.x * VecPos3.z);

			// --- ポリゴンの内側にいるとき --- //
			if (OnPolygon1 >= 0.0f && OnPolygon2 >= 0.0f && OnPolygon3 >= 0.0f)
			{
				// --- メッシュフィールドに起伏をつける処理 --- //
				if (pInputKeyboard->GetPress(DIK_0) == true)
				{
					pVtx[nIdxVtx].pos.y += 1.0f;
					pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos.y += 1.0f;
					pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos.y += 1.0f;
					pVtx[nIdxVtx + 1].pos.y += 1.0f;
				}
				else if (pInputKeyboard->GetPress(DIK_9) == true)
				{
					pVtx[nIdxVtx].pos.y -= 1.0f;
					pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos.y -= 1.0f;
					pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos.y -= 1.0f;
					pVtx[nIdxVtx + 1].pos.y -= 1.0f;
				}

				D3DXVECTOR3 Vec1, Vec2, nor;

				// --- 法線を求める処理 --- // 
				Vec1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
				Vec2 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx + 1].pos;

				// --- 正規化 --- //
				D3DXVec3Normalize(&Vec1, &Vec1);
				D3DXVec3Normalize(&Vec2, &Vec2);

				// --- 外積を算出して法線の決定 --- //
				D3DXVec3Cross(&nor, &Vec2, &Vec1);

				// --- 正規化 --- //
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0)
				{
					float fPolHeight = -(((PlayerPosCenter.x - pVtx[nIdxVtx].pos.x) * nor.x) + (-pVtx[nIdxVtx].pos.y * nor.y) + ((PlayerPosCenter.z - pVtx[nIdxVtx].pos.z) * nor.z)) / nor.y;

					// --- 頂点バッファをアンロックする --- //
					m_pVtxBuff->Unlock();

					m_fHeight = fPolHeight;
					return fPolHeight + m_pos.y;
				}
			}
		}
	}

	return 0.0f;
}

// =================================================
// 当たり判定
// =================================================
bool CMesh::bCollision(D3DXVECTOR3 Pos, D3DXVECTOR3 pPosOld)
{
	if (this != NULL)
	{
		CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得

		CDebugProc* pDebugProc = CManager::GetDrbugProc();				// デバッグ表示のポインタ

		D3DXVECTOR3 PlayerPos = {};

		D3DXVECTOR3 PlayerPosCenter = Pos - m_pos;
		D3DXVECTOR3 PlayerPosLeftUp;

		// --- 現在のプレイヤーの位置を、メッシュフィールドの左上を基準で算出 --- //
		PlayerPosLeftUp.x = Pos.x - (m_pos.x - (m_fMaxSizeX * 0.5f));
		PlayerPosLeftUp.z = Pos.z - (m_pos.z + (m_fMaxSizeZ * 0.5f));

		PlayerPosLeftUp.z *= -1;	// 負の符号の除去

		// --- プレイヤーの位置で判定継続かを決める --- //
		if (Pos.x > m_pos.x - (m_fMaxSizeX * 0.5f) && Pos.x < m_pos.x + (m_fMaxSizeX * 0.5f)
			&& Pos.z > m_pos.z - (m_fMaxSizeZ * 0.5f) && Pos.z < m_pos.z + (m_fMaxSizeZ * 0.5f))
		{
			D3DXVECTOR3 VecLine1, VecLine2, VecLine3;	// ポリゴンの頂点同士のベクトルを保存
			D3DXVECTOR3 VecPos1, VecPos2, VecPos3;		// プレイヤーとポリゴンの頂点とのベクトルを保存
			float OnPolygon1, OnPolygon2, OnPolygon3;	// プレイヤーがポリゴンの外積結果を保存

			VERTEX_3D* pVtx;		// 頂点情報へのポインタ

			// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
			m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// --- 相対値を出して、どのポリゴン上にいるかを特定する --- //
			int nIdxX = (int)PlayerPosLeftUp.x / (int)m_fSizeX;
			int nIdxZ = (int)PlayerPosLeftUp.z / (int)m_fSizeZ;

			// --- 求めたいポリゴンの頂点番号を算出 --- //
			int nIdxVtx = nIdxX + nIdxZ * (m_nNumDevideX + 1);

			// ***** 左のポリゴン(偶数) ***** //
			// --- それぞれの頂点からベクトルを算出 --- //
			VecLine1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;
			VecLine2 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx].pos;
			VecLine3 = pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos - pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecLine1, &VecLine1);
			D3DXVec3Normalize(&VecLine2, &VecLine2);
			D3DXVec3Normalize(&VecLine3, &VecLine3);

			// --- それぞれの3頂点からプレイヤーの位置までのベクトルを算出 --- //
			VecPos1 = PlayerPosCenter - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;
			VecPos2 = PlayerPosCenter - pVtx[nIdxVtx].pos;
			VecPos3 = PlayerPosCenter - pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecPos1, &VecPos1);
			D3DXVec3Normalize(&VecPos2, &VecPos2);
			D3DXVec3Normalize(&VecPos3, &VecPos3);

			// --- プレイヤーがポリゴンの内側にいるかの判定用 --- //
			OnPolygon1 = (VecLine1.z * VecPos1.x) - (VecLine1.x * VecPos1.z);
			OnPolygon2 = (VecLine2.z * VecPos2.x) - (VecLine2.x * VecPos2.z);
			OnPolygon3 = (VecLine3.z * VecPos3.x) - (VecLine3.x * VecPos3.z);

			// --- ポリゴンの内側にいるとき --- //
			if (OnPolygon1 >= 0.0f && OnPolygon2 >= 0.0f && OnPolygon3 >= 0.0f)
			{
				return true;
			}

			// ***** 右のポリゴン(奇数) ***** //
			// --- それぞれの頂点からベクトルを算出 --- //
			VecLine1 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx + 1].pos;
			VecLine2 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;
			VecLine3 = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecLine1, &VecLine1);
			D3DXVec3Normalize(&VecLine2, &VecLine2);
			D3DXVec3Normalize(&VecLine3, &VecLine3);

			// --- それぞれの3頂点からプレイヤーの位置までのベクトルを算出 --- //
			VecPos1 = PlayerPosCenter - pVtx[nIdxVtx + 1].pos;
			VecPos2 = PlayerPosCenter - pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;
			VecPos3 = PlayerPosCenter - pVtx[nIdxVtx].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecPos1, &VecPos1);
			D3DXVec3Normalize(&VecPos2, &VecPos2);
			D3DXVec3Normalize(&VecPos3, &VecPos3);

			// --- プレイヤーがポリゴンの内側にいるかの判定用 --- //
			OnPolygon1 = (VecLine1.z * VecPos1.x) - (VecLine1.x * VecPos1.z);
			OnPolygon2 = (VecLine2.z * VecPos2.x) - (VecLine2.x * VecPos2.z);
			OnPolygon3 = (VecLine3.z * VecPos3.x) - (VecLine3.x * VecPos3.z);

			// --- ポリゴンの内側にいるとき --- //
			if (OnPolygon1 >= 0.0f && OnPolygon2 >= 0.0f && OnPolygon3 >= 0.0f)
			{
				return true;
			}
		}
	}

	return false;
}

// =================================================
// 内積の計算(meshring)
// =================================================
float CMesh::DotCalRing(D3DXVECTOR3 Pos, D3DXVECTOR3 pPosOld)
{
	if (this != NULL)
	{
		CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得

		CDebugProc* pDebugProc = CManager::GetDrbugProc();				// デバッグ表示のポインタ

		CPlayer* pPlayer = CGame::GetPlayer();

		D3DXVECTOR3 PlayerPos = {};
		D3DXVECTOR3 VEC = VEC_NULL;

		D3DXVECTOR3 PlayerPosCenter = Pos - m_pos;
		D3DXVECTOR3 PlayerPosLeftUp;

		// --- 現在のプレイヤーの位置を、メッシュフィールドの左上を基準で算出 --- //
		PlayerPosLeftUp.x = Pos.x - (m_pos.x - (m_fMaxSizeX * 0.5f));
		PlayerPosLeftUp.z = Pos.z - (m_pos.z + (m_fMaxSizeZ * 0.5f));

		PlayerPosLeftUp.z *= -1;	// 負の符号の除去

		D3DXVECTOR3 VecLine1, VecLine2, VecLine3;	// ポリゴンの頂点同士のベクトルを保存
		D3DXVECTOR3 VecPos1, VecPos2, VecPos3;		// プレイヤーとポリゴンの頂点とのベクトルを保存
		float OnPolygon1, OnPolygon2, OnPolygon3;	// プレイヤーがポリゴンの外積結果を保存

		VERTEX_3D* pVtx;		// 頂点情報へのポインタ

		// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// --- 相対値を出して、どのポリゴン上にいるかを特定する --- //
		//int nIdxX = (int)PlayerPosLeftUp.x / (int)m_fSizeX;
		//int nIdxZ = (int)PlayerPosLeftUp.z / (int)m_fSizeZ;

		// --- 求めたいポリゴンの頂点番号を算出 --- //
		//int nIdxVtx = nIdxX + nIdxZ * (m_nNumDevideX + 1);

		for (int nCnt = 0; nCnt < m_nNumVtx; nCnt++)
		{
			// ***** 左のポリゴン(偶数) ***** //
			// --- それぞれの頂点からベクトルを算出 --- //
			VecLine1 = pVtx[nCnt].pos - pVtx[nCnt + (m_nNumDevideX + 1)].pos;
			VecLine2 = pVtx[nCnt + (m_nNumDevideX + 2)].pos - pVtx[nCnt].pos;
			VecLine3 = pVtx[nCnt + (m_nNumDevideX + 1)].pos - pVtx[nCnt + (m_nNumDevideX + 2)].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecLine1, &VecLine1);
			D3DXVec3Normalize(&VecLine2, &VecLine2);
			D3DXVec3Normalize(&VecLine3, &VecLine3);

			// --- それぞれの3頂点からプレイヤーの位置までのベクトルを算出 --- //
			VecPos1 = PlayerPosCenter - pVtx[nCnt + (m_nNumDevideX + 1)].pos;
			VecPos2 = PlayerPosCenter - pVtx[nCnt].pos;
			VecPos3 = PlayerPosCenter - pVtx[nCnt + (m_nNumDevideX + 2)].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecPos1, &VecPos1);
			D3DXVec3Normalize(&VecPos2, &VecPos2);
			D3DXVec3Normalize(&VecPos3, &VecPos3);

			// --- プレイヤーがポリゴンの内側にいるかの判定用 --- //
			OnPolygon1 = (VecLine1.z * VecPos1.x) - (VecLine1.x * VecPos1.z);
			OnPolygon2 = (VecLine2.z * VecPos2.x) - (VecLine2.x * VecPos2.z);
			OnPolygon3 = (VecLine3.z * VecPos3.x) - (VecLine3.x * VecPos3.z);

			// --- ポリゴンの内側にいるとき --- //
			if (OnPolygon1 >= 0.0f && OnPolygon2 >= 0.0f && OnPolygon3 >= 0.0f)
			{
				D3DXVECTOR3 Vec1, Vec2, nor;

				// --- 法線を求める処理 --- // 
				Vec1 = pVtx[nCnt].pos - pVtx[nCnt + (m_nNumDevideX + 1)].pos;
				Vec2 = pVtx[nCnt + (m_nNumDevideX + 2)].pos - pVtx[nCnt + (m_nNumDevideX + 1)].pos;

				D3DXVec3Normalize(&Vec1, &Vec1);
				D3DXVec3Normalize(&Vec2, &Vec2);

				// --- 外積を算出して法線の決定 --- //
				D3DXVec3Cross(&nor, &Vec1, &Vec2);

				// --- 正規化 --- //
				D3DXVec3Normalize(&nor, &nor);

				D3DXVECTOR3 VecNor = pVtx[nCnt].nor;	// 一つの頂点の法線をコピー
				D3DXVec3Normalize(&VecNor, &VecNor);

				if (nor.y != 0)
				{// 垂直じゃないとき
					D3DXVECTOR3 VecMove = Pos - (pVtx[nCnt].pos + m_pos);	// プレイヤーの位置ベクトルを取得
					D3DXVec3Normalize(&VecMove, &VecMove);

					// --- ドット計算 --- //
					float fPolHeight = (VecNor.x * VecMove.x) + (VecNor.y * VecMove.y) + (VecNor.z * VecMove.z);
					fPolHeight = D3DXVec3Dot(&VecNor, &VecMove);

					// --- 頂点バッファをアンロックする --- //
					m_pVtxBuff->Unlock();

					return fPolHeight;
				}
			}

			// ***** 右のポリゴン(奇数) ***** //
			// --- それぞれの頂点からベクトルを算出 --- //
			VecLine1 = pVtx[nCnt + (m_nNumDevideX + 2)].pos - pVtx[nCnt + 1].pos;
			VecLine2 = pVtx[nCnt].pos - pVtx[nCnt + (m_nNumDevideX + 2)].pos;
			VecLine3 = pVtx[nCnt + 1].pos - pVtx[nCnt].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecLine1, &VecLine1);
			D3DXVec3Normalize(&VecLine2, &VecLine2);
			D3DXVec3Normalize(&VecLine3, &VecLine3);

			// --- それぞれの3頂点からプレイヤーの位置までのベクトルを算出 --- //
			VecPos1 = PlayerPosCenter - pVtx[nCnt + 1].pos;
			VecPos2 = PlayerPosCenter - pVtx[nCnt + (m_nNumDevideX + 2)].pos;
			VecPos3 = PlayerPosCenter - pVtx[nCnt].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecPos1, &VecPos1);
			D3DXVec3Normalize(&VecPos2, &VecPos2);
			D3DXVec3Normalize(&VecPos3, &VecPos3);

			// --- プレイヤーがポリゴンの内側にいるかの判定用 --- //
			OnPolygon1 = (VecLine1.z * VecPos1.x) - (VecLine1.x * VecPos1.z);
			OnPolygon2 = (VecLine2.z * VecPos2.x) - (VecLine2.x * VecPos2.z);
			OnPolygon3 = (VecLine3.z * VecPos3.x) - (VecLine3.x * VecPos3.z);

			// --- ポリゴンの内側にいるとき --- //
			if (OnPolygon1 >= 0.0f && OnPolygon2 >= 0.0f && OnPolygon3 >= 0.0f)
			{
				D3DXVECTOR3 Vec1, Vec2, nor;

				// --- 法線を求める処理 --- // 
				Vec1 = pVtx[nCnt].pos - pVtx[nCnt + 1].pos;
				Vec2 = pVtx[nCnt + (m_nNumDevideX + 2)].pos - pVtx[nCnt + 1].pos;

				// --- 正規化 --- //
				D3DXVec3Normalize(&Vec1, &Vec1);
				D3DXVec3Normalize(&Vec2, &Vec2);

				// --- 外積を算出して法線の決定 --- //
				D3DXVec3Cross(&nor, &Vec2, &Vec1);

				// --- 正規化 --- //
				D3DXVec3Normalize(&nor, &nor);

				D3DXVECTOR3 VecNor = pVtx[nCnt].nor;	// 一つの頂点の法線をコピー
				D3DXVec3Normalize(&VecNor, &VecNor);

				if (nor.y != 0)
				{// 垂直じゃないとき
					//	float fPolHeight = (-pPlayer->GetMoveVec().x * nor.x) + (-pPlayer->GetMoveVec().y * nor.y);	// 内積を算出
					D3DXVECTOR3 VecMove = Pos - (pVtx[nCnt].pos + m_pos);	// プレイヤーの移動ベクトルを取得
					D3DXVec3Normalize(&VecMove, &VecMove);

					// --- ドット計算 --- //
					float fPolHeight = (VecNor.x * VecMove.x) + (VecNor.y * VecMove.y) + (VecNor.z * VecMove.z);
					fPolHeight = D3DXVec3Dot(&VecNor, &VecMove);

					// --- 頂点バッファをアンロックする --- //
					m_pVtxBuff->Unlock();

					return fPolHeight;
				}
			}
		}
	}

	return 0.0f;
}

// =================================================
// 内積の計算
// =================================================
float CMesh::DotCal(D3DXVECTOR3 Pos, D3DXVECTOR3 pPosOld)
{
	if (this != NULL)
	{
		CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得

		CDebugProc* pDebugProc = CManager::GetDrbugProc();				// デバッグ表示のポインタ

		CPlayer* pPlayer = CGame::GetPlayer();

		D3DXVECTOR3 PlayerPos = {};
		D3DXVECTOR3 VEC = VEC_NULL;

		D3DXVECTOR3 PlayerPosCenter = Pos - m_pos;
		D3DXVECTOR3 PlayerPosLeftUp;

		// --- 現在のプレイヤーの位置を、メッシュフィールドの左上を基準で算出 --- //
		PlayerPosLeftUp.x = Pos.x - (m_pos.x - (m_fMaxSizeX * 0.5f));
		PlayerPosLeftUp.z = Pos.z - (m_pos.z + (m_fMaxSizeZ * 0.5f));

		PlayerPosLeftUp.z *= -1;	// 負の符号の除去

		// --- プレイヤーの位置で判定継続かを決める --- //
		//if (Pos.x > m_pos.x - (m_fMaxSizeX * 0.5f) && Pos.x < m_pos.x + (m_fMaxSizeX * 0.5f)
		//	&& Pos.z > m_pos.z - (m_fMaxSizeZ * 0.5f) && Pos.z < m_pos.z + (m_fMaxSizeZ * 0.5f))
		{
			D3DXVECTOR3 VecLine1, VecLine2, VecLine3;	// ポリゴンの頂点同士のベクトルを保存
			D3DXVECTOR3 VecPos1, VecPos2, VecPos3;		// プレイヤーとポリゴンの頂点とのベクトルを保存
			float OnPolygon1, OnPolygon2, OnPolygon3;	// プレイヤーがポリゴンの外積結果を保存

			VERTEX_3D* pVtx;		// 頂点情報へのポインタ

			// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
			m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// --- 相対値を出して、どのポリゴン上にいるかを特定する --- //
			int nIdxX = (int)PlayerPosLeftUp.x / (int)m_fSizeX;
			int nIdxZ = (int)PlayerPosLeftUp.z / (int)m_fSizeZ;

			// --- 求めたいポリゴンの頂点番号を算出 --- //
			int nIdxVtx = nIdxX + nIdxZ * (m_nNumDevideX + 1);

			// ***** 左のポリゴン(偶数) ***** //
			// --- それぞれの頂点からベクトルを算出 --- //
			VecLine1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;
			VecLine2 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx].pos;
			VecLine3 = pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos - pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecLine1, &VecLine1);
			D3DXVec3Normalize(&VecLine2, &VecLine2);
			D3DXVec3Normalize(&VecLine3, &VecLine3);

			// --- それぞれの3頂点からプレイヤーの位置までのベクトルを算出 --- //
			VecPos1 = PlayerPosCenter - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;
			VecPos2 = PlayerPosCenter - pVtx[nIdxVtx].pos;
			VecPos3 = PlayerPosCenter - pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecPos1, &VecPos1);
			D3DXVec3Normalize(&VecPos2, &VecPos2);
			D3DXVec3Normalize(&VecPos3, &VecPos3);

			// --- プレイヤーがポリゴンの内側にいるかの判定用 --- //
			OnPolygon1 = (VecLine1.z * VecPos1.x) - (VecLine1.x * VecPos1.z);
			OnPolygon2 = (VecLine2.z * VecPos2.x) - (VecLine2.x * VecPos2.z);
			OnPolygon3 = (VecLine3.z * VecPos3.x) - (VecLine3.x * VecPos3.z);

			// --- ポリゴンの内側にいるとき --- //
			//if (OnPolygon1 >= 0.0f && OnPolygon2 >= 0.0f && OnPolygon3 >= 0.0f)
			{
				D3DXVECTOR3 Vec1, Vec2, nor;

				// --- 法線を求める処理 --- // 
				Vec1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;
				Vec2 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;

				D3DXVec3Normalize(&Vec1, &Vec1);
				D3DXVec3Normalize(&Vec2, &Vec2);

				// --- 外積を算出して法線の決定 --- //
				D3DXVec3Cross(&nor, &Vec1, &Vec2);

				// --- 正規化 --- //
				D3DXVec3Normalize(&nor, &nor);

				D3DXVECTOR3 VecNor = pVtx[nIdxVtx].nor;	// 一つの頂点の法線をコピー
				D3DXVec3Normalize(&VecNor, &VecNor);

				if (nor.y != 0)
				{// 垂直じゃないとき
					D3DXVECTOR3 VecMove = Pos - (pVtx[nIdxVtx].pos + m_pos);	// プレイヤーの位置ベクトルを取得
					D3DXVec3Normalize(&VecMove, &VecMove);

					// --- ドット計算 --- //
					float fPolHeight = (nor.x * VecMove.x) + (nor.y * VecMove.y) + (nor.z * VecMove.z);

					// --- 頂点バッファをアンロックする --- //
					m_pVtxBuff->Unlock();

					return fPolHeight;
				}
			}

			// ***** 右のポリゴン(奇数) ***** //
			// --- それぞれの頂点からベクトルを算出 --- //
			VecLine1 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx + 1].pos;
			VecLine2 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;
			VecLine3 = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecLine1, &VecLine1);
			D3DXVec3Normalize(&VecLine2, &VecLine2);
			D3DXVec3Normalize(&VecLine3, &VecLine3);

			// --- それぞれの3頂点からプレイヤーの位置までのベクトルを算出 --- //
			VecPos1 = PlayerPosCenter - pVtx[nIdxVtx + 1].pos;
			VecPos2 = PlayerPosCenter - pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos;
			VecPos3 = PlayerPosCenter - pVtx[nIdxVtx].pos;

			// --- 正規化 --- //
			D3DXVec3Normalize(&VecPos1, &VecPos1);
			D3DXVec3Normalize(&VecPos2, &VecPos2);
			D3DXVec3Normalize(&VecPos3, &VecPos3);

			// --- プレイヤーがポリゴンの内側にいるかの判定用 --- //
			OnPolygon1 = (VecLine1.z * VecPos1.x) - (VecLine1.x * VecPos1.z);
			OnPolygon2 = (VecLine2.z * VecPos2.x) - (VecLine2.x * VecPos2.z);
			OnPolygon3 = (VecLine3.z * VecPos3.x) - (VecLine3.x * VecPos3.z);

			// --- ポリゴンの内側にいるとき --- //
			if (OnPolygon1 >= 0.0f && OnPolygon2 >= 0.0f && OnPolygon3 >= 0.0f)
			{
				D3DXVECTOR3 Vec1, Vec2, nor;

				// --- 法線を求める処理 --- // 
				Vec1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
				Vec2 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx + 1].pos;

				// --- 正規化 --- //
				D3DXVec3Normalize(&Vec1, &Vec1);
				D3DXVec3Normalize(&Vec2, &Vec2);

				// --- 外積を算出して法線の決定 --- //
				D3DXVec3Cross(&nor, &Vec2, &Vec1);

				// --- 正規化 --- //
				D3DXVec3Normalize(&nor, &nor);

				D3DXVECTOR3 VecNor = pVtx[nIdxVtx].nor;	// 一つの頂点の法線をコピー
				D3DXVec3Normalize(&VecNor, &VecNor);

				if (nor.y != 0)
				{// 垂直じゃないとき
					//	float fPolHeight = (-pPlayer->GetMoveVec().x * nor.x) + (-pPlayer->GetMoveVec().y * nor.y);	// 内積を算出
					D3DXVECTOR3 VecMove = Pos - (pVtx[nIdxVtx].pos + m_pos);	// プレイヤーの移動ベクトルを取得
					D3DXVec3Normalize(&VecMove, &VecMove);

					// --- ドット計算 --- //
					float fPolHeight = (VecNor.x * VecMove.x) + (VecNor.y * VecMove.y) + (VecNor.z * VecMove.z);

					// --- 頂点バッファをアンロックする --- //
					m_pVtxBuff->Unlock();

					return fPolHeight;
				}
			}
		}
	}

	return 0.0f;
}

// =================================================
// 法線の設定
// =================================================
void CMesh::SetNor(void)
{
	VERTEX_3D* pVtx;		// 頂点情報へのポインタ

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	int nIdxVtx = 0;	// 現在の頂点のインデックス

	// --- 左側のポリゴン用 --- //
	D3DXVECTOR3 VecA1;	// 法線用のベクトルを保存
	D3DXVECTOR3 VecA2;	// 法線用のベクトルを保存
	D3DXVECTOR3 norA = {};	// 法線の保存

	// --- 右側のポリゴン用 --- //
	D3DXVECTOR3 VecB1;	// 法線用のベクトルを保存
	D3DXVECTOR3 VecB2;	// 法線用のベクトルを保存
	D3DXVECTOR3 norB = {};	// 法線の保存

	// --- 複数の法線の平均を算出するために格納しておく変数 --- //
	D3DXVECTOR3 norC = {};	// 法線の保存
	D3DXVECTOR3 norD = {};	// 法線の保存
	D3DXVECTOR3 norE = {};	// 法線の保存
	D3DXVECTOR3 norF = {};	// 法線の保存

	static bool bChange[60000];	// 法線の計算結果の違いフラグ
	for (int nCnt = 0; nCnt < 60000; nCnt++)
	{
		bChange[nCnt] = true;
	}


	// ===== 各頂点の法線の設定 ===== //
	for (int nCntLine = 0; nCntLine < m_nNumDevideZ + 1; nCntLine++)
	{
		for (int nCntRow = 0; nCntRow < m_nNumDevideX + 1; nCntRow++)
		{
			if (nCntLine == 0)
			{// 上端
				if (nCntRow == 0)
				{// 左端

					if (bChange[nIdxVtx] == false)
					{// 前回計算時と同数値ならスキップ
						nIdxVtx++;
						continue;
					}

					// *** 1本目 *** //
					// --- 法線ベクトル --- //
					VecA1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;
					VecA2 = pVtx[nIdxVtx + (m_nNumDevideX + 1) + 1].pos - pVtx[m_nNumDevideX + 1].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norA, &VecA1, &VecA2);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norA, &norA);

					// *** 2本目 *** //
					// --- 法線ベクトル --- //
					VecB1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
					VecB2 = pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos - pVtx[nIdxVtx + 1].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norB, &VecB2, &VecB1);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norB, &norB);

					// --- 全法線の平均を算出 --- //
					norA = (norA + norB) / 2;

					// --- 法線の決定 --- //
					pVtx[nIdxVtx].nor = norA;

					if (pVtx[nIdxVtx].nor == m_NorOld[nIdxVtx])
					{// 前回の計算結果と動数値
						bChange[nIdxVtx] = false;
					}
					else
					{
						bChange[nIdxVtx] = true;
					}

					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存
				}
				else if (nCntRow == m_nNumDevideX)
				{// 右端
					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存

					if (bChange[nIdxVtx] == false)
					{// 前回計算時と同数値ならスキップ
						nIdxVtx++;
						continue;
					}

					// *** 1本目 *** //
					// --- 法線ベクトル --- //
					VecA1 = pVtx[nIdxVtx - 1].pos - pVtx[nIdxVtx].pos;
					VecA2 = pVtx[nIdxVtx + m_nNumDevideX + 1].pos - pVtx[nIdxVtx].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norA, &VecA2, &VecA1);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norA, &norA);

					// --- 法線の決定 --- //
					pVtx[nIdxVtx].nor = norA;

					if (pVtx[nIdxVtx].nor == m_NorOld[nIdxVtx])
					{// 前回の計算結果と動数値
						bChange[nIdxVtx] = false;
					}
					else
					{
						bChange[nIdxVtx] = true;
					}

					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存
				}
				else
				{// 中
					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存

					if (bChange[nIdxVtx] == false)
					{// 前回計算時と同数値ならスキップ
						nIdxVtx++;
						continue;
					}

					// *** 1本目 *** //
					// --- 法線ベクトル --- //
					VecB1 = pVtx[nIdxVtx - 1].pos - pVtx[nIdxVtx].pos;
					VecB2 = pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos - pVtx[nIdxVtx].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norA, &VecB2, &VecB1);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norA, &norA);

					// *** 2本目 *** //
					// --- 法線ベクトル --- //
					VecA1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;
					VecA2 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norB, &VecA1, &VecA2);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norB, &norB);

					// *** 3本目 *** //
					// --- 法線ベクトル --- //
					VecB1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
					VecB2 = pVtx[nIdxVtx + (m_nNumDevideX + 2)].pos - pVtx[nIdxVtx + 1].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norC, &VecB2, &VecB1);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norC, &norC);

					// --- 全法線の平均を算出 --- //
					norA = (norA + norB + norC) / 3;

					// --- 法線の決定 --- //
					pVtx[nIdxVtx].nor = norA;

					if (pVtx[nIdxVtx].nor == m_NorOld[nIdxVtx])
					{// 前回の計算結果と動数値
						bChange[nIdxVtx] = false;
					}
					else
					{
						bChange[nIdxVtx] = true;
					}

					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存
				}
			}
			else if (nCntLine == m_nNumDevideZ)
			{// 下端
				if (nCntRow == 0)
				{// 左端
					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存

					if (bChange[nIdxVtx] == false)
					{// 前回計算時と同数値ならスキップ
						nIdxVtx++;
						continue;
					}

					// *** 1本目 *** //
					// --- 法線ベクトル --- //
					VecA1 = pVtx[nIdxVtx - (m_nNumDevideX + 1)].pos - pVtx[nIdxVtx].pos;
					VecA2 = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norA, &VecA1, &VecA2);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norA, &norA);

					// --- 法線の決定 --- //
					pVtx[nIdxVtx].nor = norA;

					if (pVtx[nIdxVtx].nor == m_NorOld[nIdxVtx])
					{// 前回の計算結果と動数値
						bChange[nIdxVtx] = false;
					}
					else
					{
						bChange[nIdxVtx] = true;
					}

					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存
				}
				else if (nIdxVtx % (m_nNumDevideX + 1) == m_nNumDevideX)
				{// 右端
					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存

					if (bChange[nIdxVtx] == false)
					{// 前回計算時と同数値ならスキップ
						nIdxVtx++;
						continue;
					}

					// *** 1本目 *** //
					// --- 法線ベクトル --- //
					VecA1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - 1].pos;
					VecA2 = pVtx[nIdxVtx - 1 - (m_nNumDevideX + 1)].pos - pVtx[nIdxVtx - 1].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norA, &VecA1, &VecA2);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norA, &norA);

					// *** 2本目 *** //
					// --- 法線ベクトル --- //
					VecB1 = pVtx[nIdxVtx - (m_nNumDevideX + 1) - 1].pos - pVtx[nIdxVtx - (m_nNumDevideX + 1)].pos;
					VecB2 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - (m_nNumDevideX + 1)].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norB, &VecB1, &VecB2);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norB, &norB);

					// --- 全法線の平均を算出 --- //
					norA = (norA + norB) / 2;

					// --- 法線の決定 --- //
					pVtx[nIdxVtx].nor = norA;

					if (pVtx[nIdxVtx].nor == m_NorOld[nIdxVtx])
					{// 前回の計算結果と動数値
						bChange[nIdxVtx] = false;
					}
					else
					{
						bChange[nIdxVtx] = true;
					}

					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存
				}
				else
				{// 中
					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存

					if (bChange[nIdxVtx] == false)
					{// 前回計算時と同数値ならスキップ
						nIdxVtx++;
						continue;
					}

					// *** 1本目 *** //
					// --- 法線ベクトル --- //
					VecA1 = pVtx[nIdxVtx - 1 - (m_nNumDevideX + 1)].pos - pVtx[nIdxVtx - 1].pos;
					VecA2 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - 1].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norA, &VecA1, &VecA2);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norA, &norA);

					// *** 2本目 *** //
					// --- 法線ベクトル --- //
					VecB1 = pVtx[nIdxVtx - (m_nNumDevideX + 1) - 1].pos - pVtx[nIdxVtx - (m_nNumDevideX + 1)].pos;
					VecB2 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - (m_nNumDevideX + 1)].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norB, &VecB2, &VecB1);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norB, &norB);

					// *** 3本目 *** //
					// --- 法線ベクトル --- //
					VecA1 = pVtx[nIdxVtx - (m_nNumDevideX + 1)].pos - pVtx[nIdxVtx].pos;
					VecA2 = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norC, &VecA1, &VecA2);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norC, &norC);

					// --- 全法線の平均を算出 --- //
					norA = (norA + norB + norC) / 3;

					// --- 法線の決定 --- //
					pVtx[nIdxVtx].nor = norA;

					if (pVtx[nIdxVtx].nor == m_NorOld[nIdxVtx])
					{// 前回の計算結果と動数値
						bChange[nIdxVtx] = false;
					}
					else
					{
						bChange[nIdxVtx] = true;
					}

					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存
				}
			}
			else
			{// 中
				if (nIdxVtx % (m_nNumDevideX + 1) == 0)
				{// 左端
					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存

					if (bChange[nIdxVtx] == false)
					{// 前回計算時と同数値ならスキップ
						nIdxVtx++;
						continue;
					}

					// *** 1本目 *** //
					// --- 法線ベクトル --- //
					VecA1 = pVtx[nIdxVtx - (m_nNumDevideX + 1)].pos - pVtx[nIdxVtx].pos;
					VecA2 = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norA, &VecA1, &VecA2);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norA, &norA);

					// *** 2本目 *** //
					// --- 法線ベクトル --- //
					VecA1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;
					VecA2 = pVtx[nIdxVtx + (m_nNumDevideX + 1) + 1].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norB, &VecA1, &VecA2);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norB, &norB);

					// *** 3本目 *** //
					// --- 法線ベクトル --- //
					VecB1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
					VecB2 = pVtx[nIdxVtx + (m_nNumDevideX + 1) + 1].pos - pVtx[nIdxVtx + 1].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norC, &VecB2, &VecB1);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norC, &norC);

					// --- 全法線の平均を算出 --- //
					norA = (norA + norB + norC) / 3;

					// --- 法線の決定 --- //
					pVtx[nIdxVtx].nor = norA;

					if (pVtx[nIdxVtx].nor == m_NorOld[nIdxVtx])
					{// 前回の計算結果と動数値
						bChange[nIdxVtx] = false;
					}
					else
					{
						bChange[nIdxVtx] = true;
					}

					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存
				}
				else if (nIdxVtx % (m_nNumDevideX + 1) == m_nNumDevideX)
				{// 右端
					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存

					if (bChange[nIdxVtx] == false)
					{// 前回計算時と同数値ならスキップ
						nIdxVtx++;
						continue;
					}

					// *** 1本目 *** //
					// --- 法線ベクトル --- //
					VecA1 = pVtx[nIdxVtx - (m_nNumDevideX + 1) - 1].pos - pVtx[nIdxVtx - 1].pos;
					VecA2 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - 1].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norA, &VecA1, &VecA2);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norA, &norA);

					// *** 2本目 *** //
					// --- 法線ベクトル --- //
					VecB1 = pVtx[nIdxVtx - (m_nNumDevideX + 1) - 1].pos - pVtx[nIdxVtx - (m_nNumDevideX + 1)].pos;
					VecB2 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - (m_nNumDevideX + 1)].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norB, &VecB2, &VecB1);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norB, &norB);

					// *** 3本目 *** //
					// --- 法線ベクトル --- //
					VecB1 = pVtx[nIdxVtx - 1].pos - pVtx[nIdxVtx].pos;
					VecB2 = pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos - pVtx[nIdxVtx].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norC, &VecB2, &VecB1);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norC, &norC);

					// --- 全法線の平均を算出 --- //
					norA = (norA + norB + norC) / 3;

					// --- 法線の決定 --- //
					pVtx[nIdxVtx].nor = norA;

					if (pVtx[nIdxVtx].nor == m_NorOld[nIdxVtx])
					{// 前回の計算結果と動数値
						bChange[nIdxVtx] = false;
					}
					else
					{
						bChange[nIdxVtx] = true;
					}

					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存
				}
				else
				{// 中
					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存

					if (bChange[nIdxVtx] == false)
					{// 前回計算時と同数値ならスキップ
						nIdxVtx++;
						continue;
					}

					// ### 法線が合計6本 ### //
					// *** 1本目 *** //
					// --- 法線ベクトル --- //
					VecA1 = pVtx[nIdxVtx - (m_nNumDevideX + 1) - 1].pos - pVtx[nIdxVtx - 1].pos;
					VecA2 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - 1].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norA, &VecA1, &VecA2);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norA, &norA);

					// *** 2本目 *** //
					// --- 法線ベクトル --- //
					VecB1 = pVtx[nIdxVtx - (m_nNumDevideX + 1) - 1].pos - pVtx[nIdxVtx - (m_nNumDevideX + 1)].pos;
					VecB2 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx - (m_nNumDevideX + 1)].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norB, &VecB2, &VecB1);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norB, &norB);

					// *** 3本目 *** //
					// --- 法線ベクトル --- //
					VecB1 = pVtx[nIdxVtx - 1].pos - pVtx[nIdxVtx].pos;
					VecB2 = pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos - pVtx[nIdxVtx].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norC, &VecB2, &VecB1);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norC, &norC);

					// *** 4本目 *** //
					// --- 法線ベクトル --- //
					VecA1 = pVtx[nIdxVtx - (m_nNumDevideX + 1)].pos - pVtx[nIdxVtx].pos;
					VecA2 = pVtx[nIdxVtx + 1].pos - pVtx[nIdxVtx].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norD, &VecA1, &VecA2);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norD, &norD);

					// *** 5本目 *** //
					// --- 法線ベクトル --- //
					VecA1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;
					VecA2 = pVtx[nIdxVtx + (m_nNumDevideX + 1) + 1].pos - pVtx[nIdxVtx + (m_nNumDevideX + 1)].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norE, &VecA1, &VecA2);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norE, &norE);

					// *** 6本目 *** //
					// --- 法線ベクトル --- //
					VecB1 = pVtx[nIdxVtx].pos - pVtx[nIdxVtx + 1].pos;
					VecB2 = pVtx[nIdxVtx + (m_nNumDevideX + 1) + 1].pos - pVtx[nIdxVtx + 1].pos;

					// --- 外積を算出 --- //
					D3DXVec3Cross(&norF, &VecB2, &VecB1);

					// --- 正規化 --- //
					D3DXVec3Normalize(&norF, &norF);

					// --- 全法線の平均を算出 --- //
					norA = (norA + norB + norC + norD + norE + norF) / 6;

					// --- 法線の決定 --- //
					pVtx[nIdxVtx].nor = norA;

					if (pVtx[nIdxVtx].nor == m_NorOld[nIdxVtx])
					{// 前回の計算結果と動数値
						bChange[nIdxVtx] = false;
					}
					else
					{
						bChange[nIdxVtx] = true;
					}

					m_NorOld[nIdxVtx] = pVtx[nIdxVtx].nor;	// 法線の保存
				}
			}

			nIdxVtx++;
		}
	}

	// --- 頂点バッファをアンロックする --- //
	m_pVtxBuff->Unlock();
}

// =================================================
// ステージの起伏読み込み
// =================================================
void CMesh::LoadStage(void)
{
	VERTEX_3D* pVtx;

	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	if (m_pVtxBuff != NULL)
	{
		// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		FILE* pFile;

		pFile = fopen(MESHSTAGE_FILE, "r"); // ファイルを開く

		if (pFile != NULL)
		{// ファイルオープン成功時
			for (int nCnt = 0; nCnt < 6561; nCnt++) {
				(void)fscanf(pFile, "%f %f %f\n", &pVtx[nCnt].pos.x, &pVtx[nCnt].pos.y, &pVtx[nCnt].pos.z);

			}

		}
		else
		{// ファイルが存在しない

		}

		fclose(pFile);

		// --- 頂点バッファをアンロックする --- //
		m_pVtxBuff->Unlock();
	}
}

// =================================================
// ステージの起伏読み込み
// =================================================
void CMesh::LoadStageTitle(void)
{
	VERTEX_3D* pVtx;

	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	if (m_pVtxBuff != NULL)
	{
		// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		FILE* pFile;

		pFile = fopen(MESHSTAGE_FILE, "r"); // ファイルを開く

		if (pFile != NULL)
		{// ファイルオープン成功時
			for (int nCnt = 0; nCnt < 6561; nCnt++) {
				(void)fscanf(pFile, "%f %f %f\n", &pVtx[nCnt].pos.x, &pVtx[nCnt].pos.y, &pVtx[nCnt].pos.z);
			}
		}
		else
		{// ファイルが存在しない

		}

		fclose(pFile);

		// --- 頂点バッファをアンロックする --- //
		m_pVtxBuff->Unlock();
	}
}

// =================================================
// 色の変更
// =================================================
void CMesh::SetColor(D3DXCOLOR col)
{
	VERTEX_3D* pVtx;		// 頂点情報へのポインタ

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntLine = 0; nCntLine < m_nNumDevideZ + 1; nCntLine++)
	{
		for (int nCntRow = 0; nCntRow < m_nNumDevideX + 1; nCntRow++)
		{
			pVtx[0].col = col;

			pVtx++;
		}
	}

	// --- 頂点バッファをアンロックする --- //
	m_pVtxBuff->Unlock();
}

// =================================================
// 頂点座標の変更
// =================================================
void CMesh::SetVtxPos(D3DXVECTOR3 OffSet, int nIdx)
{
	CPlayer* pPlayer = CGame::GetPlayer();

	D3DXVECTOR3 RotPlayer = pPlayer->GetRotation();

	float fLength = sqrtf(m_fSizeX * m_fSizeX + m_fSizeZ * m_fSizeZ) * 0.5f;	// 対角線の長さ
	float fAngle = (float)atan2(m_fSizeZ, m_fSizeX);	// 対角線の角度

	VERTEX_3D* pVtx;		// 頂点情報へのポインタ

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[nIdx].pos = m_pos + OffSet;

	//if (nIdx % 2 == 0)
	//{
	//	pVtx[nIdx].pos.x = sinf(RotPlayer.y + m_fAngle + D3DX_PI) * m_fLength + OffSet.x;
	//	pVtx[nIdx].pos.y = pPlayer->GetPosition().y;
	//	pVtx[nIdx].pos.z = cosf(RotPlayer.y + m_fAngle + D3DX_PI) * m_fLength + OffSet.z;
	//}
	//else if (nIdx % 2 == 1)
	//{
	//	pVtx[nIdx].pos.x = sinf(RotPlayer.y - m_fAngle) * m_fLength + OffSet.x;
	//	pVtx[nIdx].pos.y = pPlayer->GetPosition().y;
	//	pVtx[nIdx].pos.z = cosf(RotPlayer.y - m_fAngle) * m_fLength + OffSet.z;
	//}
	//else if (nIdx % 4 == 2)
	//{
	//	pVtx[nIdx].pos.x = sinf(RotPlayer.y + D3DX_PI - m_fAngle) * m_fLength + OffSet.x;
	//	pVtx[nIdx].pos.y = pPlayer->GetPosition().y;
	//	pVtx[nIdx].pos.z = cosf(RotPlayer.y + D3DX_PI - m_fAngle) * m_fLength + OffSet.z;
	//}
	//else if (nIdx % 4 == 3)
	//{
	//	pVtx[nIdx].pos.x = sinf(RotPlayer.y + m_fAngle) * m_fLength + OffSet.x;
	//	pVtx[nIdx].pos.y = pPlayer->GetPosition().y;
	//	pVtx[nIdx].pos.z = cosf(RotPlayer.y + m_fAngle) * m_fLength + OffSet.z;
	//}

	// --- 頂点バッファをアンロックする --- //
	m_pVtxBuff->Unlock();
}

// =================================================
// 頂点座標の取得
// =================================================
void CMesh::GetVtxPosition(D3DXVECTOR3* pPos1, D3DXVECTOR3* pPos2)
{
	VERTEX_3D* pVtx;		// 頂点情報へのポインタ

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < m_nNumVtx; nCnt++)
	{
		if (nCnt == (m_nNumVtx / 2) * 0.25f)
		{// 特定の頂点座標を取得
			*pPos1 = pVtx[nCnt].pos;
		}

		if (nCnt == (m_nNumVtx / 2) * 0.75f)
		{// 特定の頂点座標を取得
			*pPos2 = pVtx[nCnt].pos;
		}
	}

	// --- 頂点バッファをアンロックする --- //
	m_pVtxBuff->Unlock();
}

