// 38G.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "HPComm38.h"
#include "HPComm38Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHPComm38App

BEGIN_MESSAGE_MAP(CHPComm38App, CWinApp)
	//{{AFX_MSG_MAP(CHPComm38App)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
    ON_COMMAND(ID_HELP_FINDER, CWinApp::OnHelpFinder) 
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHPComm38App construction

CHPComm38App::CHPComm38App()
{
    //First free the string allocated by MFC at CWinApp startup.
    //The string is allocated before InitInstance is called.
    if (m_pszAppName != 0)
        free((void*)m_pszAppName);
    // Change the name of the application file.
    // Unless we do this, the registry entry will be the
    // same as the exe name.  This enables us to change the exe name,
    // but keep the registry key constant.
    //The CWinApp destructor will free the memory.
    m_pszAppName=_tcsdup(_T("HPComm38"));
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHPComm38App object

CHPComm38App theApp;

/////////////////////////////////////////////////////////////////////////////
// CHPComm38App initialization

BOOL CHPComm38App::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	const char *HPComm38WindowTitle = "HPComm38";

	// This mutex prevents that another instance of the app can be created.
	CreateMutex(NULL, TRUE, _T("HPComm38"));
	BOOL bAlreadyRunning = (GetLastError() == ERROR_ALREADY_EXISTS);
    if(bAlreadyRunning) {
        SetForegroundWindow(FindWindow(NULL, HPComm38WindowTitle));
		return FALSE;
    }

	OleInitialize(NULL);

	SetRegistryKey(_T("www.hpcomm.org"));

	CHPComm38Dlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
