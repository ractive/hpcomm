// CalcFolder.cpp: implementation of the CCalcFolder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CalcFolder.h"
#include "CalcEnum.h"
#include "CalcView.h"
#include "CalcPidl.h"
#include "ContextMenu.h"
#include "ExtractIcon.h"
#include "CalcPidl.h"

extern long global_nComponents;
extern long global_nLockCount;
extern HIMAGELIST global_hImageListSmall;
extern HIMAGELIST global_hImageListLarge;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCalcFolder::CCalcFolder(LPCITEMIDLIST pidl)
{
	m_CalcPidlMgr = new CCalcPidl;
	if (m_CalcPidlMgr == NULL) {
		delete this;
		return;
	}
	InterlockedIncrement(&global_nComponents);
	m_pidl = m_CalcPidlMgr->CopyItem(pidl);
	m_nRef = 1;
}

CCalcFolder::~CCalcFolder()
{
	if(m_pidl) {
	   m_CalcPidlMgr->Delete(m_pidl);
	   m_pidl = NULL;
	}

	if(m_CalcPidlMgr) {
		delete m_CalcPidlMgr;
	}

	InterlockedDecrement(&global_nLockCount);
}

HRESULT __stdcall CCalcFolder::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>((IShellFolder *) this);
	}
	else if (riid == IID_IShellFolder)
	{
		*ppvObj = static_cast<IShellFolder*>(this);
	}
	else if (riid == IID_IPersistFolder)
	{
		*ppvObj = static_cast<IPersistFolder*>(this);
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
ULONG __stdcall CCalcFolder::AddRef()
{
	return InterlockedIncrement(&m_nRef);
}

/* 
 * IUnknown Release
 */
ULONG __stdcall CCalcFolder::Release()
{
	if (InterlockedDecrement(&m_nRef) == 0)
	{
		delete this;
		return 0;
	}

	return m_nRef;	
}

// IParseDisplayName
HRESULT __stdcall CCalcFolder::ParseDisplayName(HWND hwndOwner,	LPBC pbc, LPOLESTR lpszDisplayName,	ULONG *pchEaten, LPITEMIDLIST *ppidl, ULONG *pdwAttributes)
{
	return E_NOTIMPL;
}

// IOleContainer
HRESULT __stdcall CCalcFolder::EnumObjects(HWND hwnd, DWORD flags, LPENUMIDLIST *ppenumIDList)
{
	HRESULT hRes;
	CCalcEnum* pEnumObj = new CCalcEnum(&hRes, flags, m_pidl);
	if (FAILED(hRes)) {
		return hRes;
	}
	hRes = pEnumObj->QueryInterface(IID_IEnumIDList, (LPVOID*) ppenumIDList);
	pEnumObj->Release();
	return hRes;
}

// IShellFolder
HRESULT __stdcall CCalcFolder::BindToObject(LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, LPVOID *ppvObj)
{
	HRESULT hRes = E_NOTIMPL;
	CCalcFolder *pFolderObj=NULL;

	if (riid == IID_IShellFolder)
	{
#ifdef _DEBUG
		CCalcPidl p;
		p.TRACE("BindToObject: ");
		p.TRACEPIDL((LPITEMIDLIST)pidl);
#endif
		pFolderObj = new CCalcFolder(pidl);
		hRes = pFolderObj->QueryInterface(IID_IShellFolder, ppvObj);
		pFolderObj->Release();
	}

	return hRes;
}

HRESULT __stdcall CCalcFolder::BindToStorage(LPCITEMIDLIST pidl, LPBC pbc, REFIID riid, LPVOID *ppvObj)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CCalcFolder::CompareIDs(LPARAM lParam, LPCITEMIDLIST pidl1,	LPCITEMIDLIST pidl2)
{
	LPCALCITEM pItem1 = m_CalcPidlMgr->GetPidlData(pidl1);
	LPCALCITEM pItem2 = m_CalcPidlMgr->GetPidlData(pidl2);
	// If one item is a directory it's always above a file
	if (pItem1->bIsDir != pItem2->bIsDir) {
		return !pItem1->bIsDir ? 1 : -1;
	}
	UINT nComp1 = 0;
	UINT nComp2 = 0;
	switch(lParam) {
		// Sort the names
	case 0:
		return lstrcmpi(pItem1->sName, pItem2->sName);
	// Sort the size and CRC
	case 1:
		nComp1 = pItem1->nSize;
		nComp2 = pItem2->nSize;
		break;
	case 2:
		return lstrcmpi(pItem1->sType, pItem2->sType);
	case 3:
		nComp1 = pItem1->nCRC;
		nComp2 = pItem2->nCRC;
		break;
	default:
		return E_NOTIMPL;
	}

	if (nComp1 == nComp2) {
		return 0;
	}

	return nComp1 > nComp2 ? 1 : -1;
}

HRESULT __stdcall CCalcFolder::CreateViewObject(HWND hwndOwner, REFIID riid, LPVOID *ppvObj)
{
	HRESULT hRes = E_NOTIMPL;

	if(riid == IID_IShellView)
	{
		CCalcView* pViewObj = new CCalcView(this, m_pidl);
		hRes = pViewObj->QueryInterface(riid, ppvObj);
		pViewObj->Release();
	}
	
	return hRes;
}

HRESULT __stdcall CCalcFolder::GetAttributesOf(UINT cidl, LPCITEMIDLIST *pidl, ULONG *rgfInOut)
{
	*rgfInOut &= SFGAO_CANCOPY | SFGAO_FOLDER | SFGAO_HASSUBFOLDER;

	return S_OK;
}

HRESULT __stdcall CCalcFolder::GetUIObjectOf(HWND hwndOwner, UINT cidl, LPCITEMIDLIST *apidl, REFIID riid, UINT *prgInOut, LPVOID *ppvObj)
{
	if (riid == IID_IExtractIcon) {
		CExtractIcon   *pei;
		//LPITEMIDLIST   pidl;
		CCalcPidl pidlMgr;
		//pidl = pidlMgr.ConcatItem(m_pidl, apidl[0]);

		pei = new CExtractIcon(apidl[0]);

		/*
		The temp PIDL can be deleted because the new CExtractIcon either failed or 
		made its own copy of it.
		*/
		//pidlMgr.Delete(pidl);

		if(pei) {
			*ppvObj = pei;
			return S_OK;
		}

		return E_OUTOFMEMORY;
	}
	else if (riid == IID_IContextMenu) {
		CContextMenu* pContextMenu = new CContextMenu(this);
		HRESULT hRes = pContextMenu->QueryInterface(IID_IContextMenu, (LPVOID*) ppvObj);
		pContextMenu->Release();
		if (hRes != S_OK)
			return hRes;
	}
	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}
	reinterpret_cast<IUnknown*>(*ppvObj)->AddRef();
	return S_OK;

}

HRESULT __stdcall CCalcFolder::GetDisplayNameOf(LPCITEMIDLIST pidl, DWORD uFlags, LPSTRRET lpName)
{
	lpName->uType = STRRET_CSTR;
	LPCALCITEM pItem = NULL;
	if (pidl) {
		pItem = (LPCALCITEM)pidl->mkid.abID;
		if (pItem) {
			lstrcpy(lpName->cStr, pItem->sName);
		}
	}

	return S_OK;
}

HRESULT __stdcall CCalcFolder::SetNameOf(HWND hwnd, LPCITEMIDLIST pidl, LPCOLESTR lpszName, DWORD dwReserved, LPITEMIDLIST *pidlOut)
{
	return E_NOTIMPL;
}

// IPersist
HRESULT __stdcall CCalcFolder::GetClassID(LPCLSID lpClassID)
{
	return E_NOTIMPL;
}
	
// IPersistFolder
HRESULT __stdcall CCalcFolder::Initialize(LPCITEMIDLIST pidl)
{
	CCalcPidl mgr;
	m_ShellPidl = mgr.CopyItem(pidl);
	return S_OK;
}

