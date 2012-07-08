// FileActiveXPropPage.cpp : Implementation of the CFileActiveXPropPage property page class.

#include "stdafx.h"
#include "FileActiveX.h"
#include "FileActiveXPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CFileActiveXPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CFileActiveXPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CFileActiveXPropPage, "MYACTIVEX.FileActiveXPropPage.1",
	0x746cccbd, 0x3589, 0x4370, 0x8d, 0x61, 0xa3, 0xef, 0x1c, 0x7c, 0x12, 0x24)



// CFileActiveXPropPage::CFileActiveXPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CFileActiveXPropPage

BOOL CFileActiveXPropPage::CFileActiveXPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_MYACTIVEX_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CFileActiveXPropPage::CFileActiveXPropPage - Constructor

CFileActiveXPropPage::CFileActiveXPropPage() :
	COlePropertyPage(IDD, IDS_MYACTIVEX_PPG_CAPTION)
{
}



// CFileActiveXPropPage::DoDataExchange - Moves data between page and properties

void CFileActiveXPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CFileActiveXPropPage message handlers
