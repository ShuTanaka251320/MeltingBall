// =================================================
// 
// プレイヤー処理[player.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include"main.h"
#include"object.h"

// =================================================
// マクロ定義
#define	MAX_PART		(16)	// パーツの最大数
#define	PLAYER_PRI		(5)		// プレイヤーの優先順位
#define MAX_MATER		(4)		// 充電UIの最大メーター数
#define NUM_SCALEDOWN	(0.0f)	// スケールダウンする基準値
#define NUM_SCALEUP		(0.0f)	// スケールアップする基準値
#define MIN_HEATGAUGE	(-1000.0f)	// 熱ゲージの最小値
#define MAX_HEATGAUGE	(1800.0f)	// 熱ゲージの最大値
#define NUM_BURN		(1000.0f)	// 最大火力
#define NUM_COEINCREASE		(800.0f)	// 上昇係数をかける基準値
#define NUM_SETMATERIALRED	(10.0f)	// マテリアルを赤に変更する基準値
#define NUM_SETMATERIALBLUE	(0.0f)	// マテリアルを青に変更する基準値
#define RADIUS_NORMAL		(75.0f)	// 通常時の溶かす範囲
#define RADIUS_BURN			(130.0f)	// 最大火力時の溶かす範囲
#define	EXPAND_SPEED	(3.0f)			// 壁衝突時のメッシュリングリアクションの拡張スピード
#define	EXPAND_INRADIUS	(40.0f)			// 壁衝突時のメッシュリングリアクションの内半径
#define	EXPAND_OUTRADIUS	(50.0f)		// 壁衝突時のメッシュリングリアクションの外半径
#define	REACTRING_DEVIDE	(32)		// 壁衝突時のメッシュリングリアクションの分割数
#define	REACTRING_CREATESPAN	(15)	// 壁衝突時のメッシュリングリアクションの生成速度

// =================================================
// 前方宣言
class CModel;
class CObject2D;
class CComboUI;
class CMeshOrbit;
class CMeshRing;

// =================================================
// クラス定義
class CPlayer :public CObject
{
public:
	typedef enum
	{
		TYPE_NORMAL = 0,	// 通常
		TYPE_BIG,			// 巨大化
		TYPE_FIRE,			// 炎上
		TYPE_MAX
	}TYPE;

	CPlayer(int nPriority = PLAYER_PRI);
	~CPlayer();

	static CPlayer* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move);

	HRESULT Init(void);
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	// --- モデル読み込みや正規化関数 --- //
	HRESULT LoadModel(void);
	static bool JudgeComent(char* pStr);
	void NorRotate(float fRotDest);
	D3DXVECTOR3 GetMoveVec(void) { return m_pos - m_posOld; };

	// --- 値の設定や取得の関数 --- //
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; };
	void SetRotation(D3DXVECTOR3 rot) { m_rot = rot; };
	void PlusHeatGauge(float fSpan) { m_fHeat += fSpan; };
	void SetbHitSnow(bool bHit) { m_bHitSnowField = bHit; };

	float GetPosField(void) { return m_fposField; };
	D3DXVECTOR3 GetPosition(void) { return m_pos; };
	D3DXVECTOR3 GetPosOld(void) { return m_posOld; };
	D3DXVECTOR3 GetCamPosition(void) { return m_posCam; };
	D3DXVECTOR3 GetRotation(void) { return m_rot; };
	D3DXVECTOR3 GetMove(void) { return m_move; };
	D3DXVECTOR3 GetVecAxis(void) { return m_vecAxis; };
	D3DXVECTOR3 GetScl(void) { return m_scl; };
	bool IsMove(void) { return m_bMove; };
	D3DXMATRIX* GetMatrix(void) { return &m_mtxWorldPos; };
	float GetRadius(void) { return m_fRadius; };
	float GetheatGauge(void) { return m_fHeat; };
	CMeshRing* GetBatteryRing(int nIdx) { return m_pRing[nIdx]; };
	CMeshRing* GetCollRing(void) { return m_pCollRing; };
	CObject2D* GetComboUI(void) { return m_pCombo; }
	bool GetbHitSnow(void) { return m_bHitSnowField; };

	void SetParticle(void);
	bool Collision(D3DXVECTOR3* pPos);
	bool CollCenterLine(D3DXVECTOR3* pPos, D3DXVECTOR3 pPosOld);
	void HitSnowField(void);

private:
	void BatteryRing(void);
	void Bound(void);
	void MeshColl(void);
	void ItemColl(void);
	void WallColl(void);
	void ModelColl(void);
	void ChangeRotate(void);
	void LimitMove(void);
	void InCreaseHeatGauge(void);

	D3DXVECTOR3 m_pos;		// 位置
	D3DXVECTOR3 m_posOld;	// 過去の位置
	D3DXVECTOR3 m_posCam;	// カメラを向ける位置
	D3DXVECTOR3 m_rot;		// 向き
	D3DXVECTOR3 m_rotDest;	// 目標の向き
	D3DXVECTOR3 m_rotDiff;	// 目標と現在の向きの差分
	D3DXVECTOR3 m_move;		// 移動量
	D3DXMATRIX m_mtxWorld;	// ワールドマトリックス
	D3DXMATRIX m_mtxWorldPos;	// ワールドマトリックス
	D3DXVECTOR3 m_Offset[MAX_PART];	// オフセットの位置
	D3DXMATRIX m_mtxRot;	// 回転マトリックス(保存用)
	D3DXQUATERNION m_quat;	// クォータニオン
	D3DXVECTOR3 m_vecAxis;	// 回転軸
	D3DXVECTOR3 m_DestvecAxis;	// 目標の回転軸
	D3DXVECTOR3 m_DiffvecAxis;	// 回転軸の差分
	D3DXVECTOR3 m_scl;		// スケール
	float m_fValueRot;		// 回転角(回転量)	
	float m_fDestValueRot;	// 目標の回転角(回転量)	
	float m_fDiffValueRot;	// 目標の回転角(回転量)と現在の差分	
	float m_fMoveAngle;		// 移動向き
	float m_fDestMoveAngle;	// 目標の移動向き
	float m_fRebound;		// バウンド量
	float m_fSaveMove;		// ジャンプ量の保存
	bool m_bSetJumpForce;	// バウンド力のセットフラグ
	bool m_bJump;	// ジャンプフラグ
	float m_fposField;	// 足元の高さ
	float m_fRadius;	// プレイヤーの半径
	float m_fHeat;		// 熱ゲージ
	const char* m_pFileName;	// モデルパーティクルのファイル名
	bool m_bSoundMelt;	// 溶かす効果音の再生フラグ
	bool m_bHitSnowField;	// 雪のフィールドに当たったかのフラグ
	int m_nCntHitWall;		// 壁に当たっている時間
	bool m_bHitwall;		// 壁との衝突フラグ

	// --- プレイヤーモデルの情報 --- //
	CModel* m_apModel[MAX_PART];	// モデル(パーツ)のポインタ
	int m_nNumModel;				// モデル(パーツ)の数

	// --- プレイヤーの動作に関する変数 --- //
	bool m_bMove;		// 動いているか(何か入力されているとき)
	bool m_bPlaySound[TYPE_MAX];	// 各種効果音の再生フラグ

	// --- インスタンス生成用 --- //
	CObject2D* m_pCombo;	// コンボのUI用
	CComboUI* m_apNumber;	// コンボ数のUI用
	CMeshOrbit* pMeshOrbit;	// メッシュオービットのポインタ
	CMeshRing* m_pRing[MAX_MATER];	// 充電ゲージ用メッシュリング
	CMeshRing* m_pReactRing;	// 当たったリアクション用メッシュリング
	CMeshRing* m_pCollRing;	// 当たり判定用メッシュリング
};

#endif