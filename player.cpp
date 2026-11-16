// =================================================
// 
// プレイヤー処理[player.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "camera.h"
#include "player.h"
#include "input.h"
#include "manager.h"
#include "model.h"
#include "debugproc.h"
#include "renderer.h"
#include "texture.h"
#include "sound.h"
#include "item.h"
#include "wall.h"
#include "field.h"
#include "meshring.h"
#include "mesh.h"
#include "game.h"
#include "treasure.h"
#include "stagemodel.h"
#include "effect.h"
#include "particle.h"
#include "modelparticle.h"
#include "meshring.h"
#include "math.h"
#include<stdio.h>

// =================================================
// マクロ定義
#define	PLAYER_MOVE		(0.35f)			// プレイヤーの移動速度(回転速度)
#define	JUMP_FORCE		(5.5f)			// ジャンプ力
#define	JUMP_LIMIT		(30.0f)			// 最大ジャンプ力
#define	PLAYER_ROTMOVE	(0.3f)			// プレイヤーの回転速度
#define	PLAYER_INI		(0.4f)			// プレイヤーの慣性
#define PLAYER_JUMPCNT	(0.1f)			// プレイヤーのジャンプ力カウント
#define	GRAVITY			(-0.2f)			// 重力
#define	PLAYER_RADIUS	(10.0f)			// プレイヤーモデルの半径
#define	CAMERACHANGE_BOARDER	(-23.0f)	// カメラの視野角変更の基準move値
#define	STICK_DEADAREA	(10000)			// ジョイスティックのデッドエリア
#define	MODEL_FILE	"data\\Scripts\\player.txt"			// モデルを読み込むファイル名
#define	MOTION_FILE	"data\\Scripts\\motion.txt"			// モーションを読み込むファイル名

// =================================================
// コンストラクタ
// =================================================
CPlayer::CPlayer(int nPriority) :CObject(nPriority)
{
	m_pos = {};		// 位置
	m_posOld = {};	// 過去の位置
	m_posCam = {};	// カメラを向ける位置
	m_rot = {};		// 向き
	m_rotDest = {};	// 目標の向き
	m_rotDiff = {};	// 目標と現在の向きの差分
	m_move = {};	// 移動量
	D3DXMatrixIdentity(&m_mtxRot);
	m_quat = {};	// クォータニオン
	m_vecAxis = {};	// 回転軸
	m_fValueRot = 0.0f;	// 回転角(回転量)	
	m_fDestValueRot = 0.0f;	// 目標の回転角(回転量)	
	m_fDiffValueRot = 0.0f;		// 目標の回転角(回転量)と現在の差分	
	m_scl = VEC_NULL;		// スケール
	m_fMoveAngle = 0.0f;	// 移動向き
	m_fRebound = 0.0f;		// バウンド量
	m_fposField = {};	// 足元の高さ
	m_bSetJumpForce = false;	// バウンド力のセットフラグ
	m_bJump = false;	// ジャンプフラグ
	m_fSaveMove = 0.0f;	// ジャンプ量の保存
	memset(m_mtxWorld, 0, sizeof(m_mtxWorld));	// ワールドマトリックス
	memset(m_Offset, 0, sizeof(m_Offset));		// オフセットの位置
	memset(m_apModel, 0, sizeof(m_apModel));	// モデル(パーツ)のポインタ
	m_nNumModel = 0;		// モデル(パーツ)の数
	m_bMove = false;	// 動いているかどうか
	m_pCombo = NULL;	// コンボのUI用
	memset(&m_apNumber, 0, sizeof(m_apNumber));	// コンボ数のUI用
	pMeshOrbit = NULL;	// オービットのポインタ
	m_fRadius = 0.0f;	// プレイヤーのモデル半径
	m_fHeat = 0.0f;		// 熱ゲージ
	memset(m_pRing, 0, sizeof(m_pRing));	// 充電UI用メッシュリング
	m_pReactRing = NULL;	// 壁に当たったリアクション用メッシュリング
	m_pCollRing = NULL;	// 当たり判定用のメッシュリング
	m_pFileName = NULL;	// モデルパーティクルのファイル名
	m_bSoundMelt = false;	// 溶かす効果音フラグ
	m_bHitSnowField = false;	// 雪のフィールドに当たったかのフラグ
	m_bPlaySound[TYPE_NORMAL] = false;	// 各種効果音の再生フラグ
	m_bPlaySound[TYPE_BIG] = false;		// 各種効果音の再生フラグ
	m_bPlaySound[TYPE_FIRE] = false;	// 各種効果音の再生フラグ
	m_bHitwall = false;		// 壁との衝突フラグ
	m_nCntHitWall = 0;	// 壁との衝突時間
}

// =================================================
// デストラクタ
// =================================================
CPlayer::~CPlayer()
{

}

// =================================================
// 生成処理
// =================================================
CPlayer* CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move)
{
	CPlayer* pPlayer;	// ポインタ宣言

	// --- メモリ生成 --- //
	pPlayer = new CPlayer;

	// --- 初期化処理 --- //
	pPlayer->Init(pos, rot, move);

	return pPlayer;
}

// =================================================
// 初期化処理
// =================================================
HRESULT CPlayer::Init(void)
{
	return S_OK;
}

// =================================================
// 初期化処理(オーバーロード)
// =================================================
HRESULT CPlayer::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move)
{
	//m_pMotion = new CMotion;	// モーションのポインタ

	// --- プレイヤーモデルの読み込み --- //
	LoadModel();

	// --- モーションの設定 --- //
	//m_pMotion->Load(MOTION_FILE);
	//m_pMotion->Set(1);
	//m_pMotion->Set(0, 10);
	//m_pMotion->SetModel(&m_apModel[0], m_nNumModel);

	CObject::SetType(CObject::TYPE_PLAYER);

	//pMeshOrbit = CMeshOrbit::Create(VEC_NULL, VEC_NULL, { -10.0f, 10.0f, 0.0f }, { 10.0f, 10.0f, 0.0f }, 20.0f, 20.0f, 20.0f, 200.0f, 1, 10, CMesh::TYPE_ORBIT, true, NULL);
	//pMeshOrbit->SetColor(COLOR_LIGHTBLUE);

	// --- メンバ変数に代入 --- //
	m_pos = pos;
	m_posCam = pos;
	m_rot = rot;
	m_move = move;
	D3DXMatrixIdentity(&m_mtxRot);
	m_quat = {};	// クォータニオン
	m_vecAxis = { 1.0f, 0.0f, 0.0f };	// 回転軸
	m_DestvecAxis = { 0.0f, 0.0f, 0.0f };	// 目標の回転軸
	m_DiffvecAxis = { 0.0f, 0.0f, 0.0f };	// 回転軸の差分
	m_fValueRot = PLAYER_MOVE;	// 回転角(回転量)	
	m_fDestValueRot = PLAYER_MOVE;	// 目標の回転角(回転量)	
	m_fDiffValueRot = 0.0f;		// 目標の回転角(回転量)と現在の差分	
	m_scl = { 1.0f, 1.0f, 1.0f };	// スケール量
	m_fMoveAngle = 0.0f;	// 移動向き
	m_fRebound = 0.0f;		// バウンド量
	m_bSetJumpForce = false;	// バウンド力のセットフラグ
	m_fSaveMove = 0.0f;	// ジャンプ量の保存
	m_fposField = {};		// 足元の高さ
	m_fRadius = PLAYER_RADIUS * m_scl.y;	// 半径
	m_fHeat = 600.0f;	// 熱ゲージ

	// --- 充電ゲージ用メッシュリングの生成 --- //
	m_pRing[0] = CMeshRing::Create(m_pos, VEC_NULL, 10.0f, 20.0f, 16);
	m_pRing[1] = CMeshRing::Create(m_pos, VEC_NULL, 10.0f, 20.0f, 16);
	m_pRing[2] = CMeshRing::Create(m_pos, VEC_NULL, 10.0f, 20.0f, 16);
	m_pRing[3] = CMeshRing::Create(m_pos, VEC_NULL, 10.0f, 20.0f, 16);

	for (int nCnt = 0; nCnt < MAX_MATER; nCnt++)
	{
		m_pRing[nCnt]->SetColor((COLOR_GREEN + COLOR_YELLOW) * 0.5f);
	}

	// --- 当たり判定用のメッシュリングの生成 --- //
	m_pCollRing = CMeshRing::Create({ 100.0f, 30.0f, 0.0f }, VEC_NULL, 50.0f, 60.0f, 16, CMesh::TYPE_RING, false);
	m_pCollRing->SetColor(COLOR_GREEN);

	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CPlayer::Uninit(void)
{
	pMeshOrbit = NULL;

	for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
	{
		m_apModel[nCnt]->Uninit();
	}

	Release();	// 死亡フラグを立てる
}

// =================================================
// 更新処理
// =================================================
void CPlayer::Update(void)
{
	// --- 各種情報の取得 --- //
	CJoyPad* pJoyPad = CManager::GetJoyPad();		// ジョイパッドの取得
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ
	CSound* pSound = CManager::GetSound();			// サウンド情報の取得
	CDebugProc* pDebugProc = CManager::GetDrbugProc();	// デバッグ表示のポインタ
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得

	CCamera* pCamera = CGame::GetCamera();

	D3DXVECTOR3 CameraRot = pCamera->GetRotation();		// カメラの向きを取得

	// --- 現在位置の保存 --- //
	m_posOld = m_pos;

	// --- 毎フレーム更新時に入力されていない状態に --- //
	m_bMove = false;

	// --- 半径の計算 --- //
	m_fRadius = PLAYER_RADIUS * m_scl.y;

	float fSurcle = 2 * D3DX_PI * m_fRadius;	// 円周

	float fAngle = m_fMoveAngle;
	static float fValue = 0.0f;

	if (fAngle - m_fMoveAngle == D3DX_PI || fAngle - m_fMoveAngle == -D3DX_PI)
	{
		// --- 慣性を掛ける --- //
		m_fValueRot -= m_fValueRot * 2.0f;
	}

	m_fMoveAngle = CameraRot.y + D3DX_PI * 2.0f;	// 移動方向の設定

	m_vecAxis = { sinf(CameraRot.y + D3DX_PI * 0.5f)  , 0.0f, cosf(CameraRot.y + D3DX_PI * 0.5f) };

	// --- 進行方向に合わせた向き設定 --- //
	NorRotate(CameraRot.y - D3DX_PI);

	m_rot.y = CameraRot.y + D3DX_PI * 2.0f;

	// --- 回転量の調整 --- //
	//float fValueRot = m_fValueRot * (1.0f - (m_scl.y * 0.25f));

	// --- 移動速度に関する処理 --- //
	CField* pHeatRightRoad = CGame::GetField(CField::TYPE_RIGHTHEAT);	// ロードヒーティングフィールドの情報取得
	CField* pHeatLeftRoad = CGame::GetField(CField::TYPE_LEFTHEAT);		// ロードヒーティングフィールドの情報取得

	if (pHeatLeftRoad->bCollision(m_pos, m_posOld) == false && pHeatRightRoad->bCollision(m_pos, m_posOld) == false)
	{
		// --- ブレーキ ---//
		if (pInputKeyboard->GetPress(DIK_S) == true || pJoyPad->GetPress(CJoyPad::JOYKEY_A) == true)
		{
			m_fValueRot += (0.02f - m_fValueRot) * 0.1f;
		}

		if (m_scl.y < 1.5f)
		{
			m_fValueRot += (0.7f - m_fValueRot) * 0.3f;
		}
		else if (m_scl.y < 2.0f)
		{
			m_fValueRot += (0.45f - m_fValueRot) * 0.3f;
		}
		else if (m_scl.y < 2.5f)
		{
			m_fValueRot += (0.15f - m_fValueRot) * 0.3f;
		}
		else if (m_scl.y < 3.0f)
		{
			m_fValueRot += (0.1f - m_fValueRot) * 0.05f;
		}
		else if (m_scl.y < 3.5f)
		{
			m_fValueRot += (0.075f - m_fValueRot) * 0.05f;
		}
		else if (m_scl.y < 4.0f)
		{
			m_fValueRot += (0.05f - m_fValueRot) * 0.05f;
		}
		else if (m_scl.y >= 4.0f)
		{
			m_fValueRot += (0.02f - m_fValueRot) * 0.3f;
		}
	}
	else
	{
		// --- ブレーキ ---//
		if (pInputKeyboard->GetPress(DIK_S) == true || pJoyPad->GetPress(CJoyPad::JOYKEY_A) == true)
		{
			m_fValueRot += (0.02f - m_fValueRot) * 0.05f;
		}
	}

	// --- 回転量の反映 --- //
	m_move.x = sinf(m_fMoveAngle) * (m_fValueRot / (2 * D3DX_PI)) * fSurcle;
	m_move.z = cosf(m_fMoveAngle) * (m_fValueRot / (2 * D3DX_PI)) * fSurcle;

	m_fSaveMove = m_move.y;	// 移動量の保存

	// --- 位置の更新 --- //
	m_pos += m_move;

	if (m_bJump == false && m_bSetJumpForce == false)
	{// バウンドしていないとき
		m_pos.y = m_fposField + m_fRadius;
	}

	// --- バウンド処理 --- //
	Bound();

	// --- 各種状態にとる効果音再生 --- //
	if (m_fHeat >= NUM_BURN)
	{
		if (m_bPlaySound[TYPE_FIRE] == false)
		{// 未再生状態の場合
			pSound->PlaySound(CSound::SOUND_LABEL_MOVEFIRE);
		}

		// --- そのほかの移動効果音を停止 --- //
		pSound->Stop(CSound::SOUND_LABEL_MOVENORMAL);
		pSound->Stop(CSound::SOUND_LABEL_MOVEBIG);

		// --- 炎上状態の効果音再生フラグをオン --- //
		m_bPlaySound[TYPE_FIRE] = true;
		m_bPlaySound[TYPE_NORMAL] = false;
		m_bPlaySound[TYPE_BIG] = false;
	}
	else if (m_fHeat > NUM_SETMATERIALBLUE && m_fHeat <= NUM_BURN)
	{
		if (m_bPlaySound[TYPE_NORMAL] == false)
		{// 未再生状態の場合
			pSound->PlaySound(CSound::SOUND_LABEL_MOVENORMAL);
		}

		// --- そのほかの移動効果音を停止 --- //
		pSound->Stop(CSound::SOUND_LABEL_MOVEFIRE);
		pSound->Stop(CSound::SOUND_LABEL_MOVEBIG);

		// --- 通常状態の効果音再生フラグをオン --- //
		m_bPlaySound[TYPE_NORMAL] = true;
		m_bPlaySound[TYPE_FIRE] = false;
		m_bPlaySound[TYPE_BIG] = false;
	}
	else if (m_fHeat <= NUM_SETMATERIALBLUE)
	{
		if (m_bPlaySound[TYPE_BIG] == false)
		{// 未再生状態の場合
			pSound->PlaySound(CSound::SOUND_LABEL_MOVEBIG);
		}

		// --- そのほかの移動効果音を停止 --- //
		pSound->Stop(CSound::SOUND_LABEL_MOVEFIRE);
		pSound->Stop(CSound::SOUND_LABEL_MOVENORMAL);

		// --- 巨大化状態の効果音再生フラグをオン --- //
		m_bPlaySound[TYPE_BIG] = true;
		m_bPlaySound[TYPE_NORMAL] = false;
		m_bPlaySound[TYPE_FIRE] = false;
	}

	// --- プレイヤーの周りのメッシュリングの処理 --- //
	if (m_fHeat >= NUM_BURN && m_fHeat <= MAX_HEATGAUGE)
	{// アツアツ状態
		m_pCollRing->SetInRadius(RADIUS_BURN);
		m_pCollRing->SetOutRadius(RADIUS_BURN + 10.0f);
		CParticle::Create(m_pos, VEC_NULL, { 2.0f, 5.0f, 2.0f }, { 10.0f, 25.0f, 10.0f }, COLOR_RED, 35.0f, 35.0f, 2, 10, 5, CParticle::TYPE_HEAT);
	}
	else if (m_fHeat >= NUM_SETMATERIALRED && m_fHeat <= NUM_BURN)
	{
		m_pCollRing->SetInRadius(RADIUS_NORMAL);
		m_pCollRing->SetOutRadius(RADIUS_NORMAL + 10.0f);
	}
	else if (m_fHeat <= NUM_SETMATERIALRED)
	{
		m_pCollRing->SetInRadius(m_fRadius * 2.0f);
		m_pCollRing->SetOutRadius(m_fRadius * 2.0f + 40.0f);
	}

	// --- オフセットの位置を保存 --- //
	D3DXVECTOR3 OffSet;
	D3DXVECTOR3 Pos;

	OffSet = VEC_NULL;
	D3DXVec3TransformCoord(&Pos, &OffSet, &m_mtxWorldPos);

	if (m_pCollRing != NULL)
	{
		m_pCollRing->SetPosition(Pos);
	}

	m_bHitSnowField = false;

	CField* pSnowField = CGame::GetField(CField::TYPE_SNOW);	// 雪のフィールド情報の取得
	pSnowField->CollRing();	// リングとの当たり判定

	if (pInputKeyboard->GetTrigger(DIK_F6) == true)
	{
		pSnowField->Save();
	}

	// --- メッシュフィールドの当たり判定 --- //
	MeshColl();
	HitSnowField();

	if (m_fHeat >= NUM_SETMATERIALRED)
	{// 熱を表現するパーティクル
		CParticle::Create(m_pos, VEC_NULL, { 5.0f, 5.0f, 5.0f }, { 40.0f, 15.0f, 40.0f }, COLOR_RED, 5.0f, 5.0f, 2, 5, 8, CParticle::TYPE_HEAT);
	}

	if (m_fHeat > NUM_SCALEDOWN)
	{// 玉の縮小
		if (m_scl.y > 1.03f)
		{
			m_scl.x -= 0.02f;
			m_scl.y -= 0.02f;
			m_scl.z -= 0.02f;
		}
	}

	// --- モデルとの当たり判定 --- //
	ModelColl();

	// --- アイテムとの当たり判定 --- //
	ItemColl();

	m_bHitwall = false;	// 衝突フラグ

	// --- 移動制限 --- //
	LimitMove();

	if (m_bHitwall == false)
	{
		m_nCntHitWall = 0;
	}

	// --- 充電UIの処理 --- //
	BatteryRing();

	if (m_rot.y > D3DX_PI + CameraRot.y)
	{
		m_rot.y -= D3DX_PI * 2;
	}
	else if (m_rot.y < -D3DX_PI + CameraRot.y)
	{
		m_rot.y += D3DX_PI * 2;
	}

	//ChangeRotate();	// 回転量の調整

	pDebugProc->PrintDebug("[位置] : %f %f %f\n", m_pos.x, m_pos.y, m_pos.z);
	pDebugProc->PrintDebug("[move値] : %f %f %f\n", m_move.x, m_move.y, m_move.z);
	pDebugProc->PrintDebug("[回転軸] : %f %f %f\n", m_vecAxis.x, m_vecAxis.y, m_vecAxis.z);
	pDebugProc->PrintDebug("[移動量] : %f\n", m_fValueRot);
	pDebugProc->PrintDebug("[回転量] : %f / [↑][↓]\n", m_fValueRot);
	pDebugProc->PrintDebug("[縮小/拡大] : %f %f %f / [1][2]\n", m_scl.x, m_scl.y, m_scl.z);
	pDebugProc->PrintDebug("[熱ゲージ] : %f \n", m_fHeat);
	pDebugProc->PrintDebug("[モデルのスケール] : [1][2]\n");
	pDebugProc->PrintDebug("[起伏の上・下降幅] : [←][→]\n");
	pDebugProc->PrintDebug("[アイテムリセット] : [ENTER]\n");
	pDebugProc->PrintDebug("[起伏リセット] : [BACK SPACE]\n");
	pDebugProc->PrintDebug("[パーティクルのモデルの変更] : [9][0]\n", m_pFileName);
	pDebugProc->PrintDebug("[モデルパーティクルのファイル名] : [%s]\n", m_pFileName);
}

// =================================================
// 描画処理
// =================================================
void CPlayer::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	CCamera* pCamera = CGame::GetCamera();

	D3DXMATRIX mtxScaling, mtxRot, mtxTrans;	// 計算用マトリックス

	// ***** ゲームとしての描画 ***** //
	// プレイヤーのワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);
	D3DXMatrixIdentity(&m_mtxWorldPos);

	// --- スケールの調整 --- //
	D3DXMatrixScaling(&mtxScaling, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScaling);	// かけ合わせる

	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// 回転軸における指定の回転角からクォータニオンを作成
	D3DXQuaternionRotationAxis(&m_quat, &m_vecAxis, m_fValueRot);

	// クォータニオンから回転マトリックスを作成
	D3DXMatrixRotationQuaternion(&mtxRot, &m_quat);

	D3DXMatrixMultiply(&m_mtxRot, &m_mtxRot, &mtxRot);	// かけ合わせる

	// プレイヤーの向きを反映
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &m_mtxRot);	// かけ合わせる

	// プレイヤーの位置を反映
	D3DXMatrixTranslation(&mtxTrans,
		m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);	// かけ合わせる

	D3DXMatrixMultiply(&m_mtxWorldPos, &m_mtxWorldPos, &mtxTrans);	// かけ合わせる

	// プレイヤーのワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// --- モデルの描画 --- //
	for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
	{
		m_apModel[nCnt]->Draw(&m_mtxWorld);
	}
}

// =================================================
// モデルの読み込む処理
// =================================================
HRESULT CPlayer::LoadModel(void)
{
	// === モデル用の変数を用意 === //
	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 向き
	int nNumIdx = 0;		// モデルのインデックス
	int nIdxParent[MAX_PART] = {};		// 親のインデックス
	int nNumModel = 0;		// モデル数
	char Realize[128];		// 判別用の変数
	char aModelName[MAX_PART][128];	// モデルのファイル名
	char* pEqual = NULL;	// =を消去するためのポインタ
	int nUseShadow = 0;		// 影を使用するかどうか用の変数
	int nCntModel = 0;		// モデルのカウントアップ用(要素)
	int nCntModelNum = 0;	// モデルのカウントアップ用(名前)

	// ステージ情報を読み込み
	FILE* pFile;
	pFile = NULL;

	pFile = fopen(MODEL_FILE, "r");

	if (pFile != NULL)
	{// ファイルを開いた
		while (1)
		{
			fgets(&Realize[0], sizeof Realize, pFile);	// 最初に比較用文字の読み込み

			if (JudgeComent(&Realize[0]) == true)
			{// 何かの情報を読み込む合図を取得したとき
				if (strcmp(&Realize[0], "SCRIPT") == 0)
				{

				}
			}

			if (strstr(&Realize[0], "NUM_MODEL") != NULL)
			{// モデルの数を代入
				// 「=」の場所を見つける
				pEqual = strstr(Realize, "=");

				if (pEqual != NULL)
				{
					// アドレスを1こずらす
					pEqual++;

					// モデルの数を読み込む
					sscanf(pEqual, "%d", &m_nNumModel);
				}
			}

			if (strstr(&Realize[0], "MODEL_FILENAME") != NULL)
			{// モデルのファイル名を代入
				// 「=」の場所を見つける
				pEqual = strstr(Realize, "=");

				if (pEqual != NULL)
				{
					// アドレスを1こずらす
					pEqual++;

					// モデルの名前を読み込む
					sscanf(pEqual, "%s", &aModelName[nCntModelNum][0]);
					nCntModelNum++;
				}
			}

			if (strstr(&Realize[0], "MODELSET") != NULL)
			{
				while (1)
				{
					fgets(&Realize[0], sizeof Realize, pFile);	// 最初に比較用文字の読み込み

					if (JudgeComent(&Realize[0]) == true)
					{
						if (strstr(&Realize[0], "PARTSSET") != NULL)
						{
							while (1)
							{
								fgets(&Realize[0], sizeof Realize, pFile);	// 最初に比較用文字の読み込み

								if (JudgeComent(&Realize[0]) == true)
								{
									if (strstr(&Realize[0], "INDEX") != NULL)
									{
										pEqual = strstr(Realize, "=");

										if (pEqual != NULL)
										{
											// アドレスを1こずらす
											pEqual++;

											// モデルのインデックスを読み込む
											(void)sscanf(pEqual, "%d", &nNumIdx);
										}
									}
									if (strstr(&Realize[0], "PARENT") != NULL)
									{
										pEqual = strstr(Realize, "=");

										if (pEqual != NULL)
										{
											// アドレスを1こずらす
											pEqual++;

											// モデルのインデックスを読み込む
											(void)sscanf(pEqual, "%d", &nIdxParent[nCntModel]);
										}
									}
									if (strstr(&Realize[0], "POS") != NULL)
									{
										pEqual = strstr(Realize, "=");

										if (pEqual != NULL)
										{
											// アドレスを1こずらす
											pEqual++;

											// モデルの位置を読み込む
											(void)sscanf(pEqual, "%f %f %f", &pos.x, &pos.y, &pos.z);
										}
									}
									if (strstr(&Realize[0], "ROT") != NULL)
									{
										pEqual = strstr(Realize, "=");

										if (pEqual != NULL)
										{
											// アドレスを1こずらす
											pEqual++;

											// モデルの向きを読み込む
											(void)sscanf(pEqual, "%f %f %f", &rot.x, &rot.y, &rot.z);
										}
									}
									if (strstr(&Realize[0], "SHADOW") != NULL)
									{
										pEqual = strstr(Realize, "=");

										if (pEqual != NULL)
										{
											// アドレスを1こずらす
											pEqual++;

											// 影の有無を読み込む
											(void)sscanf(pEqual, "%d", &nUseShadow);
										}
									}
									if (strstr(&Realize[0], "END_PARTSSET") != NULL)
									{
										//Set3DModel
										m_apModel[nCntModel] = CModel::Create(pos, rot, nNumIdx, nIdxParent[nCntModel], &aModelName[nCntModel][0]);
										nCntModel++;	// モデルのカウントアップ
										break;
									}
								}
							}
						}
						else if (strstr(&Realize[0], "END_MODELSET") != NULL)
						{// モデルセットの終了
							// --- 親のポインタに代入 --- //
							for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
							{
								if (nIdxParent[nCnt] > -1)
								{
									m_apModel[nCnt]->SetParent(m_apModel[nIdxParent[nCnt]]);
								}
							}

							break;
						}
					}
				}
			}
			else if (strcmp(&Realize[0], "END_SCRIPT") == 0)
			{// スクリプト読み込みを終了する	
				break;
			}
		}

		fclose(pFile);	// ファイルを閉じる
	}
	else
	{// ファイルオープン失敗時
		return E_FAIL;
	}

	return S_OK;
}

// =================================================
// スクリプト読み込みでのコメントアウトの処理
// =================================================
bool CPlayer::JudgeComent(char* pStr)
{
	// アドレス取得用のchar型のポインタを用意
	char* pTrash = {};

	if (strncmp(pStr, "#", 1) != 0 && strncmp(pStr, "\n", 1) != 0)
	{// 文頭が、コメントアウト、改行でなかった場合
		// #があるかを判別
		pTrash = strstr(pStr, "#");

		if (pTrash != NULL)
		{// #が存在する
			// それ以降の文字列を消去
			strcpy(pTrash, "");
		}

		while (1)
		{// tabが存在する場合回し続ける
			// tabのアドレスを取得
			pTrash = strstr(pStr, "\t");

			if (pTrash != NULL)
			{// tabが存在する
				if (pTrash + 1 == NULL)
				{// 文末にあった時
					// 後ろを空白にする
					strcpy(pTrash + 1, "");
				}
				else
				{// 文頭にあった時
					// tab以降の文字列をtabがあったところに代入
					strcpy(pTrash, pTrash + 1);
				}
			}
			else if (pTrash == NULL)
			{// tabが存在しなくなったとき
				break;
			}
		}

		return true;
	}
	else
	{// コメントアウト、改行が文頭だった場合
		return false;
	}
}

// =================================================
// 向きの正規化
// =================================================
void CPlayer::NorRotate(float fRotDest)
{
	// --- カメラ情報の取得 --- //
	CCamera* pCamera = CGame::GetCamera();

	D3DXVECTOR3 CameraRot = pCamera->GetRotation();	// 向きの取得

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

// =================================================
// 移動制限
// =================================================
void CPlayer::LimitMove(void)
{
	if (m_pos.x + m_fRadius >= 950.0f)
	{
		m_nCntHitWall++;

		m_pos.x = -m_fRadius + 950.0f;

		// --- 移動制限のリアクション --- //
		if (m_nCntHitWall % REACTRING_CREATESPAN == 0)
		{
			m_pReactRing = CMeshRing::Create({ 950.0f, 20.0f, m_pos.z }, { 0.0f,  -D3DX_PI * 2.0f , D3DX_PI * 0.5f }, EXPAND_INRADIUS, EXPAND_OUTRADIUS, REACTRING_DEVIDE, CMesh::TYPE_RING, true, NULL);
			m_pReactRing->SetExpansion(true);
			m_pReactRing->SetExpansionSpan(EXPAND_SPEED);
			m_pReactRing->SetColor(COLOR_WHITE);
			m_pReactRing->SetType(CMeshRing::TYPE_HIT);
			m_pReactRing->SetNor({ -1.0f, 0.0f, 0.0f });
		}

		m_bHitwall = true;	// 衝突フラグ
	}
	if (m_pos.x - m_fRadius <= -950.0f)
	{
		m_nCntHitWall++;
		m_pos.x = m_fRadius - 950.0f;

		// --- 移動制限のリアクション --- //
		if (m_nCntHitWall % REACTRING_CREATESPAN == 0)
		{
			m_pReactRing = CMeshRing::Create({ -950.0f, 20.0f, m_pos.z }, { 0.0f,  -D3DX_PI , D3DX_PI * 0.5f }, EXPAND_INRADIUS, EXPAND_OUTRADIUS, REACTRING_DEVIDE, CMesh::TYPE_RING, true, NULL);
			m_pReactRing->SetExpansion(true);
			m_pReactRing->SetExpansionSpan(EXPAND_SPEED);
			m_pReactRing->SetColor(COLOR_WHITE);
			m_pReactRing->SetType(CMeshRing::TYPE_HIT);
			m_pReactRing->SetNor({ 1.0f, 0.0f, 0.0f });
		}
		
		m_bHitwall = true;	// 衝突フラグ
	}
	if (m_pos.z + m_fRadius >= 950.0f)
	{
		m_nCntHitWall++;
		m_pos.z = -m_fRadius + 950.0f;

		// --- 移動制限のリアクション --- //
		if (m_nCntHitWall % REACTRING_CREATESPAN == 0)
		{
			m_pReactRing = CMeshRing::Create({ m_pos.x, 20.0f, 950.0f }, { 0.0f, -D3DX_PI * 0.5F , D3DX_PI * 0.5f }, EXPAND_INRADIUS, EXPAND_OUTRADIUS, REACTRING_DEVIDE, CMesh::TYPE_RING, true, NULL);
			m_pReactRing->SetExpansion(true);
			m_pReactRing->SetExpansionSpan(EXPAND_SPEED);
			m_pReactRing->SetColor(COLOR_WHITE);
			m_pReactRing->SetType(CMeshRing::TYPE_HIT);
			m_pReactRing->SetNor({ 0.0f, 0.0f, -1.0f });
		}

		m_bHitwall = true;	// 衝突フラグ
	}
	if (m_pos.z - m_fRadius <= -950.0f)
	{
		m_nCntHitWall++;
		m_pos.z = m_fRadius - 950.0f;

		// --- 移動制限のリアクション --- //
		if (m_nCntHitWall % REACTRING_CREATESPAN == 0)
		{
			m_pReactRing = CMeshRing::Create({ m_pos.x, 20.0f, -930.0f }, { 0.0f, D3DX_PI * 0.5F , D3DX_PI * 0.5f }, EXPAND_INRADIUS, EXPAND_OUTRADIUS, REACTRING_DEVIDE, CMesh::TYPE_RING, true, NULL);
			m_pReactRing->SetExpansion(true);
			m_pReactRing->SetExpansionSpan(EXPAND_SPEED);
			m_pReactRing->SetColor(COLOR_WHITE);
			m_pReactRing->SetType(CMeshRing::TYPE_HIT);
			m_pReactRing->SetNor({ 0.0f, 0.0f, 1.0f });
		}

		m_bHitwall = true;	// 衝突フラグ
	}
}

// =================================================
// 充電用UIのメッシュリング処理
// =================================================
void CPlayer::BatteryRing(void)
{
	// --- オフセットの位置を保存 --- //
	D3DXVECTOR3 OffSet;
	D3DXVECTOR3 Pos;

	// --- メッシュリングをプレイヤーの頭上に設定 --- //
	OffSet = { 0.0f, m_fRadius + 80.0f, 0.0f };
	D3DXVec3TransformCoord(&Pos, &OffSet, &m_mtxWorldPos);

	if (m_pRing[0] != NULL)
	{
		m_pRing[0]->SetPosition(Pos);
	}

	OffSet = { 0.0f, m_fRadius + 60.0f, 0.0f };
	D3DXVec3TransformCoord(&Pos, &OffSet, &m_mtxWorldPos);

	if (m_pRing[1] != NULL)
	{
		m_pRing[1]->SetPosition(Pos);
	}

	OffSet = { 0.0f, m_fRadius + 40.0f, 0.0f };
	D3DXVec3TransformCoord(&Pos, &OffSet, &m_mtxWorldPos);

	if (m_pRing[2] != NULL)
	{
		m_pRing[2]->SetPosition(Pos);
	}

	OffSet = { 0.0f, m_fRadius + 20.0f, 0.0f };
	D3DXVec3TransformCoord(&Pos, &OffSet, &m_mtxWorldPos);

	if (m_pRing[3] != NULL)
	{
		m_pRing[3]->SetPosition(Pos);
	}

	// --- 熱ゲージの残り数でのメッシュリングの処理 --- //
	if (m_fHeat >= 450.0f)
	{
		for (int nCnt = 0; nCnt < MAX_MATER; nCnt++)
		{
			if (m_pRing[nCnt] == NULL)
			{// 使っていなかったら
				m_pRing[nCnt] = CMeshRing::Create(m_pos, VEC_NULL, 10.0f, 20.0f, 16);
				m_pRing[nCnt]->SetColor((COLOR_GREEN + COLOR_YELLOW) * 0.5f);
			}
		}
	}
	else if (m_fHeat < 450.0f && m_fHeat >= 300.0f)
	{
		for (int nCnt = 1; nCnt < MAX_MATER; nCnt++)
		{
			if (m_pRing[nCnt] == NULL)
			{// 使っていなかったら
				m_pRing[nCnt] = CMeshRing::Create(m_pos, VEC_NULL, 10.0f, 20.0f, 16);
				m_pRing[nCnt]->SetColor((COLOR_GREEN + COLOR_YELLOW) * 0.5f);
			}
		}

		if (m_pRing[0] != NULL)
		{
			m_pRing[0]->Uninit();
			m_pRing[0] = NULL;
		}
	}
	else if (m_fHeat < 300.0f && m_fHeat >= 150.0f)
	{
		for (int nCnt = 2; nCnt < MAX_MATER; nCnt++)
		{
			if (m_pRing[nCnt] == NULL)
			{// 使っていなかったら
				m_pRing[nCnt] = CMeshRing::Create(m_pos, VEC_NULL, 10.0f, 20.0f, 16);
				m_pRing[nCnt]->SetColor((COLOR_GREEN + COLOR_YELLOW) * 0.5f);
			}
		}

		if (m_pRing[1] != NULL)
		{
			m_pRing[1]->Uninit();
			m_pRing[1] = NULL;
		}
	}
	else if (m_fHeat < 150.0f && m_fHeat >= 0.0f)
	{
		for (int nCnt = 3; nCnt < MAX_MATER; nCnt++)
		{
			if (m_pRing[nCnt] == NULL)
			{// 使っていなかったら
				m_pRing[nCnt] = CMeshRing::Create(m_pos, VEC_NULL, 10.0f, 20.0f, 16);
				m_pRing[nCnt]->SetColor((COLOR_GREEN + COLOR_YELLOW) * 0.5f);
			}
		}

		if (m_pRing[2] != NULL)
		{
			m_pRing[2]->Uninit();
			m_pRing[2] = NULL;
		}
	}
	else if (m_fHeat < 0.0f)
	{
		if (m_pRing[3] != NULL)
		{
			m_pRing[3]->Uninit();
			m_pRing[3] = NULL;
		}
	}

	// --- リングの色設定 --- //
	if (m_pRing[0] != NULL)
	{
		for (int nCnt = 0; nCnt < MAX_MATER; nCnt++)
		{
			if (m_pRing[nCnt] != NULL)
			{// 50%以下でUIの色を黄色に変更
				m_pRing[nCnt]->SetColor(COLOR_LIGHTBLUE);
			}
		}
	}
	else if (m_pRing[1] == NULL)
	{
		for (int nCnt = 2; nCnt < MAX_MATER; nCnt++)
		{
			if (m_pRing[nCnt] != NULL)
			{// 50%以下でUIの色を黄色に変更
				m_pRing[nCnt]->SetColor(COLOR_YELLOW);
			}
		}
	}
	else if (m_pRing[1] != NULL)
	{
		for (int nCnt = 0; nCnt < MAX_MATER; nCnt++)
		{
			if (m_pRing[nCnt] != NULL)
			{// UIの色変更
				m_pRing[nCnt]->SetColor((COLOR_GREEN + COLOR_YELLOW) * 0.5f);
			}
		}
	}

	if (m_fHeat <= 100 && m_pRing[3] != NULL)
	{
		m_pRing[3]->SetColor(COLOR_RED);

		m_pRing[3]->FlashRing(10);
	}
}

// =================================================
// バウンド処理
// =================================================
void CPlayer::Bound(void)
{
	// --- バウンド力の設定 --- //
	if (m_move.y < m_fSaveMove && m_move.y != m_fRebound && m_bSetJumpForce == false)
	{
		m_fRebound = m_move.y;
		m_bSetJumpForce = true;
	}

	if (m_pos.y <= m_fposField + PLAYER_RADIUS)
	{
		//m_fRebound += (0.0f - m_fRebound) * 0.2f;	// バウンド力の減衰
		m_move.y = m_fRebound * 0.75f;	// バウンド力の設定

		m_bSetJumpForce = false;	// バウンド力セットフラグの解除

		m_pos.y = m_fposField + PLAYER_RADIUS;

		if (m_move.y < 0.0f)
		{
			m_bJump = false;	// ジャンプフラグの解除
			m_move.y = 0.0f;
		}
	}
}

// =================================================
// メッシュフィールドとの当たり判定
// =================================================
void CPlayer::MeshColl(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得
	CSound* pSound = CManager::GetSound();

	CField* pSnowField = CGame::GetField(CField::TYPE_SNOW);	// 雪のフィールド情報の取得
	CField* pBottomField = CGame::GetField(CField::TYPE_BOTTOM);	// 下の床の情報取得
	CField* pHeatRightRoad = CGame::GetField(CField::TYPE_RIGHTHEAT);	// ロードヒーティングフィールドの情報取得
	CField* pHeatLeftRoad = CGame::GetField(CField::TYPE_LEFTHEAT);		// ロードヒーティングフィールドの情報取得
	CField* pHeatSecretRoad = CGame::GetField(CField::TYPE_CENTERHEAT);		// 隠れロードヒーティングフィールドの情報取得

	// --- 雪の高さ --- //
	m_fposField = pSnowField->Collision(m_pos, m_posOld);

	if (pHeatRightRoad->bCollision(m_pos, m_posOld) == true)
	{// ロードヒーティングフィールド上に存在
		if (m_scl.y <= 2.5f)
		{// 大幅なスピードを抑える
			m_fValueRot += (0.6f - m_fValueRot) * 0.01f;
		}

		// --- 溶ける効果音再生 --- //
		if (m_bSoundMelt == false) {
			pSound->PlaySound(CSound::SOUND_LABEL_MELT);
			m_bSoundMelt = true;
		}

		if (m_fHeat >= NUM_COEINCREASE)
		{
			m_fHeat += 2.0f;
		}
		else if (m_fHeat < NUM_COEINCREASE)
		{
			m_fHeat += 4.0f;
		}

		InCreaseHeatGauge();	// 熱ゲージ上昇処理
	}
	else if (pHeatLeftRoad->bCollision(m_pos, m_posOld) == true)
	{// ロードヒーティングフィールド上に存在
		if (m_scl.y <= 2.5f)
		{// 大幅なスピードを抑える
			m_fValueRot += (0.6f - m_fValueRot) * 0.01f;
		}

		// --- 溶ける効果音再生 --- //
		if (m_bSoundMelt == false) {
			pSound->PlaySound(CSound::SOUND_LABEL_MELT);
			m_bSoundMelt = true;
		}

		if (m_fHeat >= NUM_COEINCREASE)
		{
			m_fHeat += 2.0f;
		}
		else if (m_fHeat < NUM_COEINCREASE)
		{
			m_fHeat += 4.0f;
		}

		InCreaseHeatGauge();	// 熱ゲージ上昇処理
	}
	else if (pHeatSecretRoad->bCollision(m_pos, m_posOld) == true)
	{// ロードヒーティングフィールド上に存在
		if (m_scl.y <= 2.5f)
		{// 大幅なスピードを抑える
			m_fValueRot += (0.6f - m_fValueRot) * 0.01f;
		}

		// --- 溶ける効果音再生 --- //
		if (m_bSoundMelt == false) {
			pSound->PlaySound(CSound::SOUND_LABEL_MELT);
			m_bSoundMelt = true;
		}

		if (m_fHeat >= NUM_COEINCREASE)
		{
			m_fHeat += 3.0f;
		}
		else if (m_fHeat < NUM_COEINCREASE)
		{
			m_fHeat += 6.0f;
		}

		InCreaseHeatGauge();	// 熱ゲージ上昇処理
	}
	else
	{
		pSound->Stop(CSound::SOUND_LABEL_MELT);
		m_bSoundMelt = false;	// 溶かす効果音フラグの削除
	}

	if (m_fposField <= 0.0f)
	{// 2層目のメッシュフィールドよりも低ければ普通の当たり判定
		m_fposField = pBottomField->Collision(m_pos, m_posOld);
	}
}

// =================================================
// アイテムとの当たり判定
// =================================================
void CPlayer::ItemColl(void)
{
	// --- アイテムの取得処理 --- //
	for (int nCnt = 0; nCnt < MAX_ITEM; nCnt++)
	{
		CTreasure* pTreasure = CItem::GetItem(nCnt);
		CItem* pItemManager = CGame::GetItem();

		if (pTreasure != NULL)
		{// NULLチェック
			if (pTreasure->Collision(&m_pos, m_posOld, &m_move) == true)
			{
				CMeshRing* pMeshRing = pTreasure->GetMeshRing();
				pMeshRing->SetExpansion(true);
				pMeshRing->SetExpansionSpan(3.0f);
				pMeshRing->SetColor(COLOR_RED);

				pItemManager->PanishItem(nCnt);	// アイテムのポインタをNULLにする
			}
		}
	}
}

// =================================================
// 壁との当たり判定
// =================================================
void CPlayer::WallColl(void)
{
	// --- 壁との当たり判定 --- //
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		CWall* pWall = CGame::GetWall(nCnt);

		if (pWall != NULL)
		{
			pWall->Collision(&m_pos, &m_move, m_posOld);
		}
	}
}

// =================================================
// モデルとの当たり判定
// =================================================
void CPlayer::ModelColl(void)
{
	CStageModel* pStageModel = CGame::GetStagemodel();

	pStageModel->Collision(&m_pos, m_posOld, &m_move);
}

// =================================================
// 熱ゲージ上昇処理
// =================================================
void CPlayer::InCreaseHeatGauge(void)
{
	m_fHeat += 2.0f;;

	if (m_fHeat < NUM_SCALEDOWN)
	{// 水蒸気の表現
		CParticle::Create(m_pos, VEC_NULL, { 2.0f, 5.0f, 2.0f }, { 1.0f, 25.0f, 1.0f }, COLOR_GLAY, 35.0f, 35.0f, 5, 10);
	}
	else if (m_scl.y > 1.1f)
	{
		CParticle::Create(m_pos, VEC_NULL, { 2.0f, 5.0f, 2.0f }, { 1.0f, 25.0f, 1.0f }, COLOR_GLAY, 35.0f, 35.0f, 5, 10);
	}

	if (m_fHeat >= MAX_HEATGAUGE)
	{// 熱ゲージの上限
		m_fHeat = MAX_HEATGAUGE;
	}
}

// =================================================
// 回転量の変化(デバッグ用)
// =================================================
void CPlayer::ChangeRotate(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得

	if (pInputKeyboard->GetPress(DIK_UP) == true)
	{
		if (m_fValueRot < 0.59f)
		{
			m_fValueRot += 0.01f;
		}
	}

	if (pInputKeyboard->GetPress(DIK_DOWN) == true)
	{
		if (m_fValueRot > 0.01f)
		{
			m_fValueRot -= 0.01f;
		}
	}

	if (pInputKeyboard->GetTrigger(DIK_2) == true)
	{
		if (m_scl.y < 4.0f)
		{
			m_scl.x += 0.1f;
			m_scl.y += 0.1f;
			m_scl.z += 0.1f;
		}
	}

	if (pInputKeyboard->GetTrigger(DIK_1) == true)
	{
		if (m_scl.y > 1.0f)
		{
			m_scl.x -= 0.1f;
			m_scl.y -= 0.1f;
			m_scl.z -= 0.1f;
		}
	}

	if (pInputKeyboard->GetTrigger(DIK_F) == true)
	{
		m_fHeat = -600.0f;
	}

	if (pInputKeyboard->GetTrigger(DIK_R) == true)
	{
		m_fHeat = 600.0f;
	}

	if (pInputKeyboard->GetPress(DIK_W) == true)
	{
		m_fValueRot += (0.35f - m_fValueRot) * 0.2f;

		if (m_fHeat <= 600.0f)
		{
			m_fHeat += 10.0f;
		}

		if (m_scl.y > 1.0f)
		{
			m_scl.x -= 0.05f;
			m_scl.y -= 0.05f;
			m_scl.z -= 0.05f;
		}

		if (m_fHeat < 0.0f)
		{
			CParticle::Create(m_pos, VEC_NULL, { 2.0f, 5.0f, 2.0f }, { 1.0f, 15.0f, 1.0f }, COLOR_GLAY, 35.0f, 35.0f, 5, 10);
		}
	}

	static int nIdx;

	if (pInputKeyboard->GetTrigger(DIK_9) == true)
	{
		nIdx++;

		if (nIdx == 3)
		{
			nIdx = 0;
		}
	}
	if (pInputKeyboard->GetTrigger(DIK_0) == true)
	{
		nIdx--;

		if (nIdx == -1)
		{
			nIdx = 2;
		}
	}

	if (nIdx == 0)
	{
		m_pFileName = "data\\model\\item\\face.x";
	}
	else if (nIdx == 1)
	{
		m_pFileName = "data\\model\\item\\apple.x";
	}
	else if (nIdx == 2)
	{
		m_pFileName = "data\\model\\item\\grape.x";
	}

	if (pInputKeyboard->GetTrigger(DIK_SPACE) == true)
	{
		CModelParticle::Create(m_pos, VEC_NULL, { 2.0f, 5.0f, 2.0f }, { 2.0f, 10.0f, 2.0f }, COLOR_WHITE, 10, 5, 20, m_pFileName);
	}

	if (pInputKeyboard->GetPress(DIK_H) == true)
	{
		m_fHeat = 1400.0f;
	}
}

// =================================================
// 雪を踏んだ処理
// =================================================
void CPlayer::HitSnowField(void)
{
	if (m_bHitSnowField == true)
	{
		m_fHeat -= 1.5f;	// 熱ゲージの減少

		if (m_fHeat <= MIN_HEATGAUGE)
		{// 熱ゲージの下限
			m_fHeat = MIN_HEATGAUGE;
		}

		if (m_fHeat <= NUM_SCALEUP)
		{
			// --- スケールの増加 --- //
			if (m_scl.y < 4.9f)
			{
				m_scl.x += 0.2f;
				m_scl.y += 0.2f;
				m_scl.z += 0.2f;
			}

			// --- 吸収パーティクルの生成 --- //
			if (m_fHeat <= MIN_HEATGAUGE) {
				CParticle::Create(m_pos, VEC_NULL, { 5.0f, 5.0f, 5.0f }, { 2.0f, 15.0f, 2.0f }, COLOR_LIGHTBLUE, 7.0f, 7.0f, 2, 5, 6, CParticle::TYPE_FOLLOW);
			}
		}
	}
}
