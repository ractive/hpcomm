// Overwrite.h: interface for the Overwrite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OVERWRITE_H__B2EE91A0_64D4_11D3_8536_0000B45B139D__INCLUDED_)
#define AFX_OVERWRITE_H__B2EE91A0_64D4_11D3_8536_0000B45B139D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

#define IDYESALL	0
#define IDNOALL		1

class COverwrite : public CDialog
{
// Construction
public:
	COverwrite(CWnd* pParent = NULL);   // standard constructor
	CString sDirname;
	CString sFilename;

// Dialog Data
	//{{AFX_DATA(COverwrite)
	enum { IDD = IDD_OVERWRITE };
	BOOL	bAllFiles;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COverwrite)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COverwrite)
	virtual BOOL OnInitDialog();
	afx_msg void OnYes();
	afx_msg void OnNo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OVERWRITE_H__B2EE91A0_64D4_11D3_8536_0000B45B139D__INCLUDED_)
