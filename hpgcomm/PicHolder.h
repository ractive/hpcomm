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


#if !defined(AFX_PICHOLDER_H__0AB65991_2A93_11D3_81B3_0000B4A265C3__INCLUDED_)
#define AFX_PICHOLDER_H__0AB65991_2A93_11D3_81B3_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PicHolder.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPicHolder window

class CPicHolder : public CStatic
{
// Construction
public:
	CPicHolder();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPicHolder)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPicHolder();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPicHolder)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICHOLDER_H__0AB65991_2A93_11D3_81B3_0000B4A265C3__INCLUDED_)
