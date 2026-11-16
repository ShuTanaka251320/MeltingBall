// =================================================
// 
// マクロ定義[macro.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _MACRO_H_
#define _MACRO_H_

#include"main.h"

// =================================================
// マクロ定義
#define VEC_NULL		(D3DXVECTOR3(0.0f, 0.0f, 0.0f))		// nullベクトル
#define COLOR_HALFWHITE	(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f))	// 白(半透明)
#define COLOR_WHITE		(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f))	// 白
#define COLOR_RED		(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f))	// 赤
#define COLOR_GREEN		(D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f))	// 緑
#define COLOR_YELLOW	(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f))	// 黄
#define COLOR_ORANGE	(D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f))	// 橙
#define COLOR_BLUE		(D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f))	// 青
#define COLOR_LIGHTBLUE	(D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f))	// 水色
#define COLOR_BLACK		(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f))	// 黒
#define COLOR_HALFBLACK	(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.8f))	// 黒(半透明)
#define COLOR_GLAY		D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f)	// 灰色

#define CHANGERADIUS(X)	(X = (X * D3DX_PI * 0.5f))

#endif