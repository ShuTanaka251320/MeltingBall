//=============================================================================
//
// サウンド処理 [sound.h]
// Author : Shu Tanaka
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include "main.h"
#include <xaudio2.h>
#include <atomic>

//*****************************************************************************
// サウンド一覧
//*****************************************************************************
class CSound
{
public:

	typedef enum
	{
		SOUND_LABEL_GAMEBGM = 0,// ゲームBGM
		SOUND_LABEL_TUTORIALBGM,// チュートリアルBGM
		SOUND_LABEL_TITLEBGM,	// タイトルBGM
		SOUND_LABEL_RESULTBGM,	// リザルトBGM
		SOUND_LABEL_MOVENORMAL,	// 通常移動音
		SOUND_LABEL_MOVEBIG,	// 巨大化移動音
		SOUND_LABEL_MOVEFIRE,	// 炎上効果音
		SOUND_LABEL_CORNHIT,	// 三角コーンヒット
		SOUND_LABEL_CORNSPIN,	// 三角コーンスピン
		SOUND_LABEL_HYDRANT,	// 消火器効果音
		SOUND_LABEL_LIGHT,		// 街灯効果音
		SOUND_LABEL_SNOWMAN,	// 雪だるま効果音
		SOUND_LABEL_MELT,		// 雪が解ける音
		SOUND_LABEL_ENTER,		// 決定音
		SOUND_LABEL_SELECT,		// 選択音
		SOUND_LABEL_MAX
	} SOUND_LABEL;

	//*****************************************************************************
	// サウンド情報の構造体定義
	//*****************************************************************************
	typedef struct
	{
		const char* pFilename;	// ファイル名
		int nCntLoop;			// ループカウント
	} SOUNDINFO;

	CSound();
	~CSound();

	HRESULT Init(HWND hWnd);
	void Uninit(void);
	static HRESULT PlaySound(SOUND_LABEL label);
	static void Stop(SOUND_LABEL label);
	void Stop(void);

	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);
	static IXAudio2SourceVoice* GetSourceVoice(SOUND_LABEL type) { return m_apSourceVoice[type]; };

private:
	static SOUNDINFO m_aSoundInfo[SOUND_LABEL_MAX];				// サウンドの情報
	static IXAudio2* m_pXAudio2;								// XAudio2オブジェクトへのインターフェイス
	static IXAudio2MasteringVoice* m_pMasteringVoice;			// マスターボイス
	static IXAudio2SourceVoice* m_apSourceVoice[SOUND_LABEL_MAX];	// ソースボイス
	static BYTE* m_apDataAudio[SOUND_LABEL_MAX];					// オーディオデータ
	static DWORD m_aSizeAudio[SOUND_LABEL_MAX];					// オーディオデータサイズ

};

#endif
