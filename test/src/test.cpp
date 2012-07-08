#include <afx.h>									// MFC
#include<afxwin.h>
#include "../../Common/InfoStruct.h"
#include "../../Common/Util.h"
#include "../../Common/FileToken.h"
#include "../../Common/md5.h"
#include "../../Common/ConfigFile.h"
#include "../../Common/CommonDefine.h"
#include "../../Common/AppConf.h"
#include "../../Common/netcrypt.h"
#include<iostream>
#include <sys/stat.h>
#include "../../Common/zlib.h"  
#include "openssl/err.h"
using namespace std;
//void cipher_dump(const EVP_CIPHER * ctx)
//{
//	//EVP_CIPHER_CTX_cipher() 
//	EVP_CIPHER_key_length( ctx );
//	EVP_CIPHER_block_size( ctx );
//	EVP_CIPHER_iv_length( ctx );
//	EVP_CIPHER_type(ctx );
//	EVP_CIPHER_mode( ctx );
//	EVP_CIPHER_nid( ctx ); 
//} 
///*=============================================================================*/
////  Function name   : calc_cipher_length
////  Description     : �����ı����ܳ���
////  Input           : 
////                    EVP_CIPHER_CTX * ctx ���ܽṹ��
////                    int inlen ����
////  Return type     : int return cipher text length 
///*=============================================================================*/
//int calc_cipher_length(EVP_CIPHER_CTX * ctx,int inlen)
//{
//	int block_size = EVP_CIPHER_CTX_block_size( ctx );
//	int block_cnt = 0;
//	int block_final = 0;
//	int cipher_length = 0;
//
//	if ( inlen < block_size ) {
//		printf( "calc cipher length = %d\n", block_size );
//		return block_size;
//	} 
//	block_cnt = inlen / block_size;
//	cipher_length = block_cnt * block_size;
//
//	block_final = inlen - cipher_length;
//	if ( block_final >= 0 ) {
//		block_final = block_size;
//	}
//	cipher_length += block_final;
//	//printf( "calc cipher length = %d\n", cipher_length );
//	return cipher_length;
//}
///*=============================================================================*/
////  Function name   : do_crypt
////  Description     : ʹ��3DES �ӽ�������
////  Input           : 
////                    uint8_t *in		�������ݻ���
////                    int inlen			�������ݳ���
////                    uint8_t **out		������ݻ���ָ��
////                    int *outlen		������ݳ���
////                    int do_encrypt	�ӽ��ܱ�־(1:����; 0:����)
////  Return type     : int 	success return 0 or return -1  
///*=============================================================================*/
//int do_crypt(const char * pks_des_ede3,const uint8_t *in, const int inlen, uint8_t **out, int *outlen, int do_encrypt )
//{
//	//�����㷨EVP_des_ede3_cbc����
//	cipher_dump( EVP_des_ede3_cbc() );
//	//cipher_dump( EVP_aes_128_cfb128() ); 
//	uint8_t *key = (uint8_t *)pks_des_ede3;
//	int len;
//	EVP_CIPHER_CTX ctx; 
//	uint8_t *outbuf = NULL;
//	uint8_t *pb = NULL;
//	int  retval = 0;
//	int  cipher_length = 0;
//
//	if ( inlen <= 0 || in == NULL ) {
//		return 0;
//	}
//	//��ʼ���ṹ��
//	EVP_CIPHER_CTX_init(&ctx);
//	// ʹ��Triple DES
//	//��ʼ�����ܽṹ��
//	retval = EVP_CipherInit(&ctx, EVP_des_ede3_cbc(), key, NULL, do_encrypt );
//	// ����������峤��
//	if ( do_encrypt ) {
//		cipher_length = calc_cipher_length( &ctx, inlen );
//		outbuf = (uint8_t *)malloc( cipher_length );
//	}
//	else {
//		outbuf = (uint8_t *)malloc( inlen );
//	}
//	pb = outbuf;
//	// evp ���üӽ��ܲ���
//	if (!EVP_CipherUpdate(&ctx, pb, &len, in, inlen)) {
//		/* Error */
//		free( outbuf );
//		EVP_CIPHER_CTX_cleanup(&ctx);
//		return 0;
//	}
//
//	*outlen = len;
//	pb += len;
//	len = 0; 
//	//������ɲ��ͷ�
//	if (!EVP_CipherFinal(&ctx, pb,&len)) {
//		/* Error */
//		free( outbuf );
//		EVP_CIPHER_CTX_cleanup(&ctx);
//		return 0;
//	}
//
//	*outlen += len; 
//	pb = outbuf;
//	// ��Ϊ���������·�����ܺ����ݴ�С����
//	if ( !do_encrypt ) {		
//		if ( inlen != ( *outlen ) ) {
//			pb = (uint8_t *)malloc( *outlen );
//			memcpy( pb, outbuf, *outlen );
//			free( outbuf );
//		}
//	}
//	*out= pb;
//	EVP_CIPHER_CTX_cleanup(&ctx);
//	return 1;
//}
void print(const char *promptStr,unsigned char *data,int len)
{
	int i;
	printf("%s[����=%d]\n",promptStr,len);
	for(i = 0; i < len; i++) printf("%02x", data[i]);
	printf("\n===============\n");
}
void InitOpenSSL()
{
	OpenSSL_add_all_ciphers();//���ؼ����㷨
	OpenSSL_add_all_digests();//����ժҪ�����㷨
}
int do_encrypt(const unsigned char * pks_des_ede3,const uint8_t *in, const int inlen, uint8_t *out, int *outlen)
{
	EVP_CIPHER_CTX ctx;//���������Ķ���
	const int ITERATIVE_ROUND_FOR_KEY=3;//��������
	const EVP_CIPHER *type;//�������Ͷ���
	type=EVP_des_ede3_cbc();   //�������ѡ���㷨����    

	unsigned char key[EVP_MAX_KEY_LENGTH];//��Կ
	unsigned char iv[EVP_MAX_IV_LENGTH];//��ʼ����
	//�������ʹ��MD5���Ҳ�������RSA��PCKS#5�ı�׼
	EVP_BytesToKey(type,//��Կ����
		EVP_md5(),//ժҪ��������
		NULL,
		(const unsigned char *)pks_des_ede3,//���
		(int)strlen((const char *)pks_des_ede3),//�������
		ITERATIVE_ROUND_FOR_KEY,//��������
		key,//�������Կ
		iv //����ĳ�ʼ����
		);
	//���ܳ�ʼ����ctx�Ǽ��������Ķ���
	if (!EVP_EncryptInit(&ctx,type,key,iv))
	{
		 return 0;
	}
	//���������Ķ���
	if(!EVP_EncryptUpdate(&ctx,//���������Ķ���
		out,//���ܺ������
		outlen, //���ܺ�����ݳ���
		(const unsigned char*)in, //Ҫ���ܵ�����
		inlen //Ҫ���ܵ����ݳ���
		))
	{
		return 0;
	}
	int tmp=0;
	tmp=*outlen;
	//��������
	if(!EVP_EncryptFinal(&ctx,out,outlen))
	{
		return 0;
	} 
	outlen+=tmp;
	cout << out <<endl;
	//������������ģ���Ϊ���Ļ�Ҫ����
	EVP_CIPHER_CTX_cleanup(&ctx);
	return 1;
}
int do_decrypt(const unsigned char * pks_des_ede3,const uint8_t *in, const int inlen, uint8_t *out, int *outlen)
{ 
	
	return 1;
}
void testencrypt()
{
	const int ITERATIVE_ROUND_FOR_KEY=3;
	unsigned char key[EVP_MAX_KEY_LENGTH];//��Կ
	unsigned char iv[EVP_MAX_IV_LENGTH];//��ʼ����
	EVP_CIPHER_CTX ctx;//���������Ķ���
	unsigned char out[512+8];
	int outl;
	unsigned char txtAfterDecrypt[512];
	int txtLenAfterDecrypt;

	char simpleText[]="����һ��Openssl���Գ��򣬲��öԳ��㷨��des_ede3_cbc.";
	//����
	const char *passwd="a78b5C";//���ڲ�����Կ�Ŀ����ַ���
	const EVP_CIPHER *type;//�������Ͷ���
	OpenSSL_add_all_ciphers();//���ؼ����㷨
	OpenSSL_add_all_digests();//����ժҪ�����㷨
	printf("������:%s\n",passwd);    
	printf("ԭ��:%s\n",simpleText);
	type=EVP_des_ede3_cbc();   //�������ѡ���㷨����                                                             
	//���ı������в��� ��Կ/����
	//�������ʹ��MD5���Ҳ�������RSA��PCKS#5�ı�׼
	EVP_BytesToKey(type,//��Կ����
		EVP_md5(),//ժҪ��������
		NULL,
		(const unsigned char *)passwd,//���
		(int)strlen(passwd),//�������
		ITERATIVE_ROUND_FOR_KEY,//��������
		key,//�������Կ
		iv //����ĳ�ʼ����
		);
	//���ܳ�ʼ����ctx�Ǽ��������Ķ���
	EVP_EncryptInit(&ctx,type,key,iv);

	int tmp=(int)strlen(simpleText);
	//�����������٣�����ѭ����������
	EVP_EncryptUpdate(&ctx,//���������Ķ���
		out,//���ܺ������
		&outl, //���ܺ�����ݳ���
		(const unsigned char*)simpleText, //Ҫ���ܵ�����
		(int)strlen(simpleText) //Ҫ���ܵ����ݳ���
		);
	tmp=outl;
	//��������
	EVP_EncryptFinal(&ctx,out+outl,&outl);
	outl+=tmp;
	//������������ģ���Ϊ���Ļ�Ҫ����
	EVP_CIPHER_CTX_cleanup(&ctx);

	print("����֮��Ľ����",out,outl);
	//���ܳ�ʼ�����������ͣ���Կ����ʼ��������ͼ���ʱ��ͬ��������ܲ��ܳɹ�
	EVP_DecryptInit(&ctx,type,key,iv);

	EVP_DecryptUpdate(&ctx,//���������Ķ���
		txtAfterDecrypt,   //���ܺ������
		&txtLenAfterDecrypt,//���ܺ�����ݳ���
		out,                //Ҫ���ܵ�����
		outl                //Ҫ���ܵ����ݳ���
		);
	tmp=txtLenAfterDecrypt;
	//��������
	EVP_DecryptFinal(&ctx,txtAfterDecrypt+txtLenAfterDecrypt,&txtLenAfterDecrypt);
	txtLenAfterDecrypt+=tmp;
	EVP_CIPHER_CTX_cleanup(&ctx);
	txtAfterDecrypt[txtLenAfterDecrypt]=0;

	printf("����֮��(����=%d):\n%s\n",txtLenAfterDecrypt,txtAfterDecrypt);  
	getchar();
}
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
	AfxMessageBox(bufBody);
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
	char filepart1[100] = "abcde";
	char filepart2[100] = "fghi";
	//���ð���
	body1.clear();
	strcpy(body1.strFileID,"zpwangtest");
	body1.nOffset = 1;
	body1.nPartSize = 4;
	strcpy(body1.pData,filepart2);
		//����
	bufBody = body1.getNetBuf();
	bufBodyLen = body1.getNetBufLen();

	//���ð�ͷ
	head1.clear();
	head1.nPkgSize=bufBodyLen;
	head1.nMsgId = MSGID_SEND_PART;
	head1.bUserType = '0';
	strcpy(head1.sUserId,"zpwang");
	//����
	bufHead = head1.getNetBuf();
	bufHeadLen = head1.getNetBufLen();
	if(!file.Open("tcp_send_part2.dat",CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
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

CString get_strGUID()
{
	GUID   m_guid; 
	CString   strGUID;   
	if   (S_OK ==::CoCreateGuid(&m_guid))   
	{         
		strGUID.Format("%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X" ,
			m_guid.Data1,  m_guid.Data2,   m_guid.Data3 ,
			m_guid.Data4[0],   m_guid.Data4[1],
			m_guid.Data4[2],   m_guid.Data4[3],
			m_guid.Data4[4],   m_guid.Data4[5],
			m_guid.Data4[6],   m_guid.Data4[7] );   
	}   

	return strGUID;
}
void statmethod()
{

	static int ff=0;
	ff++;
	printf("%d",ff);
}
 //APP_ITEM m_AppItem[MAX_APP_NUM];						// APP���ñ�
CConfigFile *g_pConfigFile = NULL;							// �����ļ� 
AppConf m_AppItem ;
 BOOL LoadAppConfig()
 {
	 char *pTemp = NULL;
	 char strMsg[512] = {0};
	 char APPCONF_FILE[]="C:/work/FileTransfer/FileServer/app.conf";
	 // �����ļ��Ƿ����
	 if (_access(APPCONF_FILE, 0) == -1)
	 {
		 
		 
	 }

	 // �����ļ�����������
	 g_pConfigFile = new CConfigFile();
	 if (!g_pConfigFile)
	 {
		 
	 }
	 g_pConfigFile->Init(APPCONF_FILE);
 
	 // group TASK
	 if (g_pConfigFile->FindFirstGroup("app") != 0)
	 {
		 
	 }
	 
	 int i=0;
	 APP_ITEM *appit=NULL;
	 do
	 { 
		 appit=m_AppItem.getAppItem(i);
		 // �������
		 pTemp = g_pConfigFile->Get("id");
		 if (!pTemp)
		 {
			 
		 }
		 strcpy(appit->nAppId, pTemp);//,strlen(pTemp));

		 // �Ƿ�����(ȱʡ����)
		 pTemp = g_pConfigFile->Get("enable");
		 if (pTemp)
			 appit->nEnable = atoi(pTemp);
		 else
			 appit->nEnable = 1;

		 // ������
		 pTemp = g_pConfigFile->Get("name");
		 if (!pTemp)
		 {
			 
		 }
		 strcpy(appit->strName, pTemp);

		 // ��������
		 pTemp = g_pConfigFile->Get("desc");
		 if (!pTemp)
		 {
			 
		 }
		 strcpy(appit->strDesc, pTemp);

		 // �������˽�ܴ�
		 pTemp = g_pConfigFile->Get("tokenRand");
		 if (!pTemp)
		 {
			 
		 }
		 strcpy(appit->strTokenRand, pTemp); 


		 pTemp = g_pConfigFile->Get("token-timeout");
		 if (!pTemp)
		 {
			 
		 }
		 strcpy(appit->strTokenTimeOut, pTemp); 

		 // ��������
		 pTemp = g_pConfigFile->Get("file-path");
		 if (!pTemp)
		 {
			 
		 }
		 strcpy(appit->strPath, pTemp);   
		 i++;
	 } while (g_pConfigFile->FindNextGroup() == 0);

	 delete g_pConfigFile;
	 g_pConfigFile = NULL;
	 return TRUE; 
return_fail:
	 if (g_pConfigFile)
	 {
		 delete g_pConfigFile;
		 g_pConfigFile = NULL;
	 }
	 return FALSE;
 }
 #define PATH_MAX            512 
#pragma comment(lib,"zlib.lib")
#define DEFAULT_FILE_BLOCK_SIZE         (128*1024)      //Ĭ�Ϸ�Ƭ��С
#define DEFAULT_FILE_BLOCK_SIZE         (128*1024)      //Ĭ�Ϸ�Ƭ��С
 static char privateKey[] = "E:/Plug_in/openssl/sample/openssl_one/Debug/privkey.pfx";
 static char publicKey[] = "E:/Plug_in/openssl/sample/openssl_one/Debug/pubcer.cer";
int main(int argc,char * argv[])
{
	InitOpenSSL();
	unsigned char out[512+8];
	int outl=0; 
	const unsigned char passwd[]="a78b5C";//���ڲ�����Կ�Ŀ����ַ���
	char simpleText[]="����һ��Openssl���Գ��򣬲��öԳ��㷨��des_ede3_cbc.";
	cout << do_encrypt(passwd,(uint8_t*)simpleText,strlen(simpleText),(uint8_t*)out,&outl) <<endl;


	testencrypt();

	char data[DEFAULT_FILE_BLOCK_SIZE*2];
	int errcode=0;
	uint8_t* encryptData=NULL,*dedata=NULL;
	int encryptlen=0,delen=0;
	char uuidsource[MAX_SECERTKEY_LEN+1];
	char *p_en=do_encryptSecertKey(publicKey,uuidsource); 
	char *fff=do_decryptSecertKey(privateKey,"111111",p_en); 

	CFile DatFile; 
	if (!DatFile.Open("C:/a.txt", CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary))
	{
		  return 0;
	}
	int file=0;
	do
	{ 

		int nSize = (DWORD)DatFile.Read(data, DEFAULT_FILE_BLOCK_SIZE); 
		file+=nSize;
		char srcData[MAX_FILE_BLOCK_SIZE*2]; 
		DWORD zlen=MAX_FILE_BLOCK_SIZE*2; ///ѹ����С 
		///ѹ��
		errcode=compress((BYTE *)srcData,&zlen, (BYTE *)data,nSize );
		if ( errcode!= Z_OK) 
		{ 
			cout << "\nCompress Failed!!Erorr Code:" <<errcode <<endl; 
		}

		

		errcode=do_crypt(uuidsource,(uint8_t*)srcData,zlen,&encryptData,&encryptlen,1);
		if(!errcode)
			cout << "failed!" <<endl;
	   //��������
	    memcpy(data,encryptData,encryptlen);  
		
	    errcode=do_crypt(fff,(uint8_t*)data,encryptlen,(uint8_t**)srcData,(int *)zlen,0);
	    if(!errcode)
		    cout << "failed!" <<endl;
	
	  encryptData=NULL;
	  errcode=uncompress( (BYTE *)encryptData,(uLongf*)encryptlen ,(BYTE *)encryptData,zlen);

	  if ( errcode!= Z_OK) 
		  cout << "failed!" <<endl;
	  //����encryptlen
		/*else
		cout << dedata <<endl;*/

		if(file>=DatFile.GetLength())
			break;

	}while(true);
	cout << "finish" <<endl;
	//char data[]="���Ǹ����ռ�adsf�������ǿ������ҿ�˧���Ӹ��ƴ����ṹ��������Ŀ�������ֵ������ۿ��˼�������͹��޴�ۻ���м����ǿ��˺ü���ĸ�����˰�ļ��ʿ��ֵ����Ʒ�����ʥ������ȿ����׶��ǿ��ػ��ֿ���ɽ��޷䶼�ǻ�Ʒ��ɿ������ݿ����������Կ��Ʒ��ӿ������������¦����ɽ����ٶȿ�ֻ������ϿƼ����������Ӿ練����ʥ���ڿ��ָ����������ػ��ֿ������������ɽ�����������׶����ϿƼ��������ٶȼӿ�������������ȿ����׶���ɢ�ռ䷴�������������¦����ɽ�������Ϳ��˵��Ӿ縣»����������ˮ�翪��������ɽ��޷俼�ǵ����ݿ���罣���ο������ۿ��˾��Ǵ��˽��ۿ��˴����ṹ˹�´���ֿ��ǵ����ݿ����ֵ�������տ�ʼ�Ľ��ۿ���ʥ������ṥ�����������ϵĿո����¼�����ο������ۿ��˵��о��ʹۿ����Ӿ������յ���Ʒ��ɽ��ۿ����Ĵ��տ�¼�����ϸ�¥�Ƽ�ʱ�����м��տ�ʼ�Ľṹ�Ϳ��־ͺÿ��˵��Ӿ�񻶾Ƶ�������Ŷ�ɽ���ѻ����ٶȿ콿�ۿ��˽��������ʡ��һ������ʳ�ͺÿ��˵��Ӿ�ÿ�¦����ɽ���ѻ������ǿ��־ͺÿɴ�ɭ�ֻ��ῴ�˵��Ӿ�ÿ������߼��������䷽ɱ�����������ӿڵ��Ǹ����ռ�adsf�������ǿ������ҿ�˧���Ӹ��ƴ����ṹ��������Ŀ�������ֵ������ۿ��˼�������͹��޴�ۻ���м����ǿ��˺ü���ĸ�����˰�ļ��ʿ��ֵ����Ʒ�����ʥ������ȿ����׶��ǿ��ػ��ֿ���ɽ��޷䶼�ǻ�Ʒ��ɿ������ݿ����������Կ��Ʒ��ӿ������������¦����ɽ����ٶȿ�ֻ������ϿƼ����������Ӿ練����ʥ���ڿ��ָ����������ػ��ֿ������������ɽ�����������׶����ϿƼ��������ٶȼӿ�������������ȿ����׶���ɢ�ռ䷴�������������¦����ɽ�������Ϳ��˵��Ӿ縣»����������ˮ�翪��������ɽ��޷俼�ǵ����ݿ���罣���ο������ۿ��˾��Ǵ��˽��ۿ��˴����ṹ˹�´���ֿ��ǵ����ݿ����ֵ�������տ�ʼ�Ľ��ۿ���ʥ������ṥ�����������ϵĿո����¼�����ο������ۿ��˵��о��ʹۿ����Ӿ������յ���Ʒ��ɽ��ۿ����Ĵ��տ�¼�����ϸ�¥�Ƽ�ʱ�����м��տ�ʼ�Ľṹ�Ϳ��־ͺÿ��˵��Ӿ�񻶾Ƶ�������Ŷ�ɽ���ѻ����ٶȿ콿�ۿ��˽��������ʡ��һ������ʳ�ͺÿ��˵��Ӿ�ÿ�¦����ɽ���ѻ������ǿ��־ͺÿɴ�ɭ�ֻ��ῴ�˵��Ӿ�ÿ������߼��������䷽ɱ�����������ӿڵ��Ǹ����ռ�adsf�������ǿ������ҿ�˧���Ӹ��ƴ����ṹ��������Ŀ�������ֵ������ۿ��˼�������͹��޴�ۻ���м����ǿ��˺ü���ĸ�����˰�ļ��ʿ��ֵ����Ʒ�����ʥ������ȿ����׶��ǿ��ػ��ֿ���ɽ��޷䶼�ǻ�Ʒ��ɿ������ݿ����������Կ��Ʒ��ӿ������������¦����ɽ����ٶȿ�ֻ������ϿƼ����������Ӿ練����ʥ���ڿ��ָ����������ػ��ֿ������������ɽ�����������׶����ϿƼ��������ٶȼӿ�������������ȿ����׶���ɢ�ռ䷴�������������¦����ɽ�������Ϳ��˵��Ӿ縣»����������ˮ�翪��������ɽ��޷俼�ǵ����ݿ���罣���ο������ۿ��˾��Ǵ��˽��ۿ��˴����ṹ˹�´���ֿ��ǵ����ݿ����ֵ�������տ�ʼ�Ľ��ۿ���ʥ������ṥ�����������ϵĿո����¼�����ο������ۿ��˵��о��ʹۿ����Ӿ������յ���Ʒ��ɽ��ۿ����Ĵ��տ�¼�����ϸ�¥�Ƽ�ʱ�����м��տ�ʼ�Ľṹ�Ϳ��־ͺÿ��˵��Ӿ�񻶾Ƶ�������Ŷ�ɽ���ѻ����ٶȿ콿�ۿ��˽��������ʡ��һ������ʳ�ͺÿ��˵��Ӿ�ÿ�¦����ɽ���ѻ������ǿ��־ͺÿɴ�ɭ�ֻ��ῴ�˵��Ӿ�ÿ������߼��������䷽ɱ�����������ӿڵ��Ǹ����ռ�adsf�������ǿ������ҿ�˧���Ӹ��ƴ����ṹ��������Ŀ�������ֵ������ۿ��˼�������͹��޴�ۻ���м����ǿ��˺ü���ĸ�����˰�ļ��ʿ��ֵ����Ʒ�����ʥ������ȿ����׶��ǿ��ػ��ֿ���ɽ��޷䶼�ǻ�Ʒ��ɿ������ݿ����������Կ��Ʒ��ӿ������������¦����ɽ����ٶȿ�ֻ������ϿƼ����������Ӿ練����ʥ���ڿ��ָ����������ػ��ֿ������������ɽ�����������׶����ϿƼ��������ٶȼӿ�������������ȿ����׶���ɢ�ռ䷴�������������¦����ɽ�������Ϳ��˵��Ӿ縣»����������ˮ�翪��������ɽ��޷俼�ǵ����ݿ���罣���ο������ۿ��˾��Ǵ��˽��ۿ��˴����ṹ˹�´���ֿ��ǵ����ݿ����ֵ�������տ�ʼ�Ľ��ۿ���ʥ������ṥ�����������ϵĿո����¼�����ο������ۿ��˵��о��ʹۿ����Ӿ������յ���Ʒ��ɽ��ۿ����Ĵ��տ�¼�����ϸ�¥�Ƽ�ʱ�����м��տ�ʼ�Ľṹ�Ϳ��־ͺÿ��˵��Ӿ�񻶾Ƶ�������Ŷ�ɽ���ѻ����ٶȿ콿�ۿ��˽��������ʡ��һ������ʳ�ͺÿ��˵��Ӿ�ÿ�¦����ɽ���ѻ������ǿ��־ͺÿɴ�ɭ�ֻ��ῴ�˵��Ӿ�ÿ������߼��������䷽ɱ�����������ӿڵ��Ǹ����ռ�adsf�������ǿ������ҿ�˧���Ӹ��ƴ����ṹ��������Ŀ�������ֵ������ۿ��˼�������͹��޴�ۻ���м����ǿ��˺ü���ĸ�����˰�ļ��ʿ��ֵ����Ʒ�����ʥ������ȿ����׶��ǿ��ػ��ֿ���ɽ��޷䶼�ǻ�Ʒ��ɿ������ݿ����������Կ��Ʒ��ӿ������������¦����ɽ����ٶȿ�ֻ������ϿƼ����������Ӿ練����ʥ���ڿ��ָ����������ػ��ֿ������������ɽ�����������׶����ϿƼ��������ٶȼӿ�������������ȿ����׶���ɢ�ռ䷴�������������¦����ɽ�������Ϳ��˵��Ӿ縣»����������ˮ�翪��������ɽ��޷俼�ǵ����ݿ���罣���ο������ۿ��˾��Ǵ��˽��ۿ��˴����ṹ˹�´���ֿ��ǵ����ݿ����ֵ�������տ�ʼ�Ľ��ۿ���ʥ������ṥ�����������ϵĿո����¼�����ο������ۿ��˵��о��ʹۿ����Ӿ������յ���Ʒ��ɽ��ۿ����Ĵ��տ�¼�����ϸ�¥�Ƽ�ʱ�����м��տ�ʼ�Ľṹ�Ϳ��־ͺÿ��˵��Ӿ�񻶾Ƶ�������Ŷ�ɽ���ѻ����ٶȿ콿�ۿ��˽��������ʡ��һ������ʳ�ͺÿ��˵��Ӿ�ÿ�¦����ɽ���ѻ������ǿ��־ͺÿɴ�ɭ�ֻ��ῴ�˵��Ӿ�ÿ������߼��������䷽ɱ�����������ӿڵ��Ǹ����ռ�adsf�������ǿ������ҿ�˧���Ӹ��ƴ����ṹ��������Ŀ�������ֵ������ۿ��˼�������͹��޴�ۻ���м����ǿ��˺ü���ĸ�����˰�ļ��ʿ��ֵ����Ʒ�����ʥ������ȿ����׶��ǿ��ػ��ֿ���ɽ��޷䶼�ǻ�Ʒ��ɿ������ݿ����������Կ��Ʒ��ӿ������������¦����ɽ����ٶȿ�ֻ������ϿƼ����������Ӿ練����ʥ���ڿ��ָ����������ػ��ֿ������������ɽ�����������׶����ϿƼ��������ٶȼӿ�������������ȿ����׶���ɢ�ռ䷴�������������¦����ɽ�������Ϳ��˵��Ӿ縣»����������ˮ�翪��������ɽ��޷俼�ǵ����ݿ���罣���ο������ۿ��˾��Ǵ��˽��ۿ��˴����ṹ˹�´���ֿ��ǵ����ݿ����ֵ�������տ�ʼ�Ľ��ۿ���ʥ������ṥ�����������ϵĿո����¼�����ο������ۿ��˵��о��ʹۿ����Ӿ������յ���Ʒ��ɽ��ۿ����Ĵ��տ�¼�����ϸ�¥�Ƽ�ʱ�����м��տ�ʼ�Ľṹ�Ϳ��־ͺÿ��˵��Ӿ�񻶾Ƶ�������Ŷ�ɽ���ѻ����ٶȿ콿�ۿ��˽��������ʡ��һ������ʳ�ͺÿ��˵��Ӿ�ÿ�¦����ɽ���ѻ������ǿ��־ͺÿɴ�ɭ�ֻ��ῴ�˵��Ӿ�ÿ������߼��������䷽ɱ�����������ӿڵ��Ǹ����ռ�adsf�������ǿ������ҿ�˧���Ӹ��ƴ����ṹ��������Ŀ�������ֵ������ۿ��˼�������͹��޴�ۻ���м����ǿ��˺ü���ĸ�����˰�ļ��ʿ��ֵ����Ʒ�����ʥ������ȿ����׶��ǿ��ػ��ֿ���ɽ��޷䶼�ǻ�Ʒ��ɿ������ݿ����������Կ��Ʒ��ӿ������������¦����ɽ����ٶȿ�ֻ������ϿƼ����������Ӿ練����ʥ���ڿ��ָ����������ػ��ֿ������������ɽ�����������׶����ϿƼ��������ٶȼӿ�������������ȿ����׶���ɢ�ռ䷴�������������¦����ɽ�������Ϳ��˵��Ӿ縣»����������ˮ�翪��������ɽ��޷俼�ǵ����ݿ���罣���ο������ۿ��˾��Ǵ��˽��ۿ��˴����ṹ˹�´���ֿ��ǵ����ݿ����ֵ�������տ�ʼ�Ľ��ۿ���ʥ������ṥ�����������ϵĿո����¼�����ο������ۿ��˵��о��ʹۿ����Ӿ������յ���Ʒ��ɽ��ۿ����Ĵ��տ�¼�����ϸ�¥�Ƽ�ʱ�����м��տ�ʼ�Ľṹ�Ϳ��־ͺÿ��˵��Ӿ�񻶾Ƶ�������Ŷ�ɽ���ѻ����ٶȿ콿�ۿ��˽��������ʡ��һ������ʳ�ͺÿ��˵��Ӿ�ÿ�¦����ɽ���ѻ������ǿ��־ͺÿɴ�ɭ�ֻ��ῴ�˵��Ӿ�ÿ������߼��������䷽ɱ�����������ӿڵ��Ǹ����ռ�adsf�������ǿ������ҿ�˧���Ӹ��ƴ����ṹ��������Ŀ�������ֵ������ۿ��˼�������͹��޴�ۻ���м����ǿ��˺ü���ĸ�����˰�ļ��ʿ��ֵ����Ʒ�����ʥ������ȿ����׶��ǿ��ػ��ֿ���ɽ��޷䶼�ǻ�Ʒ��ɿ������ݿ����������Կ��Ʒ��ӿ������������¦����ɽ����ٶȿ�ֻ������ϿƼ����������Ӿ練����ʥ���ڿ��ָ����������ػ��ֿ������������ɽ�����������׶����ϿƼ��������ٶȼӿ�������������ȿ����׶���ɢ�ռ䷴�������������¦����ɽ�������Ϳ��˵��Ӿ縣»����������ˮ�翪��������ɽ��޷俼�ǵ����ݿ���罣���ο������ۿ��˾��Ǵ��˽��ۿ��˴����ṹ˹�´���ֿ��ǵ����ݿ����ֵ�������տ�ʼ�Ľ��ۿ���ʥ������ṥ�����������ϵĿո����¼�����ο������ۿ��˵��о��ʹۿ����Ӿ������յ���Ʒ��ɽ��ۿ����Ĵ��տ�¼�����ϸ�¥�Ƽ�ʱ�����м��տ�ʼ�Ľṹ�Ϳ��־ͺÿ��˵��Ӿ�񻶾Ƶ�������Ŷ�ɽ���ѻ����ٶȿ콿�ۿ��˽��������ʡ��һ������ʳ�ͺÿ��˵��Ӿ�ÿ�¦����ɽ���ѻ������ǿ��־ͺÿɴ�ɭ�ֻ��ῴ�˵��Ӿ�ÿ������߼��������䷽ɱ�����������ӿڵ��Ǹ����ռ�adsf�������ǿ������ҿ�˧���Ӹ��ƴ����ṹ��������Ŀ�������ֵ������ۿ��˼�������͹��޴�ۻ���м����ǿ��˺ü���ĸ�����˰�ļ��ʿ��ֵ����Ʒ�����ʥ������ȿ����׶��ǿ��ػ��ֿ���ɽ��޷䶼�ǻ�Ʒ��ɿ������ݿ����������Կ��Ʒ��ӿ������������¦����ɽ����ٶȿ�ֻ������ϿƼ����������Ӿ練����ʥ���ڿ��ָ����������ػ��ֿ������������ɽ�����������׶����ϿƼ��������ٶȼӿ�������������ȿ����׶���ɢ�ռ䷴�������������¦����ɽ�������Ϳ��˵��Ӿ縣»����������ˮ�翪��������ɽ��޷俼�ǵ����ݿ���罣���ο������ۿ��˾��Ǵ��˽��ۿ��˴����ṹ˹�´���ֿ��ǵ����ݿ����ֵ�������տ�ʼ�Ľ��ۿ���ʥ������ṥ�����������ϵĿո����¼�����ο������ۿ��˵��о��ʹۿ����Ӿ������յ���Ʒ��ɽ��ۿ����Ĵ��տ�¼�����ϸ�¥�Ƽ�ʱ�����м��տ�ʼ�Ľṹ�Ϳ��־ͺÿ��˵��Ӿ�񻶾Ƶ�������Ŷ�ɽ���ѻ����ٶȿ콿�ۿ��˽��������ʡ��һ������ʳ�ͺÿ��˵��Ӿ�ÿ�¦����ɽ���ѻ������ǿ��־ͺÿɴ�ɭ�ֻ��ῴ�˵��Ӿ�ÿ������߼��������䷽ɱ�����������ӿڵ��Ǹ����ռ�adsf�������ǿ������ҿ�˧���Ӹ��ƴ����ṹ��������Ŀ�������ֵ������ۿ��˼�������͹��޴�ۻ���м����ǿ��˺ü���ĸ�����˰�ļ��ʿ��ֵ����Ʒ�����ʥ������ȿ����׶��ǿ��ػ��ֿ���ɽ��޷䶼�ǻ�Ʒ��ɿ������ݿ����������Կ��Ʒ��ӿ������������¦����ɽ����ٶȿ�ֻ������ϿƼ����������Ӿ練����ʥ���ڿ��ָ����������ػ��ֿ������������ɽ�����������׶����ϿƼ��������ٶȼӿ�������������ȿ����׶���ɢ�ռ䷴�������������¦����ɽ�������Ϳ��˵��Ӿ縣»����������ˮ�翪��������ɽ��޷俼�ǵ����ݿ���罣���ο������ۿ��˾��Ǵ��˽��ۿ��˴����ṹ˹�´���ֿ��ǵ����ݿ����ֵ�������տ�ʼ�Ľ��ۿ���ʥ������ṥ�����������ϵĿո����¼�����ο������ۿ��˵��о��ʹۿ����Ӿ������յ���Ʒ��ɽ��ۿ����Ĵ��տ�¼�����ϸ�¥�Ƽ�ʱ�����м��տ�ʼ�Ľṹ�Ϳ��־ͺÿ��˵��Ӿ�񻶾Ƶ�������Ŷ�ɽ���ѻ����ٶȿ콿�ۿ��˽��������ʡ��һ������ʳ�ͺÿ��˵��Ӿ�ÿ�¦����ɽ���ѻ������ǿ��־ͺÿɴ�ɭ�ֻ��ῴ�˵��Ӿ�ÿ������߼��������䷽ɱ�����������ӿ�";

	//uint8_t* encryptData=NULL,*dedata=NULL;
	//int encryptlen=0,delen=0;
	//int errcode=0;
	////////////////////////////////////////////////////////////////////////////
	////���ܲ��� 
	//clock_t start, finish; 
	//double   duration; 
	///* ����һ���¼�������ʱ��*/ 
	//
	//try
	//{
	//	for (int i =0;i< 10;i++)
	//	{
	//	
	//	start = clock();
	//	char uuidsource[MAX_SECERTKEY_LEN+1];
	//	char *p_en=do_encryptSecertKey(publicKey,uuidsource); 
	//	 
	//	//��������
	//	errcode=do_crypt(uuidsource,(uint8_t*)data,strlen(data),&encryptData,&encryptlen,1);
	//	 
	//	///������Կ
	//	char *fff=do_decryptSecertKey(privateKey,"111111",p_en); 
	// 
	//	//����
	//	errcode=do_crypt(fff,(uint8_t*)encryptData,encryptlen,&dedata,&delen,0);
	//	if(errcode!=0){
	//		cout << "��ȷ��..." << strlen(data) <<endl;
	//		/*cout << dedata <<endl;
	//		cout << uuidsource <<endl;
	//		cout << encryptData <<endl;*/
	//	}else
	//	{
	//		cout << "error..." <<endl;
	//		/*cout << ERR_get_error() <<endl; 
	//		cout << uuidsource <<endl;
	//		cout << encryptData <<endl;*/
	//		 
	//	}
	//	finish = clock(); 
	//	duration = (double)(finish - start) / CLOCKS_PER_SEC; 
	//	printf( "%f seconds\n", duration ); 
	//	}

	//}catch (CException* e)
	//{
	//	return 0;
	//}
	
	//FILE * tempfs;
	//char   *p_dest; 
	//p_dest   =   new   char[250];  
	//::GetTempPath(250,p_dest);
	//strcat(p_dest,"a.cer");
	//cout << p_dest <<endl;
	//
	//char* mySTR="�����Ǳ�д���һ��:�Ұ��л����񹲺͹���"; 
	//tempfs=fopen(p_dest,"wb+");
	//fwrite(mySTR,strlen(mySTR),1,tempfs);
	//fclose(tempfs);
	//struct _stat insize;
	//_stat("C:/abcd.key",&insize);
	//cout << insize.st_size <<endl;	
	// //���Լ���
	//char pData[MAX_FILE_BLOCK_SIZE*2];	
	//CFile file,DatFile1; 
	//int nSize=0;
	//int nPartSize=DEFAULT_FILE_BLOCK_SIZE;
	//if (!file.Open("d:/ddd.doc", CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary ))
	//{
	//}
	//if (!DatFile1.Open("D:/DatFile1.doc", CFile::modeCreate | CFile::modeWrite |CFile::typeBinary))
	//{ 
	//}
	//int errcode=0;
	//do{
	//	nSize = (DWORD)file.Read(pData, nPartSize); 
	//	if(nSize==0)
	//		break;
	//	uint8_t* encryptData=NULL;
	//	uint8_t* unencryptData=NULL;
	//	int unencryptLen=0;
	//	int encryptlen=0;
	//	errcode=do_crypt((uint8_t*)pData,nSize,&encryptData,&encryptlen,1);
	//	if(errcode)
	//	{
	//		errcode=do_crypt((uint8_t*)encryptData,encryptlen,&unencryptData,&unencryptLen,0);
	//		if(errcode)
	//		{
	//			DatFile1.Write(unencryptData,unencryptLen);
	//		}else
	//		{
	//			cout << "Unencrypt Error" <<endl;
	//		}
	//	}else
	//	{
	//		cout << "Error" <<endl;
	//	}
	//}while(true);
	//file.Close();
	//DatFile1.Close();
	//CFile DatFile,DatFile1,datafile2; 
	//if (!DatFile.Open("d:/ddd.doc", CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary ))
	//{ 
	//}
	//if (!DatFile1.Open("D:/DatFile1.doc", CFile::modeCreate | CFile::modeWrite |CFile::typeBinary))
	//{ 
	//} 
	//if (!datafile2.Open("D:/DatFile2.doc", CFile::modeCreate | CFile::modeWrite |CFile::typeBinary))
	//{ 
	//} 
	//int i =1;
	//
	//do{  
	//	char srcData[DEFAULT_FILE_BLOCK_SIZE+1];
	//	char dscData[DEFAULT_FILE_BLOCK_SIZE*2];
	//	char pData[DEFAULT_FILE_BLOCK_SIZE*2];
	//	DWORD  nSize= (DWORD)DatFile.Read(&srcData, DEFAULT_FILE_BLOCK_SIZE);   
	//	datafile2.Write(srcData,nSize); 
	//	
	//	if(nSize==0)
	//		break; 
	//	DWORD zlen=sizeof(pData) * 2; ///ѹ����С
	//	DWORD zlen1=sizeof(pData) * 2; ///ѹ����С 
	//	 
	//	///ѹ��
	//	//if (compress((BYTE *)srcData,&zlen, (BYTE *)m_RQ_SendPart_body.pData,nSize ) != Z_OK)
	//	//if (compress2((Bytef *)pData,(uLongf*)&zlen,  (const Bytef *)srcData,(uLongf)nSize ,9) != Z_OK)
	//	if(i==5)
	//		cout << "i"<<endl;
	//	
	//	int errcode=compress((Bytef *)pData,(uLongf*)&zlen,  (const Bytef *)srcData,(uLongf)nSize);
	//	if (errcode != Z_OK)
	//	{ 
	//		cout << i <<"compress failed!Err:" << errcode <<endl;
	//	} 
	//	//strcpy((char*)dscData, "garbage");
	//	 errcode=uncompress((Bytef *)dscData, (uLongf*)&zlen1,(Bytef *)pData, (uLongf)zlen);
	//	if (errcode != Z_OK)
	//	{
	//		cout << i << "uncompress failed!,Err:" << errcode << endl;
	//	}
	//	/*if(strcmp(srcData,dscData) || nSize!=zlen1)
	//	{
	//	break;
	//	}*/
	//	DatFile1.Write(dscData,zlen1); 
	//	 i++;
	//}while(true);
	//DatFile1.Close();
	//DatFile.Close();
	//cout << "" << i <<endl; 
	// char pDstBuf[521];
	// char pData[521];
	//
	////////////////////////////////////////////////////////////////////////////�����ݿ�
	//char pSrcBuf[]="askdfjlkadsjfklasdjklgjklasdfjg;dfkljh;afdjhk;jad;kjhlfdjl;hkjfd;kjag;kdfjgksdfj;gdflkgjdaf;kjg;fd";
	//DWORD zlen=strlen(pSrcBuf); 
	//if (compress((BYTE *)pDstBuf,&zlen, (BYTE *)pSrcBuf,zlen ) != Z_OK)
	//{
	//	cout << "Compress Failed!" <<endl;
	//}
	//cout << pDstBuf <<endl;
	//unsigned long nOrgSize=98;
	//	cout << "Uncompress Failed!" <<endl;
	//pData[nOrgSize]='\0';
	//cout << "AA" <<endl;
	//char * ff;
	//ff=(char *)malloc(20);
	//strcpy(ff,"ddd");
	//if(ff!=NULL)
	//{
	//	free(ff);
	//	ff=NULL;
	//}
	//cout << "" <<endl;
	/*CFile sekf;
	sekf.Open("c:/abcd.key",CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary);
	sekf.Seek(10,CFile::begin);
	char * fbufstr=NULL;
	fbufstr=(char *)malloc(10);
	int is=sekf.Read(fbufstr,5);
	fbufstr[is]='\0';
	cout << fbufstr <<endl;


	for (int i =0;i<10;i++)
	{
	statmethod();
	}

	LoadAppConfig();
	APP_ITEM *ap=m_AppItem.getAppItem("222");
	cout <<  ap->strTokenRand <<endl;


	float lenprog=(float)75/((float)660252672/(float)131072);
	float progress=23;
	for (int i =0;i<5034;i++)
	{
	progress+=lenprog;
	cout << progress  <<" " ;
	}*/
	/*CString   timestr   =   "20120504104617";  
	long int ff=atol("20120504104617");
	cout << ff <<endl;
	int   a,b,c ,d,e,f  ;   
	sscanf(timestr.GetBuffer(timestr.GetLength()),"%4d%2d%2d%2d%2d%2d",&a,&b,&c,&d,&e,&f);   
	CTime time(a,b,c,d,e,f);   
	CTime curTime=CTime::GetCurrentTime();
	CTimeSpan ts=curTime-time;
	printf("%d day %02d:%02d:%02d", (long)ts.GetDays(), (long)ts.GetHours(), (long)ts.GetMinutes(), (long)ts.GetSeconds()); */
	

	//strcpy(m_AppItem[0].nAppId,"123456");
	//strcpy(m_AppItem[0].strTokenRand,"d333233232323");
	//strcpy(m_AppItem[0].strTokenTimeOut,"30");
	///*MSG_RET_SEND_COMPLETE_BODY bb,cc;
	//memcpy(bb.strFileID,"sdfsdf",4);
	//memcpy(bb.strFileLocation,"ssssss",6);*/

	////cc.setNetBuf(bb.getNetBuf(),bb.getNetBufLen());
	////printf(cc.strFileLocation);
	//char token[]="nxut2u5aqg5hpxemnpfj7rack94a3gth|1|2|20120504104617|10.10.9.172|1000|a50bb88a55479474739af46c161c7834";
	//FileToken ftoken(token);
	////ftoken.setToken(token);
	//char * sdf=NULL;
	//sdf=(char *)malloc(200);
	//strcpy(sdf,ftoken.getsummd5());
	//std::string mde=MD5(sdf,strlen(sdf)).toString();
	//printf("MD5:%s",mde.c_str());
	//printf("Token Vaild:%d\nAPPID:%s\nIP:%s\nsummd5:%s",ftoken.Validate(),ftoken.getAppID(),ftoken.getIP(),ftoken.getsummd5());
	//char gentoken[]="nxut2u5aqg5hpxemnpfj7rack94a3gth|1|123456|20120507185517|10.10.9.172|1000|d333233232323";
	//nxut2u5aqg5hpxemnpfj7rack94a3gth|1|123456|20120507185517|10.10.9.172|1000|93ea25552f2347b260e7870acd53ddc5
	//setlocale(LC_ALL, "Chinese-simplified");
	
	
	//CString ff=argv[1];
	//char ffc[PATH_MAX+1];
	//strcpy(ffc,ff);
	//setlocale(LC_ALL, "");
	//std::locale::global(std::locale(""));
	//cout <<MD5(ifstream(ff, ios_base::binary)).toString().c_str() <<endl;
	////cout <<MD5(ifstream(argv[1], ios_base::binary)).toString().c_str() <<endl;
	//cout << "�Ƕ���Ƽ�" <<endl;
	
	/*
	int len=strlen(token);
	char summd5[500];
	memcpy(summd5,token+35,6); 
	summd5[6]='\0';
	printf(summd5);*/
	/*char * uuid=NULL;
	uuid=(char*)malloc(32);
	get_GUID(uuid);
	printf("UUID:%s\n%d\n",uuid,strlen(uuid));*/
	//test_tcp_init();
	//FILE * dfile,*dfile1;
	//dfile=fopen("c:\\abcd.key","rb");
	//dfile1=fopen("c:\\ab.key","wb");
	//char * fbuf;
	//AfxMessageBox(get_strGUID());
	//fbuf=(char *)malloc(1024*1000);
	//size_t f=fread(&fbuf,1024*1000,1,dfile);
	//fwrite(&fbuf,1024*1000,1,dfile1);
	//fclose(dfile1);
	//fclose(dfile); 
	/*CFile dtfile,dtfile1; 
	dtfile.Open("c:\\hello.exe",CFile::modeRead|CFile::typeBinary);
	dtfile1.Open("c:\\he.exe",CFile::modeCreate|CFile::modeReadWrite|CFile::typeBinary);
	char * fbuf;

	fbuf=(char *)malloc(dtfile.GetLength());
	WORD nsize=(WORD)dtfile.Read(fbuf,dtfile.GetLength());
	dtfile1.Write(fbuf,dtfile.GetLength());

	dtfile1.Close();
	dtfile.Close();*/
	//test_tcp_init_cont();
	//test_tcp_send_part();
	//test_tcp_send_complete();

};