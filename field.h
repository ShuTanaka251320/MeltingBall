// =================================================
// 
// フィールド処理[field.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _FIELD_H_
#define _FIELD_H_

#include"meshfield.h"
#include"macro.h"

// =================================================
// マクロ定義

// =================================================
// クラス定義
class CField :public CMeshField
{
public:
	typedef enum
	{
		TYPE_BOTTOM = 0,	// 最下層フィールド
		TYPE_SNOW,			// 雪のフィールド
		TYPE_RIGHTHEAT,		// 右のロードヒーティング
		TYPE_LEFTHEAT,		// 左のロードヒーティング
		TYPE_CENTERHEAT,		// 左のロードヒーティング
		TYPE_MAX
	}TYPE;

	CField();
	~CField();

	static CField* Create(D3DXVECTOR3 pos = VEC_NULL, D3DXVECTOR3 rot = VEC_NULL, float fSizeX = 50.0f, float fSizeZ = 50.0f,
		float fMaxXSize = 100.0f, float fMaxZSize = 100.0f, int DevideX = 2, int DevideZ = 2,
		CMesh::TYPE type = TYPE_FIELD, bool bDraw = true, const char* pFileName = NULL);

	HRESULT Init(D3DXVECTOR3 pos = VEC_NULL, D3DXVECTOR3 rot = VEC_NULL, float fSizeX = 50.0f, float fSizeZ = 50.0f,
		float fMaxXSize = 100.0f, float fMaxZSize = 100.0f, int DevideX = 2, int DevideZ = 2,
		CMesh::TYPE type = TYPE_FIELD, bool bDraw = true, const char* pFileName = NULL);
	void Uninit();
	void Update();void Draw();

	int GetPersentHeight(void);

private:
	void CreateStage(void);
};

#endif