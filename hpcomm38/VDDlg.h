#if !defined(AFX_VDDLG_H__12DFB5A4_A6B6_11D3_832C_0000B4A265C3__INCLUDED_)
#define AFX_VDDLG_H__12DFB5A4_A6B6_11D3_832C_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VDDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVDDlg dialog

class CVDDlg : public CDialog
{
// Construction
public:
	CString sPath;
	CVDDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVDDlg)
	enum { IDD = IDD_VDDLG };
	CTreeCtrl	VDTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVDDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVDDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void getIconsFromType(char type, const CString &sName, int &nIcon, int &nSelIcon);
	void getNextCountedWord(CString &source, CString &word);
	void getNextWord(CString &source, CString &word);
	bool getFileContent(CString &sText, const CString *sFileName);
	bool ReadVD(HTREEITEM hParent, const CString *sFileName);
	CImageList * pImageList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VDDLG_H__12DFB5A4_A6B6_11D3_832C_0000B4A265C3__INCLUDED_)
