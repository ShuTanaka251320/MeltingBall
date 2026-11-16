// =================================================
// 
// ビルボード処理[billboard.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

#include "main.h"
#include "object.h"

// =================================================
// マクロ定義
#define BILLBOARD_PRI	(7)		// ビルボードの優先順位

// =================================================
// クラス定義
class CBillboard :public CObject
{
public:
	CBillboard(int nPriority = BILLBOARD_PRI);
	~CBillboard();
	static CBillboard* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, CObject::TYPE type, int nPri = BILLBOARD_PRI, const char* pFileName = NULL);

	HRESULT Init(void) { return S_OK; };
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, CObject::TYPE type, const char* pFileName = NULL);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	// --- メンバ変数の取得、変更する関数 --- //
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; };
	void SetMove(const D3DXVECTOR3 move) { m_move = move; };
	void SetRotation(const D3DXVECTOR3 rot) { m_rot = rot; };

	D3DXVECTOR3 GetPosition(void) { return m_pos; };
	D3DXVECTOR3 GetMove(void) { return m_move; };
	D3DXVECTOR3 GetRotation(void) { return m_rot; };
	
	void AlphaDecrease(int nMaxLife, int nCurrentLife);

	// --- テクスチャの指定 --- //
	void SetTexture(int nIdx) { m_nIdxTex = nIdx; };

private:
	LPDIRECT3DTEXTURE9 m_pTexture;		// テクスチャへのポインタ
	LPDIRECT3DVERTEXBUFFER9	m_pVtxBuff;	// 頂点バッファへのポインタ
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス

	// --- ポリゴンに使う変数 --- //
	D3DXVECTOR3 m_pos;		// 位置
	D3DXVECTOR3 m_move;		// 移動量
	D3DXVECTOR3 m_rot;		// 向き
	D3DXCOLOR m_col;		// 色
	float m_fLength;		// ポリゴンの対角線の長さ
	float m_fAngle;			// ポリゴンの対角線の角度
	float m_fWidth;			// 横幅
	float m_fHeight;		// 縦幅
	float m_Alpha;			// 透明度

	bool m_bDisp;			// 表示するかどうか
	int m_nIdxTex;			// テクスチャのインデックス
};

#endif