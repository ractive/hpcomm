/*************************************************************************
  HPGComm
**************************************************************************

Copyright (C) January 2001 by HP and Jean-Pierre Bergamin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
***************************************************************************/


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
vd_type CVD::
IsVD(BOOL b38Only /* = FALSE */)
{
	BOOL bIs39VD = FALSE;
	BOOL bIs38VD = FALSE;
	BOOL bIsVD = FALSE;

	CFileFind finder;

    // Can we find an HP38G directory?
	bIsVD = finder.FindFile(path_ + "HP38DIR.000");
	finder.Close();

	bIsVD &= finder.FindFile(path_ + "HP38DIR.CUR");
    finder.Close();

	bIs38VD = bIsVD;

	// We did not find the HP38 files
	if (b38Only) {
		return bIs38VD ? HP38_VD : NO_VD;
	}

	// Maybe it's an HP39G directory...
	bIsVD = finder.FindFile(path_ + "HP39DIR.000");

    // Can we find the second file?
    bIsVD &= finder.FindFile(path_ + "HP39DIR.CUR");
    bIs39VD = bIsVD;

	if (bIs38VD && bIs39VD) {
		return BOTH_VD;
	}

	if (bIs38VD) {
		return HP38_VD;
	}

	if (bIs39VD) {
		return HP39_VD;
	}

	return NO_VD;

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

FILE *CVD::
OpenDirFile(CString &filename, BOOL bOnly38 /* = FALSE */)
{
    if (filename != "")
        return fopen(filename, "r");

    CString fn;
	FILE * fp = 0;
	if (!bOnly38) {
		fn = path_ + "HP39DIR.000";
		fp = fopen(fn, "r");
		if (fp != 0) {
			filename = "HP39DIR.000";
			return fp;
		}
	}

    fn = path_ + "HP38DIR.000";
	fp = fopen(fn, "r");
	filename = "HP38DIR.000";

    return fp;
}
