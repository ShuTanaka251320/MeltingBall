// =================================================
// 
// エフェクト処理[effect.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include"effect.h"
#include"renderer.h"
#include"manager.h"
#include"score.h"
#include"texture.h"
#include"game.h"
#include"player.h"
#include"macro.h"
#include"input.h"

// =================================================
// 静的メンバ変数
LPDIRECT3DTEXTURE9 CEffect::m_pTexture = NULL;	// テクスチャのポインタ

// =================================================
// コンストラクタ
// =================================================
CEffect::CEffect(int nPriority) :CBillboard(nPriority)
{
	// --- 各種変数のクリア --- //
	m_nMaxLife = 0;	// 設定寿命
	m_nLife = 0;	// 寿命
	m_typeEffect = TYPE_DEF;	// エフェクタイプ
	m_SpawnPos = VEC_NULL;
}

// =================================================
// デストラクタ
// =================================================
CEffect::~CEffect()
{

}

// =================================================
// エフェクトの初期化処理
// =================================================
HRESULT CEffect::Init(void)
{

	return S_OK;
}

// =================================================
// エフェクトの初期化処理
// =================================================
HRESULT CEffect::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, int nLife, CObject::TYPE type, TYPE Effecttype, const char* pFileName)
{
	CTexture* pTexture = CManager::GetTexture();

	// --- ビルボードオブジェクトの初期化処理 --- //
	CBillboard::Init(pos, rot, move, col, fWidth, fHeight, type);

	if (pFileName != NULL)
	{
		// --- テクスチャ登録 --- //
		m_nIdxTex = pTexture->Register(pFileName);
	}
	else
	{
		m_nIdxTex = -1;
	}

	CBillboard::SetTexture(m_nIdxTex);

	// --- メンバ変数に代入 --- //
	m_nLife = nLife;	// 寿命
	m_nMaxLife = nLife;	// 最大寿命
	m_type = type;	// オブジェクトのタイプ
	m_typeEffect = Effecttype;	// エフェクトのタイプ

	if (m_typeEffect == TYPE_FOLLOW)
	{// 吸収タイプ
		CPlayer* pPlayer = CGame::GetPlayer();
		D3DXVECTOR3 Destpos = pPlayer->GetPosition();	// 目標の位置(プレイヤーの位置)
		D3DXMATRIX* mtxWorld = pPlayer->GetMatrix();	// プレイヤーのマトリックスを取得
		D3DXVECTOR3 DestVec;	// 向かうベクトル(方向)
		D3DXVECTOR3 OffSet = {};	// オフセット

		float fAngle = (float)(rand() % 629 - 314) / 100.0f;	// 周りの生成角度を出す

		// --- オフセットの設定 --- //
		OffSet.x = sinf(fAngle) * (80.0f + pPlayer->GetRadius());
		OffSet.y = cosf(fAngle);
		OffSet.z = cosf(fAngle) * (80.0f + pPlayer->GetRadius());

		D3DXVec3TransformCoord(&m_SpawnPos, &OffSet, mtxWorld);

		m_SpawnPos.y -= 30.0f;
		SetPosition(m_SpawnPos);	// 位置の反映

		DestVec = Destpos - m_SpawnPos;	// 目標地点までのベクトル
		DestVec.y -= pPlayer->GetRadius();
		move = DestVec * 0.07f;	// 移動量の設定

		SetMove(move);	// 移動量の反映
	}

	return S_OK;
}

// =================================================
// エフェクトの終了処理
// =================================================
void CEffect::Uninit()
{
	// --- ビルボードオブジェクトの終了処理 --- //
	CBillboard::Uninit();
}

// =================================================
// エフェクトの更新処理
// =================================================
void CEffect::Update(void)
{
	// --- 位置、移動量、向きの取得 --- //
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 move = GetMove();
	D3DXVECTOR3 rot = GetRotation();

	if (m_typeEffect == TYPE_HEAT)
	{// 熱のエフェクト処理
		move.x += (0.0f - move.x) * 0.25f;
		move.z += (0.0f - move.z) * 0.25f;
		SetMove(move);
	}
	else if (m_typeEffect == TYPE_FOLLOW)
	{// プレイヤーに吸いつく


	}

	// --- posの更新 --- //
	pos += move;
	SetPosition(pos);

	//AlphaDecrease(m_nMaxLife, m_nLife);	// 寿命依存で透明度減少

	// --- ビルボードオブジェクトの更新処理(位置の反映等) --- //
	CBillboard::Update();

	// --- 寿命の減少 --- //
	m_nLife--;

	if (m_nLife <= 0)
	{// 寿命切れ
		Uninit();
	}
}

// =================================================
// エフェクトの描画処理
// =================================================
void CEffect::Draw(void)
{
	if (CManager::GetDrawParticle() == true)
	{
		CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

		LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ
		CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得

		//static bool bZtest = true;

		//if (pInputKeyboard->GetTrigger(DIK_7) == true)
		//{
		//	bZtest = bZtest ^ true;
		//}

		// Zテストを無効にする
		if (m_typeEffect != TYPE_FOLLOW)
		{
			pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
			pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		}

		// αブレンディングを加算合成に設定
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		// --- ビルボードオブジェクトの描画処理 --- //
		CBillboard::Draw();

		// αブレンディングをもとに戻す
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		// Zテストを有効にする
		pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
}

// =================================================
// エフェクトの生成
// =================================================
CEffect* CEffect::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, int nLife, CObject::TYPE type, int nPri, TYPE Effecttype, const char* pFileName)
{
	CEffect* pEffect;	// ポインタ宣言

	// --- オブジェクトの生成 --- //
	pEffect = new CEffect(nPri);

	// --- 初期化 --- //
	pEffect->Init(pos, rot, move, col, fWidth, fHeight, nLife, type, Effecttype, pFileName);

	return pEffect;
}
