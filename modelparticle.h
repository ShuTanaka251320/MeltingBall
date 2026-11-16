// =================================================
// 
// モデルでのパーティクル処理[modelparticle.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _MODELPARTICLE_H_
#define _MODELPARTICLE_H_

#include "main.h"
#include "object.h"

class CObjectXEffect;

// =================================================
// マクロ定義
#define MAX_PARTICLE			(1024)		// パーティクルの最大数

// =================================================
// クラス定義
class CModelParticle:public CObject
{
public:
	CModelParticle(int nPriority = 8);
	~CModelParticle();
	static CModelParticle* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, 
		D3DXVECTOR3 Minmove, D3DXCOLOR col, float fscale, int nNumParticle, int nLife, const char *pFileName, int nPri = 6);
	
	HRESULT Init(void) { return S_OK; };
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, 
		D3DXVECTOR3 Minmove, D3DXCOLOR col, float fscale, int nNumParticle, int nLife, const char* pFileName);

	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; };

private:
	CObjectXEffect* m_apModelEffect[MAX_PARTICLE];
	D3DXVECTOR3 m_pos;	// 位置
	D3DXVECTOR3 m_move;	// 移動量
	D3DXVECTOR3 m_Minmove;	// 最低移動量
	D3DXCOLOR m_col;	// 色
	float m_fscale;		// 拡縮率
	float m_fHeight;	// 縦幅
	int m_nLife;		// 寿命
	int m_nNumParticle;	// パーティクルの量
	const char* m_pModelFileName;	// 使用するモデルのファイル名
};

#endif
