#pragma once

#include <io.h>
#include <winsock2.h>

// Windows版本号
// Windows 2000   0x0500
// Windows XP     0x0501
// Windows Vista  0x0600
// Windows 7      0x0601
//#define _WIN32_WINNT=0x0500

// SOCKET操作超时(ms)
#define LISTEN_TIMEOUT					100				// 监听连接超时
#define SEND_TIMEOUT					10000			// 发送数据包超时
#define RECV_TIMEOUT					10000			// 接收数据包超时

#define ERROR_RECV_OK					0				// 接收数据包成功
#define ERROR_RECV_PACKAGE_HEADER		-1				// 数据包头接收错误
#define ERROR_RECV_PACKAGE_BODY			-2				// 数据包体接收错误
#define ERROR_CONNECTION_LOST			-3				// 数据包接收错误
#define ERROR_PACKAGE_FORMAT			-4				// 数据包格式不正确

#define SPLIT_MAXLEN					64				// 分隔字符串的最大片段长度
#define TAG_LEN							128				// 数据标记长度

 
//...													// 保留(功能授权)
#define INFO_ITEM_ICID					21				// 智能卡号
#define INFO_ITEM_CRC					22				// 内容校验
#define INFO_ITEM_MAC					23				// MAC地址
#define INFO_ITEM_KEY					24				// 加密后的密钥
#define INFO_ITEM_KEYDATE				25				// 密钥时间戳(YYYYMMDD)
#define INFO_ITEM_ENCORGSIZE			26				// 加密前的长度
#define INFO_ITEM_MIP					27				// 转发组播地址
#define INFO_ITEM_MPORT					28				// 转发组播端口
#define INFO_ITEM_END					0xFE			// 结束属性
#define INFO_ITEM_EOF					0xFF			// 结束列表
#define MAX_ATTRLIST_SIZE				500				// 通信块中属性列表最大长度 

#define MSG_BUFSIZE	(100*1024)	// 消息包大小(文件分片为64KB,保留适当余量) 
#define TRANS_BLOCK_SIZE				4096			// 传输分片大小
#define MAX_FILENAME_LEN				64				// 文件名最大长度

#define MSGID_INIT						1				// 初始登录
#define MSGID_REINIT					2				// 续传初始化
#define MSGID_PUTPART					3				// 分片上传
#define MSGID_FIN						4				// 上传完成


#pragma pack(1)
// 请求包头格式
typedef struct 
{
	DWORD nPkgSize;										// 包的大小(消息体大小)
	WORD nMsgId;										// 消息编码
	DWORD nSessionId;										// 请求者
	BYTE nCheck;										// 校验(消息头各字节XOR)
}MSG_REQ_COMMON_HEAD;


// 应答包头格式
typedef struct 
{
	DWORD nPkgSize;										// 包的大小(消息体大小)
	WORD nMsgId;										// 消息编码
	BYTE bResult;										// 请求是否成功(若失败，后继的为错误消息包)
	DWORD nSessionId;										// 请求者
	BYTE nCheck;										// 校验(消息头各字节XOR)
}MSG_RET_COMMON_HEAD;

//应答分片
typedef struct
{
	char strFileName[MAX_FILENAME_LEN+1];				// 文件名称
	DWORD nUpdateTime;									// 文件更新时间
	DWORD nOrgSize;										// 文件大小
	DWORD nZipSize;										// 压缩大小
	DWORD nDataSize;									// 数据块大小
	DWORD nOffset;										// 分片偏移
	DWORD nPartSize;									// 分片大小 
} MSG_RET_GETPART_BODY;


/////////////////////////////
// 上传分片
typedef struct
{
	char nSessionID[MAX_FILENAME_LEN+1];				// 请求的SessionID [唯一标识]
	char strMd5[16];                                    //MD5
	DWORD nOrgSize;										// 文件大小
	int nOffset;										// 分片偏移
	DWORD nPartSize;									// 分片大小
	char pData[0];										// 分片内容(变长)
} MSG_REQ_PUTPART_BODY;
#pragma pack()

// 初始化连接
bool InitSocket(SOCKET &sockListen, char strInterface[], short nPort);

// 接受连接
bool AcceptSocket(SOCKET &sockListen, SOCKET &sockClient, DWORD &nIP, long nTimeout = LISTEN_TIMEOUT);


// 连接服务器/断开连接
bool Connect(SOCKET &sock, const char strHost[], short nPort);
bool ConnectUseIP(SOCKET &sock, const char strIP[], short nPort);
void Disconnect(SOCKET &sock);

// 判断是否有数据
bool IsDataAvailable(SOCKET sock, int nTimeOut);

// 收发进度
typedef struct
{
	// 任务数
	int nTaskNum;			// 文件数目

	// 累计数据量
	int nTaskTotal;			// 总数据量
	int nTaskTrans;			// 已发送、接收数据量

	// 某个文件的数据量
	int nTotal;				// 总数据量
	int nOffset;			// 数据偏移量(HTTP头大小)
	int nTrans;				// 已发送、接收数据量

	// 收发状态
	bool bFinished;			// 接收线程已停止
	bool bCancel;			// 用户点击取消
	bool bFail;				// 收发失败
} COMM_STATUS;

// 收发数据
bool SendBuffer(SOCKET sock, char *pBuffer, long nLength, long nTimeout = SEND_TIMEOUT, int *pCommStatus /*文件读取的位置 */=0);
bool RecvBuffer(SOCKET sock, char *pBuffer, long nLength, long nTimeout = RECV_TIMEOUT);  
int LoadData(LPCTSTR strFileName, char *pFileBuf, DWORD &nFileLen, bool bFull, int nOffset);