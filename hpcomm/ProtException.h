// ProtException.h: interface for the CProtException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROTEXCEPTION_H__CFF950F3_5BD6_11D3_823C_0000B4A265C3__INCLUDED_)
#define AFX_PROTEXCEPTION_H__CFF950F3_5BD6_11D3_823C_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Prot.h"
class CProtException : public CException  
{

public:
	CString sMessage;
	/*typedef enum {SUCCESS, OPENPORT, RETRIES, DEVICE, TIMEOUT,
				  CANCELLED, FILEERROR, ERRORPACKET, UNKNOWN,
				  LOWBATTERY, SYNTAX} ERRCODE;*/

	CProtException(CProt::ERRCODE code);
	virtual ~CProtException();
	CProt::ERRCODE error;
};

#endif // !defined(AFX_PROTEXCEPTION_H__CFF950F3_5BD6_11D3_823C_0000B4A265C3__INCLUDED_)
