// Serial.h: interface for the CSerial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIAL_H__429F0FA1_6C66_11D3_82D5_0000B4A265C3__INCLUDED_)
#define AFX_SERIAL_H__429F0FA1_6C66_11D3_82D5_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProtException.h"

struct comdaemoninfo {
	HANDLE		hComPort;
	HANDLE		hKill;
	HANDLE		hResume;
	HANDLE 		hReceived;
	HWND *		phWnd;
};


class CSerial  
{
public:
	int nTimeout;
	HANDLE hResume;
	void UnleashDaemon();
	DWORD InBufferLength();
	HWND hNotificationWnd;
	HWND SetNotificationWindow(HWND hWnd);
	bool PortIsOpen();
	void FlushComPort(bool bFlushRecvBuffer = true);
	bool ReadCharFromCom(char * ch);
	bool ComActivity();
	void KillComDaemon();
	HANDLE CreateComDaemon(HWND hWnd);
	void ClosePort();
	DWORD ReadBufferFromCom(char * buf, DWORD dwBufLen);
	bool WriteBufferToCom(char * buf, DWORD dwBufLen);
	void SetTimeout(int x);
	void OpenPort(int nComPort);
	CSerial();
	virtual ~CSerial();
	HANDLE hComPort;
private:
	static UINT ComDaemon(LPVOID lParam);
	HANDLE hKill;
	DWORD evMask;
	//OVERLAPPED osComDaemon;
	CWinThread * pThread;
	comdaemoninfo * daemoninfo;
protected:
	void SendChar(char ch);
	HANDLE hReceived;
	void ThrowProtException(CProtException::ERRCODE errcode, DWORD dwSysErrcode = 0, CString sMsg = "");
	void ThrowProtException(CProtException * pEx);
};

#endif // !defined(AFX_SERIAL_H__429F0FA1_6C66_11D3_82D5_0000B4A265C3__INCLUDED_)
