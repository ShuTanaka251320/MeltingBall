// =================================================
// 
// Xオブジェクトでのパーティクル処理[objectXeffect.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _OBJECTEFFECTX_H_
#define _OBJECTEFFECTX_H_

#include"main.h"
#include"object.h"
#include"xmodel.h"

// =================================================
// マクロ定義
#define OBJXEFFECT_PRI	(4)		// モデルの優先順位

// =================================================
// クラス定義
class CObjectXEffect :public CObject
{
public:
	CObjectXEffect(int nPriority = OBJXEFFECT_PRI);
	~CObjectXEffect();

	static CObjectXEffect* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float scale, int nLife, CObject::TYPE type, int nPri, const char* pFileName);
	HRESULT Init(void) { return S_OK; };
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float scale, int nLife, CObject::TYPE type, int nPri, const char* pFileName);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Draw(int type);
	void NorRotate(float fRotDest);

	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; };
	void SetRotation(D3DXVECTOR3 rot) { m_rot = rot; };

	// --- メンバ変数の取得、設定 --- //
	D3DXVECTOR3 GetPosition(void) { return m_pos; };
	D3DXVECTOR3 GetRotation(void) { return m_rot; };
	D3DXMATRIX *GetWorldMtx(void) { return &m_mtxWorld; };

private:
	void Bound(void);

	D3DXVECTOR3 m_pos;			// 位置(オフセット)
	D3DXVECTOR3 m_rot;			// 向き
	D3DXVECTOR3 m_rotate;		// 回転する向き
	D3DXVECTOR3 m_rotDest;		// 目標の向き
	D3DXVECTOR3 m_rotDiff;		// 目標と現在の向きの差分
	D3DXVECTOR3 m_move;			// 移動量
	D3DXVECTOR3 m_scl;			// スケール量
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	bool m_bDisp;					// 表示するかどうか
	int m_nIdxModel;	// 1Xモデルのインデックス
	int m_nLife;				// 寿命
	float m_sin;		// sinカーブ用

	float m_fRebound;		// バウンド量
	float m_fSaveMove;		// ジャンプ量の保存
	bool m_bSetJumpForce;	// バウンド力のセットフラグ
	bool m_bJump;	// ジャンプフラグ
};

#endif