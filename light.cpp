// =================================================
// 
// ライト処理[light.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include"light.h"
#include"manager.h"
#include"renderer.h"

// =================================================
// コンストラクタ
// =================================================
CLight::CLight()
{
	// --- メンバ変数のクリア --- //
	memset(m_aLight, 0, sizeof(m_aLight));
}

// =================================================
// デストラクタ
// =================================================
CLight::~CLight()
{

}

// =================================================
// 初期化処理
// =================================================
HRESULT CLight::Init(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	int nCntLight;

	D3DXVECTOR3 vecDir0, vecDir1, vecDir2;	// ライトの方向のベクトル

	for (nCntLight = 0; nCntLight < MAX_LIGHT; nCntLight++)
	{
		// ライトの情報をクリアする
		ZeroMemory(&m_aLight[nCntLight], sizeof(D3DLIGHT9));

		// ライトの種類を設定
		m_aLight[nCntLight].Type = D3DLIGHT_DIRECTIONAL;
	}

	// ライトの拡散光の設定
	m_aLight[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_aLight[1].Diffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
	m_aLight[2].Diffuse = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);

	// ライトの方向を設定
	vecDir0 = D3DXVECTOR3(0.22f, -0.87f, 0.44f);
	vecDir1 = D3DXVECTOR3(-0.18f, 0.88f, -0.44f);
	vecDir2 = D3DXVECTOR3(0.89f, -0.11f, 0.44f);

	D3DXVec3Normalize(&vecDir0, &vecDir0);	// ベクトルを正規化する
	D3DXVec3Normalize(&vecDir1, &vecDir1);	// ベクトルを正規化する
	D3DXVec3Normalize(&vecDir2, &vecDir2);	// ベクトルを正規化する

	m_aLight[0].Direction = vecDir0;
	m_aLight[1].Direction = vecDir1;
	m_aLight[2].Direction = vecDir2;

	pDevice->SetLight(0, &m_aLight[0]);

	// ライトを有効化
	pDevice->LightEnable(0, TRUE);

	pDevice->SetLight(1, &m_aLight[1]);

	// ライトを有効化
	pDevice->LightEnable(1, TRUE);

	pDevice->SetLight(2, &m_aLight[2]);

	// ライトを有効化
	pDevice->LightEnable(2, TRUE);

	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CLight::Uninit(void)
{

}

// =================================================
// 更新処理
// =================================================
void CLight::Update(void)
{

}
