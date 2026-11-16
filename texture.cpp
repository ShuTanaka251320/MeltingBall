// =================================================
// 
// テクスチャ処理[texture.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include"texture.h"
#include"manager.h"
#include"renderer.h"

// =================================================
// 静的メンバ変数宣言
int CTexture::m_nNumAll = 0;

// =================================================
// コンストラクタ
// =================================================
CTexture::CTexture()
{
	memset(m_apTexName, 0, sizeof(m_apTexName));
	memset(m_apTexture, 0, sizeof(m_apTexture));
	memset(m_aTexName, 0, sizeof(m_aTexName));
}

// =================================================
// デストラクタ
// =================================================
CTexture::~CTexture()
{

}

// =================================================
// テクスチャの読み込み(必ず使うであろう)
// =================================================
HRESULT CTexture::Load(void)
{
	return S_OK;
}

// =================================================
// テクスチャの破棄
// =================================================
void CTexture::Unload(void)
{
	for (int nCnt = 0; nCnt < MAX_TEX; nCnt++)
	{
		if (m_apTexture[nCnt] != NULL)
		{
			m_apTexture[nCnt]->Release();
			m_apTexture[nCnt] = NULL;
			m_nNumAll--;
		}
	}
}

// =================================================
// テクスチャの登録
// =================================================
int CTexture::Register(const char* pFileName)
{

	int nIdx = m_nNumAll;	// アドレスを保存

	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	for (int nCnt = 0; nCnt < MAX_TEX; nCnt++)
	{
		if (m_aTexName[nCnt] == pFileName)
		{
			return nCnt;	// パスがまったく同じのが存在している場合、その時のインデックスを返す		
		}
	}

	// --- 今まで読み込んだことのないテクスチャ --- //
	// --- テクスチャの読み込み --- // 
	D3DXCreateTextureFromFile(pDevice,
		pFileName,
		&m_apTexture[nIdx]);

	// --- テクスチャのパスを保存 --- //
	m_apTexName[nIdx] = (char*)pFileName;
	m_aTexName[nIdx] = m_apTexName[nIdx];

	m_nNumAll++;	// 総数のカウントアップ

	return nIdx;	// インデックスを返す
}

// =================================================
// アドレス取得
// =================================================
LPDIRECT3DTEXTURE9 CTexture::GetAddress(int nIdx)
{
	if (nIdx >= 0)
	{
		return m_apTexture[nIdx];
	}
	else
	{
		return NULL;
	}
}
