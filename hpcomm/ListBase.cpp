// ListBase.cpp : implementation file
//

#include "stdafx.h"
#include "hpcomm.h"
#include "ListBase.h"
#include "Properties.h"
#include "GrobDlg.h"
#include "MainFrame.h"
#include "ProtException.h"
#include "CalcTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListBase

IMPLEMENT_DYNCREATE(CListBase, CListView)

CListBase::CListBase()
{
	m_pDragImage = NULL;
	nDropItem = -1;
	nItem = 0;
	bInternalCF = false;
	bSortAscending = TRUE;
	nSortedCol = 0;
	bNewFolder = false;
	pEdit = NULL;
	nScrollWait = InitialScrollWait;
}

CListBase::~CListBase()
{
	if (m_pDragImage)
		delete m_pDragImage;

}


BEGIN_MESSAGE_MAP(CListBase, CListView)
	//{{AFX_MSG_MAP(CListBase)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_COMMAND(ID_DIRPOPUP_EXPLORE, OnDirpopupExplore)
	ON_COMMAND(ID_FILE_OPEN, OnOpeninexplorer)
	ON_COMMAND(ID_DIRPOPUP_SEND, OnDirpopupSend)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHeaderclick)
	ON_COMMAND(ID_POPUP_DELETE, OnPopupDelete)
	ON_COMMAND(ID_POPUP_OPEN, OnPopupOpen)
	ON_COMMAND(ID_POPUP_OPENWITHNOTEPAD, OnPopupOpenwithnotepad)
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_POPUP_SEND_KERMIT, OnPopupSendKermit)
	ON_COMMAND(ID_POPUP_SEND_XMODEM, OnPopupSendXmodem)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_COMMAND(ID_RENAME, OnRename)
	ON_WM_CREATE()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_WM_CLOSE()
	ON_COMMAND(ID_SELECTALL, OnSelectall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBase diagnostics

#ifdef _DEBUG
void CListBase::AssertValid() const
{
	CListView::AssertValid();
}

void CListBase::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CListBase message handlers

void CListBase::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW* pNMListView = (NM_LISTVIEW *)pNMHDR;

	CFilerEntry * pEntry = GetEntry(pNMListView->iItem);

	if (pFiler->isAvailable() && pFiler->canBeDragged(pEntry)) {
		DoClipOp();
		*pResult = 0;

		return;
	}

	*pResult = 1;
}

bool CListBase::TestHit(CPoint point)
{
	CListCtrl& ListCtrl = GetListCtrl();
	// We must keep the old hilited item to "de-hilite" it!
	nOldDropItem = nDropItem;
	nDropItem = ListCtrl.HitTest(point);
	
	// Check if the item is valid (!= -1) and if it is a directory and if
	// it's not the same item as before.
	if ((nDropItem != -1) && IsDirectory(nDropItem) && nDropItem != nOldDropItem) {
		// Drophilite the directory-item.
		ListCtrl.SetItemState(nOldDropItem, ~LVIS_DROPHILITED, LVIF_STATE);
		ListCtrl.SetItemState(nDropItem, LVIS_DROPHILITED, LVIF_STATE);
	}
	else if ((nOldDropItem != -1) && (nOldDropItem != nDropItem)) {
		// Remove the hilited state from the old hilited item.
		ListCtrl.SetItemState(nOldDropItem, ~LVIS_DROPHILITED, LVIF_STATE);
	}

	if (!bDragFilerIntern) {
		ListCtrl.UpdateWindow();
		return true;
	}

	bool ret = true;
	if (nDropItem != -1) {
		CFilerEntry * pDropDir = GetEntry(nDropItem);
		// Make sure it is a dir
		if (!pDropDir->Directory())
			pDropDir = pDropDir->Parent();

		if (bDragFilerIntern) {
			// It is not allowed to drop in the same directory a the drag started
			ret = (pDragFromDir != pDropDir);
		}
	}
	else {
		ret = (pDragFromDir != pFiler->currentDir());
	}

	ListCtrl.UpdateWindow ();
	return ret;

}
#ifdef _DEBUG
CHPCommDoc* CListBase::GetDocument() // non-debug version is inline
{
	VERIFY(m_pDocument->IsKindOf(RUNTIME_CLASS(CHPCommDoc)));
	return (CHPCommDoc*)m_pDocument;
}
#endif //_DEBUG

void CListBase::ChangeDir(CString *psDir)
{

}

void CListBase::OnDelete()
{
	if (!pFiler->isAvailable())
		return;

	CListCtrl& ListCtrl = GetListCtrl();

	// If the user is editing a filename and hits delete we unfortunately
	// end up here :-(
	// If pEdit != NULL we know that the user is editing.
	if (pEdit != NULL) {
		// As GetEditControl only returns a CTempWnd we make sure we
		// have the correct CWnd *.
		pEdit = ListCtrl.GetEditControl();
		pEdit->SendMessage(WM_KEYDOWN, VK_DELETE, 0);
		return;
	}
    CWaitCursor wait;
	unsigned int nCount = ListCtrl.GetSelectedCount();
	if (nCount == 0)
		return;

	// Check if a directory is deleted.
	bool bDir = false;

	CFilerEntryList list;
	int nItem = -1;
	for (unsigned int i = 0; i < nCount; i++) {
		nItem = ListCtrl.GetNextItem(nItem, LVNI_SELECTED);
		CFilerEntry * fe = GetEntry(nItem);
		bDir |= fe->Directory();
		list.AddTail(fe);		
	}

	try {
		pFiler->rm(&list);
	}
	catch (CProtException *pEx) {
		ExceptionHandler(pEx);
	}

	UpdateAllViews(bDir);

}

void CListBase::OnInitialUpdate() 
{
	ModifyStyle(WS_VISIBLE, 0);

	CListCtrl& ListCtrl = GetListCtrl();

	nItem = 0;
	
	int nSubItem = 0;

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 200;
	lvc.pszText = _T("Name");
	ListCtrl.InsertColumn(0, &lvc);
	lvc.fmt = LVCFMT_RIGHT;
	lvc.iSubItem = nSubItem++;
	lvc.cx = 100;
	lvc.pszText = _T("Size");
	ListCtrl.InsertColumn(1, &lvc);
	if (pFiler->hasDateTime()) {
		lvc.fmt = LVCFMT_RIGHT;
		lvc.iSubItem = nSubItem++;
		lvc.cx = 100;
		lvc.pszText = _T("Date/Time");
		ListCtrl.InsertColumn(2, &lvc);
	}
	if (pFiler->hasCRC()) {
		lvc.fmt = LVCFMT_RIGHT;
		lvc.iSubItem = nSubItem++;
		lvc.cx = 100;
		lvc.pszText = _T("Checksum");
		ListCtrl.InsertColumn(3, &lvc);
		
	}
	
	bool bRes = UpdateContent();

	((CMainFrame *)AfxGetMainWnd())->BumpComInfoDlg(pFiler, READING, bRes);

}

void CListBase::AddItem(CFilerEntry *pEntry)
{
	CListCtrl& ListCtrl = GetListCtrl();
	// We insert the item and get back the index of it.
	LV_ITEM item;
	item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	item.iImage = I_IMAGECALLBACK;
	item.pszText = (LPTSTR)(LPCTSTR)pEntry->Name();
	item.lParam = (DWORD)pEntry;
	item.iItem = nItem++;
	item.iSubItem = 0;
	int iItem = ListCtrl.InsertItem(&item);

	// Add Date
	item.mask = LVIF_TEXT;
	item.iItem = iItem;
	item.iSubItem = 2;
	if (pFiler->hasDateTime() && pEntry->Name() != "") {
		SYSTEMTIME st;
		char sTime[15];
		char sDate[15];
		char sDateTime[31];
		sDateTime[0] = '\0';

		if (pFiler->getDateTime(pEntry, st)) {
			// Or use LOCALE_USER_DEFAULT???
			GetDateFormat(LOCALE_SYSTEM_DEFAULT, DATE_SHORTDATE, &st, NULL, sDate, 15);
			GetTimeFormat(LOCALE_SYSTEM_DEFAULT, TIME_NOSECONDS, &st, NULL, sTime, 15);
			strcpy(sDateTime, sDate);
			strcat(sDateTime, " ");
			strcat(sDateTime, sTime);
		}
		item.pszText = sDateTime;
		ListCtrl.SetItem(&item);
	}
	else if (pFiler->hasCRC()) {
		unsigned int nCRC;
		pFiler->getCRC(pEntry, nCRC);
		CString sCRC;
		sCRC.Format("%u", nCRC);
		item.pszText = (LPTSTR)(LPCTSTR)sCRC;
		ListCtrl.SetItem(&item);
	}

	// Only show the size if the size is set.  This also means that
	// the size of a directory will never be shown.
	if (pEntry->Size() != CFilerEntry::NA) {
		char buf[20];
		sprintf(buf, "%ld", pEntry->Size());
		item.mask = LVIF_TEXT;
		item.iItem = iItem;
		item.iSubItem = 1;
		item.pszText = buf;
		ListCtrl.SetItem(&item);
	}

}


int CListBase::CmpItems(CString cstL, CString cstR, int nCol)
{
	if (nCol == 0) {
		return cstL.CompareNoCase(cstR);
	}
	// FIXME: Replace the call to isLocal() when we work in the
	// CFiler capability stuff in HPComm 3.1. (mjd)
	else if ((nCol == 1) || ((nCol == 2) && !pFiler->isLocal())) {
		if (cstR == cstL)
			return 0;
		else if (atol((LPCTSTR)cstL) < atol((LPCTSTR)cstR))
			return -1;
		else
			return 1;
	}
	else if (nCol == 2) {
		COleDateTime odtL;
		COleDateTime odtR;
		odtL.ParseDateTime(cstL);
		odtR.ParseDateTime(cstR);
		COleDateTimeSpan spanElapsed = odtL - odtR;
		if (spanElapsed.GetStatus() == COleDateTimeSpan::invalid) {
			return 0;
		}
		return (int)spanElapsed.GetTotalSeconds();

	}
	return 0;

}

void CListBase::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVDISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if (!pFiler->isAvailable()) {
		*pResult = 1;
		return;
	}
	pEdit = GetListCtrl().GetEditControl();
	if (pEdit) {
		pEdit->GetWindowText(sOldItemName);
		pEdit->LimitText(_MAX_FNAME);
		*pResult = 0;
	}
	else {
		*pResult = 1;
	}
}

void CListBase::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CWaitCursor wait;
	*pResult = 0;
	if (!pFiler->isAvailable()) {
		pFiler->invalidate(pFiler->root());

		try {
			pFiler->reconnect();
		}
		catch (CProtException * pEx) {
			ExceptionHandler(pEx);
		}

		UpdateAllViews(true);
		return;
	}
	CListCtrl& ListCtrl = GetListCtrl();
	NMLISTVIEW* pNMListView = (NM_LISTVIEW *)pNMHDR;
	if (ListCtrl.HitTest(pNMListView->ptAction) == -1) {
		// The doubleclick was not on an item. Nothing to do!
		return;
	}
	// Get the entry from the data field.
	CFilerEntry * pEntry = GetEntry(pNMListView->iItem);
	//pFiler->fromString(pFiler->toString(pEntry));
	if (pEntry->Directory()) {
		// If it is a directory we change the current dir and update the views.
		pFiler->currentDir(pEntry);
		// Update also the associated treeview.
		UpdateAllViews(true);
	}
	else if (pFiler->isLocal()) {
		// First we check if it's a BMP-file to display it's content.
		CString sFileName = pEntry->Name();
		if ((sFileName.Right(4) == ".bmp") || (sFileName.Right(4) == ".BMP") || (sFileName.Right(4) == ".grb")|| (sFileName.Right(4) == ".gro")) {
			CGrobDlg dlg(this);
			dlg.nScreenDlg = 0;
			dlg.sFileName = pFiler->toString(pEntry);
			dlg.DoModal();
			dlg.sFileName = dlg.sFileName.Left(dlg.sFileName.ReverseFind('\\'));
			CFilerEntry * fe = pFiler->fromString(dlg.sFileName);
			if (pFiler->isValid(fe)) {
				pFiler->reRead(fe);
				UpdateAllViews(true);
			}
        } else if (((sFileName.Right(6) == ".flash") || (sFileName.Right(6) == ".FLASH")) &&
            pFiler->hasFlash()) {
            // Ok, they have a flash file, so close the port first...
            ((CMainFrame *)AfxGetMainWnd())->Disconnect();
            // Now pass the flash file to the flash program.
            pFiler->updateFlash(pFiler->toString(pEntry));
		} else if (sFileName.Right(9) == ".hpbackup") {
            ((CMainFrame *)AfxGetMainWnd())->OnCalculatorRestore();	
		}
		else {
			// Open the file.
			ShellExecute(m_hWnd, "open", pFiler->toString(pEntry), NULL, NULL, SW_SHOWNORMAL);
		}
	}
	
	*pResult = 0;
}

void CListBase::OnDirpopupExplore() 
{
	// We just open the new dir.
	pFiler->currentDir(GetEntry(nPopItem));
	UpdateAllViews(true);
}

void CListBase::OnOpeninexplorer() 
{
	CListCtrl& ListCtrl = GetListCtrl();
	POSITION pos = ListCtrl.GetFirstSelectedItemPosition();
	if (pos) {
		int nItem = ListCtrl.GetNextSelectedItem(pos);
		ShellExecute(m_hWnd, "open", pFiler->toString(GetEntry(nItem)), NULL, NULL, SW_SHOWNORMAL);
	}
}

void CListBase::OnDirpopupSend() 
{
	// TODO: Add your command handler code here
	
}

void CListBase::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVDISPINFO* pDispInfo = (NMLVDISPINFO*)pNMHDR;
	*pResult = 0;
	if (bNewFolder) {
		CWaitCursor wait;

		CString sName;
		// Check if the values in pDispInfo->item are valid
		// If not we have to get the item name from the list itself.
		if (pDispInfo->item.mask == 0) {
			sName = GetListCtrl().GetItemText(GetListCtrl().GetItemCount() - 1, 0); 
		}
		else {
			sName = pDispInfo->item.pszText;
		}
		// The user created a new folder.
		// Now after he entered the desired name, we create
		// the dir.
		CFilerEntry *currentDir = pFiler->currentDir();
		if (pFiler->newFolder(currentDir, sName)) {
			// The newFolder could be created.

			// Update the contents of the views.
			UpdateAllViews(true);
			pEdit = NULL;
			bNewFolder = false;

			return;
		}

		// The new folder could not be created.
		MessageBox("Could not create the new directory.\nPlease check if the name you entered is valid.", "Error", MB_OK | MB_ICONEXCLAMATION);

		// If the name was not valid, keep renaming.....
		OnRename();

		return;
	}
	*pResult = 0;

	if (pEdit) {
		CString sName;
		pEdit = GetListCtrl().GetEditControl();
		pEdit->GetWindowText(sName);
		if (sOldItemName != sName) {
			// Only do some work if the names are different.
			CFilerEntry *fe = (CFilerEntry *)pDispInfo->item.lParam;
			if (pDispInfo->item.pszText != 0) {
				if (!Rename(fe, pDispInfo->item.pszText) && pEdit != NULL) {
					// The entry could not be renamed.
					AfxMessageBox("The file could not be renamed!\nMake sure the name is unique and valid.", MB_OK | MB_ICONSTOP);
					pEdit->SetWindowText(sOldItemName);
				}
			}
		}
	}

	pEdit = NULL;
}

void CListBase::OnHeaderclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (!pFiler->isAvailable()) {
		*pResult = 1;
		return;
	}
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	
	nSortedCol = phdn->iItem;
	bSortAscending = !bSortAscending;
	Sort();
	
	*pResult = 0;
}

void CListBase::OnPopupDelete() 
{
	// TODO: Add your command handler code here
	
}

void CListBase::OnPopupOpen() 
{
	ShellExecute(m_hWnd, "open", pFiler->toString(GetEntry(nPopItem)), NULL, NULL, SW_SHOWNORMAL);
	
}

void CListBase::OnPopupOpenwithnotepad() 
{
	ShellExecute(m_hWnd, "open", "notepad.exe", pFiler->toString(GetEntry(nPopItem)), NULL, SW_SHOWNORMAL);
	
}

void CListBase::OnProperties() 
{
	CListCtrl& ListCtrl = GetListCtrl();
	POSITION pos = ListCtrl.GetFirstSelectedItemPosition();
	int nItem = ListCtrl.GetNextSelectedItem(pos);

	if (nItem == -1)
		return;
	
	CString sFileName = pFiler->toString(GetEntry(nItem));
	POINT point;
	point.x = 20;
	point.y = 20;
	ShowExplorerMenu(sFileName, point, true);
}

void CListBase::OnPopupSendKermit() 
{
	// TODO: Add your command handler code here
	
}

void CListBase::OnPopupSendXmodem() 
{
	// TODO: Add your command handler code here
	
}

void CListBase::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
    if (pFiler->isAvailable() == false)
        return;

	NMLISTVIEW* pNMListView = (NM_LISTVIEW *)pNMHDR;
	CListCtrl& ListCtrl = GetListCtrl();
	int nItem = ListCtrl.HitTest(pNMListView->ptAction);
	CString sName;
	if (nItem != -1) {
		sName = pFiler->toString(GetEntry(nItem));
	}
	else {
		sName = "DummyName";
	}
	
	if (sName.Right(1) == ":")
		sName += "\\";

	LPCTSTR sText = (LPCTSTR)sName;
	if (pFiler->isLocal()) {
		if (ShowExplorerMenu(sText, pNMListView->ptAction)) {
			// As we don't know what kind of operation the user executed on the
			// item (i.e. a delete command) we reRead the current directory.
			pFiler->reRead(pFiler->currentDir());
			UpdateAllViews(true);
			*pResult = 0;
		}
		else {
			*pResult = 1;
		}
	}
	else {
		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_CALCPOPUP));
		CMenu * pPopupMenu = menu.GetSubMenu(0);
		UINT cb_format;
		BOOL bInternal;
		if (!((CMainFrame* )AfxGetMainWnd())->CheckClipboard(cb_format, bInternal)) {
			// We don't have valid clipboard data, so we
			// gray out the paste command.
			pPopupMenu->EnableMenuItem(ID_EDIT_PASTE, MF_BYCOMMAND | MF_GRAYED);
		}
		if (nItem == -1) {
			pPopupMenu->EnableMenuItem(ID_EDIT_COPY, MF_BYCOMMAND | MF_GRAYED);
			pPopupMenu->EnableMenuItem(ID_DELETE, MF_BYCOMMAND | MF_GRAYED);
		}
		ASSERT(pPopupMenu != NULL);
		CWnd * pPopupOwner = this;
		POINT pt = pNMListView->ptAction;
		ClientToScreen(&pt);
		pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, pPopupOwner);
		
		*pResult = 0;
	}
	return;
}

BOOL CListBase::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_EDITLABELS;
	return CListView::PreCreateWindow(cs);
}

BOOL CListBase::ReadDir()
{
	CWaitCursor wait;
	SetRedraw(FALSE);
	CListCtrl& ListCtrl = GetListCtrl();
	// First delete all items!!!
	ListCtrl.DeleteAllItems();
	
	// Get the first entry

	CFilerEntry * fe = pFiler->currentDir();
	try {
		if (!pFiler->findFile(fe)) {
			SetRedraw();
			return FALSE;
		}
	}
	catch (CProtException * pEx) {
		ExceptionHandler(pEx);	
	}

	// While there are entries, we add them to the listview.
    while((fe = pFiler->findFileNext()) != NULL)
    {
        // First we add all directories
        if (fe->Directory()) {
			AddItem(fe);
		}
    }

	try {
		VERIFY(pFiler->findFile(pFiler->currentDir()));
	}
	catch (CProtException * pEx) {
		ExceptionHandler(pEx);	
	}
	

	// Now we add all normal files.
	while((fe = pFiler->findFileNext()) != NULL)
	{
        if (!fe->Directory()) {
			AddItem(fe);
		}
    }

	Sort();

	SetRedraw();

	return TRUE;

}

BOOL CListBase::Rename(CFilerEntry * pEntry, char *psNew)
{
	// Show hourglass
	CWaitCursor wait;
	BOOL bRet;
	try {
		bRet = pFiler->rename(pEntry, psNew);
	}
	catch (CProtException * pEx) {
		ExceptionHandler(pEx);
	}

	// Update the tree only if a dir was renamed.
	UpdateAllViews(pEntry->Directory());	
	return bRet;

}

void CListBase::Sort()
{
	CListCtrl& ListCtrl = GetListCtrl();
	int x = 0;
	//Determine how many subdirectories are shown
	for(x; (x < ListCtrl.GetItemCount()) && (IsDirectory(x)); x++);
	if (ListCtrl.GetItemText(0, 0) == "..") {
		// Do not sort directories if the size header was clicked
		if (nSortedCol != 1)
			SortItems(nSortedCol, bSortAscending, 1, x - 1);
		
		// Sort files
		SortItems(nSortedCol, bSortAscending, x);
	}
	else {
		if (nSortedCol != 1)
			SortItems(nSortedCol, bSortAscending, 0, x - 1);
		
		// Sort files
		SortItems(nSortedCol, bSortAscending, x);
	
	}

}

BOOL CListBase::SortItems(int nCol, BOOL bAscending, int low, int high)
{
	CListCtrl& ListCtrl = GetListCtrl();

	if( nCol >= ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount())
		return FALSE;
	if( high == -1 )
		high = ListCtrl.GetItemCount() - 1;
	int lo = low;
	int hi = high;
	CString midItem;
	if( hi <= lo )
		return FALSE;
	midItem = ListCtrl.GetItemText( (lo+hi)/2, nCol );
	// loop through the list until indices cross
	while( lo <= hi )	{
 		// rowText will hold all column text for one row 
		CStringArray rowText;
 		// find the first element that is greater than or equal to
 		// the partition element starting from the left Index.
		if( bAscending ) {
  			//while( ( lo < high ) && (atoi(GetItemText(lo, nCol)) < midItem ) )
			while( ( lo < high ) && ( CmpItems( ListCtrl.GetItemText(lo, nCol) , midItem, nCol) < 0 ) )
   				++lo;
		}
		else {
  			//while( ( lo < high ) && (atoi(GetItemText(lo, nCol)) > midItem ) )
			while( ( lo < high ) && ( CmpItems( ListCtrl.GetItemText(lo, nCol) , midItem, nCol) > 0 ) )
   				++lo;
		}
		// find an element that is smaller than or equal to
 		// the partition element starting from the right Index.
		if( bAscending ) {
  			//while( ( hi > low ) && (atoi(GetItemText(hi, nCol)) > midItem ) )
			while( ( hi > low ) && ( CmpItems(ListCtrl.GetItemText(hi, nCol) , midItem, nCol ) > 0 ) )
   				--hi;
		}
		else {
  			//while( ( hi > low ) && (atoi(GetItemText(hi, nCol)) < midItem ) )
			while( ( hi > low ) && ( CmpItems(ListCtrl.GetItemText(hi, nCol) , midItem, nCol ) < 0 ) )
   				--hi;
		}
		// if the indexes have not crossed, swap
 		// and if the items are not equal
		if( lo <= hi ) {
			// swap only if the items are not equal
			//if(atoi(GetItemText(lo, nCol)) != atoi(GetItemText(hi, nCol)) )			/*!!!*
			if( CmpItems (ListCtrl.GetItemText(lo, nCol) , ListCtrl.GetItemText(hi, nCol), nCol  ) != 0) {
	   			// swap the rows
				LV_ITEM lvitemlo, lvitemhi;
	   			int nColCount = ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
	   			rowText.SetSize( nColCount );
				int i;
	   			for( i=0; i < nColCount; i++)
					rowText[i] = ListCtrl.GetItemText(lo, i);
	   			lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	   			lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;
	   			lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED |
	      					LVIS_FOCUSED |  LVIS_SELECTED |
	      					LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;
				lvitemhi = lvitemlo;
	   			lvitemhi.iItem = hi;
				ListCtrl.GetItem( &lvitemlo );
	   			ListCtrl.GetItem( &lvitemhi );
				for( i=0; i< nColCount; i++)
	      			ListCtrl.SetItemText(lo, i, ListCtrl.GetItemText(hi, i) );
				lvitemhi.iItem = lo;
	   			ListCtrl.SetItem( &lvitemhi );
				for( i=0; i< nColCount; i++)
	      			ListCtrl.SetItemText(hi, i, rowText[i]);
				lvitemlo.iItem = hi;
	   			ListCtrl.SetItem( &lvitemlo );
			}	  		
			
			++lo;
			--hi;
		}
	}
	// If the right index has not reached the left side of array
	// must now sort the left partition.
	if( low < hi )
 		SortItems( nCol, bAscending , low, hi);
	// If the left index has not reached the right side of array
	// must now sort the right partition.
	if( lo < high )
 		SortItems( nCol, bAscending , lo, high );
	return TRUE;

}

BOOL CListBase::IsDirectory(UINT nItem)
{
	return (GetEntry(nItem))->Directory();
}

CFilerEntry * CListBase::GetEntry(UINT nItem)
{
	return ((CFilerEntry *)GetListCtrl().GetItemData(nItem));
}

void CListBase::ShowNotConnected()
{
	// Check if it already shows "Not connected"
	CListCtrl &ListCtrl = GetListCtrl();
	bool bUpdate = false;
	if (ListCtrl.GetItemCount() == 0) {
		bUpdate = true;
	}
	else if (ListCtrl.GetItemData(0) != 0) {
		bUpdate = true;
	}
	
	if (bUpdate) {
		// No, there is some other thing shown
        CCalcTypes *ct = CCalcTypes::instance();
		ListCtrl.DeleteAllItems();
		ListCtrl.InsertItem(0, "Doubleclick to try to reconnect", ct->icon("Note"));
		ListCtrl.SetItemData(0, 0);
	}
}

void CListBase::OnRename() 
{
	CListCtrl &ListCtrl = GetListCtrl();
	POSITION pos = ListCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL) {
		ListCtrl.EditLabel(ListCtrl.GetNextSelectedItem(pos));
	}
}

void CListBase::OnDragLeave() 
{
	// Remove the hilited state of the item we dropped on.
	GetListCtrl().SetItemState(nOldDropItem, ~LVIS_DROPHILITED, LVIF_STATE);

	CListView::OnDragLeave();
}

DROPEFFECT CListBase::OnDragScroll(DWORD dwKeyState, CPoint point) 
{
	RECT rect;
	GetClientRect(&rect);
	
	// The whole point of nScrollWait is that after a scroll up/down
	// we sleep a little bit so that the scrolling is not too fast.

	// This boolean is use to determine if the scroll direction changed.
	static bool bScrollUp = true;

	// Check if the point is in the upper or lower area of the window.
	// If yes, scroll up or down.
	if (point.y > rect.top && point.y < rect.top + ScrollRange) {
		
		SendMessage(WM_VSCROLL, SB_LINEUP, 0L);
		
		// If the direction of the scrolling changed, reset the waiting amount.
		if (!bScrollUp) {
			nScrollWait = InitialScrollWait;
			bScrollUp = true;
		}
		else {
			// Decrease the waiting time every time a line was scrolled.
			// This way the scrolling becomes faster the more you scroll
			// Cool, eh :-)
			nScrollWait -= nScrollWait / 10;
		}
		Sleep(nScrollWait);
	}
	else if (point.y < rect.bottom && point.y > rect.bottom - ScrollRange) {
		SendMessage(WM_VSCROLL, SB_LINEDOWN, 0L);

		if (bScrollUp) {
			nScrollWait = InitialScrollWait;
			bScrollUp = false;
		}
		else {
			nScrollWait -= nScrollWait / 10;
		}
		Sleep(nScrollWait);
	}
	return CListView::OnDragScroll(dwKeyState, point);
}

DROPEFFECT CListBase::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	if (!pFiler->isAvailable())
		return DROPEFFECT_NONE;

	nScrollWait = InitialScrollWait;

	char sName[50];
	bInternalCF = false;
	pDataObject->BeginEnumFormats();
	FORMATETC format;
	// Enum all available clipbooard formats.
	while (pDataObject->GetNextFormat(&format)) {
		int n = GetClipboardFormatName(format.cfFormat, sName, 50);
		if (strcmp(sName, HPCommClipboardFormat) == 0) {
			// We found our own internal clipboard format.
			cb_format = format.cfFormat;
			bInternalCF = true;
			break;
		}
	}
	// We found an internal clipboard format
	if (bInternalCF) {
		HGLOBAL hData = pDataObject->GetGlobalData(cb_format);
		clipInfo_ * pClipInfo = (clipInfo_ *)GlobalLock(hData);
		GlobalUnlock(hData);
		// Check if the dragging started within the same filer.
		bDragFilerIntern = (pClipInfo->pFromFiler == pFiler);
		// Determine the directory the drag started from.
		pDragFromDir = pClipInfo->pFromFirstEntry;
		if (!pClipInfo->pFromFirstEntry->Directory())
			pDragFromDir = pDragFromDir->Parent();
		
	}
	else {
		bDragFilerIntern = false;
		return DROPEFFECT_NONE;
	}

	return OnDragOver(pDataObject, dwKeyState, point);
}

int CListBase::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	oleDropTarget.Register(this);

	return 0;
}

DROPEFFECT CListBase::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	if (!pFiler->isAvailable())
		return DROPEFFECT_NONE;

	DROPEFFECT dropeffect = DROPEFFECT_NONE;
	// If the shift key is pressed, we perform a move operation.
	if ((dwKeyState == MK_SHIFT) || (dwKeyState == (MK_SHIFT | MK_LBUTTON))) {
		dropeffect = DROPEFFECT_MOVE;
	}
	else {
		dropeffect = DROPEFFECT_COPY;
	}
	
	// Drophilite a dir if we drag over.
	if (!TestHit(point))
		dropeffect = DROPEFFECT_NONE;

	return dropeffect;
	
	//return CListView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL CListBase::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	if (DROPEFFECT_NONE)
		return FALSE;

	CWaitCursor wait;

	CFilerEntry * pDropTarget;
	int nItem = GetListCtrl().HitTest(point);

	// We determine if the drop-point is over a directory.
	if (nItem != -1 && GetEntry(nItem)->Directory()) {
		// Yes, it is. So we set the droptarget to the entry where the drop occured.
		pDropTarget = GetEntry(nItem);
	}
	else {
		// We are not over a dir, so we choose the current dir as the droptarget.
		pDropTarget = pFiler->currentDir();
	}

	// Depending on the clipboard format we process the drop action.
	if (bInternalCF) {
		// The drag and drop operation started in HPComm
		HGLOBAL hData = pDataObject->GetGlobalData(cb_format);
		clipInfo_ * pClipInfo = (clipInfo_ *)GlobalLock(hData);
		GlobalUnlock(hData);

        // FIXME: What about DROPEFFECT_NONE, DROPEFFECT_LINK, or DROPEFFECT_SCROLL??
        bool removeAfterCopy = (dropEffect == DROPEFFECT_MOVE);

		// Convert the CFilerEntry* array back into a list.
		CFilerEntryList fromList;
		bool bMsg = true;
		for (CFilerEntry **fe = &(pClipInfo->pFromFirstEntry); *fe != 0; fe++) {
			if (!pFiler->isLocal() && !bDragFilerIntern && (*fe)->Directory()) {
				// The user tried to drag a dir to the calc which is not supported yet.
				if (bMsg) {
					MessageBox("The dragging of a PC directory to the calc\nis not supported yet!", "Info", MB_OK|MB_ICONEXCLAMATION);
					// Show the message only once.
					bMsg = false;
				}
			}
			else {
				fromList.AddTail(*fe);
			}
		}

		// "Finish" the drop operation
		CListView::OnDrop(pDataObject, dropEffect, point);

		// Now call the filer to perform the operation.
        try {
			pFiler->cp(removeAfterCopy, pClipInfo->pFromFiler, 
					   &fromList, pDropTarget);
		}
		catch (CProtException * pEx) {
			ExceptionHandler(pEx);
		}


		// Always update also the tree, because the PC can send a file which is
		// a dir for the calc.
		UpdateAllViews(true);
	}
	else {
		// The drag and drop operation started from elsewhere, e.g. in the explorer.
		HDROP hDrop = (HDROP)pDataObject->GetGlobalData(CF_HDROP);
		
		CFiler * fromFiler = 0;
		((CMainFrame *)AfxGetMainWnd())->PrepareExtDrop(hDrop, fromFiler);

        bool removeAfterCopy = (dropEffect == DROPEFFECT_MOVE);
		// "Finish" the drop operation
        CListView::OnDrop(pDataObject, dropEffect, point);

		try {
			pFiler->cp(removeAfterCopy, hDrop, fromFiler, pDropTarget);
		}
		catch (CProtException * pEx) {
			ExceptionHandler(pEx);
		}

		// Pretty messy to determine here if we had a dir dropped.
		// Just update also the tree.
		UpdateAllViews(true);
	}

	// Remove the hilited state of the item we dropped on.
	GetListCtrl().SetItemState(nItem, ~LVIS_DROPHILITED, LVIF_STATE);

	//return CListView::OnDrop(pDataObject, dropEffect, point);
	return TRUE;
}

void CListBase::OnEditCopy() 
{
	// Do a file op on the clipboard (TRUE)
	DoClipOp(TRUE);
}

void CListBase::OnEditCut() 
{
	// Do a file op on the clipboard (TRUE)
	DoClipOp(TRUE);
}

void CListBase::OnEditPaste() 
{
	BOOL bInternal = FALSE;
	UINT cb_format;
	
	if (!((CMainFrame* )AfxGetMainWnd())->CheckClipboard(cb_format, bInternal)) {
		return;
	}
	// Open the clipboard to access the data.
	if (!OpenClipboard()) {
		((CHPCommApp *)AfxGetApp())->LastErrorMessage(GetLastError());
		return;
	}

	HGLOBAL hClipData = GetClipboardData(cb_format);
	clipInfo_ * pClipInfo = (clipInfo_ *)GlobalLock(hClipData);
	GlobalUnlock(hClipData);
	if (bInternal) {
		CFilerEntryList fromList;
		for (CFilerEntry **fe = &(pClipInfo->pFromFirstEntry); *fe != 0; fe++) {
			fromList.AddTail(*fe);
		}

		// Now call the filer to perform the operation.
		try {
			pFiler->cp(false, pClipInfo->pFromFiler, &fromList, pFiler->currentDir());
		}
		catch (CProtException * pEx) {
			ExceptionHandler(pEx);
		}

	}
	else {
		// The drag and drop operation started from elsewhere, e.g. in the explorer.
		HDROP hDrop = (HDROP)hClipData;
		
		CFiler * fromFiler = 0;
		if (!((CMainFrame *)AfxGetMainWnd())->PrepareExtDrop(hDrop, fromFiler)) {
			CloseClipboard();
			return;
		}
		// Check if we have a valid nPopItem, which means, the user pastes it in 
		// a dir.
		CFilerEntry * pDropTarget = pFiler->currentDir();
		if (nPopItem >= 0 && nPopItem < GetListCtrl().GetItemCount()) {
			if (IsDirectory(nPopItem)) {
				pDropTarget = GetEntry(nPopItem);
			}
		}

		if (!pFiler->isValid(pDropTarget))
			pDropTarget = pFiler->currentDir();
		
		try {
			pFiler->cp(false, hDrop, fromFiler, pDropTarget);
		}
		catch (CProtException * pEx) {
			ExceptionHandler(pEx);
		}

	}

	CloseClipboard();
	UpdateAllViews(true);
}

void CListBase::OnEditUndo() 
{
	
}

bool CListBase::DoClipOp(bool bClipboard /* = FALSE */, bool bCut /* = FALSE*/)
{
	CListCtrl &ListCtrl = GetListCtrl();

	int nCount = ListCtrl.GetSelectedCount();

	CFilerEntryList fromList;

	// The size of pFrom.
	int nSize = 0;

	char *pFrom = new char[nCount * (_MAX_PATH + 1) + 1];
	// Set it to 0
	memset(pFrom, '\0', nCount * (_MAX_PATH + 1) + 1);
	char *cursor = pFrom;

	POSITION pos;
	pos = ListCtrl.GetFirstSelectedItemPosition();
	
	if (pos == NULL) {
		// No items were selected.
		return false;
	}

	// Iterate through the ListView with the entries that will be dragged
	while (pos) {
		int nItem = ListCtrl.GetNextSelectedItem(pos);
		fromList.AddHead(GetEntry(nItem));

		// Get the filename of the CFileEntry
		CString sPath = pFiler->toString(GetEntry(nItem));
		nSize += sPath.GetLength() + 1;
		char elem[_MAX_PATH];
		strcpy(elem, sPath);
		// Copy the filename to pFrom
		strcpy(cursor, elem);
		cursor += strlen(elem) + 1;
	}

	nSize++; // The ending zero char

	if (bClipboard) {
		if (!OpenClipboard()) {
			((CHPCommApp *)AfxGetApp())->LastErrorMessage(GetLastError());
			return false;
		}
		if (!EmptyClipboard()) {
			((CHPCommApp *)AfxGetApp())->LastErrorMessage(GetLastError());	
			return false;
		}	
	}

	

	// Create the data-source which holds the information about which files are dragged.
	// Allocate memory for the data
	HGLOBAL hDropFiles = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(DROPFILES) + nSize);
	// Get the pointer of the allocated memory
	DROPFILES* pDropData = (DROPFILES*)GlobalLock(hDropFiles);
	pDropData->pFiles = sizeof(DROPFILES);

	// Copy the filenames to the memory
	memcpy((((char*)(pDropData)) + sizeof(DROPFILES)), pFrom, nSize);

	// Unlock the memory
	GlobalUnlock(hDropFiles);

	// Now build the clipInfo struct.
  	unsigned int nClipInfoSize = sizeof(clipInfo_) + 
	                             (nCount + 1) * sizeof(CFilerEntry*);
	clipInfo_ * pClipInfo = (clipInfo_ *)new char[nClipInfoSize];
	pClipInfo->pFromFiler = pFiler;

	CFilerEntry **pfe = &(pClipInfo->pFromFirstEntry);
	pos = fromList.GetHeadPosition();
	while (pos)
	{
		CFilerEntry *fe = fromList.GetNext(pos);
		*pfe++ = fe;
	}

	// Terminate the list.
	*pfe = 0;

    // Allocate global memory to hold the clipInfostruct.
	const int options = GMEM_SHARE | GMEM_MOVEABLE | GMEM_ZEROINIT;
	HGLOBAL hGlobalClipInfo = GlobalAlloc(options, nClipInfoSize);

	// Get the ptr to the global allocated memory.
	clipInfo_ * pGlobalClipInfo = (clipInfo_ *)GlobalLock(hGlobalClipInfo);

	// Copy the list to the global memory.
	memcpy(pGlobalClipInfo, pClipInfo, nClipInfoSize);

	GlobalUnlock(hGlobalClipInfo);

	// We register a new clipboard format to transfer the file as a CFilerList.
	UINT cb_format = RegisterClipboardFormat(HPCommClipboardFormat);

	DROPEFFECT dropEffect = DROPEFFECT_NONE;

	if (bClipboard) {
		// "Put the data to the clipboard" (if you know what I mean)
		// FIXME: This is being used after the GlobalUnlock.  Is this alright?
		SetClipboardData(cb_format, hGlobalClipInfo);
		SetClipboardData(CF_HDROP, hDropFiles);
		CloseClipboard();
	}
	else {
		COleDataSource * pDataSource = new COleDataSource;

		// "Put the data to the clipboard" (if you know what I mean)
		// FIXME: This is being used after the GlobalUnlock.  Is this alright?
		pDataSource->CacheGlobalData(cb_format, hGlobalClipInfo);
		pDataSource->CacheGlobalData(CF_HDROP, hDropFiles);

		// Start dragging. This function only returns if the drag-operation is completed.
		dropEffect = pDataSource->DoDragDrop(DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_SCROLL, NULL);
		delete pDataSource;
	}

#if 0
	// FIXME: Do we need this?  If the files have been moved, the move will
	// have deleted the fromList files, and the removeAfterCopy arg will
	// ensure that the CFilerEntry*s have been reread.

	// If the file has been moved or cutted...
	if (dropEffect == DROPEFFECT_MOVE) {
		// ...delete the files.
		try {
			pFiler->rm(&fromList);
		}
		catch (CProtException * pEx) {
			ExceptionHandler(pEx);		
		}
	}
#endif

	delete pClipInfo;
	delete pFrom;
	return true;
}


bool CListBase::UpdateContent()
{
	nPopItem = -1;
	if (!pFiler->isAvailable()) {
		ShowNotConnected();
		return false;
	}
	else {
		return (ReadDir() == TRUE);
	}
}

void CListBase::UpdateAllViews(bool bTree)
{
	// If the tree also has to be updated, call UpdateAllView, otherwise
	// we simply call UpdateContent.
	if (bTree) {
		((CMainFrame *)AfxGetMainWnd())->UpdateAllViews(pFiler, bTree);
	}
	else {
		UpdateContent();
	}
}

CFilerEntry * CListBase::GetSelectedEntry()
{
	unsigned int nCount = GetListCtrl().GetSelectedCount();
	if (nCount == 0)
		return 0;

	return GetEntry(GetListCtrl().GetNextItem(-1, LVNI_SELECTED));

}

void CListBase::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	// If the listview control needs information about an item, this method is
	// called.
	if (pDispInfo->item.mask & LVIF_IMAGE) {
		int icon, selicon;
		pFiler->getIcons((CFilerEntry *)pDispInfo->item.lParam, icon, selicon);
		// Set the image.
		pDispInfo->item.iImage = icon;
	}
	*pResult = 0;
}

void CListBase::ConnectionFailedDlg()
{
	CDialog dlg(IDD_CONNECTFAILED);
	dlg.DoModal();
}

void CListBase::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
//	GetListCtrl().GetImageList(LVSIL_NORMAL)->Detach();
//	GetListCtrl().GetImageList(LVSIL_SMALL)->Detach();
	
	CListView::OnClose();
}

void CListBase::ExceptionHandler(CProtException *pEx)
{
	((CMainFrame *)AfxGetMainWnd())->ExceptionHandler(pEx);
}

void CListBase::OnSelectall() 
{
	CListCtrl& ListCtrl = GetListCtrl();
	UINT nCount = ListCtrl.GetItemCount();
	for (UINT i = 0; i < nCount; i++) {
		ListCtrl.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}
}

BOOL CListBase::ShowExplorerMenu(LPCTSTR pszPath, POINT point, bool bShowProperties /* = false */)
{

    LPMALLOC pMalloc;
    LPSHELLFOLDER psfFolder, psfNextFolder;
    LPITEMIDLIST pidlMain, pidlItem, pidlNextItem, *ppidl;
    LPCONTEXTMENU pContextMenu;
    CMINVOKECOMMANDINFO ici;
    ULONG ulCount, ulAttr;
    TCHAR tchPath[MAX_PATH];
    WCHAR wchPath[MAX_PATH];    
    UINT nCount, nCmd;
    BOOL bResult;
    HMENU hMenu;

	HWND hwnd = m_hWnd;

    //
    // Make sure the file name is fully qualified and in Unicode format.
    //
    GetFullPathName (pszPath, sizeof (tchPath) / sizeof (TCHAR), tchPath, NULL);

    if (IsTextUnicode (tchPath, lstrlen (tchPath), NULL))
        lstrcpy ((char *) wchPath, tchPath);
    else
        MultiByteToWideChar (CP_ACP, 0, pszPath, -1, wchPath,
                             sizeof (wchPath) / sizeof (WCHAR));

    //
    // Get pointers to the shell's IMalloc interface and the desktop's
    // IShellFolder interface.
    //
    bResult = FALSE;

    if (!SUCCEEDED (SHGetMalloc (&pMalloc)))
        return bResult;

    if (!SUCCEEDED (SHGetDesktopFolder (&psfFolder))) {
        pMalloc->Release();
        return bResult;
    }

    //
    // Convert the path name into a pointer to an item ID list (pidl).
    //

    if (SUCCEEDED (psfFolder->ParseDisplayName(hwnd, NULL, wchPath, &ulCount, &pidlMain, &ulAttr)) && (pidlMain != NULL)) {

        if (nCount = GetPCount (pidlMain)) { // nCount must be > 0
            //
            // Initialize psfFolder with a pointer to the IShellFolder
            // interface of the folder that contains the item whose context
            // menu we're after, and initialize pidlItem with a pointer to
            // the item's item ID. If nCount > 1, this requires us to walk
            // the list of item IDs stored in pidlMain and bind to each
            // subfolder referenced in the list.
            //
            pidlItem = pidlMain;

            while (--nCount) {
                //
                // Create a 1-item item ID list for the next item in pidlMain.
                //
				
                pidlNextItem = DuplicateItem (pMalloc, pidlItem);
                if (pidlNextItem == NULL) {
                    pMalloc->Free (pidlMain);
                    psfFolder->Release ();
                    pMalloc->Release();
                    return bResult;
                }

                //
                // Bind to the folder specified in the new item ID list.
                //
                if (!SUCCEEDED (psfFolder->BindToObject(pidlNextItem, NULL, IID_IShellFolder, (void **)&psfNextFolder))) {
                    pMalloc->Free(pidlNextItem);
                    pMalloc->Free(pidlMain);
                    psfFolder->Release();
                    pMalloc->Release();
                    return bResult;
                }

                //
                // Release the IShellFolder pointer to the parent folder
                // and set psfFolder equal to the IShellFolder pointer for
                // the current folder.
                //
                psfFolder->Release();
                psfFolder = psfNextFolder;

                //
                // Release the storage for the 1-item item ID list we created
                // just a moment ago and initialize pidlItem so that it points
                // to the next item in pidlMain.
                //
                pMalloc->Free(pidlNextItem);
                pidlItem = GetNextItem (pidlItem);
            }

            //
            // Get a pointer to the item's IContextMenu interface and call
            // IContextMenu::QueryContextMenu to initialize a context menu.
            //
            ppidl = &pidlItem;
            if (SUCCEEDED (psfFolder->GetUIObjectOf(hwnd, 1, (const struct _ITEMIDLIST **)ppidl, IID_IContextMenu, NULL, (LPVOID *)&pContextMenu))) {
                hMenu = CreatePopupMenu ();
                if (SUCCEEDED (pContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_EXPLORE))) {
					if (!bShowProperties) {
						ClientToScreen (&point);

						//
						// Display the context menu.
						//
						nCmd = TrackPopupMenu (hMenu, TPM_LEFTALIGN |
							TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD,
							point.x, point.y, 0, hwnd, NULL);
	                        ici.lpVerb          = MAKEINTRESOURCE (nCmd - 1);
					}
					else {
						ici.lpVerb = "properties";
						nCmd = 1;
					}
                    //
                    // If a command was selected from the menu, execute it.
                    //
                    if (nCmd) {
						char cmdname[1024];
						pContextMenu->GetCommandString(nCmd - 1, GCS_VERB, NULL, cmdname, 1024);
                        ici.cbSize          = sizeof (CMINVOKECOMMANDINFO);
                        ici.fMask           = 0;
                        ici.hwnd            = hwnd;
                        ici.lpParameters    = NULL;
                        ici.lpDirectory     = NULL;
                        ici.nShow           = SW_SHOWNORMAL;
                        ici.dwHotKey        = 0;
                        ici.hIcon           = NULL;

						// The problem till now is, that the command only act on
						// one selected file.
						// For now we check what kind of command the user has choosen
						// it we can executed it, let's do it.
						if (strcmp(cmdname, "delete") == 0) {
							OnDelete();
							bResult = TRUE;
						}
						else if (strcmp(cmdname, "copy") == 0) {
							bResult = DoClipOp(TRUE);
						}
						else {
							bResult = (SUCCEEDED (pContextMenu->InvokeCommand(&ici)));
						}
                    }
                }
                DestroyMenu (hMenu);
                pContextMenu->Release();
            }
        }
        pMalloc->Free(pidlMain);
    }

    //
    // Clean up and return.
    //
    psfFolder->Release();
    pMalloc->Release();

    return bResult;

}

LPITEMIDLIST CListBase::DuplicateItem(LPMALLOC pMalloc, LPITEMIDLIST pidl)
{

    USHORT nLen;
    LPITEMIDLIST pidlNew;

    nLen = pidl->mkid.cb;
    if (nLen == 0)
        return NULL;

    pidlNew = (LPITEMIDLIST) pMalloc->Alloc (nLen + sizeof (USHORT));
    if (pidlNew == NULL)
        return NULL;

    CopyMemory (pidlNew, pidl, nLen);
    *((USHORT*) (((LPBYTE) pidlNew) + nLen)) = 0;

    return pidlNew;

}

LPITEMIDLIST CListBase::GetNextItem(LPITEMIDLIST pidl)
{
    USHORT nLen;

    if ((nLen = pidl->mkid.cb) == 0)
        return NULL;
    
    return (LPITEMIDLIST) (((LPBYTE) pidl) + nLen);

}

UINT CListBase::GetPCount(LPITEMIDLIST pidl)
{
    USHORT nLen;
    UINT nCount;

    nCount = 0;
    while ((nLen = pidl->mkid.cb) != 0) {
        pidl = GetNextItem (pidl);
        nCount++;
    }
    return nCount;

}

void CListBase::newFolder()
{
	CString sName;

	CFilerEntry *parent = pFiler->currentDir();
	sName = pFiler->suggestFolderName(parent);
	int nIcon = pFiler->getDirIcon();

	CListCtrl& ListCtrl = GetListCtrl();
	int nCount = ListCtrl.GetItemCount();
	ListCtrl.InsertItem(nCount, sName, nIcon);
	bNewFolder = true;
	ListCtrl.EditLabel(nCount);
}

