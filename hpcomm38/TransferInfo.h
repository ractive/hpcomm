#if !defined(AFX_TRANSFERINFO_H__D8BB7EAC_A2BC_11D3_8325_0000B4A265C3__INCLUDED_)
#define AFX_TRANSFERINFO_H__D8BB7EAC_A2BC_11D3_8325_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransferInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransferInfo dialog

class CTransferInfo : public CDialog
{
// Construction
public:
	void ShowInitScreen();
	void ShowUserInfo();
	void Close();
	void Pump();
	static void UpdateDlg(CString& sFileName, bool bRecv);
	CTransferInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTransferInfo)
	enum { IDD = IDD_TRANSFERINFO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransferInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTransferInfo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	UINT TimerID;
	bool bMainWndWasEnabled;
	static CTransferInfo * pseudoThis;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSFERINFO_H__D8BB7EAC_A2BC_11D3_8325_0000B4A265C3__INCLUDED_)
