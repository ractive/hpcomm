// CalcPidl.cpp: implementation of the CCalcPidl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CalcPidl.h"
#include <stdio.h>

extern long global_nLockCount;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCalcPidl::CCalcPidl()
{
	m_pMalloc = NULL;
	if (FAILED(SHGetMalloc(&m_pMalloc))) {
		m_pMalloc = NULL;
		delete this;
	}

	InterlockedIncrement(&global_nLockCount);
}

CCalcPidl::~CCalcPidl()
{
	if (m_pMalloc) {
		m_pMalloc->Release();
	}

	InterlockedDecrement(&global_nLockCount);
}

LPITEMIDLIST CCalcPidl::GetNextItem(LPCITEMIDLIST pidl)
{
	// Check for valid pidl.
	if(pidl == NULL)
		return NULL;

	// Get the size of the specified item identifier. 
	int cb = pidl->mkid.cb; 

	// If the size is zero, it is the end of the list. 
	if (cb == 0) 
		return NULL; 

	// Add cb to pidl (casting to increment by bytes). 
	pidl = (LPITEMIDLIST) (((LPBYTE) pidl) + cb); 

	// Return NULL if it is null-terminating, or a pidl otherwise. 
	return (pidl->mkid.cb == 0) ? NULL : (LPITEMIDLIST) pidl; 
}

UINT CCalcPidl::GetSize(LPCITEMIDLIST pidl)
{
	UINT cbTotal = 0;
	if (pidl)
	{
		cbTotal += sizeof(pidl->mkid.cb);    // Null terminator
		while (pidl)
		{
			cbTotal += pidl->mkid.cb;
			pidl = GetNextItem(pidl);
		}
	}
	return cbTotal;
}

LPITEMIDLIST CCalcPidl::CopyItem(LPCITEMIDLIST pidl)
{
	if (pidl == NULL) {
		return NULL;
	}

	UINT cb = 0;

	// Calculate size of list.
	cb = GetSize(pidl);

	LPITEMIDLIST pidlRet = (LPITEMIDLIST)m_pMalloc->Alloc(cb);
	if (pidlRet)
		CopyMemory(pidlRet, pidl, cb);

	return pidlRet;
}

LPITEMIDLIST CCalcPidl::ConcatItem(LPCITEMIDLIST pidlBase, LPCITEMIDLIST pidlAdd)
{
	if(pidlBase == NULL) {
		return CopyItem(pidlAdd);
	}
	if(pidlAdd == NULL)
		return (LPITEMIDLIST)pidlBase;

	LPITEMIDLIST pidlNew;

	UINT cb1 = GetSize(pidlBase) - sizeof(pidlBase->mkid.cb);
	UINT cb2 = GetSize(pidlAdd);

	pidlNew = (LPITEMIDLIST)m_pMalloc->Alloc(cb1 + cb2);
	if (pidlNew)
	{
		CopyMemory(pidlNew, pidlBase, cb1);
		CopyMemory(((LPSTR)pidlNew) + cb1, pidlAdd, cb2);
	}
	return pidlNew;
}

LPITEMIDLIST CCalcPidl::Create(LPCALCITEM pItem)
{
	LPITEMIDLIST   pidlNew;
	USHORT         uSize;

	pidlNew = NULL;

	/*
	Calculate the size. This consists of the ITEMIDLIST, the PIDL structure plus 
	the size of the data. We subtract the size of an HKEY because that is included 
	in uInSize.
	*/
	uSize = sizeof(ITEMIDLIST) + sizeof(CALCITEM);

	/*
	Allocate the memory, adding an additional ITEMIDLIST for the NULL terminating 
	ID List.
	*/
	pidlNew = (LPITEMIDLIST)m_pMalloc->Alloc(uSize + sizeof(ITEMIDLIST));

	if(pidlNew) {
		LPITEMIDLIST   pidlTemp = pidlNew;
		LPCALCITEM     pData;

		//set the size of this item
		pidlTemp->mkid.cb = uSize;

		//set the data for this item
		pData = (LPCALCITEM)pidlTemp->mkid.abID;
		pData->nSize = pItem->nSize;
		pData->nCRC = pItem->nCRC;
		pData->type = pItem->type;
		lstrcpy(pData->sName, pItem->sName);
		lstrcpy(pData->sType, pItem->sType);
		pData->bIsDir = pItem->bIsDir;

		//set the NULL terminator to 0
		pidlTemp = GetNextItem(pidlTemp);
		if (pidlTemp) {
			pidlTemp->mkid.cb = 0;
			pidlTemp->mkid.abID[0] = 0;
		}
	}

	return pidlNew;
}

void CCalcPidl::Delete(LPITEMIDLIST pidl)
{
	m_pMalloc->Free(pidl);
}

LPCALCITEM CCalcPidl::GetPidlData(LPCITEMIDLIST pidl)
{
	if (pidl == NULL) {
		return NULL;
	}

	return (LPCALCITEM)pidl->mkid.abID;
}

BOOL CCalcPidl::IsDirectory(LPCITEMIDLIST pidl)
{
	if (pidl == NULL) {
		return FALSE;
	}
	LPCALCITEM pItem = GetPidlData(pidl);
	if (pItem == 0) {
		return FALSE;
	}

	return pItem->bIsDir;
}

void CCalcPidl::TRACEPIDL(LPITEMIDLIST pidl)
{
	LPCALCITEM pItem = GetPidlData(pidl);
	if (pItem) {
		OutputDebugString(pItem->sName);
		TCHAR s[20];
		sprintf(s, "%x", pidl);
		OutputDebugString(" Adress:");
		OutputDebugString(s);
		OutputDebugString("\r\n");
	}
}


void CCalcPidl::TRACE(TCHAR *sTrace)
{
	OutputDebugString(sTrace);
}
