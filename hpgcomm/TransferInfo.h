/*************************************************************************
  HPGComm
**************************************************************************

Copyright (C) January 2001 by HP and Jean-Pierre Bergamin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
***************************************************************************/


#if !defined(AFX_TRANSFERINFO_H__D8BB7EAC_A2BC_11D3_8325_0000B4A265C3__INCLUDED_)
#define AFX_TRANSFERINFO_H__D8BB7EAC_A2BC_11D3_8325_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransferInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransferInfo dialog

#include "RegBinding.h"


class CTransferInfo : public CDialog
{
// Construction
public:
	void SetSendPos(unsigned long ulBytes);
	BOOL m_bDetailsShown;

	CString m_sPath;
	void ShowInitScreen(BOOL bXModem);
	void ShowUserInfo();
	void Pump();
	static void UpdateDlg(CString& sFileName, BOOL bRecv, int nPackets = -1);
	static void SetRange(unsigned long unSize);
	static void SetPos(unsigned long unEffBytes, unsigned long unTotalBytes);
	CTransferInfo(CWnd* pParent = NULL);   // standard constructor
	HANDLE m_hCancelEvent;
// Dialog Data
	//{{AFX_DATA(CTransferInfo)
	enum { IDD = IDD_TRANSFERINFO };
	CButton	m_DetailsGroup;
	CProgressCtrl	m_ProgressBar;
	CAnimateCtrl	m_Animation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransferInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTransferInfo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDetails();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_bShowDetails;
	unsigned long m_ulSize;
	unsigned long m_unPos;
	unsigned long m_unTotalBytes;
	time_t m_StartTime;
	unsigned int m_nPackets;
	void ShowDetails(BOOL bShow = TRUE);
	BOOL m_bPlaying;
	UINT TimerID;
	bool m_bMainWndWasEnabled;
	static CTransferInfo * pseudoThis;
	CRegBinding rb;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSFERINFO_H__D8BB7EAC_A2BC_11D3_8325_0000B4A265C3__INCLUDED_)
