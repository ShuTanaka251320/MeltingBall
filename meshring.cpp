// =================================================
// 
// メッシュリング処理[meshring.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "main.h"
#include "meshring.h"
#include "manager.h"
#include "input.h"
#include "macro.h"
#include "debugproc.h"
#include "texture.h"
#include "player.h"
#include "game.h"
#include "camera.h"
#include "stagemodel.h"
#include "player.h"
#include "scene.h"
#include <assert.h>
#include <stdio.h>

// =================================================
// マクロ定義

// =================================================
// コンストラクタ
// =================================================
CMeshRing::CMeshRing()
{
	m_nIdxTexture = 0;	// テクスチャのインデックス
	m_nDevideX = 0;		// Xの分割数
	m_nDevideZ = 0;		// Zの分割数
	m_fSpanExpand = 0.0f;	// 拡大するスパン
	m_fSpanReduction = 0.0f;	// 縮小するスパン
	m_bExpand = false;	// 拡大フラグ
	m_bReduction = false;	// 縮小フラグ
	m_col = COLOR_WHITE;	// 色
	m_type = TYPE_DEF;		// タイプ
}

// =================================================
// デストラクタ
// =================================================
CMeshRing::~CMeshRing()
{

}

// =================================================
// メッシュリングの生成処理
// =================================================
CMeshRing* CMeshRing::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSizeInside, float fSizeOutside,
	int Devide, CMesh::TYPE type, bool bDraw, const char* pFileName)
{
	CMeshRing* pMeshRing = new CMeshRing;	// メッシュフィールドのインスタンス生成

	// --- 初期化 --- //
	pMeshRing->Init(pos, rot, fSizeInside, fSizeOutside, Devide, type, bDraw, pFileName);

	return pMeshRing;
}

// =================================================
// メッシュリングの初期化処理
// =================================================
HRESULT CMeshRing::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSizeInside, float fSizeOutside,
	int Devide, CMesh::TYPE type, bool bDraw, const char* pFileName)
{
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	int nNumVtx = Devide * 2;	// 頂点数
	int nNumIdx = nNumVtx + 2;	// インデックス数

	// --- メッシュの初期化処理 --- //
	CMesh::Init(pos, rot, fSizeInside, fSizeOutside, Devide, nNumVtx, nNumIdx, type, bDraw);

	// --- テクスチャの設定 --- //
	if (pFileName != NULL)
	{
		m_nIdxTexture = pTexture->Register(pFileName);
	}
	else
	{
		m_nIdxTexture = -1;
	}

	SetTexture(m_nIdxTexture);

	m_bExpand = false;	// 拡大フラグ
	m_bReduction = false;	// 縮小フラグ

	return S_OK;
}

// =================================================
// メッシュリングの終了処理
// =================================================
void CMeshRing::Uninit(void)
{
	CMesh::Uninit();
}

// =================================================
// メッシュリングの更新処理
// =================================================
void CMeshRing::Update()
{
	// --- 変数宣言 --- //
	CDebugProc* pDebugProc = CManager::GetDrbugProc();	// デバッグ表示のポインタ

	float fInRadius = GetInRadius();
	float fOutRadius = GetOutRadius();

	// --- 拡縮処理 --- //
	if (m_bExpand == true)
	{// 拡大
		Expansion(m_fSpanExpand);
		DecreaseAlpha(0.02f);
	}
	if (m_bReduction == true)
	{// 縮小
		Reduction(m_fSpanReduction);
		DecreaseAlpha(0.02f);
	}

	// --- 頂点座標更新処理 --- //
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	VERTEX_3D* pVtx;

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	int nIdxVtx = 0;

	// --- 頂点座標の設定 --- //
	for (int nCnt = 0; nCnt < GetDevideSurcle(); nCnt++)
	{// 外側頂点の設定
		float fAngle = 2.0f * D3DX_PI * nCnt / GetDevideSurcle();	// 中心(m_pos)空の角度

		float c = cosf(fAngle);
		float s = sinf(fAngle);

		pVtx[0].pos = { s * fOutRadius, 0.0f,  c * fOutRadius };

		pVtx++;
	}

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < GetDevideSurcle(); nCnt++)
	{// 内側頂点の設定
		float fAngle = 2.0f * D3DX_PI * nCnt / GetDevideSurcle();	// 中心(m_pos)空の角度

		float c = cos(fAngle);
		float s = sin(fAngle);

		pVtx[GetDevideSurcle()].pos = { s * fInRadius,0.0f,  c * fInRadius };

		pVtx++;
	}

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();

	CMesh::Update();

	// --- 各種条件での終了処理 --- //
	if (m_col.a <= 0.0f)
	{// 透明になったら終了処理
		Uninit();
	}
	else if (GetOutRadius() <= 0.0f)
	{// 外円の半径が一定以下で終了処理
		Uninit();
	}
	else if (GetOutRadius() >= 1000.0f)
	{// 外円の半径が一定以上で終了処理
		Uninit();
	}
}

// =================================================
// メッシュリングの描画処理
// =================================================
void CMeshRing::Draw(void)
{
	CMesh::Draw();
}

// =================================================
// リングの拡大処理
// =================================================
void CMeshRing::Expansion(float fSpan)
{
	int nIdx = 0;	// 頂点座標のインデックス
	float ExpandInRadius = GetInRadius();		// 拡大していく値を保存
	float ExpandOutRadius = GetOutRadius();	// 拡大していく値を保存

	ExpandInRadius = ExpandInRadius + fSpan;
	ExpandOutRadius = ExpandOutRadius + fSpan;

	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	VERTEX_3D* pVtx;

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// --- 頂点座標の設定 --- //
	for (int nCnt = 0; nCnt < GetDevideX(); nCnt++)
	{// 外側頂点の設定
		float fAngle = 2.0f * D3DX_PI * nCnt / GetDevideX();	// 中心(m_pos)空の角度

		float c = cosf(fAngle);
		float s = sinf(fAngle);

		pVtx[0].pos = { s * ExpandOutRadius, GetPosition().y, c * ExpandOutRadius };

		pVtx++;
	}

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < GetDevideX(); nCnt++)
	{// 内側頂点の設定
		float fAngle = 2.0f * D3DX_PI * nCnt / GetDevideX();	// 中心(m_pos)空の角度

		float c = cosf(fAngle);
		float s = sinf(fAngle);

		pVtx[GetDevideX()].pos = { s * ExpandInRadius, GetPosition().y, c * ExpandInRadius };

		pVtx++;
	}

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();

	// --- 半径の変更を反映 --- //
	SetInRadius(ExpandInRadius);
	SetOutRadius(ExpandOutRadius);
}

// =================================================
// リングの縮小処理
// =================================================
void CMeshRing::Reduction(float fSpan)
{
	int nIdx = 0;	// 頂点座標のインデックス
	float ReductInRadius = GetInRadius();		// 拡大していく値を保存
	float ReductOutRadius = GetOutRadius();	// 拡大していく値を保存

	ReductInRadius = ReductInRadius - fSpan;
	ReductOutRadius = ReductOutRadius - fSpan;

	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	VERTEX_3D* pVtx;

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// --- 頂点座標の設定 --- //
	for (int nCnt = 0; nCnt < GetDevideX(); nCnt++)
	{// 外側頂点の設定
		float fAngle = 2.0f * D3DX_PI * nCnt / GetDevideX();	// 中心(m_pos)空の角度

		float c = cosf(fAngle);
		float s = sinf(fAngle);

		pVtx[0].pos = { s * ReductOutRadius, GetPosition().y, c * ReductOutRadius };

		pVtx++;
	}

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < GetDevideX(); nCnt++)
	{// 内側頂点の設定
		float fAngle = 2.0f * D3DX_PI * nCnt / GetDevideX();	// 中心(m_pos)空の角度

		float c = cosf(fAngle);
		float s = sinf(fAngle);

		pVtx[GetDevideX()].pos = { s * ReductInRadius, GetPosition().y, c * ReductInRadius };

		pVtx++;
	}

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();

	// --- 半径の変更を反映 --- //
	SetInRadius(ReductInRadius);
	SetOutRadius(ReductOutRadius);
}

// =================================================
// リングの色設定
// =================================================
void CMeshRing::SetColor(D3DXCOLOR col)
{
	int nIdx = 0;	// 頂点座標のインデックス

	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	VERTEX_3D* pVtx;

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// --- 頂点座標の設定 --- //
	for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++)
	{// 外側頂点の設定
		pVtx[0].col = col;

		pVtx++;
	}

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();

	m_col = col;
}

// =================================================
// リングの透明度処理
// =================================================
void CMeshRing::DecreaseAlpha(float fSpan)
{
	m_col.a -= fSpan;

	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	VERTEX_3D* pVtx;

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// --- 頂点座標の設定 --- //
	for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++)
	{// 外側頂点の設定
		pVtx[0].col = D3DXCOLOR(m_col.r, m_col.g, m_col.b, m_col.a);

		pVtx++;
	}

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();
}

// =================================================
// リングの点滅処理
// =================================================
void CMeshRing::FlashRing(float fSpan)
{
	m_nCounterFlash++;	// 頂点座標のインデックス

	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	VERTEX_3D* pVtx;

	if (m_nCounterFlash % 10 > 4)
	{
		// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
		pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// --- 頂点座標の設定 --- //
		for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++)
		{// 外側頂点の設定
			pVtx[0].col = D3DXCOLOR(m_col.r, m_col.g, m_col.b, 0.0f);

			pVtx++;
		}

		// --- 頂点バッファをアンロックする --- //
		pVtxBuff->Unlock();
	}
	else
	{
		// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
		pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// --- 頂点座標の設定 --- //
		for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++)
		{// 外側頂点の設定
			pVtx[0].col = D3DXCOLOR(m_col.r, m_col.g, m_col.b, m_col.a);

			pVtx++;
		}

		// --- 頂点バッファをアンロックする --- //
		pVtxBuff->Unlock();
	}
}

// =================================================
// 法線の設定
// =================================================
void CMeshRing::SetNor(D3DXVECTOR3 nor)
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	VERTEX_3D* pVtx;

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++)
	{
		pVtx[0].nor = nor;

		pVtx++;
	}

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();
}
