// =================================================
// 
// メッシュ関連処理[mesh.h]
// Author : Shu Tanaka
// 
// =================================================
#ifndef _MESH_H_
#define _MESH_H_

#include"main.h"
#include"object.h"

// =================================================
// マクロ定義
#define MESH_PRI	(3)	// メッシュの優先順位

// =================================================
// クラス定義
class CMesh : public CObject
{
public:
	typedef enum
	{
		TYPE_FIELD = 0,	// フィールド
		TYPE_CYLINDER,	// シリンダー
		TYPE_RING,		// リング
		TYPE_SPHIRE,	// スフィア
		TYPE_DOME,		// ドーム
		TYPE_ORBIT,		// オービット
		TYPE_FLOOR,		// フロア(今回はスタジアム用)
		TYPE_MAX
	}TYPE;

	CMesh(int nPriority = MESH_PRI);
	~CMesh();

	// meshfield
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSizeX, float fSizeZ, float fMaxSizeX, float fMaxSizeZ,
		int nDevideX, int nDevideZ, int nNumVtx, int nNumIdx, CMesh::TYPE type, bool bDraw);

	// meshring
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSizeInside, float fSizeOutside,
		int nDevide, int nNumVtx, int nNumIdx, CMesh::TYPE type, bool bDraw);	

	// meshcylinder
	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSizeInside, float fSizeOutside,
		int nDevide, int nDevideheight, int nNumVtx, int nNumIdx, CMesh::TYPE type, bool bDraw, bool bDrawOutSide);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	float Collision(D3DXVECTOR3 pPos, D3DXVECTOR3 pPosOld, D3DXMATRIX Worldmtx, D3DXVECTOR3 posVtx[4]);
	float Collision(D3DXVECTOR3 pPos, D3DXVECTOR3 pPosOld);
	bool bCollision(D3DXVECTOR3 pPos, D3DXVECTOR3 pPosOld);
	float DotCal(D3DXVECTOR3 pPos, D3DXVECTOR3 pPosOld);
	float DotCalRing(D3DXVECTOR3 pPos, D3DXVECTOR3 pPosOld);
	void LoadStage(void);
	void LoadStageTitle(void);

	// --- メンバ変数の取得、変更する関数 --- //
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; };
	void SetRotation(const D3DXVECTOR3 rot) { m_rot = rot; };
	void SetSizeX(const float fSizeX) { m_fSizeX = fSizeX; };
	void SetSizeZ(const float fSizeZ) { m_fSizeZ = fSizeZ; };
	void SetLength(const float fWidth, float fHeight) { m_fLength = sqrtf(fWidth * fWidth + fHeight * fHeight) * 0.5f; };
	void SetAngle(const float fWidth, float fHeight) { m_fAngle = (float)atan2(fWidth, fHeight); };
	void SetMtxWorld(D3DXVECTOR3 Offset, D3DXMATRIX mtxWorld) {D3DXVec3TransformCoord(&m_pos, &Offset, &mtxWorld);};
	void SetVtxPos(D3DXVECTOR3 OffSet, int nIdx);
	void SetColor(D3DXCOLOR col);
	void SetInRadius(float fSize) {  m_fInRadius = fSize; }
	void SetOutRadius(float fSize) {  m_fOutRadius = fSize; }

	D3DXVECTOR3 GetPosition(void) { return m_pos; };
	D3DXVECTOR3 GetRotation(void) { return m_rot; };
	LPDIRECT3DVERTEXBUFFER9 GetVtxBuff(void) { return m_pVtxBuff; };
	D3DXMATRIX GetWorldMtx(void) { return m_mtxWorld; };
	float GetSizeX(void) { return m_fSizeX; };
	float GetSizeZ(void) { return m_fSizeZ; };
	float GetLength(void) { return m_fLength; };
	float GetAngle(void) { return m_fAngle; };
	int GetDevideX(void) { return m_nNumDevideX; };
	int GetDevideY(void) { return m_nNumDevideY; };
	int GetDevideZ(void) { return m_nNumDevideZ; };
	int GetDevideSurcle(void) { return m_nNumDeiveSurcle; };
	int GetNumVtx(void) {return m_nNumVtx;};
	float GetHeight(void) { return m_fHeight; };
	float GetInRadius(void) { return m_fInRadius; }
	float GetOutRadius(void) { return m_fOutRadius; }
	bool IsDraw(void) { return m_bDraw; };
	TYPE GetType(void) { return m_type; };
	float GetPosHeight(D3DXVECTOR3 pPos, D3DXVECTOR3 pPosOld);
	D3DXCOLOR GetCol(void) { return m_col; };
	void GetVtxPosition(D3DXVECTOR3 *pPos1, D3DXVECTOR3* pPos2);

	// --- テクスチャの指定 --- //
	void SetTexture(int nIdx) { m_nIdxTex = nIdx; };

private:
	void SetNor(void);
	void SetVtxPosition(const char *pFileName);

	// --- メンバ変数 --- //
	LPDIRECT3DTEXTURE9 m_pTexture;		// テクスチャのポインタ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファのポインタ	
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;	// インデックスバッファへのポインタ
	D3DXMATRIX m_mtxWorld;				// ワールドマトリックス
	int m_nIdxTex;						// テクスチャのインデックス

	int m_nNumVtx;	// 頂点数
	int m_nNumIdx;	// インデックス数
	int m_nNumPri;	// プリミティブ数

	int m_nNumDevideX;	// Xの分割数
	int m_nNumDevideY;	// Yの分割数
	int m_nNumDevideZ;	// Zの分割数

	int m_nNumDeiveSurcle;	// リングの分割数

	D3DXVECTOR3 m_pos;	// 位置
	D3DXVECTOR3 m_rot;	// 向き
	D3DXVECTOR3 m_NorOld[50000];	// 過去の法線
	float m_fLength;	// ポリゴンの対角線の長さ
	float m_fAngle;		// ポリゴンの対角線の角度
	float m_fSizeX;		// 横幅
	float m_fSizeY;		// 高さ
	float m_fSizeZ;		// 縦幅
	float m_fMaxSizeX;	// 最大横幅
	float m_fMaxSizeZ;	// 最大縦幅
	float m_fHeight;	// 高さ
	float m_fRadius;	// 半径
	float m_fInRadius;	// 内半径
	float m_fOutRadius;	// 外半径

	D3DXCOLOR m_col;	// 色

	bool m_bDraw;		// 描画するかどうか
	bool m_bDrawOutSide;	// 外側を描画するかどうか

	TYPE m_type;		// メッシュのタイプ
};

#endif