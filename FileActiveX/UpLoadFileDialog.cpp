// UpLoadFileDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileActiveX.h"
#include "UpLoadFileDialog.h"
#include "afxdialogex.h"


// UpLoadFileDialog �Ի���

IMPLEMENT_DYNAMIC(UpLoadFileDialog, CDialog)

UpLoadFileDialog::UpLoadFileDialog(CWnd* pParent /*=NULL*/)
	: CDialog(UpLoadFileDialog::IDD, pParent)
{
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif

}

UpLoadFileDialog::~UpLoadFileDialog()
{
}

void UpLoadFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(UpLoadFileDialog, CDialog)
END_MESSAGE_MAP()


// UpLoadFileDialog ��Ϣ�������
