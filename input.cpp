// =================================================
// 
// 入力処理[input.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include"input.h"
#include"debugproc.h"
#include"manager.h"

// =================================================
// マクロ定義
#define CURSOR_ON	(true)	// カーソルの表示設定
#define DEADZONE	(0.1f)	// ジョイパッドスティックのデッドゾーン

// =================================================
// 静的メンバ変数変数
LPDIRECTINPUT8 CInput::m_pInput = NULL;	// DirectInputオブジェクトへのポインタ
HWND CInputMouse::m_hWnd = NULL;		// スクリーン領域の保持

// ***********************************************************
// 基本クラスの処理
// ***********************************************************

// =================================================
// コンストラクタ
// =================================================
CInput::CInput()
{
	// --- メンバ変数のクリア --- //
	m_pDevice = NULL;
	m_pInput = NULL;
}

// =================================================
// デストラクタ
// =================================================
CInput::~CInput()
{

}

// =================================================
// 初期化処理
// =================================================
HRESULT CInput::Init(HINSTANCE hInstance, HWND hWnd)
{
	if (m_pInput == NULL)
	{
		// DirectInputオブジェクトの生成
		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, NULL)))
		{
			return E_FAIL;
		}
	}
	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CInput::Uninit(void)
{
	// デバイスの破棄(アクセス権の解放)
	if (m_pDevice != NULL)
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	// オブジェクトの破棄
	if (m_pInput != NULL)
	{
		m_pInput->Release();
		m_pInput = NULL;
	}
}

// ***********************************************************
// 派生クラスの処理(キーボード)
// ***********************************************************

// =================================================
// コンストラクタ
// =================================================
CInputKeyboard::CInputKeyboard()
{
	// --- メンバ変数のクリア --- //
	memset(&m_aKey[0], 0, sizeof(BYTE));
	memset(&m_aKeyRelease[0], 0, sizeof(BYTE));
	memset(&m_aKeyTrigger[0], 0, sizeof(BYTE));
}

// =================================================
// デストラクタ
// =================================================
CInputKeyboard::~CInputKeyboard()
{

}

// =================================================
// キーボードの初期化
// =================================================
HRESULT CInputKeyboard::Init(HINSTANCE hInstance, HWND hWnd)
{
	// --- 親クラスの初期化処理 --- //
	CInput::Init(hInstance, hWnd);

	// 入力デバイスの生成
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	// データのフォーマットを設定
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	// 協調モードを設定
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	// キーボードのアクセス権を獲得
	m_pDevice->Acquire();

	return S_OK;
}

// =================================================
// キーボードの終了処理
// =================================================
void CInputKeyboard::Uninit(void)
{
	// 親クラスの終了処理
	CInput::Uninit();
}

// =================================================
// キーボードの更新処理
// =================================================
void CInputKeyboard::Update(void)
{
	BYTE aKeyState[NUM_KEY_MAX];			// キーボードの入力情報

	// 入力デバイスからデータを取得
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (int nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
		{
			m_aKeyTrigger[nCntKey] = (aKeyState[nCntKey] | m_aKey[nCntKey]) ^ m_aKey[nCntKey];	// トリガー時の処理
			m_aKeyRelease[nCntKey] = (aKeyState[nCntKey] & m_aKey[nCntKey]) ^ m_aKey[nCntKey];	// リリース時の処理
			m_aKey[nCntKey] = aKeyState[nCntKey];		// キーボードのプレス情報を保存
		}
	}
	else
	{
		// キーボードのアクセス権を獲得
		m_pDevice->Acquire();
	}
}

// =================================================
// キーボードのプレス情報を取得
// =================================================
bool CInputKeyboard::GetPress(int nKey)
{
	return (m_aKey[nKey] & 0x80) ? true : false;
}

// =================================================
// キーボードのトリガー情報を取得
// =================================================
bool CInputKeyboard::GetTrigger(int nKey)
{
	return (m_aKeyTrigger[nKey] & 0x80) ? true : false;
}

// =================================================
// キーボードのリリース情報を取得
// =================================================
bool CInputKeyboard::GetRelease(int nKey)
{
	return (m_aKeyRelease[nKey] & 0x80) ? true : false;
}

// ***********************************************************
// 派生クラスの処理(マウス)
// ***********************************************************

// =================================================
// コンストラクタ
// =================================================
CInputMouse::CInputMouse()
{
	// --- メンバ変数のクリア --- //
	memset(&m_MouseState, 0, sizeof(m_MouseState));
	memset(m_aMousePress, 0, sizeof(m_aMousePress));
	memset(m_aMouseTrigger, 0, sizeof(m_aMouseTrigger));
	memset(m_aMouseRelease, 0, sizeof(m_aMouseRelease));
	m_pos = {};
	m_bMousePush = false;
}

// =================================================
// デストラクタ
// =================================================
CInputMouse::~CInputMouse()
{

}

// =================================================
// マウスの初期化
// =================================================
HRESULT CInputMouse::Init(HINSTANCE hInstance, HWND hWnd)
{
	// --- 親クラスの初期化処理 --- //
	CInput::Init(hInstance, hWnd);

	// --- 入力デバイスの生成 --- //
	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	// --- データのフォーマットを設定 --- //
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		return E_FAIL;
	}

	// --- 協調モードを設定 --- //
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	// --- マウスのアクセス権を獲得 --- //
	m_pDevice->Acquire();

	// --- ゲームウィンドウのクライアント領域を保管 --- //
	m_hWnd = hWnd;

	ShowCursor(CURSOR_ON);	// カーソルの表示設定

	return S_OK;
}

// =================================================
// マウスの終了処理
// =================================================
void CInputMouse::Uninit(void)
{
	// --- 親クラスの終了処理 --- //
	CInput::Uninit();
}

// =================================================
// マウスの更新処理
// =================================================
void CInputMouse::Update(void)
{
	DIMOUSESTATE2 MouseState;

	// --- カーソル位置の取得 --- //
	GetCursorPos(&m_pos);

	// --- ゲームウィンドウのクライアント領域に変換 --- //
	ScreenToClient(m_hWnd, &m_pos);

	// --- 入力デバイスからデータを取得 --- //
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &MouseState)))
	{

		for (int nCnt = 0; nCnt < MOUSE_MAX; nCnt++)
		{
			m_aMouseTrigger[nCnt] = (MouseState.rgbButtons[nCnt] | m_MouseState.rgbButtons[nCnt]) ^ m_MouseState.rgbButtons[nCnt];	// トリガー時の処理
			m_aMouseRelease[nCnt] = (MouseState.rgbButtons[nCnt] & m_MouseState.rgbButtons[nCnt]) ^ m_MouseState.rgbButtons[nCnt];	// リリース時の処理
		}
		m_MouseState = MouseState;
	}
	else
	{
		// --- マウスのアクセス権を獲得 --- //
		m_pDevice->Acquire();
	}
}

// =================================================
// マウスのプレス情報を取得
// =================================================
bool CInputMouse::GetPress(TYPE nButton)
{
	return (m_MouseState.rgbButtons[nButton] & 0x80) ? true : false;
}

// =================================================
// マウスのトリガー情報を取得
// =================================================
bool CInputMouse::GetTrigger(TYPE nButton)
{
	return (m_aMouseTrigger[nButton] & 0x80) ? true : false;
}

// =================================================
// マウスのリリース情報を取得
// =================================================
bool CInputMouse::GetRelease(TYPE nButton)
{
	return (m_aMouseRelease[nButton] & 0x80) ? true : false;
}
// ***********************************************************
// 派生クラスの処理(ジョイパッド)
// ***********************************************************

// =================================================
// コンストラクタ
// =================================================
CJoyPad::CJoyPad()
{

}

// =================================================
// デストラクタ
// =================================================
CJoyPad::~CJoyPad()
{
	memset(&m_State, 0, sizeof(m_State));				// ジョイパッドのプレス情報のクリア
	memset(&m_StateTrigger, 0, sizeof(m_StateTrigger));	// ジョイパッドのトリガー情報のクリア
	memset(&m_StateRelease, 0, sizeof(m_StateRelease));	// ジョイパッドのリリース情報のクリア
	memset(&m_Vibration, 0, sizeof(m_Vibration));		// ジョイパッドのバイブレーション情報のクリア
	m_nTimerVibration = 0;		// ジョイパッドのバイブレーションタイマーのクリア
}

// =================================================
// ジョイパッドの初期化
// =================================================
HRESULT CJoyPad::Init(void)
{
	// メモリのクリア
	memset(&m_State, 0, sizeof(XINPUT_STATE));

	// XInputのステートを設定(有効にする)
	XInputEnable(true);

	return S_OK;
}

// =================================================
// ジョイパッドの終了処理
// =================================================
void CJoyPad::Uninit(void)
{
	// XInputのステートを設定(無効にする)
	XInputEnable(false);
}

// =================================================
// ジョイパッドの更新処理
// =================================================
void CJoyPad::Update(void)
{
	XINPUT_STATE joykeyState;				// ジョイパッドの入力情報
	//XINPUT_STATE state;

	DWORD result = XInputGetState(0, &joykeyState);

	// ジョイパッドの状態を取得
	if (XInputGetState(0, &joykeyState) == ERROR_SUCCESS)
	{
		m_StateTrigger.Gamepad.wButtons = (m_State.Gamepad.wButtons | joykeyState.Gamepad.wButtons) ^ m_State.Gamepad.wButtons;	// トリガー時の処理
		m_State = joykeyState;		// ジョイパッドのプレス情報を保存 

		m_StateTrigger.Gamepad.sThumbLX = (m_State.Gamepad.sThumbLX ^ joykeyState.Gamepad.sThumbLX) | m_State.Gamepad.sThumbLX;
		m_StateTrigger.Gamepad.sThumbLY = (m_State.Gamepad.sThumbLY ^ joykeyState.Gamepad.sThumbLY) | m_State.Gamepad.sThumbLY;

		m_StateRelease.Gamepad.wButtons = (m_State.Gamepad.wButtons & m_StateRelease.Gamepad.wButtons) ^ m_StateRelease.Gamepad.wButtons;	// リリース時の処理
	}

	m_nTimerVibration--;

	if (m_nTimerVibration <= 0)
	{
		m_Vibration.wLeftMotorSpeed = 0;
		m_Vibration.wRightMotorSpeed = 0;

		// ジョイパッドの状態を設定(停止する)
		XInputSetState(0, &m_Vibration);
	}
}

// =================================================
// ジョイパッドの情報取得
// =================================================
bool CJoyPad::GetPress(JOYKEY key)
{
	return (m_State.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

// =================================================
// ジョイパッドの情報取得
// =================================================
bool CJoyPad::GetTrigger(JOYKEY key)
{
	return (m_StateTrigger.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

// =================================================
// ジョイパッドの情報取得
// =================================================
bool CJoyPad::GetRelease(JOYKEY key)
{
	return (m_StateRelease.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

// =================================================
// ジョイパッドスティックの傾き取得
// =================================================
bool CJoyPad::GetStickDir(STICK_TYPE type, D3DXVECTOR2* Vec)
{
	if (type == STICK_TYPE_LEFT)
	{
		(*Vec).x = (float)m_State.Gamepad.sThumbLX / 32767;
		(*Vec).y = (float)-m_State.Gamepad.sThumbLY / 32767;
	}
	else if(type == STICK_TYPE_RIGHT)
	{
		(*Vec).x = (float)m_State.Gamepad.sThumbRX / 32767;
		(*Vec).y = (float)-m_State.Gamepad.sThumbRY / 32767;
	}

	// 動かしたか判定
	if (-DEADZONE <= (*Vec).x && (*Vec).x <= DEADZONE &&
		-DEADZONE <= (*Vec).y && (*Vec).y <= DEADZONE)
	{
		return false;
	}
	else
	{
		return true;
	}
}

// =================================================
// ジョイパッドの情報取得
// =================================================
D3DXVECTOR2 CJoyPad::GetStickDirTrigger(STICK_TYPE type)
{
	D3DXVECTOR2 result = D3DXVECTOR2(0, 0);

	switch (type)
	{
	case STICK_TYPE_LEFT:
		result.x = m_StateTrigger.Gamepad.sThumbLX;
		result.y = m_StateTrigger.Gamepad.sThumbLY;
		break;
	case STICK_TYPE_RIGHT:
		result.x = m_StateTrigger.Gamepad.sThumbLX;
		result.y = m_StateTrigger.Gamepad.sThumbLY;
		break;

	}

	return result;
}

// =================================================
// ジョイパッドの振動処理
// =================================================
void CJoyPad::SetVib(int nValueLeft, int nValueRight, int nTimer)
{
	m_Vibration.wLeftMotorSpeed = nValueLeft;
	m_Vibration.wRightMotorSpeed = nValueRight;

	// ジョイパッドの状態を設定(起動する)
	XInputSetState(0, &m_Vibration);

	m_nTimerVibration = nTimer;
}
