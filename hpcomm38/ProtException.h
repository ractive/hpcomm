// ProtException.h: interface for the CProtException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROTEXCEPTION_H__429F0FA2_6C66_11D3_82D5_0000B4A265C3__INCLUDED_)
#define AFX_PROTEXCEPTION_H__429F0FA2_6C66_11D3_82D5_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProtException : public CException  
{
public:
	typedef enum {SUCCESS, OPENPORT, RETRIES, DEVICE, TIMEOUT,
				  CANCELLED, SKIPPED, FILEERROR, ERRORPACKET, UNKNOWN,
				  LOWBATTERY, SYNTAX, MEMORY, VDINIT} ERRCODE;

	CProtException(ERRCODE errcode, DWORD dwSysErrcode = 0, CString sMsg = "");
	virtual ~CProtException();
	CString sMessage;
	ERRCODE error;
	DWORD dwError;
};

#endif // !defined(AFX_PROTEXCEPTION_H__429F0FA2_6C66_11D3_82D5_0000B4A265C3__INCLUDED_)
