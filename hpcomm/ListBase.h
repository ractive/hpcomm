//////////////////////////////////////////////////////////////////////
/////////////////////              HPComm            /////////////////
//////////////////////////////////////////////////////////////////////




#if !defined(AFX_LISTBASE_H__E40AA54B_1221_11D3_AE2A_0000E8627120__INCLUDED_)
#define AFX_LISTBASE_H__E40AA54B_1221_11D3_AE2A_0000E8627120__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListBase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListBase view

#include <afxole.h>

#include "HPCommDoc.h"
#include "Filer.h"
#include "Settings.h"

class CProtException;

class CListBase : public CListView
{
protected:
	CListBase();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CListBase)

// Attributes
public:
	void ExceptionHandler(CProtException * pEx);
	void newFolder();
	void ConnectionFailedDlg();
	CFilerEntry * GetSelectedEntry();
	CFilerEntry * pDragFromDir;
	UINT cb_format;
	bool bInternalCF;
	bool bDragFilerIntern;
	bool UpdateContent();
	CSettings * pSettings;
	bool DoClipOp(bool bClipboard = false, bool bCut = false);
	COleDropTarget oleDropTarget;
	void ShowNotConnected();
	CFiler * pFiler;

// Operations
private:
	enum {InitialScrollWait = 400};
	enum {ScrollRange = 30};
	CEdit * pEdit;
	CString sOldName;
	LPITEMIDLIST GetNextItem(LPITEMIDLIST pidl);
	LPITEMIDLIST DuplicateItem (LPMALLOC pMalloc, LPITEMIDLIST pidl);
	UINT GetPCount(LPITEMIDLIST pidl);
	BOOL ShowExplorerMenu(LPCTSTR pszPath, POINT point, bool bShowProperties = false);
	bool bNewFolder;
	BOOL bSortAscending;
	int nSortedCol;

	void UpdateAllViews(bool bTree);
	CFilerEntry * GetEntry(UINT nItem);
	BOOL IsDirectory(UINT nItem);
	CHPCommDoc * GetDocument();
	CString sOldItemName;
	CImageList * pImageList, * pBigImageList;
	int nPopItem;
	BOOL SortItems(int nCol, BOOL bAscending, int low = 0, int high = -1);
	void Sort();
	BOOL Rename(CFilerEntry * pEntry, char * psNew);
	BOOL ReadDir();
	int CmpItems(CString cstL, CString cstR, int nCol);
	void AddItem(CFilerEntry * pEntry);
	UINT nItem;
	virtual void ChangeDir(CString * psDir);
	CImageList * m_pDragImage;
	bool TestHit(CPoint point);
	int nScrollWait;	// Sleep so many ms after a scroll operation 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListBase)
	public:
	virtual void OnInitialUpdate();
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragScroll(DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	int nDropItem, nOldDropItem;
	virtual ~CListBase();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CListBase)
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDirpopupExplore();
	afx_msg void OnOpeninexplorer();
	afx_msg void OnDirpopupSend();
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeaderclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPopupDelete();
	afx_msg void OnPopupOpen();
	afx_msg void OnPopupOpenwithnotepad();
	afx_msg void OnProperties();
	afx_msg void OnPopupSendKermit();
	afx_msg void OnPopupSendXmodem();
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRename();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEditUndo();
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelete();
	afx_msg void OnClose();
	afx_msg void OnSelectall();
	//}}AFX_MSG
	afx_msg void OnMRUDir();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG  // debug version in HPCommView.cpp
inline CHPCommDoc* CListBase::GetDocument()
   { return (CHPCommDoc*)m_pDocument; }
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTBASE_H__E40AA54B_1221_11D3_AE2A_0000E8627120__INCLUDED_)
