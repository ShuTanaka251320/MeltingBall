// =================================================
// 
// マネージャー処理[manager.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _MANAGER_H_
#define _MANAGER_H_

#include"main.h"
#include"object.h"
#include"scene.h"

// =================================================
// 前方宣言
class CSound;
class CInputKeyboard;
class CTexture;
class CInputMouse;
class CMotion;
class CRenderer;
class CJoyPad;
class CDebugProc;
class CFade;
class CScene;
class CXModel;

// =================================================
// クラス定義
class CManager
{
public:
	CManager();
	~CManager();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static void SetMode(CScene::MODE mode);
	static CScene::MODE GetMode(void);

	static CRenderer* GetRenderer(void) { return m_pRenderer; };
	static CInputKeyboard* GetInputKeyboard(void) { return m_pInputKeyboard; };
	static CInputMouse* GetInputMouse(void) { return m_pInputMouse; };
	static CJoyPad* GetJoyPad(void) { return m_pJoyPad; };
	static CDebugProc* GetDrbugProc(void) { return m_pDebugProc; };

	static CSound* GetSound(void) { return m_pSound; };

	static CXModel* GetXModel(void) { return m_pXModel;; };
	static CTexture* GetTexture(void) { return m_pTexture; };
	static CScene* GetScene(void) { return m_pScene; };
	static CFade* GetFade(void) { return m_pFade; };

	static bool GetDrawParticle(void) { return m_bDrawParticle; };
	static bool GetTutorial(void) { return m_bTutorial; };
	static bool GetFinish(void) { return m_bFinish; };
	static bool GetPause(void) { return m_bPause; };
	static void SetPause(bool bPause) { m_bPause = bPause; };
	static void SetStopSound(bool bStop) { m_bStopSound = bStop; };

private:
	static CRenderer* m_pRenderer;				// レンダラーオブジェクトのポインタ
	static CInputKeyboard* m_pInputKeyboard;	// キーボードオブジェクトのポインタ
	static CInputMouse* m_pInputMouse;			// マウスオブジェクトのポインタ
	static CJoyPad* m_pJoyPad;					// ジョイパッドオブジェクトのポインタ
	static CDebugProc* m_pDebugProc;			// デバッグ表示のポインタ
	static CSound* m_pSound;					// サウンドのポインタ

	static CXModel* m_pXModel;		// モデルマネージャのポインタ
	static CTexture* m_pTexture;	// テクスチャのポインタ
	static CScene* m_pScene;		// シーンのポインタ
	static CFade* m_pFade;			// フェードのポインタ

	static int m_nRank;		// 順位

	static bool m_bPause;		// ポーズ判定
	static bool m_bFinish;		// ゲームが終了
	static bool m_bTutorial;	// チュートリアル終了フラグ
	static bool m_bResult;		// リザルト終了フラグ
	static bool m_bStopSound;	// サウンド(BGM)終了フラグ
	static bool m_bDrawParticle;	// パーティクルの描画フラグ
};

#endif