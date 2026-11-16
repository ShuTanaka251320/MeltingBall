// =================================================
// 
// Xモデル処理[Xmodel.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include"Xmodel.h"
#include"manager.h"
#include"renderer.h"
#include"texture.h"

// =================================================
// 静的メンバ変数宣言
int CXModel::m_nNumAll = 0;

// =================================================
// コンストラクタ
// =================================================
CXModel::CXModel()
{
	memset(m_apModelName, 0, sizeof(m_apModelName));
	memset(m_aModelName, 0, sizeof(m_aModelName));
	memset(m_vtxMin, 0, sizeof(m_vtxMin));
	memset(m_vtxMax, 0, sizeof(m_vtxMax));
	memset(m_nIdxtex, 0, sizeof(m_nIdxtex));
	memset(m_pMesh, 0, sizeof(m_pMesh));
	memset(m_pBuffMat, 0, sizeof(m_pBuffMat));
	memset(m_dwNumMat, 0, sizeof(m_dwNumMat));
}

// =================================================
// デストラクタ
// =================================================
CXModel::~CXModel()
{

}

// =================================================
// テクスチャの読み込み(必ず使うであろう)
// =================================================
HRESULT CXModel::Load(void)
{
	return S_OK;
}

// =================================================
// テクスチャの破棄
// =================================================
void CXModel::Unload(void)
{
	for (int nCnt = 0; nCnt < MAX_TEX; nCnt++)
	{
		if (m_pBuffMat[nCnt] != NULL)
		{
			m_pBuffMat[nCnt] = NULL;
			m_pMesh[nCnt] = NULL;
			m_dwNumMat[nCnt] = NULL;
			m_nNumAll--;
		}
	}
}

// =================================================
// テクスチャの登録
// =================================================
int CXModel::Register(const char* pFileName)
{
	int nIdx = m_nNumAll;	// アドレスを保存

	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	int nNumVtx;		// 頂点数
	DWORD dwSizeFVF;	// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;				// 頂点バッファへのポインタ

	for (int nCnt = 0; nCnt < MAX_MODEL; nCnt++)
	{
		if (m_aModelName[nCnt] == pFileName)
		{
			return nCnt;	// パスがまったく同じのが存在している場合、その時のインデックスを返す		
		}
	}

	// --- 今まで読み込んだことのないモデル --- //
	for (int nCnt = 0; nCnt < MAX_MAT; nCnt++)
	{
		m_nIdxtex[nIdx][nCnt] = -1;
	}

	// Xファイルの読み込み
	D3DXLoadMeshFromX(pFileName,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&m_pBuffMat[nIdx],
		NULL,
		&m_dwNumMat[nIdx],
		&m_pMesh[nIdx]);

	D3DXMATERIAL* pMat;

	if (m_pBuffMat != NULL)
	{
		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)m_pBuffMat[nIdx]->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)m_dwNumMat[nIdx]; nCntMat++)
		{// マテリアルの数分回す
			if (pMat[nCntMat].pTextureFilename != NULL)
			{// テクスチャファイルが存在する
				// --- テクスチャのインデックスを取得 --- //
				m_nIdxtex[nIdx][nCntMat] = pTexture->Register(pMat[nCntMat].pTextureFilename);
			}
		}
	}
	if (m_pMesh != NULL)
	{
		// 頂点数を取得
		 nNumVtx = m_pMesh[nIdx]->GetNumVertices();

		// 頂点フォーマットのサイズを取得
		 dwSizeFVF = D3DXGetFVFVertexSize(m_pMesh[nIdx]->GetFVF());

		// 頂点バッファをロック
		m_pMesh[nIdx]->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
		{
			D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;

			// すべての頂点を比較してモデルの最小値、最大値を抜き出す
			// 最小値
			if (vtx.x <= m_vtxMin[nIdx].x)
			{
				m_vtxMin[nIdx].x = vtx.x;
			}
			if (vtx.y <= m_vtxMin[nIdx].y)
			{
				m_vtxMin[nIdx].y = vtx.y;
			}
			if (vtx.z <= m_vtxMin[nIdx].z)
			{
				m_vtxMin[nIdx].z = vtx.z;
			}
			// 最大値
			if (vtx.x >= m_vtxMax[nIdx].x)
			{
				m_vtxMax[nIdx].x = vtx.x;
			}
			if (vtx.y >= m_vtxMax[nIdx].y)
			{
				m_vtxMax[nIdx].y = vtx.y;
			}
			if (vtx.z >= m_vtxMax[nIdx].z)
			{
				m_vtxMax[nIdx].z = vtx.z;
			}

			pVtxBuff += dwSizeFVF;
		}

		// メッシュをアンロック
		m_pMesh[nIdx]->UnlockVertexBuffer();
	}

	// --- モデルのパスを保存 --- //
	m_apModelName[nIdx] = (char*)pFileName;
	m_aModelName[nIdx] = m_apModelName[nIdx];

	m_nNumAll++;	// 総数のカウントアップ

	return nIdx;	// インデックスを返す
}

