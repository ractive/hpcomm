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


// Serial.cpp: implementation of the CSerial class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HPComm38.h"
#include "Serial.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Prototype
//UINT ComDaemon(LPVOID lParam);


CSerial::CSerial()
{
	hComPort = INVALID_HANDLE_VALUE;

	daemoninfo = 0;
	pThread = 0;

	m_ulBytesSent = 0;
}

CSerial::~CSerial()
{
	KillComDaemon();
	//ClosePort();
}

void CSerial::OpenPort(int nComPort)
{
	if (PortIsOpen())
		return;

	CString sPort;
	sPort.Format(_T("COM%d"), nComPort);
	// DWORD dwError = 0L;

	//Try to open the port
	bool bSuccess = FALSE;
	hComPort = ::CreateFile(sPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
	if (hComPort == INVALID_HANDLE_VALUE) {
		// FIXME: Must work out what to do with the error code one day...
		ThrowProtException(CProtException::OPENPORT, GetLastError());
    }

    DCB dcb = {0};
	dcb.DCBlength = sizeof(dcb);
    // Set settings
    if (!GetCommState(hComPort, &dcb))
        ThrowProtException(CProtException::DEVICE, GetLastError());

    dcb.BaudRate = 9600;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.fParity = FALSE;
    dcb.StopBits = ONESTOPBIT;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
	dcb.EvtChar = '\0';
    dcb.fAbortOnError = TRUE;

    if (!SetCommState(hComPort, &dcb) || hComPort == INVALID_HANDLE_VALUE)
        ThrowProtException(CProtException::DEVICE, GetLastError());

    //Set timeout to 10 sec.
	COMMTIMEOUTS TimeOut;

	TimeOut.ReadIntervalTimeout = 0;
	TimeOut.ReadTotalTimeoutMultiplier = 0;
	TimeOut.ReadTotalTimeoutConstant = 0;
	TimeOut.WriteTotalTimeoutMultiplier = 0;
	TimeOut.WriteTotalTimeoutConstant = 0;

	if (!SetCommTimeouts(hComPort, &TimeOut)) {
        ThrowProtException(CProtException::DEVICE, GetLastError());		
	}

    SetTimeout(10);
    FlushComPort();
}

void CSerial::ThrowProtException(CProtException::ERRCODE errcode, DWORD dwSysErrcode, CString sMsg)
{
	CProtException* pEx = new CProtException(errcode, dwSysErrcode, sMsg);
	ThrowProtException(pEx);
}

void CSerial::ThrowProtException(CProtException *pEx)
{
	throw pEx;
}

void CSerial::SetTimeout(int x)
{
	nTimeout = x * 1000; // milliseconds
}

bool CSerial::WriteBufferToCom(char *buf, DWORD dwBufLen)
{
	bool bRet = false;
	DWORD dwRes;
	OVERLAPPED osWrite = {0};

	// Create the overlapped structure
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL) {
		// The event could not be created.
		return false;
	}
	DWORD dwWritten;
	if (!WriteFile(hComPort, buf, dwBufLen, &dwWritten, &osWrite)) {
		DWORD dwError = GetLastError();
		if (dwError != ERROR_IO_PENDING) {
			// The write operation could not be started. 
			// This indicates a quite bad problem.
			ThrowProtException(CProtException::DEVICE, GetLastError());
		}
		else {
			// The operation started and the IO is pending.
			// Wait for the operation until it's finished.
			dwRes = WaitForSingleObject(osWrite.hEvent, nTimeout);
			// The operationis finished.
			if (dwRes == WAIT_OBJECT_0) {
				// The state of osWrite.hEvent is signaled which says, the operation
				// completed
				if (GetOverlappedResult(hComPort, &osWrite, &dwWritten, FALSE)) {
					// Now check if all the bytes could be written.
					// The only way to check that is to compare the dwWritten and
					// the dwBufLen. It it's the same, the operatoin was successful.
					m_ulBytesSent += dwWritten;
					bRet = (dwBufLen == dwWritten);
				}
			}
		}
	}
	else {
		// The operation completed immediately

		// Check if everything could be written
		m_ulBytesSent += dwWritten;
		bRet = (dwBufLen == dwWritten);
	}

	CloseHandle(osWrite.hEvent);
	return bRet;
}

DWORD CSerial::ReadBufferFromCom(char *buf, DWORD dwBufLen)
{
	bool bRet = false;
	bool bWaitingOnRead = false;
	DWORD dwRes;
	OVERLAPPED osRead = {0};
	// Create the overlapped event. This event is signaled if the read operation
	// is completed.
	osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (osRead.hEvent == NULL) {
		ThrowProtException(CProtException::UNKNOWN, GetLastError());
	}
	DWORD dwRead;
	if (!ReadFile(hComPort, buf, dwBufLen, &dwRead, &osRead)) {
		DWORD dwError = GetLastError();
		if (dwError != ERROR_IO_PENDING) {
			// The read operation could not be started. 
			// This indicates a quite bad problem.
			ThrowProtException(CProtException::DEVICE, GetLastError());
		}
		else {
			// The read is in progress, but not finished yet.
			bWaitingOnRead = true;
		}
	}
	//else  The read operation completed immediately.

	if (bWaitingOnRead) {
		// Wait as long as we defined the timeout value and see what happened.
		dwRes = WaitForSingleObject(osRead.hEvent, nTimeout);
		switch (dwRes) {
		case WAIT_OBJECT_0:
			if (!GetOverlappedResult(hComPort, &osRead, &dwRead, FALSE)) {
				// There went something wrong.
				ThrowProtException(CProtException::UNKNOWN, GetLastError());
			}
			else {
				// The read operation completed.
				bWaitingOnRead = false;
			}
			break;

		case WAIT_TIMEOUT:
			// The operation is still pending.
			// Here would be a good place to do some background stuff....
			TRACE("Timeout: %d   Length: %d\n", nTimeout, dwBufLen);
			// Since we don't want to wait for it, cancel this read.
			//CancelIo(hComPort);
			break;

		default:
			// Hmm, WaitForSingleObject returned some funny things.
			ThrowProtException(CProtException::UNKNOWN, GetLastError());
			break;
		}
	}

	CloseHandle(osRead.hEvent);

	// Return how may bytes could be read. 
	return dwRead;
}

void CSerial::ClosePort()
{
	// Check if the handle is valid
	if (hComPort != INVALID_HANDLE_VALUE) {
		// Close it.
		CloseHandle(hComPort);
        hComPort = INVALID_HANDLE_VALUE;
	}
}

HANDLE CSerial::CreateComDaemon(HWND hWnd)
{
	if (pThread != 0) {
		// We already have a thread.
		return false;
	}
	
	// EV_RXCHAR says that WaitCommEvent will signal the hEvent member of it's
	// OVERLAPPED struct if any char is received.
	if (!SetCommMask(hComPort, EV_RXCHAR)) {
		return false;
	}

	// We pass the thread 3 handles.
	// hKill		If this event is set, the thread terminates
	// hResume		If this event is set, the thread is resumed and listens for
	//              receiving characters again.
	// hComPort		The handle of the com-port.

	// Create the struct that will be passe to the thread
	daemoninfo = new comdaemoninfo;

	// Create the event handles.
	hKill = CreateEvent(NULL, TRUE, FALSE, NULL);
	daemoninfo->hKill = hKill;

	hResume = CreateEvent(NULL, TRUE, FALSE, NULL);
	daemoninfo->hResume = hResume;

	hReceived = CreateEvent(NULL, TRUE, FALSE, NULL);
	daemoninfo->hReceived = hReceived;

	daemoninfo->hComPort = hComPort;

	hNotificationWnd = hWnd;
	daemoninfo->phWnd = &hNotificationWnd;

	// Start the thread which waits for an event to occur, either ov.hEvent or hCancel.
	// The thread is initially resumed. Call CSerial::UnleashDaemon to start listening
	// to the com-port.
	pThread = AfxBeginThread(ComDaemon, daemoninfo, THREAD_PRIORITY_NORMAL, 0, 0);
	pThread->m_bAutoDelete = false;

	// The thread also gets a ae event-handle which is set if a char is received.
	// This gives another possibility to wait for this
	// event (i.e. not in a window (which receives a message), but in another thread).

	return hReceived;
}

void CSerial::KillComDaemon()
{
	if (pThread == 0)
		return;

	UnleashDaemon();

	SetEvent(hKill);

	// Wait till the thread exited
	WaitForSingleObject(pThread->m_hThread, INFINITE);
	
	CloseHandle(hKill);
	CloseHandle(hResume);

	if (daemoninfo != 0) {
		delete daemoninfo;
		daemoninfo = 0;	
	}

	if (pThread != NULL) {
		delete pThread;
		pThread = NULL;
	}

	evMask = 0;
}

bool CSerial::ComActivity()
{
	// If a char was received on the comport evMask == EV_RXFLAG.
	return (evMask == EV_RXCHAR);
}

bool CSerial::ReadCharFromCom(char *ch)
{
	return (ReadBufferFromCom(ch, 1) == 1);
}

void CSerial::FlushComPort(bool bFlushRecvBuffer /* = true */)
{
	// This call only flushes the transmit buffer.
	if (hComPort != INVALID_HANDLE_VALUE) {
		FlushFileBuffers(hComPort);
	}
	
	if (bFlushRecvBuffer) {
		DWORD nInBufLen = InBufferLength();
		if (nInBufLen > 0 && hComPort != INVALID_HANDLE_VALUE) {
			char * buf = new char[nInBufLen];
			ReadBufferFromCom(buf, nInBufLen);
			delete [] buf;
		}
	}
}

bool CSerial::PortIsOpen()
{
	return (hComPort != INVALID_HANDLE_VALUE);
}

HWND CSerial::SetNotificationWindow(HWND hWnd)
{
	HWND hOldWnd = hNotificationWnd;
	// Check if the window really exists
	if (::IsWindow(hWnd) && (pThread != 0)) {
		// Suspend the thread and set the new hWnd.
		pThread->SuspendThread();
		hNotificationWnd = hWnd;
		pThread->ResumeThread();
	}

	return hOldWnd;
}

DWORD CSerial::InBufferLength()
{
	COMSTAT comstat;
	DWORD errors;
	ClearCommError(hComPort, &errors, &comstat);
	return comstat.cbInQue;
}

void CSerial::UnleashDaemon()
{
	if (pThread != 0) {
		SetEvent(hResume);
	}
}

void CSerial::SendChar(char ch)
{
	WriteBufferToCom(&ch, 1);
}

UINT CSerial::ComDaemon(LPVOID lParam)
{
	comdaemoninfo * info = (comdaemoninfo *)lParam;

	// We wait for the first time hResume is set.
	if (WaitForSingleObject(info->hResume, INFINITE) != WAIT_OBJECT_0) {
		// Oooops, what was this?
		return 1;
	}

	ResetEvent(info->hResume);

	OVERLAPPED osWait = {0, 0, 0, 0, NULL};
	osWait.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	DWORD evMask = 0;
	DWORD dwError = 0;




	// Wait for the first char to be received.
	if (WaitCommEvent(info->hComPort, &evMask, &osWait)) {
		// There should be already something at the com-port.
		int x = 3;
	}

	// Build a handle array.
	HANDLE handles[3];
	handles[0] = osWait.hEvent;
	handles[1] = info->hKill;
	handles[2] = info->hResume;

	while (1) {
		// Wait for an event forever. Either the hKill or the ov.hEvent is set to 
		// active state.
		TRACE("Waiting for hKill or a char\n");
		DWORD dwRet = WaitForMultipleObjects(3, handles, FALSE, INFINITE);
		if (dwRet == WAIT_OBJECT_0) {
			// Check again if there was really a character received

			dwError = GetLastError();
			if (dwError == 0) {
			}

			// !!! this var does not contain valid data if WaitCommEvent is executed. !!!
			DWORD bytes;
			if (!GetOverlappedResult(info->hComPort, &osWait, &bytes, FALSE)) {
				DWORD dwError = GetLastError();
				if (dwError != ERROR_IO_PENDING) {
					// Hmm, there went something wrong.
					break;
				}
			}

			// The GetOverlappedResult method said everything was ok.
			// Check again if the evMask is set correctly
			if (!(evMask & EV_RXCHAR)) {
				// Hmm, another strange thing happened. There was probably 
				// another comm-event.
				// Just keep on listening...
				continue;
			}


			// Check if the I/O-Buffer contains something.
			COMSTAT comstat;
			DWORD errors;
			ClearCommError(info->hComPort, &errors, &comstat);
			if (comstat.cbInQue == 0) {
				// Hmmm, the I/O-Buffer does not contain something.
				TRACE("In buffer == 0 after a WaitCommEvent!\n");
				WaitCommEvent(info->hComPort, &evMask, &osWait);
				// Keep on listening.
				//ResetEvent(osWait.hEvent);
				continue;
			}

			// Now we are QUITE sure a char received :-)

			// Set the event to indicate that.
			SetEvent(info->hReceived);
			TRACE("Set the hReceived event. evMask: %d\n", evMask);
			// Send the notification window a message that an event occured
			::PostMessage(*(info->phWnd), WM_COMDAEMON, 0, COMACTIVITY);

			// Now we wait for the info->hResume event to be set so that the thread
			// can continue to listen to the com-port. Also listen to hCancel events
			// so the the thread can terminate.
			HANDLE handlearray[2];
			handlearray[0] = info->hKill;
			handlearray[1] = info->hResume;
			DWORD dwRet = WaitForMultipleObjects(2, handlearray, FALSE, INFINITE);
			if (dwRet == WAIT_OBJECT_0) {
				// The OBJECT_0 is in this case hKill which is the cancel event.
				// Leave the while loop and end the thread.
				break;
			}
			else if (dwRet == (WAIT_OBJECT_0 + 1)) {
				// OBJECT_1 is the resume event. Reset it.
				ResetEvent(info->hResume);
				ResetEvent(info->hReceived);

				// Wait for another char to arrive
				evMask = 0;
				ResetEvent(osWait.hEvent);
				WaitCommEvent(info->hComPort, &evMask, &osWait);

				TRACE("Another WaitCommEvent was started.\n");
				continue;
			}
			// else continue to listen to the com-port.
		}
		else if (dwRet == (WAIT_OBJECT_0 + 1)) {
			// The hCancel event was set.
			// Leave the while loop and end the thread.
			break;
		}
		else if (dwRet == (WAIT_OBJECT_0 + 2)) {
			// Hmm, there is nothing to resume. Just reset the event and
			// keep on listening...
			ResetEvent(info->hResume);
			continue;
		}
	}
	
	CloseHandle(osWait.hEvent);

	// Inform the window that the thread will end.
	::PostMessage(*(info->phWnd), WM_COMDAEMON, 0, COMDAEMONDIED);

	return 0;
}
