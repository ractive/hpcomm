// ArchiveNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "hpcomm.h"
#include "ArchiveNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArchiveNameDlg dialog


CArchiveNameDlg::CArchiveNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CArchiveNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CArchiveNameDlg)
	m_ArchiveName = _T("");
	//}}AFX_DATA_INIT
}


void CArchiveNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArchiveNameDlg)
	DDX_Text(pDX, IDC_ARCHIVENAME, m_ArchiveName);
	DDV_MaxChars(pDX, m_ArchiveName, _MAX_FNAME);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArchiveNameDlg, CDialog)
	//{{AFX_MSG_MAP(CArchiveNameDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArchiveNameDlg message handlers
