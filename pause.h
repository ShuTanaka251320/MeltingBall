// =================================================
// 
// 一時停止処理[Pause.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _PAUSE_H_
#define _PAUSE_H_

#include"object2D.h"

// =================================================
// クラス定義
class CPause
{
public:
	// =================================================
	// ポーズ処理の状態
	typedef enum
	{
		PAUSE_ON = 0,		// ポーズ中
		PAUSE_OFF,			// ポーズ解除中
		PAUSE_MAX
	}PAUSE;

	// =================================================
	// UIの種類
	typedef enum
	{
		TYPE_CONTINUE = 0,	// コンティニュー
		TYPE_RETRY,			// リトライ
		TYPE_QUIT,			// やめる
		TYPE_MAX
	}TYPE;

	CPause();
	~CPause();

	void Init();
	void Uninit();
	void Update();
	void Draw();

	static PAUSE GetPause(void) { return m_state; };

private:
	static void InitColor(void);		// ポーズ選択肢の色初期化
	static void InitChoice(void);		// ポーズ選択肢の初期化
	static void InitDisp(void);			// ポーズポリゴンの表示

	static PAUSE m_state;	// ポーズ状態
	static int m_nSelect;	// 選択肢
	bool m_bDisp;	// 表示するかどうか
	static TYPE m_type[TYPE_MAX];	// ポーズ選択肢の種類
	static CObject2D* m_pBg;		// ポーズ画面中に表示する背景
	static CObject2D* m_pUI[TYPE_MAX];	// 選択肢のUI

};

#endif