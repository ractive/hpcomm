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


// PicHolder.cpp : implementation file
//

#include "stdafx.h"
#include "HPComm38.h"
#include "PicHolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPicHolder

CPicHolder::CPicHolder()
{
}

CPicHolder::~CPicHolder()
{
}


BEGIN_MESSAGE_MAP(CPicHolder, CStatic)
	//{{AFX_MSG_MAP(CPicHolder)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPicHolder message handlers

void CPicHolder::OnMouseMove(UINT nFlags, CPoint point) 
{
	::SetCursor(LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_MAGNI)));
	CStatic::OnMouseMove(nFlags, point);
}

void CPicHolder::OnLButtonUp(UINT nFlags, CPoint point) 
{
	GetParent()->PostMessage(WM_COMMAND, IDC_MAGNIFY, 0);
	CStatic::OnLButtonUp(nFlags, point);
}

void CPicHolder::OnRButtonUp(UINT nFlags, CPoint point) 
{
	GetParent()->PostMessage(WM_COMMAND, IDC_SHRINK, 0);
	CStatic::OnRButtonUp(nFlags, point);
}

void CPicHolder::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CStatic::OnLButtonDblClk(nFlags, point);
}
