// =================================================
// 
// モデル処理[model.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "model.h"
#include "camera.h"
#include "manager.h"
#include "macro.h"
#include "texture.h"
#include "renderer.h"
#include "player.h"
#include "game.h"

// =================================================
// マクロ定義
#define	MODEL_MOVE	(10.0f)	// モデルの移動量
#define	MAX_FANCE	(21)	// 柵の最大数
#define	PLAYER_XFIX	(20)	// プレイヤーのXサイズの補正
#define	PLAYER_YFIX	(00)	// プレイヤーのYサイズの補正
#define	PLAYER_ZFIX	(20)	// プレイヤーのZサイズの補正

// =================================================
// コンストラクタ
// =================================================
CModel::CModel()
{
	// --- メンバ変数のクリア --- //
	m_pMesh = NULL;				// メッシュ(頂点情報)へのポインタ
	m_pBuffMat = NULL;			// マテリアルのポインタ
	m_dwNumMat = NULL;			// マテリアルの数
	m_nIdx = 0;					// 自分のインデックス
	m_nIdxParent = 0;			// 親のインデックス
	m_pos = VEC_NULL;			// 位置(オフセット)
	m_rot = VEC_NULL;			// 向き
	m_move = VEC_NULL;			// 移動量
	m_mtxWorld;					// ワールドマトリックス
	m_vtxMin = VEC_NULL;		// モデルの最小値
	m_vtxMax = VEC_NULL;		// モデルの最大値
	m_bDisp = false;			// 表示するかどうか
	memset(m_nIdxtex, 0, sizeof(m_nIdxtex));
	m_col = COLOR_WHITE;	// 色
}

// =================================================
// デストラクタ
// =================================================
CModel::~CModel()
{

}

// =================================================
// 生成処理
// =================================================
CModel* CModel::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdx, int nIdxParent, const char* ModelFileName)
{
	CModel* pModel;	// ポインタ宣言

	// --- メモリの生成 --- //
	pModel = new CModel;

	// --- 初期化処理 --- //
	pModel->Init(pos, rot, nIdx, nIdxParent, ModelFileName);

	return pModel;
}

// =================================================
// 初期化処理
// =================================================
HRESULT CModel::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdx, int nIdxParent, const char* ModelFileName)
{
	int nNumVtx;		// 頂点数
	DWORD dwSizeFVF;	// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;				// 頂点バッファへのポインタ

	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	// --- 各種変数を初期化 --- //
	m_pos = pos;	// 位置
	m_rot = rot;	// 向き	
	m_posOffSet = pos;	// 位置(オフセット)
	m_rotOffSet = rot;	// 向き(オフセット)
	m_vtxMin = D3DXVECTOR3(10000, 10000, 10000);	// モデルの最小の位置
	m_vtxMax = D3DXVECTOR3(-10000, -10000, -10000);	// モデルの最大の位置
	m_dwNumMat = 0;	// マテリアル数
	m_col = COLOR_WHITE;	// 色
	m_nIdx = nIdx;				// 自分のインデックス
	m_nIdxParent = nIdxParent;	// 親のインデックス

	// --- テクスチャインデックスの初期化 --- //
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
// 終了処理
// =================================================
void CModel::Uninit(void)
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
}

// =================================================
// 更新処理
// =================================================
void CModel::Update(void)
{

}

// =================================================
// 描画処理
// =================================================
void CModel::Draw(const D3DXMATRIX* pMtxParent)
{
	CPlayer* pPlayer = CGame::GetPlayer();

	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;				// マテリアルデータへのポインタ
	D3DXMATRIX mtxParent;			// 計算用マトリックス

	D3DMATERIAL9 pMatShadow;		// シャドウマテリアルデータへのポインタ
	D3DXMATRIX mtxShadow;	// 影のマトリックス
	D3DLIGHT9 light;		// ライト情報
	D3DXVECTOR4 posLight;	// ライトの位置
	D3DXVECTOR3 pos, normal;	// 平面上の任意の点、法線ベクトル
	D3DXPLANE plane;		// 平面情報

	// --- ライトの位置を設定 --- //
	pDevice->GetLight(0, &light);
	posLight = D3DXVECTOR4(-light.Direction.x, -light.Direction.y * 0.8f, -light.Direction.z, 0.0f);

	float fHeight = 1.0f;

	// --- 平面情報の作成 --- //
	pos = D3DXVECTOR3(0, pPlayer->GetPosField() + 2.0f, 0);
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

	// --- 親モデルの確認 --- //
	if (m_pParent != NULL)
	{// 親モデルが存在している
		mtxParent = m_pParent->GetMtxWorld();
	}
	else
	{// 自分が親モデル
		mtxParent = *pMtxParent;
	}

	// --- 親のモデルとかけ合わせる --- //
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);

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
				if (pPlayer->GetheatGauge() >= NUM_SETMATERIALRED)
				{
					// --- 赤に変色 --- //
					m_col.r = 1.0f;
					m_col.g = 0.0f;
					m_col.b = 0.0f;
				}
				else if (pPlayer->GetheatGauge() < NUM_SETMATERIALBLUE)
				{
					// --- 青に変色 --- //
					m_col.b = 1.0f;
					m_col.r = 0.0f;
					m_col.g = 0.0f;
				}

				pMat[0].MatD3D.Diffuse = m_col;	// マテリアルの色を反映

				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);	// マテリアルを設定

				if (m_nIdxtex[nCntMat] >= 0)
				{
					// テクスチャの設定
					pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxtex[nCntMat]));
				}
				else
				{// テクスチャが存在しない
					// テクスチャの設定
					pDevice->SetTexture(0, NULL);
				}

				// プレイヤー(パーツ)の描画
				m_pMesh->DrawSubset(nCntMat);
			}
		}
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

// =================================================
// モデルの回転処理
// =================================================
void CModel::ModelRotate(float fRotDest)
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
