// HPCommDoc.cpp : implementation of the CHPCommDoc class
//

#include "stdafx.h"
#include "HPComm.h"

#include "HPCommDoc.h"
#include "CommDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHPCommDoc

IMPLEMENT_DYNCREATE(CHPCommDoc, CDocument)

BEGIN_MESSAGE_MAP(CHPCommDoc, CDocument)
	//{{AFX_MSG_MAP(CHPCommDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHPCommDoc construction/destruction

CHPCommDoc::CHPCommDoc()
{
}

CHPCommDoc::~CHPCommDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
// CHPCommDoc diagnostics

#ifdef _DEBUG
void CHPCommDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CHPCommDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHPCommDoc commands
