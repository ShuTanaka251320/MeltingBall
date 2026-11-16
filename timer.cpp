// =================================================
// 
// タイマー処理[timer.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include"timer.h"
#include"renderer.h"
#include"manager.h"
#include"texture.h"
#include"macro.h"

// =================================================
// マクロ定義
#define INIT_TIMER		(180)		// タイマーの初期値
#define TIMER_WIDTH		(30.0f)		// 数字ポリゴンの横幅
#define TIMER_HEIGHT	(60.0f)		// 数字ポリゴンの縦幅

// =================================================
// コンストラクタ
// =================================================
CTimer::CTimer(int nPriority) :CObject(nPriority)
{
	// --- 書くメンバ変数のクリア --- //
	memset(m_apNumber, 0, sizeof(m_apNumber));	// 数字のポインタ
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 向き
	m_fLength = 0.0f;	// 対角線の長さ
	m_fAngle = 0.0f;	// 対角線の角度
	m_fWidth = 0.0f;	// 横幅
	m_fHeight = 0.0f;	// 縦幅
	m_nTimer = 0;		// 獲得スコア
	m_dwExecLastTime = 0;	// 時間を減らす用
	m_bTimer = false;		// タイマー作動フラグ
	m_bGameOver = false;	// ゲームオーバーフラグ
}

// =================================================
// デストラクタ
// =================================================
CTimer::~CTimer()
{

}

// =================================================
// 初期化処理
// =================================================
HRESULT CTimer::Init(void)
{
	return S_OK;
}

// =================================================
// 初期化処理(オーバーロード)
// =================================================
HRESULT CTimer::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, int nTime,  float fWidth, float fHeight, const char* pFileName)
{
	for (int nCnt = 0; nCnt < MAX_TIMER_DIG; nCnt++) {
		// --- 数字のポリゴン生成 --- //
		m_apNumber[nCnt] = CNumber::Create(pos, VEC_NULL, VEC_NULL, col, fWidth, fHeight);
		m_pos = pos;
		m_rot = rot;
		m_fWidth = fWidth;
		m_fHeight = fHeight;
		m_apNumber[nCnt]->SetNumber(0);

		if (pFileName != NULL)
		{
			CTexture* pTexture = CManager::GetTexture();
			int nIdx = pTexture->Register(pFileName);
			m_apNumber[nCnt]->SetTexture(nIdx);
		}
		else
		{
			m_apNumber[nCnt]->SetTexture(-1);
		}
	}

	m_bTimer = true;	// タイマーフラグ
	m_nTimer = nTime;	// 初期制限時間
	m_dwExecLastTime = 0;	// 時間を計測

	return S_OK;
}

// =================================================
// 終了処理
// =================================================
void CTimer::Uninit()
{
	for (int nCnt = 0; nCnt < MAX_TIMER_DIG; nCnt++) {
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
void CTimer::Update()
{
	for (int nCnt = 0; nCnt < MAX_TIMER_DIG; nCnt++) {
		if (m_apNumber[nCnt] != NULL)
		{// NULLチェック
			// --- 位置の変更 --- //
			m_apNumber[nCnt]->CNumber::SetPosition(D3DXVECTOR3(m_pos.x + (nCnt * m_fWidth), m_pos.y, m_pos.z));

			// --- 更新処理 --- //
			m_apNumber[nCnt]->Update();
		}
	}

	if (m_bTimer == true && m_nTimer > 0)
	{// タイマーフラグが立っているとき、残り時間があるとき減少
		DecreaseTimer(1);
	}

	if (m_nTimer <= 30 && m_nTimer > 15)
	{// 30秒以下でポリゴンの色変更
		for (int nCnt = 0; nCnt < MAX_TIMER_DIG; nCnt++) {
			m_apNumber[nCnt]->SetCol(COLOR_YELLOW);
		}
	}
	else if (m_nTimer <= 15)
	{// 15秒以下
		for (int nCnt = 0; nCnt < MAX_TIMER_DIG; nCnt++) {
			m_apNumber[nCnt]->SetCol(COLOR_RED);
		}
	}

	if (m_nTimer <= 0)
	{// 時間切れ
		m_bGameOver = true;
	}
}

// =================================================
// 描画処理
// =================================================
void CTimer::Draw()
{
	for (int nCnt = 0; nCnt < MAX_TIMER_DIG; nCnt++) {
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
CTimer* CTimer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXCOLOR col, int nTime, float fWidth, float fHeight, const char* pFileName)
{
	CTimer* pTimer;	// ポインタ宣言

	// --- オブジェクトの生成 --- //
	pTimer = new CTimer;

	// --- 初期化 --- //
	pTimer->Init(pos, rot, move, col, nTime, fWidth, fHeight, pFileName);

	return pTimer;
}

// =================================================
// スコア加算
// =================================================
void CTimer::DecreaseTimer(const int nTimer)
{
	int aTex[MAX_TIMER_DIG];

	DWORD dwCurrentTime = timeGetTime();		// 現在時刻を取得

	if ((dwCurrentTime - m_dwExecLastTime) >= 1000)
	{// 1秒経過
		// 1秒毎にタイマーを1減らす
		m_nTimer--;
		m_dwExecLastTime = dwCurrentTime;		// 処理開始時刻を取得
	}

	for (int nCnt = 0; nCnt < MAX_TIMER_DIG; nCnt++)
	{
		if (nCnt == 2)
		{
			aTex[nCnt] = m_nTimer % (int)pow(10, (MAX_TIMER_DIG - nCnt));
		}
		else
		{
			aTex[nCnt] = m_nTimer % (int)pow(10, (MAX_TIMER_DIG - nCnt)) / ((int)pow(10, ((MAX_TIMER_DIG - 1) - nCnt)));
		}

		// --- テクスチャ設定 --- //
		m_apNumber[nCnt]->SetNumber(aTex[nCnt]);
	}
}