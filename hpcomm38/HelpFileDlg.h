#if !defined(AFX_HELPFILEDLG_H__23689791_AD05_11D3_8337_0000B4A265C3__INCLUDED_)
#define AFX_HELPFILEDLG_H__23689791_AD05_11D3_8337_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HelpFileDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHelpFileDlg dialog

class CHelpFileDlg : public CDialog
{
// Construction
public:
	CStringArray * pSa;
	CHelpFileDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHelpFileDlg)
	enum { IDD = IDD_HELPFILEDLG };
	CListBox	m_ListBox;
	// This string contains the choosen help file from the list box.
	CString	m_sHelpFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHelpFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHelpFileDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkHelpfiles();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HELPFILEDLG_H__23689791_AD05_11D3_8337_0000B4A265C3__INCLUDED_)
