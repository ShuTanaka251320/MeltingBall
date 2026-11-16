// =================================================
// 
// メッシュリング処理[meshring.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _MESHRING_H_
#define _MESHRING_H_

#include"mesh.h"
#include"macro.h"

// =================================================
// マクロ定義

// =================================================
// クラス定義
class CMeshRing :public CMesh
{
public :
	typedef enum
	{
		TYPE_DEF = 0,	// 通常
		TYPE_HIT,		// 壁にヒット時のリアクション用
		TYPE_MAX
	}TYPE;

	CMeshRing();
	~CMeshRing();

	static CMeshRing* Create(D3DXVECTOR3 pos = VEC_NULL, D3DXVECTOR3 rot = VEC_NULL, 
		float fSizeInside = 50.0f, float fSizeOutside = 100.0f,int Devide = 4, 
		CMesh::TYPE type = TYPE_RING, bool bDraw = true, const char* pFileName = NULL);

	HRESULT Init(D3DXVECTOR3 pos = VEC_NULL, D3DXVECTOR3 rot = VEC_NULL,
		float fSizeInside = 50.0f, float fSizeOutside = 100.0f, int Devide = 4,
		CMesh::TYPE type = TYPE_RING, bool bDraw = true, const char* pFileName = NULL);
	void Uninit();
	void Update();
	void Draw();

	void SetType(TYPE type) { m_type = type; };
	void SetExpansion(bool bSet) { m_bExpand = bSet; };
	void SetReduction(bool bSet) { m_bReduction = bSet; };

	void SetNor(D3DXVECTOR3 rot);
	void SetExpansionSpan(float fSpan) { m_fSpanExpand = fSpan; };
	void SetReductionSpan(float fSpan) { m_fSpanReduction = fSpan; };
	void SetColor(D3DXCOLOR col);
	void FlashRing(float fSpan);

private:
	void Expansion(float fSpan);
	void Reduction(float fSpan);
	void DecreaseAlpha(float fSpan);

	D3DXCOLOR m_col;	// 色
	int m_nIdxTexture;	// テクスチャのインデックス
	int m_nDevideX;	// Xの分割数
	int m_nDevideZ;	// Zの分割数
	float m_fSpanExpand;	// 拡大するスパン
	float m_fSpanReduction;	// 縮小するスパン
	bool m_bExpand;	// 拡大フラグ
	bool m_bReduction;	// 縮小フラグ
	int m_nCounterFlash;	// 点滅カウンター
	TYPE m_type;	// リングの用途タイプ
};

#endif