// DirectoryEdit.cpp : implementation file
//

#include "stdafx.h"
#include "hpcomm38.h"
#include "DirEditCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirEditCtrl

CDirEditCtrl::CDirEditCtrl()
{
}

CDirEditCtrl::~CDirEditCtrl()
{
}


BEGIN_MESSAGE_MAP(CDirEditCtrl, CEdit)
	//{{AFX_MSG_MAP(CDirEditCtrl)
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirEditCtrl message handlers

void CDirEditCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Let's see if the return key was hit.
	if (nChar == 13) {
		CString sText;
		GetWindowText(sText);
		CFileFind finder;
		// Check if the directory exists.
		if (!finder.FindFile(sText)) {
			// Give an error message that this directory is not valid.
			CString sMsg = sText;
			sMsg += " does not exist.  Do you want to create it?";
			if (MessageBox(sMsg, "ERROR", MB_YESNO | MB_ICONSTOP) == IDYES) {
				if (sText.Right(1) != "\\")
					sText += "\\"; // Add a trailing backslash
				BOOL bRet = CreateDirectory(sText, NULL);
				DWORD dwError = GetLastError();
				int nPos = 0;
				// dwError == 3 means that the specified path was not found.
				// For us this means that we have some sub-directories that
				// don't exist yet.
				if (!bRet && (dwError == 3)) {
					nPos = sText.Find("\\");
					// The dir could not be created, cause a subdirectory was not
					// created yet.
					// Now we just create the whole path. If a subdir already exists
					// is not important, then the CreateDirectory just fails.
					BOOL bRes = true;
					while (nPos != -1) {
						CString sDir = sText.Left(nPos = sText.Find("\\", ++nPos));
						if (sDir.IsEmpty())
							break;
						bRes &= CreateDirectory(sDir, NULL);
					}
					// Check if the last dir could be created or not.
					if (!bRes) {
						MessageBox("The directory could not be created.", "ERROR", MB_OK | MB_ICONSTOP);
					}
				}
			}
		}
		finder.Close();
	}
	
	CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}
