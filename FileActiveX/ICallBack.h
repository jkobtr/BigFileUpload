#pragma once

class ICallBack
{
public:
	int m_ErrorCode;
public:
	virtual void OnSuccesful() = 0;//�����ɹ�
	virtual void OnFailed() = 0;//����ʧ��
	virtual void OnCanel() = 0;//����ʧ��
	virtual void OnProgress(int progressvalue) = 0;//����
};

