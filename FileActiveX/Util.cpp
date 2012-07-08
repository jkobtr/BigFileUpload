#include <afx.h> 
#include <malloc.h> 
#include "Util.h"
#include "stdafx.h"
//#define DEBUG_UTIL
 

/*
  初始化服务端监听SOCKET
*/
bool InitSocket(SOCKET &sockListen, char strInterface[], short nPort)
{
	SOCKADDR_IN sockLocal;
	sockListen = socket(AF_INET, SOCK_STREAM, 0);
	if (sockListen == INVALID_SOCKET) 
	{
		//char strMsg[128];
	    //sprintf(strMsg, "socket() fail, rc=%d", WSAGetLastError());
		//SysLogCache.Put(strMsg, MSGLEVEL_ERROR);		
		return false;
    }
	
	sockLocal.sin_family = AF_INET;
	sockLocal.sin_addr.s_addr = INADDR_ANY;
	if (strInterface[0])
		sockLocal.sin_addr.S_un.S_addr = inet_addr(strInterface);
	else
		sockLocal.sin_addr.S_un.S_addr = 0;
    sockLocal.sin_port = htons(nPort);
	if (bind(sockListen, (struct sockaddr FAR *)&sockLocal, sizeof(sockLocal)) == SOCKET_ERROR)
	{
		closesocket(sockListen);
		sockListen = INVALID_SOCKET;

		//char strMsg[128];
	    //sprintf(strMsg, "bind() fail, rc=%d !", WSAGetLastError());
		//SysLogCache.Put(strMsg, MSGLEVEL_ERROR);		
		return false;
	}
	
	if (listen(sockListen, SOMAXCONN) < 0) 
	{
		closesocket(sockListen);
		sockListen = INVALID_SOCKET;

		//char strMsg[128];
		//sprintf(strMsg, "listen() failed, rc=%d !", WSAGetLastError());
		//SysLogCache.Put(strMsg, MSGLEVEL_ERROR);
		return false;
	}
	
	return true;
}


/*
  接受客户端连接
*/
bool AcceptSocket(SOCKET &sockListen, SOCKET &sockClient, DWORD &nIP, long nTimeout)
{
	struct timeval	interval;						// 查询的等待时间
	fd_set			listenSockSet;					// 监听客户连接的Socket集合
	SOCKADDR_IN		sockAccept;				        // 本地连接客户的Socket结点

	// 检查是否有连接请求
	interval.tv_sec = nTimeout / 1000;
	interval.tv_usec = nTimeout % 1000 * 1000;
	FD_ZERO(&listenSockSet);
	FD_SET(sockListen, &listenSockSet);
	int nPendingSock = select(0, &listenSockSet, NULL, NULL, &interval);
	
	// 等待超时，中间没有连接请求
	if (nPendingSock == 0) 
		return false;

	// Socket操作发生错误
	//if (nPendingSock == SOCKET_ERROR)
	if (nPendingSock != 1)
	{
		//char strMsg[128];
		//sprintf(strMsg, "listen select() fail, rc=%d !", WSAGetLastError());
		//SysLogCache.Put(strMsg, MSGLEVEL_ERROR);
		return false;	
	}
	
	// 接受等待中的连接请求
    DWORD nSockAcceptLen = sizeof(sockAccept);
	sockClient = accept(sockListen,(struct sockaddr FAR *)&sockAccept,(int FAR *)&nSockAcceptLen);
	if (sockClient == INVALID_SOCKET)
	{
		//char strMsg[128];
		//sprintf(strMsg, "accept() fail, rc=%d", WSAGetLastError());
		//SysLogCache.Put(strMsg, MSGLEVEL_WARNING);
		return false;
	}

	// 客户端IP地址
	nIP = *(long *)&sockAccept.sin_addr;
	return true;
}


 

/*
  连接服务器(服务器名、端口)
*/
bool Connect(SOCKET &sock, const char strHost[], short nPort)
{
	PHOSTENT phe;
	SOCKADDR_IN dest_sin;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		return false;
	dest_sin.sin_family = AF_INET;
	phe = gethostbyname(strHost);
	if (phe == NULL) 
	{
		closesocket(sock);
		sock = INVALID_SOCKET;

		//char strMsg[128];
        //sprintf(strMsg, "gethostbyname(%s) fail, rc=%d", strHost, WSAGetLastError());
		//SysLogCache.Put(strMsg, MSGLEVEL_DEBUG);
		return false;
	}
	memcpy((char FAR *)&(dest_sin.sin_addr), phe->h_addr, phe->h_length);
	dest_sin.sin_port = htons(nPort);
	if (connect(sock, (PSOCKADDR)&dest_sin, sizeof(dest_sin)) < 0) 
	{
		closesocket(sock);
		sock = INVALID_SOCKET;

		//char strMsg[128];
		//sprintf(strMsg, "connect(%s:%d) fail, rc=%d", strHost, nPort, WSAGetLastError());
		//SysLogCache.Put(strMsg, MSGLEVEL_DEBUG);
		return false;
	}
	return true;
}


/*
  连接服务器(IP地址，端口)
*/
bool ConnectUseIP(SOCKET &sock, const char strIP[], short nPort)
{
	SOCKADDR_IN dest_sin;

	memset(&dest_sin, 0, sizeof(SOCKADDR_IN));
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		return false;

	dest_sin.sin_family = AF_INET;
	dest_sin.sin_addr.S_un.S_addr = inet_addr(strIP);
	dest_sin.sin_port = htons(nPort);
	if (connect(sock, (PSOCKADDR)&dest_sin, sizeof(dest_sin)) < 0) 
	{
		closesocket(sock);
		sock = INVALID_SOCKET;

		//char strMsg[128];
		//sprintf(strMsg, "connect(%s:%d) fail, rc=%d", strIP, nPort, WSAGetLastError());
		//SysLogCache.Put(strMsg, MSGLEVEL_DEBUG);
		return false;
	}
	return true;
}


/*
  断开SOCKET连接
*/
void Disconnect(SOCKET &sock)
{
	if (sock != INVALID_SOCKET)
	{
		shutdown(sock, SD_BOTH);
		closesocket(sock);
		sock = INVALID_SOCKET;

		//SysLogCache.Put("Disconnect socket successfully !", MSGLEVEL_DEBUG);
	}
}


/*
  发送缓冲区，超时则失败
*/
bool SendBuffer(SOCKET sock,char *pBuffer, long nLength, long nTimeout, int *pCommStatus)
{
	int status;								// 通信操作的结果
	int nSent;								// 已发送的字节数
	fd_set sockSet;							// 监听Socket集合
	int nWritingSock;						// 是否可以发送信息
	struct timeval interval;				// 等待的时间间隔

	// 空缓冲区
	if (nLength <= 0)
		return true;

	nSent = 0;
	FD_ZERO(&sockSet);
	FD_SET(sock,&sockSet);
	interval.tv_sec = nTimeout / 1000;
	interval.tv_usec = nTimeout % 1000 * 1000;
	while (true)
	{
		// 检查是否可以发送数据
		nWritingSock = select(0, NULL, &sockSet, NULL, &interval);
		
		// 超时出错
		if (nWritingSock != 1)
		{
#ifdef DEBUG_UTIL
			SysLogCache.Put("write select() fail.", MSGLEVEL_DEBUG);
#endif
			return false;
		}
		
		// 接收缓冲区中的数据
		status = send(sock, pBuffer+nSent, nLength-nSent,0);
		
		// 接收数据出错
		if (status <= 0)
		{
#ifdef DEBUG_UTIL
			SysLogCache.Put("send() fail - disconnect.", MSGLEVEL_DEBUG);
#endif
			return false;
		}
		
		// 增加接收到的字节数
		nSent += status;

		// 发送状态通知
		if (pCommStatus)
		{
			//pCommStatus->nTotal = 0;
			//pCommStatus->nOffset = 0;
			pCommStatus = &nSent;
		}
		
		// 如果发送完全部的数据则返回
		if (nSent >= nLength)
		{
#ifdef DEBUG_UTIL
			SysLogCache.Put("send():", MSGLEVEL_DEBUG);
			SysLogCache.Put(pBuffer, MSGLEVEL_DEBUG);
#endif
			return true;
		}
	}
}


/*
  检查SOCKET上是否有数据到达
*/
bool IsDataAvailable(SOCKET sock, int nTimeOut)
{
	fd_set sockSet;
	struct timeval interval;

	FD_ZERO(&sockSet);
	FD_SET(sock,&sockSet);
	interval.tv_sec = nTimeOut / 1000;
	interval.tv_usec = nTimeOut % 1000 * 1000;
	return (select(0, &sockSet, NULL, NULL, &interval) == 1);
}


/*
  从SOCKET接收数据
*/
bool RecvBuffer(SOCKET sock, char *pBuffer, long nLength, long nTimeout)
{
	int status;								// 通信操作的结果
	long nReceived;							// 接收到的字节数
	fd_set sockSet;							// 监听Socket集合
	int nReadingSock;						// 请求信息的客户数
	struct timeval interval;				// 等待的时间间隔
	

	// 要接收的数据为0
	if (nLength <= 0)
		return true;

	nReceived = 0;
	FD_ZERO(&sockSet);
	FD_SET(sock, &sockSet);
	interval.tv_sec = nTimeout / 1000;
	interval.tv_usec = nTimeout % 1000 * 1000;
	while (true)
	{
		// 检查接收缓冲区中是否有数据
		nReadingSock = select(0, &sockSet, NULL, NULL, &interval);

		// 超时出错
		if (nReadingSock != 1) 
		{
			int nError = WSAGetLastError();
			return false;
		}
	
		// 接收缓冲区中的数据
		status = recv(sock, pBuffer+nReceived, nLength-nReceived, 0);

		// 接收数据出错
		if (status <= 0)
		{
			int nError = WSAGetLastError();
			return false;
		}
		
		// 增加接收到的字节数
		nReceived += status;

		// 如果接收到全部的数据则返回
		if (nReceived >= nLength)
			return true;
	}
}


/*
  从SOCKET接收现有的数据(不等待)
*/



int LoadData(LPCTSTR strFileName, char *pFileBuf, DWORD &nFileLen, bool bFull, int nOffset)
{
	DWORD nSize;
	CFile DatFile;
	if (!DatFile.Open((LPCTSTR)strFileName, CFile::modeRead | CFile::shareDenyNone))
	{ 
		return 1;
	}

	try
	{
		// 忽略文件头
		if (nOffset > 0)
			DatFile.Seek(nOffset, CFile::begin); 

		// 读入尽可能多的数据
		nSize = (DWORD)DatFile.Read(pFileBuf, nFileLen);
		if (bFull && (nSize != nFileLen))
		{ 
			DatFile.Close();
			return 2;
		}
		DatFile.Close();
	}
	catch (CFileException *e)
	{
		e->Delete();
		DatFile.Abort();
		return 3;
	}

	// 空文件
	//if (nSize <= 0)
	//	return false;

	// 真实数据长度
	nFileLen = nSize;
	return 4;
}