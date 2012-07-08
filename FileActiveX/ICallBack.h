#pragma once

class ICallBack
{
public:
	int m_ErrorCode;
public:
	virtual void OnSuccesful() = 0;//操作成功
	virtual void OnFailed() = 0;//操作失败
	virtual void OnCanel() = 0;//操作失败
	virtual void OnProgress(int progressvalue) = 0;//进度
};

