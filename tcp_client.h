// ===========================================
// 
// ソケット通信クライアント[tcp_client.h]
// Author : Shu Tanaka
// 
// ===========================================

#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include"main.h"

// ===========================================
// クラス定義
class CTcpClient
{
public:
	CTcpClient();
	~CTcpClient();

	bool Init(const char *pIPAddress, int nPortNumber);
	bool Init(SOCKET sock);
	void Uninit(void);
	int Send(char *pSendData, int nSendDataSize);
	int Recv(int *pRecvData, int nRecvDataSize);

	SOCKET GetSocket(void) { return m_sock; };

private:
	SOCKET m_sock;	// 通信用ソケット
};

#endif