// CalcView.cpp: implementation of the CCalcView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CalcView.h"
#include "CalcPidl.h"
#include "PropertyPage.h"
#include "DockingWindow.h"
#include "CalcFolder.h"
#include "resource.h"
#include <stdio.h>
#include "GUID.h"

extern long global_nComponents;
extern HIMAGELIST global_hImageListSmall;
extern HIMAGELIST global_hImageListLarge;
extern int WideCharToLocal(LPTSTR pLocal, LPWSTR pWide, DWORD dwChars);

int CCalcView::m_nSortColumn = 0;
BOOL CCalcView::m_bSortAscending = TRUE;

#define TOOLBAR_ID   (L"HPCommShell")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCalcView::CCalcView(CCalcFolder * pFolder, LPITEMIDLIST pidl)
{
	#ifdef INITCOMMONCONTROLSEX

	INITCOMMONCONTROLSEX iccex;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&iccex);

	#else

	InitCommonControls();

	#endif   //INITCOMMONCONTROLSEX

	m_hMenu = NULL;
	m_pDockingWindow = NULL;
	m_pCalcPidl = new CCalcPidl();
	if (!m_pCalcPidl) {
		delete this;
		return;
	}
	
	m_pSFParent = pFolder;
	if (m_pSFParent) {
		m_pSFParent->AddRef();
	}

	m_nRef = 1;
	m_pidl = m_pCalcPidl->CopyItem(pidl);
	m_pDockingWindow = NULL;

	m_pMalloc = NULL;
	if (FAILED(SHGetMalloc(&m_pMalloc))) {
		delete this;
		return;
	}

	m_uState = SVUIA_DEACTIVATE;

	InterlockedIncrement(&global_nLockCount);
}

CCalcView::~CCalcView()
{
	if(m_pidl) {
		m_pCalcPidl->Delete(m_pidl);
		m_pidl = NULL;
	}

	if(m_pSFParent) {
		m_pSFParent->Release();
	}

	if(m_pMalloc) {
		m_pMalloc->Release();
	}

	if (m_pCalcPidl) {
		delete m_pCalcPidl;
	}


	InterlockedDecrement(&global_nLockCount);
}

/* 
 * IUnknown QueryInterface
 */
HRESULT __stdcall CCalcView::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
	if (IsEqualIID(riid, IID_IUnknown))
	{
		*ppvObj = static_cast<IUnknown*>((IShellView *) this);
	}
	else if (IsEqualIID(riid, IID_IShellView))
	{
		*ppvObj = static_cast<IShellView*>(this);
	}
	else if (IsEqualIID(riid, IID_IOleWindow))
	{
		*ppvObj = static_cast<IOleWindow*>(this);
	}
	else if (IsEqualIID(riid, IID_IOleCommandTarget))
	{
		*ppvObj = static_cast<IOleCommandTarget*>(this);
	}
	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}
	reinterpret_cast<IUnknown*>(*ppvObj)->AddRef();

	return S_OK;
}

/* 
 * IUnknown AddRef
 */
ULONG __stdcall CCalcView::AddRef()
{
	return (InterlockedIncrement(&m_nRef));
}

/* 
 * IUnknown Release
 */
ULONG __stdcall CCalcView::Release()
{
	if (InterlockedDecrement(&m_nRef) == 0)
	{
		delete this;
		return 0;
	}

	return m_nRef;	
}

// IOleWindow Implementation

HRESULT __stdcall CCalcView::GetWindow(HWND * lphwnd)
{
	*lphwnd = m_hMainWnd;
	return S_OK;
}

HRESULT __stdcall CCalcView::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}

// IOleCommandTarget implementation
STDMETHODIMP CCalcView::QueryStatus(  const GUID *pguidCmdGroup, 
                                       ULONG cCmds, 
                                       OLECMD prgCmds[], 
                                       OLECMDTEXT *pCmdText)
{
ULONG i;

//only process the commands for our command group
if(pguidCmdGroup && (*pguidCmdGroup != CLSID_HPCommShell))
   return OLECMDERR_E_UNKNOWNGROUP;

//make sure prgCmds is not NULL
if(!prgCmds)
   return E_POINTER;

//run through all of the commands and supply the correct information
for(i = 0; i < cCmds;i++)
   {
   /*switch(prgCmds[i].cmdID)
      {
      case IDM_VIEW_KEYS:
         prgCmds[i].cmdf = OLECMDF_SUPPORTED | OLECMDF_ENABLED;
         break;
      }*/
   }

return S_OK;
}

/**************************************************************************

   CShellView::Exec()
   
**************************************************************************/

STDMETHODIMP CCalcView::Exec(   const GUID *pguidCmdGroup, 
                                 DWORD nCmdID, 
                                 DWORD nCmdExecOpt, 
                                 VARIANTARG *pvaIn, 
                                 VARIANTARG *pvaOut)
{
//only process the commands for our command group
if(pguidCmdGroup && (*pguidCmdGroup == CLSID_CmdGrp))
   {
   OnCommand(nCmdID, 0, NULL);
   return S_OK;
   }

return OLECMDERR_E_UNKNOWNGROUP;
}

HRESULT __stdcall CCalcView::TranslateAccelerator(LPMSG lpmsg)
{
	return E_NOTIMPL;
}

#ifdef _FIX_ENABLEMODELESS_CONFLICT
	HRESULT __stdcall CCalcView::EnableModelessSV(BOOL fEnable)
	{
		return E_NOTIMPL;

	}
#else
	HRESULT __stdcall CCalcView::EnableModeless(BOOL fEnable)
	{
		return E_NOTIMPL;
	}
#endif

HRESULT __stdcall CCalcView::UIActivate(UINT uState)
{
	//don't do anything if the state isn't really changing
	if(m_uState == uState)
	   return S_OK;

	//OnActivate handles the menu merging and internal state
	OnActivate(uState);

	//remove the docking window
	if(FALSE)
	   AddRemoveDockingWindow(TRUE);

	//only do this if we are active
	if(uState != SVUIA_DEACTIVATE) {
		TCHAR szName[MAX_PATH];
		LRESULT  lResult;
		int      nPartArray[1] = {-1};

		//update the status bar
		LoadString(global_hModule, IDS_TITLE, szName, sizeof(szName));

		//m_pSFParent->GetFolderPath(szName + lstrlen(szName), sizeof(szName) - lstrlen(szName));

		//set the number of parts
		m_pShellBrowser->SendControlMsg(FCW_STATUS, SB_SETPARTS, 1, (LPARAM)nPartArray, &lResult);

		//set the text for the parts
		m_pShellBrowser->SendControlMsg(FCW_STATUS, SB_SETTEXT, 0, (LPARAM)szName, &lResult);

		//add the docking window if necessary
		AddRemoveDockingWindow(TRUE);
	}

	return S_OK;

/*	switch (uState)
	{
		case SVUIA_DEACTIVATE:
			break;
		case SVUIA_ACTIVATE_NOFOCUS:
			break;
		case SVUIA_ACTIVATE_FOCUS:
			break;
		case SVUIA_INPLACEACTIVATE:
			break;
		default:
			break;
	}
	return S_OK;*/
}

HRESULT __stdcall CCalcView::Refresh()
{
	//empty the list
	ListView_DeleteAllItems(m_hWndListView);

	//refill the list
	FillList();

	return S_OK;
}

HRESULT __stdcall CCalcView::CreateViewWindow(IShellView  *lpPrevView, LPCFOLDERSETTINGS lpfs, IShellBrowser  * psb, RECT * prcView, HWND  *phWnd)
{
	WNDCLASS wc;

	*phWnd = NULL;

	//if our window class has not been registered, then do so
	if(!GetClassInfo(global_hModule, NS_CLASS_NAME, &wc))
	   {
	   ZeroMemory(&wc, sizeof(wc));
	   wc.style          = CS_HREDRAW | CS_VREDRAW;
	   wc.lpfnWndProc    = (WNDPROC)WndProc;
	   wc.cbClsExtra     = 0;
	   wc.cbWndExtra     = 0;
	   wc.hInstance      = global_hModule;
	   wc.hIcon          = NULL;
	   wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
	   wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
	   wc.lpszMenuName   = NULL;
	   wc.lpszClassName  = NS_CLASS_NAME;
   
	   if(!RegisterClass(&wc))
		  return E_FAIL;
	   }

	//set up the member variables
	m_pShellBrowser = psb;
	m_FolderSettings = *lpfs;

	//get our parent window
	m_pShellBrowser->GetWindow(&m_hwndParent);

	*phWnd = CreateWindowEx(   0,
							   NS_CLASS_NAME,
							   NULL,
							   WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
							   prcView->left,
							   prcView->top,
							   prcView->right - prcView->left,
							   prcView->bottom - prcView->top,
							   m_hwndParent,
							   NULL,
							   global_hModule,
							   (LPVOID)this);
                           
	if(!*phWnd)
	   return E_FAIL;

	m_pShellBrowser->AddRef();

	return S_OK;
}

HRESULT __stdcall CCalcView::DestroyViewWindow()
{
	//Make absolutely sure all our UI is cleaned up.
	UIActivate(SVUIA_DEACTIVATE);

	if(m_hMenu)
	   DestroyMenu(m_hMenu);

	if (m_hMainWnd)
	{
		DestroyWindow(m_hMainWnd);
		m_hMainWnd = NULL;
	}



	//release the shell browser object
	m_pShellBrowser->Release();

	return S_OK;
}

HRESULT __stdcall CCalcView::GetCurrentInfo(LPFOLDERSETTINGS lpfs)
{
	*lpfs = m_FolderSettings;
	return S_OK;
}

HRESULT __stdcall CCalcView::AddPropertySheetPages(DWORD dwReserved,
						LPFNADDPROPSHEETPAGE lpfn, LPARAM lparam)
{
	CreatePropertyPage(lpfn, lparam);
	return S_OK;
}

HRESULT __stdcall CCalcView::SaveViewState()
{
	return S_OK;
}
HRESULT __stdcall CCalcView::SelectItem(LPCITEMIDLIST pidlItem, UINT uFlags)
{
	return E_NOTIMPL;
}
HRESULT __stdcall CCalcView::GetItemObject(UINT uItem, REFIID riid, LPVOID *ppv)
{
	*ppv = NULL;
	return E_NOTIMPL;
}

BOOL CCalcView::InitListView()
{
	int nSubItem = 0;

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.iSubItem = nSubItem;
	lvc.cx = 200;
	lvc.pszText = TEXT("Name");
	if (ListView_InsertColumn(m_hWndListView, nSubItem++, &lvc) == -1) {
		return FALSE;
	}
	lvc.fmt = LVCFMT_RIGHT;
	lvc.iSubItem = nSubItem;
	lvc.cx = 100;
	lvc.pszText = TEXT("Size");
	if (ListView_InsertColumn(m_hWndListView, nSubItem++, &lvc) == -1) {
		return FALSE;
	}

	lvc.fmt = LVCFMT_LEFT;
	lvc.iSubItem = nSubItem;
	lvc.cx = 100;
	lvc.pszText = TEXT("Type");
	if (ListView_InsertColumn(m_hWndListView, nSubItem++, &lvc) == -1) {
		return FALSE;
	}


	lvc.fmt = LVCFMT_RIGHT;
	lvc.iSubItem = nSubItem;
	lvc.cx = 100;
	lvc.pszText = TEXT("Checksum");
	if (ListView_InsertColumn(m_hWndListView, nSubItem++, &lvc) == -1) {
		return FALSE;
	}
	
	if (global_hImageListSmall)
		ListView_SetImageList(m_hWndListView, global_hImageListSmall, LVSIL_SMALL);

	if (global_hImageListLarge)
		ListView_SetImageList(m_hWndListView, global_hImageListLarge, LVSIL_NORMAL);

	return TRUE;
}

BOOL CCalcView::AddRemoveDockingWindow(BOOL bAdd)
{
#if (_WIN32_IE < 0x0400)

return FALSE;

#else //(_WIN32_IE < 0x0400)

BOOL              bReturn = FALSE;
HRESULT           hr;
IServiceProvider  *pSP;

//get the browser's IServiceProvider
hr = m_pShellBrowser->QueryInterface((REFIID)IID_IServiceProvider, (LPVOID*)&pSP);
if(SUCCEEDED(hr))
   {
   IDockingWindowFrame *pFrame;

   //get the IDockingWindowFrame pointer
   hr = pSP->QueryService(SID_SShellBrowser, IID_IDockingWindowFrame, (LPVOID*)&pFrame);
   if(SUCCEEDED(hr))
      {
      if(bAdd)
         {
         hr = S_OK;
         if(!m_pDockingWindow)
            {
            //create the toolbar object
            m_pDockingWindow = new CDockingWindow(this, m_hMainWnd);
            }

         if(m_pDockingWindow)
            {
            //add the toolbar object
            hr = pFrame->AddToolbar((IDockingWindow*)m_pDockingWindow, TOOLBAR_ID, 0);

            if(SUCCEEDED(hr))
               {
               bReturn = TRUE;
               }
            }
         }
      else
         {
         if(m_pDockingWindow)
            {
            hr = pFrame->RemoveToolbar((IDockingWindow*)m_pDockingWindow, DWFRF_NORMAL);

            if(SUCCEEDED(hr))
               {
               /*
               RemoveToolbar should release the toolbar object which will cause 
               it to destroy itself. Our toolbar object is no longer valid at 
               this point.
               */
               m_pDockingWindow = NULL;
               bReturn = TRUE;
               }
            }
         }
      pFrame->Release();
      }
   pSP->Release();
   }

return bReturn;
#endif   //(_WIN32_IE < 0x0400)
}


void CCalcView::FillList()
{
	LPENUMIDLIST   pEnumIDList;

	if(SUCCEEDED(m_pSFParent->EnumObjects(m_hMainWnd, SHCONTF_NONFOLDERS | SHCONTF_FOLDERS, &pEnumIDList)))
	{
		LPITEMIDLIST   pidl;
		DWORD          dwFetched;

		//turn the listview's redrawing off
		SendMessage(m_hWndListView, WM_SETREDRAW, FALSE, 0);

		while((S_OK == pEnumIDList->Next(1, &pidl, &dwFetched)) && dwFetched) {
			LV_ITEM  lvItem;

			ZeroMemory(&lvItem, sizeof(lvItem));

			//set the mask
			lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;

			//add the item to the end of the list
			lvItem.iItem = ListView_GetItemCount(m_hWndListView);

			//set the item's data
			lvItem.lParam = (LPARAM)m_pCalcPidl->CopyItem(pidl);
#ifdef _DEBUG
			m_pCalcPidl->TRACE("FillList ");
			m_pCalcPidl->TRACEPIDL((LPITEMIDLIST)lvItem.lParam);
			LPCALCITEM pItem = m_pCalcPidl->GetPidlData((LPITEMIDLIST)lvItem.lParam);
			pItem->type;
#endif
			//lvItem.lParam = (LPARAM)pidl;

			//get text on a callback basis
			lvItem.pszText = LPSTR_TEXTCALLBACK;

			//get the image on a callback basis
			lvItem.iImage = I_IMAGECALLBACK;

			//add the item
			ListView_InsertItem(m_hWndListView, &lvItem);
		}

		//sort the items
		SortList();

		//turn the listview's redrawing back on and force it to draw
		SendMessage(m_hWndListView, WM_SETREDRAW, TRUE, 0);
		InvalidateRect(m_hWndListView, NULL, TRUE);
		UpdateWindow(m_hWndListView);

		pEnumIDList->Release();
	}
}

LRESULT CALLBACK CCalcView::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	CCalcView  *pThis = (CCalcView*)GetWindowLong(hWnd, GWL_USERDATA);

	switch (uMessage) {
		case WM_NCCREATE:
		  {
		  LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
		  pThis = (CCalcView*)(lpcs->lpCreateParams);
		  SetWindowLong(hWnd, GWL_USERDATA, (LONG)pThis);

		  //set the window handle
		  pThis->m_hMainWnd = hWnd;
		  }
		  break;

		case WM_SIZE:
		  return pThis->OnSize(LOWORD(lParam), HIWORD(lParam));

		case WM_CREATE:
		  return pThis->OnCreate();

		case WM_SETFOCUS:
		  return pThis->OnSetFocus();

		case WM_KILLFOCUS:
		  return pThis->OnKillFocus();

		case WM_ACTIVATE:
		  return pThis->OnActivate(SVUIA_ACTIVATE_FOCUS);

		/*case WM_COMMAND:
		  return pThis->OnCommand(   GET_WM_COMMAND_ID(wParam, lParam), 
									 GET_WM_COMMAND_CMD(wParam, lParam), 
									 GET_WM_COMMAND_HWND(wParam, lParam));*/

		case WM_INITMENUPOPUP:
		  return pThis->UpdateMenu((HMENU)wParam);

		case WM_NOTIFY:
		  return pThis->OnNotify((UINT)wParam, (LPNMHDR)lParam);

		case WM_SETTINGCHANGE:
		  return pThis->OnSettingChange((LPCTSTR)lParam);
	}

	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}


LRESULT CCalcView::OnNotify(UINT CtlID, LPNMHDR lpnmh)
{
	switch(lpnmh->code) {
	case NM_SETFOCUS:
		OnSetFocus();
		break;
	
	case NM_KILLFOCUS:
		OnDeactivate();
		break;

	case HDN_ENDTRACK: {
	/*      g_nColumn1 = ListView_GetColumnWidth(m_hwndList, 0);
		  g_nColumn2 = ListView_GetColumnWidth(m_hwndList, 1);*/
		return 0;
	}
	case HDN_ITEMCLICK: {
		HD_NOTIFY *phdn = (HD_NOTIFY *)lpnmh;
		if (m_nSortColumn == phdn->iItem) {
			m_bSortAscending = !m_bSortAscending;
		}
		else {
			m_nSortColumn = phdn->iItem;
		}

		SortList();
		return 0;
	}
	case LVN_DELETEITEM: {
		NM_LISTVIEW *lpnmlv = (NM_LISTVIEW*)lpnmh;
		//delete the pidl because we made a copy of it
		m_pCalcPidl->Delete((LPITEMIDLIST)lpnmlv->lParam);
		break;
	}

	#ifdef LVN_ITEMACTIVATE
   
	case LVN_ITEMACTIVATE:

	#else    //LVN_ITEMACTIVATE

	case NM_DBLCLK:
	case NM_RETURN:

	#endif   //LVN_ITEMACTIVATE
		{
		LV_ITEM        lvItem;

		ZeroMemory(&lvItem, sizeof(lvItem));
		lvItem.mask = LVIF_PARAM;

	#ifdef LVN_ITEMACTIVATE

		LPNMLISTVIEW   lpnmlv = (LPNMLISTVIEW)lpnmh;
		lvItem.iItem = lpnmlv->iItem;

	#else    //LVN_ITEMACTIVATE

		lvItem.iItem = -1;

		//need to find the selected item
		{
		int   i;

		for(i = 0; i < ListView_GetItemCount(m_hwndList); i++) {
			if(ListView_GetItemState(m_hwndList, i, LVIS_SELECTED)) {
				lvItem.iItem = i;
				break;
			}
		}
		//we didn't find a selected item
		if(-1 == lvItem.iItem)
			return 0;
		}

	#endif   //LVN_ITEMACTIVATE

		if(ListView_GetItem(m_hWndListView, &lvItem)) {
			 //only allow directories to be activated
			if (m_pCalcPidl->IsDirectory((LPITEMIDLIST)lvItem.lParam)) {
				m_pShellBrowser->BrowseObject((LPITEMIDLIST)lvItem.lParam, SBSP_DEFBROWSER | SBSP_RELATIVE);
			}
		}

		return 0;
	}
   
	case LVN_GETDISPINFO: {
		LV_DISPINFO    *lpdi = (LV_DISPINFO *)lpnmh;
		LPITEMIDLIST   pidl = (LPITEMIDLIST)lpdi->item.lParam;

		//is the sub-item information being requested?
		if(lpdi->item.iSubItem) {
			if (pidl == NULL) {
				return 0;
			}
			//is the text being requested?
			if(lpdi->item.mask & LVIF_TEXT) {
				//is this a value or a key?
				LPCALCITEM pItem = m_pCalcPidl->GetPidlData(pidl);
				switch(lpdi->item.iSubItem) {
				case 1:
					sprintf(lpdi->item.pszText, "%d", pItem->nSize);
					break;
				case 2:
					lstrcpyn(lpdi->item.pszText, pItem->sType, lpdi->item.cchTextMax);
					break;
				case 3:
					sprintf(lpdi->item.pszText, "%d", pItem->nCRC);
					break;
				}
			}
		}
		  //the item text is being requested
		else {
			//is the text being requested?
			if(lpdi->item.mask & LVIF_TEXT) {
				STRRET   str;

				if(SUCCEEDED(m_pSFParent->GetDisplayNameOf(pidl, SHGDN_NORMAL | SHGDN_INFOLDER, &str))) {
					if(STRRET_WSTR == str.uType) {
						  WideCharToLocal(lpdi->item.pszText, str.pOleStr, lpdi->item.cchTextMax);

						  //delete the string buffer
						  m_pMalloc->Free(str.pOleStr);
					}
					else if (STRRET_CSTR == str.uType) {
						lstrcpyn(lpdi->item.pszText, str.cStr, lpdi->item.cchTextMax);
					}

				}
			}

			//is the image being requested?
			if(lpdi->item.mask & LVIF_IMAGE) {
				IExtractIcon   *pei;

				if(SUCCEEDED(m_pSFParent->GetUIObjectOf(m_hMainWnd, 1, (LPCITEMIDLIST*)&pidl, IID_IExtractIcon, NULL, (LPVOID*)&pei))) {
					UINT  uFlags;

					//GetIconLoaction will give us the index into our image list
					pei->GetIconLocation(GIL_FORSHELL, NULL, 0, &lpdi->item.iImage, &uFlags);

					pei->Release();
				}
			}
		}
	}
	return 0;
	
	}

	return 0;
}


LRESULT CCalcView::OnSize(WORD wWidth, WORD wHeight)
{
	//resize the ListView to fit our window
	if(m_hWndListView) {
		MoveWindow(m_hWndListView, 0, 0, wWidth, wHeight, TRUE);
	}

	return 0;
}

LRESULT CCalcView::OnCreate(void)
{
	//create the ListView
	if(CreateList()) {
	   if(InitListView()) {
		  FillList();
		}
	}

	return 0;
}

BOOL CCalcView::CreateList(void)
{
	DWORD dwStyle;

	dwStyle = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_SINGLESEL | LVS_REPORT | LVS_SHAREIMAGELISTS;

	m_hWndListView = CreateWindowEx(WS_EX_CLIENTEDGE,
									 WC_LISTVIEW,
									 NULL,
									 dwStyle,
									 0,
									 0,
									 0,
									 0,
									 m_hMainWnd,
									 (HMENU)ID_LISTVIEW,
									 global_hModule,
									 NULL);

	if(!m_hWndListView)
	   return FALSE;

	UpdateShellSettings();

	return TRUE;
}

/**************************************************************************

   CShellView::UpdateShellSettings()
   
**************************************************************************/

typedef void (WINAPI *PFNSHGETSETTINGSPROC)(LPSHELLFLAGSTATE lpsfs, DWORD dwMask);

void CCalcView::UpdateShellSettings(void)
{
	#if (_WIN32_IE >= 0x0400)
	SHELLFLAGSTATE       sfs;
	HINSTANCE            hinstShell32;

	/*
	Since SHGetSettings is not implemented in all versions of the shell, get the 
	function address manually at run time. This allows the extension to run on all 
	platforms.
	*/

	ZeroMemory(&sfs, sizeof(sfs));

	/*
	The default, in case any of the following steps fails, is classic Windows 95 
	style.
	*/
	sfs.fWin95Classic = TRUE;

	hinstShell32 = LoadLibrary(TEXT("shell32.dll"));
	if(hinstShell32)
	   {
	   PFNSHGETSETTINGSPROC pfnSHGetSettings;

	   pfnSHGetSettings = (PFNSHGETSETTINGSPROC)GetProcAddress(hinstShell32, TEXT("SHGetSettings"));
	   if(pfnSHGetSettings)
		  {
		  (*pfnSHGetSettings)(&sfs, SSF_DOUBLECLICKINWEBVIEW | SSF_WIN95CLASSIC);
		  }
	   FreeLibrary(hinstShell32);
	   }

	DWORD dwExStyles = 0;

	if(!sfs.fWin95Classic && !sfs.fDoubleClickInWebView)
	   dwExStyles |= LVS_EX_ONECLICKACTIVATE | 
					  LVS_EX_TRACKSELECT | 
					  LVS_EX_UNDERLINEHOT;

	ListView_SetExtendedListViewStyle(m_hWndListView, dwExStyles);

	#endif   //(_WIN32_IE >= 0x0400)
}


LRESULT CCalcView::OnSetFocus(void)
{
	/*
	Tell the browser one of our windows has received the focus. This should always 
	be done before merging menus (OnActivate merges the menus) if one of our 
	windows has the focus.
	*/
	m_pShellBrowser->OnViewWindowActive(this);

	OnActivate(SVUIA_ACTIVATE_FOCUS);

	return 0;
}


LRESULT CCalcView::OnKillFocus(void)
{
	OnActivate(SVUIA_ACTIVATE_NOFOCUS);

	return 0;
}


LRESULT CCalcView::OnCommand(DWORD dwCmdID, DWORD dwCmd, HWND hwndCmd)
{
/*switch(dwCmdID)
   {
   case IDM_VIEW_KEYS:
      g_bViewKeys = ! g_bViewKeys;
      Refresh();
      break;

   case IDM_VIEW_IDW:
      g_bShowIDW = ! g_bShowIDW;
      AddRemoveDockingWindow(g_bShowIDW);
      break;
   
   case IDM_MYFILEITEM:
      MessageBeep(MB_OK);
      break;
   }*/

return 0;
}

LRESULT CCalcView::UpdateMenu(HMENU hMenu)
{
/*	CheckMenuItem(hMenu, IDM_VIEW_KEYS, MF_BYCOMMAND | (g_bViewKeys ? MF_CHECKED: MF_UNCHECKED));

	if(CanDoIDockingWindow()) {
		EnableMenuItem(hMenu, IDM_VIEW_IDW, MF_BYCOMMAND | MF_ENABLED);
		CheckMenuItem(hMenu, IDM_VIEW_IDW, MF_BYCOMMAND | (g_bShowIDW ? MF_CHECKED: MF_UNCHECKED));
	}
	else {
		EnableMenuItem(hMenu, IDM_VIEW_IDW, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		CheckMenuItem(hMenu, IDM_VIEW_IDW, MF_BYCOMMAND | MF_UNCHECKED);
	}*/

	return 0;
}

LRESULT CCalcView::OnActivate(UINT uState)
{
//don't do anything if the state isn't really changing
if(m_uState == uState)
   return S_OK;

OnDeactivate();

//only do this if we are active
if(uState != SVUIA_DEACTIVATE)
   {
   //merge the menus
   m_hMenu = CreateMenu();
   
   if(m_hMenu)
      {
   	OLEMENUGROUPWIDTHS   omw = {0, 0, 0, 0, 0, 0};
      MENUITEMINFO         mii;
      TCHAR                szText[MAX_PATH];

      m_pShellBrowser->InsertMenusSB(m_hMenu, &omw);

      //build the top level menu
      //get the menu item's text
      LoadString(global_hModule, IDS_MI_HPCOMMSHELL, szText, sizeof(szText));
      
      ZeroMemory(&mii, sizeof(mii));
      mii.cbSize = sizeof(mii);
      mii.fMask = MIIM_SUBMENU | MIIM_TYPE | MIIM_STATE;
      mii.fType = MFT_STRING;
      mii.fState = MFS_ENABLED;
      mii.dwTypeData = szText;
      mii.hSubMenu = BuildRegistryMenu();

      //insert our menu into the menu bar
      if(mii.hSubMenu)
         {
         InsertMenuItem(m_hMenu, FCIDM_MENU_HELP, FALSE, &mii);
         }

      //get the view menu so we can merge with it
      ZeroMemory(&mii, sizeof(mii));
      mii.cbSize = sizeof(mii);
      mii.fMask = MIIM_SUBMENU;
      
      if(GetMenuItemInfo(m_hMenu, FCIDM_MENU_VIEW, FALSE, &mii))
         {
         //MergeViewMenu(mii.hSubMenu);
         }

      //add the items that should only be added if we have the focus
      if(SVUIA_ACTIVATE_FOCUS == uState)
         {
         //get the file menu so we can merge with it
         ZeroMemory(&mii, sizeof(mii));
         mii.cbSize = sizeof(mii);
         mii.fMask = MIIM_SUBMENU;
      
         if(GetMenuItemInfo(m_hMenu, FCIDM_MENU_FILE, FALSE, &mii))
            {
            //MergeFileMenu(mii.hSubMenu);
            }
         }

      m_pShellBrowser->SetMenuSB(m_hMenu, NULL, m_hMainWnd);
      }
   }

m_uState = uState;

return 0;
}

HMENU CCalcView::BuildRegistryMenu(void)
{
	HMENU hSubMenu = CreatePopupMenu();

	if(hSubMenu)
	   {
/*	   TCHAR          szText[MAX_PATH];
	   MENUITEMINFO   mii;
	   int            nTools,
					  i;
	   //get the number of items in our global array
	   for(nTools = 0; g_Tools[nTools].idCommand != -1; nTools++)
		  {
		  }

	   //add the menu items
	   for(i = 0; i < nTools; i++)
		  {
		  LoadString(g_hInst, g_Tools[i].idMenuString, szText, sizeof(szText));
      
		  ZeroMemory(&mii, sizeof(mii));
		  mii.cbSize = sizeof(mii);
		  mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;

		  if(TBSTYLE_SEP != g_Tools[i].bStyle)
			 {
			 mii.fType = MFT_STRING;
			 mii.fState = MFS_ENABLED;
			 mii.dwTypeData = szText;
			 mii.wID = g_Tools[i].idCommand;
			 }
		  else
			 {
			 mii.fType = MFT_SEPARATOR;
			 }
      
		  //tack this item onto the end of the menu
		  InsertMenuItem(hSubMenu, (UINT)-1, TRUE, &mii);
		  }
	   }*/
	}
	return hSubMenu;
}



/**************************************************************************

   CShellView::OnDeactivate()
   
**************************************************************************/

void CCalcView::OnDeactivate(void)
{
	if(m_uState != SVUIA_DEACTIVATE) {
		if(m_hMenu) {
			m_pShellBrowser->SetMenuSB(NULL, NULL, NULL);

			m_pShellBrowser->RemoveMenusSB(m_hMenu);

			DestroyMenu(m_hMenu);

			m_hMenu = NULL;
		}

		m_uState = SVUIA_DEACTIVATE;
	}
}

/**************************************************************************

   CShellView::MergeFileMenu()
   
**************************************************************************/

/*void CCalcView::MergeFileMenu(HMENU hSubMenu)
{
	if(hSubMenu) {
		MENUITEMINFO   mii;
		TCHAR          szText[MAX_PATH];

		ZeroMemory(&mii, sizeof(mii));

		//add a separator
		mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;
		mii.fType = MFT_SEPARATOR;
		mii.fState = MFS_ENABLED;

		//insert this item at the beginning of the menu
		InsertMenuItem(hSubMenu, 0, TRUE, &mii);

		//add the file menu items
		LoadString(g_hInst, IDS_MI_FILEITEM, szText, sizeof(szText));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;
		mii.fType = MFT_STRING;
		mii.fState = MFS_ENABLED;
		mii.dwTypeData = szText;
		mii.wID = IDM_MYFILEITEM;

		//insert this item at the beginning of the menu
		InsertMenuItem(hSubMenu, 0, TRUE, &mii);
	}
}


void CCalcView::MergeViewMenu(HMENU hSubMenu)
{
	if(hSubMenu) {
		MENUITEMINFO   mii;
		TCHAR          szText[MAX_PATH];

		ZeroMemory(&mii, sizeof(mii));

		//add a separator at the correct position in the menu
		mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;
		mii.fType = MFT_SEPARATOR;
		mii.fState = MFS_ENABLED;
		InsertMenuItem(hSubMenu, FCIDM_MENU_VIEW_SEP_OPTIONS, FALSE, &mii);

		//add the view menu items at the correct position in the menu
		LoadString(g_hInst, IDS_MI_VIEW_KEYS, szText, sizeof(szText));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;
		mii.fType = MFT_STRING;
		mii.fState = MFS_ENABLED;
		mii.dwTypeData = szText;
		mii.wID = IDM_VIEW_KEYS;
		InsertMenuItem(hSubMenu, FCIDM_MENU_VIEW_SEP_OPTIONS, FALSE, &mii);
	}
}*/

LRESULT CCalcView::OnSettingChange(LPCTSTR lpszSection)
{
//if(0 == lstrcmpi(lpszSection, TEXT("ShellState")))
   {
   UpdateShellSettings();
   return 0;
   }

	return 0;
}

int CALLBACK CCalcView::CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lpData)
{
	CCalcFolder * pFolder = (CCalcFolder *)lpData;
	if(pFolder == NULL)
		return 0;

	int nRet = (int)pFolder->CompareIDs(m_nSortColumn, (LPITEMIDLIST)lParam1, (LPITEMIDLIST)lParam2);
	if (!m_bSortAscending) {
		nRet = -nRet;
	}

	return nRet;
}


void CCalcView::SortList()
{
	ListView_SortItems(m_hWndListView, CompareItems, (LPARAM)m_pSFParent);
}
