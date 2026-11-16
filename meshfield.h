// =================================================
// 
// メッシュフィールド処理[meshfield.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _MESHFIELD_H_
#define _MESHFIELD_H_

#include"mesh.h"
#include"macro.h"

// =================================================
// マクロ定義
#define MESHSTAGE_FILE			"data\\Scripts\\field.txt"		// 読み込むメッシュフィールドファイル名
#define MESHSTAGESAVE_FILE		"data\\Scripts\\FieldEdit.txt"	// 保存するメッシュフィールドファイル名

// =================================================
// クラス定義
class CMeshField :public CMesh
{
public :
	CMeshField();
	~CMeshField();

	static CMeshField* Create(D3DXVECTOR3 pos = VEC_NULL, D3DXVECTOR3 rot = VEC_NULL, float fSizeX = 50.0f, float fSizeZ = 50.0f,
		float fMaxXSize = 100.0f, float fMaxZSize = 100.0f,	int DevideX = 2, int DevideZ = 2, 
		CMesh::TYPE type = TYPE_FIELD, bool bDraw = true, const char* pFileName = NULL);

	HRESULT Init(D3DXVECTOR3 pos = VEC_NULL, D3DXVECTOR3 rot = VEC_NULL, float fSizeX = 50.0f, float fSizeZ = 50.0f, 
		float fMaxXSize = 100.0f, float fMaxZSize = 100.0f, int DevideX = 2, int DevideZ = 2,
		CMesh::TYPE type = TYPE_FIELD, bool bDraw = true, const char* pFileName = NULL);
	void Uninit();
	void Update();
	void Draw();

	float GetPosHeight(D3DXVECTOR3 pPos, D3DXVECTOR3 pPosOld);
	void DecreaseHeight(D3DXVECTOR3 pos, D3DXVECTOR3 posOld, D3DXVECTOR3 scl);
	void IncreaseHeight(D3DXVECTOR3 pos, D3DXVECTOR3 posOld);
	void ResetHeight(void);
	void DownHeight(void);
	void CollRing(void);

	void SetDecreaseSpan(float fSpan) { m_fDecreaseSpan = fSpan; };
	void SetIncreaseSpan(float fSpan) { m_fIncreaseSpan = fSpan; };
	void Save(void);
	void Load(void);
	void EndSave(void);
	void LoadEndSave(void);

private:

	int m_nIdxTexture;	// テクスチャのインデックス

	int m_nDevideX;	// Xの分割数
	int m_nDevideZ;	// Zの分割数

	float m_fDecreaseSpan;	// 頂点座標の下げる数値
	float m_fIncreaseSpan;	// 頂点座標の上げる数値
};

#endif