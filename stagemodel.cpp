// =================================================
// 
// ステージモデル管理処理[stagemodel.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "stagemodel.h"
#include "camera.h"
#include "manager.h"
#include "macro.h"
#include "texture.h"
#include "objectX.h"
#include "player.h"
#include "model.h"
#include "particle.h"
#include "meshfield.h"
#include "meshring.h"
#include "debugproc.h"
#include "shadow.h"
#include "input.h"
#include "score.h"
#include "tutorial.h"
#include "object2D.h"
#include "sound.h"
#include "player.h"
#include "modelparticle.h"
#include<stdio.h>

// =================================================
// マクロ定義
#define MESH_SIZE	(6)		// メッシュの分割数
#define	STAGE_FILE	"data\\Scripts\\stage.txt"	// 読み込むステージのファイル名

// =================================================
// 静的メンバ変数宣言
int CStageModel::m_nNumAll = 0;		// モデル総数
CStageObj* CStageModel::m_pStageObj[MAX_MODEL] = {};	// ステージモデルのポインタ

// =================================================
// コンストラクタ
// =================================================
CStageModel::CStageModel(int nPriority)
{

}

// =================================================
// デストラクタ
// =================================================
CStageModel::~CStageModel()
{

}

// =================================================
// 初期化処理
// =================================================
HRESULT CStageModel::Init(void)
{
	if (m_nNumAll < 0)
	{
		m_nNumAll = 0;
	}

	// --- モデルの設置 --- //
	LoadStage();

	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CStageModel::Uninit(void)
{
	memset(m_pStageObj, 0, sizeof m_pStageObj);	// ステージモデルポインタの初期化
}

// =================================================
// 更新処理
// =================================================
void CStageModel::Update(void)
{

}

// =================================================
// 描画処理
// =================================================
void CStageModel::Draw(void)
{

}

// =================================================
// 当たり判定
// =================================================
bool CStageModel::Collision(D3DXVECTOR3* pPos, D3DXVECTOR3 pPosOld, D3DXVECTOR3* pMove)
{
	CObject* pObjTop = CObject::GetTop(STAGEMODEL_PRI);				// オブジェクトのポインタ
	CDebugProc* pDebugProc = CManager::GetDrbugProc();	// デバッグ表示のポインタ
	CJoyPad* pJoyPad = CManager::GetJoyPad();		// ジョイパッドの取得
	CCamera* pCamera = CGame::GetCamera();			// カメラの取得
	CPlayer* pPlayer = CGame::GetPlayer();			// プレイヤーのポインタ
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ
	CSound* pSound = CManager::GetSound();			// サウンドの取得
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得

	if (CManager::GetMode() == CScene::MODE_TUTORIAL)
	{
		pCamera = CTutorial::GetCamera();	// カメラの取得
		pPlayer = CTutorial::GetPlayer();	// プレイヤーのポインタ
	}

	// --- 当たり判定用の変数 --- //
	D3DXVECTOR3 pos = VEC_NULL;
	D3DXVECTOR3 vtxMax = VEC_NULL;
	D3DXVECTOR3 vtxMin = VEC_NULL;
	D3DXVECTOR3 vtxMaxLocal = VEC_NULL;
	D3DXVECTOR3 vtxMinLocal = VEC_NULL;

	D3DXVECTOR3 LinetoVec1 = VEC_NULL;
	D3DXVECTOR3 LinetoVec2 = VEC_NULL;
	D3DXVECTOR3 LinetoVec3 = VEC_NULL;
	D3DXVECTOR3 LinetoVec4 = VEC_NULL;

	// --- 指定したインデックスのオブジェクトを取得 --- //
	CObject::TYPE type;

	while (1)
	{
		CObject* pObjNext = pObjTop->GetNext();

		type = pObjTop->GetType();

		if (type != CObject::TYPE_STAGEMODEL || type == CStageObj::TYPE_STAGE)
		{// earlyreturn
			pObjTop = pObjNext;	// 次のオブジェクトのアドレスを代入

			if (pObjTop == NULL)
			{// 次がなければ処理を抜ける
				break;
			}

			continue;
		}

		// --- 各種キャストで可読性を上げる --- //
		CStageObj::TYPE type = dynamic_cast<CStageObj*>(pObjTop)->GetType();	// ステージモデルのタイプを取得

		if (type == CStageObj::TYPE_STAGE)
		{// earlyreturn
			pObjTop = pObjNext;	// 次のオブジェクトのアドレスを代入

			if (pObjTop == NULL)
			{// 次がなければ処理を抜ける
				break;
			}

			continue;
		}

		CStageModel* pModel = dynamic_cast<CStageModel*>(pObjTop);
		const D3DXMATRIX* mtxWorld = dynamic_cast<CObjectX*>(pObjTop)->GetWorldMtx();

		vtxMax = vtxMaxLocal = dynamic_cast<CObjectX*>(pObjTop)->GetVtxMax();
		vtxMin = vtxMinLocal = dynamic_cast<CObjectX*>(pObjTop)->GetVtxMin();

		D3DXVec3TransformCoord(&vtxMax, &vtxMax, mtxWorld);	// ワールド基準座標に変換
		D3DXVec3TransformCoord(&vtxMin, &vtxMin, mtxWorld);	// ワールド基準座標に変換

		pos = dynamic_cast<CObjectX*>(pObjTop)->GetPosition();

		D3DXVECTOR3 PosVtx[4] = { VEC_NULL, VEC_NULL, VEC_NULL, VEC_NULL };
		D3DXVECTOR3 PosVtxLocal[4] = { VEC_NULL, VEC_NULL, VEC_NULL, VEC_NULL };
		D3DXVECTOR3 LineVec[4] = { VEC_NULL, VEC_NULL, VEC_NULL, VEC_NULL };
		D3DXVECTOR3 VectoPos[4] = { VEC_NULL, VEC_NULL, VEC_NULL, VEC_NULL };

		float OnPolygon[4] = {};

		PosVtxLocal[0] = PosVtx[0] = D3DXVECTOR3(vtxMinLocal.x - pPlayer->GetRadius() * 1.5f, vtxMinLocal.y, vtxMaxLocal.z + pPlayer->GetRadius() * 1.5f);
		D3DXVec3TransformCoord(&PosVtx[0], &PosVtx[0], mtxWorld);

		PosVtxLocal[1] = PosVtx[1] = D3DXVECTOR3(vtxMaxLocal.x + pPlayer->GetRadius() * 1.5f, vtxMinLocal.y, vtxMaxLocal.z + pPlayer->GetRadius() * 1.5f);
		D3DXVec3TransformCoord(&PosVtx[1], &PosVtx[1], mtxWorld);

		PosVtxLocal[2] = PosVtx[2] = D3DXVECTOR3(vtxMaxLocal.x + pPlayer->GetRadius() * 1.5f, vtxMinLocal.y, vtxMinLocal.z - pPlayer->GetRadius() * 1.5f);
		D3DXVec3TransformCoord(&PosVtx[2], &PosVtx[2], mtxWorld);

		PosVtxLocal[3] = PosVtx[3] = D3DXVECTOR3(vtxMinLocal.x - pPlayer->GetRadius() * 1.5f, vtxMinLocal.y, vtxMinLocal.z - pPlayer->GetRadius() * 1.5f);
		D3DXVec3TransformCoord(&PosVtx[3], &PosVtx[3], mtxWorld);

		// --- プレイヤーとのベクトルを算出 --- //
		for (int nCnt = 0; nCnt < 4; nCnt++) {
			VectoPos[nCnt] = *pPos - PosVtx[nCnt];
		}

		// --- モデルの境界線ベクトルを算出 --- //
		LineVec[0] = PosVtx[1] - PosVtx[0];
		LineVec[1] = PosVtx[2] - PosVtx[1];
		LineVec[2] = PosVtx[3] - PosVtx[2];
		LineVec[3] = PosVtx[0] - PosVtx[3];

		// --- プレイヤーが境界線ベクトル内にいるかの判定 --- //
		for (int nCnt = 0; nCnt < 4; nCnt++) {
			OnPolygon[nCnt] = (LineVec[nCnt].z * VectoPos[nCnt].x) - (LineVec[nCnt].x * VectoPos[nCnt].z);
		}

		// --- 各種判定用変数の宣言 --- //
		float fRate;		// 面積比率
		D3DXVECTOR3 CrossPoint;	// モデルとの交点	
		D3DXVECTOR3 EdgePos;	// モデルの端の座標	
		D3DXVECTOR3 vecNor = {};		// 側面の逆法線ベクトル
		D3DXVECTOR3 vecCutLine = {};	// 交点からの境界線ベクトル
		D3DXVECTOR3 vecToPosOld = {};	// プレイヤーの過去位置とのベクトル 
		D3DXVECTOR3 vecLineNor = {};	// 正規化した境界線ベクトル
		float fVecPosToNor = 0.0f;		// 逆法線との外積
		D3DXVECTOR3 vecLine = {};		// 境界線ベクトル
		D3DXVECTOR3 vecToPos = {};		// プレイヤーとのベクトル
		D3DXVECTOR3 vecMove = {};		// 移動ベクトル
		float fVecPos = 0.0f;
		float fVecPosOld = 0.0f;
		bool bInModel[4] = { false, false, false, false };	// モデル内判定
		bool bUnderModel[4] = { false, false, false, false };	// モデル内判定

		float fPosToMove = 0.0f;					// vecToPosとの外積
		float fLineToMove = 0.0f;					// vecLineとの外積

		D3DXVECTOR3 vec;

		vecMove = *pPos - pPosOld;	// 移動ベクトル

		// --- 側面の当たり判定処理 --- //
		for (int nCnt = 0; nCnt < 4; nCnt++)
		{
			// --- 各種境界線ベクトルの算出 --- //
			vecLine = PosVtx[(nCnt + 1) % 4] - PosVtx[nCnt];

			int nRepairedFloat;	// 正規化用

			// --- 正規化 --- //
			nRepairedFloat = (int)vecLine.x * 10 * 3;
			nRepairedFloat /= 10 * 3;
			vecLine.x = (float)nRepairedFloat;

			// --- 現在位置との関係を外積を使い求める --- //
			vecToPos = *pPos - PosVtx[nCnt];

			fVecPos = (vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z);

			// --- 正規化 --- //
			nRepairedFloat = (int)fVecPos * 10 * 3;
			nRepairedFloat /= 10 * 3;
			fVecPos = (float)nRepairedFloat;

			// --- 過去位置との関係を外積を使い求める --- //
			vecToPosOld = pPosOld - PosVtx[nCnt];

			fVecPosOld = (vecLine.z * vecToPosOld.x) - (vecLine.x * vecToPosOld.z);

			// --- 正規化 --- //
			nRepairedFloat = (int)fVecPosOld * 10 * 3;
			nRepairedFloat /= 10 * 3;
			fVecPosOld = (float)nRepairedFloat;

			// --- 現在位置との外積 --- //
			fPosToMove = (vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z);

			// --- 最大値との外積 --- //
			fLineToMove = (vecLine.z * vecMove.x) - (vecLine.x * vecMove.z);

			// --- 比率の算出 --- //
			fRate = fPosToMove / fLineToMove;

			vecNor.x = vecLine.z;
			vecNor.z = -vecLine.x;

			D3DXVec3Normalize(&vecNor, &vecNor);

			// --- 法線との外積 --- //
			fVecPosToNor = (vecNor.z * vecMove.x) - (vecNor.x * vecMove.z);

			vecCutLine = vecLine;	// 境界線ベクトルの複製

			// --- 外積(向き)の結果 --- //
			if (fVecPosToNor == 0)
			{
				fVecPosToNor = fabsf(fVecPosToNor);	// 絶対値に変換
				D3DXVec3Normalize(&vecCutLine, &vecCutLine);

				vec = vecCutLine * fVecPosToNor;
			}
			else if (fVecPosToNor > 0)
			{
				fVecPosToNor = fabsf(fVecPosToNor);
				D3DXVec3Normalize(&vecCutLine, &vecCutLine);

				vec = -vecCutLine * fVecPosToNor;
			}
			else if (fVecPosToNor < 0)
			{
				fVecPosToNor = fabsf(fVecPosToNor);
				D3DXVec3Normalize(&vecCutLine, &vecCutLine);

				vec = vecCutLine * fVecPosToNor;
			}

			// ---  モデルタイプごとに異なる処置 --- //
			if (type == CStageObj::TYPE_LIGHT)
			{// 街灯の処理
				if (fVecPos >= -20.0f && fVecPosOld <= 20.0f)
				{// 現在はモデル内だが、過去はモデル外の位置
					// --- 交点の算出 --- //
					if (fRate <= 1.0f && fRate >= 0.0f)
					{
						pPos->x = PosVtx[nCnt].x + (vecLine.x * fRate) + vec.x;
						pPos->z = PosVtx[nCnt].z + (vecLine.z * fRate) + vec.z;

						if (pPlayer->GetheatGauge() >= NUM_SETMATERIALRED) {
							dynamic_cast<CStageObj*>(pObjTop)->SetbHit(true);	// 衝突フラグを立てる	

							// --- 効果音の再生 --- //
							if (dynamic_cast<CStageObj*>(pObjTop)->GetbPlaySound() == false)
							{
								pSound->PlaySound(CSound::SOUND_LABEL_LIGHT);
								dynamic_cast<CStageObj*>(pObjTop)->SetbPlaySound(true);
							}
						}

						return true;
					}
				}
			}
			else if (type == CStageObj::TYPE_SNOWMAN)
			{// 雪だるまの処理
				if (fVecPos >= -20.0f && fVecPosOld <= 20.0f)
				{
					if (fRate <= 1.0f && fRate >= 0.0f)
					{
						// --- プレイヤーのスケールによる処理の変動 --- //
						if (pPlayer->GetheatGauge() >= NUM_SETMATERIALRED)
						{// マテリアルの色が赤いとき
							CModelParticle::Create(pos, VEC_NULL, { 8.0f, 5.0f, 8.0f }, { -4.0f, 10.0f, -4.0f }, COLOR_WHITE, 3, 10, 15, "data\\MODEL\\snowball.x");
							dynamic_cast<CStageObj*>(pObjTop)->Uninit();

							// --- 効果音の再生 --- //
							pSound->PlaySound(CSound::SOUND_LABEL_SNOWMAN);

							return true;
						}
						else if (pPlayer->GetScl().y < 3.0f)
						{// 小さいとき
							pPos->x = PosVtx[nCnt].x + (vecLine.x * fRate) + vec.x;
							pPos->z = PosVtx[nCnt].z + (vecLine.z * fRate) + vec.z;

							return true;
						}
						else if (pPlayer->GetScl().y >= 3.0f)
						{// 大きいとき
							CModelParticle::Create(pos, VEC_NULL, { 8.0f, 5.0f, 8.0f }, { -4.0f, 10.0f, -4.0f }, COLOR_WHITE, 3, 10, 15, "data\\MODEL\\snowball.x");
							dynamic_cast<CStageObj*>(pObjTop)->Uninit();	// モデルの消去

							// --- 効果音の再生 --- //
							pSound->PlaySound(CSound::SOUND_LABEL_SNOWMAN);

							return true;
						}
					}
				}
			}
			else if (type == CStageObj::TYPE_HYDRANT)
			{
				if (fVecPos >= -20.0f && fVecPosOld <= 20.0f)
				{// 現在はモデル内だが、過去はモデル外の位置
					// --- 交点の算出 --- //
					if (fRate <= 1.0f && fRate >= 0.0f)
					{// 側面の当たり判定
						pPos->x = PosVtx[nCnt].x + (vecLine.x * fRate) + vec.x;
						pPos->z = PosVtx[nCnt].z + (vecLine.z * fRate) + vec.z;

						dynamic_cast<CStageObj*>(pObjTop)->SetbHit(true);	// 衝突フラグを立てる

						if (dynamic_cast<CStageObj*>(pObjTop)->GetbPlaySound() == false)
						{
							// --- 効果音の再生 --- //
							pSound->PlaySound(CSound::SOUND_LABEL_HYDRANT);
							dynamic_cast<CStageObj*>(pObjTop)->SetbPlaySound(true);
						}

						return true;
					}
				}
			}
			else if (type == CStageObj::TYPE_CORN)
			{
				if (fVecPos >= -30.0f && fVecPosOld <= 30.0f)
				{// 現在はモデル内だが、過去はモデル外の位置
					// --- 交点の算出 --- //
					if (fRate <= 1.0f && fRate >= 0.0f)
					{// 側面の当たり判定
						D3DXVECTOR3 VecMove;

						VecMove = pos - pPlayer->GetPosition();	// 移動方向ベクトルの算出
						D3DXVec3Normalize(&VecMove, &VecMove);

						dynamic_cast<CStageObj*>(pObjTop)->SetVec(VecMove);	// 移動方向ベクトルの設定
						dynamic_cast<CStageObj*>(pObjTop)->SetfMove(20.0f);	// 移動量の設定
						dynamic_cast<CStageObj*>(pObjTop)->ResetHitCnt();	// ヒットフラグカウンタのリセット
						dynamic_cast<CStageObj*>(pObjTop)->SetbHit(true);	// 衝突フラグを立てる

						// --- パーティクルの生成 --- //
						CParticle::Create(pos, VEC_NULL, { 2.0f, 3.0f, 2.0f }, { -2.0f, 3.0f, -2.0f }, COLOR_YELLOW, 5.0f, 5.0f, 3, 5, 6, CParticle::TYPE_DEF);

						// --- 効果音の再生 --- //
						pSound->PlaySound(CSound::SOUND_LABEL_CORNHIT);
						pSound->PlaySound(CSound::SOUND_LABEL_CORNSPIN);

						return true;
					}
				}
			}
			else
			{
				if (fVecPos >= -20.0f && fVecPosOld <= 20.0f)
				{// 現在はモデル内だが、過去はモデル外の位置
					// --- 交点の算出 --- //
					if (fRate <= 1.0f && fRate >= 0.0f)
					{
						//if (pPos->y <= PosVtx[nCnt].y - 0.5f && pPos->y >= vtxMin.y - 20.0f)
						{// 側面の当たり判定
							pPos->x = PosVtx[nCnt].x + (vecLine.x * fRate) + vec.x;
							pPos->z = PosVtx[nCnt].z + (vecLine.z * fRate) + vec.z;

							return true;
						}
					}
				}
			}

			if (fVecPos > 0 && fVecPosOld > 0 && pPos->y >= vtxMin.y - 20.0f)
			{// 現在も過去もモデル内
				bInModel[nCnt] = true;
			}
			else if (fVecPos > 0 && fVecPosOld > 0 && pPos->y >= vtxMin.y - 80.0f)
			{// 現在も過去もモデル内
				bUnderModel[nCnt] = true;
			}
		}

		pObjTop = pObjNext;	// 次のオブジェクトのアドレスを代入
	}

	return false;
}

// =================================================
// 横からの当たり判定処理
// =================================================
bool CStageModel::Coll(float fVecPos, float fVecPosOld, D3DXVECTOR3 vecLine, D3DXVECTOR3 vec, D3DXVECTOR3 vtxMin, float fRate, D3DXVECTOR3* pPos, D3DXVECTOR3 PosVtx[4], int nIdx)
{
	if (fVecPos > 0 && fVecPosOld <= 0)
	{// 現在はモデル内だが、過去はモデル外の位置
		// --- 交点の算出 --- //
		if (fRate <= 1.0f && fRate >= 0.0f)
		{
			if (pPos->y <= PosVtx[nIdx].y - 0.5f && pPos->y >= vtxMin.y - 20.0f)
			{// 側面の当たり判定
				pPos->x = PosVtx[nIdx].x + (vecLine.x * fRate) + vec.x;
				pPos->z = PosVtx[nIdx].z + (vecLine.z * fRate) + vec.z;

				return true;
			}
		}
	}
}

// =================================================
// 当たり判定(アイテム)
// =================================================
bool CStageModel::Collision(D3DXVECTOR3* pPos, D3DXVECTOR3 pPosOld, D3DXVECTOR3* pShadowHeight, CObject::TYPE Objtype)
{
	CObject* pObjTop = CObject::GetTop(STAGEMODEL_PRI);	// オブジェクトのポインタ

	// --- 当たり判定用の変数 --- //
	D3DXVECTOR3 pos = {};
	D3DXVECTOR3 vtxMax = {};
	D3DXVECTOR3 vtxMin = {};
	D3DXVECTOR3 vtxMaxLocal = {};
	D3DXVECTOR3 vtxMinLocal = {};

	D3DXVECTOR3 LinetoVec1 = {};
	D3DXVECTOR3 LinetoVec2 = {};
	D3DXVECTOR3 LinetoVec3 = {};
	D3DXVECTOR3 LinetoVec4 = {};

	// --- 指定したインデックスのオブジェクトを取得 --- //
	CObject::TYPE type;

	while (1)
	{
		CObject* pObjNext = pObjTop->GetNext();

		type = pObjTop->GetType();

		if (type != CObject::TYPE_STAGEMODEL)
		{// ステージモデルでない場合最初に戻る
			pObjTop = pObjNext;	// 次のオブジェクトのアドレスを代入

			if (pObjTop == NULL)
			{// 次がなければ処理を抜ける
				break;
			}

			continue;
		}

		const D3DXMATRIX* mtxWorld = dynamic_cast<CObjectX*>(pObjTop)->GetWorldMtx();

		vtxMax = vtxMaxLocal = dynamic_cast<CObjectX*>(pObjTop)->GetVtxMax();
		vtxMin = vtxMinLocal = dynamic_cast<CObjectX*>(pObjTop)->GetVtxMin();

		D3DXVec3TransformCoord(&vtxMax, &vtxMax, mtxWorld);	// ワールド基準座標に変換
		D3DXVec3TransformCoord(&vtxMin, &vtxMin, mtxWorld);	// ワールド基準座標に変換

		D3DXVECTOR3 PosVtx[4] = {};
		D3DXVECTOR3 PosVtxLocal[4] = {};
		D3DXVECTOR3 LineVec[4] = {};
		D3DXVECTOR3 VectoPos[4] = {};

		float OnPolygon[4] = {};

		PosVtxLocal[0] = PosVtx[0] = D3DXVECTOR3(vtxMinLocal.x, vtxMaxLocal.y, vtxMaxLocal.z);
		D3DXVec3TransformCoord(&PosVtx[0], &PosVtx[0], mtxWorld);

		PosVtxLocal[1] = PosVtx[1] = D3DXVECTOR3(vtxMaxLocal.x, vtxMaxLocal.y, vtxMaxLocal.z);
		D3DXVec3TransformCoord(&PosVtx[1], &PosVtx[1], mtxWorld);

		PosVtxLocal[2] = PosVtx[2] = D3DXVECTOR3(vtxMaxLocal.x, vtxMaxLocal.y, vtxMinLocal.z);
		D3DXVec3TransformCoord(&PosVtx[2], &PosVtx[2], mtxWorld);

		PosVtxLocal[3] = PosVtx[3] = D3DXVECTOR3(vtxMinLocal.x, vtxMaxLocal.y, vtxMinLocal.z);
		D3DXVec3TransformCoord(&PosVtx[3], &PosVtx[3], mtxWorld);

		// --- プレイヤーとのベクトルを算出 --- //
		for (int nCnt = 0; nCnt < 4; nCnt++) {
			VectoPos[nCnt] = *pPos - PosVtx[nCnt];
			D3DXVec3Normalize(&VectoPos[nCnt], &VectoPos[nCnt]);
		}

		// --- モデルの境界線ベクトルを算出 --- //
		LineVec[0] = PosVtx[1] - PosVtx[0];
		LineVec[1] = PosVtx[2] - PosVtx[1];
		LineVec[2] = PosVtx[3] - PosVtx[2];
		LineVec[3] = PosVtx[0] - PosVtx[3];

		// --- オブジェクトが境界線ベクトル内にいるかの判定 --- //
		for (int nCnt = 0; nCnt < 4; nCnt++) {
			OnPolygon[nCnt] = (LineVec[nCnt].z * VectoPos[nCnt].x) - (LineVec[nCnt].x * VectoPos[nCnt].z);
		}

		if (OnPolygon[0] >= 0.0f && OnPolygon[1] >= 0.0f && OnPolygon[2] >= 0.0f && OnPolygon[3] >= 0.0f)
		{
			pShadowHeight->y = vtxMax.y;	// アイテムの影の高さを設定
			return true;
		}

		pObjTop = pObjNext;	// 次のオブジェクトのアドレスを代入
	}

	return false;
}

// =================================================
// ステージ読み込み
// =================================================
HRESULT CStageModel::LoadStage(void)
{
	// === モデル用の変数を用意 === //
	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 向き
	int nNumModel = 0;		// モデル数
	char Realize[128];		// 判別用の変数
	char aModelName[CStageObj::TYPE_MAX][128];	// モデルのファイル名
	char* pEqual = NULL;	// =を消去するためのポインタ
	int nUseShadow = 0;		// 影を使用するかどうか用の変数
	int nCntModel = 0;		// モデルのカウントアップ用(要素)
	int nCntModelNum = 0;	// モデルのカウントアップ用(名前)
	int nIdx = 0;			// モデルのファイル名インデックス

	// ステージ情報を読み込み
	FILE* pFile;
	pFile = NULL;

	pFile = fopen(STAGE_FILE, "r");

	if (pFile != NULL)
	{// ファイルを開いた
		while (1)
		{
			fgets(&Realize[0], sizeof Realize, pFile);	// 最初に比較用文字の読み込み

			if (CPlayer::JudgeComent(&Realize[0]) == true)
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
					sscanf(pEqual, "%d", &m_nNumAll);
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

					if (CPlayer::JudgeComent(&Realize[0]) == true)
					{
						if (strstr(&Realize[0], "INDEX") != NULL)
						{
							pEqual = strstr(Realize, "=");

							if (pEqual != NULL)
							{
								// アドレスを1こずらす
								pEqual++;

								// モデルのインデックスを読み込む
								(void)sscanf(pEqual, "%d", &nIdx);
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
						if (strstr(&Realize[0], "END_MODELSET") != NULL)
						{// モデルセットの終了	
						 //Set3DModel
							//CHANGERADIUS(rot.x);
							//CHANGERADIUS(rot.y);
							//CHANGERADIUS(rot.z);
							m_pStageObj[m_nNumAll] = CStageObj::Create(pos, rot, (CStageObj::TYPE)nIdx, &aModelName[nIdx][0]);
							nCntModel++;	// モデルのカウントアップ
							m_nNumAll++;	// 総数のカウントアップ
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


