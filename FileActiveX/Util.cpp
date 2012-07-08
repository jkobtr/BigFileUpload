#include <afx.h> 
#include <malloc.h> 
#include "Util.h"
#include "stdafx.h"
//#define DEBUG_UTIL
 

/*
  ��ʼ������˼���SOCKET
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
  ���ܿͻ�������
*/
bool AcceptSocket(SOCKET &sockListen, SOCKET &sockClient, DWORD &nIP, long nTimeout)
{
	struct timeval	interval;						// ��ѯ�ĵȴ�ʱ��
	fd_set			listenSockSet;					// �����ͻ����ӵ�Socket����
	SOCKADDR_IN		sockAccept;				        // �������ӿͻ���Socket���

	// ����Ƿ�����������
	interval.tv_sec = nTimeout / 1000;
	interval.tv_usec = nTimeout % 1000 * 1000;
	FD_ZERO(&listenSockSet);
	FD_SET(sockListen, &listenSockSet);
	int nPendingSock = select(0, &listenSockSet, NULL, NULL, &interval);
	
	// �ȴ���ʱ���м�û����������
	if (nPendingSock == 0) 
		return false;

	// Socket������������
	//if (nPendingSock == SOCKET_ERROR)
	if (nPendingSock != 1)
	{
		//char strMsg[128];
		//sprintf(strMsg, "listen select() fail, rc=%d !", WSAGetLastError());
		//SysLogCache.Put(strMsg, MSGLEVEL_ERROR);
		return false;	
	}
	
	// ���ܵȴ��е���������
    DWORD nSockAcceptLen = sizeof(sockAccept);
	sockClient = accept(sockListen,(struct sockaddr FAR *)&sockAccept,(int FAR *)&nSockAcceptLen);
	if (sockClient == INVALID_SOCKET)
	{
		//char strMsg[128];
		//sprintf(strMsg, "accept() fail, rc=%d", WSAGetLastError());
		//SysLogCache.Put(strMsg, MSGLEVEL_WARNING);
		return false;
	}

	// �ͻ���IP��ַ
	nIP = *(long *)&sockAccept.sin_addr;
	return true;
}


 

/*
  ���ӷ�����(�����������˿�)
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
  ���ӷ�����(IP��ַ���˿�)
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
  �Ͽ�SOCKET����
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
  ���ͻ���������ʱ��ʧ��
*/
bool SendBuffer(SOCKET sock,char *pBuffer, long nLength, long nTimeout, int *pCommStatus)
{
	int status;								// ͨ�Ų����Ľ��
	int nSent;								// �ѷ��͵��ֽ���
	fd_set sockSet;							// ����Socket����
	int nWritingSock;						// �Ƿ���Է�����Ϣ
	struct timeval interval;				// �ȴ���ʱ����

	// �ջ�����
	if (nLength <= 0)
		return true;

	nSent = 0;
	FD_ZERO(&sockSet);
	FD_SET(sock,&sockSet);
	interval.tv_sec = nTimeout / 1000;
	interval.tv_usec = nTimeout % 1000 * 1000;
	while (true)
	{
		// ����Ƿ���Է�������
		nWritingSock = select(0, NULL, &sockSet, NULL, &interval);
		
		// ��ʱ����
		if (nWritingSock != 1)
		{
#ifdef DEBUG_UTIL
			SysLogCache.Put("write select() fail.", MSGLEVEL_DEBUG);
#endif
			return false;
		}
		
		// ���ջ������е�����
		status = send(sock, pBuffer+nSent, nLength-nSent,0);
		
		// �������ݳ���
		if (status <= 0)
		{
#ifdef DEBUG_UTIL
			SysLogCache.Put("send() fail - disconnect.", MSGLEVEL_DEBUG);
#endif
			return false;
		}
		
		// ���ӽ��յ����ֽ���
		nSent += status;

		// ����״̬֪ͨ
		if (pCommStatus)
		{
			//pCommStatus->nTotal = 0;
			//pCommStatus->nOffset = 0;
			pCommStatus = &nSent;
		}
		
		// ���������ȫ���������򷵻�
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
  ���SOCKET���Ƿ������ݵ���
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
  ��SOCKET��������
*/
bool RecvBuffer(SOCKET sock, char *pBuffer, long nLength, long nTimeout)
{
	int status;								// ͨ�Ų����Ľ��
	long nReceived;							// ���յ����ֽ���
	fd_set sockSet;							// ����Socket����
	int nReadingSock;						// ������Ϣ�Ŀͻ���
	struct timeval interval;				// �ȴ���ʱ����
	

	// Ҫ���յ�����Ϊ0
	if (nLength <= 0)
		return true;

	nReceived = 0;
	FD_ZERO(&sockSet);
	FD_SET(sock, &sockSet);
	interval.tv_sec = nTimeout / 1000;
	interval.tv_usec = nTimeout % 1000 * 1000;
	while (true)
	{
		// �����ջ��������Ƿ�������
		nReadingSock = select(0, &sockSet, NULL, NULL, &interval);

		// ��ʱ����
		if (nReadingSock != 1) 
		{
			int nError = WSAGetLastError();
			return false;
		}
	
		// ���ջ������е�����
		status = recv(sock, pBuffer+nReceived, nLength-nReceived, 0);

		// �������ݳ���
		if (status <= 0)
		{
			int nError = WSAGetLastError();
			return false;
		}
		
		// ���ӽ��յ����ֽ���
		nReceived += status;

		// ������յ�ȫ���������򷵻�
		if (nReceived >= nLength)
			return true;
	}
}


/*
  ��SOCKET�������е�����(���ȴ�)
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
		// �����ļ�ͷ
		if (nOffset > 0)
			DatFile.Seek(nOffset, CFile::begin); 

		// ���뾡���ܶ������
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

	// ���ļ�
	//if (nSize <= 0)
	//	return false;

	// ��ʵ���ݳ���
	nFileLen = nSize;
	return 4;
}