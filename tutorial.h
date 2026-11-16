// =================================================
// 
// チュートリアル処理[tutorial.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _TUTORIAL_H_
#define _TUTORIAL_H_

#include "scene.h"
#include "item.h"
#include "game.h"

// =================================================
// マクロ定義

// =================================================
// 前方宣言
class CPlayer;
class CMeshField;
class CItem;
class CTimer;
class CScore;
class CCamera;
class CLight;
class CObject2D;
class CObject3D;
class CField;
class CStageModel;

// =================================================
// クラス定義
class CTutorial :public CScene
{
public:

	// =================================================
	// 列挙型宣言
	typedef enum
	{
		TYPE_GAME = 0,	// 移動チュートリアル
		TYPE_PLAYER,	// カメラ移動チュートリアル
		TYPE_BIG,		// ジャンプチュートリアル
		TYPE_ROADHEAT,	// ハイジャンプチュートリアル
		TYPE_BURN,		// コンボチュートリアル
		TYPE_OBJECT,	// アクションチュートリアル
		TYPE_FINISH,	// チュートリアル終了
		TYPE_MAX
	}TYPE;

	typedef enum
	{
		POL_NONE = 0,	// タイプなし
		POL_CHOICE = 0,	// チュートリアル開始選択肢
		POL_INTRO,	// ゲームの導入
		POL_PLAYER,	// プレイヤーの説明
		POL_RISK,	// プレイヤーの説明
		POL_ROADHEAT,	// プレイヤーの説明
		POL_HEATTIME,	// プレイヤーの説明
		POL_PURPOSE,// ゲーム目的
		POL_CONTROLL,// ゲーム目的
		POL_HOWMOVE,// ゲーム目的
		POL_BREAK,// ゲーム目的
		POL_FINISH,	// チュートリアル終了
		POL_MAX,
	}POLYGON;

	CTutorial(CScene::MODE mode = CScene::MODE_TUTORIAL);
	~CTutorial();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CPlayer* GetPlayer(void) { return m_pPlayer; };
	static CMeshField* GetMesh(void) { return m_pMesh; };
	static CScore* GetScore(void) { return m_pScore; };
	static CItem* GetItem(int nIdx) { return m_pItem[nIdx]; };
	static CObject2D* GetUI(POLYGON nIdx) { return m_pUI[nIdx]; };

	static CCamera* GetCamera(void) { return m_pCamera; };
	static CLight* GetLight(void) { return m_pLight; };
	static CTutorial* GetTutorial(void) { return m_pTutorial; };

	static void PanishItem(int nIdx) { m_pItem[nIdx] = NULL; };

private:
	void ChoiceStart(void);
	void PlayTutorial(void);

	static CPlayer* m_pPlayer;		// プレイヤーのポインタ
	static CMeshField* m_pMesh;		// メッシュのポインタ
	static CScore* m_pScore;		// スコアのポインタ
	static CCamera* m_pCamera;		// カメラのポインタ
	static CLight* m_pLight;		// ライトのポインタ
	static CItem* m_pItem[MAX_ITEM];	// アイテムのポインタ
	static CTutorial* m_pTutorial;	// チュートリアルシーンのポインタ
	static CObject2D* m_pUI[32];	// 表示UI
	static CField* m_pField[MAX_FIELD];		// 床のポインタ
	static CStageModel* m_pStageModel;	// ステージモデルマネージャーのポインタ
	static CObject2D* m_pStartUI;	// チュートリアルスタートのポリゴン用
	static CObject2D* m_pSkipUI;	// チュートリアルスキップのポリゴン用
	static CObject2D* m_pBG;	// 選択肢背景のポリゴン用

	int m_nCntMove;		// 動いたフレーム数
	int m_nCntCamera;	// カメラ移動カウント
	float m_aAlpha[32];	// 各ポリゴンのα値
	bool m_bDisp[TYPE_MAX];		// チュートリアル表示フラグ
	int m_nIdxTex;		// テクスチャのインデックス
	bool m_bSkip;		// チュートリアルのスキップフラグ
	bool m_bStart;		// チュートリアルの開始フラグ
	int m_nSelect;		// 選択番号
	bool m_bSelect;		// チュートリアルを選択したか
};

#endif
