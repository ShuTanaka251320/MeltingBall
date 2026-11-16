// =================================================
// 
// 便利計算関数[math.h]
// Author : Shu Tanaka
// 
// =================================================
#include"math.h"

// =================================================
// コンストラクタ
// =================================================
CMath::CMath()
{

}

// =================================================
// デストラクタ
// =================================================
CMath::~CMath()
{

}

// =================================================
// 距離を算出
// =================================================
float CMath::CalLength(D3DXVECTOR3* pPos1, D3DXVECTOR3* pPos2)
{
	float fLength = powf((pPos1->x - pPos2->x) * (pPos1->x - pPos2->x) + (pPos1->z - pPos2->z) * (pPos1->z - pPos2->z), 0.5f);

	return fLength;
}

// =================================================
// rotを正規化
// =================================================
D3DXVECTOR3 CMath::RepairRot(D3DXVECTOR3 *pIn)
{
	if (pIn->x > D3DX_PI * 2)
	{
		pIn->x -= D3DX_PI * 2;
	}
	if (pIn->x < -D3DX_PI * 2)
	{
		pIn->x += D3DX_PI * 2;
	}

	if (pIn->y > D3DX_PI * 2)
	{
		pIn->y -= D3DX_PI * 2;
	}
	if (pIn->y < -D3DX_PI * 2)
	{
		pIn->y += D3DX_PI * 2;
	}

	if (pIn->z > D3DX_PI * 2)
	{
		pIn->z -= D3DX_PI * 2;
	}
	if (pIn->z < -D3DX_PI * 2)
	{
		pIn->z += D3DX_PI * 2;
	}

	return *pIn;
}
