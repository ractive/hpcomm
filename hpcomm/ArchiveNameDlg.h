#if !defined(AFX_ARCHIVENAMEDLG_H__70DD1D43_4445_11D3_8203_0000B4A265C3__INCLUDED_)
#define AFX_ARCHIVENAMEDLG_H__70DD1D43_4445_11D3_8203_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArchiveNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CArchiveNameDlg dialog

class CArchiveNameDlg : public CDialog
{
// Construction
public:
	CArchiveNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CArchiveNameDlg)
	enum { IDD = IDD_ARCHIVENAME };
	CString	m_ArchiveName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArchiveNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CArchiveNameDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARCHIVENAMEDLG_H__70DD1D43_4445_11D3_8203_0000B4A265C3__INCLUDED_)
