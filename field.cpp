// =================================================
// 
// フィールド処理[field.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "main.h"
#include "field.h"
#include "manager.h"
#include "input.h"
#include "macro.h"
#include "debugproc.h"
#include "texture.h"
#include "player.h"
#include "game.h"
#include <assert.h>
#include <stdio.h>

// =================================================
// マクロ定義

// =================================================
// コンストラクタ
// =================================================
CField::CField()
{

}

// =================================================
// デストラクタ
// =================================================
CField::~CField()
{

}

// =================================================
// フィールドの生成処理
// =================================================
CField* CField::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSizeX, float fSizeZ, float fMaxXSize, float fMaxZSize,
	int DevideX, int DevideZ, CMesh::TYPE type, bool bDraw, const char* pFileName)
{
	CField* pField = new CField;	// メッシュフィールドのインスタンス生成

	// --- 初期化 --- //
	pField->Init(pos, rot, fSizeX, fSizeZ, fMaxXSize, fMaxZSize,
		DevideX, DevideZ, type, bDraw, pFileName);

	return pField;
}

// =================================================
// フィールドの初期化処理
// =================================================
HRESULT CField::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSizeX, float fSizeZ, float fMaxXSize, float fMaxZSize,
	int DevideX, int DevideZ, CMesh::TYPE type, bool bDraw, const char* pFileName)
{
	// --- メッシュの初期化処理 --- //
	CMeshField::Init(pos, rot, fSizeX, fSizeZ, fMaxXSize, fMaxZSize, DevideX, DevideZ, TYPE_FIELD, true, pFileName);

	return S_OK;
}

// =================================================
// フィールドの終了処理
// =================================================
void CField::Uninit(void)
{
	CMeshField::Uninit();

}

// =================================================
// フィールドの更新処理
// =================================================
void CField::Update()
{
	// --- 変数宣言 --- //
	CDebugProc* pDebugProc = CManager::GetDrbugProc();	// デバッグ表示のポインタ

	CMeshField::Update();
}

// =================================================
// フィールドの描画処理
// =================================================
void CField::Draw(void)
{
	CMeshField::Draw();
}

// =================================================
// 頂点座標の減少割合算出
// =================================================
int CField::GetPersentHeight(void)
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	VERTEX_3D* pVtx;

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	float nCntUnderField = 0;
	float nCntRoadHeat = 0;
	float VtxNum = GetNumVtx();

	for (int nCnt = 0; nCnt < VtxNum; nCnt++)
	{
		if (pVtx[nCnt].pos.y < 0.0f)
		{// 雪のフィールドが道路フィールドよりも下に存在
			if (pVtx[nCnt].pos.x > -FIELD_SIZE / 2 && pVtx[nCnt].pos.x < (-FIELD_SIZE / 2) + ROADHEAT_SIZEX)
			{
				nCntRoadHeat++;
				continue;
			}
			if (pVtx[nCnt].pos.x < FIELD_SIZE / 2 && pVtx[nCnt].pos.x > FIELD_SIZE / 2 - ROADHEAT_SIZEX)
			{
				nCntRoadHeat++;
				continue;
			}

			nCntUnderField++;
		}
	}

	float Persent = (nCntUnderField) / (VtxNum - nCntRoadHeat);	// 割合を算出
//	Persent = Persent - (Persent / 10 * 3);

	int nPersent = Persent * 1000.0f;	// 整数に変換

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();

	return nPersent;
}

// =================================================
// ステージの起伏作成
// =================================================
void CField::CreateStage(void)
{
	// --- 変数宣言 --- //
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得

	CPlayer* pPlayer = CGame::GetPlayer();							// プレイヤーの取得
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	D3DXVECTOR3 PlayerPos = {};

	// --- 現在のプレイヤーの位置を、メッシュフィールドの左上を基準で算出 --- //
	PlayerPos.x = pPlayer->GetPosition().x + (GetSizeX() * GetDevideX()) / 2;
	PlayerPos.z = pPlayer->GetPosition().z - (GetSizeZ() * GetDevideZ()) / 2;

	PlayerPos.z *= -1;	// 負の符号の除去

	// --- 相対値を出して、どのポリゴン上にいるかを特定する --- //
	int nIdxX = (int)PlayerPos.x / (int)GetSizeX();
	int nIdxZ = (int)PlayerPos.z / (int)GetSizeZ();

	// --- 求めたいポリゴンの頂点を算出 --- //
	int nIdxVtx = nIdxX + nIdxZ * (GetDevideX() + 1);

	VERTEX_3D* pVtx;

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// --- メッシュフィールドに起伏をつける処理 --- //
	if (pInputKeyboard->GetPress(DIK_0) == true)
	{
		pVtx[nIdxVtx].pos.y += 2.0f;
		pVtx[nIdxVtx + (GetDevideX() + 1)].pos.y += 2.0f;
		//pVtx[nIdxVtx + (pMesh->GetDevideX() + 2)].pos.y += 3.0f;
		//pVtx[nIdxVtx + 1].pos.y += 3.0f;
	}
	else if (pInputKeyboard->GetPress(DIK_9) == true)
	{
		pVtx[nIdxVtx].pos.y -= 2.0f;
		pVtx[nIdxVtx + (GetDevideX() + 1)].pos.y -= 2.0f;
		//pVtx[nIdxVtx + (pMesh->GetDevideX() + 2)].pos.y -= 3.0f;
		//pVtx[nIdxVtx + 1].pos.y -= 3.0f;
	}

	if (pInputKeyboard->GetTrigger(DIK_F9) == true)
	{// 地形保存
		FILE* pFile;

		pFile = fopen(MESHSTAGESAVE_FILE, "w"); // ファイルを開く

		if (pFile != NULL)
		{// ファイルオープン成功時
			for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++) {
				fprintf(pFile, "%.2f %.2f %.2f\n", pVtx[nCnt].pos.x, pVtx[nCnt].pos.y, pVtx[nCnt].pos.z);
			}

			fclose(pFile);
		}
		else
		{// ファイルが存在しない
			pVtx[nIdxVtx].pos.y = 100000.0f;
		}

	}

	if (pInputKeyboard->GetTrigger(DIK_F10) == true)
	{// 地形読み込み
		FILE* pFile;

		pFile = fopen(MESHSTAGE_FILE, "r"); // ファイルを開く

		if (pFile != NULL)
		{// ファイルオープン成功時
			for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++) {
				(void)fscanf(pFile, "%f %f %f\n", &pVtx[nCnt].pos.x, &pVtx[nCnt].pos.y, &pVtx[nCnt].pos.z);

			}

			fclose(pFile);
		}
		else
		{// ファイルが存在しない

		}
	}

	if (pInputKeyboard->GetTrigger(DIK_F11) == true)
	{// 地形リセット
		for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++) {
			if (pVtx[nCnt].pos.y != 0.0f) {
				pVtx[nCnt].pos.y = 0.0f;
			}
		}
	}

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();
}
