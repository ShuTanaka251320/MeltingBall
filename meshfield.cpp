// =================================================
// 
// メッシュフィールド処理[meshfield.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "main.h"
#include "meshfield.h"
#include "manager.h"
#include "input.h"
#include "macro.h"
#include "debugproc.h"
#include "texture.h"
#include "player.h"
#include "camera.h"
#include "game.h"
#include "player.h"
#include "meshring.h"
#include "math.h"
#include <assert.h>
#include <stdio.h>

// =================================================
// マクロ定義
#define MESHFIELD_XSIZE		(100.0f)	// メッシュフィールドのXサイズ
#define MESHFIELD_XMAXSIZE	(3200.0f)	// メッシュフィールドのXの最大サイズ
#define MESHFIELD_X			(32)			// 列数(X)

#define MESHFIELD_ZSIZE		(100.0f)	// メッシュフィールドのZサイズ(奥行き)
#define MESHFIELD_ZMAXSIZE	(3200.0f)	// メッシュフィールドのZの最大サイズ
#define MESHFIELD_Z			(32)		// 行数(Z)

#define NUM_IDXBUFF			((2 * MESHFIELD_X) + 2 + (MESHFIELD_Z - 1) * ((2 * MESHFIELD_X) + 4))	// インデックスバッファの数
#define NUM_VERTEX			((MESHFIELD_X + 1) * (MESHFIELD_Z + 1))	// 頂点数	

// =================================================
// コンストラクタ
// =================================================
CMeshField::CMeshField()
{
	m_nIdxTexture = 0;	// テクスチャのインデックス
	m_nDevideX = 0;		// Xの分割数
	m_nDevideZ = 0;		// Zの分割数
	m_fDecreaseSpan = 0.0f;	// 頂点座標の下げる数値
	m_fIncreaseSpan = 0.0f;	// 頂点座標の上げる数値
}

// =================================================
// デストラクタ
// =================================================
CMeshField::~CMeshField()
{

}

// =================================================
// メッシュフィールドの生成処理
// =================================================
CMeshField* CMeshField::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSizeX, float fSizeZ, float fMaxXSize, float fMaxZSize,
	int DevideX, int DevideZ, CMesh::TYPE type, bool bDraw, const char* pFileName)
{
	CMeshField* pMeshField = new CMeshField;	// メッシュフィールドのインスタンス生成

	// --- 初期化 --- //
	pMeshField->Init(pos, rot, fSizeX, fSizeZ, fMaxXSize, fMaxZSize,
		DevideX, DevideZ, type, bDraw, pFileName);

	return pMeshField;
}

// =================================================
// メッシュフィールドの初期化処理
// =================================================
HRESULT CMeshField::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSizeX, float fSizeZ, float fMaxXSize, float fMaxZSize,
	int DevideX, int DevideZ, CMesh::TYPE type, bool bDraw, const char* pFileName)
{
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	int nNumVtx = (DevideX + 1) * (DevideZ + 1);	// 頂点数
	int nNumIdx = (2 * DevideX) + 2 + (DevideZ - 1) * ((2 * DevideX) + 4);	// インデックス数

	D3DXVECTOR3 Set = VEC_NULL;
	D3DXVECTOR3 MPos = { 0.0f, pos.y, 0.0f };

	//D3DXVec3TransformCoord(&Set, &MPos, &Worldmtx);

	// --- メッシュの初期化処理 --- //
	CMesh::Init(pos, rot, fSizeX, fSizeZ, fMaxXSize, fMaxZSize, DevideX, DevideZ, nNumVtx, nNumIdx, type, bDraw);

	// --- テクスチャの設定 --- //
	if (pFileName != NULL)
	{
		m_nIdxTexture = pTexture->Register(pFileName);
	}
	else
	{
		m_nIdxTexture = -1;
	}

	m_fDecreaseSpan = 4.0f;
	m_fIncreaseSpan = 3.0f;

	SetTexture(m_nIdxTexture);

	return S_OK;
}

// =================================================
// メッシュフィールドの終了処理
// =================================================
void CMeshField::Uninit(void)
{
	CMesh::Uninit();

}

// =================================================
// メッシュフィールドの更新処理
// =================================================
void CMeshField::Update()
{
	// --- 変数宣言 --- //
	CDebugProc* pDebugProc = CManager::GetDrbugProc();	// デバッグ表示のポインタ
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得

	if (pInputKeyboard->GetTrigger(DIK_LEFT) == true)
	{
		m_fDecreaseSpan -= 0.1f;
		m_fIncreaseSpan -= 0.1f;
	}
	if (pInputKeyboard->GetTrigger(DIK_RIGHT) == true)
	{
		m_fDecreaseSpan += 0.1f;
		m_fIncreaseSpan += 0.1f;
	}

	CMesh::Update();
}

// =================================================
// メッシュフィールドの描画処理
// =================================================
void CMeshField::Draw(void)
{
	CMesh::Draw();
}

// =================================================
// 頂点座標の保存
// =================================================
void CMeshField::Save(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	VERTEX_3D* pVtx;

	FILE* pFile;
	pFile = fopen("data\\scripts\\Allvtxsave.txt", "r");

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++)
	{
		fscanf(pFile, "%f %f %f\n", &pVtx[nCnt].pos.x, &pVtx[nCnt].pos.y, &pVtx[nCnt].pos.z);
	}

	fclose(pFile);

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();
}

// =================================================
// ゲーム終了時の頂点座標の保存
// =================================================
void CMeshField::EndSave(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	VERTEX_3D* pVtx;

	FILE* pFile;
	pFile = fopen("data\\scripts\\endvtx.txt", "w");

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++)
	{
		fprintf(pFile, "%f\n", pVtx[nCnt].pos.y);
	}

	fclose(pFile);

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();
}

// =================================================
// 頂点座標の読み込み
// =================================================
void CMeshField::Load(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	VERTEX_3D* pVtx;

	FILE* pFile;
	pFile = fopen("data\\scripts\\vtxsave.txt", "r");

	float trash = 0.0f;

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++)
	{
		if (pVtx[nCnt].pos.x >= -FIELD_SIZE / 2 && pVtx[nCnt].pos.x <= (-FIELD_SIZE / 2) + ROADHEAT_SIZEX)
		{
			fscanf(pFile, "%f %f %f\n", &trash, &pVtx[nCnt].pos.y, &trash);
		}
	}

	fclose(pFile);

	pFile = fopen("data\\scripts\\vtxsave.txt", "r");

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++)
	{
		if (pVtx[nCnt].pos.x <= FIELD_SIZE / 2 && pVtx[nCnt].pos.x >= FIELD_SIZE / 2 - ROADHEAT_SIZEX)
		{
			fscanf(pFile, "%f %f %f\n", &trash, &pVtx[nCnt].pos.y, &trash);
		}
	}

	fclose(pFile);

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();
}

// =================================================
// ゲーム終了時の頂点座標の読み込み
// =================================================
void CMeshField::LoadEndSave(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	VERTEX_3D* pVtx;

	FILE* pFile;
	pFile = fopen("data\\scripts\\endvtx.txt", "r");

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++)
	{
		fscanf(pFile, "%f\n", &pVtx[nCnt].pos.y);
	}

	fclose(pFile);

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();
}

// =================================================
// 頂点座標を下げる
// =================================================
void CMeshField::DecreaseHeight(D3DXVECTOR3 pos, D3DXVECTOR3 posOld, D3DXVECTOR3 scl)
{
	// --- 変数宣言 --- //
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得

	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	D3DXVECTOR3 PlayerPos = {};

	// --- 現在のプレイヤーの位置を、メッシュフィールドの左上を基準で算出 --- //
	PlayerPos.x = pos.x + (GetSizeX() * GetDevideX()) / 2;
	PlayerPos.z = pos.z - (GetSizeZ() * GetDevideZ()) / 2;

	PlayerPos.z *= -1;	// 負の符号の除去

	// --- 相対値を出して、どのポリゴン上にいるかを特定する --- //
	int nIdxX = (int)PlayerPos.x / (int)GetSizeX();
	int nIdxZ = (int)PlayerPos.z / (int)GetSizeZ();

	// --- 求めたいポリゴンの頂点を算出 --- //
	int nIdxVtx = nIdxX + nIdxZ * (GetDevideX() + 1);

	VERTEX_3D* pVtx;

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	if (scl.y < 1.6f)
	{// 通常
		// --- メッシュフィールドに起伏をつける処理 --- //
		pVtx[nIdxVtx].pos.y -= m_fDecreaseSpan;
		pVtx[nIdxVtx + (GetDevideX() + 1)].pos.y -= m_fDecreaseSpan;
		pVtx[nIdxVtx + (GetDevideX() + 2)].pos.y -= m_fDecreaseSpan;
		pVtx[nIdxVtx + 1].pos.y -= m_fDecreaseSpan;
	}
	else if (scl.y >= 1.6f && scl.y < 2.5f)
	{// 大
		// --- メッシュフィールドに起伏をつける処理 --- //
		pVtx[nIdxVtx].pos.y -= m_fDecreaseSpan;
		pVtx[nIdxVtx + (GetDevideX() + 1)].pos.y -= m_fDecreaseSpan;
		pVtx[nIdxVtx + (GetDevideX() + 2)].pos.y -= m_fDecreaseSpan;
		pVtx[nIdxVtx + 1].pos.y -= m_fDecreaseSpan;

		if ((nIdxVtx + 1) % (GetDevideX() + 1) != GetDevideX())
		{
			pVtx[nIdxVtx + 2].pos.y -= m_fDecreaseSpan;
			pVtx[nIdxVtx + (GetDevideX() + 1) + 2].pos.y -= m_fDecreaseSpan;
		}

		if (nIdxVtx % (GetDevideX() + 1) != 0)
		{
			pVtx[nIdxVtx - 1].pos.y -= m_fDecreaseSpan;
			pVtx[nIdxVtx + (GetDevideX() + 1) - 2].pos.y -= m_fDecreaseSpan;
		}

		CCamera* pCamera = CGame::GetCamera();

		if ((pCamera->GetRotation().y >= D3DX_PI * 0.25f && pCamera->GetRotation().y <= D3DX_PI * 0.75f)
			|| (pCamera->GetRotation().y <= -D3DX_PI * 0.25f && pCamera->GetRotation().y >= -D3DX_PI * 0.75f))
		{
			// --- 上のポリゴンも下げる --- //
			pVtx[nIdxVtx - (GetDevideX() + 1)].pos.y -= m_fDecreaseSpan;
			pVtx[nIdxVtx - (GetDevideX() + 2)].pos.y -= m_fDecreaseSpan;

			// --- 下のポリゴンも下げる --- //
			pVtx[nIdxVtx + (GetDevideX() + 1) + (GetDevideX() + 1)].pos.y -= m_fDecreaseSpan;
			pVtx[nIdxVtx + (GetDevideX() + 2) + (GetDevideX() + 2)].pos.y -= m_fDecreaseSpan;
		}

	}
	else if (scl.y >= 2.5f)
	{// 超巨大
		// --- メッシュフィールドに起伏をつける処理 --- //
		pVtx[nIdxVtx].pos.y -= m_fDecreaseSpan;
		pVtx[nIdxVtx + (GetDevideX() + 1)].pos.y -= m_fDecreaseSpan;
		pVtx[nIdxVtx + (GetDevideX() + 2)].pos.y -= m_fDecreaseSpan;
		pVtx[nIdxVtx + 1].pos.y -= m_fDecreaseSpan;

		if ((nIdxVtx + 1) % (GetDevideX() + 1) != GetDevideX())
		{
			pVtx[nIdxVtx + 2].pos.y -= m_fDecreaseSpan;
			pVtx[nIdxVtx + (GetDevideX() + 1) + 2].pos.y -= m_fDecreaseSpan;
		}

		if (nIdxVtx % (GetDevideX() + 1) != 0)
		{
			pVtx[nIdxVtx - 1].pos.y -= m_fDecreaseSpan;
			pVtx[nIdxVtx + (GetDevideX() + 1) - 1].pos.y -= m_fDecreaseSpan;
		}
	}

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();
}

// =================================================
// 頂点座標を上げる
// =================================================
void CMeshField::IncreaseHeight(D3DXVECTOR3 pos, D3DXVECTOR3 posOld)
{
	// --- 変数宣言 --- //
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得

	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得

	D3DXVECTOR3 PlayerPos = {};

	// --- 現在のプレイヤーの位置を、メッシュフィールドの左上を基準で算出 --- //
	PlayerPos.x = pos.x + (GetSizeX() * GetDevideX()) / 2;
	PlayerPos.z = pos.z - (GetSizeZ() * GetDevideZ()) / 2;

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
	pVtx[nIdxVtx].pos.y += m_fIncreaseSpan;
	pVtx[nIdxVtx + (GetDevideX() + 1)].pos.y += m_fIncreaseSpan;
	pVtx[nIdxVtx + (GetDevideX() + 2)].pos.y -= m_fIncreaseSpan;
	pVtx[nIdxVtx + 1].pos.y -= m_fIncreaseSpan;

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();
}

// =================================================
// 頂点座標を初期化
// =================================================
void CMeshField::ResetHeight(void)
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得
	VERTEX_3D* pVtx;

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++)
	{
		pVtx[nCnt].pos.y = 25.0f;
	}

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();
}

// =================================================
// 頂点座標を強制降下
// =================================================
void CMeshField::DownHeight(void)
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得
	VERTEX_3D* pVtx;

	// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++)
	{
		pVtx[nCnt].pos.y = -100.0f;
	}

	// --- 頂点バッファをアンロックする --- //
	pVtxBuff->Unlock();
}

// =================================================
// メッシュのフィールドとリングの当たり判定
// =================================================
void CMeshField::CollRing(void)
{
	CPlayer* pPlayer = CGame::GetPlayer();	// プレイヤーの取得
	if (pPlayer != NULL) {
		CMeshRing* pRing = pPlayer->GetCollRing();	// 当たり判定用リングの取得

		float fLength;	// フィールドの頂点座標とリングの距離
		D3DXVECTOR3 VtxPos;	// 頂点座標のpos
		D3DXVECTOR3 RingPos;	// リングのpos
		float fRingRadius = pRing->GetInRadius();	// リングの内径

		LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();			// 頂点情報の取得
		VERTEX_3D* pVtx;

		// --- 頂点バッファをロックし、頂点情報へのポインタを取得 --- //
		pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCnt = 0; nCnt < GetNumVtx(); nCnt++)
		{
			VtxPos = pVtx[nCnt].pos;	// 頂点座標をコピー
			RingPos = pRing->GetPosition();	// リングの位置を取得

			fLength = CMath::CalLength(&VtxPos, &RingPos);	// 頂点とリングの距離を計算

			// --- リングの円内で頂点座標を下げる --- //
			if (fLength <= fRingRadius && VtxPos.y >= -30.0f) {// プレイヤーの直径内
				float fRate = (fRingRadius - fLength) / fRingRadius;	// リングの中心との割合

				pVtx[nCnt].pos.y -= m_fDecreaseSpan * fRate;	// 頂点座標の減少
				pPlayer->SetbHitSnow(true);	// ヒットフラグを設定
			}

		}

		// --- 頂点バッファをアンロックする --- //
		pVtxBuff->Unlock();
	}
}


