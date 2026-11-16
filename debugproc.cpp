// =================================================
// 
// デバッグ表示の処理[debugproc.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include<stdio.h>
#include<stdarg.h>
#include"debugproc.h"
#include"renderer.h"
#include"manager.h"
#include"main.h"

// =================================================
// 静的メンバ変数変数
LPDIRECT3DDEVICE9 CDebugProc::m_pDevice = NULL;

// =================================================
// コンストラクタ
// =================================================
CDebugProc::CDebugProc()
{
	// --- メンバ変数のクリア --- //
	m_pDevice = NULL;		// デバイスのポインタ
	m_pFont = NULL;			// フォントのポインタ
	memset(&m_aStrDebug[0], 0, sizeof m_aStrDebug);	// デバッグ表示用の文字列
}

// =================================================
// デストラクタ
// =================================================
CDebugProc::~CDebugProc()
{

}

// =================================================
// 初期化処理
// =================================================
void CDebugProc::Init(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	m_pDevice = pRenderer->GetDevice();

	// デバッグ表示用フォントの設定
	D3DXCreateFont(m_pDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
		"Terminal", &m_pFont);

	// デバッグ情報バッファのクリア
	memset(&m_aStrDebug[0], 0, sizeof m_aStrDebug);
}

// =================================================
// 終了処理
// =================================================
void CDebugProc::Uninit(void)
{
	// デバッグ表示用のフォントの破棄
	if (m_pFont != NULL)
	{
		m_pFont->Release();
		m_pFont = NULL;
	}
}

// =================================================
// 更新処理
// =================================================
void CDebugProc::Update(void)
{
}

// =================================================
// 描画処理
// =================================================
void CDebugProc::Draw(void)
{
	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	//char aStr[256];		// 画面に表示される文字列
#if _DEBUG
	// テキストの描画
	m_pFont->DrawTextA(NULL, m_aStrDebug, -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));
#endif
	// デバッグ情報バッファのクリア
	memset(&m_aStrDebug[0], 0, sizeof m_aStrDebug);
}

// =================================================
// デバッグ情報を登録
// =================================================
void CDebugProc::PrintDebug(const char* fmt, ...)
{
	va_list ap;
	char aStr[MAX_CHAR] = {};	// %〇以外の中身を保存
	char aStr2[MAX_CHAR] = {};	// 引数の中身を一時保存

	strcpy(&aStr[0], fmt);	// 保存

	// 可変長引数を1個の変数にまとめる
	va_start(ap, fmt);

	while (*fmt != '\0')
	{
		if (*fmt == '%')
		{
			fmt++;
			//nCnt++;
			switch (*fmt)
			{
			case 'f':
				//va_arg(ap, double);
				sprintf(&aStr2[0], "%.2f", va_arg(ap, double));
				// 取り出した情報をデバッグ情報バッファに連結する
				strcat(&m_aStrDebug[0], &aStr2[0]);
				break;

			case 'd':
				sprintf(&aStr2[0], "%d", va_arg(ap, int));
				// 取り出した情報をデバッグ情報バッファに連結する
				strcat(&m_aStrDebug[0], &aStr2[0]);
				break;
			case 'c':
				sprintf(&aStr2[0], "%c", va_arg(ap, char));
				// 取り出した情報をデバッグ情報バッファに連結する
				strcat(&m_aStrDebug[0], &aStr2[0]);
				break;

			case 's':
				sprintf(&aStr2[0], "%s", va_arg(ap, char*));
				// 取り出した情報をデバッグ情報バッファに連結する
				strcat(&m_aStrDebug[0], &aStr2[0]);
				break;
			}
		}
		else
		{
			sprintf(&aStr2[0], "%c", *fmt);
			strcat(&m_aStrDebug[0], &aStr2[0]);
		}

		fmt++;
		//nCnt++;
		// デバッグ情報バッファのクリア
		memset(&aStr2[0], 0, sizeof aStr2);
	}

	va_end(ap);
}
