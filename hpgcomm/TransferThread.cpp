// TransferThread.cpp : implementation file
//

#include "stdafx.h"
#include "hpcomm38.h"
#include "TransferThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransferThread

IMPLEMENT_DYNCREATE(CTransferThread, CWinThread)

CTransferThread::CTransferThread()
{
	m_pInfoDlg = 0;
}

CTransferThread::~CTransferThread()
{
}

BOOL CTransferThread::InitInstance()
{
	/*CTransferInfo dlg;
	dlg.m_hCancelEvent = m_hCancelEvent;
	m_pInfoDlg = &dlg;
	dlg.DoModal();*/
	m_pInfoDlg = new CTransferInfo;
	m_pInfoDlg->Create(IDD_TRANSFERINFO, AfxGetMainWnd());
	return TRUE;
}

int CTransferThread::ExitInstance()
{
	if (m_pInfoDlg) {
		m_pInfoDlg->DestroyWindow();
	}
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CTransferThread, CWinThread)
	//{{AFX_MSG_MAP(CTransferThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_THREAD_MESSAGE(WM_TRANSFER_RANGE, OnRange)
	ON_THREAD_MESSAGE(WM_TRANSFER_POS, OnPos)
	ON_THREAD_MESSAGE(WM_TRANSFER_SENDPOS, OnSendPos)
	ON_THREAD_MESSAGE(WM_TRANSFER_QUIT, OnQuit)
	ON_THREAD_MESSAGE(WM_TRANSFER_INFO, OnInfo)
	ON_THREAD_MESSAGE(WM_TRANSFER_FILEINFO, OnFileInfo)
	ON_THREAD_MESSAGE(WM_TRANSFER_INIT, OnInit)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransferThread message handlers

void CTransferThread::OnRange(UINT pParam, long lParam)
{
	m_pInfoDlg->SetRange(lParam);
}

void CTransferThread::OnPos(UINT pParam, long lParam)
{
	m_pInfoDlg->SetPos(pParam, lParam);
}

void CTransferThread::OnSendPos(UINT pParam, long lParam)
{
	m_pInfoDlg->SetSendPos(lParam);
}

void CTransferThread::OnQuit(UINT pParam, long lParam)
{
	::PostQuitMessage(0);
}

void CTransferThread::OnInfo(UINT pParam, long lParam)
{
	m_pInfoDlg->ShowUserInfo();
}

void CTransferThread::OnFileInfo(UINT pParam, long lParam)
{
	CString s((LPTSTR)pParam);
	m_pInfoDlg->UpdateDlg(s, lParam);
}

void CTransferThread::OnInit(UINT pParam, long lParam)
{
	if (m_pInfoDlg && ::IsWindow(m_pInfoDlg->m_hWnd))
		m_pInfoDlg->ShowInitScreen(lParam);
}


BOOL CTransferThread::OnIdle(LONG lCount) 
{
	// TODO: Add your specialized code here and/or call the base class
	//Pump();
	return CWinThread::OnIdle(lCount);
}

void CTransferThread::Pump()
{
	MSG msg;

	// Retrieve and dispatch any waiting messages.

	while (::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (!PumpMessage ()) {
			::PostQuitMessage (0);
			return;
		}
	}

	if (m_pInfoDlg) {
		m_pInfoDlg->Pump();
	}
}
