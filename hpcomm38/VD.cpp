// VD.cpp: implementation of the CVD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVD::CVD(CString &path)
{
    path_ = path;

	// Check for a trailing backslash
	if (path_.Right(1) != "\\")
		path_ += "\\";
}


CVD::~CVD()
{

}


// Check if the selected directory contains the files
// HP38DIR.CUR and HP38DIR.000
// If yes, tell the user in the status window
// that this dir could be a VD. (or not)
bool CVD::
IsVD()
{
	BOOL bIsVD = FALSE;
	
	CFileFind finder;

    // Can we find an HP38G directory?
	bIsVD = finder.FindFile(path_ + "HP38DIR.000");
    if (bIsVD == TRUE)
    {
        finder.Close();
        // Can we find the second file?
        bIsVD = finder.FindFile(path_ + "HP38DIR.CUR");

        return bIsVD == TRUE;
    }

    finder.Close();
	// Maybe it's an HP39G directory...
	bIsVD = finder.FindFile(path_ + "HP39DIR.000");
    if (bIsVD == FALSE)
        return false;

    // Can we find the second file?
    bIsVD = finder.FindFile(path_ + "HP39DIR.CUR");
    
    return bIsVD == TRUE;
}


bool CVD::
IsTheDirFile(CString &fname)
{
    if (fname.GetLength() < 11)
        return false;

    CString s = fname.Left(8);
    if (s != "HP38DIR." && s != "HP39DIR.")
        return false;

	int nLast = fname.Right(1)[0] - '0';
    if (nLast < 0 || nLast > 9)
        return false;

    return true;
}


bool CVD::
IsTheCurFile(CString &fname)
{
    if (fname.GetLength() < 11)
        return false;

    if (fname == "HP38DIR.CUR" || fname == "HP39DIR.CUR")
        return true;

    return false;
}

bool CVD::
IsAVDFile(CString &fname)
{
	if (fname.GetLength() < 11)
		return false;

	CString sFileName = fname.Left(7);
    return (sFileName == "HP38DIR" || sFileName == "HP39DIR");
}


bool CVD::
IdentifyAndRemoveHeader(CString &header)
{
    CString h = header.Left(8);
    if (h != "HP38AscA" && h != "HP39AscA")
        return false;

    header.Delete(0, 8);

    return true;
}
    

FILE *CVD::
OpenDirFile(const CString *filename)
{
    if (filename != 0)
        return fopen(*filename, "r");

    CString fn = path_ + "HP39DIR.000";
	FILE *fp = fopen(fn, "r");
    if (fp != 0)
        return fp;

    fn = path_ + "HP38DIR.000";
	fp = fopen(fn, "r");

    return fp;
}
