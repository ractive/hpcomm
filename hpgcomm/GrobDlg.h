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


#if !defined(AFX_GROBDLG_H__7B01B633_27D4_11D3_81A7_0000B4A265C3__INCLUDED_)
#define AFX_GROBDLG_H__7B01B633_27D4_11D3_81A7_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GrobDlg.h : header file
//

#include "HPComm38.h"
#include "Image.h"
#include "PicHolder.h"

/////////////////////////////////////////////////////////////////////////////
// CGrobDlg dialog

class CGrobDlg : public CDialog
{
// Construction
public:
	int nScreenDlg;
	CString sCurDir;
	bool bBorderBlack;
	bool bGrayscale;
	unsigned char * screendata;
	CString sSavedAs;
	int nSize;
	//CMenu * pMenu;
	HBITMAP MirrorBitmap(BOOL bLateral);
	BOOL bBMP;
	CString sFileName;
	void PosPic();
	CImage image;
	CGrobDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGrobDlg)
	enum { IDD = IDD_GROBVIEW };
	CPicHolder	m_Pic;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrobDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    void Border(bool on);
	void UpdateBorder();
	bool bBorderThin;
	bool bBorder;

	// Generated message map functions
	//{{AFX_MSG(CGrobDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCopy();
	afx_msg void OnSave();
	afx_msg void OnRotate();
	afx_msg void OnFlip();
	afx_msg void OnMirror();
	afx_msg void OnInvert();
	afx_msg void OnMagnify();
	afx_msg void OnShrink();
	afx_msg void OnReload();
	afx_msg void OnBorder();
	afx_msg void OnClose();
	afx_msg void OnCustomsize();
	afx_msg void OnStandardsize();
	afx_msg void OnBorderblack();
	afx_msg void OnBorderwhite();
	afx_msg void OnBorderthick();
	afx_msg void OnBorderthin();
	afx_msg void OnBwgrob();
	afx_msg void OnGraygrob();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool bPosFirst;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROBDLG_H__7B01B633_27D4_11D3_81A7_0000B4A265C3__INCLUDED_)
