// CalcPidl.h: interface for the CCalcPidl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALCPIDL_H__F491666A_340B_4D54_ABDC_3DA6E993492D__INCLUDED_)
#define AFX_CALCPIDL_H__F491666A_340B_4D54_ABDC_3DA6E993492D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _CALCITEM {
	USHORT cb;
	int type;
	UINT nSize;
	UINT nCRC;
	TCHAR sName[40];
	TCHAR sType[40];
	BOOL bIsDir;
} CALCITEM, *LPCALCITEM;


class CCalcPidl  
{
public:
	void TRACE(TCHAR * sTrace);
	void TRACEPIDL(LPITEMIDLIST pidl);
	BOOL IsDirectory(LPCITEMIDLIST pidl);
	LPCALCITEM GetPidlData(LPCITEMIDLIST pidl);
	void Delete(LPITEMIDLIST pidl);
	LPITEMIDLIST ConcatItem(LPCITEMIDLIST pidlBase, LPCITEMIDLIST pidlAdd);
	LPITEMIDLIST CopyItem(LPCITEMIDLIST pidl);
	UINT GetSize(LPCITEMIDLIST pidl);
	LPITEMIDLIST GetNextItem(LPCITEMIDLIST pidl);
	CCalcPidl();
	virtual ~CCalcPidl();
	LPITEMIDLIST Create(LPCALCITEM pItem);
private:

	LPMALLOC m_pMalloc;
};

#endif // !defined(AFX_CALCPIDL_H__F491666A_340B_4D54_ABDC_3DA6E993492D__INCLUDED_)
