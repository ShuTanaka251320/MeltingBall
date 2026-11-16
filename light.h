// =================================================
// 
// ライト処理[light.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _LIGHT_H_
#define _LIGHT_H_

#include"main.h"

// =================================================
// マクロ定義
#define MAX_LIGHT	(3)		// ライトの最大数

class CLight
{
public:
	CLight();
	~CLight();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

private:
	D3DLIGHT9 m_aLight[MAX_LIGHT];	// ライトの情報
};

#endif