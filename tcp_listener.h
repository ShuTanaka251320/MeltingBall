// ===========================================
// 
// ソケット通信サーバー[tcp_listener.h]
// Author : Shu Tanaka
// 
// ===========================================

#ifndef _TCP_LISTENER_H_
#define _TCP_LISTENER_H_

#include"main.h"

class CTcpClient;

class CTcpListener
{
public:
	CTcpListener();
	~CTcpListener();

	bool Init(int nPortNumber);
	void Uninit(void);
	CTcpClient *Accept(void);

private:
	SOCKET m_sockServer;	// 接続待ち受けソケット
};

#endif