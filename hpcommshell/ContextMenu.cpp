// ContextMenu.cpp: implementation of the CContextMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ContextMenu.h"
#include "CalcView.h"

extern long global_nComponents;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContextMenu::CContextMenu(CCalcFolder * pCalcFolder)
{
	m_pCalcFolder = pCalcFolder;
	InterlockedIncrement(&global_nComponents);
	m_nRef = 1;
}

CContextMenu::~CContextMenu()
{
	InterlockedDecrement(&global_nComponents);
}

/* 
 * IUnknown QueryInterface
 */
HRESULT __stdcall CContextMenu::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>((IContextMenu *) this);
	}
	else if (riid == IID_IContextMenu)
	{
		*ppvObj = static_cast<IContextMenu*>(this);
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
ULONG __stdcall CContextMenu::AddRef()
{
	return (InterlockedIncrement(&m_nRef));
}

/* 
 * IUnknown Release
 */
ULONG __stdcall CContextMenu::Release()
{
	if (InterlockedDecrement(&m_nRef) == 0)
	{
		delete this;
		return 0;
	}
	return m_nRef;	
}

HRESULT __stdcall CContextMenu::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlag)
{
	MENUITEMINFO mii;
	mii.cbSize = sizeof(mii);

	mii.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID;
	mii.fType = MFT_STRING;
	mii.fState = MF_ENABLED;
	mii.wID = idCmdFirst+10;
	mii.dwTypeData = "My First Command";
	InsertMenuItem(hmenu, mii.wID, FALSE, &mii);
	mii.wID = idCmdFirst+11;
	mii.dwTypeData = "My Second Command";
	InsertMenuItem(hmenu, mii.wID, FALSE, &mii);

	return ResultFromScode(MAKE_SCODE(SEVERITY_SUCCESS, 0, (USHORT)(mii.wID - idCmdFirst)));
}


HRESULT __stdcall CContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO lpici)
{
	short action = LOWORD(lpici->lpVerb);
	if (action == 10)
		MessageBox(NULL, "Action 10", "ContextMenu Action 1 Invoked", MB_OK);
	else if (action == 11)
		MessageBox(NULL, "Action 11", "ContextMenu Action 2 Invoked", MB_OK);
	/*else
		// Unexpected action*/

	return S_FALSE;
}

HRESULT __stdcall CContextMenu::GetCommandString(UINT idCmd, UINT uType, UINT *pwReserved, LPSTR pszName, UINT cchMax)
{
	switch (idCmd)
	{
		case 10: 
			if (uType == GCS_VERB)
				strcpy(pszName, "First");
			else
				strcpy(pszName, "Status string for first context menu entry");
			break;
		case 11:
			if (uType == GCS_VERB)
				strcpy(pszName, "Second");
			else
				strcpy(pszName, "Status string for second context menu entry");
			break;
		default:
			return E_NOTIMPL;
	}
	
	return S_OK;
}
