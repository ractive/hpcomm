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

class CProt : public CSerial  
{
public:
	//virtual void ClientRequest(CString &sCurPath) = 0;
	CProt();
	virtual ~CProt();

	// Send all files which are defined with pIOReq.
	virtual void SendFiles() = 0;

	// Receive the files which are defined with pIOReq
	virtual void RecvFiles() = 0;

	// This opens the com-port and tries to get information about the calculator

	// Clean-up
	//virtual void DisConnect() = 0;

	// Get the content of the current directory of the calculator

	// Get the amount of free memory of the calc.

	// The command in sCommand is executed on the calculator.

	// These variables are used to hint to the MainFrame whether it should
	// refresh the windows.
	bool bTransactionFinished;
	bool bNewFileCreated;

	// If HP38DIR.CUR and HP38DIR.xxx was sent this bool is set to true
	bool bVDFilesSent;

	// This variable says CMainFrame if the 38G has sent some files or if it
	// was just requesting files.
	// CMainFrame then can decide to be enabled or not.
	bool bSendingOnly;
	
	void (*pfnTransferInfo)(CString &sFileName, bool bRecv);

private:
	bool bWasReadonly;
	bool bFirst;
	bool bRecv;
	HANDLE hRecvFile;
	HANDLE hSendFile;
	bool bUseBuffer;
	POSITION pos;
protected:
	CString sReqFileName;
	CString sCurPCPath;
	bool OpenFile();
	bool CloseFile();
	bool CreateFile();
	bool PutCharToFile(char ch);
	bool GetCharFromFile(char &ch);

	bool GetFileName(CString & sFileName);
	unsigned int GetFileSize();
};

#endif // !defined(AFX_PROT_H__E0221B81_07D0_11D3_AE2A_0000E8627120__INCLUDED_)
