// AboutDlg.cpp: implementation of the CAboutDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AboutDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_GPL, gplURL);
	DDX_Control(pDX, IDC_WWWHPCOMM, hpcommURL);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	hpcommURL.SetURL("http://www.hpcomm.org");
	gplURL.SetURL("http://www.gnu.org/copyleft/gpl.html");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
