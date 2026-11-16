// =================================================
// 
// パーティクル処理[particle.cpp]
// Author : Shu Tanaka
// 
// =================================================

#include "main.h"
#include "particle.h"
#include "effect.h"
#include "macro.h"
#include "objectXeffect.h"

// =================================================
// マクロ定義
#define NUMBER_LENGTH			(629)		// 範囲を決める数値
#define PI						(314)		// 円周率 * 100

// =================================================
// コンストラクタ
// =================================================
CParticle::CParticle(int nPriority) :CObject(nPriority)
{
	// --- 各種メンバ変数のクリア --- //
	m_pos = VEC_NULL;	// 位置
	m_move = VEC_NULL;	// 移動量
	m_Minmove = VEC_NULL;	// 移動量
	m_nLife = 0;		// 寿命
	m_nNumParticle = 0;	// パーティクルの数
	m_fWidth = 0;		// 横幅
	m_type = TYPE_DEF;	// タイプ
}

// =================================================
// デストラクタ
// =================================================
CParticle::~CParticle()
{

}

// =================================================
// パーティクルの初期化処理
// =================================================
HRESULT CParticle::Init(void)
{
	return S_OK;
}

// =================================================
// パーティクルの初期化処理
// =================================================
HRESULT CParticle::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, int nNumParticle, int nLife)
{
	// --- 各種メンバ変数に代入 --- //
	m_pos = pos;	// 位置
	m_move = move;	// 移動量
	m_Minmove = VEC_NULL;	// 最小移動量
	m_col = col;	// 色
	m_nNumParticle = nNumParticle;	// パーティクル数
	m_nLife = nLife;	// 寿命
	m_fWidth = fWidth;	// 横幅
	m_fHeight = fHeight;// 高さ
	m_type = TYPE_DEF;	// タイプ

	return S_OK;
}

// =================================================
// パーティクルの初期化処理
// =================================================
HRESULT CParticle::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXVECTOR3 Minmove, D3DXCOLOR col, float fWidth, float fHeight, int nNumParticle, int nLife, TYPE type)
{
	// --- 各種メンバ変数に代入 --- //
	m_pos = pos;	// 位置s
	m_move = move;	// 移動量
	m_Minmove = Minmove;	// 最小移動量
	m_col = col;	// 色
	m_nNumParticle = nNumParticle;	// パーティクル数
	m_nLife = nLife;	// 寿命
	m_fWidth = fWidth;	// 横幅
	m_fHeight = fHeight;// 高さ
	m_type = type;		// タイプ

	return S_OK;
}

// =================================================
// パーティクルの終了処理
// =================================================
void CParticle::Uninit(void)
{
	// --- 自身の破棄 --- //
	Release();
}

// =================================================
// パーティクルの更新処理
// =================================================
void CParticle::Update(void)
{
	float fAngle;
	float fMoveX;
	float fMoveY;
	float fMoveZ;
	float fWidth;
	float fHeight;
	int nLife;
	static const char* pFilename = "data\\TEXTURE\\effect000.jpg";	// テクスチャ名
	static const char* pModelName;	// モデル名
	D3DXVECTOR3 Maxmove = m_move;	//最大移動量の保存
	D3DXVECTOR3 move = VEC_NULL;
	D3DXVECTOR3 pos = VEC_NULL;		// 生成する位置

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

			if (m_type != TYPE_SNOW)
			{
				// --- Y軸は必ず上向き --- //
				move.y = fabs(move.y);
			}
			else if(m_type == TYPE_SNOW)
			{
				move.y -= move.y * 2;
			}

			pos.x = rand() % 100 - 50;
			pos.y = rand() % 100 - 50;
			pos.z = rand() % 100 - 50;

			fWidth = rand() % (int)m_fWidth + m_fWidth * 0.2f;
			fHeight = rand() % (int)m_fHeight + m_fWidth * 0.2f;

			// --- 寿命の設定 --- //
			nLife = rand() % 50 + 10;

			if (m_type == TYPE_DEF)
			{
				nLife = rand() % 20 + 30;
			}
			else if (m_type == TYPE_HEAT)
			{
				nLife = rand() % 20 + 30;
			}
			else if (m_type == TYPE_FOLLOW)
			{
				nLife = rand() % 5 + 10;
			}
			else if (m_type == TYPE_ROADHEAT)
			{
				nLife = rand() % 5 + 30;

				pos.x = rand() % 200 - 100;
				pos.z = rand() % 1400 - 700;
			}
			if (m_type == TYPE_SNOW)
			{
				nLife = rand() % 40 + 60;
			}

			// --- エフェクトの生成 --- //
			CEffect::Create(m_pos + pos,
				VEC_NULL,
				move,
				m_col,
				fWidth,
				fHeight,
				nLife,
				CObject::TYPE_EFFECT,
				5,
				(CEffect::TYPE)m_type);
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
void CParticle::Draw(void)
{

}

// =================================================
// パーティクルの生成
// =================================================
CParticle* CParticle::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, int nNumParticle, int nLife, int nPri)
{
	CParticle* pParticle;	// ポインタ宣言

	// --- オブジェクトの生成 --- //
	pParticle = new CParticle(nPri);

	// --- 初期化 --- //
	pParticle->Init(pos, rot, move, col, fWidth, fHeight, nNumParticle, nLife);

	return pParticle;
}

// =================================================
// パーティクルの生成
// =================================================
CParticle* CParticle::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXVECTOR3 Minmove, D3DXCOLOR col, float fWidth, float fHeight, int nNumParticle, int nLife, int nPri, TYPE type)
{
	CParticle* pParticle;	// ポインタ宣言

	// --- オブジェクトの生成 --- //
	pParticle = new CParticle(nPri);

	// --- 初期化 --- //
	pParticle->Init(pos, rot, move, Minmove, col, fWidth, fHeight, nNumParticle, nLife, type);

	return pParticle;
}