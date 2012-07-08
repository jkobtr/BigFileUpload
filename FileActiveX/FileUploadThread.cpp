#include "FileUploadThread.h" 
using namespace std; 
time_t start,finish;
FileUploadThread::FileUploadThread()
{
	client=NULL;
	m_inCanel=FALSE;
	
}
FileUploadThread::~FileUploadThread()
{

}
int reconnectNUM=0; //重复次数 
DWORD FileUploadThread::ThreadMethod() 
{   
	try
	{ 
		/*FILE * tempfs;
		char   *p_dest;  
		p_dest   =   new   char[250];  
		::GetTempPath(250,p_dest);
		strcat(p_dest,"infoNet.cer");
		strcpy_s(m_certPath,p_dest); 
		char* mySTR="这里是被写入的一行:我爱中华人民共和国。"; 
		tempfs=fopen(p_dest,"wb+");
		fwrite(mySTR,strlen(mySTR),1,tempfs);
		fclose(tempfs); */

		m_state=false;
		m_currenProgress=-2;
		if(!InitUpload())
		{
			//this->pCallBack->OnFailed();
			return 0;
		}
		this->pCallBack->OnSuccesful();//模拟操作成功，回调主线程   
		m_state=true;  
	}  
	catch(...)
	{
		return -1;
	}
	
	return 0; 
}

BOOL FileUploadThread::ReConnectSend(int nfileSize,int nPartSize,char * pFileid)
{ 
	BOOL binit=TRUE; 
	//初始化应答声明
	char * tbufHead=NULL,*tbufBody=NULL;
	MSG_RET_COMMON_HEAD thead1;
	MSG_RET_INIT_CONT_BODY tbody1;
reconnect:
	int err=0;
	CloseSocket();
	Sleep(MAX_RECONTINUE_SLEEPTIME); //等待10
	if(reconnectNUM>=MAX_RECONTINUE_NUM || m_inCanel)
	{
		m_ErrMSG="\n超过续传重新连接初始化次数！";
		m_SendLog.Append("\n超过续传重新连接初始化次数！");
		binit=FALSE;
		goto cleanup;
	}
	//重新连接
	if (!Connect(client,m_ConnIP ,(short)m_ConnPort))
	{
		reconnectNUM++;
		m_ErrMSG="\n续传重新连接初始化失败！";
		m_SendLog.Append("\n续传重新连接初始化失败！！");
		binit=FALSE;
		goto reconnect;
	}
	//////////////////////////////////////////////////////////////////////////
	//重试初始化
	//////////////////////////////////////////////////////////////////////////
	if(!SendInitCont(nfileSize))
	{
		m_ErrMSG="\n续传发送初始化请求失败！"; 
		m_SendLog.Append("\n续传发送初始化请求失败！");
		binit=FALSE;
		goto cleanup;
	} 
	////////////////////////////////////////////////////////////////////////
	//初始化应答
	////////////////////////////////////////////////////////////////////////// 
	tbufHead=(char*)malloc(MSG_RET_COMMON_HEAD_LEN);
	if(!RecvBuffer(client,tbufHead, MSG_RET_COMMON_HEAD_LEN))
	{
		binit=FALSE;
		goto cleanup;
	}
	thead1.setNetBuf(tbufHead,MSG_REQ_COMMON_HEAD_LEN);  
	tbufBody=(char*)malloc(thead1.nPkgSize); 
	 m_MsgState=MSG_STATE_INITCONT;
	if(!thead1.bResult)
	{	
		//m_ErrMSG.Append("续传初始化验证失败！"); 
		RecvError(thead1.nPkgSize); 
		binit=FALSE;
		goto cleanup;
	} 
	 m_MsgState=MSG_STATE_SENDPARTDATA;
	if(!RecvBuffer( client,tbufBody, thead1.nPkgSize))
	{
		m_ErrMSG="\n重新接受初始化包体失败！"; 
		m_SendLog.Append("\n重新接受初始化包体失败！");
		binit=FALSE;
		goto cleanup;
	} 
	tbody1.setNetBuf(tbufBody,thead1.getNetBufLen());  
	strcpy_s(sUserId,thead1.sUserId);  //修改当前用户名 
	m_nOffset=tbody1.nOffset+1;      //成功的位移的下一个位置开始
	//m_SendLog.AppendFormat("%d\n",m_nOffset);
	//////////////////////////////////////////////////////////////////////////
	//续传文件
	////////////////////////////////////////////////////////////////////////// 
	if(!SendPacketGetpartCont(nfileSize,nPartSize,pFileid))
	{
		m_ErrMSG="\n断点上传切片失败！";
		m_SendLog.Append("\n断点上传切片失败！");
		binit=FALSE; 
		 
		goto reconnect; 
	}  
	binit=true;
	m_ErrMSG="\n断点续传成功！"; 
cleanup: 
	free(tbufHead);
	//tbufHead=NULL;
	free(tbufBody);
	//tbufBody=NULL;
	return binit;
}
BOOL FileUploadThread::SendPacketGetpartCont(int nfileSize,int nPartSize,char * pFileid)
{
	//////////////////////////////////////////////////////////////////////////
	//文件大小
	//////////////////////////////////////////////////////////////////////////  
	char * phead=NULL,*pbody=NULL;
	char * pFileBuf=NULL;
	int n_hlen,nbodylen;
	 
	MSG_REQ_SENDPART_BODY m_RQ_SendPart_body; //上传分片包体
	MSG_REQ_COMMON_HEAD m_RQ_SendPart_head;  //包头 
	MSG_RET_COMMON_HEAD mTrecvHead;
	MSG_RET_SENDPART_BODY mTrecvbody;
	int errcode=0;
	DWORD nSize;
	CFile DatFile; 
	CString fb;  //上传文件分片次数
	int icous=0;  //判断上传失败当前位移重新传 接受失败 断点从下个位移开始
	//int nfilelen=m_fileSeek; //文件读取位置
	bool isRun=true;
	float lenprog=((float)(100-m_contprogress)/((float)((nfileSize)-(m_fileSeek))/(float)nPartSize)); //计算进度刻度
	float progress=(float)m_contprogress; //开始的进度数
	//至少读一次分片数据 

	int ii=m_nOffset; 
	if (!DatFile.Open(m_InputFilePath, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary))
	{
		m_ErrMSG="读取文件失败！"; 
		goto cleanup;
	} 
	DatFile.Seek((m_nOffset-1)*nPartSize,CFile::begin); 
	//m_SendLog.AppendFormat("\nFile Size:%d\n文件位移:%d\n分片大小:%d\nm_nOffset:%d",nfileSize,m_fileSeek,nPartSize,ii);  
	int MaxSendPark=nfileSize / nPartSize + ((nfileSize%nPartSize == 0)?0:1);
	//Progress(1);
	do
	{
		if(m_inCanel)
		{
			isRun=false;
			goto cleanup; 
		}
		progress+=lenprog;
		m_contprogress=(int)progress; //进度
		Progress((int)progress);
		try
		{ 
			m_RQ_SendPart_body.clear();
			// 读入nPartSize多的数据 
			nSize = (DWORD)DatFile.Read(m_RQ_SendPart_body.pData, nPartSize); 
			if(m_isCompress)
			{
				char srcData[MAX_FILE_BLOCK_SIZE*2]; 
				DWORD zlen=MAX_FILE_BLOCK_SIZE*2; ///压缩大小
				///压缩
				errcode=compress((BYTE *)srcData,&zlen, (BYTE *)m_RQ_SendPart_body.pData,nSize );
				if (errcode!= Z_OK) 
				{
					m_SendLog.AppendFormat("\nCompress Failed!!Erorr Code:%d",errcode);  
					isRun=false;
					goto cleanup; 
				}
				//memcpy(m_RQ_SendPart_body.pData,srcData,zlen); 
				//m_RQ_SendPart_body.nPartSize=zlen;  //切片大小
				if(m_isEncrypt)
				{
					uint8_t* encryptData=NULL,*dedata=NULL;
					int encryptlen=0,delen=0; 
					errcode=do_crypt(SECERTKEY,(uint8_t*)srcData,zlen,&encryptData,&encryptlen,1); 
					if(errcode)
					{
						memcpy(m_RQ_SendPart_body.pData,encryptData,encryptlen); 
						m_RQ_SendPart_body.nPartSize=encryptlen;
					}else
					{
						m_SendLog.AppendFormat("\nEncrypt Failed!!Erorr Code:%d",errcode); 
						isRun=false;
						goto cleanup; 
					}
				}else
				{
					memcpy(m_RQ_SendPart_body.pData,srcData,zlen); 
					m_RQ_SendPart_body.nPartSize=zlen;  //切片大小
				} 
				/*uint8_t *dedatas=NULL;
				int delens=0; 
				int ff=do_crypt(SECERTKEY,(uint8_t*)m_RQ_SendPart_body.pData,m_RQ_SendPart_body.nPartSize,&dedatas,&delens,0);  
				char srcData1[MAX_FILE_BLOCK_SIZE*2]; 
				DWORD zlen1=MAX_FILE_BLOCK_SIZE*2; ///压缩大小
				ff=uncompress((BYTE *)srcData1,&zlen1, (BYTE *)dedatas,delens);
				CString dea;
				dea.Format("%d:%d",ff,ii);
				LogFile(dea,dea.GetLength());*/
			}
			else
			{
				//不压缩至加密
				if(m_isEncrypt)
				{
					uint8_t* encryptData=NULL;
					int encryptlen=0; 
					int errcode=0;
					errcode=do_crypt(SECERTKEY,(uint8_t*)m_RQ_SendPart_body.pData,nSize,&encryptData,&encryptlen,1); 
					if(errcode)
					{
						memcpy(m_RQ_SendPart_body.pData,encryptData,encryptlen);  
						m_RQ_SendPart_body.nPartSize=encryptlen;
					}else
					{
						m_SendLog.AppendFormat("\nEncrypt Failed!!Erorr Code:%d !UUID:%s",errcode,SECERTKEY); 
						isRun=false;
						goto cleanup; 
					} 
				}else
				{
					m_RQ_SendPart_body.nPartSize=nSize;
				}
				//m_RQ_SendPart_body.nPartSize=nSize;
			}
			//请求包体
			//////////////////////////////////////////////////////////////////////////
			m_RQ_SendPart_body.nOffset=ii; //文件偏移 
			//m_RQ_SendPart_body.nCompress_Size=zlen;
			strcpy_s(m_RQ_SendPart_body.strFileID,pFileid);  //文件ID  
			pbody=m_RQ_SendPart_body.getNetBuf();
			nbodylen=m_RQ_SendPart_body.getNetBufLen(); 
			//////////////////////////////////////////////////////////////////////////
			//请求包头
			//////////////////////////////////////////////////////////////////////////
			m_RQ_SendPart_head.clear(); 
			m_RQ_SendPart_head.nMsgId=MSGID_SEND_PART; //切片上传 
			strcpy_s(m_RQ_SendPart_head.sUserId,sUserId); //用户信息
			m_RQ_SendPart_head.nPkgSize=nbodylen; 
			phead=m_RQ_SendPart_head.getNetBuf();
			n_hlen=m_RQ_SendPart_head.getNetBufLen();  

			if(!SendBuffer(client,phead,n_hlen)){
				m_SendLog.Append("\n续传分片上传:上传分片包头失败!");
				isRun=false;
				goto cleanup; 
			}
			if(!SendBuffer(client,pbody,nbodylen)){
				m_SendLog.Append("\n续传分片上传:上传分片包体失败!");
				isRun=false;
				goto cleanup; 
			} 
			//////////////////////////////////////////////////////////////////////////
			//初始化应答
			////////////////////////////////////////////////////////////////////////// 
			//应答
			char * tbufHead=NULL,*tbufBody=NULL;
			tbufHead=(char*)malloc(MSG_RET_COMMON_HEAD_LEN);
			if(!RecvBuffer(client, tbufHead, MSG_RET_COMMON_HEAD_LEN)){
				m_SendLog.Append("\n续传分片上传：接受应答包头失败!");
				icous=1;
				isRun=false;
				goto cleanup; 
			}
			mTrecvHead.setNetBuf(tbufHead,MSG_RET_COMMON_HEAD_LEN);  
			if(!mTrecvHead.bResult){ 
				
				RecvError(mTrecvHead.nPkgSize);   ///记录是因为传输错误 
				icous=1;
				isRun=false;
				goto cleanup; 
			}
			tbufBody=(char*)malloc(mTrecvHead.nPkgSize);
			if(!RecvBuffer(client, tbufBody, mTrecvHead.nPkgSize)){
				m_SendLog.Append("\n续传分片上传:接受应答包体失败!");
				icous=1;
				isRun=false;
				goto cleanup; 
			} 
			mTrecvbody.setNetBuf(tbufBody, mTrecvHead.nPkgSize);

			free(tbufHead); 
			free(tbufBody);  
			 
			m_fileSeek += nSize;
			if(m_fileSeek>=nfileSize ){ //判断位置和分片大小
				isRun=true;
				goto cleanup; 
			}
		}
		catch (CFileException *e)
		{
			m_ErrMSG.Append(e->m_strFileName);
			e->Delete();
			DatFile.Abort();
			isRun=false;
			goto cleanup; 
		} 
		//空文件
		if (nSize <= 0){
			isRun=false;
			goto cleanup; 
		}
		// 真实数据长度
		ii++;
		 
	}while(true);  
cleanup:  
	//fb.Format("\nFile Seek Postion:%d\nReSendPartCount:%d\nSend end Offset:%d\n\n",m_fileSeek,ii,(ii-1)*nPartSize);
	//m_SendLog.Append(fb); 
	DatFile.Close();
	if(isRun)
		Progress(100);
	return isRun;
}
void FileUploadThread::Progress(int nprogr)
{
	if(m_currenProgress<nprogr)
	{
		m_currenProgress=nprogr;
		if(m_currenProgress<=100 && m_currenProgress>=-1)
			this->pCallBack->OnProgress(nprogr); 
	}
}
BOOL FileUploadThread::SendInitCont(int nfileSize)
{
	BOOL isrun=TRUE; 
	if(m_InputFilePath.IsEmpty()){
		m_ErrMSG="Input File is Empty!";
		return FALSE;
	}else
		if(m_fileMd5.IsEmpty())
			return FALSE;//m_fileMd5=MD5(ifstream(m_InputFilePath, ios_base::binary)).toString().c_str();  
	 
	/////////////////////////////////////////////////////////////////////////////
	//消息定义
	//////////////////////////////////////////////////////////////////////////
	char * bufHead=NULL,*bufBody=NULL;
	int bufHeadLen,bufBodyLen;
	MSG_REQ_COMMON_HEAD head1;
	MSG_REQ_INIT_CONT_BODY body1; 
	//////////////////////////////////////////////////////////////////////////
	//系统初始化
	////////////////////////////////////////////////////////////////////////// 
	//设置包头
	head1.clear(); 
	head1.nMsgId = MSGID_INIT_CONT;
	head1.bUserType = '0';
	//编码
	body1.clear();
	body1.bFileCheckType = '0';
	strcpy_s(body1.strVersion,"1.0.0");
	//body1. = m_nOffset; //文件大小
	//body1.nFileCheckSize =md5str;
	body1.nFileSize=nfileSize; //文件大小
	strcpy_s(body1.strFileID,m_FileName); //文件ID
	memcpy(body1.strFileCheck,m_fileMd5,m_fileMd5.GetLength());
	body1.nFileCheckSize=m_fileMd5.GetLength();
	body1.nTokenSize = m_InputToken.GetLength();
	strcpy_s(body1.strToken,m_InputToken);  //token令牌
	//编码
	bufBody = body1.getNetBuf();
	bufBodyLen = body1.getNetBufLen(); 
	strcpy_s(head1.sUserId,sUserId); 
	head1.nPkgSize=bufBodyLen; 
	bufHead = head1.getNetBuf();
	bufHeadLen = head1.getNetBufLen();   
	//char * sdbuffer=NULL;
	//sdbuffer=(char *)malloc(bufHeadLen+bufBodyLen);
	//memcpy(sdbuffer,bufHead,bufHeadLen);
	//memcpy(sdbuffer+bufHeadLen,bufBody,bufBodyLen);
	
	if (!SendBuffer(client,bufHead,bufHeadLen))
	{
		m_SendLog.Append("\n初始化头请求失败！");
		isrun=FALSE;
		goto cleanup;  
	}
	if (!SendBuffer(client,bufBody,bufBodyLen))
	{
		m_SendLog.Append("\n初始化头请求失败！");
		isrun=FALSE;
		goto cleanup;  
	}
	 
	/*head1.setNetBuf(sdbuffer,bufHeadLen);
	body1.setNetBuf(sdbuffer+bufHeadLen,bufHeadLen+bufBodyLen);*/
cleanup:  
	bufHead=NULL;  
	bufBody=NULL;  
	return isrun;
}
void InitTime()
{
	start=clock();   //开始时间
}
//统计时间
double DurationTime()
{
	finish=clock();  //结束时间
	return (double)(finish-start)/CLOCKS_PER_SEC;
}

BOOL FileUploadThread::SendInit(int nOrgSize)
{
	BOOL isrend=TRUE;
	//char * sdbuffer=NULL;
	//////////////////////////////////////////////////////////////////////////
	// 读取文件的MD5
	//////////////////////////////////////////////////////////////////////////
	 m_fileMd5="";
	if(m_InputFilePath.IsEmpty()){
		m_ErrMSG="Input File is Empty!";
		return FALSE;
	}else 
	{
		
		InitTime();
		
		//添加临时编码设置
		std::locale loc1 = std::locale::global(std::locale(".936"));
		{
			m_fileMd5=MD5(ifstream(m_InputFilePath, ios_base::binary)).toString().c_str();   
		} 
		std::locale::global(std::locale(loc1)); 
		m_MD5duration=DurationTime();
	}
	/////////////////////////////////////////////////////////////////////////////
	//消息定义
	//////////////////////////////////////////////////////////////////////////
	char * bufHead=NULL,*bufBody=NULL;
	int bufHeadLen,bufBodyLen;
	MSG_REQ_COMMON_HEAD head1;
	MSG_REQ_INIT_BODY body1;

	//////////////////////////////////////////////////////////////////////////
	//系统初始化
	////////////////////////////////////////////////////////////////////////// 
	//设置包头
	head1.clear(); 
	head1.nMsgId = MSGID_INIT;
	head1.bUserType = '0';
	//编码
	body1.clear();
	body1.bFileCheckType = '0';
	strcpy_s(body1.strVersion,"1.0.0");
	body1.nFileSize = nOrgSize; //文件大小
	//body1.nFileCheckSize =md5str;

	memcpy(body1.strFileCheck,m_fileMd5,m_fileMd5.GetLength());
	body1.nFileCheckSize=m_fileMd5.GetLength();
	body1.nTokenSize = m_InputToken.GetLength();
	strcpy_s(body1.strToken,m_InputToken);  //token令牌
	//编码
	bufBody = body1.getNetBuf();
	bufBodyLen = body1.getNetBufLen(); 
	head1.nPkgSize=bufBodyLen; 
	bufHead = head1.getNetBuf();
	bufHeadLen = head1.getNetBufLen();   
	
	//sdbuffer=(char *)malloc(bufHeadLen+bufBodyLen);
	//memcpy(sdbuffer,bufHead,bufHeadLen);
	//memcpy(sdbuffer+bufHeadLen,bufBody,bufBodyLen);
	if (!SendBuffer(client,bufHead,bufHeadLen))
	{
		m_SendLog.Append("初始化头请求失败！");
		isrend=FALSE;
		goto cleanup;
	} 
	if (!SendBuffer(client,bufBody,bufBodyLen))
	{
		m_SendLog.Append("初始化头请求失败！");
		isrend=FALSE;
		goto cleanup;
	} 
	isrend=TRUE;
cleanup:
	/*delete[] sdbuffer;
	sdbuffer=NULL;
	delete[] bufHead; */
	bufHead=NULL; 
	bufBody=NULL;
	return isrend;
}
BOOL FileUploadThread::Send_Init_ShakeHands()
{ 
	
	BOOL isTrue=TRUE;
	char * tbufHead=NULL,*tbufbody;
	MSG_REQ_COMMON_HEAD shakehead;
	MSG_REQ_SHAKEHANDS_BODY ShakeHands;
	MSG_RET_COMMON_HEAD tbufHeadbuf;
	MSG_RET_SEND_COMPLETE_BODY tbuftbody;
	char uuidsource[MAX_SECERTKEY_LEN+1];
	char *p_en=do_encryptSecertKey(CERINFOPATH,uuidsource); 
	strcpy(SECERTKEY,uuidsource);
	//LogFile(uuidsource,strlen(uuidsource));
	//////////////////////////////////////////////////////////////////////////
	//发送握手请求
	//////////////////////////////////////////////////////////////////////////
	shakehead.clear(); 
	shakehead.nMsgId = MSGID_INIT_SHAKEHANDS;
	shakehead.bUserType = '0'; 
	ShakeHands.clear(); 
	ShakeHands.setNetBuf((char *)p_en,MAX_SECERTKEY_LEN); 
	shakehead.nPkgSize=ShakeHands.getNetBufLen();
	char *headbuf,*bodybuf;
	headbuf=shakehead.getNetBuf();
	if (!SendBuffer(client,headbuf,shakehead.getNetBufLen()))
	{
		m_SendLog.Append("初始化头请求失败！");
		isTrue=FALSE;
		goto cleanup;
	} 
	bodybuf=ShakeHands.getNetBuf();
	if (!SendBuffer(client,bodybuf,ShakeHands.getNetBufLen()))
	{
		m_SendLog.Append("初始化头请求失败！");
		isTrue=FALSE;
		goto cleanup;
	}

	//////////////////////////////////////////////////////////////////////////
	//接受响应
	//////////////////////////////////////////////////////////////////////////
	tbufHead=(char*)malloc(MSG_RET_COMMON_HEAD_LEN);
	if(!RecvBuffer(client, tbufHead, MSG_RET_COMMON_HEAD_LEN))
	{
		isTrue=FALSE;
		goto cleanup;
	}
	tbufHeadbuf.setNetBuf(tbufHead,MSG_RET_COMMON_HEAD_LEN);
	if(!tbufHeadbuf.bResult)
	{
		isTrue=FALSE;
		goto cleanup;
	}
	tbufbody=(char*)malloc(tbufHeadbuf.nPkgSize);
	if(!RecvBuffer(client, tbufbody, tbufHeadbuf.nPkgSize))
	{
		isTrue=FALSE;
		goto cleanup;
	}
	tbuftbody.setNetBuf(tbufbody,tbufHeadbuf.nPkgSize);

cleanup:
	free(tbufHead);
	 
	free(tbufbody);
	 
	return isTrue;
}
BOOL FileUploadThread::SendPacketGetpart(const int snOffset,int fileSize,int nPartSize,char * pFileid)
{  
	//CFile ff;
	//CString endtime;
	
	Progress(0);
	//////////////////////////////////////////////////////////////////////////
	//文件大小
	////////////////////////////////////////////////////////////////////////// 
	//int nOffset=snOffset;
	//char ncheck=sncheck;
	char * phead=NULL,*pbody=NULL;
	char * pFileBuf=NULL;
	int n_hlen,nbodylen; 
	MSG_REQ_SENDPART_BODY m_RQ_SendPart_body; //上传分片包体
	MSG_REQ_COMMON_HEAD m_RQ_SendPart_head;  //包头 
	MSG_RET_COMMON_HEAD mTrecvHead;
	MSG_RET_SENDPART_BODY mTrecvbody;
	int errcode=0;
	DWORD nSize;
	CFile DatFile; 
	CString fb;  //上传文件分片次数 
	m_fileSeek=0;
	int icous=0;  //判断上传失败当前位移重新传 接受失败 断点从下个位移开始
	bool isRun=true;
	float lenprog=((float)100/((float)fileSize/(float)nPartSize)); //计算进度
	float progress=0; //开始的进度数
	
	//至少读一次分片数据 
	int ii=snOffset; 
	Progress(1);
	if (!DatFile.Open(m_InputFilePath, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary))
	{
		m_ErrMSG="读取文件失败！"; 
		goto cleanup;
	}  
	
	//ff.Open("c:\llog.log",CFile::modeCreate|CFile::modeReadWrite | CFile::shareDenyNone | CFile::typeBinary);
	do
	{ 
		if(m_inCanel)
		{
			isRun=false;
			goto cleanup; 
		}
		progress+=lenprog;
		m_contprogress=(int)progress; //进度
		Progress((int)progress); 
		try
		{  
			m_RQ_SendPart_body.clear();
			// 读入nPartSize多的数据 
			nSize = (DWORD)DatFile.Read(m_RQ_SendPart_body.pData, nPartSize); 
			m_fileSeek += nSize;
			if(m_isCompress)
			{
				char srcData[MAX_FILE_BLOCK_SIZE*2]; 
				DWORD zlen=MAX_FILE_BLOCK_SIZE*2; ///压缩大小
				///压缩
				//InitTime();
				errcode=compress((BYTE *)srcData,&zlen, (BYTE *)m_RQ_SendPart_body.pData,nSize );
				if (errcode!= Z_OK) 
				{
					m_SendLog.AppendFormat("\nCompress Failed!!Erorr Code:%d",errcode);  
					isRun=false;
					goto cleanup; 
				}
				
				/*endtime.Format("|\n;%0.3f\n",DurationTime());
				ff.Write(endtime,endtime.GetLength());
				ff.Flush();*/
				//memcpy(m_RQ_SendPart_body.pData,srcData,zlen); 
				//m_RQ_SendPart_body.nPartSize=zlen;  //切片大小
				if(m_isEncrypt)
				{
					uint8_t* encryptData=NULL,*dedata=NULL;
					int encryptlen=0,delen=0; 
					errcode=do_crypt(SECERTKEY,(uint8_t*)srcData,zlen,&encryptData,&encryptlen,1); 
					if(errcode)
					{
						memcpy(m_RQ_SendPart_body.pData,encryptData,encryptlen); 
						m_RQ_SendPart_body.nPartSize=encryptlen;
					}else
					{
						m_SendLog.AppendFormat("\nEncrypt Failed!!Erorr Code:%d",errcode); 
						isRun=false;
						goto cleanup; 
					}
				}else
				{
					memcpy(m_RQ_SendPart_body.pData,srcData,zlen); 
					m_RQ_SendPart_body.nPartSize=zlen;  //切片大小
				} 
				/*uint8_t *dedatas=NULL;
				int delens=0; 
				int ff=do_crypt(SECERTKEY,(uint8_t*)m_RQ_SendPart_body.pData,m_RQ_SendPart_body.nPartSize,&dedatas,&delens,0);  
				char srcData1[MAX_FILE_BLOCK_SIZE*2]; 
				DWORD zlen1=MAX_FILE_BLOCK_SIZE*2; ///压缩大小
				ff=uncompress((BYTE *)srcData1,&zlen1, (BYTE *)dedatas,delens);
				CString dea;
				dea.Format("%d:%d",ff,ii);
				LogFile(dea,dea.GetLength());*/
			}
			else
			{
				//不压缩至加密
				if(m_isEncrypt)
				{
					uint8_t* encryptData=NULL;
					int encryptlen=0; 
					int errcode=0;
					errcode=do_crypt(SECERTKEY,(uint8_t*)m_RQ_SendPart_body.pData,nSize,&encryptData,&encryptlen,1); 
					if(errcode)
					{
						memcpy(m_RQ_SendPart_body.pData,encryptData,encryptlen);  
						m_RQ_SendPart_body.nPartSize=encryptlen;
					}else
					{
						m_SendLog.AppendFormat("\nEncrypt Failed!!Erorr Code:%d !UUID:%s",errcode,SECERTKEY); 
						isRun=false;
						goto cleanup; 
					} 
				}else
				{
					m_RQ_SendPart_body.nPartSize=nSize;
				}
				//m_RQ_SendPart_body.nPartSize=nSize;
			}
			m_RQ_SendPart_body.nOffset=ii; //文件偏移
			
			//m_RQ_SendPart_body.nCompress_Size=zlen;
			strcpy_s(m_RQ_SendPart_body.strFileID,pFileid);  //文件ID  
			pbody=m_RQ_SendPart_body.getNetBuf();
			nbodylen=m_RQ_SendPart_body.getNetBufLen(); 
			//////////////////////////////////////////////////////////////////////////
			//请求包头
			//////////////////////////////////////////////////////////////////////////
			m_RQ_SendPart_head.clear(); 
			m_RQ_SendPart_head.nMsgId=MSGID_SEND_PART; //切片上传 
			strcpy_s(m_RQ_SendPart_head.sUserId,sUserId); //用户信息
			m_RQ_SendPart_head.nPkgSize=nbodylen; 
			phead=m_RQ_SendPart_head.getNetBuf();
			n_hlen=m_RQ_SendPart_head.getNetBufLen();   
			if(!SendBuffer(client,phead,n_hlen)){
				m_SendLog.Append("\n上传分片包头失败!");
				isRun=false;
				goto cleanup; 
			}
			
			if(!SendBuffer(client,pbody,nbodylen)){
				m_SendLog.Append("\n上传分片包体失败!");
				isRun=false;
				goto cleanup; 
			}
			//////////////////////////////////////////////////////////////////////////
			//初始化应答
			////////////////////////////////////////////////////////////////////////// 
			//应答
			char * tbufHead=NULL,*tbufBody=NULL;
			tbufHead=(char*)malloc(MSG_RET_COMMON_HEAD_LEN);
			if(!RecvBuffer(client, tbufHead, MSG_RET_COMMON_HEAD_LEN)){
				icous=1;
				isRun=false;
				goto cleanup; 
			} 
			mTrecvHead.setNetBuf(tbufHead,MSG_RET_COMMON_HEAD_LEN);  
			//LogFile(mTrecvHead.getNetBuf(),mTrecvHead.getNetBufLen());
			if(!mTrecvHead.bResult){
				
				//达到续传尝试次数限制？ 
				RecvError(mTrecvHead.nPkgSize); 
				icous=1;
				isRun=false;
				goto cleanup; 
			}
			
			tbufBody=(char*)malloc(mTrecvHead.nPkgSize);
			if(!RecvBuffer(client, tbufBody, mTrecvHead.nPkgSize)){
				isRun=false;
				icous=1;
				goto cleanup; 
			} 
			mTrecvbody.setNetBuf( tbufBody, mTrecvHead.nPkgSize);
			free(tbufHead);
			 
			free(tbufBody);  
			 
			
			if(m_fileSeek>=fileSize){ //判断
				isRun=true;
				goto cleanup; 
			}
		}
		catch (CFileException *e)
		{
			m_ErrMSG.Append(e->m_strFileName);
			e->Delete();
			DatFile.Abort();
			isRun=false;
			goto cleanup; 
		} 
		//空文件
		if (nSize <= 0){
			isRun=false;
			goto cleanup; 
		}
		// 真实数据长度
		ii++;
		//m_nOffset=ii;
	}while(true);  
	
cleanup: 
	//m_nOffset=ii+icous; 
	//m_nOffset=mTrecvbody.nOffset+1;  //从错误的一个切片开始
	//fb.Format("\nPartSize:%d\nOffset:%d\nReSendPartCount:%d",nPartSize,m_nOffset,ii);
	//m_SendLog.Append(fb); 
	DatFile.Close();
	if(isRun)
	{ 
		Progress(100);
	}
	return isRun;
}
BOOL FileUploadThread::SendCompleteFile(char * pFileId)
{
	CString fd;
	BOOL updateCompleteFile=TRUE;
	//请求数据包数据定义
	char * bufHead=NULL,*bufBody=NULL;
	int bufHeadLen,bufBodyLen;
	MSG_REQ_COMMON_HEAD head1;
	MSG_REQ_SEND_COMPLETE_BODY body1;

	//响应数据包数据定义
	bufHead=NULL;
	bufBody=NULL;
	char * bufHead2=NULL,*bufBody2=NULL;
	MSG_RET_COMMON_HEAD head2;
	MSG_RET_SEND_COMPLETE_BODY body2; 

	//设置包体
	body1.clear();
	strcpy_s(body1.strFileID,pFileId);
	//编码
	bufBody = body1.getNetBuf();
	bufBodyLen = body1.getNetBufLen();

	//设置包头
	head1.clear();
	head1.nPkgSize=bufBodyLen;
	head1.nMsgId = MSGID_SEND_COMPLETE;
	head1.bUserType = '0';
	strcpy_s(head1.sUserId,sUserId);
	//编码
	bufHead = head1.getNetBuf();
	bufHeadLen = head1.getNetBufLen();
	if(!SendBuffer(client,bufHead,bufHeadLen))
	{	
		updateCompleteFile= FALSE;
		goto cleanup;
	}
	if(!SendBuffer(client,bufBody,bufBodyLen))
	{
		updateCompleteFile= FALSE;
		goto cleanup;
	}
	/*if(m_MD5duration>2)
	{*/
		//Sleep((int)m_MD5duration);
		
		//fd.Format("%0.3f",m_MD5duration);
		//LogFile(fd,fd.GetLength());
	//}
	bufHead2=(char*)malloc(MSG_RET_COMMON_HEAD_LEN);
	if(!RecvBuffer(client, bufHead2, MSG_RET_COMMON_HEAD_LEN))
	{
		updateCompleteFile= FALSE;
		goto cleanup;
	}
	head2.setNetBuf(bufHead2,MSG_REQ_COMMON_HEAD_LEN); 

	if(!head2.bResult)
	{
		RecvError(head2.nPkgSize); 
		updateCompleteFile= FALSE;
		goto cleanup; 
	}
	bufBody2=(char*)malloc(head2.nPkgSize);
	if(!RecvBuffer(client, bufBody2, head2.nPkgSize))
	{
		updateCompleteFile= FALSE;
		goto cleanup;
	}
	body2.setNetBuf(bufBody2,head2.nPkgSize); 
	m_OutPutFileLocation=body2.strFileLocation; 
	updateCompleteFile=TRUE;
cleanup: 

	free(bufHead2); 
	 
	free(bufBody2); 
	 
	//m_ErrMSG.Append(body2.strFileLocation);
	return updateCompleteFile;
}
BOOL FileUploadThread::RecvError(long nlength)
{
	BOOL rend=FALSE; 
	char *tBufBody=NULL;
	MSG_RET_ERROR_BODY  msgbody; 
	tBufBody=(char *)malloc(nlength); 
	if(!RecvBuffer(client, tBufBody, nlength))
	{
		rend=FALSE;
		goto cleanup;
	}
	
	msgbody.setNetBuf(tBufBody,nlength);  
	m_ErrMSG.Append("\n");
	m_ErrMSG.Append(msgbody.strErrorMsg); 
	m_SendLog.Append("\n");
	m_SendLog.Append(msgbody.strErrorMsg);
cleanup: 
	free(tBufBody);
	return rend;
}
//////////////////////////////////////////////////////////////////////////
//是否不能继续上传
BOOL FileUploadThread::isHellError(int errid)
{
	switch(errid)
	{
	case ERRORID_FILE_CHANGED : //文件已变更
		return TRUE; 
	case ERRORID_FILE_SAVE_ERROR :// 文件保存失败
		return TRUE;
	case ERRORID_INVALID_TOKEN : //无效令牌
		return TRUE;   
	}
	 
	return FALSE;
}
void FileUploadThread::LogFile(const char * logmsg,int logstrlen)
{
	CFile log;
	log.Open("c:/log.txt",CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
	log.Write(logmsg,logstrlen);
	log.Close();
}
void FileUploadThread::getIPORPort()
{ 
	char * dd=NULL;
	dd=(char *)malloc(500);
	strcpy(dd,m_InputToken);
	dd[m_InputToken.GetLength()]='\0';
	FileToken ftk(dd);
	m_ConnIP=ftk.getIP();
	m_ConnPort=ftk.getPort();
	free(dd); 
}
bool FileUploadThread::InitUpload(void)
{ 
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	//m_fileMd5="";
	Progress(-1);
	//////////////////////////////////////////////////////////////////////////
	//参数初始化
	//////////////////////////////////////////////////////////////////////////
	m_ErrMSG="";
	m_OutPutFileLocation=""; 
	m_inCanel=false;   
	bool comp=TRUE;  ///文件传输的过程是否正常完成
	m_FileName="";
	m_SendLog="";  
	//////////////////////////////////////////////////////////////////////////
	


	getIPORPort(); 
	int nOffset=1; //文件偏移
	int nOrgSize=0; //文件大小
	struct _stat insize;
	_stat(m_InputFilePath,&insize);
	nOrgSize=insize.st_size;  
	
	m_fileSize=nOrgSize; 

	//初始化应答声明
	char * tbufHead=NULL,*tbufBody=NULL; 
	MSG_RET_COMMON_HEAD thead1;
	MSG_RET_INIT_BODY tbody1;
	
	if(m_InputFilePath.IsEmpty())
	{
		comp=FALSE;
		m_ErrMSG="无效的文件路径！";
		goto cleanup;
	} 

	if(nOrgSize>MAX_FILE_SIZE)
	{
		comp=FALSE;
		m_ErrMSG="超过文件最大限制，最大限制500MB";
		goto cleanup;
	}

	//////////////////////////////////////////////////////////////////////////
	// 初始化连接
	////////////////////////////////////////////////////////////////////////// 
	int reconncount=0;
reconnect:	
	if(reconncount>MAX_RECONNECT_NUM || m_inCanel)  
	{
		m_ErrMSG="\n达到重联尝试次数限制！";
	    m_SendLog.Append("\n达到重联尝试次数限制！");	
		comp=FALSE;
		goto cleanup;
	
	}
	//if(!InitSocket(client,m_ConnIP,m_ConnPort))
	if (!Connect(client, m_ConnIP, (short)m_ConnPort))
	{
		if(reconncount<MAX_RECONNECT_NUM)  //限制重新连接次数
		{
			reconncount++;
			goto reconnect;
		}else
		{
			m_ErrMSG="与文件服务器连接失败！网络不通！"; 
			comp=FALSE; 
			goto cleanup; 
		}
		
	}
	
	if(!SendInit(nOrgSize))
	{
		m_ErrMSG="\n发送初始化失败！"; 
		m_SendLog.Append("\n发送初始化失败！");
		comp=FALSE;
		goto cleanup;
	}
	
	//////////////////////////////////////////////////////////////////////////
	//初始化应答
	////////////////////////////////////////////////////////////////////////// 
	tbufHead=(char*)malloc(MSG_RET_COMMON_HEAD_LEN);
	if(!RecvBuffer(client, tbufHead, MSG_RET_COMMON_HEAD_LEN))
		goto cleanup;  
	thead1.setNetBuf(tbufHead,MSG_REQ_COMMON_HEAD_LEN);  
	tbufBody=(char*)malloc(thead1.nPkgSize);
	m_MsgState=MSG_STATE_INIT;
	if(!thead1.bResult)
	{
		//接受错误重新连接
		comp=RecvError(thead1.nPkgSize); 
		goto cleanup;
	}
	m_MsgState=MSG_STATE_SENDPARTDATA;
	if(!RecvBuffer(client,tbufBody, thead1.nPkgSize))
	{
		m_ErrMSG="\n接受初始化包体失败！";
		m_SendLog.Append("\n接受初始化包体失败！");
		comp=FALSE;
		goto cleanup;
	}
	tbody1.setNetBuf(tbufBody,thead1.getNetBufLen()); 
	m_isCompress=tbody1.bCompress;
	m_isEncrypt=tbody1.bEncrypt;
	
	//////////////////////////////////////////////////////////////////////////
	//tbody1.strMinVersion
	free(tbufHead);
 
	free(tbufBody); 
	 
	strcpy_s(sUserId,thead1.sUserId);   
	m_FileName=tbody1.strFileID;
	
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////添加握手步骤///////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	if(m_isEncrypt)
	{
		/////////////////////////////写临时证书/////////////////////////////////////////////
		char  p_dest[MAX_PATH];   //临时文件路径 
		::GetTempPath(250,p_dest);
		strcat(p_dest,"Info.cer");
		strcpy_s(CERINFOPATH,p_dest);
		FILE * tempfs;
		tempfs=fopen(p_dest,"wb+");
		fwrite(tbody1.nCertBuf,tbody1.nCertSize,1,tempfs);
		fclose(tempfs);
		///发送握手请求
		if(!Send_Init_ShakeHands())
		{
			comp=FALSE;
			goto cleanup; 
		}
	}
	//strcpy(m_FileName,tbody1.strFileID);
	//////////////////////////////////////////////////////////////////////////
	//文件切片上传
	////////////////////////////////////////////////////////////////////////// 
	
	if(!SendPacketGetpart( nOffset,nOrgSize,tbody1.nPartSize,tbody1.strFileID))
	{ 
		if(m_inCanel) //取消的上传直接退出。不重试
		{
			comp=FALSE;
			goto cleanup; 
		}
		//m_ErrMSG="\n重新连接,开始续传...";  
		if(!ReConnectSend(nOrgSize,tbody1.nPartSize,tbody1.strFileID))
		{
			m_ErrMSG="\n续传失败..";
			m_SendLog.Append("\n续传失败");
			comp=FALSE;
			goto cleanup;
		} 
		comp=true;
	}
	
	//////////////////////////////////////////////////////////////////////////
	//文件上传完成请求
	//////////////////////////////////////////////////////////////////////////
	if(!SendCompleteFile(tbody1.strFileID)) {
		
		m_ErrMSG="\n文件上传完成请求失败！";
		m_SendLog.Append("\n文件上传完成请求失败！");
		comp=FALSE;
		goto cleanup;
	}
	 m_MsgState=MSG_STATE_SENDCOMPTELE;
	comp=true;
	//m_OutputParameter=pReqGetpartBody->pData;  
cleanup: 
	
	if(!comp)
	{
		//只在上传文件过程才删除文件
		if(m_MsgState>1)
		{
			if(!SendFailed(tbody1.strFileID))
			{
				m_ErrMSG.Append("\n上传失败删除文件");
				m_SendLog.Append("\n上传失败删除文件");
			}
		}
		
		if(m_inCanel)
		{
			this->pCallBack->m_ErrorCode=-1;
			this->pCallBack->OnCanel();
		}else
		{ 
			this->pCallBack->m_ErrorCode=-2;
			comp=false;
			this->pCallBack->OnFailed();
		}
	}
	CloseSocket(); 
	return comp;
}
bool FileUploadThread::SendFailed(char * pFileId)
{ 
	bool isdel=TRUE;
	if (client == INVALID_SOCKET)
	{
		return isdel; 
	}
	
	char * bufHead=NULL,*bufBody=NULL;
	int bufHeadLen,bufBodyLen;
	MSG_REQ_COMMON_HEAD head1;
	MSG_REQ_SEND_COMPLETE_BODY body1;
	char * bufHead2=NULL,*bufBody2=NULL;
	MSG_RET_COMMON_HEAD head2; 
	//设置包体
	body1.clear(); 
	strcpy_s(body1.strFileID,pFileId); 
	//编码
	bufBody = body1.getNetBuf();
	bufBodyLen = body1.getNetBufLen();
	
	//设置包头
	head1.clear();
	head1.nPkgSize=bufBodyLen;
	head1.nMsgId = MSGID_SEND_FAILED;
	head1.bUserType = '0';
	strcpy_s(head1.sUserId,sUserId);
	//编码
	bufHead = head1.getNetBuf();
	bufHeadLen = head1.getNetBufLen();
	
	if(!SendBuffer(client,bufHead,bufHeadLen))
	{
		//m_ErrMSG.Append("\n1");
		isdel= FALSE;
		goto cleanup;
	}
	if(!SendBuffer(client,bufBody,bufBodyLen))
	{
		//m_ErrMSG.Append("\n2");
		isdel= FALSE;
		goto cleanup;
	}
	
	bufHead2=(char*)malloc(MSG_RET_COMMON_HEAD_LEN);
	if(!RecvBuffer(client, bufHead2, MSG_RET_COMMON_HEAD_LEN))
	{
		//m_ErrMSG.Append("\n3");
		isdel= FALSE;
		goto cleanup;
	}
	head2.setNetBuf(bufHead2,MSG_REQ_COMMON_HEAD_LEN); 

	if(!head2.bResult)
	{ 
		RecvError(head2.nPkgSize); 
		//m_ErrMSG.Append("\n4");
		isdel= FALSE;
		goto cleanup; 
	}
	bufBody2=(char*)malloc(head2.nPkgSize);
	if(!RecvBuffer(client, bufBody2, head2.nPkgSize))
	{
		//m_ErrMSG.Append("\n5");
		isdel= FALSE;
		goto cleanup;
	}  
cleanup: 
	free(bufHead2);
	 
	free(bufBody2);
	 
	return isdel; 
} 
bool FileUploadThread::CloseSocket()
{
	// 关闭连接
	if (client != INVALID_SOCKET)
	{
		closesocket(client);
		client = INVALID_SOCKET;
		BOOL bDontLinger = FALSE;
		setsockopt(client,SOL_SOCKET,SO_DONTLINGER,(const char*)&bDontLinger,sizeof(BOOL));
	}
	// 连接中断，尝试重连
	/*if (WaitForSingleObject(hStopServerEvent, 0) != WAIT_OBJECT_0)
	{
	char strMsg[MSG_BUFSIZE]; 
	sprintf(strMsg, "Wait %d seconds and try to reconnect server ...", LOGON_RETRY_INTERVAL);
	m_SendLog.AppendFormat(strMsg, MSGLEVEL_INFO);
	if (WaitForSingleObject(hStopServerEvent, LOGON_RETRY_INTERVAL*1000) != WAIT_OBJECT_0)
	return true;
	}*/
	m_SendLog.Append("\nClose Socket Connected!");
	return true;
}
bool FileUploadThread::SendCanel()
{
	CloseSocket(); 
	m_inCanel=true;
	return true;
}