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


// TransferInfo.cpp : implementation file
//

#include "stdafx.h"
#include "HPComm38.h"
#include "TransferInfo.h"
#include "VD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTransferInfo dialog
CTransferInfo * CTransferInfo::pseudoThis = 0;

CTransferInfo::CTransferInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CTransferInfo::IDD, pParent), rb("Settings")
{
	//{{AFX_DATA_INIT(CTransferInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	rb.Bind(m_bShowDetails, "Transfer details", FALSE);
	
	TimerID = 1;
	QuitTimerID = 2;
	HideTimerID = 3;
	m_bVisible = FALSE;
}


void CTransferInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransferInfo)
	DDX_Control(pDX, IDC_DETAILSGROUP, m_DetailsGroup);
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressBar);
	DDX_Control(pDX, IDC_ANIMATE, m_Animation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransferInfo, CDialog)
	//{{AFX_MSG_MAP(CTransferInfo)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_DETAILS, OnDetails)
	ON_MESSAGE(WM_TRANSFER_POS, OnPos)
	ON_MESSAGE(WM_TRANSFER_RANGE, OnRange)
	ON_MESSAGE(WM_TRANSFER_SENDPOS, OnSendPos)
	ON_MESSAGE(WM_TRANSFER_HIDE, OnHide)
	ON_MESSAGE(WM_TRANSFER_QUIT, OnQuit)
	ON_MESSAGE(WM_TRANSFER_INFO, OnInfo)
	ON_MESSAGE(WM_TRANSFER_FILEINFO, OnFileInfo)
	ON_MESSAGE(WM_TRANSFER_INIT, OnInit)

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

void CTransferInfo::UpdateDlg(CString &sFileName, BOOL bRecv, int nPackets)
{
	if(!::IsWindow(pseudoThis->m_hWnd)) {
		return;
	}

	pseudoThis->KillTimer(pseudoThis->HideTimerID);
	pseudoThis->KillTimer(pseudoThis->TimerID);
	
	if (!pseudoThis->m_bPlaying && ::IsWindow(pseudoThis->m_Animation.m_hWnd)) {
		pseudoThis->m_Animation.Play(0, 21, -1);
		pseudoThis->m_bPlaying = TRUE;
	}

	CString sInfo;
	CVD vd(pseudoThis->m_sPath);
	if (vd.IsAVDFile(sFileName)) {
		if (bRecv) {
			pseudoThis->m_ProgressBar.ModifyStyle(WS_VISIBLE, 0);
			pseudoThis->SetWindowText("Receiving...");
			sInfo = "\r\nReceiving directory information\r\nfrom the calculator";			
		}
		else {
			pseudoThis->m_ProgressBar.ModifyStyle(0, WS_VISIBLE);
			pseudoThis->Invalidate();
			pseudoThis->SetWindowText("Sending...");
			sInfo = "\r\nSending directory information\r\nto the calculator";		
		}
	}
	else {
		if (bRecv) {
			pseudoThis->m_ProgressBar.ModifyStyle(WS_VISIBLE, 0);
			pseudoThis->SetWindowText("Receiving file...");
			sInfo = "\r\nReceiving file\r\n" + sFileName + "\r\nfrom the calculator";
		}
		else {
			pseudoThis->m_ProgressBar.ModifyStyle(0, WS_VISIBLE);
			pseudoThis->Invalidate();
			pseudoThis->SetWindowText("Sending file...");
			sInfo = "\r\nSending file\r\n" + sFileName + "\r\nto the calculator";
		}
	}

	pseudoThis->SetDlgItemText(IDC_INFO, (LPCTSTR)sInfo);
}

void CTransferInfo::SetRange(unsigned long unSize)
{
	time(&pseudoThis->m_StartTime);
	pseudoThis->m_nPackets = 0;
	pseudoThis->m_ulSize = unSize;
	pseudoThis->m_unPos = 0;
	pseudoThis->m_ProgressBar.SetRange32(0, unSize);
	pseudoThis->SetDlgItemText(IDC_BYTES, "0");
	pseudoThis->SetDlgItemInt(IDC_PACKETS, 0);
}

void CTransferInfo::SetPos(unsigned long unEffBytes, unsigned long unTotalBytes)
{
	pseudoThis->m_unPos = unEffBytes;
	pseudoThis->m_unTotalBytes = unTotalBytes;

	pseudoThis->m_nPackets++;


	time_t now;
	time(&now);
	int nSec = (int)difftime(now, pseudoThis->m_StartTime);
	CString sRunningTime;
	sRunningTime.Format("%02d:%02d", nSec / 60, nSec % 60);
	if (!::IsWindow(pseudoThis->m_hWnd)) {
		// If the window doesn't exist any more, we leave the thread

		return;
	}
	int nPercent = 0;
	if (pseudoThis->m_ulSize > 0) {
		nPercent = (100 * pseudoThis->m_unPos)/pseudoThis->m_ulSize;
	}
	if (nPercent > 100) {
		nPercent = 100;
	}
	CString sPercent;
	sPercent.Format("%d%%", nPercent);
	pseudoThis->SetDlgItemText(IDC_PERCENT, sPercent);
	pseudoThis->SetDlgItemText(IDC_TIMERUNNING, sRunningTime);
	pseudoThis->m_ProgressBar.SetPos(pseudoThis->m_unPos);
	pseudoThis->SetDlgItemInt(IDC_BYTES, pseudoThis->m_unPos);
	pseudoThis->SetDlgItemInt(IDC_PACKETS, pseudoThis->m_nPackets);
	if (nSec > 0) {
		int nCPS = pseudoThis->m_unPos / nSec * 8 / 10; // * 8 / 10 "substracts" the start and stop bit
		pseudoThis->SetDlgItemInt(IDC_CPS, nCPS);
		pseudoThis->SetDlgItemInt(IDC_BPS, pseudoThis->m_unTotalBytes * 8 / nSec);

		if (nCPS > 0 && pseudoThis->m_ulSize > 0 && pseudoThis->m_ulSize > pseudoThis->m_unPos) {
			int nRemaining = (pseudoThis->m_ulSize - pseudoThis->m_unPos) / nCPS + 1;
			CString sRemainingTime;
			sRemainingTime.Format("%02d:%02d", nRemaining / 60, nRemaining % 60);
			pseudoThis->SetDlgItemText(IDC_REMAINING, sRemainingTime);
		}
	}
}


void CTransferInfo::SetSendPos(unsigned long ulBytes)
{
	pseudoThis->m_ProgressBar.ShowWindow(SW_HIDE);
	pseudoThis->m_nPackets++;
	pseudoThis->m_unPos += ulBytes;
	time_t now;
	time(&now);
	int nSec = (int)difftime(now, pseudoThis->m_StartTime);
	CString sRunningTime;
	sRunningTime.Format("%02d:%02d", nSec / 60, nSec % 60);
	pseudoThis->SetDlgItemText(IDC_TIMERUNNING, sRunningTime);

	pseudoThis->SetDlgItemInt(IDC_BYTES, pseudoThis->m_unPos);
	pseudoThis->SetDlgItemInt(IDC_PACKETS, pseudoThis->m_nPackets);
	if (nSec > 0) {
		int nCPS = pseudoThis->m_unPos / nSec;
		pseudoThis->SetDlgItemInt(IDC_CPS, nCPS);
		pseudoThis->SetDlgItemInt(IDC_BPS, pseudoThis->m_unPos * 8 / nSec);
	}

}

BOOL CTransferInfo::Pump()
{
	MSG msg;

	// Retrieve and dispatch any waiting messages.

	while (::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (!AfxGetApp ()->PumpMessage ()) {
			::PostQuitMessage (0);
			return FALSE;
		}
	}
	return TRUE;
}

void CTransferInfo::ShowUserInfo()
{
	KillTimer(TimerID);
	// Start a timer. If 2 sec. are passed and the timer was not killed,
	// the UserInfo is shown (in OnTimer()).
	TimerID = SetTimer(1, 1000, NULL);
}

BOOL CTransferInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CenterWindow();
	m_Animation.Open(IDR_CALCTRANS);
	m_bPlaying = FALSE;
	//m_bMainWndWasEnabled = (AfxGetMainWnd()->EnableWindow(FALSE) == FALSE);

	InvalidateRect(NULL);

	m_hCancelEvent = NULL;
	m_bDetailsShown = TRUE;
	ShowDetails(m_bShowDetails);

	m_unTotalBytes = 0;
	m_unPos = 0;
	m_ulSize = 0;

	ShowInitScreen(FALSE);

	AfxGetMainWnd()->EnableWindow(TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTransferInfo::ShowInitScreen(BOOL bXModem)
{
	KillTimer(HideTimerID);
	KillTimer(TimerID);
	m_Animation.Seek(22);
	m_Animation.Stop();
	m_bPlaying = FALSE;
	m_ProgressBar.ModifyStyle(WS_VISIBLE, 0);
	if (bXModem) {
		SetDlgItemText(IDC_INFO, "\r\nChoose a file to transfer...");
		SetWindowText("Choose file...");
		
	}
	else {
		SetDlgItemText(IDC_INFO, "\r\nWaiting for calculator...");
		SetWindowText("Waiting...");
	}

}

void CTransferInfo::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == TimerID) {
		KillTimer(TimerID);
		MessageBeep(MB_OK);
		SetDlgItemText(IDC_INFO, "\r\nSelect the files to transfer\r\non the calculator");
		SetWindowText("Please choose");
		m_Animation.Stop();
		m_Animation.Seek(22);
		m_bPlaying = FALSE;
		m_ProgressBar.SetPos(0);
		m_ProgressBar.ModifyStyle(WS_VISIBLE, 0);
		Invalidate();
		m_unPos = 0;
		m_unTotalBytes = 0;
		m_ulSize = 0;
		m_nPackets = 0;
		time(&m_StartTime);
		SetDlgItemText(IDC_CPS, "");
		SetDlgItemText(IDC_BPS, "");
		SetDlgItemText(IDC_PACKETS, "");
		SetDlgItemText(IDC_REMAINING, "");
		SetDlgItemText(IDC_TIMERUNNING, "");
		SetDlgItemText(IDC_BYTES, "");
		SetDlgItemText(IDC_PERCENT, "");

		//SetEvent(m_hUpdate);
	}
	else if (nIDEvent == QuitTimerID) {
		KillTimer(nIDEvent);
		OnClose();
	}
	else if (nIDEvent == HideTimerID) {
		KillTimer(nIDEvent);
		PostMessage(WM_CLOSE, 0, 0);
		TRACE("Hide window\r\n");
	}

	CDialog::OnTimer(nIDEvent);
}

void CTransferInfo::OnClose() 
{
	//AfxGetMainWnd()->EnableWindow(m_bMainWndWasEnabled);

	/*if (m_hCancelEvent != NULL) {
		SetEvent(m_hCancelEvent);
	}*/
	TRACE("OnClose\n");
	if (::IsWindow(pseudoThis->m_hWnd)) {
		AfxGetMainWnd()->SendMessage(WM_TRANSFERINFOCLOSED);
	}

	m_bShowDetails = m_bDetailsShown;

	CDialog::OnClose();
}


void CTransferInfo::OnDetails() 
{
	ShowDetails(!m_bDetailsShown);	
}

void CTransferInfo::ShowDetails(BOOL bShow /* = TRUE */)
{
	if (m_bDetailsShown && bShow) {
		return;
	}
	if (!m_bDetailsShown && !bShow) {
		return;
	}


	RECT dlgrect, inforect;
	GetWindowRect(&dlgrect);
	m_DetailsGroup.GetWindowRect(&inforect);
	static int offset = dlgrect.bottom - inforect.bottom;
	int nOldBottom = dlgrect.bottom;

	if (bShow) {
		// Expand the dialog
		dlgrect.bottom += ((inforect.bottom - inforect.top) + offset);
		SetDlgItemText(IDC_DETAILS, "<<< Details");
		m_bDetailsShown = TRUE;
	}
	else {
		// Make the dialog smaller.
		dlgrect.bottom -= ((inforect.bottom - inforect.top) + offset);
		SetDlgItemText(IDC_DETAILS, "Details >>>");
		m_bDetailsShown = FALSE;
	}
	
	MoveWindow(&dlgrect);


}

void CTransferInfo::OnRange(UINT pParam, long lParam)
{
	SetRange(lParam);
}

void CTransferInfo::OnPos(UINT pParam, long lParam)
{
	SetPos(pParam, lParam);
}

void CTransferInfo::OnSendPos(UINT pParam, long lParam)
{
	SetSendPos(lParam);
}

void CTransferInfo::OnQuit(UINT pParam, long lParam)
{
	SetTimer(QuitTimerID, 1000, NULL);
}

void CTransferInfo::OnHide(UINT pParam, long lParam)
{
	KillTimer(HideTimerID);
	SetTimer(HideTimerID, lParam, NULL);
}

void CTransferInfo::OnInfo(UINT pParam, long lParam)
{
	ShowUserInfo();
}

void CTransferInfo::OnFileInfo(UINT pParam, long lParam)
{
	CString s((LPTSTR)pParam);
	UpdateDlg(s, lParam);
}

void CTransferInfo::OnInit(UINT pParam, long lParam)
{
	KillTimer(HideTimerID);
	if (lParam == TRUE) {
		ShowInitScreen(lParam);
	}
}


void CTransferInfo::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	if (!m_bVisible) {
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	}
	
	CDialog::OnWindowPosChanging(lpwndpos);
}

