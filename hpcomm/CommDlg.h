//////////////////////////////////////////////////////////////////////
/////////////////////              HPComm            /////////////////
//////////////////////////////////////////////////////////////////////



#if !defined(AFX_COMMDLG_H__35A4A685_D95B_11D2_B351_0000E8627120__INCLUDED_)
#define AFX_COMMDLG_H__35A4A685_D95B_11D2_B351_0000E8627120__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CommDlg.h : header file
//

class CSettings;

/////////////////////////////////////////////////////////////////////////////
// CCommDlg dialog

class CCommDlg : public CDialog
{
// Construction
public:
	CCommDlg(CSettings *pSettings);

	// Show all elements in the dialog box
	void Set4xMode();
	// Hide all the elements that are not used with the 38
	void Set38Mode();
	// Show a button that was hidden by HideButton()
	void ShowButton(CButton * pButton);
	// Hide a button
	void HideButton(CButton * pButton);
	// Enable a button that was disabled
	void EnableButton(CButton * pButton);
	// Disable a button
	void DisableButton(CButton * pButton);

// Dialog Data
	//{{AFX_DATA(CCommDlg)
	enum { IDD = IDD_COMMSET };
	CComboBox	m_Calc;
	CComboBox	m_Transio;
	CComboBox	m_Checksum;
	CComboBox	m_Baud;
	CButton	m_Com1;
	CButton	m_Com2;
	CButton	m_Com3;
	CButton	m_Com4;
	BOOL	bFinish;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSettings *pSettings;

	// Generated message map functions
	//{{AFX_MSG(CCommDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeCalc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMDLG_H__35A4A685_D95B_11D2_B351_0000E8627120__INCLUDED_)
