// ComInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "hpcomm.h"
#include "ComInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define YELLOW	RGB(255, 255,  70)
#define GREEN	RGB(  0, 128,   0)
#define RED		RGB(255,   0,   0)
#define WHITE	RGB(255, 255, 255)
#define BLACK	RGB(  0,   0,   0)
#define GRAY	RGB( 80,  80,  80)
#define PURPLE	RGB(160, 160, 174)

// Instance ptr for callback.
CComInfoDlg *CComInfoDlg::instance_ = 0;
int CComInfoDlg::nComPort = 0;

void CComInfoDlg::NewDlg(int comPort)
{
	VERIFY(instance_==0);

	instance_ = new CComInfoDlg;
    instance_->SetBitmap(IDB_SPLASH, CBitmapDialog::BITMAP_CENTER);

	if (comPort > 0)
		nComPort = comPort;
	
    instance_->Create(IDD_COMINFO, AfxGetMainWnd());
    
}

void CComInfoDlg::
SetText(void)
{
	// We fill the static text fields with the correct
	// text.
	char s[1024];
	sprintf(s, "Trying to open COM %d                                                                          ", nComPort);
	m_Stage0.SetWindowText(s);
	sprintf(s, "Connecting to the calculator                                                                   ");
	m_Stage1.SetWindowText(s);
	sprintf(s, "Getting calculator information                                                                 ");
	m_Stage2.SetWindowText(s);
	sprintf(s, "Reading PC contents                                                                            ");
	m_Stage3.SetWindowText(s);
	sprintf(s, "Reading calculator contents                                                                    ");
	m_Stage4.SetWindowText(s);
}

void CComInfoDlg::ShowAboutDlg()
{
	// If we don't have already an instance of this dialog
	// we create one.
    if (instance_ == 0)
        NewDlg(0);
	
	instance_->SetSplash(false);
}

void CComInfoDlg::SetPort(int comPort)
{
	if (comPort > 0)
		nComPort = comPort;
}

void CComInfoDlg::BumpDlg(int stage, bool bSuccess /* = true */)
{
	if (instance_ == 0)
		return;

	switch (stage) {
	case INITDLG:
		instance_->m_Stage0.SetTextColor(YELLOW);
		instance_->m_Stage0.InvalidateRect(NULL);
		break;
	case TRYOPENPORT:
		if (bSuccess) {
			instance_->m_Stage0.SetTextColor(GREEN);
			instance_->m_Tick0.ModifyStyle(0, WS_VISIBLE);
			instance_->m_Tick0.InvalidateRect(NULL);
		}
		else {
			instance_->m_Stage0.SetTextColor(RED);
		}
		instance_->m_Stage0.InvalidateRect(NULL);
		instance_->m_Stage1.SetTextColor(YELLOW);
		instance_->m_Stage1.InvalidateRect(NULL);
		break;
	case CONNECTTOCALC:
		if (bSuccess) {
			instance_->m_Stage1.SetTextColor(GREEN);
			instance_->m_Tick1.ModifyStyle(0, WS_VISIBLE);
			instance_->m_Tick1.InvalidateRect(NULL);
		}
		else {
			instance_->m_Stage1.SetTextColor(RED);
		}
		instance_->m_Stage1.InvalidateRect(NULL);
		instance_->m_Stage2.SetTextColor(YELLOW);
		instance_->m_Stage2.InvalidateRect(NULL);
		break;
	case GETTINGCALCINFO:
		if (bSuccess) {
			instance_->m_Stage2.SetTextColor(GREEN);
			instance_->m_Tick2.ModifyStyle(0, WS_VISIBLE);
			instance_->m_Tick2.InvalidateRect(NULL);
		}
		else {
			instance_->m_Stage2.SetTextColor(RED);		
		}
		instance_->m_Stage2.InvalidateRect(NULL);
		instance_->m_Stage3.SetTextColor(YELLOW);
		instance_->m_Stage3.InvalidateRect(NULL);
		break;
	case READINGPC:
		if (bSuccess) {
			instance_->m_Stage3.SetTextColor(GREEN);
			instance_->m_Tick3.ModifyStyle(0, WS_VISIBLE);
			instance_->m_Tick3.InvalidateRect(NULL);
		}
		else {
			instance_->m_Stage3.SetTextColor(RED);
		}
		instance_->m_Stage3.InvalidateRect(NULL);
		instance_->m_Stage4.SetTextColor(YELLOW);
		instance_->m_Stage4.InvalidateRect(NULL);
		break;
	case READINGCALC:
		if (bSuccess) {
			instance_->m_Stage4.SetTextColor(GREEN);
			instance_->m_Tick4.ModifyStyle(0, WS_VISIBLE);
			instance_->m_Tick4.InvalidateRect(NULL);
		}
		else {
			instance_->m_Stage4.SetTextColor(RED);
		}
		instance_->m_Stage4.InvalidateRect(NULL);
		break;
	}

	instance_->UpdateWindow();
}

void CComInfoDlg::DeleteDlg()
{
	if (instance_ == 0)
		return;

	// Close the dialog box.
	instance_->DestroyWindow();

	delete instance_;

	instance_ = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CComInfoDlg dialog

CComInfoDlg::CComInfoDlg(CWnd* pParent /*=NULL*/)
	: CBitmapDialog(CComInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CComInfoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CComInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CBitmapDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComInfoDlg)
	DDX_Control(pDX, IDC_TICK4, m_Tick4);
	DDX_Control(pDX, IDC_TICK3, m_Tick3);
	DDX_Control(pDX, IDC_TICK2, m_Tick2);
	DDX_Control(pDX, IDC_TICK1, m_Tick1);
	DDX_Control(pDX, IDC_TICK0, m_Tick0);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CComInfoDlg, CBitmapDialog)
	//{{AFX_MSG_MAP(CComInfoDlg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComInfoDlg message handlers



BOOL CComInfoDlg::OnInitDialog() 
{
	CBitmapDialog::OnInitDialog();
	m_Brush.CreateSolidBrush(PURPLE); // color purple brush
	//m_Brush.CreateStockObject(GRAY_BRUSH);
	m_Stage0.SubclassDlgItem(IDC_STAGE0, this);
	m_Stage1.SubclassDlgItem(IDC_STAGE1, this);
	m_Stage2.SubclassDlgItem(IDC_STAGE2, this);
	m_Stage3.SubclassDlgItem(IDC_STAGE3, this);
	m_Stage4.SubclassDlgItem(IDC_STAGE4, this);

	SetText();

    m_Stage0.SetBkColor(PURPLE);
	m_Stage1.SetBkColor(PURPLE);
	m_Stage2.SetBkColor(PURPLE);
	m_Stage3.SetBkColor(PURPLE);
	m_Stage4.SetBkColor(PURPLE);

	m_Stage0.SetTextColor(GRAY);
	m_Stage1.SetTextColor(GRAY);
	m_Stage2.SetTextColor(GRAY);
	m_Stage3.SetTextColor(GRAY);
	m_Stage4.SetTextColor(GRAY);

	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


HBRUSH CComInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CBitmapDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch (nCtlColor)
	{
	//Edit controls need white background and black text
	//Note the 'return hbr' which is needed to draw the Edit
	//control's internal background (as opposed to text background)
	case CTLCOLOR_EDIT:
		pDC->SetTextColor(RGB(0,0,0));
		pDC->SetBkColor(RGB(255,255,255));
		return hbr;
	//For listboxes, scrollbars, buttons, messageboxes and dialogs,
	//use the new brush (m_brush)
	case CTLCOLOR_LISTBOX:
	case CTLCOLOR_SCROLLBAR:
	case CTLCOLOR_BTN:
	case CTLCOLOR_MSGBOX:
	case CTLCOLOR_DLG:
		return m_Brush;
	//This shouldn't occurr since we took all the cases, but
	//JUST IN CASE, return the new brush
	default:
		return m_Brush;
	} 
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CComInfoDlg::BringToTop()
{
	if (instance_ != 0) {
		instance_->SetForegroundWindow();
		instance_->UpdateWindow();
	}
}

void CComInfoDlg::OnOK() 
{
	// Restore the normal splash window.
	SetSplash(true);
	DeleteDlg();
}

void CComInfoDlg::OnCancel() 
{
	OnOK();
}

void CComInfoDlg::SetSplash(bool bSplash)
{
	if (bSplash) {
		// If this is the splash window, we make all the 
		// text-field visible...
		m_Stage0.ModifyStyle(0, WS_VISIBLE);
		m_Stage0.InvalidateRect(NULL);
		m_Stage1.ModifyStyle(0, WS_VISIBLE);
		m_Stage1.InvalidateRect(NULL);
		m_Stage2.ModifyStyle(0, WS_VISIBLE);
		m_Stage2.InvalidateRect(NULL);
		m_Stage3.ModifyStyle(0, WS_VISIBLE);
		m_Stage3.InvalidateRect(NULL);
		m_Stage4.ModifyStyle(0, WS_VISIBLE);
		m_Stage4.InvalidateRect(NULL);
		// .. but hide the OK button.
		GetDlgItem(IDOK)->ModifyStyle(WS_VISIBLE, 0);
	}
	else {
		// If this is the about box, we hide all the
		// text-fields ...
		m_Stage0.ModifyStyle(WS_VISIBLE, 0);
		m_Stage0.InvalidateRect(NULL);
		m_Stage1.ModifyStyle(WS_VISIBLE, 0);
		m_Stage1.InvalidateRect(NULL);
		m_Stage2.ModifyStyle(WS_VISIBLE, 0);
		m_Stage2.InvalidateRect(NULL);
		m_Stage3.ModifyStyle(WS_VISIBLE, 0);
		m_Stage3.InvalidateRect(NULL);
		m_Stage4.ModifyStyle(WS_VISIBLE, 0);
		m_Stage4.InvalidateRect(NULL);
		// ... but show the OK button.
		GetDlgItem(IDOK)->ModifyStyle(0, WS_VISIBLE);
	}

	InvalidateRect(NULL);
	UpdateWindow();
}
