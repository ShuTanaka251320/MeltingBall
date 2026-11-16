// =================================================
// 
// 一時停止処理[pause.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include "pause.h"
#include "input.h"
#include "fade.h"
#include "game.h"
#include "sound.h"
#include "texture.h"
#include "macro.h"
#include "manager.h"

// =================================================
// メンバ変数宣言
CPause::PAUSE CPause::m_state = PAUSE_OFF;	// ポーズ状態
int CPause::m_nSelect = 0;		// 選択肢している番号
CPause::TYPE CPause::m_type[TYPE_MAX] = {};	// ポーズ選択肢の種類
CObject2D* CPause::m_pBg = NULL;			// ポーズ画面中に表示する背景
CObject2D* CPause::m_pUI[TYPE_MAX] = {};	// 選択肢

// =================================================
// コンストラクタ
// =================================================
CPause::CPause()
{

}

// =================================================
// デストラクタ
// =================================================
CPause::~CPause()
{

}

// =================================================
// 一時停止の初期化処理
// =================================================
void CPause::Init(void)
{
	
}

// =================================================
// 背景の終了処理
// =================================================
void CPause::Uninit(void)
{
	m_state = PAUSE_OFF;	// ポーズ状態
	m_nSelect = 0;		// 選択肢している番号
	memset(m_type, 0, sizeof(m_type));	// ポーズ選択肢の種類
	m_pBg = NULL;			// ポーズ画面中に表示する背景
	memset(m_pUI, 0, sizeof(m_pUI));	// 選択肢
}

// =================================================
// 一時停止の更新処理
// =================================================
void CPause::Update(void)
{
	CTexture* pTexture = CManager::GetTexture();	// テクスチャのポインタ

	if (CManager::GetPause() == true)
	{// ポーズ中

		if (m_pBg == NULL && m_pUI[TYPE_CONTINUE] == NULL && m_pUI[TYPE_RETRY] == NULL && m_pUI[TYPE_QUIT] == NULL)
		{
			m_pBg = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f),
				VEC_NULL,
				VEC_NULL,
				COLOR_HALFBLACK,
				SCREEN_WIDTH,
				SCREEN_HEIGHT,
				CObject::TYPE_BG,
				8);

			// --- テクスチャ設定 --- //
			int nIdxTex = -1;
			m_pBg->SetTexture(nIdxTex);

			// --- UIポリゴン --- //
			m_pUI[TYPE_CONTINUE] = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 200.0f, 0.0f),
				VEC_NULL,
				VEC_NULL,
				COLOR_WHITE,
				500.0f,
				350.0f,
				CObject::TYPE_BG,
				8);

			// --- テクスチャ設定 --- //
			nIdxTex = pTexture->Register("data\\TEXTURE\\continue.png");
			m_pUI[TYPE_CONTINUE]->SetTexture(nIdxTex);

			// --- UIポリゴン --- //
			m_pUI[TYPE_RETRY] = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f),
				VEC_NULL,
				VEC_NULL,
				COLOR_WHITE,
				500.0f,
				350.0f,
				CObject::TYPE_BG,
				8);

			// --- テクスチャ設定 --- //
			nIdxTex = pTexture->Register("data\\TEXTURE\\restart.png");
			m_pUI[TYPE_RETRY]->SetTexture(nIdxTex);

			// --- UIポリゴン --- //
			m_pUI[TYPE_QUIT] = CObject2D::Create(D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 200.0f, 0.0f),
				VEC_NULL,
				VEC_NULL,
				COLOR_WHITE,
				500.0f,
				350.0f,
				CObject::TYPE_BG,
				8);

			// --- テクスチャ設定 --- //
			nIdxTex = pTexture->Register("data\\TEXTURE\\quit.png");
			m_pUI[TYPE_QUIT]->SetTexture(nIdxTex);
		}


		CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();	// キーボードの取得
		CJoyPad* pJoyPad = CManager::GetJoyPad();	// ジョイパッドの取得

		m_pBg->SetCol(COLOR_HALFBLACK);

		if (pInputKeyboard->GetTrigger(DIK_S) == true || pJoyPad->GetTrigger(CJoyPad::JOYKEY_DOWN) == true)
		{// 下方向への入力
			m_nSelect++;

			if (m_nSelect == 3)
			{
				m_nSelect = TYPE_CONTINUE;
			}
		}
		else if (pInputKeyboard->GetTrigger(DIK_W) == true || pJoyPad->GetTrigger(CJoyPad::JOYKEY_UP) == true)
		{	// 上方向への入力
			m_nSelect--;

			if (m_nSelect == -1)
			{
				m_nSelect = TYPE_QUIT;
			}
		}

		if (m_nSelect == TYPE_CONTINUE)
		{// CONTINUEが選択中
			// 頂点カラーの設定
			m_pUI[TYPE_CONTINUE]->SetCol(COLOR_WHITE);
			m_pUI[TYPE_RETRY]->SetCol(COLOR_HALFWHITE);
			m_pUI[TYPE_QUIT]->SetCol(COLOR_HALFWHITE);
		}
		else if (m_nSelect == TYPE_RETRY)
		{// RETRYが選択中
			// 頂点カラーの設定
			m_pUI[TYPE_CONTINUE]->SetCol(COLOR_HALFWHITE);
			m_pUI[TYPE_RETRY]->SetCol(COLOR_WHITE);
			m_pUI[TYPE_QUIT]->SetCol(COLOR_HALFWHITE);
		}
		else if (m_nSelect == TYPE_QUIT)
		{// QUITが選択中
			// 頂点カラーの設定
			m_pUI[TYPE_CONTINUE]->SetCol(COLOR_HALFWHITE);
			m_pUI[TYPE_RETRY]->SetCol(COLOR_HALFWHITE);
			m_pUI[TYPE_QUIT]->SetCol(COLOR_WHITE);
		}

		if (pInputKeyboard->GetTrigger(DIK_RETURN) == true || pJoyPad->GetTrigger(CJoyPad::JOYKEY_A) == true)
		{
			CManager::SetPause(false);

			if (m_nSelect == TYPE_CONTINUE)
			{// CONTINUEが選択中
				m_nSelect = 0;
				m_state = PAUSE_OFF;
			}
			else if (m_nSelect == TYPE_RETRY)
			{// RETRYが選択中
				CManager::SetStopSound(true);	// BGM終了フラグを立てる

				m_nSelect = 0;
				m_state = PAUSE_OFF;
				CFade::SetFade(CScene::MODE_GAME);
			}
			else if (m_nSelect == TYPE_QUIT)
			{// QUITが選択中
				CManager::SetStopSound(true);	// BGM終了フラグを立てる

				m_nSelect = 0;
				m_state = PAUSE_OFF;
				CFade::SetFade(CScene::MODE_TITLE);
			}
		}
	}
	else if (CManager::GetPause() == false)
	{
		if (m_pBg != NULL && m_pUI[TYPE_CONTINUE] != NULL && m_pUI[TYPE_RETRY] != NULL && m_pUI[TYPE_QUIT] != NULL)
		{
			m_pBg->Uninit();
			m_pBg = NULL
				;
			for (int nCnt = 0; nCnt < TYPE_MAX; nCnt++)
			{

				m_pUI[nCnt]->Uninit();
				m_pUI[nCnt] = NULL;
			}
		}

		InitChoice();	// 選択番号の初期化
	}
}

// =================================================
// 一時停止の描画処理
// =================================================
void CPause::Draw(void)
{

}

// =================================================
// ポリゴン色の初期化
// =================================================
void CPause::InitColor(void)
{

}

// =================================================
// 選択肢の初期化
// =================================================
void CPause::InitChoice(void)
{
	m_nSelect = 0;
}
