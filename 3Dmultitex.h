// =================================================
// 
// 3Dマルチテクスチャポリゴン処理[3DMultiTex.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _3DMULTITEX_H_
#define _3DMULTITEX_H_

#include"object3DMultiTex.h"
#include"macro.h"

// =================================================
// マクロ定義

// =================================================
// クラス定義
class C3DMultiTex :public CObject3DMultiTex
{
public:
	C3DMultiTex();
	~C3DMultiTex();

	static C3DMultiTex* Create(D3DXVECTOR3 pos = VEC_NULL, D3DXVECTOR3 rot = VEC_NULL, D3DXVECTOR3 move = VEC_NULL, float fSizeX = 50.0f, float fSizeZ = 50.0f, const char* pFileName[10] = {});

	HRESULT Init(D3DXVECTOR3 pos = VEC_NULL, D3DXVECTOR3 rot = VEC_NULL, D3DXVECTOR3 move = VEC_NULL, float fSizeX = 50.0f, float fSizeZ = 50.0f, const char* pFileName[10] = {});
	void Uninit();
	void Update();
	void Draw();

private:

};

#endif