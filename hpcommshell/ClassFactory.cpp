/* 
 * The Class Factory for the Component
 */

/* 
 * Includes
 */
#include "stdafx.h"
#include "ClassFactory.h"
#include "CalcFolder.h"

/* 
 * Definitions
 */
extern long global_nLockCount;

/* 
 * Constructor
 */
ClassFactory::ClassFactory()
{
	m_nRefCount = 1;
}

ClassFactory::~ClassFactory()
{
}

/* 
 * IUnknown QueryInterface - Returns pointer to IClassFactory 
 */
STDMETHODIMP ClassFactory::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
	if ((riid == IID_IUnknown) || (riid == IID_IClassFactory))
	{
		*ppvObj = static_cast<IClassFactory*>(this);
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
 * IUnknown AddRef - increments the reference count
 */
STDMETHODIMP_(ULONG) ClassFactory::AddRef()
{
	// Increment the reference count for the class factory
	// To be thread-safe, we use InterlockedIncrement
	return InterlockedIncrement(&m_nRefCount);
}

/* 
 * IUnknown Release - decrements the reference count
 */
STDMETHODIMP_(ULONG) ClassFactory::Release()
{
	// Decrement the reference count for the class factory
	// To be thread-safe, we use InterlockedDecrement
	if (InterlockedDecrement(&m_nRefCount) == 0)
	{
		// When the reference count has hit 0, self-destruct!
		delete this;
		return 0;
	}
	return m_nRefCount;
}

/* 
 * IClassFactory CreateInstance
 */
STDMETHODIMP ClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppvObj)
{
	HRESULT hRes;
	*ppvObj = NULL;
	
	CCalcFolder *obj = new CCalcFolder(NULL);

	hRes = obj->QueryInterface(riid, ppvObj);
	obj->Release();

	return hRes;
}

/* 
 * IClassFactory CreateInstance
 */
STDMETHODIMP ClassFactory::LockServer(BOOL fLock)
{
	if (fLock)
		InterlockedIncrement(&global_nLockCount);
	else
		InterlockedDecrement(&global_nLockCount);

	return S_OK;
}
