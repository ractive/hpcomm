/*************************************************************************
                     MyIPWinClient
**************************************************************************

Copyright (C) April, 2000 Jean-Pierre Bergamin, james@ractive.ch

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


#if !defined(AFX_TRAYICON_H__9175C083_6CAB_11D3_82D5_0000B4A265C3__INCLUDED_)
#define AFX_TRAYICON_H__9175C083_6CAB_11D3_82D5_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrayIcon.h : header file
//

// This code is "stolen" from the MSDN article C/C++ Q&A by Paul DiLascia from
// the Microsoft Systems Journal (Copyright © 1995), Periodicals 1996


/////////////////////////////////////////////////////////////////////////////
// CTrayIcon command target

class CTrayIcon : public CCmdTarget
{
	DECLARE_DYNCREATE(CTrayIcon)

	CTrayIcon();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	static BOOL m_bIsAnimated;
	void AnimateIcon(HICON * aIcon, UINT nCount, UINT nLoops, UINT nDelay);
	void SetMenu(UINT nMenuID);
	bool IsSet();
	void SetNotificationWindow(CWnd * pNotifyWnd, UINT uCbMsg);
	void Init(UINT uID);
	bool RemoveIcon();
	bool SetIcon(HICON hIcon, LPCSTR lpTip);
	bool SetIcon(UINT nID);
	virtual ~CTrayIcon();
	LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrayIcon)
	//}}AFX_VIRTUAL

// Implementation
	void KillAnimationThread();
protected:
	NOTIFYICONDATA m_nid;
	UINT m_MenuID;

	// Generated message map functions
	//{{AFX_MSG(CTrayIcon)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CWinThread * pAnimationThread;
	struct tagAnimationThreadInfo {
		HANDLE hKillMe;
		NOTIFYICONDATA nid;
		HICON * aIcon;
		int nIconCount;
		int nLoops;
		UINT nDelay; // in milliseconds
	} ThreadInfo;
	static UINT AnimationThread(LPVOID lpInfo);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAYICON_H__9175C083_6CAB_11D3_82D5_0000B4A265C3__INCLUDED_)
