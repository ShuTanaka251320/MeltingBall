// =================================================
// 
// ゲーム処理[game.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _GAME_H_
#define _GAME_H_

#include "scene.h"
#include "item.h"
#include "field.h"

// =================================================
// マクロ定義
#define MAX_RING	(5)		// 設置するメッシュリング数
#define MAX_FIELD	(5)		// 設置するメッシュフィールド数
#define	MODEL_FILE	"data\\Scripts\\player.txt"			// モデルを読み込むファイル名
#define	MOTION_FILE	"data\\Scripts\\motion.txt"			// モーションを読み込むファイル名
#define	STAGELOAD_FILE	"data\\Scripts\\stageLoad.txt"	// 読み込むステージエディットファイル名
#define	STAGESAVE_FILE	"data\\Scripts\\StageSave.txt"	// 保存するステージエディットファイル名
#define FIELD_SIZE		(1900)		// フィールドの全体サイズ
#define FIELD_MESHSIZE	(10)	// メッシュのサイズ
#define ROADHEAT_SIZEZ			(1900)	// ロードヒーティングのZサイズ
#define ROADHEAT_SIZEX			(200)	// ロードヒーティングのXサイズ
#define ROADHEAT_SIZE_MESHSIZE	(50)	// ロードヒーティング1つのサイズ
#define SECRETROADHEAT_SIZEZ	(150)	// 隠れロードヒーティングのZサイズ
#define SECRETROADHEAT_SIZEX	(150)	// 隠れロードヒーティングのXサイズ
#define SECRETROADHEAT_SIZE_MESHSIZE	(50)	// 隠れロードヒーティング1つのサイズ

// =================================================
// 前方宣言
class CPlayer;
class CField;
class CMeshRing;
class CMesh;
class CItem;
class CTimer;
class CScore;
class CCamera;
class CLight;
class CObject2D;
class CObject3D;
class CPause;
class CWall;
class CStageModel;

// =================================================
// クラス定義
class CGame :public CScene
{
public :
	CGame(CScene::MODE mode = CScene::MODE_GAME);
	~CGame();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//static CPlayer* GetPlayer(void) { return m_pPlayer; };
	//static CMeshField* GetMesh(void) { return m_pMesh; };
	static CTimer* GetTimer(void) { return m_pTimer; };
	//static CScore* GetScore(void) { return m_pScore; };
	//static CItem* GetItem(int nIdx) { return m_pItem[nIdx]; };

	//static CCamera* GetCamera(void) { return m_pCamera; };
	//static CLight* GetLight(void) { return m_pLight; };
	//static CGame* GetGame(void) { return m_pGame; };
	static CPause* GetPause(void) { return m_pPause; };

	static void PanishItem(int nIdx) { m_pItem[nIdx] = NULL; };

	static CPlayer* GetPlayer(void) { return m_pPlayer; };
	static CScore* GetScore(void) { return m_pScore; };
	static CCamera* GetCamera(void) { return m_pCamera; };
	static CLight* GetLight(void) { return m_pLight; };
	static CWall* GetWall(int nIdx) { return m_pWall[nIdx]; };
	static CField* GetField(int nIdx) { return m_pField[nIdx]; };
	static CMeshRing* GetMeshRing(int nIdx) { return m_pRing[nIdx]; };
	static CItem* GetItem(void) { return m_pItem; };
	static CStageModel* GetStagemodel(void) { return m_pStageModel; };

private:
	void SaveScore(void);
	void CreateStage(void);

	//static CPlayer* m_pPlayer;		// プレイヤーのポインタ
	//static CMeshField* m_pMesh;		// メッシュのポインタ
	//static CScore* m_pScore;		// スコアのポインタ
	//static CItem* m_pItem[MAX_ITEM];		// アイテムのポインタ
	static CObject2D* m_pUI;		// UIのポインタ
	//static CItemNumUI* m_pItemNumUI;	// アイテム数のUIポインタ
	//static CItemNumUI* m_pItemGetNumUI;	// アイテム取得数のUIポインタ
	//static CCamera* m_pCamera;		// カメラのポインタ
	//static CLight* m_pLight;		// ライトのポインタ
	static CGame *m_pGame;		// ゲームシーンのポインタ

	static CPause* m_pPause;		// ポーズのポインタ
	static CTimer* m_pTimer;		// タイマーのポインタ
	static CPlayer* m_pPlayer;					// プレイヤーのポインタ
	static CScore* m_pScore;					// スコアのポインタ
	static CCamera* m_pCamera;		// カメラのポインタ
	static CLight* m_pLight;		// ライトのポインタ
	static CMesh* m_pMesh;			// メッシュのポインタ
	static CWall* m_pWall[4];		// 壁のポインタ
	static CField* m_pField[CField::TYPE_MAX];		// 床のポインタ
	static CMeshRing* m_pRing[MAX_RING];		// メッシュリングのポインタ
	static CItem* m_pItem;			// アイテムのポインタ
	static CStageModel* m_pStageModel;	// ステージモデルマネージャーのポインタ

	static int m_nIdxTex;		// テクスチャのインデックス

};

#endif
