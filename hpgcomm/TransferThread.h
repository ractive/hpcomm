#if !defined(AFX_TRANSFERTHREAD_H__106682CB_346B_4770_A29B_B33DE9E3CA5F__INCLUDED_)
#define AFX_TRANSFERTHREAD_H__106682CB_346B_4770_A29B_B33DE9E3CA5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransferThread.h : header file
//

#define WM_TRANSFER_UPDATE (WM_USER + 33)
#define WM_TRANSFER_RANGE (WM_USER + 34)
#define WM_TRANSFER_POS (WM_USER + 35)
#define WM_TRANSFER_QUIT (WM_USER + 36)
#define WM_TRANSFER_INFO (WM_USER + 37)
#define WM_TRANSFER_FILEINFO (WM_USER + 38)
#define WM_TRANSFER_INIT (WM_USER + 39)
#define WM_TRANSFER_SENDPOS (WM_USER + 40)

#include "TransferInfo.h"


/////////////////////////////////////////////////////////////////////////////
// CTransferThread thread

class CTransferThread : public CWinThread
{
	DECLARE_DYNCREATE(CTransferThread)
protected:
	CTransferThread();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	HANDLE m_hCancelEvent;
	void Pump();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransferThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	CTransferInfo * m_pInfoDlg;

protected:
	virtual ~CTransferThread();
	// Generated message map functions
	//{{AFX_MSG(CTransferThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
public:
	afx_msg void OnPos(UINT pParam, long lParam);
	afx_msg void OnSendPos(UINT pParam, long lParam);
	afx_msg void OnRange(UINT pParam, long lParam);
	afx_msg void OnQuit(UINT pParam, long lParam);
	afx_msg void OnInfo(UINT pParam, long lParam);
	afx_msg void OnFileInfo(UINT pParam, long lParam);
	afx_msg void OnInit(UINT pParam, long lParam);

	DECLARE_MESSAGE_MAP()
private:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSFERTHREAD_H__106682CB_346B_4770_A29B_B33DE9E3CA5F__INCLUDED_)
