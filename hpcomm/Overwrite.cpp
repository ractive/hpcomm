// Overwrite.cpp: implementation of the COverwrite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Overwrite.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COverwrite dialog


COverwrite::COverwrite(CWnd* pParent /*=NULL*/)
	: CDialog(COverwrite::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewFilename)
	sFilename = _T("");
	sDirname = _T("");
	bAllFiles = FALSE;
	//}}AFX_DATA_INIT
}


void COverwrite::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COverwrite)
#if 0
	DDX_Check(pDX, IDC_ALLFILES, bAllFiles);
#endif
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COverwrite, CDialog)
	//{{AFX_MSG_MAP(COverwrite)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_BN_CLICKED(IDNO, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COverwrite message handlers


BOOL COverwrite::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString s("Directory \"" + sDirname + "\" already contains an entry called ");
	s += "\"" + sFilename + "\"";
	SetDlgItemText(IDC_OVER1,  s);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COverwrite::OnYes() 
{
	int res;
	UpdateData();
#if 0
	if (bAllFiles)
		res = IDYESALL;
	else
		res = IDYES;
#else
	res = IDYES;
#endif

	EndDialog(res);
}

void COverwrite::OnNo() 
{
	int res;
	UpdateData();
#if 0
	if (bAllFiles)
		res = IDNOALL;
	else
		res = IDNO;
#else
	res = IDNO;
#endif

	EndDialog(res);
}
