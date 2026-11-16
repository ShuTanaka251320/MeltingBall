// =================================================
// 
// パーティクル処理[particle.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "main.h"
#include "object.h"

class CEffect;

// =================================================
// マクロ定義
#define MAX_PARTICLE			(1024)		// パーティクルの最大数

// =================================================
// クラス定義
class CParticle:public CObject
{
public:
	typedef enum
	{
		TYPE_DEF = 0,	// デフォルト	
		TYPE_FOLLOW,	// 吸引タイプ
		TYPE_HEAT,		// 熱エフェクトタイプ
		TYPE_ROADHEAT,	// ロードヒーティング用
		TYPE_SNOW,	// 雪パーティクル
		TYPE_MAX
	}TYPE;

	CParticle(int nPriority = 8);
	~CParticle();

	static CParticle* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, int nNumParticle, int nLife, int nPri = 8);
	static CParticle* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXVECTOR3 Minmove, D3DXCOLOR col, float fWidth, float fHeight, int nNumParticle, int nLife, int nPri = 6, TYPE type = TYPE_DEF);
	HRESULT Init(void);
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight,int nNumParticle, int nLife);
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXVECTOR3 Minmove, D3DXCOLOR col, float fWidth, float fHeight,int nNumParticle, int nLife, TYPE type = TYPE_DEF);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; };

private:
	CEffect* m_apEffect[MAX_PARTICLE];
	D3DXVECTOR3 m_pos;	// 位置
	D3DXVECTOR3 m_move;	// 移動量
	D3DXVECTOR3 m_Minmove;	// 最低移動量
	D3DXCOLOR m_col;	// 色
	float m_fWidth;		// 横幅
	float m_fHeight;	// 縦幅
	int m_nLife;		// 寿命
	int m_nNumParticle;	// パーティクルの量
	TYPE m_type;		// パーティクルタイプ
};

#endif
