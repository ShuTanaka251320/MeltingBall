// =================================================
// 
// 3Dマルチテクスチャポリゴン処理[3DMultiTex.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "main.h"
#include "3DMultiTex.h"
#include "Object3DMultiTex.h"
#include "manager.h"
#include "input.h"
#include "macro.h"
#include "debugproc.h"
#include "texture.h"
#include <assert.h>
#include <stdio.h>

// =================================================
// マクロ定義

// =================================================
// コンストラクタ
// =================================================
C3DMultiTex::C3DMultiTex()
{

}

// =================================================
// デストラクタ
// =================================================
C3DMultiTex::~C3DMultiTex()
{

}

// =================================================
// メッシュフィールドの生成処理
// =================================================
C3DMultiTex* C3DMultiTex::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, float fSizeX, float fSizeZ, const char* pFileName[10])
{
	C3DMultiTex* p3DMultiTex = new C3DMultiTex;	// メッシュフィールドのインスタンス生成

	// --- 初期化 --- //
	p3DMultiTex->Init(pos, rot, move, fSizeX, fSizeZ, &pFileName[0]);

	return p3DMultiTex;
}

// =================================================
// メッシュフィールドの初期化処理
// =================================================
HRESULT C3DMultiTex::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, float fSizeX, float fSizeZ, const char* pFileName[10])
{
	// --- メッシュの初期化処理 --- //
	CObject3DMultiTex::Init(pos, rot, move, COLOR_WHITE, fSizeX, fSizeZ, CObject::TYPE_BG, pFileName);

	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	int nIdx = NULL;

	for (int nCnt = 0; nCnt < 10; nCnt++, pFileName++)
	{
		if (pFileName[0] != NULL)
		{
			nIdx = pTexture->Register(pFileName[0]);
			SetTexture(nCnt, nIdx);
		}
	}

	return S_OK;
}

// =================================================
// メッシュフィールドの終了処理
// =================================================
void C3DMultiTex::Uninit(void)
{
	CObject3DMultiTex::Uninit();
}

// =================================================
// メッシュフィールドの更新処理
// =================================================
void C3DMultiTex::Update()
{
	CObject3DMultiTex::Update();
}

// =================================================
// メッシュフィールドの描画処理
// =================================================
void C3DMultiTex::Draw(void)
{
	CObject3DMultiTex::Draw();
}
