// VD.h: interface for the CVD class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VD_H__3F366D2D_1691_11D4_9909_0060B0A2563C__INCLUDED_)
#define AFX_VD_H__3F366D2D_1691_11D4_9909_0060B0A2563C__INCLUDED_

#include <stdio.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVD  
{
public:
    CVD(CString &path);
    virtual ~CVD();
    
    bool IsVD();
    bool IsTheDirFile(CString &fname);
    bool IsTheCurFile(CString &fname);
    bool IsAVDFile(CString &fname);
    static bool IdentifyAndRemoveHeader(CString &header);
    FILE *OpenDirFile(const CString *fname);

private:
    CString path_;
};

#endif // !defined(AFX_VD_H__3F366D2D_1691_11D4_9909_0060B0A2563C__INCLUDED_)
