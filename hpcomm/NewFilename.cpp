// NewFilename.cpp : implementation file
//

#include "stdafx.h"
#include "hpcomm.h"
#include "NewFilename.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewFilename dialog


CNewFilename::CNewFilename(CWnd* pParent /*=NULL*/)
	: CDialog(CNewFilename::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewFilename)
	sNewFilename = _T("");
	//}}AFX_DATA_INIT
}


void CNewFilename::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewFilename)
	DDX_Text(pDX, IDC_FILENAME, sNewFilename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewFilename, CDialog)
	//{{AFX_MSG_MAP(CNewFilename)
	ON_BN_CLICKED(IDC_SKIP, OnSkip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewFilename message handlers


BOOL CNewFilename::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetDlgItemText(IDC_INFO, sOldFilename + " is not a valid filename.\n\nPlease confirm or enter the new name below.");
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewFilename::OnSkip() 
{
	// TODO: Add your control notification handler code here
	EndDialog(IDIGNORE);	
}
