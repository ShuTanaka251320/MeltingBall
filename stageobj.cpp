// =================================================
// 
// ステージモデル処理[stagemodel.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "stageobj.h"
#include "camera.h"
#include "manager.h"
#include "macro.h"
#include "texture.h"
#include "objectX.h"
#include "player.h"
#include "model.h"
#include "particle.h"
#include "effect.h"
#include "meshfield.h"
#include "debugproc.h"
#include "shadow.h"
#include "input.h"
#include "score.h"
#include "tutorial.h"
#include "object2D.h"
#include "sound.h"
#include "player.h"
#include "math.h"
#include "meshring.h"
#include "modelparticle.h"
#include<stdio.h>

// =================================================
// マクロ定義
#define MESH_SIZE	(6)		// メッシュの分割数
#define	STAGE_FILE	"data\\Scripts\\stage.txt"	// 読み込むステージのファイル名

// =================================================
// 静的メンバ変数宣言

// =================================================
// コンストラクタ
// =================================================
CStageObj::CStageObj(int nPriority) :CObjectX(nPriority)
{
	// --- メンバ変数のクリア --- //
	m_bDisp = false;		// 表示するかどうか
	m_SizeMax = {};			// 最大値
	m_SizeMin = {};			// 最小値
	m_sinCurve = 0.0f;
	m_fMove = 0.0f;	// 移動量
	m_nCntHit = 0;	// ヒットフラグカウンタ
	m_bPlaySound = false;	// 効果音再生フラグ
	m_bHit = false;			// ヒットフラグ
	m_type = TYPE_STAGE;	// タイプ
}

// =================================================
// デストラクタ
// =================================================
CStageObj::~CStageObj()
{
}

// =================================================
// 生成処理
// =================================================
CStageObj* CStageObj::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, TYPE type, const char* ModelFileName)
{
	CStageObj* pStageObj;	// ポインタ宣言

	// --- メモリの生成 --- //
	pStageObj = new CStageObj;

	// --- 初期化処理 --- //
	pStageObj->Init(pos, rot, type, ModelFileName);

	return pStageObj;
}

// =================================================
// 初期化処理
// =================================================
HRESULT CStageObj::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, TYPE type, const char* ModelFileName)
{
	// --- コライダー用変数 --- //
	float fSurface;	// 面積
	float XSize;	// Xサイズ
	float ZSize;	// Zサイズ
	int XDevide;	// X分割数
	int ZDevide;	// Z分割数

	// --- Xオブジェクトの初期化処理 --- //
	fSurface = CObjectX::Init(pos, rot, (CObject::TYPE)TYPE_STAGEMODEL, true, ModelFileName);

	// --- 最大・最小値の取得 --- //
	m_SizeMax = CObjectX::GetVtxMax();
	m_SizeMin = CObjectX::GetVtxMin();

	m_type = type;
	m_fMove = 0.0f;		// コーンの移動量
	m_nCntHit = 120;	// ヒットフラグカウンタ

	// --- モデルのサイズ --- //
	XSize = m_SizeMax.x - m_SizeMin.x;
	ZSize = m_SizeMax.z - m_SizeMin.z;

	// --- サイズに合わせたメッシュの分割数 --- //
	XDevide = (int)XSize / ((int)XSize / MESH_SIZE);
	ZDevide = (int)ZSize / ((int)ZSize / MESH_SIZE);
	CStageModel::IncreaseNumModel();	// 総数の増加

	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CStageObj::Uninit(void)
{
	CStageModel::DecreaseNumModel();

	CObjectX::Uninit();
}

// =================================================
// 更新処理
// =================================================
void CStageObj::Update(void)
{
	CTexture* pTexture = CManager::GetTexture();	// テクスチャの取得
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得
	CJoyPad* pJoyPad = CManager::GetJoyPad();	// ジョイパッドの取得
	CSound* pSound = CManager::GetSound();	// サウンドの取得
	CPlayer* pPlayer = CGame::GetPlayer();	// プレイヤーの取得

	if (m_type == TYPE_HYDRANT && m_bHit == true)
	{// 衝突済みの消火栓
		D3DXVECTOR3 pos = GetPosition();

		// --- モデルパーティクルの生成 --- //
		CModelParticle::Create(pos, VEC_NULL, { 2.0f, 15.0f, 2.0f }, { -2.0f, 5.0f, -2.0f }, COLOR_WHITE, 1, 2, 2, "data\\MODEL\\effect\\water.x", 5);
	}
	if (m_type == TYPE_LIGHT && m_bHit == true)
	{// 衝突済みの街灯
		D3DXVECTOR3 pos = GetPosition();
		pos.y += 150.0f;

		// --- エフェクトの生成 --- //
		CEffect::Create(pos, VEC_NULL, { 0.0f, 5.0f, 0.0f }, COLOR_RED, 25.0f, 25.0f, 5);
	}
	if (m_type == TYPE_CORN && m_bHit == true)
	{// コーンの衝突処理
		HitCorn();
	}

	CObjectX::Update();	// Xオブジェクトの更新処理
}

// =================================================
// 描画処理
// =================================================
void CStageObj::Draw(void)
{
	CObjectX::Draw();
}

// =================================================
// コーンの衝突処理
// =================================================
void CStageObj::HitCorn(void)
{
	D3DXVECTOR3 PosCorn = GetPosition();	// コーンの位置
	D3DXVECTOR3 PosOldCorn = GetPosition();	// コーンの位置
	D3DXVECTOR3 Rot = GetRotation();	// コーンの位置
	D3DXVECTOR3 Move;	// 移動量

	m_fMove += (0.0f - m_fMove) * 0.05f;	// 慣性をかける

	Move.x = m_VecMove.x * m_fMove;	// 移動量の決定
	Move.z = m_VecMove.z * m_fMove;	// 移動量の決定

	// --- 位置を設定 --- //
	PosCorn.x += Move.x;
	PosCorn.z += Move.z;

	if (PosOldCorn.x > PosCorn.x)
	{// 上、下壁に右から衝突
		if (PosCorn.z >= FIELD_SIZE / 2)
		{			// --- 移動制限のリアクション --- //
			m_pReactRing = CMeshRing::Create({ PosCorn.x, 20.0f, 950.0f }, { 0.0f, -D3DX_PI * 0.5F , D3DX_PI * 0.5f }, EXPAND_INRADIUS, EXPAND_OUTRADIUS, REACTRING_DEVIDE, CMesh::TYPE_RING, true, NULL);
			m_pReactRing->SetExpansion(true);
			m_pReactRing->SetExpansionSpan(EXPAND_SPEED);
			m_pReactRing->SetColor(COLOR_WHITE);
			m_pReactRing->SetType(CMeshRing::TYPE_HIT);
			m_pReactRing->SetNor({ 0.0f, 0.0f, -1.0f });

			PosCorn.z = FIELD_SIZE / 2;
			m_VecMove.x = -m_VecMove.z;
			m_VecMove.z = m_VecMove.x;
		}
		if (PosCorn.z <= -FIELD_SIZE / 2)
		{
			// --- 移動制限のリアクション --- //
			m_pReactRing = CMeshRing::Create({ PosCorn.x, 20.0f, -930.0f }, { 0.0f, D3DX_PI * 0.5F , D3DX_PI * 0.5f }, EXPAND_INRADIUS, EXPAND_OUTRADIUS, REACTRING_DEVIDE, CMesh::TYPE_RING, true, NULL);
			m_pReactRing->SetExpansion(true);
			m_pReactRing->SetExpansionSpan(EXPAND_SPEED);
			m_pReactRing->SetColor(COLOR_WHITE);
			m_pReactRing->SetType(CMeshRing::TYPE_HIT);
			m_pReactRing->SetNor({ 0.0f, 0.0f, 1.0f });

			PosCorn.z = -FIELD_SIZE / 2;
			m_VecMove.x = m_VecMove.z;
			m_VecMove.z = -m_VecMove.x;
		}
	}
	else if (PosOldCorn.x < PosCorn.x)
	{// 上、下壁に左から衝突
		if (PosCorn.z >= FIELD_SIZE / 2)
		{
			// --- 移動制限のリアクション --- //
			m_pReactRing = CMeshRing::Create({ PosCorn.x, 20.0f, 950.0f }, { 0.0f, -D3DX_PI * 0.5F , D3DX_PI * 0.5f }, EXPAND_INRADIUS, EXPAND_OUTRADIUS, REACTRING_DEVIDE, CMesh::TYPE_RING, true, NULL);
			m_pReactRing->SetExpansion(true);
			m_pReactRing->SetExpansionSpan(EXPAND_SPEED);
			m_pReactRing->SetColor(COLOR_WHITE);
			m_pReactRing->SetType(CMeshRing::TYPE_HIT);
			m_pReactRing->SetNor({ 0.0f, 0.0f, -1.0f });

			PosCorn.z = FIELD_SIZE / 2;
			m_VecMove.x = m_VecMove.z;
			m_VecMove.z = -m_VecMove.x;
		}
		if (PosCorn.z <= -FIELD_SIZE / 2)
		{
			// --- 移動制限のリアクション --- //
			m_pReactRing = CMeshRing::Create({ PosCorn.x, 20.0f, -930.0f }, { 0.0f, D3DX_PI * 0.5F , D3DX_PI * 0.5f }, EXPAND_INRADIUS, EXPAND_OUTRADIUS, REACTRING_DEVIDE, CMesh::TYPE_RING, true, NULL);
			m_pReactRing->SetExpansion(true);
			m_pReactRing->SetExpansionSpan(EXPAND_SPEED);
			m_pReactRing->SetColor(COLOR_WHITE);
			m_pReactRing->SetType(CMeshRing::TYPE_HIT);
			m_pReactRing->SetNor({ 0.0f, 0.0f, 1.0f });

			PosCorn.z = -FIELD_SIZE / 2;
			m_VecMove.x = -m_VecMove.z;
			m_VecMove.z = m_VecMove.x;
		}
	}
	if (PosOldCorn.z >= PosCorn.z)
	{// 左右の壁に左から衝突
		if (PosCorn.x >= FIELD_SIZE / 2)
		{
			// --- 移動制限のリアクション --- //
			m_pReactRing = CMeshRing::Create({ 950.0f, 20.0f, PosCorn.z }, { 0.0f,  -D3DX_PI * 2.0f , D3DX_PI * 0.5f }, EXPAND_INRADIUS, EXPAND_OUTRADIUS, REACTRING_DEVIDE, CMesh::TYPE_RING, true, NULL);
			m_pReactRing->SetExpansion(true);
			m_pReactRing->SetExpansionSpan(EXPAND_SPEED);
			m_pReactRing->SetColor(COLOR_WHITE);
			m_pReactRing->SetType(CMeshRing::TYPE_HIT);
			m_pReactRing->SetNor({ -1.0f, 0.0f, 0.0f });

			PosCorn.x = FIELD_SIZE / 2;

			//m_VecMove *= -1.0f;
			m_VecMove.x = m_VecMove.z;
			m_VecMove.z = m_VecMove.x;
		}
		if (PosCorn.x <= -FIELD_SIZE / 2)
		{
			// --- 移動制限のリアクション --- //
			m_pReactRing = CMeshRing::Create({ -950.0f, 20.0f, PosCorn.z }, { 0.0f,  -D3DX_PI , D3DX_PI * 0.5f }, EXPAND_INRADIUS, EXPAND_OUTRADIUS, REACTRING_DEVIDE, CMesh::TYPE_RING, true, NULL);
			m_pReactRing->SetExpansion(true);
			m_pReactRing->SetExpansionSpan(EXPAND_SPEED);
			m_pReactRing->SetColor(COLOR_WHITE);
			m_pReactRing->SetType(CMeshRing::TYPE_HIT);
			m_pReactRing->SetNor({ 1.0f, 0.0f, 0.0f });

			PosCorn.x = -FIELD_SIZE / 2;

			//m_VecMove *= -1.0f;

			m_VecMove.x = -m_VecMove.z;
			m_VecMove.z = -m_VecMove.x;
		}
	}
	else if (PosOldCorn.z < PosCorn.z)
	{// 左右の壁に右から衝突
		if (PosCorn.x >= FIELD_SIZE / 2)
		{
			// --- 移動制限のリアクション --- //
			m_pReactRing = CMeshRing::Create({ 950.0f, 20.0f, PosCorn.z }, { 0.0f,  -D3DX_PI * 2.0f , D3DX_PI * 0.5f }, EXPAND_INRADIUS, EXPAND_OUTRADIUS, REACTRING_DEVIDE, CMesh::TYPE_RING, true, NULL);
			m_pReactRing->SetExpansion(true);
			m_pReactRing->SetExpansionSpan(EXPAND_SPEED);
			m_pReactRing->SetColor(COLOR_WHITE);
			m_pReactRing->SetType(CMeshRing::TYPE_HIT);
			m_pReactRing->SetNor({ -1.0f, 0.0f, 0.0f });


			PosCorn.x = FIELD_SIZE / 2;

			m_VecMove.x = -m_VecMove.z;
			m_VecMove.z = -m_VecMove.x;
		}
		if (PosCorn.x <= -FIELD_SIZE / 2)
		{
			// --- 移動制限のリアクション --- //
			m_pReactRing = CMeshRing::Create({ -950.0f, 20.0f, PosCorn.z }, { 0.0f,  -D3DX_PI , D3DX_PI * 0.5f }, EXPAND_INRADIUS, EXPAND_OUTRADIUS, REACTRING_DEVIDE, CMesh::TYPE_RING, true, NULL);
			m_pReactRing->SetExpansion(true);
			m_pReactRing->SetExpansionSpan(EXPAND_SPEED);
			m_pReactRing->SetColor(COLOR_WHITE);
			m_pReactRing->SetType(CMeshRing::TYPE_HIT);
			m_pReactRing->SetNor({ 1.0f, 0.0f, 0.0f });


			PosCorn.x = -FIELD_SIZE / 2;

			//m_VecMove *= -1.0f;

			m_VecMove.x = m_VecMove.z;
			m_VecMove.z = m_VecMove.x;
		}
	}

	Rot.y += 0.2f;	// モデルの回転
	Rot = CMath::RepairRot(&Rot);	// 正規化

	SetPosition(PosCorn);
	SetRotation(Rot);

	// --- ヒット判定カウンタの処理 --- //
	m_nCntHit--;
	if (m_nCntHit <= 0)
	{
		m_bHit = false;
	}
}
