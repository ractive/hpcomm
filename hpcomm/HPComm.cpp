//////////////////////////////////////////////////////////////////////
/////////////////////              HPComm            /////////////////
//////////////////////////////////////////////////////////////////////



// HPComm.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "HPComm.h"
#include "MainFrame.h"
#include "HPCommDoc.h"
#include "ListBase.h"
#include "Kermit.h"
#ifdef FILER_TEST
#include "FilerPC.h"
#endif
#include "Splash.h"
#include "ComInfoDlg.h"
#include "NewFilename.h"

#include <afxadv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHPCommApp

BEGIN_MESSAGE_MAP(CHPCommApp, CWinApp)
	//{{AFX_MSG_MAP(CHPCommApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

// This defines the name of our internal clipboard format.
const char* HPCommClipboardFormat = "www.hpcomm.org:dnd:100";

/////////////////////////////////////////////////////////////////////////////
// CHPCommApp construction

CHPCommApp::CHPCommApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHPCommApp object

CHPCommApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CHPCommApp initialization

BOOL CHPCommApp::InitInstance()
{
    // The next line is a tribute to the sheer genius of marketing...
	const char* HPCommWindowTitle = "HP Graphing Calculator PC Connectivity Kit 3.0";

	// This mutex prevents that another instance of the app can be created.
	CreateMutex(NULL, TRUE, _T("HPComm"));
	BOOL bAlreadyRunning = (GetLastError() == ERROR_ALREADY_EXISTS);
	if(bAlreadyRunning) {
		SetForegroundWindow(FindWindow(NULL, HPCommWindowTitle));
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("www.hpcomm.org"));

	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	//m_pRecentFileList->UpdateMenu();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CHPCommDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CListBase));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

#ifdef SPLASH
	CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
#endif

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.

	SetWindowText(m_pMainWnd->m_hWnd, HPCommWindowTitle);

	// Show the cominfodlg for 2 seconds and delete it then.
	CMainFrame * pWnd = (CMainFrame *)m_pMainWnd;
	
	// The startup phase is over. This is needed in the exception handler.
	pWnd->bStartup = false;

	Sleep(1000);
	pWnd->pKermit->pfConnectDeleteCb();

	pWnd->ShowPanes();
//#define FILENAMETEST
#ifdef FILENAMETEST
	CNewFilename dlg;
	dlg.sOldFilename = "Oldname";
	dlg.sNewFilename = "NewName";
	CString sRes;
	switch(dlg.DoModal()) {
	case IDOK: sRes = "OK"; break;
	case IDCANCEL: sRes = "Cancel"; break;
	case IDIGNORE: sRes = "Skip"; break;
	}
	MessageBox(NULL, dlg.sNewFilename + "\n" + sRes + " was hit.", "Test", MB_OK);
#endif
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

    return TRUE;
}


// App command to run the dialog
void CHPCommApp::OnAppAbout()
{
    CComInfoDlg::ShowAboutDlg();
}

/////////////////////////////////////////////////////////////////////////////
// CHPCommApp message handlers


BOOL CHPCommApp::PreTranslateMessage(MSG* pMsg)
{
#ifdef SPLASH
    // CG: The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;
#endif

	return CWinApp::PreTranslateMessage(pMsg);
}

void CHPCommApp::LastErrorMessage(DWORD lastError)
{
	if (lastError == 0)
		return;
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		lastError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);
	// Process any inserts in lpMsgBuf.
	// ...
	// Display the string.
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
	// Free the buffer.
	LocalFree( lpMsgBuf );

}

BOOL CHPCommApp::GetMRUDirName(int n, CString &sName)
{
	if (n > m_pRecentFileList->GetSize())
		return FALSE;
	sName = m_pRecentFileList->m_arrNames[n];
	return !(sName.IsEmpty());
}

const CString CHPCommApp::OurDir()
{
    char path[_MAX_PATH];
    DWORD pathLen = GetModuleFileName(0, path, _MAX_PATH);
    CString cpath(path);

    int lastSepPos = cpath.ReverseFind('\\');
    if (lastSepPos == -1) {
        TRACE("Could not find directory name.\n");
        
        return "";
    }

    cpath = cpath.Left(lastSepPos);

    return cpath;
}

void CHPCommApp::RemoveMRU(int nIndex)
{
	m_pRecentFileList->Remove(nIndex);
}
