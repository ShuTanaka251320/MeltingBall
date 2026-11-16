// =================================================
// 
// テクスチャ処理[texture.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include"main.h"
#include<string>

using namespace std;

// =================================================
// マクロ定義
#define MAX_TEX			(128)	// テクスチャの最大数
#define MAX_CHAR_NUM	(128)	// 文字の最大数

// =================================================
// クラス定義
class CTexture
{
public:
	CTexture();
	~CTexture();

	HRESULT Load(void);
	 void Unload(void);

	int Register(const char *pFileName);
	 LPDIRECT3DTEXTURE9 GetAddress(int nIdx);

private:
	LPDIRECT3DTEXTURE9 m_apTexture[MAX_TEX];	// テクスチャのポインタ
	char *m_apTexName[MAX_TEX];		// テクスチャの名前アドレスを保存
	string m_aTexName[MAX_TEX];		// テクスチャの名前を保存
	static int m_nNumAll;	// テクスチャの総数
};

#endif