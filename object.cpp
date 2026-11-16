// =================================================
// 
// オブジェクト処理[object.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include"object.h"
#include"object2D.h"
#include"manager.h"
#include"camera.h"
#include"renderer.h"
#include"game.h"
#include"title.h"
#include"result.h"
#include"scene.h"
#include"tutorial.h"

// =================================================
// メンバ変数宣言
CObject* CObject::m_apTop[MAX_PRI] = {};
CObject* CObject::m_apCur[MAX_PRI] = {};
int CObject::m_nNumAll = 0;

// =================================================
// コンストラクタ
// =================================================
CObject::CObject(int nPriority)
{
	// --- 先頭オブジェクトの設定 --- //
	if (m_apTop[nPriority] == NULL)
	{// 先頭がNULLだった場合、自分を先頭に設定
		m_apTop[nPriority] = this;
	}

	if (m_apCur[nPriority] != NULL)
	{// すでに最後尾が存在する(最初でない)場合

		// --- 現在最後尾に入っているアドレスを、前のオブジェクトアドレスに代入 --- //
		m_pPrev = m_apCur[nPriority];

		// --- 最後尾を自分に設定 --- //
		m_apCur[nPriority] = this;
	}
	else if (m_apCur[nPriority] == NULL)
	{// まだ1つもオブジェクトを生成していない

		// --- 最後尾を自分に設定 --- //
		m_apCur[nPriority] = this;

		// --- 自分の前のオブジェクトアドレスをNULLに --- //
		m_pPrev = NULL;
	}

	CObject* pObjectPrev = m_pPrev;

	if (pObjectPrev != NULL)
	{// 前のオブジェクトアドレスが存在
		pObjectPrev->m_pNext = m_apCur[nPriority];
	}
	else
	{// 前のオブジェクトアドレスが存在しない(自分が先頭)

	}

	m_pNext = NULL;

	m_nPriority = nPriority;	// 優先順位の設定
	m_nNumAll++;	// 総数を増やす
	m_bDeath = false;
}

// =================================================
// デストラクタ
// =================================================
CObject::~CObject()
{

}

// =================================================
// 全オブジェクトの破棄
// =================================================
void CObject::ReleaseAll(void)
{
	// --- アドレスを格納するための変数 --- //
	CObject* pObject[MAX_PRI];
	CObject* pObjectNext[MAX_PRI];

	for (int nCnt = 0; nCnt < MAX_PRI; nCnt++)
	{
		pObject[nCnt] = m_apTop[nCnt];	// 先頭のアドレスを取得

		while (pObject[nCnt] != NULL)
		{
			pObjectNext[nCnt] = pObject[nCnt]->m_pNext;	// 次のオブジェクトを保存

			// --- 終了処理 --- //
			pObject[nCnt]->Uninit();
			pObject[nCnt]->ObjPanish();

			pObject[nCnt] = pObjectNext[nCnt];	// 次のオブジェクトのアドレスを代入
		}
	}
}

// =================================================
// 全オブジェクトの更新処理
// =================================================
void CObject::UpdateAll(void)
{
	// --- アドレスを格納するための変数 --- //
	CObject* pObject[MAX_PRI];		// 現在のアドレス
	CObject* pObjectNext[MAX_PRI];	// 次のアドレス

	for (int nCnt = 0; nCnt < MAX_PRI; nCnt++)
	{
		pObject[nCnt] = m_apTop[nCnt];	// 先頭のアドレスを取得

		while (pObject[nCnt] != NULL)
		{
			pObjectNext[nCnt] = pObject[nCnt]->m_pNext;	// 次のオブジェクトを保存

			if (pObject[nCnt]->m_bDeath == false)
			{
				// --- 更新処理 --- //
				pObject[nCnt]->Update();
			}

			pObject[nCnt] = pObjectNext[nCnt];	// 次のオブジェクトのアドレスを代入
		}
	}

	for (int nCnt = 0; nCnt < MAX_PRI; nCnt++)
	{
		pObject[nCnt] = m_apTop[nCnt];	// 先頭のアドレスを取得

		while (pObject[nCnt] != NULL)
		{
			pObjectNext[nCnt] = pObject[nCnt]->m_pNext;	// 次のオブジェクトを保存

			if (pObject[nCnt]->m_bDeath == true)
			{
				// --- オブジェクトの破棄 --- //
				pObject[nCnt]->ObjPanish();
			}

			pObject[nCnt] = pObjectNext[nCnt];	// 次のオブジェクトのアドレスを代入
		}
	}
}

// =================================================
// 全オブジェクトの描画処理
// =================================================
void CObject::DrawAll(void)
{
	if (CManager::GetMode() == CScene::MODE_GAME)
	{
		CCamera* pCamera = CGame::GetCamera();

		// --- カメラの設置 --- //
		pCamera->SetCamera();
	}
	if (CManager::GetMode() == CScene::MODE_TITLE)
	{
		CCamera* pCamera = CTitle::GetCamera();

		// --- カメラの設置 --- //
		pCamera->SetCamera();
	}
	if (CManager::GetMode() == CScene::MODE_TUTORIAL)
	{
		CCamera* pCamera = CTutorial::GetCamera();

		// --- カメラの設置 --- //
		if (pCamera != NULL)
		{
			pCamera->SetCamera();
		}
	}
	if (CManager::GetMode() == CScene::MODE_RESULT)
	{
		CCamera* pCamera = CResult::GetCamera();

		// --- カメラの設置 --- //
		if (pCamera != NULL)
		{
			pCamera->SetCamera();
		}
	}

	// --- アドレスを格納するための変数 --- //
	CObject* pObject[MAX_PRI];		// 現在のアドレス
	CObject* pObjectNext[MAX_PRI];	// 次のアドレス

	for (int nCnt = 0; nCnt < MAX_PRI; nCnt++)
	{
		pObject[nCnt] = m_apTop[nCnt];	// 先頭のアドレスを取得

		while (pObject[nCnt] != NULL)
		{
			pObjectNext[nCnt] = pObject[nCnt]->m_pNext;	// 次のオブジェクトを保存

			// --- 描画処理 --- //
			pObject[nCnt]->Draw();

			pObject[nCnt] = pObjectNext[nCnt];	// 次のオブジェクトのアドレスを代入
		}
	}
}

// =================================================
// 自分自身の破棄
// =================================================
void CObject::ObjPanish(void)
{
	// --- オブジェクトのアドレスを格納する変数 --- //
	CObject* pObject = this;					// 破棄予定のアドレス
	CObject* pObjectNext = pObject->GetNext();	// 次のアドレス
	CObject* pObjectPrev = pObject->GetPrev();	// 前のアドレス

	if (pObjectNext != NULL && pObjectPrev != NULL)
	{// 前と後ろ、両方にオブジェクトが存在している

		// --- 前のオブジェクトアドレスを譲渡 --- //
		pObjectNext->m_pPrev = pObjectPrev;	// 繋ぎなおす

		// --- 後ろのオブジェクトアドレスを譲渡 --- //
		pObjectPrev->m_pNext = pObjectNext;	// 繋ぎなおす
	}
	else if (pObjectNext != NULL && pObjectPrev == NULL)
	{// オブジェクトが、後ろに存在しているが、前には存在しない(自分が先頭)

		// --- 次のオブジェクトアドレスを先頭に --- //
		m_apTop[pObject->m_nPriority] = pObjectNext;
		pObjectNext->m_pPrev = NULL;	// 先頭が存在しない
	}
	else if (pObjectNext == NULL && pObjectPrev != NULL)
	{// オブジェクトが、前に存在しているが、後ろに存在しない(自分が最後尾)

		// --- 前のアドレスを最後尾に --- //
		m_apCur[pObject->m_nPriority] = pObjectPrev;
		pObjectPrev->m_pNext = NULL;	// 後ろが存在しない
	}
	else
	{// 両方存在しない(自分1つだけ生成されている状態)
		m_apTop[pObject->m_nPriority] = NULL;
		m_apCur[pObject->m_nPriority] = NULL;
	}

	delete this;	// 自身の破棄
}
