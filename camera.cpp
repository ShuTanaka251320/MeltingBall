// =================================================
// 
// カメラ処理[camera.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include"camera.h"
#include"player.h"
#include"main.h"
#include"manager.h"
#include"macro.h"
#include"input.h"
#include"renderer.h"
#include"debugproc.h"
#include"game.h"
#include"tutorial.h"
#include"scene.h"

// =================================================
// マクロ定義
#define CAMERA_MOVE	(3.0f)	// カメラの移動速度

// =================================================
// コンストラクタ
// =================================================
CCamera::CCamera()
{
	// --- 各メンバ変数のクリア --- //
	m_posV = VEC_NULL;
	m_posVDest = VEC_NULL;
	m_posR = VEC_NULL;
	m_posRDest = VEC_NULL;
	m_vecU = VEC_NULL;
	m_rot = VEC_NULL;
	m_ClickPos = {};
	m_moveVec = {};
	m_bPushRight = false;
	m_bPushLeft = false;
	m_fPerspective = 0.0f;
	m_fHeight = 0.0f;
	m_fLength = 0.0f;
}

// =================================================
// デストラクタ
// =================================================
CCamera::~CCamera()
{

}

// =================================================
// 初期化処理
// =================================================
HRESULT CCamera::Init(void)
{
	// --- メンバ変数の初期化 --- //
	m_posV = D3DXVECTOR3(0.0f, CAMERA_HEIGHT, CAMERA_LENGTH);
	m_posVDest = D3DXVECTOR3(0.0f, CAMERA_HEIGHT, CAMERA_LENGTH);
	m_posR = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_posRDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_ClickPos = {};
	m_moveVec = {};
	m_bPushRight = false;
	m_bPushLeft = false;
	m_nCntStop = 0;
	m_fPerspective = 45.0f;
	m_fHeight = CAMERA_HEIGHT;
	m_bCameraMove = false;
	m_fLength = CAMERA_LENGTH;

	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CCamera::Uninit(void)
{

}

// =================================================
// 更新処理
// =================================================
void CCamera::Update(void)
{
	// --- キーボードの取得 --- //
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();
	CJoyPad* pJoypad = CManager::GetJoyPad();

	if (CManager::GetMode() == CScene::MODE_TITLE)
	{// タイトル画面
		// 目的の視点を設定
		m_posVDest = D3DXVECTOR3(m_posR.x + (sinf(m_rot.y) * m_fLength), 200, m_posR.z + (cosf(m_rot.y) * m_fLength));

		// 視点の補正
		m_posV += (m_posVDest - m_posV) * 0.2f;
	}
	else if (CManager::GetMode() == CScene::MODE_TUTORIAL)
	{// チュートリアル画面
		static D3DXVECTOR3 posV = {};

		if (CTutorial::GetUI(CTutorial::POL_INTRO) != NULL)
		{// 俯瞰視点
			m_posVDest = D3DXVECTOR3(m_posR.x + (sinf(m_rot.y) * -1000), 1400, m_posR.z + (cosf(m_rot.y) * -1000));
		}
		else if (CTutorial::GetUI(CTutorial::POL_PLAYER) != NULL)
		{// プレイヤー注視点
			m_posVDest = D3DXVECTOR3(m_posR.x + (sinf(m_rot.y) * -200.0f), 200.0f, m_posR.z + (cosf(m_rot.y) * -200.0f));
		}
		else if (CTutorial::GetUI(CTutorial::POL_HEATTIME) != NULL)
		{
			m_posRDest = D3DXVECTOR3(800.0f, 10.0f, 0.0f);
			m_posVDest = D3DXVECTOR3(m_posR.x + (sinf(m_rot.y) * -400.0f), 300.0f, m_posR.z + (cosf(m_rot.y) * -400.0f));
		}
		else if (CTutorial::GetUI(CTutorial::POL_ROADHEAT) != NULL)
		{// ロードヒーティング注視点
			m_posRDest = D3DXVECTOR3(800.0f, 10.0f, 0.0f);
			m_posVDest = D3DXVECTOR3(m_posR.x + (sinf(m_rot.y) * -400.0f), 300.0f, m_posR.z + (cosf(m_rot.y) * -400.0f));
		}
		else if (CTutorial::GetUI(CTutorial::POL_PURPOSE) != NULL)
		{
			m_posRDest = VEC_NULL;
			m_posVDest = D3DXVECTOR3(m_posR.x + (sinf(m_rot.y) * -200.0f), 200.0f, m_posR.z + (cosf(m_rot.y) * -200.0f));
		}
		else if (CTutorial::GetUI(CTutorial::POL_CONTROLL) != NULL)
		{
			m_posRDest = VEC_NULL;
			m_posVDest = D3DXVECTOR3(m_posR.x + (sinf(m_rot.y) * -200.0f), 200.0f, m_posR.z + (cosf(m_rot.y) * -200.0f));
		}
		else if (CTutorial::GetUI(CTutorial::POL_HOWMOVE) != NULL)
		{// プレイヤー注視点
			m_posRDest = VEC_NULL;
			m_posVDest = D3DXVECTOR3(m_posR.x + (sinf(m_rot.y) * -200.0f), 200.0f, m_posR.z + (cosf(m_rot.y) * -200.0f));
		}
		else if (CTutorial::GetUI(CTutorial::POL_FINISH) != NULL)
		{// 俯瞰してん
			m_posVDest = D3DXVECTOR3(m_posR.x + (sinf(m_rot.y) * -1000), 1400, m_posR.z + (cosf(m_rot.y) * -1000));
		}

		m_posR += (m_posRDest - m_posR) * 0.1f;
		m_posV += (m_posVDest - m_posV) * 0.1f;
	}
	else if (CManager::GetMode() == CScene::MODE_GAME)
	{// ゲーム画面
		// --- プレイヤーの取得 --- //
		CPlayer* pPlayer = CGame::GetPlayer();
		D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();
		D3DXVECTOR3 PlayerRot = pPlayer->GetRotation();
		D3DXVECTOR3 PlayerMove = pPlayer->GetMove();

		// --- 注視点の取得 --- //
		D3DXVECTOR3 CamPos = pPlayer->GetCamPosition();

		if (CManager::GetPause() == false)
		{
			m_posRDest = PlayerPos;

			// 目的の視点を設定
			m_posVDest = D3DXVECTOR3(m_posR.x + (sinf(m_rot.y) * m_fLength), CAMERA_HEIGHT, m_posR.z + (cosf(m_rot.y) * m_fLength));

			// 視点の補正
			m_posV += (m_posVDest - m_posV) * 0.2f;

			// 注視点の補正
			m_posR += (m_posRDest - m_posR) * 0.2f;

			D3DXVECTOR2 Vec = {};

			// ***** カメラの回転処理 ***** //
			// --- 注視点が中心 --- //
			if (pInputKeyboard->GetPress(DIK_D) == true)
			{
				m_rot.y += 0.02f;

				m_posV.x = m_posR.x + (sinf(m_rot.y) * m_fLength);
				m_posV.z = m_posR.z + (cosf(m_rot.y) * m_fLength);
			}
			else if (pInputKeyboard->GetPress(DIK_A) == true)
			{
				m_rot.y -= 0.02f;

				m_posV.x = m_posR.x + sinf(m_rot.y) * m_fLength;
				m_posV.z = m_posR.z + cosf(m_rot.y) * m_fLength;
			}

			if (pJoypad->GetStickDir(CJoyPad::STICK_TYPE_LEFT, &Vec) == true)
			{
				m_rot.y += Vec.x * 0.02f;

				m_posV.x = m_posR.x + (sinf(m_rot.y) * m_fLength);
				m_posV.z = m_posR.z + (cosf(m_rot.y) * m_fLength);
			}

			if (m_rot.y >= D3DX_PI * 2)
			{
				m_rot.y -= D3DX_PI * 2;
			}
			else if (m_rot.y <= -D3DX_PI * 2)
			{
				m_rot.y += D3DX_PI * 2;
			}
		}
		else
		{
			//if (pInputKeyboard->GetPress(DIK_D) == true)
			//{
			//	m_rot.y += 0.02f;

			//	m_posV.x = m_posR.x + (sinf(m_rot.y) * m_fLength);
			//	m_posV.z = m_posR.z + (cosf(m_rot.y) * m_fLength);
			//}
			//else if (pInputKeyboard->GetPress(DIK_A) == true)
			//{
			//	m_rot.y -= 0.02f;

			//	m_posV.x = m_posR.x + sinf(m_rot.y) * m_fLength;
			//	m_posV.z = m_posR.z + cosf(m_rot.y) * m_fLength;
			//}

			//if (pInputKeyboard->GetPress(DIK_UP) == true)
			//{
			//	m_posV.z += 2.0f;
			//}
			//if (pInputKeyboard->GetPress(DIK_DOWN) == true)
			//{
			//	m_posV.z += -2.0f;

			//}
			//if (pInputKeyboard->GetPress(DIK_LEFT) == true)
			//{
			//	m_posV.x += -2.0f;
			//}
			//if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
			//{
			//	m_posV.x += 2.0f;
			//}
			//if (pInputKeyboard->GetPress(DIK_U) == true)
			//{
			//	m_posV.y += -2.0f;
			//}
			//if (pInputKeyboard->GetPress(DIK_M) == true)
			//{
			//	m_posV.y += 2.0f;
			//}
			//if (pInputKeyboard->GetPress(DIK_I) == true)
			//{
			//	m_posR.y += -2.0f;
			//}
			//if (pInputKeyboard->GetPress(DIK_K) == true)
			//{
			//	m_posR.y += 2.0f;
			//}
			//if (pInputKeyboard->GetPress(DIK_T) == true)
			//{
			//	m_posR.x += -2.0f;
			//}
			//if (pInputKeyboard->GetPress(DIK_B) == true)
			//{
			//	m_posR.x += 2.0f;
			//}
			//if (pInputKeyboard->GetPress(DIK_Y) == true)
			//{
			//	m_posR.z += -2.0f;
			//}
			//if (pInputKeyboard->GetPress(DIK_H) == true)
			//{
			//	m_fLength += -2.0f;

			//}
			//if (pInputKeyboard->GetPress(DIK_J) == true)
			//{
			//	m_fLength += 2.0f;
			//}
			//if (pInputKeyboard->GetPress(DIK_N) == true)
			//{
			//	m_posR.z += 2.0f;
			//}
		}
	}
	else if (CManager::GetMode() == CScene::MODE_RESULT)
	{// リザルト画面
		m_rot = VEC_NULL;

		m_posR = VEC_NULL;
		m_posVDest = D3DXVECTOR3(m_posR.x + (sinf(m_rot.y) * -1200), 2200, m_posR.z + (cosf(m_rot.y) * -1200));

		m_posV += (m_posVDest - m_posV) * 0.02f;
	}
}

// =================================================
// セット処理
// =================================================
void CCamera::SetCamera(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();	// レンダラーのポインタ

	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// Direct3Dデバイスへのポインタ

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	// プロジェクションマトリックスを作成(透視投影)(奥行き)
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
		D3DXToRadian(m_fPerspective),
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
		10.0f,
		10000.0f);

	// --- プロジェクションマトリックスを作成(平行投影)(2Dっぽく) --- //
	//D3DXMatrixOrthoLH(&m_mtxProjection,
	//	(float)SCREEN_WIDTH,
	//	(float)SCREEN_HEIGHT,
	//	10.0f,
	//	10000.0f);

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&m_mtxView,
		&m_posV,
		&m_posR,
		&m_vecU);

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);
}

// =================================================
// 視野角の変更
// =================================================
void CCamera::SetRotate(D3DXVECTOR3 rot, float fLength)
{
	D3DXVECTOR3 rotDest;
	D3DXVECTOR3 rotDiff;
	// --- モデルの向きの設定 --- //
	rotDest.y = rot.y;	// 目標の角度を設定
	rotDiff.y = rotDest.y - m_rot.y;			// 現在と目標の角度の差分を算出

	// もし、差分がπを超えたら
	if (rotDiff.y > D3DX_PI)
	{
		rotDiff.y -= D3DX_PI * 2;
	}
	else if (rotDiff.y < -D3DX_PI)
	{
		rotDiff.y += D3DX_PI * 2;
	}

	// 回転に補正を掛ける
	m_rot.y += rotDiff.y * 0.2f;

	// もし、現在の角度がπを超えたら
	if (m_rot.y > D3DX_PI)
	{
		m_rot.y -= D3DX_PI * 2;
	}
	else if (m_rot.y < -D3DX_PI)
	{
		m_rot.y += D3DX_PI * 2;
	}
}

// =================================================
// 視野角の変更
// =================================================
void CCamera::SetPerspective(float fPer, float fIni)
{
	float fDiffPer;		// 視野角の差分
	float fDestPer;		// 目標視野角

	fDestPer = fPer;	// 目標の設定
	fDiffPer = fDestPer - m_fPerspective;	// 現在との差分を算出

	m_fPerspective += fDiffPer * fIni;	// 慣性をかけて目標の視野角に設定
}

