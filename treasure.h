// =================================================
// 
// アイテム(トレジャー)処理[treasure.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _TREASURE_H_
#define _TREASURE_H_

#include"main.h"
#include"objectX.h"
#include"item.h"

// =================================================
// マクロ定義
#define TREASURE_PRI	(4)		// モデルの優先順位

// =================================================
// 前方宣言
class CMeshRing;
class CShadow;

// =================================================
// クラス定義
class CTreasure :public CObjectX
{
public:
	CTreasure(int nPriority = TREASURE_PRI);
	~CTreasure();

	static CTreasure* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, CItem::TYPE type, const char* ModelFileName);
	HRESULT Init(void) { return S_OK; };
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, CItem::TYPE type, const char* ModelFileName);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	CItem::TYPE GetType(void) { return m_type; };
	CMeshRing* GetMeshRing(void) { return m_pMeshRing; };
	void SetGetFlag(bool bFlag) { m_bGet = bFlag; };
	bool Collision(D3DXVECTOR3* pPos, D3DXVECTOR3 pPosOld, D3DXVECTOR3* pMove);

private:
	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_move;			// 移動量
	CItem::TYPE m_type;				// 種類
	float m_fsin;				// sinカーブでの移動量を保管
	bool m_bDisp;				// 表示するかどうか
	bool m_bGet;				// 取得しているかどうか
	bool m_bLive;				// 取得可能か
	bool m_bGetAny;				// どれか取得したか

	D3DXMATRIX* m_mtx;			// ワールドマトリックス
	D3DXMATRIX* m_mtxNext;		// くっつけるワールドマトリックス
	D3DXMATRIX* m_mtxFollow;	// ついていくワールドマトリックス
	CShadow* m_pShadow;			// 影のポインタ
	CMeshRing* m_pMeshRing;		// メッシュリングのポインタ
};

#endif