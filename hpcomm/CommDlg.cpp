// CommDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HPComm.h"
#include "CommDlg.h"
#include "HPCommDoc.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCommDlg dialog


CCommDlg::CCommDlg(CSettings *pSettings)
	: CDialog(CCommDlg::IDD)
{
	//{{AFX_DATA_INIT(CCommDlg)
	bFinish = FALSE;
	//}}AFX_DATA_INIT
	this->pSettings = pSettings;
}


void CCommDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommDlg)
	DDX_Control(pDX, IDC_CALC, m_Calc);
	DDX_Control(pDX, IDC_TRANSIO, m_Transio);
	DDX_Control(pDX, IDC_CHECKSUM, m_Checksum);
	DDX_Control(pDX, IDC_BAUDRATE, m_Baud);
	DDX_Control(pDX, IDC_COM1, m_Com1);
	DDX_Control(pDX, IDC_COM2, m_Com2);
	DDX_Control(pDX, IDC_COM3, m_Com3);
	DDX_Control(pDX, IDC_COM4, m_Com4);
	DDX_Check(pDX, IDC_FINISH, bFinish);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCommDlg, CDialog)
	//{{AFX_MSG_MAP(CCommDlg)
	ON_CBN_SELCHANGE(IDC_CALC, OnSelchangeCalc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommDlg message handlers

BOOL CCommDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	switch (pSettings->dwBaud) {
	case 9600: m_Baud.SetCurSel(3); break;
	case 4800: m_Baud.SetCurSel(2);	break;
	case 2400: m_Baud.SetCurSel(1); break;
	case 1200: m_Baud.SetCurSel(0); break;
	default: VERIFY(1==0); break;
	}

	m_Transio.SetCurSel(pSettings->nTransIO);
	m_Checksum.SetCurSel(pSettings->nChk - 1);
	m_Calc.SetCurSel(pSettings->nCalcType);

	// Build an array of possible com ports.
	CButton* aButtonArray[4] = {&m_Com1, &m_Com2, &m_Com3, &m_Com4};
	for (int i = 1; i <= 4; i++) {
		// Turn it off by default.
		aButtonArray[i - 1]->SetCheck(0);

		// Try to open the port
		CString sPort;
		sPort.Format(_T("\\\\.\\COM%d"), i);
		HANDLE hComPort = ::CreateFile(sPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
		// Disable the comport if it couldn't be opened and if it's not the opened one.
		if (hComPort == INVALID_HANDLE_VALUE && i != pSettings->nComPort) {
			DisableButton(aButtonArray[i - 1]);
		}
		// FIXME: Is there a danger under NT of closing an invalid handle?
		CloseHandle(hComPort);
	}

	// Do we have a default, and is it available?
	if (pSettings->nComPort > 0 && pSettings->nComPort < 5) {
		// Yes.
		aButtonArray[pSettings->nComPort - 1]->SetCheck(1);
	} 
	else {
		// No, so choose the first available one.
		int nAvailable = 0;
		// Work backwards from the top.
		for (int i = 4; i > 0; i--) {
			if (aButtonArray[i - 1]->IsWindowEnabled())
				nAvailable = i;
		}
		if (nAvailable > 0)
			aButtonArray[nAvailable - 1]->SetCheck(1);
		else {
			TRACE("No com ports available.\n");
		}
	}

	if (pSettings->nCalcType == HP38)
		Set38Mode();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCommDlg::DisableButton(CButton * pButton)
{
	pButton->ModifyStyle(0, WS_DISABLED);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->UpdateWindow();
}

void CCommDlg::EnableButton(CButton * pButton)
{
	pButton->ModifyStyle(WS_DISABLED, 0);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->UpdateWindow();
}

void CCommDlg::HideButton(CButton * pButton)
{
	pButton->ModifyStyle(WS_VISIBLE, WS_DISABLED);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->UpdateWindow();
}

void CCommDlg::ShowButton(CButton * pButton)
{
	pButton->ModifyStyle(WS_DISABLED, WS_VISIBLE);
	pButton->InvalidateRect(NULL, TRUE);
	pButton->UpdateWindow();
}

void CCommDlg::OnOK() 
{
	// TODO: Add extra validation here
	CButton* aButtonArray[4] = {&m_Com1, &m_Com2, &m_Com3, &m_Com4};
	
	pSettings->nTransIO = m_Transio.GetCurSel();
	pSettings->nChk = m_Checksum.GetCurSel() + 1;
	pSettings->nCalcType = m_Calc.GetCurSel();

	switch (m_Baud.GetCurSel()) {
	case 0: pSettings->dwBaud = 1200; break;
	case 1: pSettings->dwBaud = 2400; break;
	case 2: pSettings->dwBaud = 4800; break;
	case 3: pSettings->dwBaud = 9600; break;
	}

	for (int i = 1; i <= 4; i++) {
		if ((aButtonArray[i - 1]->GetCheck() == 1) && aButtonArray[i - 1]->IsWindowEnabled()) {
			pSettings->nComPort = i;
		}
	}

	CDialog::OnOK();
}

void CCommDlg::OnSelchangeCalc() 
{
	// TODO: Add your control notification handler code here
	if (m_Calc.GetCurSel() == HP38) {
		Set38Mode();
	}
	else 
		Set4xMode();
}

void CCommDlg::Set38Mode()
{
	m_Baud.SetCurSel(3);
	m_Checksum.SetCurSel(3);
	HideButton((CButton *)&m_Checksum);
	HideButton((CButton *)&m_Baud);
	HideButton((CButton *)&m_Transio);
	InvalidateRect(NULL, TRUE);
	UpdateWindow();
}

void CCommDlg::Set4xMode()
{
	m_Baud.SetCurSel(3);
	m_Checksum.SetCurSel(3);
	ShowButton((CButton *)&m_Checksum);
	ShowButton((CButton *)&m_Baud);
	ShowButton((CButton *)&m_Transio);
	InvalidateRect(NULL, TRUE);
	UpdateWindow();
}
