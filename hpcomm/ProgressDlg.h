#if !defined(AFX_PROGRESSDLG_H__B9C01826_3693_11D3_81CF_0000B4A265C3__INCLUDED_)
#define AFX_PROGRESSDLG_H__B9C01826_3693_11D3_81CF_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

class CProgressDlg : public CDialog
{
// Construction
public:
	bool bSend;
// Definitions for the update types
	typedef enum {
		RANGE,		// Used to set the range of the progress bar
					// param = unsigned int* 
		RANGEALL,	// Used to set the range of the progress bar for all files
		FILEPOS,	// How many bytes are transferred of the current file
        ERRORS,		// 	"				How many error occured
        PACKETNO,	// How many packets have been transferred
        FILENAME,	// Used to indicate which file is to be transferred next
					// param = CString*
		HPFILENAME	// Indicates under what filename the HP stores the file (renaming)
	};

	bool * bCancel;
	bool * bSkipFile;
	static void Close();
	BOOL Pump();
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProgressDlg)
	enum { IDD = IDD_PROGRESSDLG };
	CProgressCtrl	ProgressAll;
	CProgressCtrl	ProgressFile;
	//}}AFX_DATA

	// Static member function that is used as a callback function
	// so that CProt can reports its status.
	// Take care what you do with the void ptr. Use the nType to determine what the
	// original var-type was.
	static void UpdateDlg(int nType, void* param);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgressDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSkip();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	// That the static member function UpdateDlg (which has not this pointer) 
	// can access the class members, we provide a static member var pseudoThis
	// which is set to the this pointer in OnCreate.
	static CProgressDlg * pseudoThis;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSDLG_H__B9C01826_3693_11D3_81CF_0000B4A265C3__INCLUDED_)
