/*************************************************************************
  HPGComm
**************************************************************************

Copyright (C) January 2001 by HP and Jean-Pierre Bergamin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
***************************************************************************/


// 38GDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HPComm38.h"
#include "HPComm38Dlg.h"
#include "AboutDlg.h"
#include "GrobDlg.h"
#include "Kermit.h"
#include "VDDlg.h"
#include "HelpFileDlg.h"
#include <imagehlp.h>
#include "VD.h"
#include "XModem.h"
#include <afxpriv.h>              // For WM_KICKIDLE


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHPComm38Dlg *CHPComm38Dlg::m_pPseudoThis = 0;

/////////////////////////////////////////////////////////////////////////////
// CHPComm38Dlg dialog

CHPComm38Dlg::CHPComm38Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHPComm38Dlg::IDD, pParent), rb("Settings")
{
	//{{AFX_DATA_INIT(CHPComm38Dlg)
	m_sPath = _T("C:\\");
	m_sInfo = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nComPort = 1;

	rb.Bind(m_nComPort, "Com-port", 1);
	rb.Bind(m_sPath, "Last directory", "C:\\");
	rb.Bind(m_sHelpLanguage, "Help-language", "");

	VERIFY(m_pPseudoThis==0);
	m_pPseudoThis = this;

	m_pWaitInfoDlg = 0;
	m_pScreenData = 0;

	m_bVDInitInfoShown = false;

	m_nScreenDlg = 0;

	m_bDirInexistent = FALSE;

}

void CHPComm38Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHPComm38Dlg)
	DDX_Control(pDX, IDC_CALPIC, m_CalcPic);
	DDX_Control(pDX, IDC_DIRECTORY, m_DirEditBox);
	DDX_Control(pDX, IDC_SHOWVD, m_ShowVD);
	DDX_Control(pDX, IDC_CREATEDIR, m_CreateDir);
	DDX_Control(pDX, IDC_INFOSTATIC, m_InfoStatic);
	DDX_Control(pDX, IDC_INFO, m_InfoCtrl);
	DDX_Control(pDX, IDC_COMPORT, ComPortCtrl);
	DDX_Text(pDX, IDC_DIRECTORY, m_sPath);
	DDX_Text(pDX, IDC_INFO, m_sInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHPComm38Dlg, CDialog)
	//{{AFX_MSG_MAP(CHPComm38Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CHANGEDIR, OnChangeDir)
	ON_WM_CLOSE()
	ON_BN_CLICKED(ID_HELP_ABOUT, OnHelpAbout)
	ON_BN_CLICKED(IDC_SHOWVD, OnShowVD)
	ON_COMMAND(ID_HELP_FINDER, OnHelpFinder)
	ON_COMMAND(ID_HELP_LANGUAGE, OnHelpLanguage)
	ON_CBN_SELCHANGE(IDC_COMPORT, OnSelchangeComport)
	ON_BN_CLICKED(IDC_CREATEDIR, OnCreateDir)
	ON_BN_CLICKED(IDC_HIDEINFO, HideInfo)
	ON_BN_CLICKED(IDC_GETXMODEM, OnGetXModem)
	ON_EN_CHANGE(IDC_DIRECTORY, OnChangeDirectory)
	ON_WM_TIMER()
	ON_COMMAND(ID_RESTORE, OnRestore)
	ON_WM_SIZE()
	ON_CBN_EDITCHANGE(IDC_DIRECTORY, OnEditchangeDirectory)
	ON_CBN_SELCHANGE(IDC_DIRECTORY, OnSelchangeDirectory)
    ON_COMMAND(ID_HELP, OnHelpFinder)
	ON_COMMAND(ID_DEFAULT_HELP, OnHelpFinder)
	ON_CBN_DROPDOWN(IDC_DIRECTORY, OnDropdownDirectory)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, ToolTipCallback)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_COMDAEMON, OnComdaemonMsg)
	ON_MESSAGE(WM_TRANSFERINFOCLOSED, OnTransferInfoClosed)
	ON_MESSAGE(WM_DIRECTORYCHANGE, OnChangeDir)
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHPComm38Dlg message handlers

BOOL CHPComm38Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// Create the menu
    m_pMenu = new CMenu;
	if (m_pMenu && m_pMenu->LoadMenu(IDR_MAINFRAME)) {
		// and set it.
		SetMenu(m_pMenu);
	}

	HideInfo();

	FillHistoryCombo();

	// Make sure the directory entry field contains the correct dir.
	UpdateData(FALSE);

	m_pScreenData = 0;

	m_pProt = new CKermit();

	if (m_pProt == NULL)
		return false;

	try {
		m_pProt->OpenPort(m_nComPort);
	}
	catch (CProtException * pEx) {
		// Ensure the dialog is friendlier when we first start.
		if (pEx->error == CProtException::OPENPORT) {
			CString sMsg;
			sMsg.Format("Welcome to HPGComm.  I couldn't use your computer's COM%d serial port.\r\nAfter closing this message, please choose a different serial port\r\nfrom the provided list.", m_nComPort);
			MessageBox(sMsg, "Welcome", MB_OK | MB_ICONINFORMATION);
			pEx->Delete();
		}
		else {
			ExceptionHandler(pEx);
		}
	}

	HICON hIcon = NULL;
	hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_NEWDIR), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	if (hIcon) {
		m_CreateDir.SetIcon(hIcon);
	}

	hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_CALC), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	if (hIcon) {
		m_ShowVD.SetIcon(hIcon);
	}


	hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_CHANGEDIR), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	if (hIcon) {
		((CButton *)GetDlgItem(IDC_CHANGEDIR))->SetIcon(hIcon);
	}

	hIcon = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_HIDEINFO), IMAGE_ICON, 16, 8, LR_DEFAULTCOLOR);
	if (hIcon) {
		((CButton *)GetDlgItem(IDC_HIDEINFO))->SetIcon(hIcon);
	}

	// Tooltip initialization
	RECT rect, whole_rect;
	GetClientRect(&whole_rect);
	ClientToScreen(&whole_rect);

	EnableToolTips();
	m_ToolTip.Create(this);
	m_ToolTip.SetDelayTime(500);

	//m_ToolTip.AddTool(&m_CreateDir, _T("Create new directory"));
	m_ToolTip.AddTool(&m_CreateDir);
	m_ToolTip.AddTool(GetDlgItem(IDC_CHANGEDIR));
	//m_ToolTip.AddTool(&m_ShowVD, _T("Show the calculator files"));
	m_ToolTip.AddTool(&m_ShowVD);
	m_ToolTip.AddTool(GetDlgItem(IDC_COMPORT));
	m_ToolTip.AddTool(GetDlgItem(IDC_DIRECTORY));
	m_ToolTip.AddTool(GetDlgItem(IDC_HIDEINFO));
	
	m_CalcPic.GetClientRect(&rect);
	m_CalcPic.ClientToScreen(&rect);
	int nWidth = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;
	rect.top = rect.top - whole_rect.top;
	rect.bottom = rect.top + nHeight;
	rect.left = rect.left - whole_rect.left;
	rect.right = rect.left + nWidth;

	m_ToolTip.AddTool(this, _T(""), &rect, IDC_CALCPIC);

	m_pProt->CreateComDaemon(m_hWnd);
	m_pProt->UnleashDaemon();

	ComPortCtrl.SetCurSel(m_nComPort - 1);

	/*CString s = "Just testing...\r\n\r\nLine 2";
	ShowInfo(s);*/

	HandleCreateDirButton();

	//m_TrayIcon.SetNotificationWindow(this, WM_TRAYNOTIFICATION);
	//m_TrayIcon.SetMenu(IDR_TRAY);

	SetCalcPic(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHPComm38Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHPComm38Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHPComm38Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CHPComm38Dlg::OnComdaemonMsg(UINT pParam, long lParam) 
{
	BOOL bIsXModem = FALSE;
	if (lParam == COMACTIVITY) {
		UpdateData(TRUE);

		// Hide the info window.
		HideInfo();

		UpdateData();

		try {
            // First we have to check if the data at the Com-port is a screenshot
            // of a kermit-packet.
            // If the first char is 27 it's a screenshot for 48 & 38
            // If the second char is 'W' or 'G', it is a screenshot for 49/39/40
            // Otherwise it's a kermit-packet.
            char cFirstChar;
            char cSecondChar;
            if (!m_pProt->ReadCharFromCom(&cFirstChar)) {
                throw new CProtException(CProtException::TIMEOUT, 0, "");
            }
            if (cFirstChar == 0x15 || cFirstChar == 'D') {
				CXModem XModem;
				bIsXModem = TRUE;
				m_pProt->ClosePort();
				XModem.OpenPort(m_nComPort);
				XModem.m_sCurPCPath = m_sPath;
				XModem.Transfer(true);
				XModem.ClosePort();
				m_pProt->OpenPort(m_nComPort);
				m_pProt->Reset();
				m_pProt->KillComDaemon();
				m_pProt->CreateComDaemon(m_hWnd);
				//m_pProt->UnleashDaemon();
				bIsXModem = FALSE;
				//return;
			}
            else if (cFirstChar == 27) {
                // We have a screen shot waiting to be shown...
				CWaitCursor wait;
                ReadScreenData(cFirstChar);
            }
            else {
                if (!m_pProt->ReadCharFromCom(&cSecondChar)) {
                    throw new CProtException(CProtException::TIMEOUT, 0, "");
                }

                if ((cSecondChar == 'W') || (cSecondChar == 'G')) {
                	CWaitCursor wait;
					ReadScreenData(cSecondChar);
                }
                else {
                    m_pProt->ClientRequest(cFirstChar, cSecondChar, m_sPath);
					UpdateHistory(m_sPath);
				}
            }
        }

		catch (CProtException * pEx) {
			ExceptionHandler(pEx);
			if (bIsXModem) {
				/*XModem.ClosePort();
				m_pProt->OpenPort(m_nComPort);
				m_pProt->Reset();
				m_pProt->KillComDaemon();
				m_pProt->CreateComDaemon(m_hWnd);
				m_pProt->UnleashDaemon();
				bIsXModem = FALSE;*/
			}
			else {
				m_pProt->Reset();
				m_pProt->UnleashDaemon();
			}
			return;
		}

		// If the transaction was finished (this means the 38G wrote
		// a directory file to disk and a new file was created on the
		// PC, we reread the content of the PC dir, too.
		if (m_pProt->m_bTransactionFinished) {
			m_pProt->m_bNewFileCreated = false;
			m_pProt->m_bTransactionFinished = false;
		}

		// The screendata contains something. This means for us to display
		// The screenshot.
		if (m_pScreenData != 0) {
			ShowScreenShot();
		}

		// TRACE("Daemon resumed by CMainFrame\r\n");
		m_pProt->UnleashDaemon();
	}

}

void CHPComm38Dlg::ExceptionHandler(CProtException *pEx)
{
	CString sError;
	CVD vd(m_sPath);
	BOOL bDirError = FALSE;

	switch (pEx->error) {
	case CProtException::SYNTAX:
		sError = "Invalid Syntax\r\n\r\nThe file does not\r\ncontain valid data or a valid program.";
		break;
	case CProtException::LOWBATTERY:
		sError = "The calculator batteries are running low.  To ensure reliable communications,\r\nplease replace them before you use this program again.";
		break;
	case CProtException::MEMORY:
		sError = "The calculator is out of memory.\r\n\r\nPlease delete some unused variables on your calculator\r\nand try again.";
		break;
	case CProtException::OPENPORT:
		sError.Format("The COM%d serial port could not be opened.\r\n\r\nPlease choose another serial port.", m_nComPort);
		break;
	case CProtException::DEVICE:
		sError = "The serial port could not be initialized correctly.";
		break;
	case CProtException::DIRERROR:
		sError = "The directory " + m_sPath + " does not exist.";
		break;
	case CProtException::FILEERROR:
		sError = "The file " + pEx->sMessage + " could not be opened\r\nIt may be open in another application\r\nor the filename you entered on the calculator is incorrect.";
		break;
	case CProtException::VDINIT:
		sError = "The current directory (" + m_sPath + ") is not an initialized \r\ndirectory.\r\n\r\nChoose YES on the calculator to initialize it.";
		break;
	case CProtException::UNKNOWN:
		sError = "An unknown error occured.";
		break;
	case CProtException::RETRIES:
	case CProtException::TIMEOUT:
		sError = "There was no answer from the calculator.\r\n\r\nPlease try again after checking the connection.";
		break;
	case CProtException::CANCELLED:
		sError = "The Transfer was cancelled.";
		break;
	default: sError = "Error message from the calculator:\r\n\r\n" + pEx->sMessage;
	}

	if ((pEx->dwError != 0) && (pEx->error != CProtException::OPENPORT)) {
		// There's a windows error code.
		// Convert that to a string and show it, too.
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			pEx->dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);

		// Add the string to the sError.
		sError += "\r\n\r\nThe Windows error message for this error is:\r\n\r\n";
		sError += (LPCTSTR)lpMsgBuf;
		
		// Free the buffer.
		LocalFree( lpMsgBuf );
	}

	// Update the info window with this info.
	if (!m_bVDInitInfoShown || pEx->error != CProtException::VDINIT) {
		ShowInfo(sError);
		MessageBeep(MB_ICONEXCLAMATION);
		m_bVDInitInfoShown = true;
	}
	else {
		HideInfo();
	}

	m_bVDInitInfoShown = vd.IsTheCurFile(pEx->sMessage);

	if (m_pProt) {
		m_pProt->Reset();
		m_pProt->FlushComPort();
		m_pProt->KillComDaemon();
		m_pProt->CreateComDaemon(m_hWnd);
		m_pProt->UnleashDaemon();
	}

	if (pEx->error == CProtException::DIRERROR && pEx->dwError != -1) {
		m_bDirInexistent = TRUE;
		OnCreateDir();
	}
	
	if (pEx->error == CProtException::RETRIES) {
		OSVERSIONINFO osinfo;
		osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osinfo);
		if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
			if(MessageBox("There was no answer from the calculator.\r\n\r\nDue to stability problems of Windows 95/98, we recommend that you restart HPGComm.\r\n\r\nOtherwise the program or even Windows can crash.\r\n\r\nDo you now want to close the program now? (Recommended)", "ALERT", MB_YESNO | MB_ICONEXCLAMATION) == IDYES) {
				pEx->Delete();
				EndDialog(1);
			}
		}
	}	
	
	pEx->Delete();
}

void CHPComm38Dlg::OnChangeDir() 
{
	LPMALLOC pMalloc;
            	
	LPITEMIDLIST pidl;
	// Get some memory for the ID-List SHBrowseForFolder will return
	if (SUCCEEDED(SHGetMalloc(&pMalloc))) {
		
		// Make sure the m_sPath var contains the correct string.
		UpdateData();

		// The directory name will be in there.
		TCHAR szDisplayName[MAX_PATH] = "";
		BROWSEINFO binfo = {0};
		binfo.hwndOwner = m_hWnd; 
		// NULL means that the Desktop is the root.
		binfo.pidlRoot = NULL; 
		binfo.pszDisplayName = szDisplayName; 
		binfo.lpszTitle = "Please choose a directory:"; 

		// BIF_RETURNONLYFSDIRS:	The OK button is only activated if we choose
		//							a filesystem directory
		// BIF_STATUSTEXT:			Show a status text.
		binfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT; 

		// Set the callback function.
		binfo.lpfn = CHPComm38Dlg::BrowseCallbackProc; 
		binfo.lParam = 0;
		
		// Show the little "please wait" dialog.
		if (m_pWaitInfoDlg == 0) {
			m_pWaitInfoDlg = new CDialog();
			m_pWaitInfoDlg->Create(IDD_WAITINFO, this);
		}

		EnableWindow(FALSE);

		// Now display the dialog box where the user can choose the directory.
		pidl = SHBrowseForFolder(&binfo);

		if (pidl) {
			SHGetPathFromIDList(pidl,szDisplayName);
			m_sPath = szDisplayName;
			UpdateData(FALSE);
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
		
		HandleCreateDirButton();

		EnableWindow(TRUE);
	}

	UpdateData();
	SetCalcPic(FALSE);
}

int CALLBACK CHPComm38Dlg::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	VERIFY(m_pPseudoThis!=0);
	switch(uMsg) {
	case BFFM_INITIALIZED: {
		if (!m_pPseudoThis->m_sPath.IsEmpty()) {
			// We want that the box shows our directory selected in the beginning.


			// WParam is TRUE since you are passing a path.
			// It would be FALSE if you were passing a pidl.
			::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)m_pPseudoThis->m_sPath);
		}

		break;
	}
	case BFFM_SELCHANGED: {
		if (m_pPseudoThis->m_pWaitInfoDlg != 0) {
			// The window is initialized.
			// We delete the "wait info" dialog.
			m_pPseudoThis->m_pWaitInfoDlg->DestroyWindow();
			delete m_pPseudoThis->m_pWaitInfoDlg;
			m_pPseudoThis->m_pWaitInfoDlg = 0;		
		}

		CString sFile;
		LPSTR psFile = sFile.GetBuffer(MAX_PATH);

		if (psFile == 0) {
			sFile.ReleaseBuffer();
			break;
		}

		// Get the path of the selection.
		if (SHGetPathFromIDList((LPITEMIDLIST)lp ,psFile)) {
			// Check if the selected directory contains the files
			// HP38DIR.CUR and HP38DIR.000
			// If yes, tell the user in the status window
			// that this dir could be a VD. (or not)
			
			sFile.ReleaseBuffer();
			CString sMessage = "This directory ";

			CVD vd(sFile);
			vd_type type = vd.IsVD();
            if (type == HP38_VD) {
				sMessage += "contains HP38 files.";
			}
			else if(type == HP39_VD) {
				sMessage += "contains HP39 files.";
			}
			else {
				sMessage += "does not contain calculator files.";	
			}

			// Update the status text.
			::SendMessage(hwnd, BFFM_SETSTATUSTEXT,0,(LPARAM)(LPTSTR)(LPCTSTR)sMessage);
		}
		else {
			sFile.ReleaseBuffer();
		}

		break;
	}
	
	default:
		break;

	}
	return 0;
}

void CHPComm38Dlg::ReadScreenData( char ScreenType )
{
    unsigned int nBytesToRead = 1080 - 1;
    
    switch (ScreenType) {
    case 27: 
        nBytesToRead = 1080 - 1;
        break;
    case 'G':
        nBytesToRead = 64 * 17 * 2;
        break;
    case 'W':
        nBytesToRead = 64 * 17;
        break;
    }

    m_pScreenData = new unsigned char [nBytesToRead+1];
    *m_pScreenData = ScreenType;
	DWORD dwRead;
	if ((dwRead = m_pProt->ReadBufferFromCom((char *)(m_pScreenData + 1), nBytesToRead)) != nBytesToRead) {
		MessageBox("There was a problem collecting the screen image", "Screen image error", MB_OK|MB_ICONEXCLAMATION);
		delete [] m_pScreenData;
		m_pScreenData = 0;
	}
}
void CHPComm38Dlg::ShowScreenShot()
{
	CGrobDlg grobDlg(this);
	grobDlg.sFileName.Empty();
	grobDlg.sCurDir = m_sPath;
	grobDlg.screendata = m_pScreenData;
	grobDlg.nScreenDlg = m_nScreenDlg;
	m_pScreenData = 0;
	m_pProt->UnleashDaemon();
	m_nScreenDlg++;
	grobDlg.DoModal();
	m_nScreenDlg--;

	EnableWindow(m_nScreenDlg == 0);

	// We delete the pointer in the dialog, cause when we make more than one screenshot
	// the pScreenData only contains the last one.
	// This way every char * is deleted, not only the last one.
	delete [] grobDlg.screendata;
	grobDlg.screendata = 0;
	m_pScreenData = 0;
}

void CHPComm38Dlg::OnClose() 
{
	// Don't close the app if a Transfer takes place
	if (m_pProt && m_pProt->m_bTransfering == TRUE) {
		return;
	}

	if (m_nScreenDlg > 0) {
		MessageBox("Please close all screenshot windows before you leave HPGComm", "Error", MB_OK | MB_ICONINFORMATION);
		return;
	}

	if (m_pWaitInfoDlg != 0) {
		// The window is initialized.
		// We delete the "wait info" dialog.
		m_pWaitInfoDlg->DestroyWindow();
		delete m_pWaitInfoDlg;
		m_pWaitInfoDlg = 0;		
	}

	if (m_pScreenData != 0) {
		delete [] m_pScreenData;
	}

    if (m_pProt != 0)
    {
        m_pProt->ClosePort();
        delete m_pProt;
        m_pProt = 0;
    }

	if (m_pMenu != 0)
		delete m_pMenu;

	UpdateData(TRUE);

	CDialog::OnClose();
}

void CHPComm38Dlg::OnOK() 
{
    // We deliberately don't call the base here, as otherwise this
    // causes the app to exit.
	// CDialog::OnOK();
}

void CHPComm38Dlg::OnHelpAbout() 
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CHPComm38Dlg::OnTransferInfoClosed(UINT pParam, long lParam)
{
	if (m_pProt) {
		m_pProt->StopAll();
	}
}

void CHPComm38Dlg::OnHelpFinder() 
{
	// Check if the user already choose a language for his help file.

	// If necessary this call opens a dialog where the user can choose
	// its language. sHelpLanguage contains a valid language then.
	ChooseHelpFileName();

    CDialog::OnHelpFinder();
}

void CHPComm38Dlg::OnShowVD() 
{
	UpdateData();
	// Check if this could be a VD.
    CVD vd(m_sPath);
	if (vd.IsVD() == NO_VD) {
		CString sMsg = m_sPath;
		sMsg += " does not contain valid calculator files";
		MessageBox(sMsg, "Error", MB_OK | MB_ICONSTOP);
		return;
	}

	// Show the content of the VD.
	CVDDlg dlg;
	dlg.m_sPath = m_sPath;
	dlg.DoModal();
}

void CHPComm38Dlg::ChooseHelpFileName()
{
	CWinApp * pApp = AfxGetApp();

	CString sHelpFilePath = pApp->m_pszHelpFilePath;
	int nPos;
	// Extract the path without the file name, but with trailing backslash
	if ((nPos = sHelpFilePath.ReverseFind('\\')) == -1) {
		m_sHelpLanguage.Empty();
		return;
	}
	
	sHelpFilePath = sHelpFilePath.Left(nPos + 1);

	// Let's see if we already have a choosen language.
	if (m_sHelpLanguage.IsEmpty()) {
		// Let the user choose his help file.

		CHelpFileDlg dlg;

		// Break if something goes wrong or if we could not successfully
		// get the language help-file.  Also if the file is not found,
		// process it.  CDialog::OnHelpFinder will display an appropriate
		// help message.
		CFileFind finder;
		if (!finder.FindFile(sHelpFilePath + "*.hlp")) {
            MessageBox("There are no help (*.hlp) files available.", "Help language", MB_OK | MB_ICONSTOP);
			m_sHelpLanguage.Empty();
			return;
		}
		
		// Store the found file-titles in a string array.  We pass the
		// reference to it to the dialog.  This is necessary because we
		// cannot add something to the list-box of the dialog until it's
		// created.
		CStringArray sa;

        BOOL res = TRUE;
		while (res)
        {
            res = finder.FindNextFile();
            CString sHFName = finder.GetFileTitle();
            if (sHFName == "HPComm38")
                sHFName = "English";
			sa.Add(sHFName);
		}

		dlg.pSa = &sa;

		// Let the user choose his language in the dialog.
		if (dlg.DoModal() == IDOK) {
			// Check if the user selected something.
			if (dlg.m_sHelpFile.IsEmpty())
				return;

			m_sHelpLanguage = dlg.m_sHelpFile;
		}
	}

    if (m_sHelpLanguage == "English")
        m_sHelpLanguage = "HPComm38";

	strcpy((char *)pApp->m_pszHelpFilePath, sHelpFilePath + m_sHelpLanguage + ".hlp");
	// OK, done!
}

void CHPComm38Dlg::OnHelpLanguage() 
{
	CWinApp * pApp = AfxGetApp();

	// Store the old strings
	CString sTmpHelpPath = pApp->m_pszHelpFilePath;
	CString sTmpHelpLanguage = m_sHelpLanguage;
	// Empty the language string so that it will be updated.
	m_sHelpLanguage.Empty();
	ChooseHelpFileName();
	// If the setting is the same as before or if it is empty, restore the vars.
	// This also handles the case where the user cancels the dialog.
	if ((sTmpHelpPath == pApp->m_pszHelpFilePath) || m_sHelpLanguage.IsEmpty()) {
		m_sHelpLanguage = sTmpHelpLanguage;
		if (m_sHelpLanguage.IsEmpty()) {
			// If there is no language at all, we choose the "default" help file.
			m_sHelpLanguage = "HPComm38";
		}
		strcpy((char *)pApp->m_pszHelpFilePath, sTmpHelpPath);
	}
}

void CHPComm38Dlg::OnSelchangeComport() 
{
	int nNewComPort = ComPortCtrl.GetCurSel() + 1;
	if (nNewComPort != m_nComPort) {
		m_nComPort = nNewComPort;
		try {
			m_pProt->ClosePort();
			m_pProt->KillComDaemon();
			m_pProt->OpenPort(m_nComPort);
			m_pProt->CreateComDaemon(m_hWnd);
			m_pProt->UnleashDaemon();
			HideInfo();
		}
		catch (CProtException * pEx) {
			ExceptionHandler(pEx);
		}
	}
}

void CHPComm38Dlg::ShowInfo(CString &sInfoMsg)
{
	if (!m_bInfoShown) {
		OnRestore();
		// Make the two controls visible
		m_InfoStatic.ModifyStyle(0, WS_VISIBLE);
		m_InfoCtrl.ModifyStyle(0, WS_VISIBLE);
		RECT dlgrect, staticrect, inforect;
		GetWindowRect(&dlgrect);
		m_InfoStatic.GetWindowRect(&staticrect);
		m_InfoCtrl.GetWindowRect(&inforect);

		// Enlarge the dialog box of the amout the
		// two control need to be displayed.
		dlgrect.bottom += (inforect.bottom - staticrect.top);
		MoveWindow(&dlgrect);

		// Display the message
		m_sInfo = sInfoMsg;
		UpdateData(FALSE);

		m_bInfoShown = true;
		
		// Force the window to be redrawn
		InvalidateRect(NULL);
		UpdateWindow();
	}
}

void CHPComm38Dlg::HideInfo()
{
	if (m_bInfoShown) {
		// Hide the two controls
		m_InfoStatic.ModifyStyle(WS_VISIBLE, 0);
		m_InfoStatic.InvalidateRect(NULL);
		m_InfoCtrl.ModifyStyle(WS_VISIBLE, 0);
		m_InfoCtrl.InvalidateRect(NULL);
		RECT dlgrect, staticrect, inforect;
		GetWindowRect(&dlgrect);
		m_InfoStatic.GetWindowRect(&staticrect);
		m_InfoCtrl.GetWindowRect(&inforect);

		// Make the dialog smaller.
		dlgrect.bottom -= (inforect.bottom - staticrect.top);
		MoveWindow(&dlgrect);

		m_bInfoShown = false;

		// Force the window to be redrawn
		InvalidateRect(NULL);
		UpdateWindow();
	}
}

BOOL CHPComm38Dlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message== WM_LBUTTONDOWN || pMsg->message== WM_LBUTTONUP || pMsg->message== WM_MOUSEMOVE)
		m_ToolTip.RelayEvent(pMsg);	

	return CDialog::PreTranslateMessage(pMsg);
}

void CHPComm38Dlg::OnCreateDir() 
{
	UpdateData(TRUE);

	// The path must have a trailing backslash
	CString sPath = m_sPath;
	if (sPath.Right(1) != "\\") {
		sPath += "\\";
	}
	
	if (PathExists(sPath)) {
		MessageBox("Please enter the name of the directory you want to create in the edit box and try again.", "Create directory", MB_OK | MB_ICONINFORMATION);
		return;
	}
	
	// Ask the user if he wants to really create the directory
	CString sMsg;
	if (m_bDirInexistent) {
		sMsg = "The directory " + m_sPath + " does not exist.\r\n\r\nIf you create it, you can hit \"YES\" on the calculator to initialize the new directory.\r\n\r\nDo you want to create it now?";
	}
	else {
		sMsg = "Do you want to create the directory\r\n\r\n" + m_sPath;
	}

	if (MessageBox(sMsg, "Create directory?", MB_YESNO | MB_ICONQUESTION) == IDNO) {
		return;
	}
	
	if (!MakeSureDirectoryPathExists(sPath)) {
		MessageBox("The directory could not be created!", "ERROR", MB_OK | MB_ICONEXCLAMATION);
	}
	else {
		m_bDirInexistent = FALSE;
	}
}

void CHPComm38Dlg::OnChangeDirectory() 
{
	// Set a timer.
	// When the timer runs out, OnTimer will decide if the 
	// CreateDir button is enabled or disabled (if the directory
	// already exists).
	SetTimer(DIR_TIMER, 750, NULL);
}

void CHPComm38Dlg::OnTimer(UINT nIDEvent) 
{
	// If it our timer?	
	if (nIDEvent == DIR_TIMER) {
		KillTimer(nIDEvent);
		HandleCreateDirButton();
		SetCalcPic(FALSE);
	}

	CDialog::OnTimer(nIDEvent);
}

void CHPComm38Dlg::EnableCreateDir(BOOL bEnable)
{
	if (bEnable) {
		m_CreateDir.ModifyStyle(WS_DISABLED, 0);
	}
	else {
		m_CreateDir.ModifyStyle(0, WS_DISABLED);
	}

	m_CreateDir.Invalidate();
}


void CHPComm38Dlg::HandleCreateDirButton()
{
		UpdateData(TRUE);
		CMenu * pMenu = GetMenu();
		if (!PathExists(m_sPath)) {
			// If the dir doesn't exist, we enable the createDir button...
			EnableCreateDir(TRUE);
			if (pMenu) {
				pMenu->GetSubMenu(0)->EnableMenuItem(IDC_CREATEDIR, MF_BYCOMMAND | MF_ENABLED);	
			}
		}
		else {
			//... else we disable it
			EnableCreateDir(FALSE);
			if (pMenu) {
				GetMenu()->GetSubMenu(0)->EnableMenuItem(IDC_CREATEDIR, MF_BYCOMMAND | MF_GRAYED);	
			}
		}

}

void CHPComm38Dlg::OnRestore() 
{
	ShowTaskIcon(TRUE);	
	CWnd * pChildWnd = GetWindow(GW_CHILD);
	if (pChildWnd) {
		pChildWnd->SetForegroundWindow();
	}
	ShowWindow(SW_RESTORE);
	if (pChildWnd) {
		pChildWnd->SetActiveWindow();
	}
}

void CHPComm38Dlg::ShowTaskIcon(BOOL bShow /* = true */)
{
	ShowWindow(bShow ? SW_SHOW : SW_HIDE);
}

void CHPComm38Dlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	// Hide the window if we have a tray icon.
	if (nType == SIZE_MINIMIZED) {
		//m_TrayIcon.SetIcon(IDR_MAINFRAME);
		//ShowTaskIcon(FALSE);
	}
	
}

void CHPComm38Dlg::OnEditchangeDirectory() 
{
	// TODO: Add your control notification handler code here
	SetTimer(DIR_TIMER, 750, NULL);	
}

void CHPComm38Dlg::UpdateHistory(CString &sPath)
{
	if (!sPath.IsEmpty()) {
		AfxGetApp()->AddToRecentFileList(sPath);
		FillHistoryCombo();
	}
}

void CHPComm38Dlg::FillHistoryCombo()
{
	m_DirEditBox.ResetContent();
	CHPComm38App * pApp = (CHPComm38App *)AfxGetApp();
	if (pApp) {
		CString sName;
		for (int i = 0; i < HISTORY_SIZE; i++) {
			pApp->GetMRUDirName(i, sName);
			if (!sName.IsEmpty()) {
				m_DirEditBox.AddString(sName);
			}
		}
	}

	UpdateData(FALSE);
}


void CHPComm38Dlg::SetCalcPic(BOOL bAfterSelection)
{
	CString sPath;
	if (bAfterSelection) {
		int nSel = m_DirEditBox.GetCurSel();
		if (nSel > -1) {
			m_DirEditBox.GetLBText(nSel, sPath);
		}
		else {
			sPath = m_sPath;
		}
	}
	else {
		sPath = m_sPath;
	}

	CVD vd(sPath);
	vd_type type = vd.IsVD();

	if (type == HP39_VD && m_dwPicShown == IDB_CALCIMAGE) {
		// The 39 pic is already loaded.
		return;
	}
	
	if (type == HP38_VD && m_dwPicShown == IDB_CALCIMAGE38) {
		// The 38 pic is already loaded.
		return;
	}

	if (type == NO_VD && m_dwPicShown == IDB_NOCALC) {
		// The "nocalc" pic is already loaded.
		return;
	}

	CString sToolText = "This directory contains ";
	if (type == HP38_VD) {
		m_dwPicShown = IDB_CALCIMAGE38;
		sToolText += "HP38 files";
	}
	else if (type == NO_VD) {
		m_dwPicShown = IDB_NOCALC;
		sToolText += "no valid calculator files";
	}
	else {
		m_dwPicShown = IDB_CALCIMAGE;
		sToolText += "HP39 files";
	}

	m_ToolTip.UpdateTipText(sToolText, this, IDC_CALCPIC);

	RECT rect;
	m_CalcPic.GetClientRect(&rect);
	HBITMAP hBitmap = (HBITMAP)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(m_dwPicShown), IMAGE_BITMAP, rect.right - rect.left, rect.bottom - rect.top, LR_DEFAULTCOLOR);

	m_CalcPic.SetBitmap(hBitmap);

	CMenu * pMenu = GetMenu();
	if (type == NO_VD) {
		// Disable the SHOW_VD button and the menu entry if a directory with no calculator
		// files is selected
		m_ShowVD.ModifyStyle(0, WS_DISABLED);
		if (pMenu) {
			GetMenu()->GetSubMenu(0)->EnableMenuItem(IDC_SHOWVD, MF_BYCOMMAND | MF_GRAYED);	
		}
	}
	else {
		// Enable them...
		m_ShowVD.ModifyStyle(WS_DISABLED, 0);
		if (pMenu) {
			GetMenu()->GetSubMenu(0)->EnableMenuItem(IDC_SHOWVD, MF_BYCOMMAND | MF_ENABLED);				
		}

	}

	m_ShowVD.Invalidate();
}

void CHPComm38Dlg::OnSelchangeDirectory() 
{
	SetCalcPic(TRUE);
}

void CHPComm38Dlg::OnDropdownDirectory() 
{
	// If the combo box is empty, we add the current directory to the combo box list
	// It looks better.
	if (m_DirEditBox.GetCount() == 0) {
		m_DirEditBox.AddString(m_sPath);
	}	
}

BOOL CHPComm38Dlg::ToolTipCallback(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{

	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
	UINT nID =pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND)
	{
		// idFrom is actually the HWND of the tool
		nID = ::GetDlgCtrlID((HWND)nID);
		if(nID)
		{	
			if (nID == IDC_SHOWVD && (m_ShowVD.GetStyle() & WS_DISABLED)) {
				pTTT->lpszText = MAKEINTRESOURCE(IDC_SHOWVD_DIS);
			}
			else if (nID == IDC_CREATEDIR && (m_CreateDir.GetStyle() & WS_DISABLED)) {
				pTTT->lpszText = MAKEINTRESOURCE(IDC_CREATEDIR_DIS);
			}
			else if (nID == IDC_DIRECTORY) {
				m_DirEditBox.GetWindowText(m_sShownDir);
				pTTT->lpszText = (LPTSTR)(LPCTSTR)m_sShownDir;
			}
			else {
				pTTT->lpszText = MAKEINTRESOURCE(nID);
			}
			pTTT->hinst = AfxGetResourceHandle();
			return(TRUE);
		}
	}
	return(FALSE);

}

BOOL CHPComm38Dlg::PathExists(CString sPath)
{
	// Check if the directory still exists...
	CFileFind finder;
	if (sPath.Right(1) != "\\") {
		sPath += "\\";
	}
	sPath += "*.*";
	BOOL bFound = finder.FindFile(sPath);
	if (bFound) {
		// This must be called before calling IsDirectory()
		finder.FindNextFile();
	}

	finder.Close();

	return bFound;
}

void CHPComm38Dlg::OnGetXModem()
{
	try {
		m_pProt->ClosePort();
		CXModem XModem;
		XModem.OpenPort(m_nComPort);
		XModem.m_sCurPCPath = m_sPath;
		XModem.Transfer(false);
		XModem.ClosePort();

		m_pProt->OpenPort(m_nComPort);
		m_pProt->Reset();
		m_pProt->KillComDaemon();
		m_pProt->CreateComDaemon(m_hWnd);
		m_pProt->UnleashDaemon();
	}
	catch (CProtException * pEx) {
		ExceptionHandler(pEx);
	}
}

LRESULT CHPComm38Dlg::OnKickIdle(WPARAM, LPARAM lCount)
{
       // Do idle processing here, just like CWinApp::OnIdle
       //
       TRACE("CMyDlg::OnKickIdle, do priority %d stuff\n", lCount);
       return (lCount <= 2);
}
