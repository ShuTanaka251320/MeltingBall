// =================================================
// 
// カメラ処理[camera.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include"main.h"

// =================================================
// マクロ定義
#define CAMERA_LENGTH	(-570)	// 視点と注視点の距離
#define CAMERA_HEIGHT	(400)	// 視点と注視点の距離(高さ)

// =================================================
// カメラのクラスを定義
class CCamera
{
public:

	CCamera();
	~CCamera();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void SetCamera(void);
	D3DXVECTOR3 GetPosition(void) { return m_posV; };
	D3DXVECTOR3 GetTarget(void) { return m_posR; };
	D3DXVECTOR3 GetVecU(void) { return m_vecU; };
	D3DXVECTOR3 GetRotation(void) { return m_rot; };
	float GetPerspective(void) { return m_fPerspective; };
	D3DXMATRIX GetMatrix(void) { return m_mtxProjection; };
	bool IsCameraMove(void) { return m_bCameraMove; };

	void SetRotation(D3DXVECTOR3 Vec) { m_rot = Vec; };
	void SetRotate(D3DXVECTOR3 rot, float fLength);
	void SetPerspective(float fPer, float fIni);
	void SetCameraHeight(float fHeight) { m_fHeight = fHeight; };

private:
	D3DXVECTOR3 m_posV;			// 視点
	D3DXVECTOR3 m_posVDest;		// 目的の視点
	D3DXVECTOR3 m_posR;			// 注視点
	D3DXVECTOR3 m_posRDest;		// 目的の注視点
	D3DXVECTOR3 m_vecU;			// 上方向ベクトル
	D3DXVECTOR3 m_rot;			// 向き
	D3DXMATRIX m_mtxProjection;	// プロジェクションマトリックス
	D3DXMATRIX m_mtxView;		// ビューマトリックス
	POINT m_ClickPos;			// マウスクリックした場所
	POINT m_moveVec;			// マウスでカメラ移動するベクトル
	float m_fPerspective;		// 視野角
	float m_fHeight;			// カメラの高さ
	float m_fLength;			// カメラの距離

	bool m_bPushRight;			// 右クリックしているかどうか
	bool m_bPushLeft;			// 左クリックしているかどうか
	bool m_bCameraMove;			// チュートリアルでカメラ移動したか

	int m_nCntStop;				// プレイヤーが停止している時間
};

#endif
