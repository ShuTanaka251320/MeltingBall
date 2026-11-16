// =================================================
// 
// エフェクト処理[effect.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _EFFECT_H_
#define _EFFECT_H_

#include"main.h"
#include"billboard.h"

// =================================================
// マクロ定義
#define EFFECT_PRI	(8)	// エフェクトの優先順位

// =================================================
// クラス定義
class CEffect :public CBillboard
{
public:
	typedef enum
	{
		TYPE_DEF = 0,	// デフォルト	
		TYPE_FOLLOW,	// 吸引タイプ
		TYPE_HEAT,		// 熱エフェクトタイプ
		TYPE_MAX
	}TYPE;

	CEffect(int nPriority = EFFECT_PRI);
	~CEffect();

	static CEffect *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, int nLife, CObject::TYPE type = CObject::TYPE_EFFECT, int nPri = EFFECT_PRI, TYPE Effecttype = TYPE_DEF, const char* pFileName = "data\\TEXTURE\\effect000.jpg");
	HRESULT Init(void);
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, int nLife, CObject::TYPE type = CObject::TYPE_EFFECT, TYPE Effecttype = TYPE_DEF, const char* pFileName = "data\\TEXTURE\\effect000.jpg");
	void Uninit(void);
	void Update(void);
	void Draw(void);
	int GetLife(void) { return m_nLife; };

private:
	static LPDIRECT3DTEXTURE9 m_pTexture;	// テクスチャのポインタ
	int m_nLife;	// 寿命
	int m_nMaxLife;	// 設定寿命
	int m_nIdxTex;	// テクスチャのインデックス
	CObject::TYPE m_type;	// オブジェクトタイプ
	TYPE m_typeEffect;	// エフェクトタイプ
	D3DXVECTOR3 m_SpawnPos;	// 発生ポジション
};

#endif