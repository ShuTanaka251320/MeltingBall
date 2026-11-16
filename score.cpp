// =================================================
// 
// スコア処理[score.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include"score.h"
#include"renderer.h"
#include"manager.h"
#include"macro.h"
#include"timer.h"
#include"game.h"
#include"texture.h"
#include"player.h"
#include"field.h"
#include"meshfield.h"

// =================================================
// マクロ定義
#define MAX_SCORE		(1000)	// スコアのカンスト値
#define SCORE_WIDTH		(30.0f)		// 数字ポリゴンの横幅
#define SCORE_HEIGHT	(60.0f)		// 数字ポリゴンの縦幅

// =================================================
// コンストラクタ
// =================================================
CScore::CScore(int nPrioryty) :CObject(nPrioryty)
{
	// --- 書くメンバ変数のクリア --- //
	memset(&m_apNumber[MAX_DIG], 0, sizeof(m_apNumber));	// 数字のポインタ
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 向き
	m_fLength = 0.0f;	// 対角線の長さ
	m_fAngle = 0.0f;	// 対角線の角度
	m_fWidth = 0.0f;	// 横幅
	m_fHeight = 0.0f;	// 縦幅
	m_nScore = 0;		// 獲得スコア
	m_nNumDig = 0;		// 桁数
	m_nSumScore = 0;	// 合計スコア
}

// =================================================
// デストラクタ
// =================================================
CScore::~CScore()
{

}

// =================================================
// 初期化処理
// =================================================
HRESULT CScore::Init(void)
{
	return S_OK;
}

// =================================================
// 初期化処理(オーバーロード)
// =================================================
HRESULT CScore::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, int nNumDig, const char* pFileName)
{
	for (int nCnt = 0; nCnt < nNumDig; nCnt++) {
		// --- 数字のポリゴン生成 --- //
		m_apNumber[nCnt] = CNumber::Create(pos, VEC_NULL, VEC_NULL, col, fWidth, fHeight);
		m_pos = pos;			// 位置
		m_rot = rot;			// 向き
		m_fWidth = fWidth;		// 横幅
		m_fHeight = fHeight;	// 縦幅
		m_nNumDig = nNumDig;	// 桁数

		if (m_apNumber[nCnt] != NULL)
		{// NULLチェック
			m_apNumber[nCnt]->SetNumber(0);	// 初期はすべて0に設定
		}

		// --- テクスチャの設定 --- //
		if (pFileName != NULL)
		{
			CTexture* pTexture = CManager::GetTexture();	// テクスチャの情報取得
			m_nIdxTex = pTexture->Register(pFileName);
			m_apNumber[nCnt]->SetTexture(m_nIdxTex);
		}
		else
		{
			m_nIdxTex = -1;
			m_apNumber[nCnt]->SetTexture(m_nIdxTex);
		}
	}

	m_nNumDig = nNumDig;	// 桁数を保存

	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CScore::Uninit()
{
	for (int nCnt = 0; nCnt < m_nNumDig; nCnt++) {
		if (m_apNumber[nCnt] != NULL) {
			// --- 終了処理 --- //
			m_apNumber[nCnt]->Uninit();
		}
	}

	// --- 自身の破棄 --- //
	Release();
}

// =================================================
// 更新処理
// =================================================
void CScore::Update()
{
	for (int nCnt = 0; nCnt < m_nNumDig; nCnt++) {
		if (m_apNumber[nCnt] != NULL)
		{// NULLチェック
			// --- 位置の変更 --- //
			m_apNumber[nCnt]->CNumber::SetPosition(D3DXVECTOR3(m_pos.x + (nCnt * m_fWidth), m_pos.y, m_pos.z));

			// --- 更新処理 --- //
			m_apNumber[nCnt]->Update();
		}
	}
}

// =================================================
// 描画処理
// =================================================
void CScore::Draw()
{
	for (int nCnt = 0; nCnt < m_nNumDig; nCnt++) {
		if (m_apNumber[nCnt] != NULL)
		{// NULLチェック
			// --- 描画処理 --- //
			m_apNumber[nCnt]->Draw();
		}
	}
}

// =================================================
// スコア生成
// =================================================
CScore* CScore::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, float fWidth, float fHeight, int nNumDig, const char* pFileName)
{
	CScore* pScore;	// ポインタ宣言

	// --- オブジェクトの生成 --- //
	pScore = new CScore;

	// --- 初期化 --- //
	pScore->Init(pos, rot, move, col, fWidth, fHeight, nNumDig, pFileName);

	return pScore;
}

// =================================================
// スコア加算
// =================================================
void CScore::AddScore(const int nScore)
{
	CPlayer* pPlayer = CGame::GetPlayer();

	// --- スコア加算 --- //
	m_nSumScore += nScore;

	// --- スコアカンストで固定値に --- //
	if (m_nSumScore > MAX_SCORE) {
		//m_nSumScore = MAX_SCORE;
	}

	int aTex[MAX_DIG];	// テクスチャの数字

	// --- テクスチャの設定 --- //
	for (int nCnt = 0; nCnt < m_nNumDig; nCnt++)
	{
		if (nCnt == m_nNumDig - 1) {

			aTex[nCnt] = m_nSumScore % (int)pow(10, (m_nNumDig - nCnt));
		}
		else {
			aTex[nCnt] = m_nSumScore % (int)pow(10, (m_nNumDig - nCnt)) / ((int)pow(10, ((m_nNumDig - 1) - nCnt)));
		}

		// --- テクスチャの反映 --- //
		m_apNumber[nCnt]->SetNumber(aTex[nCnt]);
	}
}

// =================================================
// スコア加算
// =================================================
void CScore::AddScore(void)
{
	CField* pField = CGame::GetField(CField::TYPE_SNOW);
	// --- 割合取得 --- //
	m_nSumScore = pField->GetPersentHeight();	// へこんでいる割合を加算

	// --- スコアカンストで固定値に --- //
	if (m_nSumScore > MAX_SCORE) {
		m_nSumScore = MAX_SCORE;
	}

	int aTex[MAX_DIG];	// テクスチャの数字

	// --- テクスチャの設定 --- //
	for (int nCnt = 0; nCnt < m_nNumDig; nCnt++)
	{
		if (nCnt == m_nNumDig - 1) {
			aTex[nCnt] = m_nSumScore % (int)pow(10, (m_nNumDig - nCnt));
		}
		else {
			aTex[nCnt] = m_nSumScore % (int)pow(10, (m_nNumDig - nCnt)) / ((int)pow(10, ((m_nNumDig - 1) - nCnt)));
		}

		// --- テクスチャの反映 --- //
		m_apNumber[nCnt]->SetNumber(aTex[nCnt]);
	}
}

// =================================================
// ポリゴンの色の変更
// =================================================
void CScore::SetCol(D3DXCOLOR col)
{
	for (int nCnt = 0; nCnt < MAX_DIG; nCnt++)
	{
		m_apNumber[nCnt]->SetCol(col);
	}
}

// =================================================
// ポリゴンの点滅処理
// =================================================
void CScore::FlashPol(int nCntFlash)
{
	for (int nCnt = 0; nCnt < MAX_DIG; nCnt++)
	{
		m_apNumber[nCnt]->FlashPol(nCntFlash);
	}
}
