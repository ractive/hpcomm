#if !defined(AFX_DIREDITCTRL_H__12DFB5A3_A6B6_11D3_832C_0000B4A265C3__INCLUDED_)
#define AFX_DIREDITCTRL_H__12DFB5A3_A6B6_11D3_832C_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirectoryEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDirEditCtrl window

class CDirEditCtrl : public CEdit
{
// Construction
public:
	CDirEditCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirEditCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDirEditCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDirEditCtrl)
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIREDITCTRL_H__12DFB5A3_A6B6_11D3_832C_0000B4A265C3__INCLUDED_)
