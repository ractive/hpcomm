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


// GrobDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GrobDlg.h"
#include "ResizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGrobDlg dialog


CGrobDlg::CGrobDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGrobDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGrobDlg)
	//}}AFX_DATA_INIT
	screendata = NULL;
    bBMP = true;
	bPosFirst = true;
}


void CGrobDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGrobDlg)
	DDX_Control(pDX, IDC_GROB, m_Pic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGrobDlg, CDialog)
	//{{AFX_MSG_MAP(CGrobDlg)
	ON_COMMAND(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_ROTATE, OnRotate)
	ON_BN_CLICKED(IDC_FLIP, OnFlip)
	ON_BN_CLICKED(IDC_MIRROR, OnMirror)
	ON_BN_CLICKED(IDC_INVERT, OnInvert)
	ON_COMMAND(IDC_MAGNIFY, OnMagnify)
	ON_COMMAND(IDC_SHRINK, OnShrink)
	ON_COMMAND(IDC_RELOAD, OnReload)
	ON_COMMAND(ID_BORDERADD, OnBorder)
	ON_WM_CLOSE()
	ON_COMMAND(ID_CUSTOMSIZE, OnCustomsize)
	ON_COMMAND(ID_STANDARDSIZE, OnStandardsize)
	ON_COMMAND(ID_BORDERBLACK, OnBorderblack)
	ON_COMMAND(ID_BORDERWHITE, OnBorderwhite)
	ON_COMMAND(ID_BORDERTHICK, OnBorderthick)
	ON_COMMAND(ID_BORDERTHIN, OnBorderthin)
	ON_COMMAND(ID_BWGROB, OnBwgrob)
	ON_COMMAND(ID_GRAYGROB, OnGraygrob)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(IDC_OPEN, OnOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGrobDlg message handlers

BOOL CGrobDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	bBorder = false;

	nSize = 0;
	sSavedAs.Empty();

	if (sFileName.IsEmpty()) {
		SetWindowText("Screen image");
	}
	else {
		SetWindowText(sFileName);
	}

	CMenu * pMenu = GetMenu();

	BOOL bRet;

	if (sFileName.IsEmpty() && screendata != NULL) {
		// We have data from a screen capture to process.
		bRet = image.LoadScreen(screendata);
	}
	else {
		// We have a file to convert.
		if ((sFileName.Right(4) == ".grb") || (sFileName.Right(4) == ".gro")) {
			bBMP = FALSE;
			bRet = image.LoadGROB(sFileName);
		}
		else {
			bBMP = TRUE;
			bRet = image.LoadBMP(sFileName);
		}
	}

	if (!bRet) {
		EndDialog(1);
	}
	bGrayscale = image.isGrayscale();
	m_Pic.SetBitmap(image.GetHandle());	
	PosPic();
	// Sleep so that the CStatic that holds the pic doesn't receive a LButtonUp message and
	// zoomes the pic.
	// Better solution?
	Sleep(GetDoubleClickTime() + 10);

	// If a BMP file was loaded, we gray the Reload As menuitem
	if (bBMP) {
		pMenu->GetSubMenu(2)->EnableMenuItem(1, MF_BYPOSITION | MF_GRAYED);	
	}

	// Check the "black border" menu item at startup.
	bBorderBlack = true;
	pMenu->GetSubMenu(2)->GetSubMenu(14)->CheckMenuItem(ID_BORDERBLACK, MF_BYCOMMAND | MF_CHECKED);	
	// Check the "thin border" menu item at startup.
	bBorderThin = true;
	pMenu->GetSubMenu(2)->GetSubMenu(13)->CheckMenuItem(ID_BORDERTHIN, MF_BYCOMMAND | MF_CHECKED);
	
	m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDD_GROBVIEW));

	AssureVisible();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CGrobDlg::OnCopy() 
{
	image.CopyToClipboard(this);
}

void CGrobDlg::PosPic()
{
	RECT grobrect, dlgrect;
	// Get the size of the grob
	m_Pic.GetClientRect(&grobrect);
	unsigned int nPicWidth = grobrect.right - grobrect.left;
	unsigned int nPicHeight = grobrect.bottom - grobrect.top;

	// Get the size of the whole window.
	GetWindowRect(&dlgrect);
	// Get the dimensions of the dialog.
	ScreenToClient(&dlgrect);


	// First we adapt the size of the dialog box if necessary.
	
	// Get the height of the title bar.
	unsigned int nTitlebarHeight = GetSystemMetrics(SM_CYSIZE);
	// Adapt the height of the dialog box
	// (The size of the menu is added automatically.)
	dlgrect.bottom = nPicHeight + nTitlebarHeight;

	CClientDC dc(NULL);
	CString sMenuString;
	CMenu * pMenu = GetMenu();
	unsigned int nMenuLength = 0;
	for (int i = 0; i < 3; i++) {
		pMenu->GetMenuString(i, sMenuString, MF_BYPOSITION);
		CSize size = dc.GetTextExtent(sMenuString);
		nMenuLength += size.cx;
	}	
	// If the pic is bigger than the menu width, we size the dialog according to the pic,
	// otherwise we leave it so that the menu is displayed correctly.
	if (nPicWidth > nMenuLength) {
		// Size the dialog so that the pic fits in and add a border (+20).
		dlgrect.right = nPicWidth + 20;		
	}
	else {
		dlgrect.right = nMenuLength + 15;
	}

	// Get the screen coord. to display the window.
	ClientToScreen(&dlgrect);

	// Cascade the screenshots if there is more than one dialog
	// Do this only for the first time we show the picture.
	if (bPosFirst) {
		bPosFirst = false;
		// Move the window to the bottom right
		int nOffset = nScreenDlg * nTitlebarHeight;
		dlgrect.bottom	+= nOffset;
		dlgrect.top		+= nOffset;
		dlgrect.left	+= nOffset;
		dlgrect.right	+= nOffset;
	}

	// Resize and reposition the window.
	MoveWindow(&dlgrect);
}

void CGrobDlg::OnSave() 
{
	int ans = 0;

	CString sFileTitle, sPath;
	// Extract the file name without path and without the extension
	sFileTitle = sFileName.Mid(sFileName.ReverseFind('\\') + 1);
	sFileTitle = sFileTitle.Left(sFileTitle.ReverseFind('.'));
	// Extract the path to the file without the filename.
	sPath = sFileName.Left(sFileName.ReverseFind('\\'));

	if (sPath.IsEmpty())
		sPath = sCurDir;
	
	OPENFILENAME openfilename;
    openfilename.lStructSize = sizeof(OPENFILENAME);
	openfilename.hwndOwner = m_hWnd;
	openfilename.hInstance = AfxGetInstanceHandle();
#ifdef GROBS
	openfilename.lpstrFilter = "GROB file\0*.gro\0BMP file\0*.bmp\0\0";
#else
	openfilename.lpstrFilter = "BMP file\0*.bmp\0\0";
#endif
	openfilename.lpstrCustomFilter = NULL;
	openfilename.nMaxCustFilter = 0;
#ifdef GROBS
	openfilename.nFilterIndex = bBMP ? 1 : 2;
#else
    openfilename.nFilterIndex = 1;
#endif
	openfilename.lpstrFile = sFileTitle.GetBuffer(_MAX_FNAME);;
	openfilename.nMaxFile = MAX_PATH;
	openfilename.lpstrFileTitle = NULL; //sFileTitle.GetBuffer(_MAX_FNAME);
	openfilename.nMaxFileTitle = 0;
	openfilename.lpstrInitialDir = sPath.GetBuffer(_MAX_DIR);
	openfilename.lpstrTitle = "Save image...";
	openfilename.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	openfilename.nFileOffset = 0;
	openfilename.nFileExtension = 0;
#ifdef GROBS
	openfilename.lpstrDefExt = bBMP ? "gro" : "bmp";
#else
    openfilename.lpstrDefExt = "bmp";
#endif
	openfilename.lCustData = 0;
	openfilename.lpfnHook = NULL;
	openfilename.lpTemplateName = NULL;
	ans = GetSaveFileName(&openfilename);

	sFileTitle.ReleaseBuffer();
	sPath.ReleaseBuffer();
	
	if (ans != IDOK) {
		// The user cancelled the save dialog, so we have nothing to do anymore.
		return;
	}

	// Let's check the file ending to determine in what format to store the pic.
	
	if (sFileTitle.Right(4) == ".bmp") {
		image.SaveBMP(&sFileTitle);
		sSavedAs = sFileTitle;
	}
	else {
		image.SaveGROB(&sFileTitle);
	}

	sFileName = sFileTitle;

}


void CGrobDlg::OnRotate() 
{
	// Sorry, it's slow code, so we better show an hourglass :-)
	CWaitCursor wait;
	image.Rotate();
	m_Pic.SetBitmap(image.GetHandle());
	PosPic();
}

void CGrobDlg::OnFlip() 
{
	CWaitCursor wait;
	image.MirrorBitmap(FALSE);
	m_Pic.SetBitmap(image.GetHandle());
	UpdateWindow();
}

void CGrobDlg::OnMirror() 
{
	CWaitCursor wait;
	image.MirrorBitmap(TRUE);
	m_Pic.SetBitmap(image.GetHandle());
	UpdateWindow();
}


void CGrobDlg::OnInvert() 
{
	CWaitCursor wait;
	image.InvertBitmap();
	m_Pic.SetBitmap(image.GetHandle());
	UpdateWindow();
}

void CGrobDlg::OnMagnify() 
{
	CWaitCursor wait;
	image.Magnify(TRUE);
	m_Pic.SetBitmap(image.GetHandle());
	PosPic();
}

void CGrobDlg::OnShrink() 
{
	CWaitCursor wait;
	image.Magnify(FALSE);
	m_Pic.SetBitmap(image.GetHandle());
	PosPic();
}


void CGrobDlg::OnReload() 
{
	image.Reset();
	m_Pic.SetBitmap(image.GetHandle());	
	PosPic();
	
}

void CGrobDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	//delete pMenu;
	CDialog::OnClose();
}

void CGrobDlg::OnCustomsize() 
{
	CResizeDlg dlg;
	CSize size = image.GetSize();
	dlg.m_Width = size.cx;
	dlg.m_Height = size.cy;
	dlg.DoModal();
	image.Resize(CSize(dlg.m_Width, dlg.m_Height));
	m_Pic.SetBitmap(image.GetHandle());
	PosPic();
}

void CGrobDlg::OnStandardsize() 
{
	// The standard display size of the HP is 131 x 64
	image.Resize(CSize(131, 64));
	m_Pic.SetBitmap(image.GetHandle());
	PosPic();
}

void CGrobDlg::OnBorderblack() 
{
	bBorderBlack = true;	
	CMenu * pMenu = GetMenu();
	// Check or uncheck the menuitem.
	pMenu->GetSubMenu(2)->GetSubMenu(14)->CheckMenuItem(ID_BORDERBLACK, MF_BYCOMMAND | MF_CHECKED);	
	pMenu->GetSubMenu(2)->GetSubMenu(14)->CheckMenuItem(ID_BORDERWHITE, MF_BYCOMMAND | MF_UNCHECKED);	
    Border(true);
	UpdateBorder();
}

void CGrobDlg::OnBorderwhite() 
{
	bBorderBlack = false;	
	CMenu * pMenu = GetMenu();
	// Check or uncheck the menuitem.
	pMenu->GetSubMenu(2)->GetSubMenu(14)->CheckMenuItem(ID_BORDERBLACK, MF_BYCOMMAND | MF_UNCHECKED);	
	pMenu->GetSubMenu(2)->GetSubMenu(14)->CheckMenuItem(ID_BORDERWHITE, MF_BYCOMMAND | MF_CHECKED);	
    Border(true);
	UpdateBorder();
}

void CGrobDlg::OnBorderthick() 
{
	bBorderThin = false;	
	CMenu * pMenu = GetMenu();
	// Check or uncheck the menuitem.
	pMenu->GetSubMenu(2)->GetSubMenu(13)->CheckMenuItem(ID_BORDERTHICK, MF_BYCOMMAND | MF_CHECKED);	
	pMenu->GetSubMenu(2)->GetSubMenu(13)->CheckMenuItem(ID_BORDERTHIN, MF_BYCOMMAND | MF_UNCHECKED);	
    Border(true);
	UpdateBorder();
}

void CGrobDlg::OnBorderthin() 
{
	bBorderThin = true;	
	CMenu * pMenu = GetMenu();
	// Check or uncheck the menuitem.
	pMenu->GetSubMenu(2)->GetSubMenu(13)->CheckMenuItem(ID_BORDERTHICK, MF_BYCOMMAND | MF_UNCHECKED);	
	pMenu->GetSubMenu(2)->GetSubMenu(13)->CheckMenuItem(ID_BORDERTHIN, MF_BYCOMMAND | MF_CHECKED);	
    Border(true);
	UpdateBorder();
}

void CGrobDlg::OnBorder() 
{
	bBorder = !bBorder;	
    Border(bBorder);
}


void CGrobDlg::Border(bool on)
{
    bBorder = on;
	CMenu * pMenu = GetMenu();
	// Check or uncheck the menuitem.
	pMenu->GetSubMenu(2)->CheckMenuItem(ID_BORDERADD, MF_BYCOMMAND | bBorder ? MF_CHECKED : MF_UNCHECKED);
	UpdateBorder();
}


void CGrobDlg::UpdateBorder()
{
	if (image.Border(bBorderBlack, bBorderThin, bBorder)) {
		m_Pic.SetBitmap(image.GetHandle());
		PosPic();
	}
}

void CGrobDlg::OnBwgrob() 
{
	// Only useful if a GROB was loaded.
	if (bBMP)
		return;

	if ((sFileName.Right(4) == ".grb") || (sFileName.Right(4) == ".gro")) {
		bBMP = FALSE;
		image.LoadGROB(sFileName, 1);
	}
	m_Pic.SetBitmap(image.GetHandle());	
	PosPic();	
	
}

void CGrobDlg::OnGraygrob() 
{
	// Only useful if a GROB was loaded.
	if (bBMP)
		return;

	if ((sFileName.Right(4) == ".grb") || (sFileName.Right(4) == ".gro")) {
		bBMP = FALSE;
		image.LoadGROB(sFileName, 2);
	}
	m_Pic.SetBitmap(image.GetHandle());	
	PosPic();	
}

void CGrobDlg::AssureVisible()
{
	RECT rect;
	RECT desk_rect;
	
	// Get the rect of the visible area of the primary screen
	SystemParametersInfo(SPI_GETWORKAREA, 0, &desk_rect, 0);
	// Get the rect of the screenshot window
	GetWindowRect(&rect);

	if (rect.right > desk_rect.right || rect.bottom > desk_rect.bottom) {
		// We have to move the screenshot window to the visible part of the screen
		RECT new_rect;
		new_rect.top = 0;
		new_rect.bottom = rect.bottom - rect.top;
		new_rect.left = 0;
		new_rect.right = rect.right - rect.left;
		MoveWindow(&new_rect, FALSE);
	}
}

BOOL CGrobDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_CHAR) {
		/*if (pMsg->wParam == 3 && pMsg->lParam == 3014657) {
			//SendMessage(WM_COMMAND, 0, IDC_COPY);
			image.CopyToClipboard(this);
		}*/
		return ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CGrobDlg::OnOpen() 
{
	// Create a tempfilename if we don't already have one
	if (m_sTempFileName.IsEmpty()) {
		CString sTempPath;
		LPCTSTR pTempPath = sTempPath.GetBuffer(_MAX_PATH);
		GetTempPath(_MAX_PATH, (LPTSTR)pTempPath);
		sTempPath.ReleaseBuffer();
		LPCTSTR pTempFile = m_sTempFileName.GetBuffer(_MAX_PATH);
		GetTempFileName(sTempPath, "hpg", 0, (LPTSTR)pTempFile);
		m_sTempFileName.ReleaseBuffer();
		m_sTempFileName += ".bmp";
	}
	// Save the pic in the temp file
	if (image.SaveBMP(&m_sTempFileName)) {
		// Try to open the created BMP file with the standard program
		ShellExecute(m_hWnd, "open", m_sTempFileName, NULL, NULL, SW_NORMAL);
	}
}

BOOL CGrobDlg::DestroyWindow() 
{
	if (!m_sTempFileName.IsEmpty()) {
		DeleteFile(m_sTempFileName);
		CString s = m_sTempFileName.Left(m_sTempFileName.GetLength() - 4);
		BOOL b = DeleteFile(m_sTempFileName.Left(m_sTempFileName.GetLength() - 4));
		int x = 3;
	}
	
	return CDialog::DestroyWindow();
}
