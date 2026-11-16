// =================================================
// 
// フェード処理[fade.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _FADE_H_
#define _FADE_H_

#include "main.h"
#include "scene.h"

// =================================================
// クラス定義
class CFade
{
public:
	// =================================================
	// フェードの状態
	typedef enum
	{
		FADE_NONE = 0,	// フェードの流れ終了
		FADE_IN,		// フェードイン
		FADE_OUT,		// フェードアウト
		FADE_MAX,
	}FADE;

	CFade();
	~CFade();

	void Init(CScene::MODE modeNext);	
	void Uninit(void);			
	void Update(void);			
	void Draw(void);			
	static void SetFade(CScene:: MODE modeNext);
	FADE GetFade(void) { return m_fade; };

private:

	LPDIRECT3DVERTEXBUFFER9	m_pVtxBuff;	// 頂点バッファへのポインタ
	static FADE m_fade;				// フェードの状態
	static CScene::MODE m_modeNext;	// 次のモード
	static D3DXCOLOR m_color;			// ポリゴン(フェード)の色
};


#endif