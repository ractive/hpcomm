// CaptureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "hpcomm.h"
#include "CaptureDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT ComDaemon(LPVOID lParam);


/////////////////////////////////////////////////////////////////////////////
// CCaptureDlg dialog


CCaptureDlg::CCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCaptureDlg::IDD, pParent)
{
    data = 0;
	//{{AFX_DATA_INIT(CCaptureDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaptureDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCaptureDlg, CDialog)
	//{{AFX_MSG_MAP(CCaptureDlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_COMDAEMON, OnMsg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCaptureDlg message handlers

void CCaptureDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	SetEvent(hCancel);
}

BOOL CCaptureDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	pProt->CloseComPort(false);
	CString sPort;
    sPort.Format(_T("COM%d"), pSettings->nComPort);
	// Try to open the com-port
	hComPort = ::CreateFile(sPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);

	if (hComPort == INVALID_HANDLE_VALUE) {
		AfxMessageBox("The com-port could not be opened!");
	}

	else {
		DCB dcb;
		GetCommState(hComPort, &dcb);
		dcb.BaudRate = pSettings->dwBaud;
		dcb.ByteSize = pSettings->nByte;
		dcb.Parity = pSettings->nParity;
		dcb.fParity = 0;
		dcb.StopBits = pSettings->nStop;
		//dcb.EvtChar = '\1';
		dcb.fOutX = FALSE;
		dcb.fInX = FALSE;
		//dcb.fAbortOnError = TRUE;

		if (!SetCommState(hComPort, &dcb) || hComPort == INVALID_HANDLE_VALUE)
			return FALSE;

		if (!SetCommMask(hComPort, EV_RXFLAG))
			return FALSE;
	}

	// Create the COM-event
	ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Wait for a COM-event.

	// Because we created the hComPort with overlapped I/O this function returns 
	// immediately.

	// The ov.hEvent is set to the active state if a char was received.
	WaitCommEvent(hComPort, &evMask, &ov);

	// Create the cancel event.
	// This event is set to active state if the user clicks on the cancel button.
	hCancel = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Create handle array.
	handles = new HANDLE[2];
	handles[0] = ov.hEvent;
	handles[1] = hCancel;

	thread = new thread_;
	thread->handles = handles;
	thread->hWnd = m_hWnd;

	// Start the thread which waits for an event to occur, either ov.hEvent or hCancel.
	AfxBeginThread(ComDaemon, thread);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// The thread send a message if an event occured.
void CCaptureDlg::OnMsg(UINT pParam, long lParam)
{
	int nRet;
	// We check if there was a character received
	if (evMask == EV_RXFLAG) {
		if (ReadScreen())
			nRet = CAPTURE_SUCCESS;
		else
			nRet = CAPTURE_FAILED;
	}
	else {
		nRet = CAPTURE_CANCELLED;
	}
	CloseHandle(ov.hEvent);
	CloseHandle(hComPort);
	delete [] handles;
	// The dialog returns the appropriate return value.
	EndDialog(nRet);	
}


// Worker-thread to wait for an event to occur.
UINT ComDaemon(LPVOID lParam)
{
	thread_ * info = (thread_ *)lParam;
	// Wait for an event forever. Either the hCancel or the ov.hEvent is set to 
	// active state.
	WaitForMultipleObjects(2, info->handles, FALSE, INFINITE);

	// Send the calling window a message that an event occured
	::PostMessage(info->hWnd, WM_COMDAEMON, 0, 0);
	return 0;
}


bool CCaptureDlg::ReadScreen()
{
	CWaitCursor wait;
	DWORD dwRead = 0;
	OVERLAPPED ov = {0};
	ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	COMMTIMEOUTS TimeOut;

	TimeOut.ReadIntervalTimeout = 100;
	TimeOut.ReadTotalTimeoutMultiplier = 4;
	TimeOut.ReadTotalTimeoutConstant = 0;
	TimeOut.WriteTotalTimeoutMultiplier = 0;
	TimeOut.WriteTotalTimeoutConstant = 0;
	
	SetCommTimeouts(hComPort, &TimeOut);

    // Read the first byte to determine what screen capture it is.
    char cType = 0;

	if (!ReadFile(hComPort, &cType, 1, &dwRead, &ov)) {
		 MessageBox("The image type was not received.", "ERROR", MB_OK|MB_ICONEXCLAMATION);

         return false;
	}

    unsigned int nBytesToRead = 0;
    data = 0;

    // Is it the HP48?
    if (cType == 27)
        nBytesToRead = 1080 - 1;
    else {
        // If it's the HP49, the first byte is the annunciators.  Save it
        // and read the next byte.
        unsigned char cAnnunciators = cType;

        if (!ReadFile(hComPort, &cType, 1, &dwRead, &ov)) {
            MessageBox("The HP49G image type was not received.", "ERROR", MB_OK|MB_ICONEXCLAMATION);

            return false;
        }

        if (cType == 'W')
            nBytesToRead = 64 * 17;	// HP49 B/W screenshot
        else if (cType == 'G')
            nBytesToRead = 64 * 17 * 2;	// HP49 grayscale screehshot.

#if 0
        // For some reason, we need to skip the next byte...
        if (!ReadFile(hComPort, &cAnnunciators, 1, &dwRead, &ov)) {
            MessageBox("The HP49G image filler was not received.", "ERROR", MB_OK|MB_ICONEXCLAMATION);

            return false;
        }
#endif
    }

    if (nBytesToRead == 0) {
        MessageBox("The image type could not be determined.", "ERROR", MB_OK|MB_ICONEXCLAMATION);

        return false;
    }

    data = new unsigned char [nBytesToRead+1];
    *data = cType;

	// attempt an asynchronous read operation
	BOOL fWaitingOnRead = FALSE;
	if (!fWaitingOnRead) {
		// Issue read operation.  Keep the first char we already read in to
		// determine the type of the screen shot.
		if (!ReadFile(hComPort, data + 1, nBytesToRead, &dwRead, &ov)) {
			DWORD dwError = GetLastError();
			if (dwError != ERROR_IO_PENDING) {   // read not delayed?
				 MessageBox("Could not read data", "ERROR", MB_OK|MB_ICONEXCLAMATION);
				 return false;
			}
			else
				fWaitingOnRead = TRUE;
		}
		else {    
			// Read in withouth delay. Unlikely, but possible.
			return true;
		}
	}

	const int READ_TIMEOUT = 4000;      // milliseconds

	DWORD dwRes;

	if (fWaitingOnRead) {
	   dwRes = WaitForSingleObject(ov.hEvent, READ_TIMEOUT);
	   switch(dwRes)
	   {
		  // Read completed.
		  case WAIT_OBJECT_0:
			if (!GetOverlappedResult(hComPort, &ov, &dwRead, FALSE)) {
				MessageBox("Could not read data", "ERROR", MB_OK|MB_ICONEXCLAMATION);
			}
			else {
			// Completed successfully.
				return true;
			}

			//  Reset flag so that another opertion can be issued.
			fWaitingOnRead = FALSE;
			break;

			default:
				MessageBox("Could not read data", "ERROR", MB_OK|MB_ICONEXCLAMATION);
	   }
	}

	return false;

}


unsigned char*CCaptureDlg::GetScreenData()
{
    return data;
}
