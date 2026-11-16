// ===========================================
// 
// ソケット通信クライアント[tcp_client.cpp]
// Author : Shu Tanaka
// 
// ===========================================
#include"tcp_client.h"

#pragma comment (lib, "ws2_32.lib")

// ===========================================
// コンストラクタ
// ===========================================
CTcpClient::CTcpClient()
{
	m_sock = INVALID_SOCKET;
}

// ===========================================
// デストラクタ
// ===========================================
CTcpClient::~CTcpClient()
{

}

// ===========================================
// 初期化処理
// ===========================================
bool CTcpClient::Init(const char* pIPAddress, int nPortNumber)
{
	//printf("[通信用のソケットを作成]\n");

	// --- ソケット作成 --- //
	m_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (m_sock == INVALID_SOCKET)
	{// 作成失敗
		//printf("ソケットの作成に失敗しました。\n");
		return false;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPortNumber);
	addr.sin_addr.S_un.S_addr = inet_addr(pIPAddress);

	// --- サーバーに接続する --- //
	if (connect(m_sock, (struct sockaddr*)&addr, sizeof(addr)) != 0)
	{
		//printf("サーバーに接続できませんでした\n");
		return false;
	}

	return true;
}

// ===========================================
// 初期化処理
// ===========================================
bool CTcpClient::Init(SOCKET sock)
{
	// --- メンバ変数に設定 --- //
	m_sock = sock;
	return true;
}

// ===========================================
// 終了処理
// ===========================================
void CTcpClient::Uninit(void)
{
	if (m_sock == INVALID_SOCKET)
	{
		return;
	}

	// --- 接続を切断 --- //
	if (m_sock != NULL) {
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
}

// ===========================================
// 送信処理
// ===========================================
int CTcpClient::Send(char* pSendData, int nSendDataSize)
{
	// --- データの送信 --- //
	if (m_sock == INVALID_SOCKET) {
		return 0;
	}

	// --- 送信処理 --- //
	int nSendByte = 0;

	nSendByte = send(m_sock, pSendData, nSendDataSize, 0);

	return nSendByte;
}
// ===========================================
// 受信処理
// ===========================================
int CTcpClient::Recv(int* pRecvData, int nRecvDataSize)
{
	// --- データの受信 --- //
	if (m_sock == INVALID_SOCKET) {
		return 0;
	}

	// --- 各種変数の宣言 --- //
	int nRecvByte;	// 接続状況の確認

	nRecvByte = recv(m_sock, (char*)pRecvData, nRecvDataSize, 0);

	if (nRecvByte <= 0)
	{// 接続を確認
		Uninit();
	}

	return nRecvByte;
}
