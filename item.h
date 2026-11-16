// =================================================
// 
// アイテム処理[item.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _ITEM_H_
#define _ITEM_H_

#include"objectX.h"

// =================================================
// マクロ定義
#define ITEM_PRI	(5)		// モデルの優先順位
#define MAX_ITEM	(300)	// アイテムの最大数

// =================================================
// 前方宣言
class CTreasure;

// =================================================
// クラス定義
class CItem 
{
public:
	typedef enum
	{
		TYPE_TREASURE = 0,	// ゲームクリア取集物のモデル
		TYPE_NEEDLE_BALL,	// 鉄球
		TYPE_MAX
	}TYPE;

	CItem(int nPriority = ITEM_PRI);
	~CItem();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	bool Collision(D3DXVECTOR3* pPos, D3DXVECTOR3 pPosOld, D3DXVECTOR3* pMove);

	static CTreasure* GetItem(int nIdx) { return m_pTreasure[nIdx]; };
	static int GetNumAll(void) { return m_nNumAll; };
	static int GetNumGet(void) { return m_nGetNum; };
	static void PlusNumAll(void) {m_nNumAll++; };
	static void DecreaseNumAll(void) {m_nNumAll--; };

	static void ResetItem(void);
	static HRESULT LoadItem(void);
	static void PanishItem(int nIdx);
	static void PanishItem(int nIdx, bool bReset);

private:
	static bool JudgeComent(char* pStr);

	static int m_nNumAll;		// 配置アイテム数
	static int m_nGetNum;		// 獲得アイテム数

	static CTreasure* m_pTreasure[MAX_ITEM];	// アイテムのポインタ
};

#endif