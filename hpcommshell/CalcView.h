// CalcView.h: interface for the CCalcView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALCVIEW_H__ED809DFC_B2CD_47FD_960B_27641E5C992C__INCLUDED_)
#define AFX_CALCVIEW_H__ED809DFC_B2CD_47FD_960B_27641E5C992C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCalcPidl;
class CDockingWindow;
class CCalcFolder;

#define NS_CLASS_NAME   (TEXT("HPCommShellViewNSClass"))
#define ID_LISTVIEW     2000


class CCalcView : public IShellView, public IOleCommandTarget
{
friend CDockingWindow;

public:
	CCalcView(CCalcFolder * pFolder, LPITEMIDLIST pidl);
	virtual ~CCalcView();

	// IUnknown
	virtual HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();

// Operations
public:
	virtual HRESULT __stdcall GetWindow(HWND * lphwnd);
	virtual HRESULT __stdcall ContextSensitiveHelp(BOOL fEnterMode);

		// *** IShellView methods ***
	virtual HRESULT __stdcall  TranslateAccelerator(LPMSG lpmsg);
#ifdef _FIX_ENABLEMODELESS_CONFLICT
	virtual HRESULT __stdcall (EnableModelessSV) (THIS_ BOOL fEnable);
#else
	virtual HRESULT __stdcall EnableModeless(BOOL fEnable);
#endif
	virtual HRESULT __stdcall UIActivate(UINT uState);
	virtual HRESULT __stdcall Refresh();
	virtual HRESULT __stdcall CreateViewWindow(IShellView  *lpPrevView, LPCFOLDERSETTINGS lpfs, IShellBrowser  * psb, RECT * prcView, HWND  *phWnd);
	virtual HRESULT __stdcall DestroyViewWindow();
	virtual HRESULT __stdcall GetCurrentInfo(LPFOLDERSETTINGS lpfs);
	virtual HRESULT __stdcall AddPropertySheetPages(DWORD dwReserved, LPFNADDPROPSHEETPAGE lpfn, LPARAM lparam);
	virtual HRESULT __stdcall SaveViewState();
	virtual HRESULT __stdcall SelectItem(LPCITEMIDLIST pidlItem, UINT uFlags);
	virtual HRESULT __stdcall GetItemObject(UINT uItem, REFIID riid, LPVOID *ppv);

	//IOleCommandTarget methods
	STDMETHOD (QueryStatus) (const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText);
	STDMETHOD (Exec) (const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdExecOpt, VARIANTARG *pvaIn, VARIANTARG *pvaOut);
	
	BOOL AddRemoveDockingWindow(BOOL bAdd);

private:
	static int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lpData);

	static int m_nSortColumn;
	static BOOL m_bSortAscending;
	void SortList();
	LRESULT OnSize(WORD wWidth, WORD wHeight);
	LRESULT OnCreate(void);
	LRESULT OnNotify(UINT CtlID, LPNMHDR lpnmh);
	LRESULT UpdateMenu(HMENU hMenu);
	LRESULT OnSetFocus(void);
	LRESULT OnKillFocus(void);
	LRESULT OnActivate(UINT uState);
	void OnDeactivate();
	LRESULT OnCommand(DWORD dwCmdID, DWORD dwCmd, HWND hwndCmd);
	LRESULT OnSettingChange(LPCTSTR lpszSection);

	HMENU BuildRegistryMenu(void);

	BOOL CreateList(void);
	void UpdateShellSettings(void);
	UINT m_uState;
	HMENU m_hMenu;
	LPMALLOC m_pMalloc;
	HWND m_hwndParent;
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	void FillList();
	CCalcFolder * m_pSFParent;
	CDockingWindow * m_pDockingWindow;
	CCalcPidl * m_pCalcPidl;
	LPITEMIDLIST m_pidl;
	BOOL InitListView();
	HWND m_hMainWnd;
	HWND m_hWndListView;
	long m_nRef;
	LPSHELLBROWSER m_pShellBrowser;
	FOLDERSETTINGS m_FolderSettings;
};

#endif // !defined(AFX_CALCVIEW_H__ED809DFC_B2CD_47FD_960B_27641E5C992C__INCLUDED_)
