// CalcEnum.cpp: implementation of the CCalcEnum class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CalcEnum.h"
#include "CalcPidl.h"
#include "CalcComm.h"
#include "EnumProxy.h"

extern long global_nComponents;

CEnumProxy m_EnumLists;


//CEnumProxy * CCalcEnum::m_EnumLists = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCalcEnum::CCalcEnum(HRESULT *pResult, DWORD dwFlags, LPCITEMIDLIST pidl)
{
	if (FAILED(SHGetMalloc(&m_pMalloc))) {
		if(pResult) {
			*pResult = E_OUTOFMEMORY;
		}

		delete this;
		return;
	}

	InterlockedIncrement(&global_nComponents);

	m_nRef = 1;

	m_EnumLists.SetEnumList(pidl);

	m_dwFlags = dwFlags;

	m_pCalcPidl = new CCalcPidl();
	if (pResult) {
		*pResult = S_OK;
	}
}

CCalcEnum::~CCalcEnum()
{
	if (m_pCalcPidl) {
		delete m_pCalcPidl;
	}

	if (m_pMalloc) {
		m_pMalloc->Release();
	}
	
	InterlockedDecrement(&global_nComponents);
}

/* 
 * IUnknown QueryInterface
 */
HRESULT __stdcall CCalcEnum::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
	if (riid == IID_IUnknown)
	{
		*ppvObj = static_cast<IUnknown*>((IEnumIDList *) this);
	}
	else if (riid == IID_IEnumIDList)
	{
		*ppvObj = static_cast<IEnumIDList*>(this);
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
ULONG __stdcall CCalcEnum::AddRef()
{
	return InterlockedIncrement(&m_nRef);
}

/* 
 * IUnknown Release
 */
ULONG __stdcall CCalcEnum::Release()
{
	if (InterlockedDecrement(&m_nRef) == 0)
	{
		delete this;
		return 0;
	}

	return m_nRef;	
}

HRESULT __stdcall CCalcEnum::Next(ULONG dwElements, LPITEMIDLIST *apidl, ULONG *pdwFetched)
{
	DWORD    dwIndex;
	HRESULT  hr = S_OK;

	if(dwElements > 1 && !pdwFetched)
	   return E_INVALIDARG;

	for(dwIndex = 0; dwIndex < dwElements; dwIndex++) {
		//is this the last item in the list?
		LPITEMIDLIST pidl = m_EnumLists.GetNext(m_dwFlags & SHCONTF_FOLDERS && !(m_dwFlags & SHCONTF_NONFOLDERS));
		if(!pidl) {
			hr =  S_FALSE;
			break;
		}
#ifdef _DEBUG
		m_pCalcPidl->TRACE("ENUM->NEXT ");
		m_pCalcPidl->TRACEPIDL(pidl);
		LPCALCITEM pItem = m_pCalcPidl->GetPidlData(pidl);
		pItem->type;
#endif
		apidl[dwIndex] = pidl;
	}

	if(pdwFetched) {
	   *pdwFetched = dwIndex;
	}

	return hr;
}

HRESULT __stdcall CCalcEnum::Skip(ULONG celt)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CCalcEnum::Reset()
{
	return E_NOTIMPL;
}

HRESULT __stdcall CCalcEnum::Clone(IEnumIDList **ppenum)
{
	return E_NOTIMPL;
}
