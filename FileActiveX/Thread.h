#pragma once
#include "stdafx.h"
#include "ICallBack.h"

class CThread
{
public:
	CThread()
	{ 
		m_pThreadFunction = CThread::EntryPoint;
		m_runthread = FALSE;
	}

	virtual ~CThread()
	{
		if ( m_hThread )
			Stop(true);                    //thread still running, so force the thread to stop!
	}

	DWORD Start(DWORD dwCreationFlags = 0)
	{
		m_runthread = true;
		m_hThread = CreateThread(NULL, 0, m_pThreadFunction, this, dwCreationFlags,&m_dwTID);

		m_dwExitCode = (DWORD)-1;

		return GetLastError();
	}

	/**//**
		* Stops the thread.
		*    
		* @param bForceKill        if true, the Thread is killed immediately
		*/
	DWORD Stop ( bool bForceKill = false )
	{
		try{
			if ( m_hThread )
			{
				//尝试"温柔地"结束线程
				if (m_runthread == TRUE)
					m_runthread = FALSE;        //first, try to stop the thread nice

				GetExitCodeThread(m_hThread, &m_dwExitCode);

				if ( m_dwExitCode == STILL_ACTIVE && bForceKill )
				{//强制杀死线程
					TerminateThread(m_hThread, DWORD(-1));
					m_hThread = NULL;
				}
			}
		}catch(...)
		{
		}


		return m_dwExitCode;
	}
	/**//**
		* Stops the thread. first tell the thread to stop itself and wait for the thread to stop itself.
		* if timeout occurs and the thread hasn't stopped yet, then the thread is killed.
		* @param timeout    milliseconds to wait for the thread to stop itself
		*/
	DWORD Stop ( WORD timeout )
	{
		Stop(false);
		WaitForSingleObject(m_hThread, timeout);//等待一段时间
		return Stop(true);
	}

	/**//**
		* suspends the thread. i.e. the thread is halted but not killed. To start a suspended thread call Resume().
		*/
	DWORD Suspend()
	{//挂起线程
		return SuspendThread(m_hThread);
	}

	/**//** 
		* resumes the thread. this method starts a created and suspended thread again.
		*/
	DWORD Resume()
	{//恢复线程
		return ResumeThread(m_hThread);
	}

	/**//**
		* sets the priority of the thread.
		* @param priority    the priority. see SetThreadPriority() in windows sdk for possible values.
		* @return true if successful
		*/
	BOOL SetPriority(int priority)
	{//设置线程优先级
		return SetThreadPriority(m_hThread, priority);
	}

	/**//**
		* gets the current priority value of the thread.
		* @return the current priority value
		*/
	int GetPriority()
	{//获取线程优先级
		return GetThreadPriority(m_hThread);
	}

	void SetICallBack(ICallBack* pCallBack)
	{
		this->pCallBack = pCallBack;
	}

protected:

	/**//**
		* 子类应该重写此方法，这个方法是实际的工作线程函数
		*/
	virtual DWORD ThreadMethod() = 0;

private:

	/**//**
		* DONT override this method.
		*
		* this method is the "function" used when creating the thread. it is static so that way
		* a pointer to it is available inside the class. this method calls then the virtual 
		* method of the parent class.
		*/
	static DWORD WINAPI EntryPoint( LPVOID pArg)
	{
		CThread *pParent = reinterpret_cast<CThread*>(pArg);
		pParent->ThreadMethod();//多态性，调用子类的实际工作函数
		return 0;
	}

private:
	HANDLE    m_hThread;                    /**////<Thread Handle    线程句柄
	DWORD    m_dwTID;                    ///<Thread ID    线程ID
	LPVOID    m_pParent;                    ///<this pointer of the parent CThread object
	DWORD    m_dwExitCode;                ///<Exit Code of the thread 线程退出码

protected:
	LPTHREAD_START_ROUTINE    m_pThreadFunction;    /**////<工作线程指针
	BOOL    m_runthread;                ///<线程是否继续运行的标志
	ICallBack* pCallBack; 
};
