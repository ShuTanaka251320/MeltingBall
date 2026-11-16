// =================================================
// 
// Xオブジェクト処理[objectX.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "objectX.h"
#include "camera.h"
#include "manager.h"
#include "macro.h"
#include "texture.h"
#include "renderer.h"
#include "game.h"

// =================================================
// コンストラクタ
// =================================================
CObjectX::CObjectX(int nPriority) :CObject(nPriority)
{
	// --- メンバ変数のクリア --- //
	m_pMesh = NULL;				// メッシュ(頂点情報)へのポインタ
	m_pBuffMat = NULL;			// マテリアルのポインタ
	m_dwNumMat = NULL;			// マテリアルの数
	m_nIdxModelParent = 0;		// 親のインデックス
	m_pos = VEC_NULL;			// 位置(オフセット)
	m_rot = VEC_NULL;			// 向き
	m_move = VEC_NULL;			// 移動量
	m_mtxWorld;					// ワールドマトリックス
	m_vtxMin = VEC_NULL;		// モデルの最小値
	m_vtxMax = VEC_NULL;		// モデルの最大値
	m_bDisp = false;			// 表示するかどうか
	memset(m_nIdxtex, 0, sizeof(m_nIdxtex));
}

// =================================================
// デストラクタ
// =================================================
CObjectX::~CObjectX()
{

}

// =================================================
// 生成処理
// =================================================
CObjectX* CObjectX::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, CObject::TYPE type, const char* ModelFileName)
{
	CObjectX* pObjectX;	// ポインタ宣言

	// --- メモリの生成 --- //
	pObjectX = new CObjectX;

	// --- 初期化処理 --- //
	pObjectX->Init(pos, rot, type, ModelFileName);

	return pObjectX;
}

// =================================================
// 初期化処理
// =================================================
HRESULT CObjectX::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, CObject::TYPE type, const char* ModelFileName)
{
	int nNumVtx;		// 頂点数
	DWORD dwSizeFVF;	// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;				// 頂点バッファへのポインタ

	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	// 各種変数を初期化
	m_pos = pos;
	m_rot = rot;
	m_vtxMin = D3DXVECTOR3(10000, 10000, 10000);
	m_vtxMax = D3DXVECTOR3(-10000, -10000, -10000);
	m_dwNumMat = 0;
	SetType(type);		// タイプ設定

	for (int nCnt = 0; nCnt < MAX_MAT; nCnt++)
	{
		m_nIdxtex[nCnt] = -1;
	}

	// Xファイルの読み込み
	D3DXLoadMeshFromX(ModelFileName,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&m_pBuffMat,
		NULL,
		&m_dwNumMat,
		&m_pMesh);

	D3DXMATERIAL* pMat;

	if (m_pBuffMat != NULL)
	{
		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
		{// マテリアルの数分回す
			if (pMat[nCntMat].pTextureFilename != NULL)
			{// テクスチャファイルが存在する
				// --- テクスチャのインデックスを取得 --- //
				m_nIdxtex[nCntMat] = pTexture->Register(pMat[nCntMat].pTextureFilename);
			}
		}
	}
	if (m_pMesh != NULL)
	{
		// 頂点数を取得
		nNumVtx = m_pMesh->GetNumVertices();

		// 頂点フォーマットのサイズを取得
		dwSizeFVF = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

		// 頂点バッファをロック
		m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
		{
			D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;

			// すべての頂点を比較してモデルの最小値、最大値を抜き出す
			// 最小値
			if (vtx.x <= m_vtxMin.x)
			{
				m_vtxMin.x = vtx.x;
			}
			if (vtx.y <= m_vtxMin.y)
			{
				m_vtxMin.y = vtx.y;
			}
			if (vtx.z <= m_vtxMin.z)
			{
				m_vtxMin.z = vtx.z;
			}
			// 最大値
			if (vtx.x >= m_vtxMax.x)
			{
				m_vtxMax.x = vtx.x;
			}
			if (vtx.y >= m_vtxMax.y)
			{
				m_vtxMax.y = vtx.y;
			}
			if (vtx.z >= m_vtxMax.z)
			{
				m_vtxMax.z = vtx.z;
			}

			pVtxBuff += dwSizeFVF;
		}

		// メッシュをアンロック
		m_pMesh->UnlockVertexBuffer();
	}

	return S_OK;
}

// =================================================
// 初期化処理(オーバーロード)
// =================================================
float CObjectX::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, CObject::TYPE type, bool bCollider, const char* ModelFileName)
{
	int nNumVtx;		// 頂点数
	DWORD dwSizeFVF;	// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;				// 頂点バッファへのポインタ

	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	// 各種変数を初期化
	m_pos = pos;
	m_rot = rot;
	m_vtxMin = D3DXVECTOR3(10000, 10000, 10000);
	m_vtxMax = D3DXVECTOR3(-10000, -10000, -10000);
	m_dwNumMat = 0;
	SetType(type);		// タイプ設定

	for (int nCnt = 0; nCnt < MAX_MAT; nCnt++)
	{
		m_nIdxtex[nCnt] = -1;	// テクスチャインデックスの初期化
	}

	// Xファイルの読み込み
	D3DXLoadMeshFromX(ModelFileName,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&m_pBuffMat,
		NULL,
		&m_dwNumMat,
		&m_pMesh);

	D3DXMATERIAL* pMat;

	if (m_pBuffMat != NULL)
	{
		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
		{// マテリアルの数分回す
			if (pMat[nCntMat].pTextureFilename != NULL)
			{// テクスチャファイルが存在する
				// --- テクスチャのインデックスを取得 --- //
				m_nIdxtex[nCntMat] = pTexture->Register(pMat[nCntMat].pTextureFilename);
			}
		}
	}
	if (m_pMesh != NULL)
	{
		// 頂点数を取得
		nNumVtx = m_pMesh->GetNumVertices();

		// 頂点フォーマットのサイズを取得
		dwSizeFVF = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

		// 頂点バッファをロック
		m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
		{
			D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;

			// すべての頂点を比較してモデルの最小値、最大値を抜き出す
			// 最小値
			if (vtx.x <= m_vtxMin.x)
			{
				m_vtxMin.x = vtx.x;
			}
			if (vtx.y <= m_vtxMin.y)
			{
				m_vtxMin.y = vtx.y;
			}
			if (vtx.z <= m_vtxMin.z)
			{
				m_vtxMin.z = vtx.z;
			}
			// 最大値
			if (vtx.x >= m_vtxMax.x)
			{
				m_vtxMax.x = vtx.x;
			}
			if (vtx.y >= m_vtxMax.y)
			{
				m_vtxMax.y = vtx.y;
			}
			if (vtx.z >= m_vtxMax.z)
			{
				m_vtxMax.z = vtx.z;
			}

			pVtxBuff += dwSizeFVF;
		}

		// メッシュをアンロック
		m_pMesh->UnlockVertexBuffer();
	}

	float fSurface;

	// --- モデルの面積を算出 --- //
	fSurface = (m_vtxMax.x - m_vtxMin.x) * (m_vtxMax.z - m_vtxMin.z);


	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);	// かけ合わせる

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans,
		m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);	// かけ合わせる

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	return fSurface;
}

// =================================================
// 終了処理
// =================================================
void CObjectX::Uninit(void)
{
	// メッシュの破棄
	if (m_pMesh != NULL)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}

	// マテリアルの破棄
	if (m_pBuffMat != NULL)
	{
		m_pBuffMat->Release();
		m_pBuffMat = NULL;
	}

	Release();
}

// =================================================
// 更新処理
// =================================================
void CObjectX::Update(void)
{

}


// =================================================
// 描画処理
// =================================================
void CObjectX::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;				// マテリアルデータへのポインタ

	D3DMATERIAL9 pMatShadow;		// シャドウマテリアルデータへのポインタ
	D3DXMATRIX mtxShadow;	// 影のマトリックス
	D3DLIGHT9 light;		// ライト情報
	D3DXVECTOR4 posLight;	// ライトの位置
	D3DXVECTOR3 pos, normal;	// 平面上の任意の点、法線ベクトル
	D3DXPLANE plane;		// 平面情報

	// --- ライトの位置を設定 --- //
	pDevice->GetLight(0, &light);
	posLight = D3DXVECTOR4(-light.Direction.x, -light.Direction.y * 0.7f, -light.Direction.z, 0.0f);

	float fHeight = 1.0f;

	// --- 平面情報の作成 --- //
	pos = D3DXVECTOR3(0, 2.0f, 0);
	normal = D3DXVECTOR3(0.0f, fHeight, 0.0f);
	D3DXPlaneFromPointNormal(&plane, &pos, &normal);

	// シャドウマトリックスの初期化
	D3DXMatrixIdentity(&mtxShadow);

	// --- シャドウマトリックスの生成 --- //
	D3DXMatrixShadow(&mtxShadow, &posLight, &plane);
	D3DXMatrixMultiply(&mtxShadow, &m_mtxWorld, &mtxShadow);

	// --- シャドウマトリックスを設定 --- //
	pDevice->SetTransform(D3DTS_WORLD, &mtxShadow);

	// 現在のマテリアルを取得(保存)
	pDevice->GetMaterial(&matDef);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		pMatShadow = pMat[nCntMat].MatD3D;
		pMatShadow.Diffuse = { 0.0f, 0.0f, 0.0f, 1.0f };	// 黒に設定

		// マテリアルの設定
		pDevice->SetMaterial(&pMatShadow);

		// テクスチャの設定
		pDevice->SetTexture(0, NULL);

		// モデルの描画
		m_pMesh->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);	// かけ合わせる

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans,
		m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);	// かけ合わせる

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 現在のマテリアルを取得(保存)
	pDevice->GetMaterial(&matDef);

	if (m_pBuffMat != NULL)
	{
		if (m_pMesh != NULL)
		{
			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
			{
				// マテリアルの設定
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

				if (m_nIdxtex >= 0)
				{
					// テクスチャの設定
					pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxtex[nCntMat]));
					//pDevice->SetTexture(0, NULL);
				}
				else
				{// テクスチャが存在しない
					// テクスチャの設定
					pDevice->SetTexture(0, NULL);
				}

				// モデルの描画
				m_pMesh->DrawSubset(nCntMat);
			}
		}
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

// =================================================
// 描画処理(シャドウマトリックス付き)
// =================================================
void CObjectX::Draw(int type)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;				// マテリアルデータへのポインタ

	D3DMATERIAL9 pMatShadow;		// シャドウマテリアルデータへのポインタ
	D3DXMATRIX mtxShadow;	// 影のマトリックス
	D3DLIGHT9 light;		// ライト情報
	D3DXVECTOR4 posLight;	// ライトの位置
	D3DXVECTOR3 pos, normal;	// 平面上の任意の点、法線ベクトル
	D3DXPLANE plane;		// 平面情報

	//if (type != (int)CGimmick::TYPE_STUDIUM && type != (int)CGimmick::TYPE_NONE)
	//{
	//	// --- ライトの位置を設定 --- //
	//	pDevice->GetLight(0, &light);
	//	posLight = D3DXVECTOR4(-light.Direction.x * 0.2f, -light.Direction.y, -light.Direction.z * 0.2f, 0.0f);

	//	float fHeight = 1.0f;

	//	//if (CManager::GetMode() == MODE_GAME)
	//	//{
	//	//	CMeshField* pMesh = CGame::GetMesh();
	//	//	fHeight = pMesh->GetPosHeight(m_pos, m_pos);
	//	//}

	//	// --- 平面情報の作成 --- //
	//	pos = D3DXVECTOR3(0, 5, 0);
	//	normal = D3DXVECTOR3(0.0f, fHeight, 0.0f);
	//	D3DXPlaneFromPointNormal(&plane, &pos, &normal);

	//	// シャドウマトリックスの初期化
	//	D3DXMatrixIdentity(&mtxShadow);

	//	// --- シャドウマトリックスの生成 --- //
	//	D3DXMatrixShadow(&mtxShadow, &posLight, &plane);
	//	D3DXMatrixMultiply(&mtxShadow, &m_mtxWorld, &mtxShadow);

	//	// --- シャドウマトリックスを設定 --- //
	//	pDevice->SetTransform(D3DTS_WORLD, &mtxShadow);

	//	// 現在のマテリアルを取得(保存)
	//	pDevice->GetMaterial(&matDef);

	//	// マテリアルデータへのポインタを取得
	//	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	//	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	//	{
	//		pMatShadow = pMat[nCntMat].MatD3D;
	//		pMatShadow.Diffuse = { 0.0f, 0.0f, 0.0f, 0.9f };	// 黒に設定

	//		// マテリアルの設定
	//		pDevice->SetMaterial(&pMatShadow);

	//		if (m_nIdxtex >= 0)
	//		{
	//			// テクスチャの設定
	//			pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxtex[nCntMat]));
	//			//pDevice->SetTexture(0, NULL);
	//		}
	//		else
	//		{// テクスチャが存在しない
	//			// テクスチャの設定
	//			pDevice->SetTexture(0, NULL);
	//		}

	//		// モデルの描画
	//		m_pMesh->DrawSubset(nCntMat);
	//	}

	//	// 保存していたマテリアルを戻す
	//	pDevice->SetMaterial(&matDef);
	//}


	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
		m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);	// かけ合わせる

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans,
		m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);	// かけ合わせる

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 現在のマテリアルを取得(保存)
	pDevice->GetMaterial(&matDef);

	if (m_pBuffMat != NULL)
	{
		if (m_pMesh != NULL)
		{
			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
			{
				// マテリアルの設定
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

				if (m_nIdxtex >= 0)
				{
					// テクスチャの設定
					pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxtex[nCntMat]));
					//pDevice->SetTexture(0, NULL);
				}
				else
				{// テクスチャが存在しない
					// テクスチャの設定
					pDevice->SetTexture(0, NULL);
				}

				// モデルの描画
				m_pMesh->DrawSubset(nCntMat);
			}
		}
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

// =================================================
// 向きの正規化
// =================================================
void CObjectX::NorRotate(float fRotDest)
{
	// --- カメラ情報の取得 --- //
	CCamera* pCamera = CGame::GetCamera();

	D3DXVECTOR3 CameraRot = pCamera->GetRotation();

	// --- モデルの向きの設定 --- //
	m_rotDest.y = fRotDest;	// 目標の角度を設定
	m_rotDiff.y = m_rotDest.y - m_rot.y;			// 現在と目標の角度の差分を算出

	// もし、差分がπを超えたら
	if (m_rotDiff.y > D3DX_PI)
	{
		m_rotDiff.y -= D3DX_PI * 2;
	}
	else if (m_rotDiff.y < -D3DX_PI)
	{
		m_rotDiff.y += D3DX_PI * 2;
	}

	// 回転に補正を掛ける
	m_rot.y += m_rotDiff.y * 0.2f;

	// もし、現在の角度がπを超えたら
	if (m_rot.y > D3DX_PI + CameraRot.y)
	{
		m_rot.y -= D3DX_PI * 2;
	}
	else if (m_rot.y < -D3DX_PI + CameraRot.y)
	{
		m_rot.y += D3DX_PI * 2;
	}
}
