// =================================================
// 
// Xオブジェクト処理[objectX.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _OBJECTX_H_
#define _OBJECTX_H_

#include"main.h"
#include"object.h"

// =================================================
// マクロ定義
#define OBJX_PRI	(5)		// モデルの優先順位

// =================================================
// クラス定義
class CObjectX :public CObject
{
public:
	CObjectX(int nPriority = OBJX_PRI);
	~CObjectX();

	static CObjectX* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, CObject::TYPE type, const char* ModelFileName);
	HRESULT Init(void) { return S_OK; };
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, CObject::TYPE type, const char* ModelFileName);
	float Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, CObject::TYPE type, bool bCollider = true, const char* ModelFileName = NULL);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Draw(int type);
	void NorRotate(float fRotDest);

	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; };
	void SetRotation(D3DXVECTOR3 rot) { m_rot = rot; };

	// --- テクスチャの指定 --- //
	void SetTexture(int nIdx) { m_nIdxtex[nIdx] = nIdx; };

	// --- メンバ変数の取得、設定 --- //
	D3DXVECTOR3 GetPosition(void) { return m_pos; };
	D3DXVECTOR3 GetRotation(void) { return m_rot; };
	D3DXVECTOR3 GetVtxMin(void) { return m_vtxMin; };
	D3DXVECTOR3 GetVtxMax(void) { return m_vtxMax; };
	D3DXMATRIX *GetWorldMtx(void) { return &m_mtxWorld; };

	LPD3DXMESH GetMesh(void) { return m_pMesh; };

private:
	LPD3DXMESH m_pMesh;			// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER m_pBuffMat;	// マテリアルのポインタ
	DWORD m_dwNumMat;			// マテリアルの数
	int m_nIdxModelParent;		// 親のインデックス
	D3DXVECTOR3 m_pos;			// 位置(オフセット)
	D3DXVECTOR3 m_rot;			// 向き
	D3DXVECTOR3 m_rotDest;		// 目標の向き
	D3DXVECTOR3 m_rotDiff;		// 目標と現在の向きの差分
	D3DXVECTOR3 m_move;			// 移動量
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 m_vtxMin, m_vtxMax;	// モデルの最小値、最大値
	bool m_bDisp;					// 表示するかどうか
	int m_nIdxtex[MAX_MAT];			// パーツごとのテクスチャのインデックス
};

#endif