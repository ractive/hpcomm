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


//////////////////////////////////////////////////////////////////////
//
// Prot.h: interface for the CProt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROT_H__E0221B81_07D0_11D3_AE2A_0000E8627120__INCLUDED_)
#define AFX_PROT_H__E0221B81_07D0_11D3_AE2A_0000E8627120__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Serial.h"
#include "HPComm38.h"
//#include "TransferThread.h"

class CTransferInfo;

class CProt : public CSerial  
{
public:
	BOOL m_bShowTransDetails;
	int GetBufferFromFile(char * buf, int nLen);
	int PutBufferToFile(char * buf, int nLen);

	//virtual void ClientRequest(CString &sCurPath) = 0;
	CProt();
	virtual ~CProt();

	// Send all files which are defined with pIOReq.
	virtual void SendFiles() = 0;

	// Receive the files which are defined with pIOReq
	virtual void RecvFiles() = 0;

	// Reset the internal setting of the class
	virtual void Reset(bool bCloseInfoDlg = true) = 0;

	// Stops ongoing transfers and closes the info dialog
	void StopAll(void);

	// The protocol handles the request
	virtual void ClientRequest(char cFirstChar, char cSecondChar, CString &sCurPath) = 0;


	// This opens the com-port and tries to get information about the calculator

	// Clean-up
	//virtual void DisConnect() = 0;

	// Get the content of the current directory of the calculator

	// Get the amount of free memory of the calc.

	// The command in sCommand is executed on the calculator.

	// These variables are used to hint to the MainFrame whether it should
	// refresh the windows.
	bool m_bTransactionFinished;
	bool m_bNewFileCreated;

	// If HP38DIR.CUR and HP38DIR.xxx was sent this bool is set to true
	bool m_bVDFilesSent;

	// This variable says CMainFrame if the 38G has sent some files or if it
	// was just requesting files.
	// CMainFrame then can decide to be enabled or not.
	bool m_bSendingOnly;

	/*void (*ProtCallback)(CString &sFileName, bool bRecv, int nPackets);
	void (*SetRange)(unsigned long unSize);
	void (*SetPos)(unsigned long unEffBytes, unsigned long unTotalBytes);*/
	void Transfer(bool bRecv = false);

	BOOL m_bTransfering;
	CString m_sCurPCPath;
	CTransferInfo * m_pTransferDlg;
private:
	bool m_bWasReadonly;
	bool m_bFirst;
	bool m_bRecv;
	HANDLE m_hRecvFile;
	HANDLE m_hSendFile;
	bool m_bUseBuffer;
	POSITION pos;
protected:
	void SetSendPos(unsigned long ulBytes);
	void FileInfo(LPCTSTR sFilename, BOOL bRecv);
	void CloseTransferWnd(UINT nWait = 1000);
	void SetPos(unsigned long ulEffBytes, unsigned long ulTotal);
	void SetRange(unsigned long ulRange);
	BOOL m_bIsXModem;
	void SetPointerBack(int nOffset, BOOL bSendFile = TRUE);
	static HANDLE m_hStop;
	static UINT TransferThreadProc(LPVOID lParam);

	static CProt * m_pPseudoThis;
	BOOL m_bCancel, m_bSkipFile;
	CString sReqFileName;
	bool OpenFile();
	bool CloseFile();
	bool CreateFile();
	bool PutCharToFile(char ch);
	bool GetCharFromFile(char &ch);

	bool GetFileName(CString & sFileName);
	unsigned int GetFileSize();
};

struct threadinfo {
	CProtException * pEx;
	bool write;
}; // I always forget this ";"


#endif // !defined(AFX_PROT_H__E0221B81_07D0_11D3_AE2A_0000E8627120__INCLUDED_)
