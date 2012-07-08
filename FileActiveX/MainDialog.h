#pragma once
#include "pictureex.h"


// CMainDialog dialog

class CMainDialog : public CDialog
{
	DECLARE_DYNAMIC(CMainDialog)

public:
	CMainDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMainDialog();

// Dialog Data
	enum { IDD = IDD_MAINDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CPictureEx m_ProgressBar;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
