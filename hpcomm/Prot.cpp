// Prot.cpp: implementation of the CProt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HPComm.h"
#include "HPCommDoc.h"
#include "Prot.h"
#include "ProgressDlg.h"
#include "Settings.h"
#include "MainFrame.h"
#include "ProtException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


struct threadinfo {
	bool write;
	CProt * prot;
}; // I always forget this damn ;


// Threadfunctiondeclaration.
UINT transferthread(LPVOID lParam)
{
	threadinfo * ti = (threadinfo *)lParam;
	CProt::ERRCODE ret;
	if (ti->write) {
		ret = ti->prot->SendFiles();
	}
	else {
		ret = ti->prot->RecvFiles();
	}
	if (ret == CProt::SUCCESS)
		return ret;
	else
		return ret;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProt::CProt(CSettings * settings)
{
    pSettings = settings;

	MAXRP = 200;
	MAXSP = 200;
	bCancel = FALSE;
	ProtCallback = 0;
	bCancel = false;
	bSkipFile = false;
	hComPort = INVALID_HANDLE_VALUE;
	bConnected = false;

	nSpad = 0;
	cSpad = 0;
	sMark = '\1';
	nBlockChkType = 1;
	// nRecvPackLength = ???;
	// nRecvSeq = ???;
	rMark = '\1';
	nParity = NOPARITY;
	nMaxSendLen = 94;
	cRctlq = '#';
	nRecvQuot = 0;
	bEbqFlag = FALSE;
	nEbq = '&';
	nSentQuot = 'Y';
	nReqBlockChkType = 1;
	nRepeatQ = '~';
	nRepeatFlag = 0;
	nRepeat = 0;

	nMaxRecvLen = 94;
	nReqTimeOut = 7;
	nRpad = 0;

	cRpad = 0;
	cREOL = '\r';
	cSEOL = '\r';
	cSctlq = '#';

	nSeq = 0;
	nSeqAll = 0;

	nErrors = -1;
	
	nParity = NOPARITY;

	//MAXRP = 200;
	//MAXSP = 200;
	pfConnectNewCb = 0;
	pfConnectBumpCb = 0;
	pfConnectDeleteCb = 0;

	bArchive = false;

	bNotify = true;
}

CProt::~CProt()
{
	CloseComPort();
}

/////////////////////////////////////////////////////////////////////////////
// CProt message handlers

bool CProt::OpenPort()
{
	CString sPort;
	sPort.Format(_T("COM%d"), pSettings->nComPort);
	// DWORD dwError = 0L;

	//Try to open the port
	bool bSuccess = FALSE;
	hComPort = ::CreateFile(sPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if (hComPort == INVALID_HANDLE_VALUE) {
		// FIXME: Must work out what to do with the error code one day...
        // DWORD dwError = GetLastError();
		ThrowProtException(OPENPORT);
    }

    DCB dcb;
    // Set settings
    GetCommState(hComPort, &dcb);
    dcb.BaudRate = pSettings->dwBaud;
    dcb.ByteSize = pSettings->nByte;
    dcb.Parity = pSettings->nParity;
    dcb.fParity = (pSettings->nParity > 0);
    dcb.StopBits = pSettings->nStop;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    dcb.fAbortOnError = TRUE;
    FlushFileBuffers(hComPort);
    if (!SetCommState(hComPort, &dcb) || hComPort == INVALID_HANDLE_VALUE)
        return false;
    
    return SetTimeout(10); //Set timeout to 10 sec.
}

bool CProt::Connect(int nComPort /* = 0 */)
{
	// If the port is already set, but different, close it.
	CloseComPort();
	// Accept this value if they specified it.
	if (nComPort > 0)
		pSettings->nComPort = nComPort;

	return OpenPort();
}

bool CProt::SetTimeout(int nTime)
{
	COMMTIMEOUTS TimeOut;

	TimeOut.ReadIntervalTimeout = MAXDWORD;
	TimeOut.ReadTotalTimeoutMultiplier = MAXDWORD;
	TimeOut.ReadTotalTimeoutConstant = nTime * 1000;
	TimeOut.WriteTotalTimeoutMultiplier = 50;
	TimeOut.WriteTotalTimeoutConstant = 2000;

	return SetCommTimeouts(hComPort, &TimeOut) == TRUE;

}


void CProt::CloseComPort(bool bDisConnect /* = true */)
{
	// NT throws an exception if you try to close an invalid handle, Win9x
	// not.  So first check if the handle is valid.
	if (hComPort != INVALID_HANDLE_VALUE) {
		CloseHandle(hComPort);

        hComPort = INVALID_HANDLE_VALUE;
    }
	if (bDisConnect)
		bConnected = false;
}

BOOL CProt::PutCharToCom(char ch)
{
	unsigned long pnBytes;

	VERIFY(hComPort!=INVALID_HANDLE_VALUE);

	WriteFile(hComPort, &ch, 1, &pnBytes, NULL);
	return (pnBytes == 1);
}

void CProt::PutCharToBuffer(char ch)
{
	// Put the char to the buffer and raise the counter.
	nCount++;
	nBufLen++;
	*cursor++ = ch;
}

BOOL CProt::PutPacketToCom(char * s)
{
	if (hComPort == INVALID_HANDLE_VALUE) {
		if (!Connect())
			return FALSE;
	}

	unsigned long pnBytes;
	WriteFile(hComPort, s, strlen(s), &pnBytes, NULL);
	return (pnBytes == strlen(s));

}

BOOL CProt::PutBufferToCom(char * s, unsigned long len)
{
	VERIFY(hComPort!=INVALID_HANDLE_VALUE);

	unsigned long pnBytes;
	WriteFile(hComPort, s, len, &pnBytes, NULL);
	return (pnBytes == len);
}

BOOL CProt::GetCharFromCom(char * ch)
{
	if (hComPort == INVALID_HANDLE_VALUE) {
		if (!Connect())
			return FALSE;
	}

	unsigned long pnBytes;
	if (!bCancel)
		ReadFile(hComPort, ch, 1, &pnBytes, NULL);
	return (pnBytes == 1);
}

unsigned long CProt::GetBufferFromCom(char * s, unsigned long len)
{
	unsigned long i;
	if (bCancel)
		return 0;
	for (i = 0; i < len; i++) {
		if (!GetCharFromCom(s++)) {
			break;
		}
	}
	return i;
}


int CProt::GetLineFromCom()
{
	int i;
	char ch;

	if (bCancel)
		return 0;
	for (i = 0; i < MAXRP; i++) {
		if (!GetCharFromCom(&ch)) {
			//Timeout
			return -1;
		}
		RecvPack[i] = ch;
		if (ch == cREOL) {
			break;
		}
	}
	return i;
}

BOOL CProt::GetCharFromBuffer(char &ch)
{
	// Count all read bytes.
	static unsigned int cnt = 0;
	if (cnt++ >= nBufLen) {
		// We reached the end of the buffer.
		// Reset the counter
		cnt = 0;
		// Return FALSE to indicate the buffer is done.
		return FALSE;
	}
	// Counter for the progress dialog.
	nCount++;
	ch = *cursor++;
	return TRUE;	
}

CProt::ERRCODE CProt::Transfer(CFiler::IORequest * IOReq)
{
    if (IsConnected() == false)
		return UNKNOWN;

	bCancel = false;
	bSkipFile = false;
	POSITION pos = IOReq->list.GetHeadPosition();
	unsigned int nSize = 0;
	while (pos) {
		nSize += IOReq->list.GetNext(pos)->entry->Size();
	}
	CProgressDlg dlg;
	dlg.bCancel = &bCancel;
	dlg.bSkipFile = &bSkipFile;
	dlg.bSend = IOReq->write;
	ProtCallback = &(dlg.UpdateDlg);
	dlg.Create(IDD_PROGRESSDLG, AfxGetMainWnd());
	// Set the callback function.
	// Set the range of the progressbar for all files.
	ProtCallback(CProgressDlg::RANGEALL, &nSize);
	threadinfo ti;
	pIOReq = IOReq;
	ti.write = IOReq->write;
	ti.prot = this;
	DWORD exit;
	CWinThread * pThread = AfxBeginThread(transferthread, &ti, 0, CREATE_SUSPENDED, NULL);
	pThread->m_bAutoDelete = false;
	pThread->ResumeThread();

	// Hmm, not sure but here could occur a "deadlock". If the Window gets not messages
	// any more and we call Pump at the wrong time, Pump will wait will a new
	// message arrives. Very unlikely, but possible.
	while (WaitForSingleObject(pThread->m_hThread, 5) != WAIT_OBJECT_0) {
		dlg.Pump();
	}
	dlg.Close();
	if (!::GetExitCodeThread(pThread->m_hThread, &exit))
		((CHPCommApp *)AfxGetApp())->LastErrorMessage(GetLastError());
	
	// If the thread exited successfully, it returns SUCCESS;
	delete pThread;
	if (CProt::CANCELLED == (CProt::ERRCODE)exit) {
		SendPacket('E', 15, "FILES CANCELLED");
	}
	ERRCODE errcode = (CProt::ERRCODE)exit;
	if ((errcode != SUCCESS) && (errcode != RETRIES)) {
		ThrowProtException(errcode);
	}
	return errcode;
}

char * CProt::AllocateBuffer(unsigned int nLen)
{
	return new char[nLen];
}

void CProt::ReAllocateBuffer(char *&buf, unsigned int nNewLen)
{
	// Now we have the correct size of the buffer.

	// Allocate a new one
	char * newbuf = new char[nNewLen];

	// Copy the data across.
	memcpy(newbuf, buf, nNewLen);

	// delete the old one
	if (buf != 0)
		delete [] buf;

	// And point with buf to it.
	buf = newbuf;
}


void CProt::Notify(int nType, void *param)
{
	if (bNotify && (ProtCallback != 0)) {
		ProtCallback(nType, param);
	}
}


bool CProt::IsConnected()
{
	// VERIFY((hComPort == INVALID_HANDLE_VALUE) && (bConnected == true));
	// For some reason, the above asserts, even when hComPort is a
	// reasonable value.  So instead we simulate it.
	if (hComPort == INVALID_HANDLE_VALUE)
		if (bConnected == true)
			VERIFY(1==0);

    return (hComPort != INVALID_HANDLE_VALUE) && (bConnected == true);
}

void CProt::SetConnected()
{
	VERIFY(hComPort!=INVALID_HANDLE_VALUE);

	bConnected = true;
}

bool CProt::SetBinary(bool bBinary)
{
	// Set the binary flag of the calc only if it's different than
	// the current setting and if the calc is connected.
    if (bConnected && bBinary != pSettings->bBinary) {
		// Try to set the calc
        if (_SetBinary(bBinary)) {
			// If it was a success, we update the setting.
			pSettings->bBinary = bBinary;
			return true;
		}
   }
	return false;
}

bool CProt::IsBinary()
{
	return pSettings->bBinary;
}

bool CProt::ReadScreenData()
{
	unsigned char data[1048];
	unsigned long pnBytes;
	ReadFile(hComPort, data, 1000, &pnBytes, NULL);
	DWORD dwWritten;
	int x = 0x10;
	WriteFile(hComPort, &x, 1, &dwWritten, NULL);
	ReadFile(hComPort, data, 1000, &pnBytes, NULL);
	return pnBytes == 1048;
}


bool CProt::SendPacket(char cType, int len, char *d)
{
	int i = 0, j, k;
	for (i = 0; i < nSpad; i++)
		SendPack[i] = cSpad;

	SendPack[i++] = sMark;
	k = i;
	SendPack[i++] = ToChar(len + nBlockChkType + 2);
	SendPack[i++] = ToChar(nSeq);
	SendPack[i++] = cType;

	for (j = len; j > 0; j--) {
		SendPack[i++] = *d++;
	}

	SendPack[i] = '\0';

	switch (nBlockChkType) {
	case 1:
		SendPack[i++] = ToChar(Chkl(SendPack + k));
		break;
	case 2:
		j = CheckSum(SendPack + k);
		SendPack[i++] = ToChar((j >> 6) & 077);
		SendPack[i++] = ToChar(j & 077);
		break;
	case 3:
		j = CHK3(SendPack + k);
		SendPack[i++] = ToChar((j >> 12) & 017);
		SendPack[i++] = ToChar((j >> 6) & 077);
		SendPack[i++] = ToChar(j & 077);
		break;
	}

	SendPack[i++] = cSEOL;
	SendPack[i++] = '\0';
	// nSentPackLength = i;

	// Sende das Packet
	PutPacketToCom(SendPack);

	return true; //oder False

}


int CProt::RecvPacket()
{
	int i, j, x, rlnpos;
	char type, pbc[4];

	nRecvSeq = -1;
	nRecvPackLength = -1;

	*RecvPack = '\0';

	if (bCancel)
		return CANCELLED;
	if (bSkipFile)
		return SKIPPED;
	
	j = GetLineFromCom();
	if (j < 0)
		return ('T');
	for (i = 0; RecvPack[i] != rMark && (i < j); i++);
	
	if (i == j)
		return ('Q');

	rlnpos = ++i;

	nRecvPackLength = UnChar(RecvPack[i++]) - nBlockChkType - 2;
	nRecvSeq = UnChar(RecvPack[i++]);

	type = RecvPack[i++];

	pRecvData = RecvPack + i;

	j = nRecvPackLength + i;

	if (j > MAXRP)
		return ('Q');
	for ( x = 0; x < nBlockChkType; x++)
		pbc[x] = RecvPack[j + x];
	RecvPack[j] = '\0';

	switch (nBlockChkType) {
	case 1:
		if (UnChar(*pbc) != Chkl(RecvPack + rlnpos))
			return ('Q');
		break;
	case 2:
		x = UnChar(*pbc) << 6 | UnChar(pbc[1]);
		if ( x != CheckSum(RecvPack + rlnpos))
			return ('Q');
		break;
	case 3:
		x = UnChar(*pbc) << 12 | UnChar(pbc[1]) << 6 | UnChar(pbc[2]);
		if (x != CHK3(RecvPack + rlnpos))
			return ('Q');
		break;
	default:
		return ('Q');
		
	}

	Notify(CProgressDlg::PACKETNO, &nSeqAll);

	return (type);
}


bool CProt::RecvStack()
{
	char type;

	// Don't try to notify...
	bNotify = false;

	sStack.Empty();
	ERRCODE code;
	while (true) {
		code = PacketType(type);
		if (code != SUCCESS) {
			ThrowProtException(code);
			bNotify = true;
			return false;
		}
		if (type == 'Q') {
			Nak();
		}
		else if (type == 'S') {
			SetParam(pRecvData);
			Ack();
		}
		else if (type == 'D') {
			Ack();
			DecodeString();
			sStack = sStack + pRecvData;
		}
		else if (type == 'X') {
			Ack();
		}
		else if (type == 'Z') {
			Ack();
		}
		else if (type == 'B') {
			Ack();
			break;
		}
		else {
			bNotify = true;
			return false;
		}
	
	}
	
	bNotify = true;

	return true;

}


char CProt::ToChar(int x)
{
	return (x + 32);
}


int CProt::Chkl(char * packet)
{
	int s, t;
	s = CheckSum(packet);
	t = (((s & 192) >> 6) + s) & 63;
	return t;
}


int CProt::CheckSum(char * p)
{
	unsigned int m;
	long s;
	m = (nParity) ? 0177 : 0377;
	for (s = 0; *p != '\0'; *p++)
		s += *p & m;

	return (s & 07777);
}


LONG CProt::CHK3(char * s)
{
	unsigned int c, q;
	long crc = 0;
	while ((c = *s++) != '\0') {
		if (nParity) c &= 0177;
		q = (crc ^ c) & 017;
		crc = (crc >> 4) ^ (q * 010201);
		q = (crc ^ (c >> 4)) & 017;
		crc = (crc >> 4 ) ^ ( q * 010201);
	}
	return crc;
}


int CProt::UnChar(char c)
{
	return (c - 32);
}


CProt::ERRCODE CProt::PacketType(char& type)
{
	int tries;

	type = RecvPacket();

	if (bCancel)
		return CANCELLED;
	if (bSkipFile)
		return SKIPPED;

	for (tries = 0; nRecvSeq != nSeq || !strchr("FXDZBYSI", type); tries++) {
		// At startup only try 2 times to shorten the waiting time till the
		// program is available
		if (!IsConnected() && tries > 2) {
			return RETRIES;
		}
		if (tries > 5) {
			return RETRIES;
		}

		if (type == 'N' && nRecvSeq == ((nSeq + 1) & 63)) {
			type = 'Y';
			return SUCCESS;
		}

		else if (type == 'E') {
			CString sError = pRecvData;
			if (sError == "Invalid Syntax") {
				// Let the use know that the file he tried to send was not a valid one.
				//MessageBox(NULL, "Invalid Syntax\n\nThe file you tried to send does not\ncontain valid data or a valid program.", "Error Message from the calculator", MB_OK | MB_ICONSTOP);
				return SYNTAX;
			}
			else if ((sError.Find("Low Battery") != -1) || (sError.Find("Low battery") != - 1)) {
				// If the battery has low battery, we have to prevent HPComm do anything
				// more, because is causes a lot of trouble :-(
				return LOWBATTERY;
			}
			else if (sError == "Insufficient memory") {
				return MEMORY;
			}
			else {
				// Another error
				//MessageBox(NULL, pRecvData, "Error Message from the calculator", MB_OK | MB_ICONSTOP);
			}
			nSeq = 0;
			return ERRORPACKET;
		}

		else {
			if (bCancel)
				return CANCELLED;
			if (bSkipFile)
				return SKIPPED;
			nErrors++;
			Notify(CProgressDlg::ERRORS, &nErrors);
			Resend();
		}

		type = RecvPacket();
	}

	SetConnected();

	return SUCCESS;
}

int CProt::Nak()
{
	int x = SendPacket('N', 0, "");
	NextPacket();
	return x;
}

int CProt::Ack()
{
	int x = SendPacket('Y', 0, "");
	NextPacket();
	return x;
}


void CProt::SetParam(char * s)
{
	int x;

	s--;
	
	x = (nRecvPackLength >= 1) ? UnChar(s[1]) : 80;
	nMaxSendLen = (x < 10) ? 80 : x;

	x = (nRecvPackLength >= 2) ? UnChar(s[2]) : 5;
	int nTimeOut = (x < 0) ? 5 : x;

	SetTimeout(nTimeOut);

	nSpad = 0; cSpad = '\0';
	if (nRecvPackLength >= 3) {
		nSpad = UnChar(s[3]);
		if (nRecvPackLength >= 4)
			cSpad = Ctl(s[4]);
		else
			cSpad = 0;
	}
	
	cSEOL = (nRecvPackLength >= 5) ? UnChar(s[5]) : '\r';
	if ((cSEOL < 2) || (cSEOL > 31)) cSEOL = '\r';

	x = (nRecvPackLength >= 6) ? s[6] : '#';
	cRctlq = ((x > 32 && x < 63) || (x > 95 && x < 127)) ? x : '#';

	nRecvQuot = (nRecvPackLength >= 7) ? s[7] : 0;
	int nQuotFlag = 0;
	if (nRecvQuot == 'Y')
		nQuotFlag = 1;
	else if ((nRecvQuot > 32 && nRecvQuot < 63) || (nRecvQuot > 95 && nRecvQuot < 127))
		nQuotFlag = 2;

	switch (nQuotFlag) {
		case 0: bEbqFlag = FALSE; break;
		case 1: if (nParity) {bEbqFlag = TRUE; nEbq = '&';} break;
		case 2: if (bEbqFlag = (nEbq == nSentQuot || nSentQuot == 'Y')) nEbq = nRecvQuot;
	}



	x = 1;
	if (nRecvPackLength >= 8) {
		x = s[8] - '0';
		if ((x < 1) || (x > 3)) x = 1;
	}

	nReqBlockChkType = x;

	if (nRecvPackLength >= 9) {
		nRepeatQ = s[9];
		nRepeatFlag = ((nRepeatQ > 32 && nRepeatQ < 63) || (nRepeatQ > 95 && nRepeatQ < 127));
	}
	else
		nRepeatFlag = FALSE;
}


void CProt::DecodeString()
{
	int a7, b8, i = 0;
	char a, string[2200];

	memset(string, '\0', 1031);
	while ((a = *pRecvData++) != '\0') {
		nRepeat = 1;
		if (nRepeatFlag) {
			if (a == nRepeatQ) {
				nRepeat = UnChar(*pRecvData++);
				a = *pRecvData++;
			}
		}

		b8 = 0;
		if (bEbqFlag) {
			if (a == nEbq) {
				b8 = 128;
				a = *pRecvData++;
			}
		}

		if (a == cRctlq) {
			a = *pRecvData++;
			a7 = a & 127;
			if (a7 > 62 && a7 < 96)
				a = Ctl(a);
		}

		a |= b8;

		for(; nRepeat > 0; nRepeat--) {
			string[i++] = a;
		}
	}
	pRecvData = string;
}


int CProt::Resend()
{
	int x;
	if (*SendPack) {
		//Sende Pack
		x = PutPacketToCom(SendPack);
	}
	else
		x = Nak();
	return x;
}


void CProt::NextPacket()
{
	nSeq = (nSeq + 1) & 63;
	nSeqAll++;
}


char CProt::Ctl(char c)
{
	return (c ^ 64);
}

bool CProt::rename(CString from, CString to)
{
	CString sProgLimiter1, sProgLimiter2;
	sProgLimiter1 = "«";
	sProgLimiter2 = "»";
	if (pSettings->nTransIO == 3) {
		sProgLimiter1 = "\\<<";
		sProgLimiter2 = "\\>>";
	}

	CString command;
	command = sProgLimiter1 + "'" + to + "'" + "'" + from + "' DUP RCL SWAP IFERR PURGE THEN PGDIR END SWAP STO" + sProgLimiter2 + "EVAL";
	nBlockChkType = 1;
	nSeq = 0;
	bool res = SendPacket('C', command.GetLength(), (LPTSTR)(LPCTSTR)command);
	res &= RecvStack();
	return (res == TRUE);

}

bool CProt::newFolder(CString sName)
{
	CString sCommand = "'";
	sCommand += sName;
	sCommand += "' CRDIR";
	nSeq = 0;
	SendPacket('C', sCommand.GetLength(), (LPTSTR)(LPCTSTR)sCommand);
	return (RecvStack() == TRUE);
}

void CProt::ThrowProtException(CProt::ERRCODE errcode)
{
	CProtException* pEx = new CProtException(errcode);
	throw pEx;
}

void CProt::SetHP(int nTransIO, int nCheckSum)
{
	CString sCommand;
	sCommand.Format("%d TRANSIO %d CKSM", nTransIO, nCheckSum);
	nSeq = 0;
	SendPacket('C', sCommand.GetLength(), (LPTSTR)(LPCTSTR)sCommand);
	RecvStack();
}

void CProt::Finish()
{
	nBlockChkType = 1;
	nSeq = 0;
	SendPacket('G', 1, "F");
}
