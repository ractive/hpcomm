// TreeBase.cpp : implementation file
//

#include "stdafx.h"
#include "hpcomm.h"
#include "TreeBase.h"
#include "MainFrame.h"
#include "ProtException.h"
#include "Properties.h"
#include "CalcTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeBase

IMPLEMENT_DYNCREATE(CTreeBase, CTreeView)

CTreeBase::CTreeBase()
{
	m_pDragImage = NULL;
	bBrowse = false;
	pEdit = NULL;
	nScrollWait = InitialScrollWait;
}


CTreeBase::~CTreeBase()
{
	if (m_pDragImage)
		delete m_pDragImage;
}


BEGIN_MESSAGE_MAP(CTreeBase, CTreeView)
	//{{AFX_MSG_MAP(CTreeBase)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_TREEPOPUP_DELETE, OnTreepopupDelete)
	ON_COMMAND(ID_TREEPOPUP_EXPLORE, OnTreepopupExplore)
	ON_COMMAND(ID_TREEPOPUP_OPEN, OnTreepopupOpen)
	ON_COMMAND(ID_FILE_OPEN, OnOpeninexplorer)
	ON_COMMAND(ID_RENAME, OnRename)
	ON_WM_CREATE()
	ON_COMMAND(ID_EDIT_PASTE, OnPaste)
	ON_NOTIFY_REFLECT(TVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_COMMAND(ID_EDIT_COPY, OnCopy)
	ON_COMMAND(ID_EDIT_CUT, OnCut)
	ON_WM_CLOSE()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeBase drawing

void CTreeBase::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CTreeBase diagnostics

#ifdef _DEBUG
void CTreeBase::AssertValid() const
{
	CTreeView::AssertValid();
}

void CTreeBase::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTreeBase message handlers

void CTreeBase::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	CFilerEntry * pEntry = GetEntry(pNMTreeView->itemNew.hItem);

	if (pFiler->isAvailable() && pFiler->canBeDragged(pEntry)) {
		DoClipOp(pNMTreeView->itemNew.hItem);
		*pResult = 0;

		return;
	}

	*pResult = 1;
}

void CTreeBase::DragEnd()
{
	m_pDragImage->DragLeave (this);
	m_pDragImage->EndDrag ();
	if (m_pDragImage)
		delete m_pDragImage;
	m_pDragImage = NULL;
	ReleaseCapture();
	GetTreeCtrl().SelectDropTarget(NULL);
}

#ifdef _DEBUG
CHPCommDoc* CTreeBase::GetDocument() // non-debug version is inline
{
	VERIFY(m_pDocument->IsKindOf(RUNTIME_CLASS(CHPCommDoc)));
	return (CHPCommDoc*)m_pDocument;
}
#endif //_DEBUG

bool CTreeBase::TestHit(CPoint point)
{
	HTREEITEM hDropItem = GetTreeCtrl().HitTest(point);
	if (hDropItem != NULL) {
		GetTreeCtrl().SelectDropTarget(hDropItem);
		bool ret = pFiler->isValidDropTarget(GetEntry(hDropItem));
		if (bDragFilerIntern) {
			ret &= (pDragFromDir != GetEntry(hDropItem));
		}
		return ret;
	}
	else {
		GetTreeCtrl().SelectDropTarget(NULL);
		return false;
	}
	

}

CString CTreeBase::BuildPath(HTREEITEM hTI)
{
    CFilerEntry *fe = GetEntry(hTI);

    if (fe == 0)
        return "";

	return pFiler->toString(fe);
}

void CTreeBase::OnDelete()
{
	CWaitCursor wait;
	CTreeCtrl &TreeCtrl = GetTreeCtrl();

	// If the user is editing a filename and hits delete we unfortunately
	// end up here :-(
	// If pEdit != NULL we know that the user is editing.
	if (pEdit != NULL) {
		// As GetEditControl only returns a CTempWnd we make sure we
		// have the correct CWnd *.
		pEdit = TreeCtrl.GetEditControl();
		pEdit->SendMessage(WM_KEYDOWN, VK_DELETE, 0);
		return;
	}

	// Get the drophilited item (not only the selected one).
	// This is necessary to handle the deletion via the context menu.
	HTREEITEM hDel = TreeCtrl.GetSelectedItem();
	// Check the validity of hDel
	if (hDel == NULL || hDel == TreeCtrl.GetRootItem())
		return;

	CFilerEntry * fe = GetEntry(hDel);

	// Don't try to delete a drive.
	if (pFiler->isLocal() && fe->Parent() == pFiler->root())
		return;
	
	bool bDelCurDir = false;
	CFilerEntry * parent = fe->Parent();
	if (parent == pFiler->currentDir()) {
		bDelCurDir = true;
	}
    {
        CWaitCursor wait;
        try {
            pFiler->rm(fe);
        }
        catch (CProtException * pEx) {
            ExceptionHandler(pEx);	
        }
    }

	// Make sure that we update the current dir if we just deleted it.
	if (bDelCurDir) {
		pFiler->currentDir(parent);
	}
	
	//TreeCtrl.SelectItem(TreeCtrl.GetParentItem(hDel));
	if (!pFiler->isValid(GetEntry(hDel)))
		TreeCtrl.DeleteItem(hDel);

	UpdateAllViews();
}

void CTreeBase::OnInitialUpdate() 
{
	ModifyStyle(WS_VISIBLE, 0);

	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	// Create an entry for the root.
	
	if (pFiler->isAvailable()) {
		CFilerEntry * pRoot = pFiler->root();
		try {
			pFiler->findFile(pRoot);
		}
		catch (CProtException * pEx) {
			ExceptionHandler(pEx);		
		}
		// First we have to add the HOME as the ROOT.
		HTREEITEM hRoot = AddItem(pRoot, TVI_ROOT);
	}

	bool bRes = UpdateContent();

	//((CMainFrame *)AfxGetMainWnd())->BumpComInfoDlg(pFiler, READING, bRes);

}

HTREEITEM CTreeBase::AddItem(CFilerEntry *pEntry, HTREEITEM hParent)
{
	TVINSERTSTRUCT tvinsert;
	TVITEM tvi;
	CTreeCtrl& TreeCtrl = GetTreeCtrl();

	// We insert the item.
	CString sName = pEntry->Name();

	/*int nIcon, nSelIcon;
	pFiler->getIcons(pEntry, nIcon, nSelIcon);*/

	tvi.mask = TVIF_CHILDREN | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	tvi.hItem = NULL;
	tvi.pszText = (LPTSTR)(LPCTSTR)sName;
	tvi.cchTextMax = pEntry->Name().GetLength();
	tvi.iImage = I_IMAGECALLBACK;
	tvi.iSelectedImage = I_IMAGECALLBACK;
	tvi.lParam = (DWORD)pEntry;
	tvi.cChildren = I_CHILDRENCALLBACK;

	tvinsert.hParent = hParent;
	tvinsert.hInsertAfter = pEntry->Directory() ? TVI_SORT:TVI_LAST;
	tvinsert.item = tvi;

	HTREEITEM hTI = TreeCtrl.InsertItem(&tvinsert);

	return hTI;

}

HTREEITEM CTreeBase::AddItem(CString sName, CString sPath, HTREEITEM hParent, BOOL bDir)
{
	HTREEITEM hTI = GetTreeCtrl().InsertItem(sName, 1, 2, hParent, bDir ? TVI_SORT:TVI_LAST);
	return hTI;
}

void CTreeBase::DeleteChildren(HTREEITEM hTI)
{
	CTreeCtrl& TreeCtrl = GetTreeCtrl();

	HTREEITEM hChild = TreeCtrl.GetChildItem(hTI), hTemp;
	while (hChild != NULL) {
		hTemp = TreeCtrl.GetNextSiblingItem(hChild);
		TreeCtrl.DeleteItem(hChild);
		hChild = hTemp;
	}

}

void CTreeBase::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (!pFiler->isAvailable()) {
		*pResult = 1;
		return;
	}

	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	TreeCtrl.GetEditControl()->LimitText(_MAX_DIR);

	// Don't allow renaming of the root and of the drive names.
	if (pTVDispInfo->item.hItem == TreeCtrl.GetRootItem()) {
		*pResult = 1;
		return;
	}

	if (pFiler->isLocal() && (TreeCtrl.GetParentItem(pTVDispInfo->item.hItem) == TreeCtrl.GetRootItem())) {
		*pResult = 1;
	}
	else {
		sOldItemPath = BuildPath(TreeCtrl.GetParentItem(pTVDispInfo->item.hItem));
		pEdit = TreeCtrl.GetEditControl();
		if (pEdit) {
			pEdit->GetWindowText(sOldItemName);
			pEdit->LimitText(_MAX_FNAME);
			*pResult = 0;
		}
		else {
			*pResult = 1;
		}
	}
}

void CTreeBase::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    if (pFiler->isAvailable() == false)
        return;

	UINT uFlags;
	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	CPoint ptTree = point;
	TreeCtrl.ScreenToClient(&ptTree);
	HTREEITEM hTI = TreeCtrl.HitTest(ptTree, &uFlags);
	if ((hTI != NULL) && (uFlags & TVHT_ONITEM)) {
		CString sName = pFiler->toString(GetEntry(hTI));
		// If we have a drive, the name is C:, but we nee a ending backslash
		if (sName.Right(1) == ":")
			sName += "\\";

		ShowExplorerMenu((LPCTSTR)sName, ptTree);
	}
	else {
		CTreeView::OnContextMenu(pWnd, point);
	}
	if (m_pOldSel != NULL) {
		TreeCtrl.Select(m_pOldSel, TVGN_CARET);
		m_pOldSel = NULL;
	}
	TreeCtrl.SelectDropTarget(NULL);
	
}

void CTreeBase::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	if (pTVDispInfo->item.hItem == INVALID_HANDLE_VALUE || pTVDispInfo->item.pszText == 0) {
		*pResult = 1;
	}

	if (pEdit) {
		CString sName;
		pEdit = GetTreeCtrl().GetEditControl();
		pEdit->GetWindowText(sName);
		if (sOldItemName != sName) {
			// Only do some work if the names are different.
			CFilerEntry *fe = (CFilerEntry *)pTVDispInfo->item.lParam;
			if (pTVDispInfo->item.pszText != 0) {
				if (!Rename((CFilerEntry *)pTVDispInfo->item.lParam, pTVDispInfo->item.pszText) && pEdit != NULL) {
					// The entry could not be renamed.
					AfxMessageBox("The file could not be renamed!\nMake sure the name is unique and valid.", MB_OK | MB_ICONSTOP);
					pEdit->SetWindowText(sOldItemName);
				}
			}
		}
	}

	pEdit = NULL;
}

void CTreeBase::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	if ((pNMTreeView->hdr.code == TVN_ITEMEXPANDING) && (pNMTreeView->action == TVE_EXPAND)) {
	
		CWaitCursor wait;

		HTREEITEM hTI = pNMTreeView->itemNew.hItem;

		// If there's a dummy, remove it.
		//RemoveDummy(hTI);

		if (!ReadDir(hTI)) {
			// Don't expand, because dir could not be read
			*pResult = 1;
			return;
		}

		//SetItemIcons(hTI);
	}
	
	*pResult = 0;
}

void CTreeBase::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (!pFiler->isAvailable()) {
		return;
	}
	UINT uFlags;
	HTREEITEM hTI = GetTreeCtrl().HitTest(point, &uFlags);
	//Check which item was right-clicked
	if ((hTI != NULL) && (uFlags & TVHT_ONITEM)) {
		m_pOldSel = GetTreeCtrl().GetSelectedItem();
		GetTreeCtrl().Select(hTI, TVGN_DROPHILITE);
		hPopItem = hTI;
	}
}

BOOL CTreeBase::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_EDITLABELS;
	return CTreeView::PreCreateWindow(cs);
}

BOOL CTreeBase::ReadDir(HTREEITEM hTI)
{
	SetRedraw(FALSE);
	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	// We get the CFilerEntry ptr from the data field of the tree and
	// start browsing the directory.
	try {
		if (!pFiler->findFile(GetEntry(hTI))) {
			SetRedraw();
			return FALSE;
		}
	}
	catch (CProtException * pEx) {
		ExceptionHandler(pEx);	
	}


	DeleteChildren(hTI);
	CFilerEntry * fe;

	// While there are files, we add them to the tree.
    while((fe = pFiler->findFileNext()) != NULL)
    {
        // ... but only if it's a directory.
        if (fe->Directory()) {
			HTREEITEM hSub = AddItem(fe, hTI);
		}
    }

	TreeCtrl.UpdateWindow();
	SetRedraw();
	return TRUE;

}

BOOL CTreeBase::Rename(CFilerEntry * pEntry, char *psNew)
{

	CWaitCursor wait;
	if (psNew == NULL)
		return FALSE;

	if (psNew[strlen(psNew) - 1] == pFiler->separator()) {
		psNew[strlen(psNew) - 1] = '\0';
	}

	BOOL bRet;
	try {
		bRet = pFiler->rename(pEntry, psNew);
	}
	catch (CProtException * pEx) {
		ExceptionHandler(pEx);
	}

	((CMainFrame *)AfxGetMainWnd())->UpdateAllViews(pFiler, true);

	return bRet;

}

void CTreeBase::ShowPopupMenu(CPoint &pt)
{
	CMenu menu;
	if (pFiler->isLocal()) {
		VERIFY(menu.LoadMenu(IDR_TREEPOPUP));
	}
	else {
		VERIFY(menu.LoadMenu(IDR_TREEPOPUPCALC));	
	}
	CMenu * pPopup = menu.GetSubMenu(0);
	VERIFY(pPopup != NULL);
	CWnd * pPopupOwner = this;
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, pPopupOwner);

}

void CTreeBase::OnTreepopupDelete() 
{
	
}

void CTreeBase::OnTreepopupExplore() 
{
	pFiler->currentDir(GetEntry(hPopItem));
	m_pOldSel = hPopItem;
	GetTreeCtrl().Select(m_pOldSel, TVGN_DROPHILITE);
	GetTreeCtrl().Select(m_pOldSel, TVGN_CARET);
	UpdateAllViews();
	
}

void CTreeBase::OnTreepopupOpen() 
{
	ShellExecute(m_hWnd, "open", BuildPath(hPopItem), NULL, NULL, SW_SHOWNORMAL);	
}

void CTreeBase::OnOpeninexplorer() 
{
	ShellExecute(m_hWnd, "open", BuildPath(GetTreeCtrl().GetSelectedItem()), NULL, NULL, SW_SHOWNORMAL);
}

HTREEITEM CTreeBase::OpenDir(CFilerEntry *pEntry)
{
	if (pEntry == 0) {
		return GetTreeCtrl().GetRootItem();
	}

	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	

	// Try to find the item under the current selected item.
	// This is the normal case (the user double clicks in the list view.
	HTREEITEM hSel = TreeCtrl.GetSelectedItem();

	TreeCtrl.Expand(hSel, TVE_EXPAND);
	CString s = TreeCtrl.GetItemText(hSel);
	// Go to the child item.
	hSel = TreeCtrl.GetChildItem(hSel);
	// In this level we iterate through all entires till we find the one.
	while (hSel != NULL) {
		if (TreeCtrl.GetItemData(hSel) == (DWORD)pEntry)
			break;
		hSel = TreeCtrl.GetNextSiblingItem(hSel);
	}
	if (hSel != NULL) {
		ReadDir(hSel);
		return hSel;
	}
	
	// The item was not a child of the selected item.
	// This only happens with a MRU entry.

    // We must open/show the item represented by pEntry
	// As we must start with the root item in the tree-view to search the desired
	// element pEntry we fill a stack with all element from pEntry and its parent
	// down to the root item. Afterwards we can up from the root to the item.
	// If we have e.g. C:\Data\Texts\Letters the stack is filled with the entries
	// C: -> Data -> Texts -> Letters
    CFilerEntryList stack;
	CFilerEntry * fe = pEntry;
	while (pFiler->isRoot(fe) != true) {
		// we add all elements to the stack
		stack.AddHead(fe);
		fe = fe->Parent();
	}

	// We start with the root-item.
	HTREEITEM hTI = TreeCtrl.GetRootItem();

	// As long as we have another entry in the stack (another dir-level) ...
	while (!stack.IsEmpty()) {
		// We read the entries in.
		ReadDir(hTI);
		// We expand this branch...
		TreeCtrl.Expand(hTI, TVE_EXPAND);
		// ... and go to the child item (this means the next upper dir-level).
		hTI = TreeCtrl.GetChildItem(hTI);
		// In this level we iterate through all entires till we find the one
		// in our path.
		while (hTI != NULL && GetEntry(hTI) != stack.GetHead()) {
			hTI = TreeCtrl.GetNextSiblingItem(hTI);
		}
		// As we found it, we remove this item from the stack and continue with
		// the next level.
		stack.RemoveHead();
	}
	//TreeCtrl.EnsureVisible(hTI);
	return hTI;
	
}

void CTreeBase::OnRename() 
{
	GetTreeCtrl().EditLabel(GetTreeCtrl().GetSelectedItem());
}


DROPEFFECT CTreeBase::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
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
		return DROPEFFECT_NONE;
	}

	return OnDragOver(pDataObject, dwKeyState, point);
}

void CTreeBase::OnDragLeave() 
{
	// If the mouse leaved, remove the drop hilite.
	GetTreeCtrl().SelectDropTarget(NULL);
	CTreeView::OnDragLeave();
}

DROPEFFECT CTreeBase::OnDragScroll(DWORD dwKeyState, CPoint point) 
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
	return CTreeView::OnDragScroll(dwKeyState, point);
}

DROPEFFECT CTreeBase::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
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
	
	//return CTreeView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL CTreeBase::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	if (DROPEFFECT_NONE)
		return FALSE;

	char sName[50];
	BOOL bInternal = FALSE;
	UINT cb_format;
	pDataObject->BeginEnumFormats();
	FORMATETC format;
	// Enum all available clipbooard formats.
	while (pDataObject->GetNextFormat(&format)) {
		int n = GetClipboardFormatName(format.cfFormat, sName, 50);
		if (strcmp(sName, HPCommClipboardFormat) == 0) {
			// We found our own internal clipboard format.
			cb_format = format.cfFormat;
			bInternal = TRUE;
			break;
		}
	}

	CFilerEntry * pDropTarget;
	HTREEITEM hTI = GetTreeCtrl().HitTest(point);

	// We determine if the drop-point is over a directory.
	if (hTI != NULL) {
		// Yes, it is. So we set the droptarget to the entry where the drop occured.
		pDropTarget = GetEntry(hTI);
	}
	else {
		// We are not over a dir and the target undefined, abort the whole thing.
		return FALSE;		
	}

	// Depending on the clipboard format we process the drop action.
	if (bInternal) {
		// The drag and drop operation started in HPComm
		HGLOBAL hData = pDataObject->GetGlobalData(cb_format);
		clipInfo_ * pClipInfo = (clipInfo_ *)GlobalLock(hData);
		GlobalUnlock(hData);

 		// Convert the CFilerEntry* array back into list.
		CFilerEntryList fromList;
		for (CFilerEntry **fe = &(pClipInfo->pFromFirstEntry); *fe != 0; fe++)
			fromList.AddTail(*fe);

       // FIXME: What about DROPEFFECT_NONE, DROPEFFECT_LINK, or
        // DROPEFFECT_SCROLL??
        bool removeAfterCopy = (dropEffect == DROPEFFECT_MOVE);

		CTreeView::OnDrop(pDataObject, dropEffect, point);
		
		try {
			pFiler->cp(removeAfterCopy, pClipInfo->pFromFiler, 
				   &fromList, pDropTarget);
		}
		catch (CProtException * pEx) {
			ExceptionHandler(pEx);
		}

		// Update the views. Will be done at the end of the method if
		// CFilerPC::refresh is implemented.
		//ReadDir(hTI);
		UpdateAllViews();
	}
	else {
		// The drag and drop operation started from elsewhere, e.g. in the explorer.
		HDROP hDrop = (HDROP)pDataObject->GetGlobalData(CF_HDROP);
		
		CFiler * fromFiler = 0;
		((CMainFrame *)AfxGetMainWnd())->PrepareExtDrop(hDrop, fromFiler);

        bool removeAfterCopy = (dropEffect == DROPEFFECT_MOVE);
        
		CTreeView::OnDrop(pDataObject, dropEffect, point);
		
		try {
			pFiler->cp(removeAfterCopy, hDrop, fromFiler, pDropTarget);
		}

		catch (CProtException * pEx) {
			ExceptionHandler(pEx);
		}

		// Pretty messy to determine here if we had a dir dropped.
		// Just update also the tree.
		UpdateAllViews();
	}


	// Remove the hilited state of the item we dropped on.
	GetTreeCtrl().SelectDropTarget(NULL);
	
	return TRUE;
}

int CTreeBase::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	oleDropTarget.Register(this);
	return 0;
}

BOOL CTreeBase::DoClipOp(HTREEITEM hTI, BOOL bClipboard /* = FALSE */, BOOL bCut /* = FALSE*/)
{
	if (hTI == NULL) {
		// No item was selected.
		return FALSE;
	}
	CTreeCtrl &TreeCtrl = GetTreeCtrl();

	// The size of pFrom.
	int nSize = 0;

	CFilerEntry *fe = GetEntry(hTI);

	CString sItemText = pFiler->toString(fe);
	nSize = sItemText.GetLength();
	char *pFrom = new char[sItemText.GetLength() + 2];
	// Set it to 0
	memset(pFrom, '\0', sItemText.GetLength() + 2);

	strcpy(pFrom, sItemText);

	nSize += 2; // The ending zero char

	if (bClipboard) {
		if (!OpenClipboard()) {
			((CHPCommApp *)AfxGetApp())->LastErrorMessage(GetLastError());
			return FALSE;
		}
		if (!EmptyClipboard()) {
			((CHPCommApp *)AfxGetApp())->LastErrorMessage(GetLastError());	
			return FALSE;
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

  	unsigned int nClipInfoSize = sizeof(clipInfo_) + 2 * sizeof(CFilerEntry*);
	clipInfo_ * pClipInfo = (clipInfo_ *)new char[nClipInfoSize];
	pClipInfo->pFromFiler = pFiler;

	// Add the single CFilerEntry*.
	CFilerEntry **pfe = &(pClipInfo->pFromFirstEntry);
	*pfe++ = fe;
	// Terminate the list.
	*pfe = 0;
#ifdef _DEBUG
	TRACE("pClipInfo: %x\n", pClipInfo);
	TRACE("pClipInfo->pFromFirstEntry: %x\n", pClipInfo->pFromFirstEntry);
	TRACE("pClipInfo->pFromFiler: %x\n", pClipInfo->pFromFiler);
#endif
    // Allocate global memory to hold the clipInfostruct.
	const int options = GMEM_SHARE | GMEM_MOVEABLE | GMEM_ZEROINIT;
	HGLOBAL hGlobalClipInfo = GlobalAlloc(options, nClipInfoSize);

	// Get the ptr to the global allocated memory.
	clipInfo_ * pGlobalClipInfo = (clipInfo_ *)GlobalLock(hGlobalClipInfo);

	// Copy the list to the global memory.
	memcpy(pGlobalClipInfo, pClipInfo, nClipInfoSize);

	GlobalUnlock(hGlobalClipInfo);
#ifdef _DEBUG
	TRACE("hGlobalClipInfo: %x\n", hGlobalClipInfo);
#endif

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

	// If the file has been moved or cutted...
	if (dropEffect == DROPEFFECT_MOVE) {
		// ...delete the files. 
		CFilerEntryList list;
		list.AddHead(fe);
		try {
			pFiler->rm(&list);
		}
		catch (CProtException * pEx) {
			ExceptionHandler(pEx);		
		}
	}

	delete pClipInfo;
	delete pFrom;

	return TRUE;
}

CFilerEntry * CTreeBase::GetEntry(HTREEITEM hTI)
{
    if (hTI == 0)
        return 0;
	return (CFilerEntry *)GetTreeCtrl().GetItemData(hTI);
}

void CTreeBase::OnPaste() 
{
	CWaitCursor wait;

	char sName[50];
	BOOL bInternal = FALSE;
	UINT cb_format;
	
	// Open the clipboard to access the data.
	if (!OpenClipboard()) {
		((CHPCommApp *)AfxGetApp())->LastErrorMessage(GetLastError());
		return;
	}

	// Start the enumeration of all available clipboard formats with the 
	// value null.
	cb_format = 0;
	// Enum all available clipbooard formats.
	while (cb_format = EnumClipboardFormats(cb_format)) {
		int n = GetClipboardFormatName(cb_format, sName, 50);
		if (strcmp(sName, HPCommClipboardFormat) == 0) {
			// We found our own internal clipboard format.
			bInternal = TRUE;
			break;
		}
		if (cb_format == CF_HDROP) {
			// The drag and drop operation started from elsewhere, e.g. in the explorer.
			break;
		}
	}
	
	// We didn't find an appropriate clibboard format: exit.
	if (cb_format == 0) {
		CloseClipboard();
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
		try {
			pFiler->cp(false, pClipInfo->pFromFiler, &fromList, pFiler->currentDir());	
		}
		catch (CProtException * pEx) {
			ExceptionHandler(pEx);
		}

	}
	else {
		HDROP hDrop = (HDROP)hClipData;
		
		CFiler * fromFiler = 0;
		((CMainFrame *)AfxGetMainWnd())->PrepareExtDrop(hDrop, fromFiler);

		HTREEITEM hSel = hPopItem;
		if (hPopItem == NULL)
			hSel = GetTreeCtrl().GetSelectedItem();
		
		ASSERT(hSel != NULL);
		CFilerEntry * pDropTarget = GetEntry(hSel);
		if (!pFiler->isValidDropTarget(pDropTarget)) {
			CloseClipboard();
			return;
		}

		// Now call the filer to perform the operation.
		try {
			pFiler->cp(false, hDrop, fromFiler, pDropTarget);
		}
		catch (CProtException * pEx) {
			ExceptionHandler(pEx);
		}

		// Pretty messy to determine here if we had a dir dropped.
		// Just update also the tree.
	}
	CloseClipboard();
	UpdateAllViews();

}

void CTreeBase::RemoveDummy(HTREEITEM hTI)
{
	CTreeCtrl &TreeCtrl = GetTreeCtrl();
	// Check if there are children and if it is a dummy item.
	if (TreeCtrl.ItemHasChildren(hTI) && 
		TreeCtrl.GetItemText(TreeCtrl.GetChildItem(hTI)) == "?") {
			// Delete the dummy.
			TreeCtrl.DeleteItem(TreeCtrl.GetChildItem(hTI));
	}

}

void CTreeBase::SetItemIcons(HTREEITEM hTI)
{
	int icon, selicon;
	GetTreeCtrl().GetItemImage(hTI, icon, selicon);
	GetTreeCtrl().SetItemImage(hTI, icon, selicon);
	/*CFilerEntry * fe = GetEntry(hTI);
	if (!pFiler->isRoot(fe)) {
		// Set the new images for the item.
		int icon;
		int selIcon;
		pFiler->getIcons(fe, icon, selIcon);
		GetTreeCtrl().SetItemImage(hTI, icon, selIcon);
	}*/
}

bool CTreeBase::UpdateContent()
{
	hPopItem = 0;
	if (!pFiler->isAvailable()) {
		ShowNotConnected();
		return false;
	}

	CTreeCtrl &TreeCtrl = GetTreeCtrl();
	if (TreeCtrl.GetItemData(TreeCtrl.GetRootItem()) == 0) {
		// "Not Connected" is shown.
		// We have to add the root item.
		ModifyStyle(0, TVS_LINESATROOT);
		TreeCtrl.DeleteAllItems();
		// This is the first update with a connection
		CFilerEntry * pRoot = pFiler->root();
		
		try {
			pFiler->findFile(pRoot);
		}
		catch (CProtException * pEx) {
			ExceptionHandler(pEx);		
		}
		// First we have to add the HOME as the ROOT.
		HTREEITEM hRoot = AddItem(pRoot, TVI_ROOT);
	}
	// Make sure the root item is expanded. Looks better.
	TreeCtrl.Expand(TreeCtrl.GetRootItem(), TVE_EXPAND);

	HTREEITEM hTI = TreeCtrl.GetSelectedItem();

	bool bRet = false;
	if (hTI != NULL) {
		bRet = (ReadDir(hTI) == TRUE);
	}

	bBrowse = true;
	bRet &= (TreeCtrl.SelectItem(OpenDir(pFiler->currentDir())) == TRUE);
	bBrowse = false;

	return bRet;

}

void CTreeBase::UpdateAllViews()
{
	((CMainFrame *)AfxGetMainWnd())->UpdateAllViews(pFiler, false);
}

void CTreeBase::ShowNotConnected()
{
	CTreeCtrl &TreeCtrl = GetTreeCtrl();
	bool bUpdate = false;
	// Check if "Not connected" is not shown yet.
	HTREEITEM hRoot = TreeCtrl.GetRootItem();
	if (hRoot == NULL) {
		bUpdate = true;
	}
	else if (TreeCtrl.GetItemData(hRoot) != 0) {
		bUpdate = true;
	}
	if (bUpdate) {
		ModifyStyle(TVS_LINESATROOT, 0);
		TreeCtrl.DeleteAllItems();
        CCalcTypes *ct = CCalcTypes::instance();
        int home = ct->icon("HOME");
		TreeCtrl.InsertItem("Not connected", home, home, TVI_ROOT);
		// If the root item has a 0 ptr as data, it indicates that
		// "Not connected is shown"
		TreeCtrl.SetItemData(hRoot, 0);
	}
}

void CTreeBase::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	if (pTVDispInfo->item.mask & TVIF_CHILDREN) {
		CString s = GetTreeCtrl().GetItemText(pTVDispInfo->item.hItem);
		CFilerEntry * fe = (CFilerEntry *)pTVDispInfo->item.lParam;
		// If this is not a valid item (i.e. after a deletion)
		// just leave.
		if (!pFiler->isValid(fe))
			return;

		try {
			if (pFiler->hasSubDir(fe)) {
				pTVDispInfo->item.cChildren = 1;
			}
			else {
				pTVDispInfo->item.cChildren = 0;		
			}
		}
		catch (CProtException * pEx) {
			ExceptionHandler(pEx);		
		}
	}

	if (pTVDispInfo->item.mask & TVIF_IMAGE) {
			int icon, selicon;
			pFiler->getIcons(GetEntry(pTVDispInfo->item.hItem), icon, selicon);
			pTVDispInfo->item.iImage = icon;
			pTVDispInfo->item.iSelectedImage = selicon;
	}
	
	if (pTVDispInfo->item.mask & TVIF_SELECTEDIMAGE) {
			int icon, selicon;
			pFiler->getIcons(GetEntry(pTVDispInfo->item.hItem), icon, selicon);
			pTVDispInfo->item.iImage = icon;
			pTVDispInfo->item.iSelectedImage = selicon;	
	}

	*pResult = 0;
}

void CTreeBase::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (bBrowse || !pFiler->isAvailable()) {
		*pResult = 1;
		return;
	}
	NMTREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	CTreeCtrl &TreeCtrl = GetTreeCtrl();

	HTREEITEM hTI = pNMTreeView->itemNew.hItem;

	// Set the current dir to the value in the data field of the item the user
	// selected.
	CFilerEntry *newDir = GetEntry(hTI);
	
	if (newDir != 0)
		pFiler->currentDir(newDir);

	UpdateAllViews();

	*pResult = 0;

}

void CTreeBase::OnCopy() 
{
	HTREEITEM hSel = GetTreeCtrl().GetSelectedItem();
	if (hSel != NULL)
		DoClipOp(hSel, TRUE);
}

void CTreeBase::OnCut() 
{
	HTREEITEM hSel = GetTreeCtrl().GetSelectedItem();
	if (hSel != NULL)
		DoClipOp(hSel, TRUE, TRUE);
}

void CTreeBase::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
//	GetTreeCtrl().GetImageList(TVSIL_NORMAL)->Detach();	

	CTreeView::OnClose();
}

void CTreeBase::ExceptionHandler(CProtException *pEx)
{
	((CMainFrame *)AfxGetMainWnd())->ExceptionHandler(pEx);
}

void CTreeBase::OnProperties() 
{
	HTREEITEM hTI = GetTreeCtrl().GetSelectedItem();
	if (hTI != NULL) {
		CString sFileName = pFiler->toString(GetEntry(hTI));
		POINT point;
		point.x = 20;
		point.y = 20;
		ShowExplorerMenu(sFileName, point, true);
	}
	
	
}

BOOL CTreeBase::ShowExplorerMenu(LPCTSTR pszPath, POINT point, bool bShowProperties /* = false */)
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
    if (SUCCEEDED (psfFolder->ParseDisplayName(hwnd,
        NULL, wchPath, &ulCount, &pidlMain, &ulAttr)) && (pidlMain != NULL)) {

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
                        ici.cbSize          = sizeof (CMINVOKECOMMANDINFO);
                        ici.fMask           = 0;
                        ici.hwnd            = hwnd;
                        ici.lpParameters    = NULL;
                        ici.lpDirectory     = NULL;
                        ici.nShow           = SW_SHOWNORMAL;
                        ici.dwHotKey        = 0;
                        ici.hIcon           = NULL;

                        if (SUCCEEDED (
                            pContextMenu->InvokeCommand(&ici)))
                            bResult = TRUE;
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

UINT CTreeBase::GetPCount(LPITEMIDLIST pidl)
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

LPITEMIDLIST CTreeBase::DuplicateItem(LPMALLOC pMalloc, LPITEMIDLIST pidl)
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

LPITEMIDLIST CTreeBase::GetNextItem(LPITEMIDLIST pidl)
{
    USHORT nLen;

    if ((nLen = pidl->mkid.cb) == 0)
        return NULL;
    
    return (LPITEMIDLIST) (((LPBYTE) pidl) + nLen);
}
