#include "stdafx.h"
#include "resource.h"

extern HMODULE global_hModule;  // Needed for toolbar buttons

BOOL CALLBACK PropCallbackDlgProc
(
	HWND hDlg, 
	UINT uMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
static PROPSHEETPAGE *ps;

	switch (uMsg)
	{
		case WM_INITDIALOG: 
			ps = (PROPSHEETPAGE *)lParam;
			return TRUE;
			break;

		case WM_NOTIFY:
			switch (((NMHDR FAR *)lParam)->code) 
			{
				case PSN_SETACTIVE:
					// Initialize the controls.
					break;

				case PSN_APPLY:
					SetWindowLong (hDlg, DWL_MSGRESULT, TRUE);
					break;

				case PSN_KILLACTIVE:
					SetWindowLong (hDlg, DWL_MSGRESULT, FALSE);
					return 1;
					break;

				case PSN_RESET:
					SetWindowLong (hDlg, DWL_MSGRESULT, FALSE);
					break;
			}

	}
	return FALSE;
} 

void CreatePropertyPage(LPFNADDPROPSHEETPAGE lpfn, LPARAM lParam)
{
PROPSHEETPAGE psp;
	psp.dwSize = sizeof (PROPSHEETPAGE);
	psp.dwFlags = PSP_USETITLE;
	psp.hInstance = global_hModule;
	psp.pszTemplate = MAKEINTRESOURCE (IDD_CALCPROP);
	psp.pszIcon = NULL;
	psp.pfnDlgProc=(DLGPROC) PropCallbackDlgProc;
	psp.pszTitle = "HelloWorld Config";
	psp.lParam = 0;
	HPROPSHEETPAGE hPage;
	hPage = CreatePropertySheetPage(&psp);
	lpfn(hPage, lParam);
}
