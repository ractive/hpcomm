class ClassFactory : public IClassFactory
{
public:
	ClassFactory();
	~ClassFactory();
	// IUnknown
	STDMETHODIMP         QueryInterface(REFIID riid, LPVOID *ppvObj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IClassFactory
	STDMETHODIMP         CreateInstance(LPUNKNOWN pUnkOuter, 
											REFIID riid, LPVOID *ppvObj);
	STDMETHODIMP         LockServer(BOOL fLock);
private:
	long m_nRefCount;
};