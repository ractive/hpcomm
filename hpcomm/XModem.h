//////////////////////////////////////////////////////////////////////
/////////////////////              HPComm            /////////////////
//////////////////////////////////////////////////////////////////////



// XModem.h: interface for the XModem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMODEM_H__E0221B83_07D0_11D3_AE2A_0000E8627120__INCLUDED_)
#define AFX_XMODEM_H__E0221B83_07D0_11D3_AE2A_0000E8627120__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Prot.h"

class XModem : public CProt  
{
public:
	bool cp(bool removeAfterCopy, CFiler * fromFiler, CFilerEntryList *fromList, CFilerEntry *to) {return false;};

	CProt::ERRCODE readRemoteDir(CString &sRemoteDir);
	void Restore(const CString &sName) { };

	void Archive(CString sArchive) { };

	XModem(CSettings * settings);
	virtual ~XModem();

    bool _SetBinary(bool bBinary);
	BOOL ReadRemoteDir();
	void Server();
	int GetFreeMem();
	BOOL ExecCommand(CString sCommand);
	BOOL SendCommand(char cType, const char * data, int nLen);
	BOOL ChangeDir(CString sDirName);
	BOOL SendFileEx(CString sFileName, UINT uSize = 0);
	int nBufLen;
	WORD CalcHPCRC(char * s);
	BOOL bTimedOut;
	void TimedOut();
	void Nak();
	void Ack();
	BOOL RecvFile();
	ERRCODE RecvFiles();
	ERRCODE SendFiles();
	void Resend();
	int SendFile();
	//unsigned Compute_CRC(char * s, int len);

protected:
	char PADCHAR, SOH, EOT, ACK, NAK, CAN, CRC, STX;
	int tries;

};

#endif // !defined(AFX_XMODEM_H__E0221B83_07D0_11D3_AE2A_0000E8627120__INCLUDED_)
