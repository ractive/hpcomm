// ExtractIcon.h: interface for the CExtractIcon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTRACTICON_H__47BBE822_8AF0_4DAD_BA93_42F6392BA300__INCLUDED_)
#define AFX_EXTRACTICON_H__47BBE822_8AF0_4DAD_BA93_42F6392BA300__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCalcPidl;

class CExtractIcon : public IExtractIcon  
{
public:
	CExtractIcon(LPCITEMIDLIST pidl);
	virtual ~CExtractIcon();

	//IUnknown methods
   STDMETHOD (QueryInterface) (REFIID riid, LPVOID * ppvObj);
   STDMETHOD_ (ULONG, AddRef) (void);
   STDMETHOD_ (ULONG, Release) (void);

   //IExtractIcon methods
   STDMETHOD (GetIconLocation) (UINT, LPTSTR, UINT, LPINT, LPUINT);
   STDMETHOD (Extract) (LPCTSTR, UINT, HICON*, HICON*, UINT);
private:
	CCalcPidl * m_CalcPidlMgr;
	long m_nRef;
	LPITEMIDLIST m_pidl;
};

#endif // !defined(AFX_EXTRACTICON_H__47BBE822_8AF0_4DAD_BA93_42F6392BA300__INCLUDED_)
