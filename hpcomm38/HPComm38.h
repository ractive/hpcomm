// 38G.h : main header file for the 38G application
//

#if !defined(AFX_38G_H__59086614_A28E_11D3_8324_0000B4A265C3__INCLUDED_)
#define AFX_38G_H__59086614_A28E_11D3_8324_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// A WM_COMDAEMON message is sent to the window which created this thread to
// indicate that the thread ended.
#define WM_COMDAEMON (WM_USER + 3)
#define COMACTIVITY		0
#define COMDAEMONDIED	1

#define WM_TRANSFERINFOCLOSED (WM_USER + 5)
#define WM_DIRECTORYCHANGE (WM_USER + 7)


/////////////////////////////////////////////////////////////////////////////
// CHPComm38App:
// See 38G.cpp for the implementation of this class
//

class CHPComm38App : public CWinApp
{
public:
	CHPComm38App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHPComm38App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CHPComm38App)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_38G_H__59086614_A28E_11D3_8324_0000B4A265C3__INCLUDED_)
