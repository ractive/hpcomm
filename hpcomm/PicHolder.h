//////////////////////////////////////////////////////////////////////
/////////////////////              HPComm            /////////////////
//////////////////////////////////////////////////////////////////////
//
//	This class is used within CGrogDlg. It is derived from CStatic.
// 
//	It handles mouse messages over the static window which displays
//	the bitmap.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_PICHOLDER_H__0AB65991_2A93_11D3_81B3_0000B4A265C3__INCLUDED_)
#define AFX_PICHOLDER_H__0AB65991_2A93_11D3_81B3_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PicHolder.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPicHolder window

class CPicHolder : public CStatic
{
// Construction
public:
	CPicHolder();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPicHolder)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPicHolder();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPicHolder)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICHOLDER_H__0AB65991_2A93_11D3_81B3_0000B4A265C3__INCLUDED_)
