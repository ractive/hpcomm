/*************************************************************************
  HPGComm
**************************************************************************

Copyright (C) January 2001 by HP and Jean-Pierre Bergamin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
***************************************************************************/


// VDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "hpcomm38.h"
#include "VDDlg.h"
#include "VD.h"
#include "NewVDNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVDDlg dialog


CVDDlg::CVDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVDDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVDDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CVDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVDDlg)
	DDX_Control(pDX, IDC_VDTREE, VDTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVDDlg, CDialog)
	//{{AFX_MSG_MAP(CVDDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CREATEDIR, OnCreatedir)
	ON_NOTIFY(TVN_SELCHANGING, IDC_VDTREE, OnSelchangingVdtree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVDDlg message handlers

BOOL CVDDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	VDTree.DeleteAllItems();

	pImageList = new CImageList;
	CBitmap bitmap;
	// Load the items to the bitmap
	bitmap.LoadBitmap(IDB_VDICONS);
	// Create an appropriate image-list.
	pImageList->Create(16, 16, ILC_COLOR24 | ILC_MASK, 50, 50);
	// Set the "windows" background color.
	pImageList->SetBkColor(GetSysColor(COLOR_WINDOW));
	// Add the "icons" to the image list.
	pImageList->Add(&bitmap, pImageList->GetBkColor());

	VDTree.SetImageList(pImageList, TVSIL_NORMAL);

	CVD vd(m_sPath);
	CString fname;
	vd_type type = vd.IsVD();

	BOOL b38Done = FALSE;

	CString sHostName;

	HTREEITEM hInsertedItem;

	if (type == HP38_VD) {
		DWORD dwMask = TVIF_TEXT | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_IMAGE;
		hInsertedItem = VDTree.InsertItem(dwMask, m_sPath, 30, 20, 0, 0, HP38_VD, NULL, NULL);		
		if (!ReadVD(hInsertedItem, sHostName)) {
			MessageBox("The calculator directory files are corrupted.\n\nPlease reinitialize them.", "ERROR", MB_OK | MB_ICONSTOP);
			EndDialog(1);
		}
		VDTree.Expand(hInsertedItem, TVE_EXPAND);
	}
	if (type == HP39_VD) {
		hInsertedItem = VDTree.InsertItem(m_sPath, 31, 20);
		if (!ReadVD(hInsertedItem, sHostName)) {
			MessageBox("The calculator directory files are corrupted.\n\nPlease reinitialize them.", "ERROR", MB_OK | MB_ICONSTOP);
			EndDialog(1);
		}
		VDTree.Expand(hInsertedItem, TVE_EXPAND);
	}
	if (type == BOTH_VD) {
		/*DWORD dwMask = TVIF_TEXT | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_IMAGE;
		hInsertedItem = VDTree.InsertItem(dwMask, sPath, 30, 20, 0, 0, HP38_VD, NULL, NULL);
		if (!ReadVD(hInsertedItem, sHostName)) {
			MessageBox("The calculator directory files are corrupted.\n\nPlease reinitialize them.", "ERROR", MB_OK | MB_ICONSTOP);
			EndDialog(1);
		}
		
		VDTree.Expand(hInsertedItem, TVE_EXPAND);
		sHostName = "";
		dwMask = TVIF_TEXT | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_IMAGE;
		hInsertedItem = VDTree.InsertItem(dwMask, sPath, 31, 20, 0, 0, HP39_VD, NULL, NULL);
		if (!ReadVD(hInsertedItem, sHostName)) {
			MessageBox("The calculator directory files are corrupted.\n\nPlease reinitialize them.", "ERROR", MB_OK | MB_ICONSTOP);
			EndDialog(1);
		}
		VDTree.Expand(hInsertedItem, TVE_EXPAND);*/
		hInsertedItem = VDTree.InsertItem(m_sPath, 31, 20);
		if (!ReadVD(hInsertedItem, sHostName)) {
			MessageBox("The calculator directory files are corrupted.\n\nPlease reinitialize them.", "ERROR", MB_OK | MB_ICONSTOP);
			EndDialog(1);
		}
		VDTree.Expand(hInsertedItem, TVE_EXPAND);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVDDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	for (int i = 0; i < dirArray.GetSize(); i++) {
		delete (dirEntry *)dirArray.GetAt(i);
	}

	dirArray.RemoveAll();

	pImageList->Detach();
	pImageList->DeleteImageList();
	delete pImageList;
	
}

bool CVDDlg::ReadVD(HTREEITEM hParent, CString &fname)
{
	static const char *header38 = "HP38AscA";
	static const char *header39 = "HP39AscA";
	static const int headerlen = strlen(header38);
	static const char *validtypes = "ABCDEFGHIJK";

	CString text;

	bool bWasNull = (fname == "");
	bool res = getFileContent(text, fname);
	if (res == false)
	{
		return false;
	}

	if (bWasNull) {
		dirEntry * dir = new dirEntry;
		CString sRootName;
		if (fname.Left(4) == "HP38") {
			sRootName = "Root38";
		}
		else {
			sRootName = "Root39";
		}
		strcpy(dir->sName, sRootName);
		strcpy(dir->sHostName, fname);
		strcpy(dir->sParent, "");
		dirArray.Add(dir);
	}

	if (text.GetLength() < headerlen)
	{
		return false;
	}
	
	if (text.Left(headerlen) != header39 && text.Left(headerlen) != header38)
	{
		return false;
	}
	
	// Remove the header.
	text.Delete(0, headerlen);
	
	res = false;
    int line = 0;

    while (1)
	{
		text.TrimLeft();
		if (text.GetLength() == 0)
		{
			return true;
		}
		
		// Now read an entry.
		
		// Get the type.
        CString sType;
        getNextWord(text, sType);
        if (sType.GetLength() != 1)
        {
			return false;
        }

		char type = sType[0];
		
		if (strchr(validtypes, type) == 0)
		{
			return false;
		}

		// Read the host name.
		CString hostName;

		getNextCountedWord(text, hostName);

		if (hostName.IsEmpty())
		{
			return false;
		}

		// Read the 38G name.
		CString name;

		getNextCountedWord(text, name);

		if (name.IsEmpty())
		{
			return false;
		}
		
		if (type == 'A') {
			dirEntry * dir = new dirEntry;
			strcpy(dir->sName, name);
			strcpy(dir->sHostName, hostName);
			strcpy(dir->sParent, VDTree.GetItemText(hParent));
			dirArray.Add(dir);
		}

		// We ignore the UP entry.
		if (name != "[UP]") {
			int nIcon, nSelIcon;
			getIconsFromType(type, name, nIcon, nSelIcon);

			HTREEITEM hDirParent = VDTree.InsertItem(name, nIcon, nSelIcon, hParent, TVI_SORT);

			// If the read entry is a directory, we read that in recursively.
			if (type == 'A') {
				CString sFullName = m_sPath + "\\" + hostName;
				if (!ReadVD(hDirParent, sFullName)) {
					return false;
				}
			}
		}

	}

	return true;
}

bool CVDDlg::getFileContent(CString &sText, CString &fname)
{
    CVD vd(m_sPath);

    FILE *fp = vd.OpenDirFile(fname);
	if (fp == 0)
		return false;

	// Read the whole file into file.
	char ch;
	while ((ch = fgetc(fp)) != EOF)
    {
        // Is it a special character?
        if (ch == '\\')
        {
            // Yes, we got the prefix.  Read the first char.
            ch = fgetc(fp);
            if (ch == EOF) {
				fclose(fp);
				return false;
			}

            // Read the second char.
            unsigned char ch2 = fgetc(fp);
            if (ch2 == EOF) {
				fclose(fp);
				return false;
			}

            // Now convert them to the char val.
            ch = '¿';
        }
		sText += ch;
	}

	fclose(fp);

	if (sText.IsEmpty())
		return false;

	sText.TrimLeft();
	sText.TrimRight();

	return true;
}

void CVDDlg::getNextWord(CString &source, CString &word)
{
    int pos = source.FindOneOf(" \t\n");
    if (source.IsEmpty() || pos == -1)
    {
        word = "";

        return;
    }

    word = source.Left(pos);
    source.Delete(0, pos);
    source.TrimLeft();

}

void CVDDlg::getNextCountedWord(CString &source, CString &word)
{
    source.TrimLeft();

    if (source.IsEmpty())
    {
        word = "";

        return;
    }

    // Grab the first char.
    char first = source[0];
    source.Delete(0,1);

    if (first == '*')
    {
        // FIXME: Need to write this one...

        return;
    }

    if (first < '0' || first > '9')
    {
        // FIXME: Write error handler.
    }

    // Ok, it's a number...

    getNextWord(source, word);
    word = first + word;
    int length;
    int res = sscanf(word, "%d", &length);

    // FIXME: Put code here to check for failed conversion.
	
	if (length > source.GetLength()) {
		word.Empty();
		return;
	}

    word = source.Left(length);
    source.Delete(0, length);
    source.TrimLeft();
}

void CVDDlg::getIconsFromType(char type, const CString &sName, int &nIcon, int &nSelIcon)
{
    nSelIcon = -1;

	switch (type) {
	case 'A':
		// Directory
		nIcon = 18;
		nSelIcon = 20;
		break;
	case 'B':
		// ApLet
		nIcon = 23;
		break;
	case 'C':
		// Note
		nIcon = 17;
		break;
	case 'D':
		// User program
		nIcon = 8;
		break;
	case 'E':
		// Variable

		// Defined are types from A to K but the calc does not use these types correctly.
		// Lists and matrices both have only type E (== Variable).
		// If we have such a type E we check the name to see if it's a list 
		// which has a name Lx or a Matrix with name Mx (where 0 < x < 9).

        // This is the default if we don't find it's a list or matrix.
        nIcon = 24;		
		
		//First check if the second char in the name is a number.
        if (sName.GetLength() < 2)
            break;

		if (isdigit(sName[1])) {
			// Yes, the 2nd char is a number and the type is E
			// Now let's see if it's a list of a matrix
			if (sName[0] == 'L') {
				// We have a list
				nIcon = 4;
			}
			else if (sName[0] == 'M') {
				// We have a matrix
				nIcon = 25;
			}
			else {
				// We have another variable
				nIcon = 24;		
			}
		}

		break;
	case 'F':
		// List
		nIcon = 4;
		break;
	case 'G':
		// Matrix
		nIcon = 25;
		break;
	case 'H':
		// Library
		nIcon = 13;
		break;
	case 'I':
		// Target List
		nIcon = 26;
		break;
	case 'J':
		// Vector
		nIcon = 27;
		break;
	case 'K':
		// Wildcard
		nIcon = 28;
		break;
	default:
		// Unknown, should not happen...
		nIcon = 14;
		break;
	}
	if (nSelIcon == -1)
	    nSelIcon = nIcon;
}

void CVDDlg::OnCreatedir() 
{
	HTREEITEM hSelItem = VDTree.GetSelectedItem();
	if (!hSelItem) {
		hSelItem = VDTree.GetRootItem();
	}
	
	CString sDir = VDTree.GetItemText(hSelItem);
	if (!IsDir(sDir) && !(VDTree.GetRootItem() == hSelItem)) {
		sDir = VDTree.GetItemText(VDTree.GetParentItem(hSelItem));
	}
	
	if (sDir == m_sPath) {
		// Get the name of the root-entry
		HTREEITEM hParent = hSelItem;
		HTREEITEM hTemp = hSelItem;
		while(hParent) {
			hParent = VDTree.GetParentItem(hTemp);
			if (hParent == NULL) {
				hParent = hTemp;
				break;
			}
			hTemp = hParent;
		}
		sDir = VDTree.GetItemData(hParent) == HP38_VD ? "Root38" : "Root39";
	}

	char * sVDFile = GetVDFileName(sDir);
	if (sVDFile == NULL) {
		return;
	}

	bool bWasReadOnly = RemoveReadOnly(m_sPath + "\\" + sVDFile);

	FILE * fp = fopen(m_sPath + "\\" + sVDFile, "a");
	if (!fp) {
		if (bWasReadOnly) {
			SetReadOnly(m_sPath + "\\" + sVDFile);
		}
		return;
	}
	
	CNewVDNameDlg dlg;
	if (dlg.DoModal() == IDCANCEL) {
		if (bWasReadOnly) {
			SetReadOnly(m_sPath + "\\" + sVDFile);
		}
		return;
	}

	CString sNewFile = GetNewVDDirName();

	fprintf(fp, " A %d %s %d %s", sNewFile.GetLength(), sNewFile, dlg.m_sNewName.GetLength(), dlg.m_sNewName);

	fclose(fp);
	
	fp = fopen(m_sPath + "\\" + sNewFile, "a+");
	if (!fp) {
		return;
	}
	
	CString sHeader;
	if (sNewFile.Left(3) == "HP39") {
		sHeader = "HP39AscA";
	}
	else {
		sHeader = "HP38AscA";
	}
	fprintf(fp, sHeader + " A %d %s 4 [UP]", sNewFile.GetLength(), sVDFile);
	fclose(fp);

	if (bWasReadOnly) {
		SetReadOnly(m_sPath + "\\" + sVDFile);
	}


	VDTree.DeleteAllItems();
	
	VDTree.InsertItem(m_sPath, 18, 20);

	CString sHostName;
	if (!ReadVD(VDTree.GetRootItem(), sHostName)) {
		MessageBox("The calculator directory files are corrupted.\n\nPlease reinitialize them.", "ERROR", MB_OK | MB_ICONSTOP);
		EndDialog(1);
	}
	
	VDTree.Expand(VDTree.GetRootItem(), TVE_EXPAND);

}

void CVDDlg::OnSelchangingVdtree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	*pResult = 0;
}

bool CVDDlg::IsDir(CString &sName)
{
	for (int i = 0; i < dirArray.GetSize(); i++) {
		if (strcmp(((dirEntry *)dirArray.GetAt(i))->sName, sName) == 0) {
			return true;
		}
	}

	return false;
}

char * CVDDlg::GetVDFileName(CString &sDir)
{
	for (int i = 0; i < dirArray.GetSize(); i++) {
		if (strcmp(((dirEntry *)dirArray.GetAt(i))->sName, sDir) == 0) {
			return ((dirEntry *)dirArray.GetAt(i))->sHostName;
		}
	}

	return NULL;
}

bool CVDDlg::SetReadOnly(CString &sFilePath)
{
	DWORD dwFileAttributes = GetFileAttributes(sFilePath);
	if (dwFileAttributes & FILE_ATTRIBUTE_READONLY) {
		// Try to add the read-only attribute again.
		if (!SetFileAttributes(sFilePath, (dwFileAttributes & FILE_ATTRIBUTE_READONLY))) {
			MessageBox("The read-only attribute could not set!", "Error", MB_OK | MB_ICONSTOP);
			return false;
		}
	}

	return true;
}

bool CVDDlg::RemoveReadOnly(CString &sFilePath)
{
	// We check if the file has the read-only attribute set, but only if it exists
	// already.
	DWORD dwFileAttributes = GetFileAttributes(sFilePath);
	if (dwFileAttributes & FILE_ATTRIBUTE_READONLY) {
		// Try to remove the read-only attribute.
		if (!SetFileAttributes(sFilePath, (dwFileAttributes & ~FILE_ATTRIBUTE_READONLY))) {
			MessageBox("The read-only attribute could not removed!", "Error", MB_OK | MB_ICONSTOP);
		}
		
		// Return the previous state of the read only attribute.
		return true;
	}
	else
		return false;
}

CString CVDDlg::GetNewVDDirName()
{
	CFileFind finder;
	if (!finder.FindFile(m_sPath + "\\HP3?DIR.*"))
		return "";

	int nMax = -1;

	while (finder.FindNextFile()) {
		int n = -1;
		sscanf(finder.GetFileName().Right(3), "%d", &n);
		nMax = max(n, nMax);
	}

	int n = -1;
	sscanf(finder.GetFileName().Right(3), "%d", &n);
	nMax = max(n, nMax);

	CString s = finder.GetFileName();
	CString sEnding;
	sEnding.Format(".%03d", nMax + 1);
	return finder.GetFileTitle() + sEnding;
}

