#if !defined(AFX_NEWFILENAME_H__8BA2CFA0_5BC2_11D3_823C_0000B4A265C3__INCLUDED_)
#define AFX_NEWFILENAME_H__8BA2CFA0_5BC2_11D3_823C_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewFilename.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewFilename dialog

class CNewFilename : public CDialog
{
// Construction
public:
	CString sOldFilename;
	//CString* sNewFilename;
	CNewFilename(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewFilename)
	enum { IDD = IDD_FILENAME };
	CString	sNewFilename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewFilename)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewFilename)
	virtual BOOL OnInitDialog();
	afx_msg void OnSkip();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWFILENAME_H__8BA2CFA0_5BC2_11D3_823C_0000B4A265C3__INCLUDED_)
