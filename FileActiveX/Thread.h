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
				//����"�����"�����߳�
				if (m_runthread == TRUE)
					m_runthread = FALSE;        //first, try to stop the thread nice

				GetExitCodeThread(m_hThread, &m_dwExitCode);

				if ( m_dwExitCode == STILL_ACTIVE && bForceKill )
				{//ǿ��ɱ���߳�
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
		WaitForSingleObject(m_hThread, timeout);//�ȴ�һ��ʱ��
		return Stop(true);
	}

	/**//**
		* suspends the thread. i.e. the thread is halted but not killed. To start a suspended thread call Resume().
		*/
	DWORD Suspend()
	{//�����߳�
		return SuspendThread(m_hThread);
	}

	/**//** 
		* resumes the thread. this method starts a created and suspended thread again.
		*/
	DWORD Resume()
	{//�ָ��߳�
		return ResumeThread(m_hThread);
	}

	/**//**
		* sets the priority of the thread.
		* @param priority    the priority. see SetThreadPriority() in windows sdk for possible values.
		* @return true if successful
		*/
	BOOL SetPriority(int priority)
	{//�����߳����ȼ�
		return SetThreadPriority(m_hThread, priority);
	}

	/**//**
		* gets the current priority value of the thread.
		* @return the current priority value
		*/
	int GetPriority()
	{//��ȡ�߳����ȼ�
		return GetThreadPriority(m_hThread);
	}

	void SetICallBack(ICallBack* pCallBack)
	{
		this->pCallBack = pCallBack;
	}

protected:

	/**//**
		* ����Ӧ����д�˷��������������ʵ�ʵĹ����̺߳���
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
		pParent->ThreadMethod();//��̬�ԣ����������ʵ�ʹ�������
		return 0;
	}

private:
	HANDLE    m_hThread;                    /**////<Thread Handle    �߳̾��
	DWORD    m_dwTID;                    ///<Thread ID    �߳�ID
	LPVOID    m_pParent;                    ///<this pointer of the parent CThread object
	DWORD    m_dwExitCode;                ///<Exit Code of the thread �߳��˳���

protected:
	LPTHREAD_START_ROUTINE    m_pThreadFunction;    /**////<�����߳�ָ��
	BOOL    m_runthread;                ///<�߳��Ƿ�������еı�־
	ICallBack* pCallBack; 
};
