#pragma once


// UpLoadFileDialog 对话框

class UpLoadFileDialog : public CDialog
{
	DECLARE_DYNAMIC(UpLoadFileDialog)

public:
	UpLoadFileDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~UpLoadFileDialog();

// 对话框数据
	enum { IDD = IDD_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
