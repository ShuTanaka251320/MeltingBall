// =================================================
// 
// タイトル処理[title.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _TITLE_H_
#define _TITLE_H_

#include "scene.h"

// =================================================
// 前方宣言
class CObject2D;
class CObject3D;
class CObjectX;
class CCamera;
class CLight;
class CMeshField;

// =================================================
// クラス定義
class CTitle :public CScene
{
public:
	CTitle(CScene::MODE mode = CScene::MODE_TITLE);
	~CTitle();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CCamera* GetCamera(void) { return m_pCamera; };
	static CLight* GetLight(void) { return m_pLight; };
	static CMeshField* GetMesh(void) { return m_pMesh; };

private:
	void CreateStage(void);
	void CreateUI(void);
	void Select(void);

	static CCamera* m_pCamera;		// カメラのポインタ
	static CLight* m_pLight;		// ライトのポインタ
	static CMeshField* m_pMesh;			// メッシュのポインタ

	static CObject2D* m_pRogo;	// ゲームタイトル用
	static CObject2D* m_pInfo;	// pressstartのポリゴン用
	static CObject2D* m_pStartUI;	// ゲームスタートのポリゴン用
	static CObject2D* m_pQuitUI;	// ゲーム終了のポリゴン用
	static CObject2D* m_pBG;	// 選択肢背景のポリゴン用
	static CObject3D* m_pSide;	// 限界値のビルのポリゴン用
	static CObjectX* m_pPlayer;	// プレイヤーのポインタ
	static int m_nIdxTex;		// テクスチャのインデックス
	static int m_nCntFade;		// 自動で画面遷移するまでのカウンター

	static int m_nSelect;	// 選択番号
};

#endif