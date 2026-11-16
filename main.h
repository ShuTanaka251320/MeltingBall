// =================================================
// 
// メイン処理[main.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _MAIN_H_
#define _MAIN_H_
#include<windows.h>
#include<stdio.h>
#include"d3dx9.h"							// 描画処理に必要
#define DIRECTINPUT_VERSION	(0x0800)		// ビルド時の警告対処用マクロ
#include "dinput.h"
#include "xaudio2.h"						// サウンドの処理に必要
#include "xinput.h"							// ジョイパッドの処理に必要

class CRenderer;
class CObject;

// =================================================
// ライブラリのリンク
#pragma comment(lib, "d3d9.lib")		// 描画処理に必要
#pragma comment(lib, "d3dx9.lib")		// [d3d9.lib]の拡張ライブラリ
#pragma comment(lib, "dxguid.lib")		// DirectXコンポーネント(部品)使用に必要
#pragma comment(lib, "winmm.lib")		// システム時刻取得に必要
#pragma comment(lib, "dinput8.lib")		// 入力処理に必要
#pragma comment(lib, "xinput.lib")		// ジョイパッド処理に必要

// =================================================
// マクロ定義
#define SCREEN_WIDTH	(1280)			// ウインドウの幅
#define SCREEN_HEIGHT	(720)			// ウインドウの高さ
#define FVF_VERTEX_2D	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)		
#define FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)		
#define FVF_VERTEX_3DMULTI	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEX2)		

// =================================================
// 頂点情報[2D]の構造体を定義
typedef struct
{
	D3DXVECTOR3 pos;	// 頂点座標(x, y, z)
	float rhw;			// 座標交換用係数
	D3DCOLOR col;		// 頂点カラー(r, g, b, a)
	D3DXVECTOR2 tex;	// テクスチャ座標(x, y)
}VERTEX_2D;

// =================================================
// 頂点情報[3D]の構造体を定義
typedef struct
{
	D3DXVECTOR3 pos;	// 頂点座標(x, y, z)
	D3DXVECTOR3 nor;	// 法線ベクトル
	D3DCOLOR col;		// 頂点カラー(r, g, b, a)
	D3DXVECTOR2 tex;	// テクスチャ座標(x, y)
}VERTEX_3D;

// =================================================
// 頂点情報[3D]の構造体を定義
typedef struct
{
	D3DXVECTOR3 pos;	// 頂点座標(x, y, z)
	D3DXVECTOR3 nor;	// 法線ベクトル
	D3DCOLOR col;		// 頂点カラー(r, g, b, a)
	D3DXVECTOR2 tex0;	// テクスチャ座標(x, y)
	D3DXVECTOR2 tex1;	// テクスチャ座標(x, y)
}VERTEX_3DMULTI;

int GetFPS(void);
void SetFinish(bool bFinish);

#endif