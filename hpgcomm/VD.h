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


// VD.h: interface for the CVD class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VD_H__3F366D2D_1691_11D4_9909_0060B0A2563C__INCLUDED_)
#define AFX_VD_H__3F366D2D_1691_11D4_9909_0060B0A2563C__INCLUDED_

#include <stdio.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Return values of isVD
/*#define HP38_VD 1
#define HP39_VD 2
#define NO_VD 0*/

typedef enum {NO_VD, HP38_VD, HP39_VD, BOTH_VD} vd_type;

class CVD  
{
public:
    CVD(CString &path);
    virtual ~CVD();
    
    vd_type IsVD(BOOL b38Only = FALSE);
    bool IsTheDirFile(CString &fname);
    bool IsTheCurFile(CString &fname);
    bool IsAVDFile(CString &fname);
    FILE *OpenDirFile(CString &fname, BOOL bOnly38 = FALSE);

private:
    CString path_;
};

#endif // !defined(AFX_VD_H__3F366D2D_1691_11D4_9909_0060B0A2563C__INCLUDED_)
