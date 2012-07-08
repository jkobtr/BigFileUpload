// MainDialog.cpp : implementation file
//

#include "stdafx.h"
#include "FileActiveX.h"
#include "MainDialog.h"


// CMainDialog dialog

IMPLEMENT_DYNAMIC(CMainDialog, CDialog)

CMainDialog::CMainDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDialog::IDD, pParent)
{

}

CMainDialog::~CMainDialog()
{
}

void CMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESSBAR, m_ProgressBar);
}


BEGIN_MESSAGE_MAP(CMainDialog, CDialog)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CMainDialog message handlers

int CMainDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	/*if (m_ProgressBar.Load(MAKEINTRESOURCE(IDR_PROGRESSBAR),_T("GIF")))
	m_ProgressBar.Draw();*/

	return 0;
}
