// =================================================
// 
// メッシュシリンダー処理[meshcylinder.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _MESHCYLINDER_H_
#define _MESHCYLINDER_H_

#include"mesh.h"
#include"macro.h"

// =================================================
// マクロ定義

// =================================================
// クラス定義
class CMeshCylinder :public CMesh
{
public :
	CMeshCylinder();
	~CMeshCylinder();

	static CMeshCylinder* Create(D3DXVECTOR3 pos = VEC_NULL, D3DXVECTOR3 rot = VEC_NULL,
		float fSizeSurcle = 50.0f, float fSizeHeight = 100.0f,int nDevideSurcle = 4, int nDevideHeight = 1,
		CMesh::TYPE type = TYPE_CYLINDER, bool bDraw = true, bool bDrawOutside = true, const char* pFileName = NULL);

	HRESULT Init(D3DXVECTOR3 pos = VEC_NULL, D3DXVECTOR3 rot = VEC_NULL,
		float fSizeSurcle = 50.0f, float fSizeHeight = 100.0f, int nDevideSurcle = 4, int nDevideHeight = 1,
		CMesh::TYPE type = TYPE_CYLINDER, bool bDraw = true, bool bDrawOutside = true, const char* pFileName = NULL);

	void Uninit();
	void Update();
	void Draw();

private:
	int m_nIdxTexture;	// テクスチャのインデックス

	int m_nDevide;	// 円の分割数
	int m_nDevideY;	// 高さの分割数
	bool m_bDrawOutSide; // 外側の描画
};

#endif