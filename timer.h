// =================================================
// 
// タイマー処理[timer.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _TIMER_H_
#define _TIMER_H_

#include"number.h"
#include"object.h"

// =================================================
// マクロ定義
#define MAX_TIMER_DIG	(3)	// 数字の合計最大桁数

// =================================================
// クラス定義
class CTimer :public CObject
{
public:
	CTimer(int nPriority = 8);
	~CTimer();

	static CTimer* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, int nTime, float fWidth, float fHeight, const char* pFileName);
	HRESULT Init(void);
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, int nTime, float fWidth, float fHeight, const char* pFileName);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	// --- メンバ変数の取得、変更する関数 --- //
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; };
	void SetMove(const D3DXVECTOR3 move) { m_pos = move; };
	void SetRotation(const D3DXVECTOR3 rot) { m_rot = rot; };
	void SetWidth(const float fWidth) { m_fWidth = fWidth; };
	void SetHeight(const float fHeight) { m_fWidth = fHeight; };
	void SetLength(const float fWidth, float fHeight) { m_fLength = sqrtf(fWidth * fWidth + fHeight * fHeight) * 0.5f; };
	void SetAngle(const float fWidth, float fHeight) { m_fAngle = (float)atan2(fWidth, fHeight); };

	D3DXVECTOR3 GetPosition(void) { return m_pos; };
	D3DXVECTOR3 GetMove(void) { return m_move; };
	D3DXVECTOR3 GetRotation(void) { return m_rot; };
	float GetWidth(void) { return m_fWidth; };
	float GetHeight(void) { return m_fHeight; };
	float GetLength(void) { return m_fLength; };
	float GetAngle(void) { return m_fAngle; };

	int GetTime(void) { return m_nTimer; };
	void DecreaseTimer(int nTimer);

	bool IsGameOver(void) { return m_bGameOver; };

private:
	CNumber* m_apNumber[MAX_TIMER_DIG];	// 数字のポインタ

	// --- ポリゴンに使う変数 --- //
	D3DXVECTOR3 m_pos;	// 位置
	D3DXVECTOR3 m_move;	// 移動量
	D3DXVECTOR3 m_rot;	// 向き
	float m_fLength;	// ポリゴンの対角線の長さ
	float m_fAngle;		// ポリゴンの対角線の角度
	float m_fWidth;		// 横幅
	float m_fHeight;	// 縦幅

	DWORD m_dwExecLastTime;	// 時間を減らす用
	int m_nTimer;		// 残り時間
	bool m_bTimer;		// タイマーを起動中かどうか
	bool m_bGameOver;	// ゲームオーバーフラグ
};

#endif