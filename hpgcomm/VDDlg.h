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


#if !defined(AFX_VDDLG_H__12DFB5A4_A6B6_11D3_832C_0000B4A265C3__INCLUDED_)
#define AFX_VDDLG_H__12DFB5A4_A6B6_11D3_832C_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VDDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVDDlg dialog

class CVDDlg : public CDialog
{
// Construction
public:
	CString m_sPath;
	CVDDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVDDlg)
	enum { IDD = IDD_VDDLG };
	CTreeCtrl	VDTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVDDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVDDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnCreatedir();
	afx_msg void OnSelchangingVdtree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString GetNewVDDirName();
	bool RemoveReadOnly(CString &sFilePath);
	bool SetReadOnly(CString &sFilePath);
	char * GetVDFileName(CString &sDir);
	bool IsDir(CString &sName);
	CPtrArray dirArray;
	struct dirEntry {
		char sName[128];
		char sHostName[128];
		char sParent[128];
	};

	void getIconsFromType(char type, const CString &sName, int &nIcon, int &nSelIcon);
	void getNextCountedWord(CString &source, CString &word);
	void getNextWord(CString &source, CString &word);
	bool getFileContent(CString &sText, CString &sFileName);
	bool ReadVD(HTREEITEM hParent, CString &sFileName);
	CImageList * pImageList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VDDLG_H__12DFB5A4_A6B6_11D3_832C_0000B4A265C3__INCLUDED_)
