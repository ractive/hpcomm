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


// 38GDlg.h : header file
//

#if !defined(AFX_38GDLG_H__59086616_A28E_11D3_8324_0000B4A265C3__INCLUDED_)
#define AFX_38GDLG_H__59086616_A28E_11D3_8324_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProtException.h"
#include "Kermit.h"
#include "RegBinding.h"
#include "TrayIcon.h"

#define DIR_TIMER 500

/////////////////////////////////////////////////////////////////////////////
// CHPComm38Dlg dialog

class CHPComm38Dlg : public CDialog
{
// Construction
public:
	DWORD m_dwPicShown;
	CHPComm38Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHPComm38Dlg)
	enum { IDD = IDD_HPCOMM38_DIALOG };
	CStatic	m_CalcPic;
	CComboBox	m_DirEditBox;
	CButton	m_ShowVD;
	CButton	m_CreateDir;
	CStatic	m_InfoStatic;
	CEdit	m_InfoCtrl;
	CComboBox	ComPortCtrl;
	CString m_sPath;
	CString	m_sInfo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHPComm38Dlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:
	
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CHPComm38Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChangeDir();
	afx_msg void OnClose();
	afx_msg void OnOK();
	afx_msg void OnHelpAbout();
	afx_msg void OnShowVD();
    afx_msg void OnHelpFinder();
	afx_msg void OnHelpLanguage();
	afx_msg void OnSelchangeComport();
	afx_msg void OnCreateDir();
	afx_msg void HideInfo();
	afx_msg void OnChangeDirectory();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRestore();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEditchangeDirectory();
	afx_msg void OnSelchangeDirectory();
	afx_msg void OnDropdownDirectory();
	//}}AFX_MSG
	afx_msg void OnComdaemonMsg(UINT pParam, long lParam);
	afx_msg void OnTransferInfoClosed(UINT pParam, long lParam);
	afx_msg BOOL ToolTipCallback( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	DECLARE_MESSAGE_MAP()
private:
	CString m_sShownDir;
	BOOL m_bDirInexistent;
	void SetCalcPic(BOOL bAfterSelection);
	void FillHistoryCombo();
	void UpdateHistory(CString &sPath);
	void ShowTaskIcon(BOOL bShow = TRUE);
	void HandleCreateDirButton();
	void EnableCreateDir(BOOL bEnable);
	CToolTipCtrl m_ToolTip;
	bool m_bInfoShown;
	bool m_bVDInitInfoShown;
	void ShowInfo(CString &sInfoMsg);
	CMenu * pMenu;
	void ChooseHelpFileName();
	// This is kind of a reference counter. It counts how much screenshot
	// dialogs are opened. The app only exits if this counter is 0, otherwise you'll
	// see a message box.
	// In OnClose() it's checked if the reference counter == 0
	int nScreenDlg;
	CDialog * pWaitInfoDlg;
	unsigned char * pScreenData;
	void ReadScreenData(char);
	void ShowScreenShot();
	static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData);
	void ExceptionHandler(CProtException * pEx);
	CKermit * pKermit;
	int nComPort;

	// Defines the laguage of the help file. This is the same string as the
	// help file is called (without extension).
	// If the help file is called Italiano.hlp this string contains "Italiano"
	// It is stored in the registry and is initially empty.
	CString sHelpLanguage;

	CRegBinding rb;
	static CHPComm38Dlg *pseudoThis;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_38GDLG_H__59086616_A28E_11D3_8324_0000B4A265C3__INCLUDED_)
