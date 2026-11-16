// =================================================
// 
// メイン処理[main.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include<iostream>
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "object2D.h"

// =================================================
// マクロ定義
#define	CLASS_NAME			"WindowClass"		// クラス名	
#define	WINDOW_NAME			"溶かせ！ ヒートボール！"		// キャプション名(変更必須)

// =================================================
// プロトタイプ宣言
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int g_nCountFPS = 0;	// FPSのカウンタ
bool g_bFinish = false;		// 終了フラグ

// =================================================
// メイン関数
// =================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstancePerv, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WindowProc,
		0,
		0,
		hInstance,
		LoadIcon(NULL, IDI_APPLICATION),
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		LoadIcon(NULL, IDI_APPLICATION)
	};

	HWND hWnd;
	MSG msg;

	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };	// 画面サイズの構造体

	DWORD dwCurrentTime;		// 現在時刻
	DWORD dwExecLastTime;		// 最後に処理した時刻

	DWORD dwFrameCount;			// フレームカウント
	DWORD dwFPSLastTime;		// 最後にFPSを計測した時間

	// ウインドウクラスの登録
	RegisterClassEx(&wcex);

	// クライアント領域を指定のサイズに調整
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// ウインドウを生成
	hWnd = CreateWindowEx(0,
		CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(rect.right - rect.left),
		(rect.bottom - rect.top),
		NULL,
		NULL,
		hInstance,
		NULL);

// **********************************************************************
// ウィンドウズ内の各種処理
	CManager* pManager = NULL;	// マネージャーのインスタンス生成

	// --- メモリの生成 --- //
	if (pManager == NULL)
	{
		pManager = new CManager;
	}

	// --- 初期化処理 --- //
	if (FAILED(pManager->Init(hInstance, hWnd, TRUE)))
	{// 初期化失敗
		return -1;
	}

	// 分解能を設定
	timeBeginPeriod(1);
	dwCurrentTime = 0;					// 初期化
	dwExecLastTime = timeGetTime();		// 現在時刻を取得

	dwFrameCount = 0;					// 初期化
	dwFPSLastTime = timeGetTime();		// 現在時刻を取得

	// ウインドウの表示
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	g_bFinish = false;	// 終了フラグをオフ

	// メッセージループ
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{// windowsの処理
			if (msg.message == WM_QUIT)
			{// WM_QUITメッセージを受け取ったらメッセージループを抜ける
				break;
			}
			else
			{// メッセージの設定
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{// DirectXの処理
			dwCurrentTime = timeGetTime();	// 現在時刻を取得
			if ((dwCurrentTime - dwFPSLastTime) >= 500)
			{
				// FPSを計測
				g_nCountFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);

				dwFPSLastTime = dwCurrentTime;		// FPSを測定した時刻を保存
				dwFrameCount = 0;					// カウントをリセット
			}
			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{// 1/60秒経過
				dwExecLastTime = dwCurrentTime;		// 処理開始時刻を取得
				dwFrameCount++;

				// 更新処理
				pManager->Update();

				// 描画処理
				pManager->Draw();

				if (g_bFinish == true)
				{
					return 0;
				}
			}
		}
	}

	// 分解能を戻す
	timeEndPeriod(1);

	// --- 終了処理 --- //
	// --- 各種メモリの破棄 --- //
	if (pManager != NULL)
	{// メモリの破棄
		// マネージャーの終了処理
		pManager->Uninit();
		delete pManager;
		pManager = NULL;
	}
// **********************************************************************

	// ウインドウクラスの登録を解除	
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	return(int)msg.wParam;
}

// =================================================
// ウインドウプロシージャ
// =================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nID;		// メッセージボックス用の変数

	switch (uMsg)
	{
	case WM_DESTROY:

		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			nID = MessageBox(hWnd, "終了してよろしいですか？", "最終確認", (MB_YESNO | MB_ICONQUESTION));
			if (nID == IDYES)
			{
				// ウインドウを破棄する
				DestroyWindow(hWnd);
			}
			break;
		}
		break;

	case WM_CLOSE:
		nID = MessageBox(hWnd, "終了してよろしいですか？", "最終確認", MB_YESNO | MB_ICONQUESTION);
		if (nID == IDYES)
		{
			// ウインドウを破棄する
			DestroyWindow(hWnd);
		}
		else
		{
			return 0;
		}
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// =================================================
// FPSを渡す
// =================================================
int GetFPS(void)
{
	return g_nCountFPS;
}

// =================================================
// 終了フラグの設定
// =================================================
void SetFinish(bool bFinish)
{
	g_bFinish = bFinish;
}