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


// Prot.cpp: implementation of the CProt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Prot.h"
#include "VD.h"
#include "TransferThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProt * CProt::m_pPseudoThis = NULL;
HANDLE CProt::m_hStop = NULL;

CProt::CProt()
{
	pos = NULL;

	m_bUseBuffer = false;

	m_bNewFileCreated = false;

	m_bTransactionFinished = false;

	m_bSendingOnly = false;

	m_bVDFilesSent = false;

	m_hRecvFile = INVALID_HANDLE_VALUE;

	m_hSendFile = INVALID_HANDLE_VALUE;

	m_bRecv = true;

	m_pPseudoThis = this;

	m_pDlgThread = 0;

	m_bTransfering = FALSE;

	m_hStop = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CProt::~CProt()
{
	CloseTransferWnd();

	ClosePort();

	CloseFile();
}

unsigned int CProt::GetFileSize()
{
	HANDLE hFile;
	if (m_bRecv) {
		hFile = m_hRecvFile;
	}
	else {
		hFile = m_hSendFile;
	}
	CFile file((int)hFile);
	return file.GetLength();
}

bool CProt::GetFileName(CString &sFileName)
{
	sFileName = sReqFileName;
	return true;
}

bool CProt::GetCharFromFile(char &ch)
{
	DWORD dwRead;
	ReadFile(m_hSendFile, &ch, 1, &dwRead, NULL);
	return dwRead == 1;
}

bool CProt::PutCharToFile(char ch)
{
	DWORD dwWritten;
	WriteFile(m_hRecvFile, &ch, 1, &dwWritten, NULL);
	return dwWritten == 1;
}

bool CProt::CreateFile()
{
	m_bRecv = true;
	
	CString sFilePath = m_sCurPCPath + "\\" + sReqFileName;

	CFileFind finder;
	bool bNewFileCreated = !finder.FindFile(sFilePath);
	finder.Close();

    CVD vd(m_sCurPCPath);
    if (vd.IsTheDirFile(sReqFileName)) {
		// The calc now writes a directory file to the PC which
		// means the transaction is finished.
		m_bTransactionFinished = true;
	}

	// If we receive the CUR file, the calc changed the directory and we
	// show the use info
	if (vd.IsTheCurFile(sReqFileName)) {
		m_bVDFilesSent = true;
	}

	m_bWasReadonly = false;

	// We check if the file has the read-only attribute set, but only if it exists
	// already.
	DWORD dwFileAttributes = GetFileAttributes(sFilePath);
	if (!bNewFileCreated && (dwFileAttributes & FILE_ATTRIBUTE_READONLY)) {
		// Some ApLets you download from the internet have the read-only flag set to 
		// the VD files HP38DIR.CUR and HP38DIR.000
		// If this is true, we remove it temporaly to let the calc update this file.
		if (vd.IsTheDirFile(sReqFileName) == false) {
			// For all other files we raise an error message and ask the user if
			// he wants to overwrite the file anyway.
			CString sMsg = "The file " + sReqFileName + " is read-only, so it can't be overwritten.\n\nDo you want to overwrite it anyway?";
			if (MessageBox(NULL, sMsg, "Warning", MB_YESNO | MB_ICONINFORMATION) == IDNO) {
				return false;
			}
		}

		// Try to remove the read-only attribute.
		if (!SetFileAttributes(sFilePath, (dwFileAttributes & ~FILE_ATTRIBUTE_READONLY))) {
			MessageBox(NULL, "The read-only attribute could not removed!", "Error", MB_OK | MB_ICONSTOP);
			return false;
		}
		
		m_bWasReadonly = true;
	}

	m_hRecvFile = ::CreateFile(m_sCurPCPath + "\\" + sReqFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwError = GetLastError();
	return m_hRecvFile != INVALID_HANDLE_VALUE;
}

bool CProt::CloseFile()
{
	bool bRet = false;
	if (m_bRecv) {
		// If we received a file, we have to close the file we created.
		if (m_hRecvFile != INVALID_HANDLE_VALUE) {
			bRet = CloseHandle(m_hRecvFile) == TRUE;
			m_hRecvFile = INVALID_HANDLE_VALUE;
            CVD vd(m_sCurPCPath);
			// If the file was read-only we set it back to that attribute.
			if (m_bWasReadonly || vd.IsTheDirFile(sReqFileName)) {
				CString sFilePath = m_sCurPCPath + "\\" + sReqFileName;
				// Get the other file-attributes and add the read-only flag.
				if (!SetFileAttributes(sFilePath, GetFileAttributes(sFilePath) | FILE_ATTRIBUTE_READONLY)) {
					MessageBox(NULL, "It was not possible to make " + sReqFileName + " read-only.", "Error", MB_OK | MB_ICONEXCLAMATION);
					return false;
				}
			}
			return bRet;
		}
		else
			return false;
	}
	else {
		// If we sent a file, se have to close the send file.
		if (m_hSendFile != INVALID_HANDLE_VALUE) {
			bRet = CloseHandle(m_hSendFile) == TRUE;
			m_hSendFile = INVALID_HANDLE_VALUE;
			return bRet;
		}
		else
			return false;
	}
}

bool CProt::OpenFile()
{
	m_bRecv = false;

    CVD vd(m_sCurPCPath);
	// If a file is opened which is not a VD-file we know that
	// it's only a send-only transaction.
	m_bSendingOnly = !vd.IsAVDFile(sReqFileName);
	m_bVDFilesSent = vd.IsTheDirFile(sReqFileName);

	m_hSendFile = ::CreateFile(m_sCurPCPath + "\\" + sReqFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);		
	return m_hSendFile != INVALID_HANDLE_VALUE;
}

int CProt::GetBufferFromFile(char *buf, int nLen)
{
	DWORD dwRead;
	ReadFile(m_hSendFile, buf, nLen, &dwRead, NULL);
	return dwRead;

}

int CProt::PutBufferToFile(char * buf, int nLen)
{
	DWORD dwWritten;
	WriteFile(m_hRecvFile, buf, nLen, &dwWritten, NULL);
	return dwWritten;
}

void CProt::Transfer(bool bRecv /* = false */)
{
	//CWaitCursor wait;
	m_bCancel = false;
	m_bSkipFile = false;
	
	unsigned int nSize = 0;
	threadinfo ti;

	ti.pEx = NULL;

	if (m_pDlgThread == NULL) {
		m_pDlgThread = (CTransferThread *)AfxBeginThread(RUNTIME_CLASS(CTransferThread));
		VERIFY(m_pDlgThread);
		m_pDlgThread->PostThreadMessage(WM_TRANSFER_INIT, 0, m_bIsXModem);
		m_pDlgThread->m_hCancelEvent = m_hStop;
	}
	else {
		m_pDlgThread->Pump();
	}



	// This method also can be called with a Null-ptr to IOReq from within
	// the method ClientRequest.
	ti.write = bRecv;

	DWORD exit;
	m_pPseudoThis = this;
	CWinThread * pThread = AfxBeginThread(TransferThreadProc, &ti, 0, CREATE_SUSPENDED, NULL);
	pThread->m_bAutoDelete = false;
	pThread->ResumeThread();

	HANDLE * handleArray = new HANDLE[2];

	handleArray[0] = pThread->m_hThread;
	handleArray[1] = m_hStop;

	m_bTransfering = TRUE;

	// Hmm, not sure but here could occur a "deadlock". If the Window gets not messages
	// any more and we call Pump at the wrong time, Pump will wait will a new
	// message arrives. Very unlikely, but possible.
	DWORD dwSigHandle;
	while (dwSigHandle = WaitForMultipleObjects(2, handleArray, FALSE, 50)) {
		if (dwSigHandle == WAIT_TIMEOUT) {
			m_pDlgThread->Pump();
			continue;
		}
		else if (dwSigHandle - WAIT_OBJECT_0 == 1) {
			m_bCancel = true;
			continue;
		}
		else if (dwSigHandle - WAIT_OBJECT_0 == 0) {
			break;
		}
	}

	WaitForSingleObject(pThread->m_hThread, 1000);

	delete [] handleArray;

	m_bTransfering = FALSE;

	if (!::GetExitCodeThread(pThread->m_hThread, &exit)) {
		ThrowProtException(CProtException::UNKNOWN, GetLastError());
	}
	
	delete pThread;


	if ((m_bTransactionFinished || m_bSendingOnly) /*&& m_pInfoDlg*/) {
		CloseTransferWnd();

	}
	else if (m_bVDFilesSent/* && m_pInfoDlg*/) {
		m_pDlgThread->PostThreadMessage(WM_TRANSFER_INFO, 0, 0);
		m_pDlgThread->Pump();
	}

	// If the thread exited successfully, it returns SUCCESS;	
	
	CProtException::ERRCODE errcode = (CProtException::ERRCODE)exit;
	if (errcode != CProtException::SUCCESS) {
		// If the thread ended without success we check if we
		// have a non NULL exception pointer.
		// If yes, throw this exception, if not, throw one with
		// the corresponding error code.
		if (ti.pEx != NULL) {
			ThrowProtException(ti.pEx);
		}
		else {
			ThrowProtException(errcode);
		}
	}
}

UINT CProt::TransferThreadProc(LPVOID lParam)
{
	threadinfo * ti = (threadinfo *)lParam;
	try {
		if (ti->write) {
			m_pPseudoThis->SendFiles();
		}
		else {
			m_pPseudoThis->RecvFiles();	
		}
	}
	catch (CProtException * pEx) {
		ti->pEx = pEx;
		CProtException::ERRCODE error = pEx->error;
		return error;
	}

	return CProtException::SUCCESS;

}

void CProt::SetPointerBack(int nOffset, BOOL bSendFile /* = TRUE */)
{
	if (0xFFFFFFFF == SetFilePointer(m_hSendFile, -nOffset, NULL, FILE_CURRENT))
		SetFilePointer(m_hSendFile, 0L, NULL, FILE_BEGIN);
}

void CProt::SetRange(unsigned long ulRange)
{
	if (m_pDlgThread) {
		m_pDlgThread->PostThreadMessage(WM_TRANSFER_RANGE, 0, ulRange);
	}
}	

void CProt::SetPos(unsigned long ulEffBytes, unsigned long ulTotal)
{
	if (m_pDlgThread) {
		m_pDlgThread->PostThreadMessage(WM_TRANSFER_POS, ulEffBytes, ulTotal);

	}
}

void CProt::SetSendPos(unsigned long ulBytes)
{
	if (m_pDlgThread) {
		m_pDlgThread->PostThreadMessage(WM_TRANSFER_SENDPOS, 0, ulBytes);
	}

}


void CProt::CloseTransferWnd()
{
	if (m_pDlgThread) {
		m_pDlgThread->PostThreadMessage(WM_TRANSFER_QUIT, 0, 0);

		if (WaitForSingleObject(m_pDlgThread->m_hThread, 1000) != WAIT_OBJECT_0) {
			TRACE("TransferThread could not be stopped\r\n");
		}
		m_pDlgThread = NULL;
	}
}

void CProt::FileInfo(LPCTSTR sFilename, BOOL bRecv)
{
	if (m_pDlgThread) {
		m_pDlgThread->PostThreadMessage(WM_TRANSFER_FILEINFO, (UINT)(LPCTSTR)sFilename, bRecv);

	}
	
}

void CProt::StopAll()
{
	if (m_bTransfering) {
		SetEvent(m_hStop);
	}
	else {
		CloseTransferWnd();
	}
}
