// =================================================
// 
// 入力処理[input.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _INPUT_H_
#define _INPUT_H_

#include"main.h"

// =================================================
// マクロ定義
#define NUM_KEY_MAX		(256)		// キーの最大数

// =================================================
// 入力クラス
class CInput
{
public:
	CInput();
	virtual ~CInput();

	virtual HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	virtual void Uninit(void);
	virtual void Update(void) = 0;

protected:
	static LPDIRECTINPUT8	m_pInput;	// DirectInputオブジェクトへのポインタ
	LPDIRECTINPUTDEVICE8	m_pDevice;	// 入力デバイスへのポインタ
};

// =================================================
// キーボードクラス
class CInputKeyboard :public CInput
{
public:
	CInputKeyboard();
	~CInputKeyboard();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(int nKey);
	bool GetTrigger(int nKey);
	bool GetRelease(int nKey);

private:
	BYTE m_aKey[NUM_KEY_MAX];			// キーボードのプレス情報
	BYTE m_aKeyTrigger[NUM_KEY_MAX];	// キーボードのトリガー情報
	BYTE m_aKeyRelease[NUM_KEY_MAX];	// キーボードのリリース情報
};

// =================================================
// マウスクラス
class CInputMouse :public CInput
{
public:
	// --- マウスのボタンの列挙型宣言 --- //
	typedef enum
	{
		MOUSE_LEFT,			// 左クリック
		MOUSE_RIGHT,		// 右クリック
		MOUSE_CENTER,		// ホイールクリック
		MOUSE_SIDE1,		// サイドボタン1
		MOUSE_SIDE2,		// サイドボタン2
		MOUSE_RESERVED1,	// 備え付きボタン1
		MOUSE_RESERVED2,	// 備え付きボタン2
		MOUSE_RESERVED3,	// 備え付きボタン3
		MOUSE_MAX
	}TYPE;

	CInputMouse();
	~CInputMouse();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);
	bool GetPress(TYPE nKey);
	bool GetTrigger(TYPE nKey);
	bool GetRelease(TYPE nKey);
	POINT GetPosition(void) { return m_pos; };
	bool GetMousePush(void) { return m_bMousePush; };
	void ChangeMousePush(bool bMousePush) { m_bMousePush = bMousePush; };

private:
	POINT m_pos;	// カーソルの位置
	DIMOUSESTATE2 m_MouseState;		// マウス情報
	BYTE m_aMousePress[MOUSE_MAX];	// マウス情報
	BYTE m_aMouseRelease[MOUSE_MAX];	// マウス情報
	BYTE m_aMouseTrigger[MOUSE_MAX];	// マウス情報
	static HWND	m_hWnd;	// スクリーン領域の保持
	bool m_bMousePush;	// マウスを押しているかどうか
};

// =================================================
// ジョイパッドクラス
class CJoyPad :public CInput
{
public:

	// =================================================
	// ジョイパッドのキーの種類
	typedef enum
	{
		JOYKEY_UP = 0,		// 十字キー(上)
		JOYKEY_DOWN,		// 十字キー(下)
		JOYKEY_LEFT,		// 十字キー(左)
		JOYKEY_RIGHT,		// 十字キー(右)
		JOYKEY_START,		// スタートボタン
		JOYKEY_BACK,		// バックボタン
		JOYKEY_LT,			// Lスティック押し込み
		JOYKEY_RT,			// Rスティック押し込み
		JOYKEY_LB,			// L1
		JOYKEY_RB,			// R1
		JOYKEY_w,			// ボタン
		JOYKEY_e,			// ボタン
		JOYKEY_A,			// Aボタン
		JOYKEY_B,			// Bボタン
		JOYKEY_X,			// Xボタン
		JOYKEY_Y,			// Yボタン
		JOYKEY_MAX
	}JOYKEY;

	// =================================================
	// ジョイパッドのスティックの種類
	typedef enum
	{
		STICK_TYPE_LEFT = 0,	// 左スティック
		STICK_TYPE_RIGHT,		// 右スティック
		STICK_TYPE_MAX
	}STICK_TYPE;


	CJoyPad();
	~CJoyPad();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	bool GetPress(JOYKEY key);
	bool GetTrigger(JOYKEY key);
	bool GetRelease(JOYKEY key);
	bool GetStickDir(STICK_TYPE type, D3DXVECTOR2 *Vec);			// ジョイスティックの傾きを取得
	D3DXVECTOR2 GetStickDirTrigger(STICK_TYPE type);	// ジョイスティックのはじきを取得
	void SetVib(int nValueLeft, int nValueRight, int nTimer);

private:
	XINPUT_STATE m_State;			// ジョイパッドのプレス情報
	XINPUT_STATE m_StateTrigger;	// ジョイパッドのトリガー情報
	XINPUT_STATE m_StateRelease;	// ジョイパッドのリリース情報
	XINPUT_VIBRATION m_Vibration;	// ジョイパッドのバイブレーション情報
	int m_nTimerVibration = 0;		// ジョイパッドのバイブレーションタイマー

};

#endif