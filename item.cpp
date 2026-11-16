// =================================================
// 
// アイテム処理[item.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "item.h"
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
#include "treasure.h"
#include<stdio.h>

// =================================================
// マクロ定義
#define MESH_SIZE	(4)		// メッシュの分割数
#define ITEM_FILENAME	("data\\SCRIPTS\\item.txt")	// アイテム配置の外部ファイル

// =================================================
// 静的メンバ変数宣言
int CItem::m_nNumAll = 0;		// アイテムの総数
int CItem::m_nGetNum = 0;		// アイテムの獲得数
CTreasure* CItem::m_pTreasure[MAX_ITEM] = {};	// アイテムのポインタ

// =================================================
// コンストラクタ
// =================================================
CItem::CItem(int nPriority)
{
	// --- メンバ変数のクリア --- //
	m_nGetNum = 0;
	m_nNumAll = 0;
}

// =================================================
// デストラクタ
// =================================================
CItem::~CItem()
{
	m_nNumAll--;
	m_nGetNum++;
}

// =================================================
// 初期化処理
// =================================================
HRESULT CItem::Init(void)
{
	return S_OK;
}

// =================================================4354
// 終了処理
// =================================================
void CItem::Uninit(void)
{
	memset(m_pTreasure, 0, sizeof(m_pTreasure));	// アイテムのポインタ
}

// =================================================
// 更新処理
// =================================================
void CItem::Update(void)
{
	CDebugProc* pDebugProc = CManager::GetDrbugProc();				// デバッグ表示のポインタ

	//for (int nCnt = 0; nCnt < m_nNumAll; nCnt++)
	//{
	//	if (m_pItem[nCnt]->m_bGet == true && m_pItem[nCnt]->m_bLive == true && m_bGetAny == false)
	//	{// 初回取得時
	//		CPlayer* pPlayer = CManager::GetPlayer();

	//		D3DXVECTOR3 Pos = pPlayer->GetPosition();
	//		D3DXVECTOR3 PosOld = pPlayer->GetPosOld();
	//		D3DXVECTOR3 Vec = PosOld - Pos;

	//		D3DXVECTOR3 OffSet = Vec * 15.0f;

	//		D3DXVec3TransformCoord(&m_pos, &OffSet, m_mtxNext);
	//		m_pItem[nCnt]->m_mtxFollow = m_mtxNext;

	//		m_mtxNext = m_pItem[nCnt]->GetWorldMtx();

	//		m_pItem[nCnt]->m_bLive = false;
	//		m_bGetAny = true;
	//	}
	//	else if (m_pItem[nCnt]->m_bGet == true && m_pItem[nCnt]->m_bLive == true && m_bGetAny == true)
	//	{// 2回目以降の取得時
	//		CPlayer* pPlayer = CManager::GetPlayer();

	//		D3DXVECTOR3 Pos = pPlayer->GetPosition();
	//		D3DXVECTOR3 PosOld = pPlayer->GetPosOld();
	//		D3DXVECTOR3 Vec = PosOld - Pos;

	//		D3DXVECTOR3 OffSet = Vec * 15.0f;

	//		D3DXVec3TransformCoord(&m_pItem[nCnt]->m_pos, &OffSet, m_mtxNext);
	//		m_pItem[nCnt]->m_mtxFollow = m_mtxNext;

	//		m_mtxNext = m_pItem[nCnt]->GetWorldMtx();

	//		m_pItem[nCnt]->m_bLive = false;
	//		m_bGetAny = true;
	//	}

	//	// ゲットした後の追従
	//	if (m_pItem[nCnt]->m_bGet == true && m_pItem[nCnt]->m_bLive == false)
	//	{
	//		CPlayer* pPlayer = CManager::GetPlayer();

	//		D3DXVECTOR3 Pos = pPlayer->GetPosition();
	//		D3DXVECTOR3 PosOld = pPlayer->GetPosOld();
	//		D3DXVECTOR3 Vec = PosOld - Pos;

	//		D3DXVECTOR3 OffSet = Vec * 15.0f;

	//		D3DXVec3TransformCoord(&m_pItem[nCnt]->m_pos, &OffSet, m_pItem[nCnt]->m_mtxFollow);
	//		m_pItem[nCnt]->SetPosition(m_pItem[nCnt]->m_pos);


	//	}

	//	// --- 壁との当たり判定 --- //
	//	for (int nCntWall = 0; nCntWall < 4; nCntWall++)
	//	{
	//		CWall* pWall = CManager::GetWall(nCntWall);

	//		if (pWall != NULL)
	//		{
	//			pWall->Collision(&m_pItem[nCnt]->m_pos, &m_move, m_pItem[nCnt]->m_pos);
	//			m_pItem[nCnt]->SetPosition(m_pItem[nCnt]->m_pos);
	//		}
	//	}
	//}
}

// =================================================
// 描画処理
// =================================================
void CItem::Draw(void)
{

}

// =================================================
// 外部ファイル読み込み
// =================================================
HRESULT CItem::LoadItem(void)
{
	FILE* pFile;
	pFile = fopen(ITEM_FILENAME, "r");

	// --- 読み込んだ値を格納する変数 --- //
	int nNumFile = 0;	// 読み込むモデルファイル数
	char aRealize[128];	// タグの判別用
	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 向き
	int nNumIdx = 0;		// モデルのインデックス
	int nNumModel = 0;		// モデル数
	char aModelName[MAX_PART][128];	// モデルのファイル名
	char* pEqual = NULL;	// =を消去するためのポインタ
	int nUseShadow = 0;		// 影を使用するかどうか用の変数
	int nCntModel = 0;		// モデルのカウントアップ用(要素)
	int nCntModelNum = 0;	// モデルのカウントアップ用(名前)

	if (pFile != NULL)
	{
		while (1)
		{
			fgets(&aRealize[0], sizeof aRealize, pFile);	// 最初に比較用文字の読み込み

			if (JudgeComent(&aRealize[0]) == true)
			{// 何かの情報を読み込む合図を取得したとき
				if (strcmp(&aRealize[0], "SCRIPT") == 0)
				{

				}
			}

			if (strstr(&aRealize[0], "NUM_MODEL") != NULL)
			{// モデルのファイル数を代入
				// 「=」の場所を見つける
				pEqual = strstr(aRealize, "=");

				if (pEqual != NULL)
				{
					// アドレスを1こずらす
					pEqual++;

					// モデルの数を読み込む
					sscanf(pEqual, "%d", &nNumFile);
				}
			}

			if (strstr(&aRealize[0], "MODEL_FILENAME") != NULL)
			{// モデルのファイル名を代入
				// 「=」の場所を見つける
				pEqual = strstr(aRealize, "=");

				if (pEqual != NULL)
				{
					// アドレスを1こずらす
					pEqual++;

					// モデルの名前を読み込む
					sscanf(pEqual, "%s", &aModelName[nCntModelNum][0]);
					nCntModelNum++;
				}
			}

			if (strstr(&aRealize[0], "MODELSET") != NULL)
			{
				while (1)
				{
					fgets(&aRealize[0], sizeof aRealize, pFile);	// 最初に比較用文字の読み込み

					if (JudgeComent(&aRealize[0]) == true)
					{
						if (strstr(&aRealize[0], "INDEX") != NULL)
						{
							pEqual = strstr(aRealize, "=");

							if (pEqual != NULL)
							{
								// アドレスを1こずらす
								pEqual++;

								// モデルのインデックスを読み込む
								(void)sscanf(pEqual, "%d", &nNumIdx);
							}
						}
						if (strstr(&aRealize[0], "POS") != NULL)
						{
							pEqual = strstr(aRealize, "=");

							if (pEqual != NULL)
							{
								// アドレスを1こずらす
								pEqual++;

								// モデルの位置を読み込む
								(void)sscanf(pEqual, "%f %f %f", &pos.x, &pos.y, &pos.z);
							}
						}
						if (strstr(&aRealize[0], "ROT") != NULL)
						{
							pEqual = strstr(aRealize, "=");

							if (pEqual != NULL)
							{
								// アドレスを1こずらす
								pEqual++;

								// モデルの向きを読み込む
								(void)sscanf(pEqual, "%f %f %f", &rot.x, &rot.y, &rot.z);
							}
						}
						if (strstr(&aRealize[0], "END_MODELSET") != NULL)
						{
							//Set3DModel
							m_pTreasure[nCntModel] = CTreasure::Create(pos, rot, (CItem::TYPE)nNumIdx, &aModelName[nNumIdx][0]);
							m_pTreasure[nCntModel]->SetPosition(pos);

							nCntModel++;	// モデルのカウントアップ
							break;
						}
					}
				}
			}

			if (JudgeComent(&aRealize[0]) == true)
			{
				if (strcmp(&aRealize[0], "END_SCRIPT") == 0)
				{// スクリプト読み込みを終了する	
					fclose(pFile);
					break;
				}
			}
		}

		return S_OK;
	}
	else
	{
		return E_FAIL;
	}

}

// =================================================
// スクリプト読み込みでのコメントアウトの処理
// =================================================
bool CItem::JudgeComent(char* pStr)
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
// 静的メンバ変数の初期化
// =================================================
void CItem::ResetItem(void)
{
	m_nNumAll = 0;
	m_nGetNum = 0;

	for (int nCnt = 0; nCnt < MAX_ITEM; nCnt++)
	{
		if (m_pTreasure[nCnt] != NULL)
		{
			delete m_pTreasure[nCnt];
		}
	}

	memset(m_pTreasure, 0, sizeof(m_pTreasure));	// アイテムのポインタ
}

// =================================================
// アイテムの消去
// =================================================
void CItem::PanishItem(int nIdx)
{
	m_pTreasure[nIdx]->Uninit();

	m_pTreasure[nIdx] = NULL;
	delete m_pTreasure[nIdx];
}

// =================================================
// アイテムの消去
// =================================================
void CItem::PanishItem(int nIdx, bool bReset)
{
	m_pTreasure[nIdx]->Uninit();

	m_pTreasure[nIdx] = NULL;
	delete m_pTreasure[nIdx];
}