#if !defined(AFX_GROBFORMATDLG_H__D5900B33_4965_11D3_8214_0000B4A265C3__INCLUDED_)
#define AFX_GROBFORMATDLG_H__D5900B33_4965_11D3_8214_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GrobFormatDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGrobFormatDlg dialog

class CGrobFormatDlg : public CDialog
{
// Construction
public:
	int nSel;
	CGrobFormatDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGrobFormatDlg)
	enum { IDD = IDD_GROBFORMATDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrobFormatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGrobFormatDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROBFORMATDLG_H__D5900B33_4965_11D3_8214_0000B4A265C3__INCLUDED_)
