//////////////////////////////////////////////////////////////////////
/////////////////////              HPComm            /////////////////
//////////////////////////////////////////////////////////////////////




#if !defined(AFX_TREEBASE_H__E40AA54A_1221_11D3_AE2A_0000E8627120__INCLUDED_)
#define AFX_TREEBASE_H__E40AA54A_1221_11D3_AE2A_0000E8627120__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeBase.h : header file
//

#include <afxole.h>
#include "HPCommDoc.h"
#include "Settings.h"
class CProtException;


/////////////////////////////////////////////////////////////////////////////
// CTreeBase view

class CTreeBase : public CTreeView
{
protected:
	CTreeBase();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTreeBase)

// Attributes
public:
	void ExceptionHandler(CProtException * pEx);
	CFilerEntry * pDragFromDir;
	UINT cb_format;
	bool bInternalCF;
	bool bDragFilerIntern;
	void ShowNotConnected();
	bool UpdateContent();
	CSettings * pSettings;
	CFilerEntry * GetEntry(HTREEITEM hTI);
	BOOL DoClipOp(HTREEITEM hTI, BOOL bClipboard = FALSE, BOOL bCut = FALSE);
	COleDropTarget oleDropTarget;
	HTREEITEM OpenDir(CFilerEntry * pEntry);
	CHPCommDoc * GetDocument();
	CFiler * pFiler;

// Operations
private:
	enum {InitialScrollWait = 400};
	enum {ScrollRange = 30};
	int nScrollWait;
	CEdit * pEdit;
	LPITEMIDLIST GetNextItem(LPITEMIDLIST pidl);
	LPITEMIDLIST DuplicateItem (LPMALLOC pMalloc, LPITEMIDLIST pidl);
	BOOL ShowExplorerMenu(LPCTSTR pszPath, POINT point, bool bShowProperties = false);
	UINT GetPCount(LPITEMIDLIST pidl);
	void UpdateAllViews();
	void SetItemIcons(HTREEITEM hTI);
	// If there is a dummy sub-entry, it is removed, otherwise nothing happens.
	void RemoveDummy(HTREEITEM hTI);
	bool bBrowse;
	HTREEITEM m_pOldSel;
	HTREEITEM hPopItem;

	CString sOldItemName;
	CString sOldItemPath;
	void ShowPopupMenu(CPoint & pt);
	BOOL Rename(CFilerEntry * pEntry, char * psNew);
	BOOL ReadDir(HTREEITEM hTI);
	void DeleteChildren(HTREEITEM hTI);
	HTREEITEM AddItem(CString sName, CString sPath, HTREEITEM hParent, BOOL bDir = FALSE);
	HTREEITEM AddItem(CFilerEntry * pEntry, HTREEITEM hParent);
	CString BuildPath(HTREEITEM hTI);
	// Hilites an entry if we drag over it.
	// Returns true if the item is a drop target (esp. the root)
	bool TestHit(CPoint point);
	CImageList * m_pDragImage;
	void DragEnd();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeBase)
	public:
	virtual void OnInitialUpdate();
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragScroll(DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTreeBase();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeBase)
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTreepopupDelete();
	afx_msg void OnTreepopupExplore();
	afx_msg void OnTreepopupOpen();
	afx_msg void OnOpeninexplorer();
	afx_msg void OnRename();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaste();
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelete();
	afx_msg void OnCopy();
	afx_msg void OnCut();
	afx_msg void OnClose();
	afx_msg void OnProperties();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#ifndef _DEBUG  // debug version in HPCommView.cpp
inline CHPCommDoc* CTreeBase::GetDocument()
   { return (CHPCommDoc*)m_pDocument; }
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEBASE_H__E40AA54A_1221_11D3_AE2A_0000E8627120__INCLUDED_)
