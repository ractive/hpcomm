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
