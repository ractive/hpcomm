// CalcEnum.h: interface for the CCalcEnum class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALCENUM_H__0F3E6E65_E2BF_4C60_823A_9A14E10A2B1F__INCLUDED_)
#define AFX_CALCENUM_H__0F3E6E65_E2BF_4C60_823A_9A14E10A2B1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCalcPidl;
class CEnumProxy;

class CCalcEnum : public IEnumIDList  
{
public:
	CCalcEnum(HRESULT *pResult, DWORD dwFlags, LPCITEMIDLIST pidl);
	virtual ~CCalcEnum();

	// IUnknown
	virtual HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();

// Operations
public:
	    virtual HRESULT __stdcall Next(
			ULONG celt,LPITEMIDLIST *rgelt, ULONG *pceltFetched);
		virtual HRESULT __stdcall Skip(ULONG celt);
		virtual HRESULT __stdcall Reset();
		virtual HRESULT __stdcall Clone(IEnumIDList **ppenum);
private:
	DWORD m_dwFlags;
	long m_nRef;
	CCalcPidl * m_pCalcPidl;
	LPMALLOC m_pMalloc;
};

#endif // !defined(AFX_CALCENUM_H__0F3E6E65_E2BF_4C60_823A_9A14E10A2B1F__INCLUDED_)
