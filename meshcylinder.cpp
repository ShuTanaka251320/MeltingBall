// =================================================
// 
// メッシュシリンダー処理[meshcylinder.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "main.h"
#include "meshcylinder.h"
#include "manager.h"
#include "input.h"
#include "macro.h"
#include "debugproc.h"
#include "texture.h"
#include "player.h"
#include "renderer.h"
#include <assert.h>
#include <stdio.h>

// =================================================
// マクロ定義

// =================================================
// コンストラクタ
// =================================================
CMeshCylinder::CMeshCylinder()
{
	m_nIdxTexture = 0;	// テクスチャのインデックス
	m_nDevide = 0;		// 円の分割数
	m_nDevideY = 0;		// 縦の分割数
}

// =================================================
// デストラクタ
// =================================================
CMeshCylinder::~CMeshCylinder()
{

}

// =================================================
// メッシュフィールドの生成処理
// =================================================
CMeshCylinder* CMeshCylinder::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSizeInside, float fSizeOutside,
	int nDevideSurcle, int nDevideHeight, CMesh::TYPE type, bool bDraw, bool bDrawOutside, const char* pFileName)
{
	CMeshCylinder* pMeshCylinder = new CMeshCylinder;	// メッシュフィールドのインスタンス生成

	// --- 初期化 --- //
	pMeshCylinder->Init(pos, rot, fSizeInside, fSizeOutside, nDevideSurcle, nDevideHeight, type, bDraw, bDrawOutside, pFileName);

	return pMeshCylinder;
}

// =================================================
// メッシュフィールドの初期化処理
// =================================================
HRESULT CMeshCylinder::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSizeSurcle, float fSizeHeight,
	int nDevideSurcle, int nDevideHeight, CMesh::TYPE type, bool bDraw, bool bDrawOutside, const char* pFileName)
{
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	int nNumVtx = nDevideSurcle * (nDevideHeight + 1);	// 頂点数
	int nNumIdx = nNumVtx + 2 * nDevideHeight + nDevideSurcle * 4;	// インデックス数
	nNumIdx = nDevideHeight * (nDevideSurcle + 2) + (nDevideHeight * nDevideSurcle);
	m_bDrawOutSide = bDrawOutside;

	// --- メッシュの初期化処理 --- //
	CMesh::Init(pos, rot, fSizeSurcle, fSizeHeight, nDevideSurcle, nDevideHeight, nNumVtx, nNumIdx, type, bDraw, bDrawOutside);

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

	return S_OK;
}

// =================================================
// メッシュフィールドの終了処理
// =================================================
void CMeshCylinder::Uninit(void)
{
	CMesh::Uninit();

}

// =================================================
// メッシュフィールドの更新処理
// =================================================
void CMeshCylinder::Update()
{
	// --- 変数宣言 --- //
	CDebugProc* pDebugProc = CManager::GetDrbugProc();	// デバッグ表示のポインタ

	CMesh::Update();
}

// =================================================
// メッシュフィールドの描画処理
// =================================================
void CMeshCylinder::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	if (m_bDrawOutSide == false)
	{

		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	}

	CMesh::Draw();

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
