// MainFrame.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "HPComm.h"

#include "MainFrame.h"
#include "TreeBase.h"
#include "ListBase.h"
#include "CommDlg.h"
#include "Kermit.h"
#include "XModem.h"
#include "Splash.h"
#include "GrobDlg.h"
#include "CaptureDlg.h"
#include "FilerPC.h"
#include "Filer4x.h"
#include "ComInfoDlg.h"
#include "ArchiveNameDlg.h"
#include "Log.h"
#include "ProtException.h"
#include "HelpFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Prototype of ComDaemon
UINT ComDaemon(LPVOID pParam);


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_COMMSET, OnCommset)
	ON_WM_SIZE()
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_COMMAND(ID_ASCII, OnAscii)
	ON_COMMAND(ID_BINARY, OnBinary)
	ON_UPDATE_COMMAND_UI(ID_ASCII, OnUpdateAscii)
	ON_UPDATE_COMMAND_UI(ID_BINARY, OnUpdateBinary)
	ON_COMMAND(ID_CAPTURE, OnCapture)
	ON_COMMAND(ID_FILE_RENAME, OnFileRename)
	ON_COMMAND(ID_FILE_NEWFOLDER, OnFileNewfolder)
	ON_COMMAND(ID_FILE_PROPERTIES, OnFileProperties)
	ON_COMMAND(ID_CALCULATOR_BACKUP, OnCalculatorBackup)
	ON_COMMAND(ID_CALCULATOR_RESTORE, OnCalculatorRestore)
	ON_COMMAND(ID_CALCULATOR_KERMIT, OnCalculatorKermit)
	ON_COMMAND(ID_CALCULATOR_XMODEM, OnCalculatorXmodem)
#if 0
	ON_COMMAND(ID_CALCULATOR_UPGRADEHP49G, OnCalculatorUpgradehp49g)
	ON_COMMAND(ID_VIEW_SETUP, OnViewSetup)
#endif
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_CALCULATOR_XMODEM, OnUpdateCalculatorXmodem)
	ON_UPDATE_COMMAND_UI(ID_CALCULATOR_KERMIT, OnUpdateCalculatorKermit)
	ON_COMMAND(ID_BOOKMARK, OnSetbookmark)
	ON_UPDATE_COMMAND_UI(ID_CALCULATOR_BACKUP, OnUpdateCalculatorBackup)
	ON_UPDATE_COMMAND_UI(ID_CALCULATOR_RESTORE, OnUpdateCalculatorRestore)
	ON_COMMAND(ID_HELP_LICENSE, OnHelpLicense)
	ON_WM_SETFOCUS()
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_CAPTURE, OnCapture)
	ON_COMMAND(ID_HELPLANGUAGE, OnHelplanguage)
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, OnHelpFinder)
#if 0
    ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
#else
    ON_COMMAND(ID_HELP, OnHelpFinder)
#endif
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, OnHelpFinder)
	ON_UPDATE_COMMAND_UI_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnUpdateViewStyles)
	ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnViewStyle)
	ON_COMMAND_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE4, OnMRUDir)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
};

CMainFrame * CMainFrame::pseudoThis = NULL;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame() : rb("Settings")
{
	rb.Bind(settings.dwBaud, "Baudrate", 9600);
	rb.Bind(settings.nComPort, "Com-port", 0);
	rb.Bind(settings.nTransIO, "Translation mode", 1);
	rb.Bind(settings.nCalcType, "Calculator type", HP48);
	rb.Bind(settings.nChk, "Checksum type", 1);
	rb.Bind(sLastDir, "Directory", "C:\\");
	rb.Bind(bFinishOnClose, "FinishOnClose", 0);
	rb.Bind(sHelpLanguage, "Help-language", "");

    pKermit = 0;
	pHPImageList = 0;
	pBigHPImageList = 0;
	pSystemImageList = 0;
	pBigSystemImageList = 0;
	pTopFiler = 0;
	pBottomFiler = 0;
	bStartup = true;
}

CMainFrame::~CMainFrame()
{
	if (pHPImageList != 0) {
		pHPImageList->Detach();
		delete pHPImageList;
	}
	if (pBigHPImageList != 0) {
		pBigHPImageList->Detach();
		delete pBigHPImageList;
	}
	if (pSystemImageList != 0) {
		pSystemImageList->Detach();
		delete pSystemImageList;
	}
	if (pBigSystemImageList != 0) {
		pBigSystemImageList->Detach();
		delete pBigSystemImageList;
	}

    if (pKermit != 0)
        delete pKermit;
	
	// Delete the filers if they were created
	if (pTopFiler != 0)
		delete pTopFiler;

	if (pBottomFiler != 0)
		delete pBottomFiler;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	pseudoThis = this;

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	/*if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}*/

    if (!m_wndReBar.Create(this))
        CLog::Program("CMainFrame::OnCreate: could not create rebar.");
    else {
		if (!m_wndReBar.AddBar(&m_wndToolBar))
            CLog::Program("CMainFrame::OnCreate: could not add rebar.");
		// Leftover: !m_wndReBar.AddBar(&m_wndDlgBar)*/)
    }
        
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_GRIPPER | CBRS_BORDER_3D | CBRS_TOOLTIPS | CBRS_FLYBY |
		CBRS_SIZE_DYNAMIC);
	
	rb.Bind(this);

	CString sName;
	CFileFind finder;
	// Check all MRU dirs if they still exist.
	// If not, delete it.
	for (int i = 0; i < 4; i++) {
		if (GetMRUName(i, sName) && !finder.FindFile(sName)) {
			((CHPCommApp *)AfxGetApp())->RemoveMRU(i);
		}
	}

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// create splitter window
	if (!m_wndSplitter.CreateStatic(this, 2, 2))
		return FALSE;

	if (!AfxOleInit())
		return FALSE;


	SIZE size;
	RECT rect;
	GetClientRect(&rect);
	
	size = CSize((rect.right - 10*GetSystemMetrics(SM_CXEDGE)) / 2, (rect.bottom - 20) / 2);

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CTreeBase), size, pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CListBase), size, pContext) ||
		!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CTreeBase), size, pContext) ||
		!m_wndSplitter.CreateView(1, 1, RUNTIME_CLASS(CListBase), size, pContext))

	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}


#ifdef SPLASH
	// CG: The following line was added by the Splash Screen component.
	CSplashWnd::ShowSplashScreen(this);
	// Show the splash screen for 1 second, then hide it.
//	Sleep(1500);
//	CSplashWnd::Kill();
#endif

	// Did we read our variables from the COM port?
	if (settings.nComPort == 0)
	{
		CCommDlg dlg(&settings);
		dlg.bFinish = bFinishOnClose;
		if (dlg.DoModal() == IDCANCEL) {
			// FIXME: If there's no serial port, and the user presses cancel,
			// we want to leave the whole app.  But doing it this way pops up
			// a window saying "Unable to create empty app" or similar.  Q: How
			// to leave the app without this?
			bFinishOnClose = dlg.bFinish;	
			m_wndSplitter.DestroyWindow();
			return FALSE;
		}
		bFinishOnClose = dlg.bFinish;	
	}

	pKermit = new CKermit(&settings);
	pXModem = 0;

    CComInfoDlg::SetPort(pKermit->pSettings->nComPort);

	bToTop = true;
	CreateImageLists();
	SetFilerPointers();
	SetSettingsPointers();

	// The next two lines are essential for displaying the status of
	// reconnect attempts.
	// We show a dialog box which informs the user that something is going on...
	pKermit->pfConnectNewCb = CComInfoDlg::NewDlg;
	pKermit->pfConnectBumpCb = CComInfoDlg::BumpDlg;
	pKermit->pfConnectDeleteCb = CComInfoDlg::DeleteDlg;

	bool loop = true;
	bool wasOpenPort = false;
	while (loop == true) {
		loop = false;
		try {
			bool res = pKermit->Connect(pKermit->pSettings->nComPort);
		}
		catch (CProtException * pEx) {
			wasOpenPort = (pEx->error == CProt::OPENPORT);
			ExceptionHandler(pEx);
			if (wasOpenPort == true)
			{
				CCommDlg dlg(pKermit->pSettings);
				dlg.bFinish = bFinishOnClose;
				if (dlg.DoModal() == IDOK)
				{
					pKermit->pfConnectDeleteCb();
					CComInfoDlg::SetPort(pKermit->pSettings->nComPort);
					loop = true;

					continue;
				}
				bFinishOnClose = dlg.bFinish;	
			}
	
			// We have to bump the dlg here to show that the connection failed
			BumpComInfoDlg(NULL, CONNECTTOCALC, false);
			BumpComInfoDlg(NULL, GETTINGCALCINFO, false);
		}
	}

	// We try to open the last accessed directory.
	CFileFind finder;
	// Check if it still exists
	if (finder.FindFile(sLastDir)) {
		// Open it.
		pTopFiler->currentDir(pTopFiler->fromString(sLastDir));
	}

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

CListBase* CMainFrame::GetUpperListView()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	CListBase* pView = DYNAMIC_DOWNCAST(CListBase, pWnd);
	return pView;
}

CListBase * CMainFrame::GetLowerListView()
{
	CWnd* pWnd = m_wndSplitter.GetPane(1, 1);
	CListBase* pView = DYNAMIC_DOWNCAST(CListBase, pWnd);
	return pView;
}

void CMainFrame::OnUpdateViewStyles(CCmdUI* pCmdUI)
{
	// TODO: customize or extend this code to handle choices on the
	// View menu.

	CListBase* pUpperListView = GetUpperListView();
	CListBase* pLowerListView = GetLowerListView();

	// if the right-hand pane hasn't been created or isn't a view,
	// disable commands in our range

	if (pUpperListView == NULL)
		pCmdUI->Enable(FALSE);
	else
	{
		DWORD dwStyle = pUpperListView->GetStyle() & LVS_TYPEMASK;

		// if the command is ID_VIEW_LINEUP, only enable command
		// when we're in LVS_ICON or LVS_SMALLICON mode

		if (pCmdUI->m_nID == ID_VIEW_LINEUP)
		{
			if (dwStyle == LVS_ICON || dwStyle == LVS_SMALLICON)
				pCmdUI->Enable();
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
			// otherwise, use dots to reflect the style of the view
			pCmdUI->Enable();
			BOOL bChecked = FALSE;

			switch (pCmdUI->m_nID)
			{
			case ID_VIEW_DETAILS:
				bChecked = (dwStyle == LVS_REPORT);
				break;

			case ID_VIEW_SMALLICON:
				bChecked = (dwStyle == LVS_SMALLICON);
				break;

			case ID_VIEW_LARGEICON:
				bChecked = (dwStyle == LVS_ICON);
				break;

			case ID_VIEW_LIST:
				bChecked = (dwStyle == LVS_LIST);
				break;

			default:
				bChecked = FALSE;
				break;
			}

			pCmdUI->SetRadio(bChecked ? 1 : 0);
		}
	}
}


void CMainFrame::OnViewStyle(UINT nCommandID)
{
	// TODO: customize or extend this code to handle choices on the
	// View menu.
	CListBase* pUpperListView = GetUpperListView();
	CListBase* pLowerListView = GetLowerListView();

	// if the right-hand pane has been created and is a CPCListView,
	// process the menu commands...
	if ((pUpperListView != NULL) && (pLowerListView != NULL))
	{
		DWORD dwStyle = -1;

		switch (nCommandID)
		{
		case ID_VIEW_LINEUP:
			{
				// ask the list control to snap to grid
				CListCtrl& PCrefListCtrl = pUpperListView->GetListCtrl();
				PCrefListCtrl.Arrange(LVA_SNAPTOGRID);
				CListCtrl& HPrefListCtrl = pLowerListView->GetListCtrl();
				HPrefListCtrl.Arrange(LVA_SNAPTOGRID);
			}
			break;

		// other commands change the style on the list control
		case ID_VIEW_DETAILS:
			dwStyle = LVS_REPORT;
			break;

		case ID_VIEW_SMALLICON:
			dwStyle = LVS_SMALLICON;
			break;

		case ID_VIEW_LARGEICON:
			dwStyle = LVS_ICON;
			break;

		case ID_VIEW_LIST:
			dwStyle = LVS_LIST;
			break;
		}

		// change the style; window will repaint automatically
		if (dwStyle != -1) {
			pUpperListView->ModifyStyle(LVS_TYPEMASK, dwStyle);
			pLowerListView->ModifyStyle(LVS_TYPEMASK, dwStyle);
		}
	}
}

void CMainFrame::OnCommset() 
{
	// TODO: Add your command handler code here
	CCommDlg dlg(&settings);
	dlg.bFinish = bFinishOnClose;
	int oldchk = settings.nChk;
	int oldtrans = settings.nTransIO;
	if (dlg.DoModal() == IDOK) {
		bFinishOnClose = dlg.bFinish;	
		pKermit->CloseComPort();
		CComInfoDlg::SetPort(pKermit->pSettings->nComPort);
		if (!pKermit->OpenPort()) {
			pBottomFiler->invalidate(pBottomFiler->root());
			pKermit->CloseComPort();
			UpdateAllViews(pBottomFiler, true);
		}
		else if (oldchk != settings.nChk || oldtrans != settings.nTransIO) {
			// The values change and we have to update the calc.
			try {
				pKermit->SetHP(settings.nTransIO, settings.nChk);
			}
			catch (CProtException * pEx) {
				ExceptionHandler(pEx);
			}
		}
	}
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (!m_wndSplitter.CanActivateNext()) {
		return;
	}
	m_wndSplitter.SetColumnInfo(0, cx / 4, 50);
	int y = cy / 2 - 30;
	if (y < 0) y = 0;
	m_wndSplitter.SetRowInfo(0, y, 50);
	m_wndSplitter.RecalcLayout();

}

void CMainFrame::OnRefresh() 
{
	CWaitCursor wait;
	pTopFiler->reRead(pTopFiler->currentDir());
	pBottomFiler->reRead(pBottomFiler->currentDir());
	UpdateAllViews(NULL, true);
}

void CMainFrame::OnAscii() 
{
	CWaitCursor wait;

    pKermit->SetBinary(false);
}

void CMainFrame::OnBinary() 
{
	CWaitCursor wait;
	
    pKermit->SetBinary(true);
}

void CMainFrame::OnUpdateAscii(CCmdUI* pCmdUI) 
{
	// Disable the menu if the calc is not connected.
	if (!pBottomFiler->isAvailable()) {
		pCmdUI->Enable(FALSE);
		return;
	}
	else {
		pCmdUI->Enable(TRUE);
	}
	// Disable the menu if we set the protocol to XModem
	if (pKermit == 0) {
		pCmdUI->Enable(FALSE);
	}
	else {
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(pKermit->IsBinary() == false);
	}
}

void CMainFrame::OnUpdateBinary(CCmdUI* pCmdUI) 
{
	// Disable the menu if the calc is not connected.
	if (!pBottomFiler->isAvailable()) {
		pCmdUI->Enable(FALSE);
		return;
	}
	else {
		pCmdUI->Enable(TRUE);
	}
	// Disable the menu if we set the protocol to XModem
	if (pKermit == 0) {
		pCmdUI->Enable(FALSE);
	}
	else {
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(pKermit->IsBinary() == true);
	}
}

CHPCommDoc * CMainFrame::GetDocument()
{
	return (CHPCommDoc *)GetActiveDocument();
}

void CMainFrame::
CreateImageLists()
{
	// First we create the necessary image lists...



	// ... and the image List for the HP.

	pHPImageList = new CImageList;
	pBigHPImageList = new CImageList;

	// todo calc-type recoginition......
	CBitmap bitmap, bigbitmap;
	// Load the items to the bitmap
	bitmap.LoadBitmap(IDB_HPITEMS);
	// Create an appropriate image-list.
	pHPImageList->Create(16, 16, ILC_COLOR24 | ILC_MASK, 50, 50);
	// Set the "windows" background color.
	pHPImageList->SetBkColor(GetSysColor(COLOR_WINDOW));
	// Add the "icons" to the image list.
	pHPImageList->Add(&bitmap, pHPImageList->GetBkColor());

	bigbitmap.LoadBitmap(IDB_HPITEMSBIG);
	pBigHPImageList->Create(32, 32, ILC_COLOR24 | ILC_MASK, 50, 50);
	pBigHPImageList->SetBkColor(GetSysColor(COLOR_WINDOW));
	pBigHPImageList->Add(&bigbitmap, pHPImageList->GetBkColor());
}

void CMainFrame::SetSettingsPointers()
{
	for (int i = 0; i < 2; i++) {
		((CTreeBase *)m_wndSplitter.GetPane(i, 0))->pSettings = &settings;
		((CListBase *)m_wndSplitter.GetPane(i, 1))->pSettings = &settings;
	}
}

void CMainFrame::
SetFilerPointers()
{
	pTopFiler = new CFilerPC;
	pBottomFiler = new CFiler4x(pKermit);

	// Walk through all 4 panes
	for (int i = 0; i < 2; i++) {
		CFiler *filer;

		CTreeBase *tree = (CTreeBase *)m_wndSplitter.GetPane(i, 0);
		CListBase *list = (CListBase *)m_wndSplitter.GetPane(i, 1);
		if (i == 0) {
			// Get the handle to the system image lists
			HIMAGELIST hSmallSystemImageList, hSystemImageList;
			SHFILEINFO shinfo;

			// Get the handle of the (normal) system imagelist.
			if (pSystemImageList == 0) {
				hSmallSystemImageList = (HIMAGELIST)SHGetFileInfo((LPCTSTR)_T("C:\\"), 0, &shinfo, sizeof(shinfo), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
				pSystemImageList = new CImageList;
				pSystemImageList->Attach(hSmallSystemImageList);
				::SendMessage(tree->m_hWnd, TVM_SETIMAGELIST, (WPARAM)TVSIL_NORMAL, (LPARAM)hSmallSystemImageList);
				::SendMessage(list->m_hWnd, LVM_SETIMAGELIST, (WPARAM)LVSIL_SMALL, (LPARAM)hSmallSystemImageList);
			}

			if (pBigSystemImageList == 0) {
				hSystemImageList = (HIMAGELIST)SHGetFileInfo((LPCTSTR)_T("C:\\"), 0, &shinfo, sizeof(shinfo), SHGFI_SYSICONINDEX);
				pBigSystemImageList = new CImageList;
				pBigSystemImageList->Attach(hSystemImageList);
				::SendMessage(list->m_hWnd, LVM_SETIMAGELIST, (WPARAM)LVSIL_NORMAL, (LPARAM)hSystemImageList);
			}
			filer = pTopFiler;
		}
		else {
			filer = pBottomFiler;
			tree->GetTreeCtrl().SetImageList(pHPImageList, TVSIL_NORMAL);
			list->GetListCtrl().SetImageList(pHPImageList, LVSIL_SMALL);
			list->GetListCtrl().SetImageList(pBigHPImageList, LVSIL_NORMAL);
		}
		tree->pFiler = filer;
		list->pFiler = filer;
	}
}

void CMainFrame::OnCapture() 
{
	CCaptureDlg captureDlg(this);
	captureDlg.pSettings = &settings;
	captureDlg.pProt = pKermit;
	int ans = captureDlg.DoModal();
	if (ans == CAPTURE_CANCELLED) {

	}
	else if (ans == CAPTURE_SUCCESS) {
		// The data pointer should now point to the screendata
	    unsigned char * data = captureDlg.GetScreenData();

		CGrobDlg grobDlg(this);
		grobDlg.nScreenDlg = 0;
		grobDlg.sFileName.Empty();
		grobDlg.sCurDir = pTopFiler->toString(pTopFiler->currentDir());
        grobDlg.screendata = data;
		grobDlg.DoModal();
		grobDlg.sFileName = grobDlg.sFileName.Left(grobDlg.sFileName.ReverseFind('\\'));
		CFilerEntry * fe = pTopFiler->fromString(grobDlg.sFileName);
		if (pTopFiler->isValid(fe)) {
			pTopFiler->reRead(fe);
			UpdateAllViews(pTopFiler, true);
		}


        delete data;
	}

	pKermit->OpenPort();
}


void CMainFrame::OnFileRename() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnFileNewfolder() 
{
    CWaitCursor wait;
	CView * pView = GetActiveView();
	if (pView->IsKindOf(RUNTIME_CLASS(CTreeBase))) {
		m_wndSplitter.ActivateNext();
		OnFileNewfolder();
	}
	else {
		((CListBase *)pView)->newFolder();	
	}
}

void CMainFrame::OnFileProperties() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnCalculatorBackup() 
{
	CArchiveNameDlg dlg;
	CTime time = CTime::GetCurrentTime();
	dlg.m_ArchiveName = time.Format("%d-%b-%Y.hpbackup");

	bool loop = true;
	while (loop == true) {
		loop = false;
		// Run the dialog.  Did they cancel?
		if (dlg.DoModal() == IDCANCEL)
			return;

		// Does it end with .hpbackup?
		if (IsArchiveFilename(dlg.m_ArchiveName) == false) {
			AfxMessageBox("Calculator archive files must have a \".hpbackup\" extension.", MB_ICONEXCLAMATION);
			
			loop = true;
		}
	}

	CString name = dlg.m_ArchiveName;
	bool bKermit = (pKermit != 0);

	// Make sure we use kermit as the protocol
	if (!bKermit)
		OnCalculatorKermit();

	try {
        CWaitCursor wait;
		CFilerEntry *archive = pBottomFiler->createArchive(pTopFiler, pTopFiler->currentDir(), name);
	}
	catch (CProtException * pEx) {
		ExceptionHandler(pEx);
	}

	// If kermit was not the previous protocol, we change it back to XModem.
	if (!bKermit) {
		OnCalculatorXmodem();
	}

	UpdateAllViews(pTopFiler, false);
}

void CMainFrame::OnCalculatorKermit() 
{
	if (pKermit != 0)
		return;

	if (pXModem != 0) {
		delete pXModem;
		pXModem = 0;
	}
	pKermit = new CKermit(&settings);
   
	((CFiler4x *)pBottomFiler)->setProtocol(pKermit);
}

void CMainFrame::OnCalculatorXmodem() 
{
	if (pXModem != 0)
		return;

	if (pKermit != 0) {
		delete pKermit;
		pKermit = 0;
	}
	pXModem = new XModem(&settings);
	((CFiler4x *)pBottomFiler)->setProtocol(pXModem);
}

void CMainFrame::OnCalculatorUpgradehp49g() 
{
	// TODO: Add your command handler code here
	
}


void CMainFrame::OnViewSetup() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::UpdateAllViews(CFiler *pFromFiler, bool bTree)
{
	CWaitCursor wait;

	// At this stage the CComInfoDlg must not exist any more.
	// Destroy the window if necessary.
	if (pKermit->pfConnectDeleteCb) {
		bToTop = false;
		pKermit->pfConnectDeleteCb();
	}

	// pFromFiler indicates the file that called this method.
	// All views that have the same filer, have to be updated.

	// If a directory operation was involved, bTree must be set to true
	// so that the treeview also is updated.

	// Walk through all 4 panes
	for (int i = 0; i < 2; i++) {
		CTreeBase *tree = (CTreeBase *)m_wndSplitter.GetPane(i, 0);
		CListBase *list = (CListBase *)m_wndSplitter.GetPane(i, 1);

		if (((tree->pFiler == pFromFiler) || (pFromFiler == 0)) && bTree)
			tree->UpdateContent();

		if ((list->pFiler  == pFromFiler) || (pFromFiler == 0))
			list->UpdateContent();
	}

	sLastDir = pTopFiler->toString(pTopFiler->currentDir());
}

void CMainFrame::OpenMRUDir(int n /* = 0 */)
{
	CString sName;
	if (GetMRUName(n, sName)) {
		CFilerEntry *fe = pTopFiler->fromString(sName);
		if (fe == 0)
			AfxMessageBox("That directory no longer exists.", MB_ICONERROR);
		else {
			pTopFiler->currentDir(fe);
			UpdateAllViews(pTopFiler, true);
		}
	}
}

BOOL CMainFrame::GetMRUName(int n, CString &sName)
{
	return ((CHPCommApp *)AfxGetApp())->GetMRUDirName(n, sName);
}

void CMainFrame::OnMRUDir(UINT nID)
{
	OpenMRUDir(nID - ID_FILE_MRU_FILE1);
}

bool CMainFrame::PrepareExtDrop(HDROP hDrop, CFiler *&fromFiler)
{
	fromFiler = pTopFiler;
	if (fromFiler == 0)
		return false;

	// Get the count of the dropped files.
	UINT n = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
	char sFile[_MAX_PATH];
	// Make sure that all the entries of the dropped files are available. 
	for (UINT i = 0; i < n; i++) {
		DragQueryFile(hDrop, i, sFile, _MAX_PATH);
		CFilerEntry * fe = fromFiler->fromString(sFile);
		// Check if the filerentry is valid.
		if (!fromFiler->isValid(fe))
			return false;
	}

	return true;

}

void CMainFrame::OnClose() 
{
	if (bFinishOnClose && pBottomFiler->isAvailable()) {
		pKermit->Finish();
	}
	CFrameWnd::OnClose();
}

void CMainFrame::OnUpdateCalculatorXmodem(CCmdUI* pCmdUI) 
{
	if (!pBottomFiler->isAvailable()) {
		pCmdUI->Enable(FALSE);
	}
	else {
		pCmdUI->Enable(TRUE);
	}

	pCmdUI->SetCheck(pXModem != 0);
}

void CMainFrame::OnUpdateCalculatorKermit(CCmdUI* pCmdUI) 
{
	if (!pBottomFiler->isAvailable()) {
		pCmdUI->Enable(FALSE);
	}
	else {
		pCmdUI->Enable(TRUE);
	}

	pCmdUI->SetCheck(pKermit != 0);
}

void CMainFrame::ShowPanes()
{
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			m_wndSplitter.GetPane(i, j)->ModifyStyle(0, WS_VISIBLE);
		}
	}
	InvalidateRect(NULL);
	UpdateWindow();
}

void CMainFrame::OnCalculatorRestore() 
{
	CFilerEntry * fe = GetUpperListView()->GetSelectedEntry();
	// Check if there was a selected file.
	if (fe == 0)
		return;

	// Does it end with .hpbackup?
	if (IsArchiveFilename(fe->Name()) == false) {
		AfxMessageBox("Calculator archive files must have a \".hpbackup\" extension.\n"
			"If it's really an archive, please rename it first.", MB_ICONEXCLAMATION);

		return;
	}

	bool bKermit = (pKermit != 0);

    int res = AfxMessageBox("Restoring will erase all data contained in the HOME directory.  Continue?", MB_YESNO);
    if (res != IDYES)
        return;

	bool ret;
	try {
		// Make sure we use kermit as the protocol
		if (!bKermit)
			OnCalculatorKermit();

		ret = pBottomFiler->restoreFromArchive(pTopFiler, fe);

		// If kermit was not the previous protocol, we change it back to XModem.
		if (!bKermit)
			OnCalculatorXmodem();
	}
	catch (CProtException * pEx) {
		ExceptionHandler(pEx);
	}

	Disconnect();
	if (ret == false)
	{
		AfxMessageBox("A problem occurred, and the calculator may not have\n"
			"restored the contents of " + fe->Name() + "\n\n"
			"You should carefully check the contents of your calculator before\n"
			"attempting to reconnect.", MB_ICONSTOP, 0);
	}
	else if (AfxMessageBox("The calculator has restored the contents of " + fe->Name() + "\n\nDo you want to try to reconnect?\n\n(First place the calculator in server mode)", MB_YESNO, 0) == IDYES) {
		// The user wants to reconnect.
		pBottomFiler->invalidate(pBottomFiler->root());

		CWaitCursor wait;
		try {
			pBottomFiler->reconnect();
		}
		catch (CProtException * pEx) {
			ExceptionHandler(pEx);
		}
	}

	UpdateAllViews(pBottomFiler, true);
}

void CMainFrame::Disconnect()
{
	if (pseudoThis == 0)
		return;
	
    if (pseudoThis->pKermit != 0) {
		pseudoThis->pKermit->CloseComPort();
	}

	pseudoThis->pBottomFiler->invalidate(pseudoThis->pBottomFiler->root());
	if (!pseudoThis->bStartup) {
		pseudoThis->UpdateAllViews(pseudoThis->pBottomFiler, true);
	}
}

void CMainFrame::OnSetbookmark()
{
	AfxGetApp()->AddToRecentFileList(pTopFiler->toString(pTopFiler->currentDir()));
}

void CMainFrame::OnUpdateCalculatorBackup(CCmdUI* pCmdUI) 
{
	if (!pBottomFiler->isAvailable()) {
		pCmdUI->Enable(FALSE);
	}
	else {
		pCmdUI->Enable(TRUE);
	}
}

void CMainFrame::OnUpdateCalculatorRestore(CCmdUI* pCmdUI) 
{
	if (!pBottomFiler->isAvailable()) {
		pCmdUI->Enable(FALSE);
	}
	else {
		pCmdUI->Enable(TRUE);
	}
}

void CMainFrame::OnHelpLicense() 
{
    CString cpath = ((CHPCommApp *)AfxGetApp())->OurDir();

    if (cpath.IsEmpty()) {
        AfxMessageBox("Could not find directory name for COPYING.txt", MB_ICONERROR);

        return;
    }

    cpath += "\\COPYING.TXT";

    ShellExecute(m_hWnd, "open", "notepad.exe", cpath, NULL, SW_SHOWNORMAL);
}

void CMainFrame::OnHelpFinder() 
{
	// Check if the user has already chosen a language for his help file.

	// If necessary this call opens a dialog where the user can choose
	// the language.  sHelpLanguage will then contain a valid language.
	ChooseHelpFileName();

    CFrameWnd::OnHelpFinder();
}

void CMainFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CFrameWnd::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	if (bToTop)
		CComInfoDlg::BringToTop();
}

void CMainFrame::BumpComInfoDlg(CFiler * pFiler, int stage, bool bSuccess /* = true */)
{
	if (pKermit->pfConnectBumpCb) {
		if (stage == READING) {
			if (pFiler == pTopFiler) {
				stage = READINGPC;
			}
			else {
				stage = READINGCALC;
			}
		}
		pKermit->pfConnectBumpCb(stage, bSuccess);
	}
}

void CMainFrame::ExceptionHandler(CProtException *pEx)
{
	CString sError;
	switch (pEx->error) {
	case CProt::SYNTAX:
		sError = "Invalid Syntax\n\nThe file you tried to send does not\ncontain valid data or a valid program.";
		pBottomFiler->reRead(pBottomFiler->currentDir());
		break;
	case CProt::LOWBATTERY:
		sError = "The calculator batteries are running low.  To ensure reliable communications,\nplease replace them before you use this program again.";
		break;
	case CProt::OPENPORT:
		sError = "The COM port could not be opened.";

		break;
	default: sError = "Error message from the calculator";
	}
	if (pEx->error == CProt::LOWBATTERY) {
		Disconnect();
	}
	
	if (pEx->error == CProt::RETRIES) {
		//CComInfoDlg::BringToTop();
		CDialog dlg(IDD_CONNECTFAILED);
		dlg.DoModal();
		if (bStartup) {
			Disconnect();
		}
	}
	else {
		MessageBox(sError, "Error message", MB_OK | MB_ICONSTOP);
	}

	pEx->Delete();
}

bool CMainFrame::IsArchiveFilename(const CString &name)
{
	// Does it end with .hpbackup?
	CString end = name;
	end = end.Right(9);
	if (end.CompareNoCase(".hpbackup") != 0) 
		return false;
	else
		return true;
}

bool CMainFrame::CheckClipboard(UINT &cb_format, BOOL &bInternal)
{
	char sName[50];

	// Open the clipboard to access the data.
	if (!OpenClipboard()) {
		((CHPCommApp *)AfxGetApp())->LastErrorMessage(GetLastError());
		return false;
	}

	// Start the enumeration of all available clipboard formats with the 
	// value null.
	cb_format = 0;
	// Enum all available clipbooard formats.
	while (cb_format = EnumClipboardFormats(cb_format)) {
		int n = GetClipboardFormatName(cb_format, sName, 50);
		if (strcmp(sName, HPCommClipboardFormat) == 0) {
			// We found our own internal clipboard format.
			bInternal = TRUE;
			break;
		}
		if (cb_format == CF_HDROP) {
			break;
		}
	}
	
	CloseClipboard();
	
	if (cb_format == 0) {
		return false;
	}

	return true;
}

void CMainFrame::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	UINT cb_format;
	BOOL bInternal;
	pCmdUI->Enable(CheckClipboard(cb_format, bInternal));
}

void CMainFrame::ChooseHelpFileName()
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
            if (sHFName == "HPCOMM")
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

    if (sHelpLanguage == "English")
        sHelpLanguage = "HPCOMM";

	strcpy((char *)pApp->m_pszHelpFilePath, sHelpFilePath + sHelpLanguage + ".hlp");
	// OK, done!

}

void CMainFrame::OnHelplanguage() 
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
