#include "AppGlobal.h"


namespace app
{
	net::ITcpServer* __TcpServer = nullptr;
	std::vector<net::ITcpClient*> __TcpGame;

	//�����߳�
	void onClientAccept(net::ITcpServer* tcp, net::S_CLIENT_BASE* c, const s32 code)
	{
		if (c == nullptr || tcp == nullptr)   return;

		LOG_MSG("new connect...%d [%s-%d] %d\n", (int)c->socketfd, c->ip, c->port,c->theadID);
	}
	//���߳�
	void onClientSecureConnect(net::ITcpServer* tcp, net::S_CLIENT_BASE* c, const s32 code)
	{
		if (c == nullptr || tcp == nullptr)   return;

		int aa = 0;
		int bb = 0;
		tcp->getSecurityCount(aa,bb);
		func::setConsoleColor(10);
		LOG_MSG("security connet...%d [%s:%d][connect:%d-%d]\n", (int)c->socketfd, c->ip, c->port, aa, bb);
		func::setConsoleColor(7);
	}
	//���߳�
	void onClientDisconnect(net::ITcpServer* tcp, net::S_CLIENT_BASE* c, const s32 code)
	{
		if (c == nullptr || tcp == nullptr)   return;
	
		int aa = 0;
		int bb = 0;
		tcp->getSecurityCount(aa, bb);
		const char* str1 = func::getShutDownError(c->shutdown_kind);
		const char* str2 = func::getCloseSocketError(code);

		func::setConsoleColor(8);
		LOG_MSG("disconnect-%d [%s::%s][con:%d-%d]\n",(int)c->socketfd, str1, str2, aa, bb);
		func::setConsoleColor(7);

		if (c->state == func::S_Connect || c->state == func::S_ConnectSecure)
		{
			c->Reset();
		}
		else if (c->state == func::S_Login)
		{
			c->state = func::S_NeedSave;
			LOG_MSG("AppGlobal leave...%d-%d \n", c->ID, (int)c->socketfd);
		}

	}
	//���߳�
	void onClientTimeout(net::ITcpServer* tcp, net::S_CLIENT_BASE* c, const s32 code)
	{
		if (c == nullptr || tcp == nullptr)   return;
	}
	//�����̻߳������߳�
	void onClientExcept(net::ITcpServer* tcp, net::S_CLIENT_BASE* c, const s32 code)
	{
		if (c == nullptr || tcp == nullptr)   return;
	}


	//�ͻ���
	////////////////
    //�����߳�
	void onConnect(net::ITcpClient* tcp, const s32 code)
	{
		//LOG_MSG("-------client connect...%d %d\n", code, tcp->getData()->ID);
	}
	//���߳�
	void onSecureConnect(net::ITcpClient* tcp, const s32 code)
	{
		LOG_MSG("-------client security connect...%d %d\n", code, tcp->getData()->ID);
	}
	//�����߳�OR���߳�
	void onDisconnect(net::ITcpClient* tcp, const s32 code)
	{
		LOG_MSG("-------client onDisconnect...%d\n", code);
	}
	//���߳�
	void onExcept(net::ITcpClient* tcp, const s32 code)
	{
		LOG_MSG("-------client onOnExcept...%d\n", code);
	}

}

