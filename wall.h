// =================================================
// 
// 壁の処理[wall.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _WALL_H_
#define _WALL_H_

#include"main.h"
#include"object3D.h"

// =================================================
// マクロ定義
#define WALL_PRI	(4)

// =================================================
// クラス定義
class CWall :public CObject3D
{
public:
	CWall(int nPriority = WALL_PRI);
	~CWall();

	static CWall* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, CObject::TYPE type, const char* pFileName = NULL);
	HRESULT Init(void);
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, CObject::TYPE type, const char* pFileName = NULL);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	bool Collision(D3DXVECTOR3* pPos, D3DXVECTOR3* pMove, D3DXVECTOR3 PosOld);

private:

};

#endif