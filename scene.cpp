// =================================================
// 
// シーン処理[scene.cpp]
// Author : Shu Tanaka
// 
// =================================================
#include"scene.h"
#include"title.h"
#include"game.h"
#include"result.h"
#include"ranking.h"
#include"texture.h"
#include"tutorial.h"
#include"manager.h"

// =================================================
// コンストラクタ
// =================================================
CScene::CScene(MODE mode)
{
	m_mode = mode;
}

// =================================================
// デストラクタ
// =================================================
CScene::~CScene()
{

}

// =================================================
// シーン生成処理
// =================================================
CScene* CScene::Create(MODE mode)
{
	CScene* pScene = NULL;	// ポインタ宣言

	switch (mode)
	{
	case MODE_TITLE:	// タイトル
		pScene = new CTitle;
		break;

	case MODE_TUTORIAL:	// チュートリアル
		pScene = new CTutorial;
		break;

	case MODE_GAME:	// ゲーム
		pScene = new CGame;
		break;

	case MODE_RESULT:	// リザルト
		pScene = new CResult;
		break;

	case MODE_RANKING:	// ランキング
		pScene = new CRanking;
		break;

	default:	// その他(基本入ることはない)
		break;
	}

	if (pScene != NULL)
	{
		// --- 各シーンの初期化処理 --- //
		pScene->Init();
	}

	return pScene;
}