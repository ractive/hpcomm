// XModem.h: interface for the CXModem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMODEM_H__D32149AD_84EC_4F1E_B504_00D7AB49B6C8__INCLUDED_)
#define AFX_XMODEM_H__D32149AD_84EC_4F1E_B504_00D7AB49B6C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Prot.h"

class CXModem : public CProt  
{
public:
	void RecvFiles();
	void SendFiles();
	BOOL SendFile();
	void ClientRequest(char cFirstChar, char cSecondChar, CString &sCurPath);
	CXModem(HANDLE hPort = NULL);
	virtual ~CXModem();
protected:
	void Reset(bool bCloseInfoDlg = true);
	void Cancel();
	char PADCHAR, SOH, EOT, ACK, NAK, CAN, CRC, STX;
	int m_nBufLen;
	char SendPack[2048];
private:
	void Resend();
	int m_nTries;
	void Nak();
	void Ack();
	WORD CalcHPCRC(char * s);
};

#endif // !defined(AFX_XMODEM_H__D32149AD_84EC_4F1E_B504_00D7AB49B6C8__INCLUDED_)
