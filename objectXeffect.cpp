// =================================================
// 
// Xオブジェクトでのパーティクル処理[objectX.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "objectXeffect.h"
#include "camera.h"
#include "manager.h"
#include "macro.h"
#include "texture.h"
#include "renderer.h"
#include "game.h"
#include "math.h"
#include "xmodel.h"

// =================================================
// コンストラクタ
// =================================================
CObjectXEffect::CObjectXEffect(int nPriority) :CObject(nPriority)
{
	// --- メンバ変数のクリア --- //
	m_pos = VEC_NULL;			// 位置(オフセット)
	m_rot = VEC_NULL;			// 向き
	m_move = VEC_NULL;			// 移動量
	m_scl = VEC_NULL;			// スケール量
	m_mtxWorld;					// ワールドマトリックス
	m_bDisp = false;			// 表示するかどうか
	m_nLife = 0;	// 寿命
	m_sin = 0.0f;	// sinカーブ用
}

// =================================================
// デストラクタ
// =================================================
CObjectXEffect::~CObjectXEffect()
{

}

// =================================================
// 生成処理
// =================================================
CObjectXEffect* CObjectXEffect::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float scale, int nLife, CObject::TYPE type, int nPri, const char* pFileName)
{
	CObjectXEffect* pObjectX;	// ポインタ宣言

	// --- メモリの生成 --- //
	pObjectX = new CObjectXEffect;

	// --- 初期化処理 --- //
	pObjectX->Init(pos, rot, move, col, scale, nLife, type, nPri, pFileName);

	return pObjectX;
}

// =================================================
// 初期化処理
// =================================================
HRESULT CObjectXEffect::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float scale, int nLife, CObject::TYPE type, int nPri, const char* pFileName)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	CXModel* pXModel = CManager::GetXModel();	// モデルマネージャの取得

	m_nIdxModel = pXModel->Register(pFileName);

	// 各種変数を初期化
	m_pos = pos;
	m_move = move;
	m_rot = rot;
	m_rotate = rot;
	m_scl = { scale, scale, scale };
	m_nLife = nLife;
	m_sin = 0.0f;

	SetType(type);		// タイプ設定


	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CObjectXEffect::Uninit(void)
{

	Release();
}

// =================================================
// 更新処理
// =================================================
void CObjectXEffect::Update(void)
{
	//m_sin += 0.0002f;
	//m_rotate.y += sinf(m_sin) * 1.0f;

	//m_rot += m_rotate * 0.1f;

	m_fSaveMove = m_move.y;	// 移動量の保存

	// --- 重力をかける --- //
	m_move.y += -0.3f;

	// --- 位置の更新 --- //
	m_pos += m_move;

	if (m_bJump == false && m_bSetJumpForce == false)
	{
		//m_pos.y = 20.0f;
	}

	// --- バウンド処理 --- //
	Bound();

	m_nLife--;

	if (m_nLife <= 0)
	{
		Uninit();
	}

	m_rot = CMath::RepairRot(&m_rot);
}


// =================================================
// 描画処理
// =================================================
void CObjectXEffect::Draw(void)
{
	CXModel* pXModel = CManager::GetXModel();	// モデルマネージャの取得

	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	D3DXMATRIX mtxScaling, mtxRot, mtxTrans;	// 計算用マトリックス
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
	pMat = (D3DXMATERIAL*)pXModel->GetBuffMat(m_nIdxModel)->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)pXModel->GetNumMaterial(m_nIdxModel); nCntMat++)
	{
		pMatShadow = pMat[nCntMat].MatD3D;
		pMatShadow.Diffuse = { 0.0f, 0.0f, 0.0f, 1.0f };	// 黒に設定

		// マテリアルの設定
		pDevice->SetMaterial(&pMatShadow);

		// テクスチャの設定
		pDevice->SetTexture(0, NULL);

		// モデルの描画
		pXModel->GetMesh(m_nIdxModel)->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// --- スケールの調整 --- //
	D3DXMatrixScaling(&mtxScaling, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScaling);	// かけ合わせる

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

	if (pXModel->GetBuffMat(m_nIdxModel) != NULL)
	{
		if (pXModel->GetMesh(m_nIdxModel) != NULL)
		{
			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)pXModel->GetBuffMat(m_nIdxModel)->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)pXModel->GetNumMaterial(m_nIdxModel); nCntMat++)
			{
				// マテリアルの設定
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

				if (pXModel->GetMaterialtex(m_nIdxModel, nCntMat) >= 0)
				{
					// テクスチャの設定
					pDevice->SetTexture(0, pTexture->GetAddress(pXModel->GetMaterialtex(m_nIdxModel, nCntMat)));
					//pDevice->SetTexture(0, NULL);
				}
				else
				{// テクスチャが存在しない
					// テクスチャの設定
					pDevice->SetTexture(0, NULL);
				}

				// モデルの描画
				pXModel->GetMesh(m_nIdxModel)->DrawSubset(nCntMat);
			}
		}
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

// =================================================
// バウンド処理
// =================================================
void CObjectXEffect::Bound(void)
{
	// --- バウンド力の設定 --- //
	if (m_move.y < m_fSaveMove && m_move.y != m_fRebound && m_bSetJumpForce == false)
	{
		m_fRebound = m_move.y;
		m_bSetJumpForce = true;
	}

	if (m_pos.y <= 20.0)
	{
		//m_fRebound += (0.0f - m_fRebound) * 0.2f;	// バウンド力の減衰
		m_move.y = m_fRebound * 0.75f;	// バウンド力の設定

		m_bSetJumpForce = false;	// バウンド力セットフラグの解除

		m_pos.y = 20.0f;

		m_pos += m_move;

		if (m_move.y < 0.0f)
		{
			m_bJump = false;	// ジャンプフラグの解除
			m_move.y = 0.0f;
		}
	}
}
