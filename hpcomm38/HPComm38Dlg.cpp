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
#include "VD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHPComm38Dlg *CHPComm38Dlg::pseudoThis = 0;

/////////////////////////////////////////////////////////////////////////////
// CHPComm38Dlg dialog

CHPComm38Dlg::CHPComm38Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHPComm38Dlg::IDD, pParent), rb("Settings")
{
	//{{AFX_DATA_INIT(CHPComm38Dlg)
	m_sPath = _T("");
	m_sInfo = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	rb.Bind(nComPort, "Com-port", 1);
	rb.Bind(m_sPath, "Last directory", "C:\\");
	rb.Bind(sHelpLanguage, "Help-language", "");

	VERIFY(pseudoThis==0);
	pseudoThis = this;

	pWaitInfoDlg = 0;
	
	m_bVDInitInfoShown = false;

	nScreenDlg = 0;
}

void CHPComm38Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHPComm38Dlg)
	DDX_Control(pDX, IDC_INFOSTATIC, m_InfoStatic);
	DDX_Control(pDX, IDC_INFO, m_InfoCtrl);
	DDX_Control(pDX, IDC_CHANGEDIR, m_DirectoryControl);
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
    ON_COMMAND(ID_HELP, OnHelpFinder)
	ON_COMMAND(ID_DEFAULT_HELP, OnHelpFinder)
	ON_CBN_SELCHANGE(IDC_COMPORT, OnSelchangeComport)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_COMDAEMON, OnComdaemonMsg)
	ON_MESSAGE(WM_TRANSFERINFOCLOSED, OnTransferInfoClosed)
	ON_MESSAGE(WM_DIRECTORYCHANGE, OnChangeDir)
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
    pMenu = new CMenu;
	if (pMenu && pMenu->LoadMenu(IDR_MAINFRAME)) {
		// and set it.
		SetMenu(pMenu);
	}

	HideInfo();

	// Make sure the directory entry field contains the correct dir.
	UpdateData(FALSE);

	pScreenData = 0;

	pKermit = new CKermit();

	if (pKermit == NULL)
		return false;

	try {
		pKermit->OpenPort(nComPort);
	}
	catch (CProtException * pEx) {
		// Ensure the dialog is friendlier when we first start.
		if (pEx->error == CProtException::OPENPORT) {
			CString sMsg;
			sMsg.Format("Welcome to HPComm38.  I couldn't use your computer's COM%d serial port.\r\nAfter closing this message, please choose a different serial port\r\nfrom the provided list.", nComPort);
			MessageBox(sMsg, "Welcome", MB_OK | MB_ICONEXCLAMATION);
			pEx->Delete();
		}
		else
			ExceptionHandler(pEx);
	}

	pKermit->CreateComDaemon(m_hWnd);
	pKermit->UnleashDaemon();

	ComPortCtrl.SetCurSel(nComPort - 1);

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
	if (lParam == COMACTIVITY) {
		
		// Hide the info window.
		HideInfo();

		// As long as the transaction is taking place we show the hourglass
		// so that the user cannot fuss around.
		BeginWaitCursor();
		UpdateData();

		try {
            // First we have to check if the data at the Com-port is a screenshot
            // of a kermit-packet.
            // If the first char is 27 it's a screenshot for 48 & 38
            // If the second char is 'W' or 'G', it is a screenshot for 49/39/40
            // Otherwise it's a kermit-packet.
            char cFirstChar;
            char cSecondChar;
            if (!pKermit->ReadCharFromCom(&cFirstChar)) {
                throw new CProtException(CProtException::TIMEOUT, 0, "");
            }
            
            if (cFirstChar == 27) {
                // We have a screen shot waiting to be shown...
                ReadScreenData(cFirstChar);
            }
            else {
                if (!pKermit->ReadCharFromCom(&cSecondChar)) {
                    throw new CProtException(CProtException::TIMEOUT, 0, "");
                }

                if ((cSecondChar == 'W') || (cSecondChar == 'G')) {
                    ReadScreenData(cSecondChar);
                }
                else
                    pKermit->ClientRequest(cFirstChar, cSecondChar, m_sPath);
            }
        }

		catch (CProtException * pEx) {
			ExceptionHandler(pEx);
			EndWaitCursor();
			pKermit->Reset();
			pKermit->UnleashDaemon();
			return;
		}

		RestoreWaitCursor();

		// If the transaction was finished (this means the 38G wrote
		// a directory file to disk and a new file was created on the
		// PC, we reread the content of the PC dir, too.
		if (pKermit->bTransactionFinished) {
			// Maybe the calc initialized the dir to a VD
			// We check that to replace the CFilerDummy with a CFiler38
			
			//Change38Filer(sLastDir);
			EndWaitCursor();

			pKermit->bNewFileCreated = false;
			pKermit->bTransactionFinished = false;
		}

		// FIXME: If the 38G only requested files we don't know when all
		// files are transmitted. So we enable the main window every time.
		// This is not a really good solution, but there's no other option.
		// FIXME: mjd: Could we do it with timers?
		if (pKermit->bSendingOnly) {
			EndWaitCursor();
		}

		// The screendata contains something. This means for us to display
		// The screenshot.
		if (pScreenData != 0) {
			ShowScreenShot();
		}

		if (!pKermit->bTransactionFinished)
			BeginWaitCursor();

		// TRACE("Daemon resumed by CMainFrame\r\n");
		pKermit->UnleashDaemon();
	}


}

void CHPComm38Dlg::ExceptionHandler(CProtException *pEx)
{
	CString sError;
	CVD vd(m_sPath);

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
		sError.Format("The COM%d serial port could not be opened.\r\n\r\nPlease choose another serial port.", nComPort);
		break;
	case CProtException::DEVICE:
		sError = "The serial port could not be initialized correctly.";
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
		sError = "There was no answer from the calculator.\r\n\r\nPlease try again after checking the connection.";
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

	pEx->Delete();
	if (pKermit) {
		pKermit->Reset();
		pKermit->FlushComPort();
	}
	EnableWindow(TRUE);
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
		if (pWaitInfoDlg == 0) {
			pWaitInfoDlg = new CDialog;
			pWaitInfoDlg->Create(IDD_WAITINFO);
			pWaitInfoDlg->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE |SWP_NOSIZE);
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
		
		EnableWindow(TRUE);
	}

	UpdateData();
}

int CALLBACK CHPComm38Dlg::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	VERIFY(pseudoThis!=0);
	switch(uMsg) {
	case BFFM_INITIALIZED: {
		if (!pseudoThis->m_sPath.IsEmpty()) {
			// We want that the box shows our directory selected in the beginning.


			// WParam is TRUE since you are passing a path.
			// It would be FALSE if you were passing a pidl.
			::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)pseudoThis->m_sPath);
		}

		break;
	}
	case BFFM_SELCHANGED: {
		if (pseudoThis->pWaitInfoDlg != 0) {
			// The window is initialized.
			// We delete the "wait info" dialog.
			pseudoThis->pWaitInfoDlg->DestroyWindow();
			delete pseudoThis->pWaitInfoDlg;
			pseudoThis->pWaitInfoDlg = 0;		
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
            if (vd.IsVD()) {
				sMessage += "contains calculator files.";
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

    pScreenData = new unsigned char [nBytesToRead+1];
    *pScreenData = ScreenType;
	DWORD dwRead;
	if ((dwRead = pKermit->ReadBufferFromCom((char *)(pScreenData + 1), nBytesToRead)) != nBytesToRead) {
		MessageBox("There was a problem collecting the screen image", "Screen image error", MB_OK|MB_ICONEXCLAMATION);
		delete [] pScreenData;
		pScreenData = 0;
	}
}
void CHPComm38Dlg::ShowScreenShot()
{
	CGrobDlg grobDlg(this);
	grobDlg.sFileName.Empty();
	grobDlg.sCurDir = m_sPath;
	grobDlg.screendata = pScreenData;
	grobDlg.nScreenDlg = nScreenDlg;
	pScreenData = 0;
	pKermit->UnleashDaemon();
	nScreenDlg++;
	grobDlg.DoModal();
	nScreenDlg--;

	// We delete the pointer in the dialog, cause when we make more than one screenshot
	// the pScreenData only contains the last one.
	// This way every char * is deleted, not only the last one.
	delete [] grobDlg.screendata;
	grobDlg.screendata = 0;
	pScreenData = 0;
	/*if (pKermit) {
		pKermit->SetNotificationWindow(m_hWnd);
	}*/
}

void CHPComm38Dlg::OnClose() 
{
	if (nScreenDlg > 0) {
		MessageBox("Please close all screenshot windows before you leave HPComm38", "Error", MB_OK | MB_ICONINFORMATION);
		return;
	}

    if (pKermit != 0)
    {
        pKermit->ClosePort();
        delete pKermit;
        pKermit = 0;
    }

	if (pMenu != 0)
		delete pMenu;

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
	pKermit->pInfoDlg = 0;
	pKermit->bTransactionFinished = false;
	pKermit->bVDFilesSent = false;
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
	if (vd.IsVD() == false) {
		CString sMsg = m_sPath;
		sMsg += " does not contain valid calculator files";
		MessageBox(sMsg, "Error", MB_OK | MB_ICONSTOP);
		return;
	}

	// Show the content of the VD.
	CVDDlg dlg;
	dlg.sPath = m_sPath;
	dlg.DoModal();
}

void CHPComm38Dlg::ChooseHelpFileName()
{
	CWinApp * pApp = AfxGetApp();

	CString sHelpFilePath = pApp->m_pszHelpFilePath;
	int nPos;
	// Extract the path without the file name, but with trailing backslash
	if ((nPos = sHelpFilePath.ReverseFind('\\')) == -1) {
		sHelpLanguage.Empty();
		return;
	}
	
	sHelpFilePath = sHelpFilePath.Left(nPos + 1);

	// Let's see if we already have a choosen language.
	if (sHelpLanguage.IsEmpty()) {
		// Let the user choose his help file.

		CHelpFileDlg dlg;

		// Break if something goes wrong or if we could not successfully
		// get the language help-file.  Also if the file is not found,
		// process it.  CDialog::OnHelpFinder will display an appropriate
		// help message.
		CFileFind finder;
		if (!finder.FindFile(sHelpFilePath + "*.hlp")) {
            MessageBox("There are no help (*.hlp) files available.", "Help language", MB_OK | MB_ICONSTOP);
			sHelpLanguage.Empty();
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
            if (sHFName.CompareNoCase("HPComm38") == 0)
                sHFName = "English";
			sa.Add(sHFName);
		}

		dlg.pSa = &sa;

		// Let the user choose his language in the dialog.
		if (dlg.DoModal() == IDOK) {
			// Check if the user selected something.
			if (dlg.m_sHelpFile.IsEmpty())
				return;

			sHelpLanguage = dlg.m_sHelpFile;
		}
	}

    if (sHelpLanguage.CompareNoCase("English") == 0)
        sHelpLanguage = "HPComm38";

	strcpy((char *)pApp->m_pszHelpFilePath, sHelpFilePath + sHelpLanguage + ".hlp");
	// OK, done!
}

void CHPComm38Dlg::OnHelpLanguage() 
{
	CWinApp * pApp = AfxGetApp();

	// Store the old strings
	CString sTmpHelpPath = pApp->m_pszHelpFilePath;
	CString sTmpHelpLanguage = sHelpLanguage;
	// Empty the language string so that it will be updated.
	sHelpLanguage.Empty();
	ChooseHelpFileName();
	// If the setting is the same as before or if it is empty, restore the vars.
	// This also handles the case where the user cancels the dialog.
	if ((sTmpHelpPath == pApp->m_pszHelpFilePath) || sHelpLanguage.IsEmpty()) {
		sHelpLanguage = sTmpHelpLanguage;
		if (sHelpLanguage.IsEmpty()) {
			// If there is no language at all, we choose the "default" help file.
			sHelpLanguage = "HPComm38";
		}
		strcpy((char *)pApp->m_pszHelpFilePath, sTmpHelpPath);
	}
}

void CHPComm38Dlg::OnSelchangeComport() 
{
	int nNewComPort = ComPortCtrl.GetCurSel() + 1;
	if (nNewComPort != nComPort) {
		nComPort = nNewComPort;
		try {
			pKermit->ClosePort();
			pKermit->KillComDaemon();
			pKermit->OpenPort(nComPort);
			pKermit->CreateComDaemon(m_hWnd);
			pKermit->UnleashDaemon();
		}
		catch (CProtException * pEx) {
			ExceptionHandler(pEx);
		}
	}
}

void CHPComm38Dlg::ShowInfo(CString &sInfoMsg)
{
	if (!m_bInfoShown) {
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
