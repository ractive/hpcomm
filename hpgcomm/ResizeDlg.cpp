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


// ResizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HPComm38.h"
#include "ResizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResizeDlg dialog


CResizeDlg::CResizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResizeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResizeDlg)
	m_Height = 0;
	m_Width = 0;
	//}}AFX_DATA_INIT
}


void CResizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResizeDlg)
	DDX_Text(pDX, IDC_HEIGHT, m_Height);
	DDX_Text(pDX, IDC_WIDTH, m_Width);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResizeDlg, CDialog)
	//{{AFX_MSG_MAP(CResizeDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResizeDlg message handlers
