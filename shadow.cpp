// =================================================
// 
// 影の処理[shadow.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "shadow.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

// =================================================
// マクロ定義
#define SHADOW_XSIZE	(5)		// 影のXサイズ
#define SHADOW_ZSIZE	(5)		// 影のZサイズ(奥行き)
#define MAX_SHADOW		(2048)	// 影数

// =================================================
// グローバル変数
LPDIRECT3DTEXTURE9	g_pTextureShadow = NULL;		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9	g_pVtxBuffShadow = NULL;	// 頂点バッファへのポインタ

// =================================================
// コンストラクタ
// =================================================
CShadow::CShadow(int nPriority) :CObject3D(nPriority)
{
	m_pos = VEC_NULL;
	m_nIdxTex = 0;
}

// =================================================
// デストラクタ
// =================================================
CShadow::~CShadow()
{

}

// =================================================
// 生成処理
// =================================================
CShadow* CShadow::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, const char* pFileName)
{
	CShadow* pShadow;

	pShadow = new CShadow;

	pShadow->Init(pos, rot, move, col, fWidth, fHeight, pFileName);

	return pShadow;
}

// =================================================
// 初期化処理
// =================================================
HRESULT CShadow::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, const char* pFileName)
{
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	CObject3D::Init(pos, rot, move, col, fWidth, fHeight, CObject::TYPE_SHADOW, pFileName);

	m_pos = pos;

	if (pFileName != NULL)
	{
		// --- テクスチャ登録 --- //
		m_nIdxTex = pTexture->Register(pFileName);
	}
	else
	{
		m_nIdxTex = -1;
	}

	CObject3D::SetTexture(m_nIdxTex);

	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CShadow::Uninit(void)
{
	CObject3D::Uninit();
}

// =================================================
//更新処理
// =================================================
void CShadow::Update(void)
{
	// --- 位置の反映 --- //
	CObject3D::SetPosition(m_pos);

	// --- Object3Dの更新処理 --- //
	CObject3D::Update();
}

// =================================================
// 描画処理
// =================================================
void CShadow::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	// αブレンディングを減算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// Zテストを無効にする
	//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	CObject3D::Draw();

	// Zテストを有効にする
	//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// αブレンディングをもとに戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
