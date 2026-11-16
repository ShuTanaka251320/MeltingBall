// =================================================
// 
// モデルでのパーティクル処理[modelparticle.cpp]
// Author : Shu Tanaka
// 
// =================================================

#include "main.h"
#include "modelparticle.h"
#include "objectXeffect.h"
#include "macro.h"

// =================================================
// マクロ定義
#define NUMBER_LENGTH			(629)		// 範囲を決める数値
#define PI						(314)		// 円周率 * 100

// =================================================
// コンストラクタ
// =================================================
CModelParticle::CModelParticle(int nPriority) :CObject(nPriority)
{
	// --- 各種メンバ変数のクリア --- //
	m_pos = VEC_NULL;	// 位置
	m_move = VEC_NULL;	// 移動量
	m_Minmove = VEC_NULL;	// 移動量
	m_nLife = 0;		// 寿命
	m_nNumParticle = 0;	// パーティクルの数
	m_fscale = 0.0f;		// 拡縮率
	m_pModelFileName = NULL;
}

// =================================================
// デストラクタ
// =================================================
CModelParticle::~CModelParticle()
{

}

// =================================================
// パーティクルの初期化処理
// =================================================
HRESULT CModelParticle::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXVECTOR3 Minmove, D3DXCOLOR col, float fscale, int nNumParticle, int nLife, const char* pFileName)
{
	// --- 各種メンバ変数に代入 --- //
	m_pos = pos;
	m_move = move;
	m_Minmove = Minmove;
	m_col = col;
	m_nNumParticle = nNumParticle;
	m_nLife = nLife;
	m_fscale = fscale;
	m_pModelFileName = pFileName;

	return S_OK;
}

// =================================================
// パーティクルの終了処理
// =================================================
void CModelParticle::Uninit(void)
{
	// --- 自身の破棄 --- //
	Release();
}

// =================================================
// パーティクルの更新処理
// =================================================
void CModelParticle::Update(void)
{
	float fAngle;
	float fMoveX;
	float fMoveY;
	float fMoveZ;
	float fsize;
	int nLife;
	static const char* pFilename = "data\\TEXTURE\\effect000.jpg";
	D3DXVECTOR3 Maxmove = m_move;	//最大移動量の保存
	D3DXVECTOR3 move = VEC_NULL;
	D3DXVECTOR3 pos = VEC_NULL;		// 生成する位置
	D3DXVECTOR3 rot = VEC_NULL;		// 回転する向き

	for (int nCnt = 0; nCnt < m_nNumParticle; nCnt++)
	{
		if (m_nLife > 0)
		{
			// --- 移動量の設定 --- //
			fAngle = (float)(rand() % NUMBER_LENGTH - PI) / 100.0f;

			// --- ランダムで移動量設定 --- //
			fMoveX = (float)(rand() % (int)Maxmove.x) + m_Minmove.x;
			fMoveY = (float)(rand() % (int)Maxmove.y) + m_Minmove.y;
			fMoveZ = (float)(rand() % (int)Maxmove.z) + m_Minmove.z;

			// --- 移動量の反映 --- //
			move.x = sinf(fAngle) * fMoveX;
			move.y = cosf(fAngle) * fMoveY;
			move.z = cosf(fAngle) * fMoveZ;

			// --- Y軸は必ず上向き --- //
			move.y = fabs(move.y);

			// --- 位置をずらす --- //
			pos.x = rand() % 100 - 50;
			pos.y = rand() % 100 - 50;
			pos.z = rand() % 100 - 50;

			// --- パーティクルの回転を設定 --- //
			rot.x = (rand() % NUMBER_LENGTH - PI) / 100.0f;
			rot.y = (rand() % NUMBER_LENGTH - PI) / 100.0f;
			rot.z = (rand() % NUMBER_LENGTH - PI) / 100.0f;

			fsize = rand() % (int)m_fscale + 1.0f;
			float fFloat = (float)(rand() % 10) / 10.0f;
			fsize += fFloat;

			// --- 寿命の設定 --- //
			nLife = rand() % 50 + 30;

			// --- モデルエフェクトの生成 --- //
			CObjectXEffect::Create(m_pos,
				rot,
				move,
				m_col,
				fsize,
				nLife,
				CObject::TYPE_EFFECT,
				5,
				m_pModelFileName);
		}
	}

	m_nLife--;

	// 寿命切れ
	if (m_nLife <= 0)
	{
		Uninit();
	}
}

// =================================================
// パーティクルの描画処理
// =================================================
void CModelParticle::Draw(void)
{

}

// =================================================
// パーティクルの生成
// =================================================
CModelParticle* CModelParticle::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXVECTOR3 Minmove, D3DXCOLOR col, float fscale, int nNumParticle, int nLife, const char* pFileName, int nPri)
{
	CModelParticle* pParticle;	// ポインタ宣言

	// --- オブジェクトの生成 --- //
	pParticle = new CModelParticle(nPri);

	// --- 初期化 --- //
	pParticle->Init(pos, rot, move, Minmove, col, fscale, nNumParticle, nLife, pFileName);

	return pParticle;
}