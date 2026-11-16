// =================================================
// 
// 影の処理[shadow.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _SHADOW_H_
#define _SHADOW_H_

#include"object3D.h"
#include"macro.h"

// =================================================
// 影のクラス定義
class CShadow : public CObject3D
{
public:
	CShadow(int nPriority = 6);
	~CShadow();

	static CShadow* Create(D3DXVECTOR3 pos = VEC_NULL, D3DXVECTOR3 rot = VEC_NULL, D3DXVECTOR3 move = VEC_NULL, D3DXCOLOR col = COLOR_WHITE, float fWidth = 15.0f, float fHeight = 15.0f, const char* pFileName = NULL);
	HRESULT Init(void) { return S_OK; };
	HRESULT Init(D3DXVECTOR3 pos = VEC_NULL, D3DXVECTOR3 rot = VEC_NULL, D3DXVECTOR3 move = VEC_NULL, D3DXCOLOR col = COLOR_WHITE, float fWidth = 15.0f, float fHeight = 15.0f, const char* pFileName = NULL);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; };

private:
	D3DXVECTOR3 m_pos;	// 位置
	int m_nIdxTex;		// テクスチャのインデックス
};

#endif