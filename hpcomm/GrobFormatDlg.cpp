// GrobFormatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "hpcomm.h"
#include "GrobFormatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGrobFormatDlg dialog


CGrobFormatDlg::CGrobFormatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGrobFormatDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGrobFormatDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGrobFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGrobFormatDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGrobFormatDlg, CDialog)
	//{{AFX_MSG_MAP(CGrobFormatDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGrobFormatDlg message handlers

BOOL CGrobFormatDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	nSel = 0;
	((CButton *)GetDlgItem(IDC_BWRADIO))->SetCheck(1);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGrobFormatDlg::OnOK() 
{
	if (((CButton *)GetDlgItem(IDC_BWRADIO))->GetCheck() == 1)
		nSel = 0;
	else if(((CButton *)GetDlgItem(IDC_OLDRADIO))->GetCheck() == 1) {
		nSel = 1;
	}
	else if(((CButton *)GetDlgItem(IDC_NEWRADIO))->GetCheck() == 1) {
		nSel = 2;
	}
	
	CDialog::OnOK();
}
