// =================================================
// 
// 壁の処理[wall.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include"wall.h"

// =================================================
// マクロ定義


// =================================================
// コンストラクタ
// =================================================
CWall::CWall(int nPriority) :CObject3D(nPriority)
{

}

// =================================================
// デストラクタ
// =================================================
CWall::~CWall()
{

}

// =================================================
// 生成処理
// =================================================
CWall* CWall::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, CObject::TYPE type, const char* pFileName)
{
	CWall* pWall;

	pWall = new CWall;

	pWall->Init(pos, rot, move, col, fWidth, fHeight, type, pFileName);

	return pWall;
}

// =================================================
// 初期化処理
// =================================================
HRESULT CWall::Init(void)
{
	return S_OK;
}

// =================================================
// 初期化処理
// =================================================
HRESULT CWall::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, CObject::TYPE type, const char* pFileName)
{
	CObject3D::Init(pos, rot, move, col, fWidth, fHeight, type, pFileName);


	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CWall::Uninit(void)
{
	CObject3D::Uninit();
}


// =================================================
// 更新処理
// =================================================
void CWall::Update(void)
{
	CObject3D::Update();
}

// =================================================
// 描画処理
// =================================================
void CWall::Draw(void)
{
	CObject3D::Draw();
}

// =================================================
// 当たり判定()
// =================================================
bool CWall::Collision(D3DXVECTOR3* pPos, D3DXVECTOR3* pMove, D3DXVECTOR3 PosOld)
{
	// --- 位置と向きを取得 --- //
	D3DXVECTOR3 pos, rot;

	pos = GetPosition();
	rot = GetRotation();

	if (pos.z - pPos->z <= 10.0f && rot.z == 0.0f && rot.y == 0.0f)
	{
		if (pos.x - GetWidth() / 2 <= pPos->x && pos.x + GetWidth() / 2 >= pPos->x)
		{
			pPos->z = pos.z - 10.0f;
			return true;
		}
	}

	if (pPos->z - pos.z <= 10.0f && rot.y == D3DX_PI && rot.z == 0.0f)
	{
		if (pos.x - GetWidth() / 2 <= pPos->x && pos.x + GetWidth() / 2 >= pPos->x)
		{
			pPos->z = pos.z + 10.0f;
			return true;
		}
	}

	if (pos.x - pPos->x <= 10.0f && rot.z == D3DX_PI * 0.5f)
	{
		if (pos.z - GetWidth() / 2 <= pPos->z && pos.z + GetWidth() / 2 >= pPos->z)
		{
			pPos->x = pos.x - 10.0f;
			return true;
		}
	}

	if (pPos->x - pos.x <= 10.0f && rot.z == -D3DX_PI * 0.5f)
	{
		if (pos.z - GetWidth() / 2 <= pPos->z && pos.z + GetWidth() / 2 >= pPos->z)
		{
			pPos->x = pos.x + 10.0f;
			return true;
		}
	}

	return false;
}

