// ProtException.cpp: implementation of the CProtException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProtException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProtException::CProtException(ERRCODE errcode, DWORD dwSysErrcode, CString sMsg)
{
	error = errcode;
	sMessage = sMsg;
	dwError = dwSysErrcode;
}

CProtException::~CProtException()
{

}
