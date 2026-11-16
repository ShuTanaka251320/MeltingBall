// =================================================
// 
// リザルト処理[result.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _RESULT_H_
#define _RESULT_H_

#include "scene.h"
#include "game.h"
#include "field.h"

// =================================================
// 前方宣言
class CObject2D;
class CObject3D;
class CCamera;
class CLight;
class CMesh;
class CScore;
class CField;

// =================================================
// クラス定義
class CResult :public CScene
{
public:
	CResult(CScene::MODE mode = CScene::MODE_RESULT);
	~CResult();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CCamera* GetCamera(void) { return m_pCamera; };
	static CLight* GetLight(void) { return m_pLight; };
	static CMesh* GetMesh(void) { return m_pMesh; };
	static CScore* GetScore(void) { return m_pScore; };

private:
	void UpdateRanking(void);

	static CCamera* m_pCamera;		// カメラのポインタ
	static CLight* m_pLight;		// ライトのポインタ
	static CMesh* m_pMesh;			// メッシュのポインタ
	static CScore* m_pScore;		// スコアのポインタ
	static CField* m_pField[CField::TYPE_MAX];		// 床のポインタ

	static CObject2D* m_pBg;	// 背景のポリゴン用
	static CObject2D* m_pUI;	// UIのポリゴン用
	static CObject3D* m_pSide;	// 限界値のビルのポリゴン用
	static int m_nIdxTex;		// テクスチャのインデックス
	static int m_nCntFade;		// 自動で画面遷移するまでのカウンター
	static int m_nScore;		// 獲得したスコア
	static int m_nRank;			// 順位
};

#endif
