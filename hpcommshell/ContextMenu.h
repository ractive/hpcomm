// ContextMenu.h: interface for the CContextMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTEXTMENU_H__9F395427_B0E6_4403_AC59_94B3CC55912A__INCLUDED_)
#define AFX_CONTEXTMENU_H__9F395427_B0E6_4403_AC59_94B3CC55912A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCalcFolder;

class CContextMenu : public IContextMenu  
{
public:
	CContextMenu(CCalcFolder * pCalcFolder);
	virtual ~CContextMenu();

	// IUnknown
	virtual HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();

// Operations
public:
	virtual HRESULT __stdcall QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
	virtual HRESULT __stdcall InvokeCommand(LPCMINVOKECOMMANDINFO lpici);
	virtual HRESULT __stdcall GetCommandString(UINT idCmd, UINT uType, UINT *pwReserved, LPSTR pszName, UINT cchMax);

private:
	long m_nRef;
	CCalcFolder *m_pCalcFolder;
};

#endif // !defined(AFX_CONTEXTMENU_H__9F395427_B0E6_4403_AC59_94B3CC55912A__INCLUDED_)
