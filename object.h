// =================================================
// 
// オブジェクト処理[object.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _OBJECT_H_
#define _OBJECT_H_

#include"main.h"

// =================================================
// マクロ定義
#define MAX_OBJ	(9192)	// オブジェクトの最大数
#define MAX_PRI	(9)		// 優先順位の総数
#define MAX_MAT	(32)	// マテリアルの最大数

// =================================================
// クラス定義
class CObject
{
public:
	typedef enum
	{
		TYPE_NONE = 0,	// なし
		TYPE_PLAYER,	// プレイヤー
		TYPE_ENEMY,		// エネミー
		TYPE_EFFECT,	// エフェクト
		TYPE_PARTICLE,	// パーティクル
		TYPE_SHADOW,	// 影
		TYPE_STAGEMODEL,// ステージのモデル
		TYPE_FIELD,		// 地面
		TYPE_BG,		// 背景
		TYPE_ITEM,		// アイテム
		TYPE_MESH,		// メッシュ
		TYPE_LINE,	// 道路の白線
		TYPE_MAX
	}TYPE;

	CObject(int nPriority = 3);
	virtual ~CObject();

	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	static void ReleaseAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	void Release(void) { m_bDeath = true; };

	void SetType(TYPE type) { m_type = type; };
	TYPE GetType(void) { return m_type; };
	int GetPriority(void) { return m_nPriority; };

	static CObject* GetTop(int nPriority) { return m_apTop[nPriority]; };
	CObject* GetPrev(void) { return m_pPrev; };
	CObject* GetNext(void) { return m_pNext; };

protected:

private:
	void ObjPanish(void);

	static CObject* m_apTop[MAX_PRI];	// 先頭のオブジェクトのポインタ
	static CObject* m_apCur[MAX_PRI];	// 最後尾のオブジェクトのポインタ
	CObject* m_pPrev;					// 前のオブジェクトのポインタ
	CObject* m_pNext;					// 次のオブジェクトのポインタ
	static int m_nNumAll;	// 総数
	int m_nID;				// 自身のID
	int m_nPriority;		// 優先順位
	TYPE m_type;			// タイプ
	bool m_bDeath;			// 死亡フラグ
};

#endif