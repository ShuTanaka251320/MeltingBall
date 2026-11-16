// =================================================
// 
// シーン処理[scene.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _SCENE_H_
#define _SCENE_H_

#include"main.h"

// =================================================
// クラス定義
class CScene
{
public:
	typedef enum
	{
		MODE_TITLE = 0,	// タイトル
		MODE_TUTORIAL,	// チュートリアル
		MODE_GAME,		// ゲーム
		MODE_RESULT,	// リザルト(ゲームクリア)
		MODE_RANKING,	// ランキング
		MODE_MAX,
	}MODE;
	
	CScene(MODE mode);
	virtual ~CScene();

	static CScene* Create(MODE mode);
	MODE GetMode(void) { return m_mode; };

	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

private:
	MODE m_mode;	// 現在のモード
};

#endif