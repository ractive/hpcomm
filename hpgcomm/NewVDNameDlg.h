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


#if !defined(AFX_NEWVDNAMEDLG_H__C1EE6384_D624_4EF4_B031_982F6F2C2C8A__INCLUDED_)
#define AFX_NEWVDNAMEDLG_H__C1EE6384_D624_4EF4_B031_982F6F2C2C8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewVDNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewVDNameDlg dialog

class CNewVDNameDlg : public CDialog
{
// Construction
public:
	CNewVDNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewVDNameDlg)
	enum { IDD = IDD_NEWVD };
	CString	m_sNewName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewVDNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewVDNameDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWVDNAMEDLG_H__C1EE6384_D624_4EF4_B031_982F6F2C2C8A__INCLUDED_)
