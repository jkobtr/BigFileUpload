#pragma once


// UpLoadFileDialog �Ի���

class UpLoadFileDialog : public CDialog
{
	DECLARE_DYNAMIC(UpLoadFileDialog)

public:
	UpLoadFileDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~UpLoadFileDialog();

// �Ի�������
	enum { IDD = IDD_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
