// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "hpcomm.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

CProgressDlg * CProgressDlg::pseudoThis = NULL;

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	// Disable the main Window. This simulates modal behaviour. The user can't 
	// switch back to the main app.
	AfxGetMainWnd ()->EnableWindow (FALSE);

}


void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDlg)
	DDX_Control(pDX, IDC_PROGRESSALL, ProgressAll);
	DDX_Control(pDX, IDC_PROGRESS, ProgressFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CProgressDlg)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_SKIP, OnSkip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg message handlers

void CProgressDlg::UpdateDlg(int nType, void* param)
{
	// Make sure it doesn't assert if we get called when no window is
	// shown.
	if (!::IsWindow(pseudoThis->m_hWnd))
		return;

	static unsigned int oldpos = 0;
	static unsigned int all = 0;
	static time_t start = 0;

	if (nType == FILENAME || nType == HPFILENAME) {
		CString * name = (CString *)param;
		if (nType == FILENAME) {
			pseudoThis->SetDlgItemText(IDC_FILENAME, *name);
		}
		else {
			CString sName;
			pseudoThis->GetDlgItemText(IDC_FILENAME, sName);
			if (*name != sName) {
				pseudoThis->SetDlgItemText(IDC_FILENAME, sName + "    renamed to: " + *name);
			}
		}
		// We have a new file, so we set the progress bar to 0
		pseudoThis->ProgressFile.SetPos(0);
		oldpos = 0;
		pseudoThis->Pump();
		return;
	}
	unsigned int *val = (unsigned int *)param;
	if (nType == RANGEALL) {
		pseudoThis->ProgressAll.SetRange32(0, *val);
		time(&start);
	}
	else if (nType == RANGE) {
		pseudoThis->ProgressFile.SetRange32(0, *val);
	}
	else if (nType == FILEPOS) {
		pseudoThis->ProgressFile.SetPos(*val);
		// Only step as much forward as we read in during the last
		// step: new - oldfileposition
		pseudoThis->ProgressAll.OffsetPos(*val - oldpos);
		CString s;
		time_t now;
		time(&now);
		all += (*val - oldpos);
		s.Format("%d CPS", (int)(all / difftime(now, start)));
		pseudoThis->SetDlgItemText(IDC_SPEED, s);
		oldpos = *val;
	}
	else if (nType == ERRORS) {
		pseudoThis->SetDlgItemInt(IDC_ERRORS, *val, false);
	}
	else if (nType == PACKETNO) {
		pseudoThis->SetDlgItemInt(IDC_PACKETNO, *val, false);
	}
	pseudoThis->UpdateWindow();
	pseudoThis->Pump();
}

BOOL CProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Looks better :-)
	CenterWindow();

	if (bSend)
		SetWindowText("Sending files...");
	else {
		SetWindowText("Receiving files...");
		// Although Kermit has the feature to discard a receiving file
		// (whith an ACK and an X in the data-filed) the HP48 doesn't
		// support it. So skipping single files is not possible while receiving.
		// We disable the button.
		GetDlgItem(IDC_SKIP)->ModifyStyle(0, WS_DISABLED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CProgressDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Set the pseudoThis = this.
	// This allows UpdateDlg to access the member vars with pseudoThis->xy
	pseudoThis = this;
	return 0;
}

BOOL CProgressDlg::Pump()
{
	MSG msg;

	// Retrieve and dispatch any waiting messages.

	while (::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (!AfxGetApp ()->PumpMessage ()) {
			::PostQuitMessage (0);
			return FALSE;
		}
	}

	// Simulate the framework's idle processing mechanism.
/*
	LONG lIdle = 0;
	while (AfxGetApp ()->OnIdle (lIdle++));
*/
	return TRUE;

}

void CProgressDlg::Close()
{
	if (::IsWindow (pseudoThis->m_hWnd)) {
		AfxGetMainWnd ()->EnableWindow (TRUE);
		pseudoThis->DestroyWindow ();
	}
}

void CProgressDlg::OnSkip() 
{
	*bSkipFile = true;	
}

void CProgressDlg::OnCancel() 
{
	*bCancel = true;	
	CDialog::OnCancel();
}
