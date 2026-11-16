// =================================================
// 
// 3Dオブジェクト処理[object3D.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

#include"main.h"
#include"object.h"

// =================================================
// マクロ定義
#define OBJ3D_PRI	(4)

// =================================================
// クラス定義
class CObject3D :public CObject
{
public:
	CObject3D(int nPriority = OBJ3D_PRI);
	~CObject3D();

	static CObject3D* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, CObject::TYPE type, const char* pFileName = NULL);
	HRESULT Init(void);
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, CObject::TYPE type, const char* pFileName = NULL);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	// --- メンバ変数の取得、変更する関数 --- //
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; };
	void SetMove(const D3DXVECTOR3 move) { m_move = move; };
	void SetRotation(const D3DXVECTOR3 rot) { m_rot = rot; };
	void SetWidth(const float fWidth) { m_fWidth = fWidth; };
	void SetHeight(const float fHeight) { m_fHeight = fHeight; };
	void SetLength(const float fWidth, float fHeight) { m_fLength = sqrtf(fWidth * fWidth + fHeight * fHeight) * 0.5f; };
	void SetAngle(const float fWidth, float fHeight) { m_fAngle = (float)atan2(fWidth, fHeight); };
	void SetCol(D3DXCOLOR col);

	D3DXVECTOR3 GetPosition(void) { return m_pos; };
	D3DXVECTOR3 GetMove(void) { return m_move; };
	D3DXVECTOR3 GetRotation(void) { return m_rot; };
	float GetWidth(void) { return m_fWidth; };
	float GetHeight(void) { return m_fHeight; };
	float GetLength(void) { return m_fLength; };
	float GetAngle(void) { return m_fAngle; };

	void DecreaseLength(float fSize) { m_fHeight += fSize; m_fWidth += fSize; };
	void IncreaseLength(float fSize) { m_fHeight -= fSize; m_fWidth -= fSize; };

	// --- テクスチャの指定 --- //
	void SetTexture(int nIdx) { m_nIdxTex = nIdx; };

private:
	LPDIRECT3DTEXTURE9 m_pTexture;		// テクスチャのポインタ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファのポインタ	
	D3DXMATRIX m_mtxWorld;				// ワールドマトリックス
	int m_nIdxTex;						// テクスチャのインデックス

	// --- ポリゴンに使う変数 --- //
	D3DXVECTOR3 m_pos;		// 位置
	D3DXVECTOR3 m_move;		// 移動量
	D3DXVECTOR3 m_rot;		// 向き
	float m_fLength;		// ポリゴンの対角線の長さ
	float m_fAngle;			// ポリゴンの対角線の角度
	float m_fWidth;			// 横幅
	float m_fHeight;		// 縦幅
	float m_fPolHeight;		// ポリゴンの高さ

};

#endif