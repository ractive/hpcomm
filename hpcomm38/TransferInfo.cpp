// TransferInfo.cpp : implementation file
//

#include "stdafx.h"
#include "HPComm38.h"
#include "TransferInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_TRANSFERINFOCLOSED (WM_USER + 5)

/////////////////////////////////////////////////////////////////////////////
// CTransferInfo dialog
CTransferInfo * CTransferInfo::pseudoThis = 0;

CTransferInfo::CTransferInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CTransferInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransferInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	TimerID = 1;
}


void CTransferInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransferInfo)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransferInfo, CDialog)
	//{{AFX_MSG_MAP(CTransferInfo)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CLOSE, Close)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransferInfo message handlers

int CTransferInfo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	pseudoThis = this;
	return 0;
}

void CTransferInfo::UpdateDlg(CString &sFileName, bool bRecv)
{
	pseudoThis->KillTimer(pseudoThis->TimerID);

	pseudoThis->InvalidateRect(NULL);
	pseudoThis->UpdateWindow();

	pseudoThis->BeginWaitCursor();

	CString sInfo;
	if (bRecv) {
		pseudoThis->SetWindowText("Receiving file...");
		sInfo = "\nReceiving file\n" + sFileName + "\nfrom the calculator";
	}
	else {
		pseudoThis->SetWindowText("Sending file...");
		sInfo = "\nSending file\n" + sFileName + "\nto the calculator";
	}

	pseudoThis->SetDlgItemText(IDC_INFO, (LPCTSTR)sInfo);

}

void CTransferInfo::Pump()
{
	MSG msg;

	// Retrieve and dispatch any waiting messages.

	while (::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (!AfxGetApp ()->PumpMessage ()) {
			::PostQuitMessage (0);
			return;
		}
	}
}

void CTransferInfo::Close()
{
	if (::IsWindow(pseudoThis->m_hWnd)) {
		AfxGetMainWnd()->EnableWindow(bMainWndWasEnabled);
		pseudoThis->DestroyWindow();
		AfxGetMainWnd()->PostMessage(WM_TRANSFERINFOCLOSED);
	}

}

void CTransferInfo::ShowUserInfo()
{
	KillTimer(TimerID);
	// Start a timer. If 2 sec. are passed and the timer was not killed,
	// the UserInfo is shown (in OnTimer()).
	TimerID = SetTimer(1, 2000, NULL);
}

BOOL CTransferInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	bMainWndWasEnabled = (AfxGetMainWnd()->EnableWindow(FALSE) == FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTransferInfo::ShowInitScreen()
{
	KillTimer(TimerID);
	SetDlgItemText(IDC_INFO, "\n\nWaiting for calculator...");
	SetWindowText("");
}

void CTransferInfo::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == TimerID) {
		KillTimer(TimerID);
		EndWaitCursor();
		MessageBeep(MB_OK);
		SetDlgItemText(IDC_INFO, "\n\nOn the calculator, select the files to transfer.");
		InvalidateRect(NULL);
		UpdateWindow();
	}
	
	CDialog::OnTimer(nIDEvent);
}
