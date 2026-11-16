// =================================================
// 
// モデル処理[model.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _MODEL_H_
#define _MODEL_H_

#include"object.h"

// =================================================
// マクロ定義
#define MODEL_PRI	(6)		// モデルの優先順位
#define MAX_PARTS	(32)	// パーツの最大数
#define MAX_MAT		(32)	// マテリアルの最大数

// =================================================
// クラス定義
class CModel
{
public:
	CModel();
	~CModel();

	static CModel* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot,int nIdx, int nIdxParent, const char *ModelFileName);
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdx, int nIdxParent, const char* ModelFileName);
	void Uninit(void);
	void Update(void);
	void Draw(const D3DXMATRIX *pMtxParent = NULL);
	void ModelRotate(float fRotDest);

	void SetParent(CModel* pModel){ m_pParent = pModel; };
	D3DXMATRIX GetMtxWorld(void) { return m_mtxWorld; };
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; };
	void SetRotation(D3DXVECTOR3 rot) { m_rot = rot; };
	D3DXVECTOR3 GetPosition(void) { return m_pos; };
	D3DXVECTOR3 GetRotation(void) { return m_rot; };

	D3DXVECTOR3 GetOffSetPosition(void) { return m_posOffSet; };
	D3DXVECTOR3 GetOffSetRotation(void) { return m_rotOffSet; };
	D3DXVECTOR3 GetVtxMinSize(void) { return m_vtxMin; };
	D3DXVECTOR3 GetVtxMaxSize(void) { return m_vtxMax; };

private:
	CModel* m_pParent;			// 親のモデルのポインタ
	LPD3DXMESH m_pMesh;			// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER m_pBuffMat;	// マテリアルのポインタ
	DWORD m_dwNumMat;			// マテリアルの数
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 m_vtxMin, m_vtxMax;	// モデルの最小値、最大値
	int m_nIdx;					// 自分のインデックス
	int m_nIdxParent;			// 親のインデックス

	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_posOffSet;	// 位置(オフセット)
	D3DXVECTOR3 m_rot;			// 向き
	D3DXVECTOR3 m_rotOffSet;	// 向き(オフセット)
	D3DXVECTOR3 m_rotDest;		// 目標の向き
	D3DXVECTOR3 m_rotDiff;		// 目標と現在の向きの差分
	D3DXVECTOR3 m_move;			// 移動量
	D3DXCOLOR m_col;			// 色

	int m_nIdxtex[MAX_MAT];		// パーツごとのテクスチャのインデックス
	bool m_bDisp;					// 表示するかどうか
};

#endif