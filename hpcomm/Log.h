// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__8F6163CE_55DF_11D3_98D0_0060B0A2563C__INCLUDED_)
#define AFX_LOG_H__8F6163CE_55DF_11D3_98D0_0060B0A2563C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLog  
{
public:
    static bool Serial(const char *fmt, ...);
    static bool Program(const char *fmt, ...);

    virtual ~CLog();

private:
	CLog();

    bool writeToFile(const CString &name, const CString &msg);

    CString serialName_;
    CString programName_;

    static CLog *instance_;
};

#endif // !defined(AFX_LOG_H__8F6163CE_55DF_11D3_98D0_0060B0A2563C__INCLUDED_)
