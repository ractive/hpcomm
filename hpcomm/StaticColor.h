#if !defined(AFX_STATICCOLOR_H__D4106D74_3975_11D3_81E5_0000B4A265C3__INCLUDED_)
#define AFX_STATICCOLOR_H__D4106D74_3975_11D3_81E5_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticColor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStaticColor window

class CStaticColor : public CStatic
{
// Construction
public:
	CStaticColor();

// Attributes
public:

// Operations
public:
	void SetTextColor(COLORREF crTextColor = 0xffffffff);
	COLORREF GetTextColor();

	void SetBkColor(COLORREF crBkColor = 0xffffffff);
	COLORREF GetBkColor();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticColor)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStaticColor();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticColor)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	COLORREF m_crTextColor;

	COLORREF m_crBkColor;

	CBrush m_brBkgnd;

	CWnd* m_pParent;
	UINT m_nMsg;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICCOLOR_H__D4106D74_3975_11D3_81E5_0000B4A265C3__INCLUDED_)
