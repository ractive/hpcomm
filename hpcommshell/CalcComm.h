// CalcComm.h: interface for the CCalcComm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALCCOMM_H__1A3B464C_6D12_4517_93CC_514CE43EFF9E__INCLUDED_)
#define AFX_CALCCOMM_H__1A3B464C_6D12_4517_93CC_514CE43EFF9E__INCLUDED_

#include "CalcPidl.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CKermit;

class CCalcComm  
{
public:
	BOOL GoHome();
	BOOL ChangeDir(LPCTSTR sDir);
	BOOL FindFirstFile();
	LPCALCITEM FindNextFile();
	CCalcComm();
	virtual ~CCalcComm();
private:
	LPCALCITEM ParseLine(TCHAR * line);
	TCHAR m_sRemoteDir[4096];
	TCHAR * m_pCurPos;
	TCHAR * m_sEntryToken;
	BOOL ReadRemoteDir();
	CKermit *m_pKermit;
};

#endif // !defined(AFX_CALCCOMM_H__1A3B464C_6D12_4517_93CC_514CE43EFF9E__INCLUDED_)
