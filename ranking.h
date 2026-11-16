// =================================================
// 
// ランキング処理[ranking.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _RANKING_H_
#define _RANKING_H_

#include "scene.h"

// =================================================
// マクロ定義
#define MAX_RANKING		(5)		// スコアをランクインさせる最大数

// =================================================
// 前方宣言
class CObject2D;
class CObject3D;
class CCamera;
class CLight;
class CMesh;
class CScore;

// =================================================
// クラス定義
class CRanking :public CScene
{
public:
	CRanking(CScene::MODE mode = CScene::MODE_RANKING);
	~CRanking();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CScore* GetRankScore(int nIdx) { return m_pScore[nIdx]; };
	static CCamera* GetCamera(void) { return m_pCamera; };

private:
	void CreateUI(void);
	void LoadRanking(void);

	static CScore* m_pScore[MAX_RANKING];		// スコアのポインタ

	static CObject2D* m_pBg;	// 背景のポリゴン用
	static CObject2D* m_pUI;	// UIのポリゴン用
	static CCamera* m_pCamera;	// カメラのポインタ
	static int m_nIdxTex;		// テクスチャのインデックス
	static int m_nCntFade;		// 自動で画面遷移するまでのカウンター
	static int m_nUpdateRank;			// 変更があった順位
};

#endif
