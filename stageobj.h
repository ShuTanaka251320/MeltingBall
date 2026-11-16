// =================================================
// 
// ステージモデル処理[stageobj.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _STAGEOBJ_H_
#define _STAGEOBJ_H_

#include"main.h"
#include"objectX.h"
#include"game.h"
#include"player.h"
#include"stagemodel.h"

// =================================================
// マクロ定義
#define STAGEOBJ_PRI	(5)		// モデルの優先順位

// =================================================
// 前方宣言
class CMeshRing;

// =================================================
// クラス定義
class CStageObj :public CObjectX
{
public:
	typedef enum
	{
		TYPE_STAGE = 0,	// ステージ
		TYPE_FENCE,		// 柵
		TYPE_SNOWMAN,	// 雪だるま
		TYPE_HYDRANT,	// 消火栓
		TYPE_LIGHT,	// 街灯
		TYPE_BOARD,	// 看板
		TYPE_CORN,	// 三角コーン
		TYPE_MAX
	}TYPE;

	CStageObj(int nPriority = STAGEOBJ_PRI);
	~CStageObj();

	static CStageObj* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, TYPE type, const char* ModelFileName);
	HRESULT Init(void) { return S_OK; };
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, TYPE type, const char* ModelFileName);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	bool Collision(D3DXVECTOR3* pPos, D3DXVECTOR3 pPosOld, D3DXVECTOR3* pMove);
	static HRESULT LoadStage(void);

	void SetbHit(bool bHit) { m_bHit = bHit; };
	bool bHit(void) { return m_bHit; };

	void SetbPlaySound(bool bPlay) { m_bPlaySound = bPlay; };
	bool GetbPlaySound(void) { return m_bPlaySound; };

	TYPE GetType(void) { return m_type; };
	void SetType(TYPE type) { m_type = type; };

	D3DXVECTOR3 GetModelMaxSize(void) { return m_SizeMax; };
	D3DXVECTOR3 GetModelMinSize(void) { return m_SizeMin; };

	void SetfMove(float fMove) { m_fMove = fMove; };
	void SetVec(D3DXVECTOR3 Vec) { m_VecMove = Vec; };
	void ResetHitCnt(void) { m_nCntHit = 60; };

private:
	static HRESULT Load(void);
	void HitCorn(void);

	bool m_bDisp;	// 表示するかどうか
	bool m_bHit;	// 衝突したかどうか

	float m_sinCurve;	// sinカーブ用の変数
	float m_fMove;		// 移動量
	D3DXVECTOR3 m_SizeMax;		// 最大値 
	D3DXVECTOR3 m_SizeMin;		// 最小値
	D3DXVECTOR3 m_VecMove;		// 移動方向ベクトル
	TYPE m_type;	// 種類
	int m_nCntHit;		// ヒットフラグのカウンタ
	bool m_bPlaySound;	// 各種モデルの効果音再生フラグ
	CMeshRing* m_pReactRing;	// 当たったリアクション用メッシュリング
};

#endif