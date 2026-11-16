// =================================================
// 
// ステージモデル管理処理[stagemodel.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _STAGEMODEL_H_
#define _STAGEMODEL_H_

#include"main.h"
#include"objectX.h"
#include"game.h"
#include"player.h"
#include"stageobj.h"

// =================================================
// マクロ定義
#define STAGEMODEL_PRI	(5)		// モデルの優先順位
#define MAX_MODEL	(512)		// モデルの最大数

// =================================================
// 前方宣言
class CStageObj;

// =================================================
// クラス定義
class CStageModel
{
public:

	CStageModel(int nPriority = STAGEMODEL_PRI);
	~CStageModel();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	bool Collision(D3DXVECTOR3* pPos, D3DXVECTOR3 pPosOld, D3DXVECTOR3* pMove);
	bool Collision(D3DXVECTOR3* pPos, D3DXVECTOR3 pPosOld, D3DXVECTOR3* pShadowHeight, CObject::TYPE Objtype);
	static HRESULT LoadStage(void);

	static int GetNumAll(void) { return m_nNumAll; };
	static void DecreaseNumModel(void) { m_nNumAll--; }
	static void IncreaseNumModel(void) { m_nNumAll++; }
	static CStageObj* GetStageObj(int nIdx) { return m_pStageObj[nIdx]; };

private:
	bool Coll(float fVecPos, float fVecPosOld, D3DXVECTOR3 VecLine, D3DXVECTOR3 vec, D3DXVECTOR3 VtxMin, float fRate, D3DXVECTOR3 *pPos, D3DXVECTOR3 posVtx[4], int nIdx);

	static HRESULT Load(void);
	
	static int m_nNumAll;	// モデルの総数
	static CStageObj* m_pStageObj[MAX_MODEL];		// ステージ設置モデル
};

#endif