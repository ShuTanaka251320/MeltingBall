// =================================================
// 
// デバッグ表示の処理[debugproc.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _DEBUGPROC_H_
#define _DEBUGPROC_H_

#include "main.h"

// =================================================
// マクロ定義
#define MAX_CHAR	(2048)

// =================================================
// デバッグ表示クラス
class CDebugProc
{
public:
	CDebugProc();
	~CDebugProc();

	void Init();
	void Uninit();
	void Update();
	void Draw();
	void PrintDebug(const char* fmt, ...);

private:
	static LPDIRECT3DDEVICE9 m_pDevice;	// デバイスのポインタ
	LPD3DXFONT m_pFont = NULL;	// フォントのポインタ
	char m_aStrDebug[MAX_CHAR];	// デバッグ表示用の文字列
};

#endif