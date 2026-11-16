// =================================================
// 
// Xモデル処理[Xmodel.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _XMODEL_H_
#define _XMODEL_H_

#include"main.h"
#include<string>

using namespace std;

// =================================================
// マクロ定義
#define MAX_MODEL			(128)	// テクスチャの最大数
#define MAX_CHAR_MODEL	(128)	// 文字の最大数

// =================================================
// クラス定義
class CXModel
{
public:
	CXModel();
	~CXModel();

	HRESULT Load(void);
	 void Unload(void);

	int Register(const char *pFileName);
	 LPDIRECT3DTEXTURE9 GetAddress(int nIdx);

	 D3DXVECTOR3 GetVtxMax(int nIdx) {return m_vtxMax[nIdx];};
	 D3DXVECTOR3 GetVtxMin(int nIdx) {return m_vtxMin[nIdx];};

	 LPD3DXMESH GetMesh(int nIdx) { return m_pMesh[nIdx]; };
	 LPD3DXBUFFER GetBuffMat(int nIdx) { return m_pBuffMat[nIdx]; };
	 DWORD GetNumMaterial(int nIdx) { return m_dwNumMat[nIdx]; };
	 int GetMaterialtex(int nIdx, int nCnt) { return m_nIdxtex[nIdx][nCnt]; }

private:
	D3DXVECTOR3 m_vtxMin[MAX_MODEL], m_vtxMax[MAX_MODEL];	// モデルの最小値、最大値
	int m_nIdxtex[MAX_MODEL][32];			// パーツごとのテクスチャのインデックス
	LPD3DXMESH m_pMesh[MAX_MODEL];			// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER m_pBuffMat[MAX_MODEL];	// マテリアルのポインタ
	DWORD m_dwNumMat[MAX_MODEL];			// マテリアルの数

	char *m_apModelName[MAX_CHAR_MODEL];		// モデルの名前アドレスを保存
	string m_aModelName[MAX_MODEL];		// モデルの名前を保存
	static int m_nNumAll;	// モデルの総数
};

#endif