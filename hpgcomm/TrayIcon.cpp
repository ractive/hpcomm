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


// TrayIcon.cpp : implementation file
//

// This code is "stolen" from the MSDN article C/C++ Q&A by Paul DiLascia from
// the Microsoft Systems Journal (Copyright © 1995), Periodicals 1996


#include "stdafx.h"
#include "TrayIcon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CTrayIcon::m_bIsAnimated = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon

IMPLEMENT_DYNCREATE(CTrayIcon, CCmdTarget)

CTrayIcon::CTrayIcon()
{
	memset(&m_nid, 0, sizeof(m_nid));
	m_nid.cbSize = sizeof(m_nid);
	ThreadInfo.hKillMe = NULL;
	pAnimationThread = NULL;
	ThreadInfo.aIcon = NULL;
}

CTrayIcon::~CTrayIcon()
{
	KillAnimationThread();
	RemoveIcon();
	if (ThreadInfo.aIcon != NULL) {
		delete [] ThreadInfo.aIcon;
	}
}



BEGIN_MESSAGE_MAP(CTrayIcon, CCmdTarget)
	//{{AFX_MSG_MAP(CTrayIcon)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon message handlers

bool CTrayIcon::SetIcon(UINT uID)
{
	HICON hIcon = NULL;
	if (uID) {
		LoadString(AfxGetInstanceHandle(), uID, m_nid.szTip, sizeof(m_nid.szTip));
		hIcon = AfxGetApp()->LoadIcon(uID);
	}
	
	return SetIcon(hIcon, NULL);
}

bool CTrayIcon::SetIcon(HICON hIcon, LPCSTR lpTip)
{   
	UINT msg;
	m_nid.uFlags = 0;
	// Set the icon
	if (hIcon) {
		// Add or replace icon in system tray
		msg = m_nid.hIcon ? NIM_MODIFY : NIM_ADD;
		m_nid.hIcon = hIcon;
		m_nid.uFlags |= NIF_ICON;
	}
	else {
		// remove icon from tray
		if (m_nid.hIcon == NULL)
			return TRUE;      // already deleted
		msg = NIM_DELETE;
	}
	// Use the tip, if any
	if (lpTip)
		strncpy(m_nid.szTip, lpTip, sizeof(m_nid.szTip));
	
	if (m_nid.szTip[0])
		m_nid.uFlags |= NIF_TIP;
	
	// Use callback if any
	if (m_nid.uCallbackMessage && m_nid.hWnd)
		m_nid.uFlags |= NIF_MESSAGE;
	
	// Do it
	bool bRet = (Shell_NotifyIcon(msg, &m_nid) == TRUE);
	
	if (msg==NIM_DELETE || !bRet)
		m_nid.hIcon = NULL;	// failed
	return bRet;
}

bool CTrayIcon::RemoveIcon()
{
	return SetIcon(0);
}

void CTrayIcon::Init(UINT uID)
{
	m_nid.uID = uID;
	LoadString(AfxGetInstanceHandle(), uID, m_nid.szTip, sizeof(m_nid.szTip));
}

LRESULT CTrayIcon::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
   if ((wParam != m_nid.uID && wParam != m_MenuID) || (lParam != WM_RBUTTONUP && lParam != WM_LBUTTONDBLCLK))
	   return 0;
   // If there's a resource menu with the same ID as the icon, use it as
   // the right-button popup menu. CTrayIcon will interprets the first
   // item in the menu as the default command for WM_LBUTTONDBLCLK
   
   CMenu menu;
   if (!menu.LoadMenu(m_MenuID))
	   return 0;
   
   CMenu* pSubMenu = menu.GetSubMenu(0);
   
   if (!pSubMenu)
	   return 0;
   
   if (lParam == WM_RBUTTONUP) {
	   // Make first menu item the default (bold font)
	   ::SetMenuDefaultItem(pSubMenu->m_hMenu, 0, TRUE);
	   // Display the menu at the current mouse location. There's a "bug"
	   // (Microsoft calls it a feature) in Windows 95 that requires calling
	   // SetForegroundWindow. To find out more, search for Q135788 in MSDN.
	   
	   CPoint mouse;
	   GetCursorPos(&mouse);
	   ::SetForegroundWindow(m_nid.hWnd);
	   ::TrackPopupMenu(pSubMenu->m_hMenu, 0, mouse.x, mouse.y, 0, m_nid.hWnd, NULL);
   }
   else  
	   // double click: execute first menu item
	   ::SendMessage(m_nid.hWnd, WM_COMMAND, pSubMenu->GetMenuItemID(0), 0);
   
   return 1; // handled
}

void CTrayIcon::SetNotificationWindow(CWnd *pNotifyWnd, UINT uCbMsg)
{
   // If the following assert fails, you're probably
	// calling me before you created your window. Oops.
	ASSERT(pNotifyWnd==NULL || ::IsWindow(pNotifyWnd->GetSafeHwnd()));
	m_nid.hWnd = pNotifyWnd->GetSafeHwnd();
	ASSERT(uCbMsg==0 || uCbMsg>=WM_USER);
	m_nid.uCallbackMessage = uCbMsg;
}

bool CTrayIcon::IsSet()
{
	return (m_nid.hIcon != NULL);
}

void CTrayIcon::SetMenu(UINT nMenuID)
{
	m_MenuID = nMenuID;
}

UINT CTrayIcon::AnimationThread(LPVOID lpInfo)
{
	if (m_bIsAnimated)
		return 1;

	tagAnimationThreadInfo * pInfo = (tagAnimationThreadInfo *)lpInfo;
	if (pInfo->aIcon == NULL) {
		return 1;
	}
	m_bIsAnimated = TRUE;
	HICON hOldIcon = pInfo->nid.hIcon;
	UINT msg;
	for (int i = 0; i < pInfo->nLoops; i++) {
		for (int j = 0; j < pInfo->nIconCount; j++) {
			pInfo->nid.uFlags = 0;
			// Set the icon
			if (pInfo->aIcon[j]) {
				// Replace icon in system tray
				msg = NIM_MODIFY;
				pInfo->nid.hIcon = pInfo->aIcon[j];
				pInfo->nid.uFlags = NIF_ICON;
			}
			else {
				delete [] pInfo->aIcon;
				pInfo->aIcon = NULL;
				return 1;
			}
			
			// Do it
			if (!Shell_NotifyIcon(msg, &pInfo->nid)) {
				delete [] pInfo->aIcon;
				pInfo->aIcon = NULL;
				return 1;
			}
			
			// Check if the thread should be killed.
			if (WaitForSingleObject(pInfo->hKillMe, pInfo->nDelay) == WAIT_OBJECT_0) {
				delete [] pInfo->aIcon;
				pInfo->aIcon = NULL;
				return 0;
			}

		}
	}
	delete [] pInfo->aIcon;
	pInfo->aIcon = NULL;
	// Set the old icon
	pInfo->nid.uFlags = NIF_ICON;
	pInfo->nid.hIcon = hOldIcon;
	Shell_NotifyIcon(msg, &pInfo->nid);
	m_bIsAnimated = FALSE;
	return 0;
}

void CTrayIcon::AnimateIcon(HICON *aIcon, UINT nCount, UINT nLoops, UINT nDelay)
{
	return;
	ThreadInfo.aIcon = new HICON[nCount];
	for (UINT i = 0; i < nCount; i++) {
		ThreadInfo.aIcon[i] = aIcon[i];
	}
	ThreadInfo.nIconCount = nCount;
	ThreadInfo.nLoops = nLoops;
	ThreadInfo.nid = m_nid;
	ThreadInfo.nDelay = nDelay;
	ThreadInfo.hKillMe = CreateEvent(NULL, FALSE, FALSE, NULL);
	pAnimationThread = AfxBeginThread(AnimationThread, &ThreadInfo);
}

void CTrayIcon::KillAnimationThread()
{
	if (ThreadInfo.hKillMe && pAnimationThread) {
		SetEvent(ThreadInfo.hKillMe);
		// Wait 5 sec. to kill the thread.
		WaitForSingleObject(pAnimationThread->m_hThread, 5000);
	}

}

