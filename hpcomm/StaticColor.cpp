// StaticColor.cpp : implementation file
//

#include "stdafx.h"
#include "hpcomm.h"
#include "StaticColor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticColor

CStaticColor::CStaticColor()
{
}

CStaticColor::~CStaticColor()
{
}


BEGIN_MESSAGE_MAP(CStaticColor, CStatic)
	//{{AFX_MSG_MAP(CStaticColor)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStaticColor message handlers

HBRUSH CStaticColor::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	// Set foreground color
	pDC->SetTextColor(m_crTextColor);

	// Set background color & brush
	pDC->SetBkColor(m_crBkColor);

	// Return a non-NULL brush if the parent's handler should not be called
    return (HBRUSH)m_brBkgnd;
}

void CStaticColor::SetTextColor(COLORREF crTextColor)
{
	// Set new text color
	m_crTextColor = crTextColor;

	// Repaint control
	Invalidate();
}

COLORREF CStaticColor::GetTextColor()
{
	return m_crTextColor;
}


void CStaticColor::SetBkColor(COLORREF crBkColor)
{
	// Set new background color
	if (crBkColor != 0xffffffff)
	{
		m_crBkColor = crBkColor;
	}
	else // Set default background color
	{
		m_crBkColor = ::GetSysColor(COLOR_BTNFACE);
	}

    m_brBkgnd.DeleteObject();
    m_brBkgnd.CreateSolidBrush(m_crBkColor);

	// Repaint control
	Invalidate();
}


COLORREF CStaticColor::GetBkColor()
{
	return m_crBkColor;
}
