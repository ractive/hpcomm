// Properties.cpp : implementation file
//

#include "stdafx.h"
#include "HPComm.h"
#include "Properties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProperties dialog


CProperties::CProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProperties)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProperties)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProperties, CDialog)
	//{{AFX_MSG_MAP(CProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProperties message handlers

BOOL CProperties::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CFileFind finder;
	if (finder.FindFile(sFileName)) {
		finder.FindNextFile();
		SetDlgItemText(IDC_FILENAME, finder.GetFileName());
		CString sTmp = finder.GetFilePath();
		sTmp = sTmp.Left(sTmp.ReverseFind('\\') + 1);
		SetDlgItemText(IDC_LOCATION, sTmp);
		CTime crtime;
		finder.GetCreationTime(crtime);
		SetDlgItemText(IDC_DATE, crtime.Format("%#c"));
		finder.GetLastWriteTime(crtime);
		SetDlgItemText(IDC_LASTWRITE, crtime.Format("%#c"));
		GetShortPathName(sFileName, sTmp.GetBuffer(MAX_PATH), MAX_PATH);
		sTmp.ReleaseBuffer();
		SetDlgItemText(IDC_DOSFILENAME, sTmp.Mid(sTmp.ReverseFind('\\') + 1));
		sTmp.Format("%lu", finder.GetLength());
		SetDlgItemText(IDC_FILESIZE, sTmp);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
