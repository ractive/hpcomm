// EnumProxy.cpp: implementation of the CEnumProxy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EnumProxy.h"
#include "CalcComm.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCalcComm global_CalcComm;


CEnumProxy::CEnumProxy()
{
	m_pFirstItem = m_pLastItem = m_pCurrentItem = NULL;
	m_pFirstList = m_pLastList = m_pCurrentList = NULL;

	m_pCalcPidl = new CCalcPidl();
	if (m_pCalcPidl == NULL) {
		delete this;
		return;
	}
}

CEnumProxy::~CEnumProxy()
{
	if (m_pCalcPidl) {
		delete m_pCalcPidl;
	}
}

BOOL CEnumProxy::AddToEnumList(LPITEMIDLIST pidl)
{
	LPENUMLIST  pNew;
	IMalloc * pMalloc = NULL;
	if (FAILED(SHGetMalloc(&pMalloc))) {
		return FALSE;
	}
	pNew = (LPENUMLIST)pMalloc->Alloc(sizeof(ENUMLIST));

#ifdef _DEBUG
	m_pCalcPidl->TRACE("AddToEnumList: ");
	m_pCalcPidl->TRACEPIDL(pidl);
#endif

	if(pNew) {
		//set the next pointer
		pNew->pNext = NULL;
		pNew->pidl = pidl;

		//is this the first item in the list?
		if(!m_pFirstItem) {
			m_pFirstItem = pNew;
			m_pCurrentItem = m_pFirstItem;
		}

		if(m_pLastItem) {
			//add the new item to the end of the list
			m_pLastItem->pNext = pNew;
		}

		//update the last item pointer
		m_pLastItem = pNew;

		return TRUE;
	}

	return FALSE;
}

BOOL CEnumProxy::AddList(LPCITEMIDLIST pidl)
{
	LPENUMCOLL pNewList;

	m_pFirstItem = m_pCurrentItem = m_pLastItem = NULL;

#ifdef _DEBUG
	m_pCalcPidl->TRACE("ADDING NEW LIST FOR: ");
	m_pCalcPidl->TRACEPIDL((LPITEMIDLIST)pidl);
#endif

	IMalloc * pMalloc = NULL;
	if (FAILED(SHGetMalloc(&pMalloc))) {
		return FALSE;
	}
	pNewList = (LPENUMCOLL)pMalloc->Alloc(sizeof(ENUMCOLL));

	if(pNewList) {
		//set the next pointer
		pNewList->pNext = NULL;
		pNewList->pidl = pidl;

		//is this the first item in the list?
		if(!m_pFirstList) {
			m_pFirstList = pNewList;
			m_pCurrentList = m_pFirstList;
		}

		if(m_pLastList) {
			//add the new item to the end of the list
			m_pLastList->pNext = pNewList;
		}

		//update the last item pointer
		m_pLastList = pNewList;
#ifdef _DEBUG
	TCHAR listaddr[20];
	sprintf(listaddr, "%x", pNewList);
	m_pCalcPidl->TRACE("New List Address: ");
	m_pCalcPidl->TRACE(listaddr);
#endif


		if (pidl) {
			LPCALCITEM pItem = m_pCalcPidl->GetPidlData(pidl);
			global_CalcComm.ChangeDir(pItem->sName);
		}
		else {
			//global_CalcComm.GoHome();			
		}

		return FillList(pNewList);
	}

	return FALSE;
}


LPITEMIDLIST CEnumProxy::GetNext(BOOL bDirOnly)
{
	if (m_pCurrentItem == NULL) {
		return NULL;
	}
	LPCITEMIDLIST pRet = m_pCurrentItem->pidl;
	m_pCurrentItem = m_pCurrentItem->pNext;
#ifdef _DEBUG
	LPCALCITEM pItem = m_pCalcPidl->GetPidlData(pRet);
	pItem->type;
#endif
	if (bDirOnly && !m_pCalcPidl->IsDirectory(pRet)) {
		return GetNext(bDirOnly);
	}
	return m_pCalcPidl->CopyItem(pRet);
}

BOOL CEnumProxy::SetEnumList(LPCITEMIDLIST pidl)
{
	LPENUMCOLL m_pCursor = m_pFirstList;
	while (m_pCursor && !IsSamePidl(m_pCursor->pidl, pidl)) {
		m_pCursor = m_pCursor->pNext;
	}

	if (m_pCursor == NULL) {
		// This list is not read in yet
		if (!AddList(pidl)) {
			return FALSE;
		}
		m_pCursor = m_pLastList;
	}

	m_pCurrentList = m_pCursor;
	m_pCurrentItem = m_pCurrentList->pList;

	return TRUE;
}

BOOL CEnumProxy::FillList(LPENUMCOLL pList)
{
	LPCALCITEM pItem = NULL;
	if (!global_CalcComm.FindFirstFile()) {
		return FALSE;
	}
	while (pItem = global_CalcComm.FindNextFile()) {
		AddToEnumList(m_pCalcPidl->Create(pItem));
	}
	
	pList->pList = m_pFirstItem;

	return TRUE;
}

BOOL CEnumProxy::IsSamePidl(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
	LPCALCITEM pItem1 = m_pCalcPidl->GetPidlData(pidl1);
	LPCALCITEM pItem2 = m_pCalcPidl->GetPidlData(pidl2);

	if (pItem1 == NULL && pItem2 == NULL) {
		return TRUE;
	}
	if (pItem1 == NULL || pItem2 == NULL) {
		return FALSE;
	}
#ifdef _DEBUG
	OutputDebugString("Comparing: ");
	OutputDebugString(pItem1->sName);
	OutputDebugString(" and ");
	OutputDebugString(pItem2->sName);
	OutputDebugString("\r\n");

#endif
	return lstrcmp(pItem1->sName, pItem2->sName) == 0;
}
