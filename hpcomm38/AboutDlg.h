// AboutDlg.h: interface for the CAboutDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ABOUTDLG_H__C2E6077E_A394_11D3_98E6_0060B0A2563C__INCLUDED_)
#define AFX_ABOUTDLG_H__C2E6077E_A394_11D3_98E6_0060B0A2563C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "HyperLink.h"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CHyperLink	gplURL;
	CHyperLink	hpcommURL;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_ABOUTDLG_H__C2E6077E_A394_11D3_98E6_0060B0A2563C__INCLUDED_)
