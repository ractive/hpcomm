/*************************************************************************
  HPGComm
**************************************************************************

Copyright (C) January 2001 by HP and Jean-Pierre Bergamin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
***************************************************************************/


// Serial.h: interface for the CSerial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIAL_H__429F0FA1_6C66_11D3_82D5_0000B4A265C3__INCLUDED_)
#define AFX_SERIAL_H__429F0FA1_6C66_11D3_82D5_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// A WM_COMDAEMON message is sent to the window which created this thread to
// indicate that the thread ended.
#define WM_COMDAEMON (WM_USER + 3)
#define COMACTIVITY		0
#define COMDAEMONDIED	1


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
	unsigned long m_ulBytesSent;
private:
	static DWORD WINAPI ComDaemon(LPVOID lParam);
	HANDLE hKill;
	DWORD evMask;
	//OVERLAPPED osComDaemon;
	HANDLE m_hThread;
	comdaemoninfo * daemoninfo;
protected:
	void SendChar(char ch);
	HANDLE hReceived;
	void ThrowProtException(CProtException::ERRCODE errcode, DWORD dwSysErrcode = 0, TCHAR *sMsg = NULL);
	void ThrowProtException(CProtException * pEx);
};

#endif // !defined(AFX_SERIAL_H__429F0FA1_6C66_11D3_82D5_0000B4A265C3__INCLUDED_)
