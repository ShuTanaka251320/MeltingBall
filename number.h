// =================================================
// 
// 数字処理[number.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _NUMBER_H_
#define _NUMBER_H_

#include"main.h"

// =================================================
// マクロ定義
#define MAX_NUMBER	(32)	// 数字の合計最大桁数

// =================================================
// クラス定義
class CNumber 
{
public:
	CNumber();
	~CNumber();

	static HRESULT Load(void);
	static void Unload(void);
	static CNumber* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight);
	
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	// --- ポリゴンの数字を変更 --- // 
	void SetNumber(int nNumber);

	// --- メンバ変数の取得、変更する関数 --- //
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; };
	void SetMove(const D3DXVECTOR3 move) { m_pos = move; };
	void SetRotation(const D3DXVECTOR3 rot) { m_rot = rot; };
	void SetWidth(const float fWidth) { m_fWidth = fWidth; };
	void SetHeight(const float fHeight) { m_fWidth = fHeight; };
	void SetLength(const float fWidth, float fHeight) { m_fLength = sqrtf(fWidth * fWidth + fHeight * fHeight) * 0.5f; };
	void SetAngle(const float fWidth, float fHeight) { m_fAngle = (float)atan2(fWidth, fHeight); };
	void SetCol(const D3DXCOLOR col);
	void FlashPol(int nCntFlash);
	float FadePol(int nFadeTime);

	D3DXVECTOR3 GetPosition(void) { return m_pos; };
	D3DXVECTOR3 GetMove(void) { return m_move; };
	D3DXVECTOR3 GetRotation(void) { return m_rot; };
	float GetWidth(void) { return m_fWidth; };
	float GetHeight(void) { return m_fHeight; };
	float GetLength(void) { return m_fLength; };
	float GetAngle(void) { return m_fAngle; };

	void SetTexture(int nIdx) { m_nIdxTex = nIdx; };

private:
	static LPDIRECT3DTEXTURE9 m_pTexture;		// テクスチャのポインタ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファのポインタ	

	// --- ポリゴンに使う変数 --- //
	D3DXVECTOR3 m_pos;		// 位置
	D3DXVECTOR3 m_move;		// 移動量
	D3DXVECTOR3 m_rot;		// 向き
	D3DXCOLOR m_col;		// 色
	float m_fLength;		// ポリゴンの対角線の長さ
	float m_fAngle;			// ポリゴンの対角線の角度
	float m_fWidth;			// 横幅
	float m_fHeight;		// 縦幅

	int m_nIdxTex;			// テクスチャインデックス
	float m_fAlpha;			// ポリゴンの透明度
	int m_nFlashCounter;	// 点滅のカウンター
	int m_nFadeCounter;		// フェードのカウンター
};

#endif