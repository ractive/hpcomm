// EnumProxy.h: interface for the CEnumProxy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENUMPROXY_H__5D7D373B_D0B6_4B7D_B615_AB68401B4000__INCLUDED_)
#define AFX_ENUMPROXY_H__5D7D373B_D0B6_4B7D_B615_AB68401B4000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _ENUMLIST {
	struct _ENUMLIST	*pNext;
	LPCITEMIDLIST		pidl;
} ENUMLIST, FAR *LPENUMLIST;

typedef struct _ENUMCOLL {
	struct _ENUMLIST	*pList;
	struct _ENUMCOLL	*pNext;
	LPCITEMIDLIST		pidl;
} ENUMCOLL, FAR *LPENUMCOLL;

class CCalcPidl;

class CEnumProxy  
{



public:
	BOOL SetEnumList(LPCITEMIDLIST pidl);
	LPITEMIDLIST GetNext(BOOL bDirOnly);
	CEnumProxy();
	virtual ~CEnumProxy();
private:
	BOOL IsSamePidl(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
	CCalcPidl * m_pCalcPidl;
	BOOL FillList(LPENUMCOLL pList);
	BOOL AddList(LPCITEMIDLIST pidl);
	BOOL AddToEnumList(LPITEMIDLIST pidl);

	LPENUMLIST m_pFirstItem;
	LPENUMLIST m_pLastItem;
	LPENUMLIST m_pCurrentItem;

	LPENUMCOLL m_pFirstList;
	LPENUMCOLL m_pLastList;
	LPENUMCOLL m_pCurrentList;
};

#endif // !defined(AFX_ENUMPROXY_H__5D7D373B_D0B6_4B7D_B615_AB68401B4000__INCLUDED_)
