#pragma once

// FileActiveXPropPage.h : Declaration of the CFileActiveXPropPage property page class.


// CFileActiveXPropPage : See FileActiveXPropPage.cpp for implementation.

class CFileActiveXPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CFileActiveXPropPage)
	DECLARE_OLECREATE_EX(CFileActiveXPropPage)

// Constructor
public:
	CFileActiveXPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_MYACTIVEX };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

