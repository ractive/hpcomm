//////////////////////////////////////////////////////////////////////
/////////////////////              HPComm            /////////////////
//////////////////////////////////////////////////////////////////////



// HPComm.h : main header file for the HPCOMM application
//

#if !defined(AFX_HPCOMM_H__9FEEA1E1_03FA_11D3_AE2A_0000E8627120__INCLUDED_)
#define AFX_HPCOMM_H__9FEEA1E1_03FA_11D3_AE2A_0000E8627120__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Filer.h"

/////////////////////////////////////////////////////////////////////////////
// CHPCommApp:
// See HPComm.cpp for the implementation of this class
//

class CHPCommApp : public CWinApp
{
public:
	void RemoveMRU(int nIndex);
    // Returns the directory the program is installed in.
	const CString OurDir();
	BOOL GetMRUDirName(int n, CString &sName);
	void LastErrorMessage(DWORD lastError);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CHPCommApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHPCommApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CHPCommApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


// Enum types of different protocols
enum ProtType {KERMIT, XMODEM, XMODEM1K, XMODEMHP};

// Define calc-types
#define HP38	0
#define HP48	1
#define HP49	2

#define WM_COMDAEMON (WM_USER + 3)

#define INITDLG			1
#define TRYOPENPORT		2
#define CONNECTTOCALC	3
#define GETTINGCALCINFO	4
#define READINGCALC		5
#define READINGPC		6
#define READING			7
#define ENDDLG			8

// This defines a structure which is being sent over the clipboard on an
// internal drag and drop/copy paste operation
struct clipInfo_ {
	CFiler * pFromFiler;
	CFilerEntry *pFromFirstEntry;
};

// This defines the name of our internal clipboard format.
extern const char* HPCommClipboardFormat;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HPCOMM_H__9FEEA1E1_03FA_11D3_AE2A_0000E8627120__INCLUDED_)
