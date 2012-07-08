#include <afx.h>											// MFC
#include "..\..\..\Common\InfoStruct.h"

//���Գ�ʼ����Ϣ
int test_tcp_init() 
{ 
	CFile file;
	char * bufHead,*bufBody;
	int bufHeadLen,bufBodyLen;
	MSG_REQ_COMMON_HEAD head1,head2;
	MSG_REQ_INIT_BODY body1,body2;
	//���ð���
	body1.clear();
	body1.bFileCheckType = '0';
	strcpy(body1.strVersion,"1.0.0");
	body1.nFileSize = 9;
	body1.nFileCheckSize = 10;
	strcpy(body1.strFileCheck,"1234567890");
	body1.nTokenSize = 5;
	strcpy(body1.strToken,"54321");
	//����
	bufBody = body1.getNetBuf();
	bufBodyLen = body1.getNetBufLen();

	//���ð�ͷ
	head1.clear();
	head1.nPkgSize=bufBodyLen;
	head1.nMsgId = MSGID_INIT;
	head1.bUserType = '0';
	//����
	bufHead = head1.getNetBuf();
	bufHeadLen = head1.getNetBufLen();
	if(!file.Open("tcp_init.dat",CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		return 1;
	file.Write(bufHead,bufHeadLen);
	file.Write(bufBody,bufBodyLen);
	file.Close();

	head2.setNetBuf(bufHead,bufHeadLen);
	body2.setNetBuf(bufBody,bufBodyLen);
	return 0; 
}  

//����������ʼ��
int test_tcp_init_cont() 
{ 
	CFile file;
	char * bufHead,*bufBody;
	int bufHeadLen,bufBodyLen;
	MSG_REQ_COMMON_HEAD head1,head2;
	MSG_REQ_INIT_CONT_BODY body1,body2;
	//���ð���
	body1.clear();
	strcpy(body1.strVersion,"1.0.0");
	strcpy(body1.strFileID,"zpwangtest");
	body1.nFileSize = 9;
	body1.bFileCheckType = '0';
	body1.nFileCheckSize = 10;
	strcpy(body1.strFileCheck,"1234567890");
	body1.nTokenSize = 5;
	strcpy(body1.strToken,"54321");
	//����
	bufBody = body1.getNetBuf();
	bufBodyLen = body1.getNetBufLen();

	//���ð�ͷ
	head1.clear();
	head1.nPkgSize=bufBodyLen;
	head1.nMsgId = MSGID_INIT_CONT;
	head1.bUserType = '0';
	strcpy(head1.sUserId,"zpwang");
	//����
	bufHead = head1.getNetBuf();
	bufHeadLen = head1.getNetBufLen();
	if(!file.Open("tcp_init_cont.dat",CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		return 1;
	file.Write(bufHead,bufHeadLen);
	file.Write(bufBody,bufBodyLen);
	file.Close();

	head2.setNetBuf(bufHead,bufHeadLen);
	body2.setNetBuf(bufBody,bufBodyLen);
	return 0; 
}  

//���Է�Ƭ�ϴ�
int test_tcp_send_part() 
{ 
	CFile file,filesrc;
	char * bufHead,*bufBody;
	int bufHeadLen,bufBodyLen;
	MSG_REQ_COMMON_HEAD head1,head2;
	MSG_REQ_SENDPART_BODY body1,body2;
	char filepart1[65536] = "abcde";
	char filepart2[65536] = "fghi";
	//���ð���
	body1.clear();
	strcpy(body1.strFileID,"zpwangtest");
	body1.nOffset = 1;
	body1.nPartSize = 45568;
	strcpy(body1.pData,filepart1);
		//����
	bufBody = body1.getNetBuf();
	bufBodyLen = body1.getNetBufLen();

	//���ð�ͷ
	head1.clear();
	head1.nPkgSize=45612;
	head1.nMsgId = MSGID_SEND_PART;
	head1.bUserType = '0';
	strcpy(head1.sUserId,"zpwang");
	//����
	bufHead = head1.getNetBuf();
	bufHeadLen = head1.getNetBufLen();
	if(!file.Open("tcp_send_part1.dat",CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		return 1;
	file.Write(bufHead,bufHeadLen);
	file.Write(bufBody,bufBodyLen);
	file.Close();

	head2.setNetBuf(bufHead,bufHeadLen);
	body2.setNetBuf(bufBody,bufBodyLen);
	return 0; 
}  

//�����ϴ����
int test_tcp_send_complete() 
{ 
	CFile file,filesrc;
	char * bufHead,*bufBody;
	int bufHeadLen,bufBodyLen;
	MSG_REQ_COMMON_HEAD head1,head2;
	MSG_REQ_SEND_COMPLETE_BODY body1,body2;
	//���ð���
	body1.clear();
	strcpy(body1.strFileID,"zpwangtest");
	//����
	bufBody = body1.getNetBuf();
	bufBodyLen = body1.getNetBufLen();

	//���ð�ͷ
	head1.clear();
	head1.nPkgSize=bufBodyLen;
	head1.nMsgId = MSGID_SEND_COMPLETE;
	head1.bUserType = '0';
	strcpy(head1.sUserId,"zpwang");
	//����
	bufHead = head1.getNetBuf();
	bufHeadLen = head1.getNetBufLen();
	if(!file.Open("tcp_send_complete.dat",CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		return 1;
	file.Write(bufHead,bufHeadLen);
	file.Write(bufBody,bufBodyLen);
	file.Close();

	head2.setNetBuf(bufHead,bufHeadLen);
	body2.setNetBuf(bufBody,bufBodyLen);
	return 0; 
}  

int test_tcp_send_complete_ret() 
{ 
	CFile file,filesrc;
	char * bufHead,*bufBody;
	int bufHeadLen,bufBodyLen;
	MSG_RET_COMMON_HEAD head1,head2;
	MSG_RET_SEND_COMPLETE_BODY body1,body2;
	//���ð���
	body1.clear();
	strcpy(body1.strFileID,"zpwangtest");
	strcpy(body1.strFileLocation,"20120506");
	//����
	bufBody = body1.getNetBuf();
	bufBodyLen = body1.getNetBufLen();

	//���ð�ͷ
	head1.clear();
	head1.nPkgSize=bufBodyLen;
	head1.nMsgId = MSGID_SEND_COMPLETE;
	head1.bUserType = '0';
	strcpy(head1.sUserId,"zpwang");
	//����
	bufHead = head1.getNetBuf();
	bufHeadLen = head1.getNetBufLen();
	if(!file.Open("tcp_send_complete.dat",CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
		return 1;
	file.Write(bufHead,bufHeadLen);
	file.Write(bufBody,bufBodyLen);
	file.Close();

	head2.setNetBuf(bufHead,bufHeadLen);
	body2.setNetBuf(bufBody,bufBodyLen);
	return 0; 
}  

int main()
{
	//test_tcp_init();
	//test_tcp_init_cont();
	test_tcp_send_part();
	//test_tcp_send_complete();
	//test_tcp_send_complete_ret();
};