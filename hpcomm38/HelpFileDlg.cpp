// HelpFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "hpcomm38.h"
#include "HelpFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHelpFileDlg dialog


CHelpFileDlg::CHelpFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHelpFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHelpFileDlg)
	m_sHelpFile = _T("");
	//}}AFX_DATA_INIT

	pSa = 0;
}


void CHelpFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHelpFileDlg)
	DDX_Control(pDX, IDC_HELPFILES, m_ListBox);
	DDX_LBString(pDX, IDC_HELPFILES, m_sHelpFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHelpFileDlg, CDialog)
	//{{AFX_MSG_MAP(CHelpFileDlg)
	ON_LBN_DBLCLK(IDC_HELPFILES, OnDblclkHelpfiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelpFileDlg message handlers

BOOL CHelpFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// Check if the string array is valid.
	if ((pSa == 0) || (pSa->GetSize() == 0)) {
		EndDialog(1);
	}

	int nIndex = 0;
	// Fill the list box with the string from the string-array
	while (nIndex < pSa->GetSize()) {
		m_ListBox.AddString(pSa->ElementAt(nIndex++));
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHelpFileDlg::OnDblclkHelpfiles() 
{
	// Update the data-member variable.
	UpdateData(FALSE);	
	// Leave the dialog;
	CDialog::OnOK();
}
