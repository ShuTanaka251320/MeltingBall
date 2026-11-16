// ===========================================
// 
// ソケット通信サーバー[tcp_listener.cpp]
// Author : Shu Tanaka
// 
// ===========================================
#include"tcp_listener.h"
#include"tcp_client.h"

// ===========================================
// コンストラクタ
// ===========================================
CTcpListener::CTcpListener()
{
	m_sockServer = INVALID_SOCKET;	// クリア
}

// ===========================================
// デストラクタ
// ===========================================
CTcpListener::~CTcpListener()
{

}

// ===========================================
// 初期化処理
// ===========================================
bool CTcpListener::Init(int nPortNumber)
{
	// --- ソケット作成 --- //
	//printf("[接続用のソケットを作成]\n");

	m_sockServer = socket(AF_INET, SOCK_STREAM, 0);

	// --- 接続を受け付けるための準備 --- //
	//printf("[接続受付の準備]\n");
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;

	// 待ち受けるポート番号を設定
	addr.sin_port = htons(nPortNumber);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	// ソケットにポートを割り当てる
	bind(m_sockServer, (struct sockaddr*)&addr, sizeof(addr));

	// 接続受付の準備
	listen(m_sockServer, 10);

	return true;
}

// ===========================================
// 終了処理
// ===========================================
void CTcpListener::Uninit(void)
{
	// --- ソケットチェック --- //
	if (m_sockServer == INVALID_SOCKET)
	{
		return;
	}

	// --- 接続受付用ソケットを閉じる --- //
	closesocket(m_sockServer);
	m_sockServer = INVALID_SOCKET;
}

// ===========================================
// 接続受付処理
// ===========================================
CTcpClient *CTcpListener::Accept(void)
{
	// --- ソケットのチェック --- //
	if (m_sockServer == INVALID_SOCKET)
	{
		return NULL;
	}

	// --- 接続待機 --- //
	//printf("[接続受付中…]\n");
	struct sockaddr_in clientAddr;
	int nLength = sizeof(clientAddr);

	// --- 接続を受け付ける --- //
	SOCKET sock = accept(m_sockServer, (struct sockaddr*)&clientAddr, &nLength);
	//printf("クライアントが接続しました。\n");

	// TCP送受信クラスのインスタンス生成
	CTcpClient* pTcpClient = new CTcpClient;

	if (pTcpClient != NULL)
	{
		// --- 初期化処理 --- //
		pTcpClient->Init(sock);
	}

	return pTcpClient;
}
