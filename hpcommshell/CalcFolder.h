// CalcFolder.h: interface for the CCalcFolder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALCFOLDER_H__C0945120_9BB4_4158_BCC0_5D480BD4CB88__INCLUDED_)
#define AFX_CALCFOLDER_H__C0945120_9BB4_4158_BCC0_5D480BD4CB88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCalcPidl;

class CCalcFolder : public IPersistFolder, public IShellFolder
{
public:
	CCalcFolder(LPCITEMIDLIST pidl);
	virtual ~CCalcFolder();

	// IUnknown
	virtual HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();

	// IParseDisplayName
	virtual HRESULT __stdcall ParseDisplayName(
		HWND hwndOwner,
		LPBC pbc,
		LPOLESTR lpszDisplayName,
		ULONG *pchEaten,
		LPITEMIDLIST *ppidl,
		ULONG *pdwAttributes);

	// IOleContainer
	virtual HRESULT __stdcall EnumObjects(
		HWND hwnd, 
		DWORD flags, 
		LPENUMIDLIST *ppenumIDList);

	// IShellFolder
	virtual HRESULT __stdcall BindToObject(
		LPCITEMIDLIST pidl,
		LPBC pbc,
		REFIID riid,
		LPVOID *ppvObj
		);
	virtual HRESULT __stdcall BindToStorage(
		LPCITEMIDLIST pidl,
		LPBC pbc,
		REFIID riid,
		LPVOID *ppvObj);
	virtual HRESULT __stdcall CompareIDs(
		LPARAM lParam,
		LPCITEMIDLIST pidl1,
		LPCITEMIDLIST pidl2
		);
	virtual HRESULT __stdcall CreateViewObject(
		HWND hwndOwner,
		REFIID riid,
		LPVOID *ppvObj
		);
	virtual HRESULT __stdcall GetAttributesOf(
		UINT cidl, 
		LPCITEMIDLIST *pidl,
		ULONG *rgfInOut
		);
	virtual HRESULT __stdcall GetUIObjectOf(
		HWND hwndOwner,
		UINT cidl,
		LPCITEMIDLIST *apidl,
		REFIID riid,
		UINT *prgInOut,
		LPVOID *ppvObj
		);
	virtual HRESULT __stdcall GetDisplayNameOf(
		LPCITEMIDLIST pidl,
		DWORD uFlags,
		LPSTRRET lpName
		);
	virtual HRESULT __stdcall SetNameOf(
		HWND hwnd,
		LPCITEMIDLIST pidl,
		LPCOLESTR lpszName,
		DWORD dwReserved,
		LPITEMIDLIST *pidlOut
		);

	// IPersist
	virtual HRESULT __stdcall GetClassID(
		LPCLSID lpClassID
		);
	
	// IPersistFolder
	virtual HRESULT __stdcall Initialize(
		LPCITEMIDLIST pidl
		);

private:
	LPITEMIDLIST m_ShellPidl;
	LPITEMIDLIST m_pidl;
	CCalcPidl * m_CalcPidlMgr;
	long m_nRef;
};

#endif // !defined(AFX_CALCFOLDER_H__C0945120_9BB4_4158_BCC0_5D480BD4CB88__INCLUDED_)
