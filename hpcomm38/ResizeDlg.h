#if !defined(AFX_RESIZEDLG_H__D8BB7EA6_A2BC_11D3_8325_0000B4A265C3__INCLUDED_)
#define AFX_RESIZEDLG_H__D8BB7EA6_A2BC_11D3_8325_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResizeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CResizeDlg dialog

class CResizeDlg : public CDialog
{
// Construction
public:
	CResizeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CResizeDlg)
	enum { IDD = IDD_RESIZEDLG };
	int		m_Height;
	int		m_Width;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResizeDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESIZEDLG_H__D8BB7EA6_A2BC_11D3_8325_0000B4A265C3__INCLUDED_)
