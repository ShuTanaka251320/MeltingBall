// =================================================
// 
// アイテム(トレジャー)処理[treasure.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "treasure.h"
#include "camera.h"
#include "manager.h"
#include "macro.h"
#include "texture.h"
#include "objectX.h"
#include "player.h"
#include "model.h"
#include "debugproc.h"
#include "sound.h"
#include "shadow.h"
#include "meshring.h"
#include "player.h"
#include "wall.h"
#include "game.h"
#include "particle.h"

// =================================================
// コンストラクタ
// =================================================
CTreasure::CTreasure(int nPriority) :CObjectX(nPriority)
{
	m_pos = VEC_NULL;	// 位置
	m_move = VEC_NULL;	// 移動量
	m_fsin = 0.0f;		// sinカーブの移動量
	m_bDisp = true;		// 表示するかどうか
	m_type = CItem::TYPE_TREASURE;	// 種類
	m_bGet = false;				// 取得しているかどうか
	m_bLive = true;			// 取得可能かどうか
	m_bGetAny = false;		// どれか取得したか

	memset(&m_pShadow, 0, sizeof(m_pShadow));	// 影のポインタ
}

// =================================================
// デストラクタ
// =================================================
CTreasure::~CTreasure()
{

}

// =================================================
// 生成処理
// =================================================
CTreasure* CTreasure::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, CItem::TYPE type, const char* ModelFileName)
{
	CTreasure* pObjectX;	// ポインタ宣言

	// --- メモリの生成 --- //
	pObjectX = new CTreasure;

	// --- 初期化処理 --- //
	pObjectX->Init(pos, rot, type, ModelFileName);

	return pObjectX;
}

// =================================================
// 初期化処理
// =================================================
HRESULT CTreasure::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, CItem::TYPE type, const char* ModelFileName)
{
	if (CItem::GetNumAll() < MAX_ITEM)
	{
		// --- モデルオブジェクトの初期化処理 --- //
		CObjectX::Init(pos, rot, (CObject::TYPE)TYPE_ITEM, ModelFileName);

		m_pShadow = CShadow::Create({ pos.x, 1.0f, pos.z }, VEC_NULL, VEC_NULL, COLOR_WHITE, 20.0f, 20.0f, "data\\TEXTURE\\effect000.jpg");

		m_pMeshRing = CMeshRing::Create({ pos.x, 10.0f, pos.z }, VEC_NULL, 10.0f, 20.0f, 16);

		m_pos = pos;
		m_type = type;	// 種類(タイプ)の設定

		CPlayer* pPlayer = CGame::GetPlayer();
		D3DXMATRIX* Playermtx = pPlayer->GetMatrix();
		m_mtxNext = Playermtx;
		
		CItem::PlusNumAll();

		return S_OK;
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CTreasure::Uninit(void)
{
	if (m_pShadow != NULL)
	{
		m_pShadow->Uninit();
	}
	
	CItem::DecreaseNumAll();	//アイテム総数の減少

	// --- モデルオブジェクトの終了処理 --- //
	CObjectX::Uninit();
}

// =================================================
// 更新処理
// =================================================
void CTreasure::Update(void)
{
	CDebugProc* pDebugProc = CManager::GetDrbugProc();				// デバッグ表示のポインタ

	// --- sinカーブによる上昇下降 --- //
	float fPosY;

	m_fsin += 0.05f;

	// もし、πを超えたら
	if (m_fsin > D3DX_PI)
	{
		m_fsin -= D3DX_PI * 2;
	}

	fPosY = sinf(m_fsin) * 0.5f;
	m_pos.y += fPosY;
	SetPosition(m_pos);

	// --- モデルの回転 --- //
	static D3DXVECTOR3 rot;
	rot.y += 0.004f;

	// 正規化
	if (rot.y > D3DX_PI)
	{
		rot.y -= D3DX_PI * 2;
	}
	// 正規化
	if (rot.x > D3DX_PI)
	{
		rot.x -= D3DX_PI * 2;
	}
	// 正規化
	if (rot.z > D3DX_PI)
	{
		rot.z -= D3DX_PI * 2;
	}

	SetRotation(rot);

	// --- モデルオブジェクトの更新処理 --- //
	CObjectX::Update();
}

// =================================================
// 描画処理
// =================================================
void CTreasure::Draw(void)
{
	if (m_bDisp == true)
	{
		// --- モデルオブジェクトの描画処理 --- //
		CObjectX::Draw();
	}
}

// =================================================
// 当たり判定
// =================================================
bool CTreasure::Collision(D3DXVECTOR3* pPos, D3DXVECTOR3 pPosOld, D3DXVECTOR3* pMove)
{
	CSound* pSound = CManager::GetSound();

	// --- プレイヤーとアイテムの距離計算 --- //
	float fLength = powf((m_pos.x - pPos->x) * (m_pos.x - pPos->x)
		+ (m_pos.z - pPos->z) * (m_pos.z - pPos->z),
		0.5f);

	if (fLength <= 30.0f && (pPos->y <= m_pos.y + 15.0f && pPos->y >= m_pos.y - 15.0f))
	{// 一定の距離以内かつ、頭もしくは脚の位置にアイテムが触れたとき

		//Sound->PlaySound(CSound::SOUND_LABEL_GET);	// 効果音再生

		// --- パーティクルの生成 --- //
		CParticle::Create(D3DXVECTOR3(m_pos.x, m_pos.y + 10.0f, m_pos.z), VEC_NULL, D3DXVECTOR3(10.0f, 10.0f, 10.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), COLOR_RED, 8.0f, 8.0f, 5, 30);

		// --- 取得アイテムの消去 --- //
		//Uninit();

		return true;
	}

	return false;
}
