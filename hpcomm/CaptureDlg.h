#if !defined(AFX_CAPTUREDLG_H__A3FA8443_2A33_11D3_81B2_0000B4A265C3__INCLUDED_)
#define AFX_CAPTUREDLG_H__A3FA8443_2A33_11D3_81B2_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaptureDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCaptureDlg dialog

#define CAPTURE_SUCCESS		0
#define CAPTURE_CANCELLED	1
#define CAPTURE_FAILED		2


#include "Settings.h"
#include "Prot.h"

struct thread_ {
	HANDLE *	handles;
	HWND		hWnd;
};


class CCaptureDlg : public CDialog
{
// Construction
public:
	unsigned char * data;
	bool ReadScreen();
    // Returns a ptr to the screen data we read in, or 0.
    // Client has the responsibility of deleting this.
    unsigned char * GetScreenData();
	CProt * pProt;
	CSettings * pSettings;
	HANDLE * handles;
	thread_ * thread;
	HANDLE hComPort;
	DWORD evMask;
	OVERLAPPED ov;
	HANDLE hCancel;
	CCaptureDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCaptureDlg)
	enum { IDD = IDD_CAPTURE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaptureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCaptureDlg)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void OnMsg(UINT pParam, long lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAPTUREDLG_H__A3FA8443_2A33_11D3_81B2_0000B4A265C3__INCLUDED_)
