// VDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "hpcomm38.h"
#include "VDDlg.h"
#include "VD.h"

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

	VDTree.InsertItem(sPath, 18, 20);

	if (!ReadVD(VDTree.GetRootItem(), 0)) {
		MessageBox("The calculator directory files are corrupted.\n\nPlease reinitialize them.", "ERROR", MB_OK | MB_ICONSTOP);
		EndDialog(1);
	}
	
	VDTree.Expand(VDTree.GetRootItem(), TVE_EXPAND);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVDDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	pImageList->Detach();
	pImageList->DeleteImageList();
	delete pImageList;
	
}

bool CVDDlg::ReadVD(HTREEITEM hParent, const CString *fname)
{
	static const char *validtypes = "ABCDEFGHIJK";

	CString text;


	bool res = getFileContent(text, fname);
	if (res == false)
	{
		return false;
	}

    if (CVD::IdentifyAndRemoveHeader(text) == false)
	{
		return false;
	}
		
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

		// We ignore the UP entry.
		if (name != "[UP]") {
			int nIcon, nSelIcon;
			getIconsFromType(type, name, nIcon, nSelIcon);

			HTREEITEM hDirParent = VDTree.InsertItem(name, nIcon, nSelIcon, hParent, TVI_SORT);

			// If the read entry is a directory, we read that in recursively.
			if (type == 'A') {
				if (!ReadVD(hDirParent, &hostName)) {
					return false;
				}
			}
		}

	}

	return true;
}

bool CVDDlg::getFileContent(CString &sText, const CString *fname)
{
    CVD vd(sPath);

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
    int nSecond; // used later.
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

        nSecond = sName[1] - '0';
		if ((nSecond >= 0) && (nSecond <= 9)) {
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
