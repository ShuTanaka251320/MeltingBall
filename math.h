// =================================================
// 
// 便利計算関数[math.h]
// Author : Shu Tanaka
// 
// =================================================

#ifndef _MATH_H_
#define _MATH_H_

#include"main.h"

// =================================================
// クラス定義
class CMath
{
public:
	CMath();
	~CMath();

	static float CalLength(D3DXVECTOR3 *pPos1, D3DXVECTOR3 *pPos2);
	static D3DXVECTOR3 RepairRot(D3DXVECTOR3* pIn);

private:

};

#endif