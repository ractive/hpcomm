#if !defined(AFX_COMINFODLG_H__8F49E241_257D_11D3_9B08_0000B4A265C3__INCLUDED_)
#define AFX_COMINFODLG_H__8F49E241_257D_11D3_9B08_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComInfoDlg.h : header file
//

#include "StaticColor.h"
#include "BitmapDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CComInfoDlg dialog

class CComInfoDlg : public CBitmapDialog
{
// Construction
public:
	//enum STAGE {START, OPENPORT, CONNECTTOCALC, GETTINGCALCINFO, READINGCALC, READINGPC};
	static void NewDlg(int nComPort);
	static void SetPort(int nComPort);
	static void BumpDlg(int stage, bool bSuccess = true);
	static void DeleteDlg();
	static void BringToTop();

    static void ShowAboutDlg();

	CComInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CComInfoDlg)
	enum { IDD = IDD_COMINFO };
	CStatic	m_Tick4;
	CStatic	m_Tick3;
	CStatic	m_Tick2;
	CStatic	m_Tick1;
	CStatic	m_Tick0;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SetSplash(bool bSplash);
	CBrush m_Brush;
	static int nComPort;
	// Instance ptr for callback.
	static CComInfoDlg *instance_;
    void SetText();
	
	// Generated message map functions
	//{{AFX_MSG(CComInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CStaticColor m_Stage0;
	CStaticColor m_Stage1;
	CStaticColor m_Stage2;
	CStaticColor m_Stage3;
	CStaticColor m_Stage4;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMINFODLG_H__8F49E241_257D_11D3_9B08_0000B4A265C3__INCLUDED_)
