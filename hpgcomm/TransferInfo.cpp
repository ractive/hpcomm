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
	DDX_Control(pDX, IDC_ANIMATE, m_Animation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransferInfo, CDialog)
	//{{AFX_MSG_MAP(CTransferInfo)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CLOSE, Close)
	ON_WM_CLOSE()
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

void CTransferInfo::UpdateDlg(CString &sFileName, bool bRecv, int nPackets)
{
	if(!::IsWindow(pseudoThis->m_hWnd)) {
		return;
	}

	pseudoThis->KillTimer(pseudoThis->TimerID);
	
	if (!pseudoThis->m_bPlaying && ::IsWindow(pseudoThis->m_Animation.m_hWnd)) {
		pseudoThis->m_Animation.Play(0, 21, -1);
		pseudoThis->m_bPlaying = TRUE;
	}

	CString sInfo;
	CVD vd(pseudoThis->m_sPath);
	if (vd.IsAVDFile(sFileName)) {
		if (bRecv) {
			pseudoThis->SetWindowText("Receiving file...");
			sInfo = "\r\nReceiving directory information\r\nfrom the calculator";			
		}
		else {
			sInfo = "\r\nSending directory information\r\nto the calculator";		
		}
	}
	else {
		if (bRecv) {
			pseudoThis->SetWindowText("Receiving file...");
			sInfo = "\r\nReceiving file\r\n" + sFileName + "\r\nfrom the calculator";
		}
		else {
			pseudoThis->SetWindowText("Sending file...");
			sInfo = "\r\nSending file\r\n" + sFileName + "\r\nto the calculator";
		}
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
	m_Animation.Open(IDR_CALCTRANS);
	m_bPlaying = FALSE;
	bMainWndWasEnabled = (AfxGetMainWnd()->EnableWindow(FALSE) == FALSE);

	InvalidateRect(NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTransferInfo::ShowInitScreen()
{
	KillTimer(TimerID);
	SetDlgItemText(IDC_INFO, "\r\nWaiting for calculator...");
	m_Animation.Seek(22);
	m_Animation.Stop();
	m_bPlaying = FALSE;
	SetWindowText("");
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
		Invalidate();
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CTransferInfo::OnClose() 
{
	Close();
}
