// ExtractIcon.cpp: implementation of the CExtractIcon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExtractIcon.h"
#include "CalcPidl.h"

extern long global_nLockCount;
extern HIMAGELIST global_hImageListLarge;
extern HIMAGELIST global_hImageListSmall;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExtractIcon::CExtractIcon(LPCITEMIDLIST pidl)
{
	m_CalcPidlMgr = new CCalcPidl();
	if(!m_CalcPidlMgr) {
		delete this;
		return;
	}

	m_pidl = m_CalcPidlMgr->CopyItem(pidl);

	m_nRef = 1;

	InterlockedIncrement(&global_nLockCount);
}

CExtractIcon::~CExtractIcon()
{
	if(m_pidl) {
		m_CalcPidlMgr->Delete(m_pidl);
		m_pidl = NULL;
	}

	if(m_CalcPidlMgr)  {
		delete m_CalcPidlMgr;
	}

	InterlockedDecrement(&global_nLockCount);
}

// IExtractIcon
HRESULT __stdcall CExtractIcon::GetIconLocation(UINT uFlags, LPSTR szIconFile, UINT  cchMax, int   *piIndex, UINT  *pwFlags)
{
	//tell the shell to always call Extract
	*pwFlags = GIL_NOTFILENAME;

	LPCALCITEM pItem = NULL;
	if (m_pidl) {
		pItem = (LPCALCITEM)m_pidl->mkid.abID;
		if (pItem) {
			*piIndex = pItem->type;
		}
	}

	/*if(uFlags & GIL_OPENICON) {
		*piIndex = 17;  //tell Extract to return the open icon
	}
	else {
		rand();
		*piIndex = 16;  //tell Extract to return the open icon
	}*/

	return S_OK;
}

HRESULT __stdcall CExtractIcon::Extract(LPCSTR pszFile, UINT nIconIndex, HICON *phiconLarge, HICON *phiconSmall,UINT nIconSize)
{
	*phiconLarge = ImageList_GetIcon(global_hImageListLarge, nIconIndex, ILD_NORMAL );
	*phiconSmall = ImageList_GetIcon(global_hImageListSmall, nIconIndex, ILD_NORMAL );

	return S_FALSE;
}


//////////////////////////////////////////////////////////////////////////
//
// IUnknown Implementation
//

/**************************************************************************

   CExtractIcon::QueryInterface

**************************************************************************/

STDMETHODIMP CExtractIcon::QueryInterface(REFIID riid, LPVOID *ppReturn)
{
	*ppReturn = NULL;

	//IUnknown
	if(IsEqualIID(riid, IID_IUnknown)) {
		*ppReturn = this;
	}

	//IExtractIcon
	else if(IsEqualIID(riid, IID_IExtractIcon)) {
		*ppReturn = (IExtractIcon*)this;
	}

	if(*ppReturn) {
		(*(LPUNKNOWN*)ppReturn)->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}                                             

/**************************************************************************

   CExtractIcon::AddRef

**************************************************************************/

STDMETHODIMP_(DWORD) CExtractIcon::AddRef()
{
	return InterlockedIncrement(&m_nRef);
}

/**************************************************************************

   CExtractIcon::Release

**************************************************************************/

STDMETHODIMP_(DWORD) CExtractIcon::Release()
{
	if(InterlockedDecrement(&m_nRef) == 0) {
		delete this;
		return 0;
	}
   
	return m_nRef;
}
