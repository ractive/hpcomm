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


// Kermit.cpp: implementation of the CKermit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Kermit.h"
#include "VD.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HANDLE CKermit::m_hStop = NULL;

CKermit::CKermit()
{
	MAXRP = 200;
	MAXSP = 200;
	bCancel = FALSE;
	ProtCallback = 0;
	bCancel = false;
	bSkipFile = false;
	bConnected = false;

	nSpad = 0;
	cSpad = 0;
	sMark = '\1';
	nBlockChkType = 1;
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

	MAXRP = 200;
	MAXSP = 200;

	bNotify = true;

	bClientRequest = false;

	pInfoDlg = 0;

	m_bTransfering = FALSE;

	m_hStop = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CKermit::~CKermit()
{
	if (pInfoDlg != 0) {
		pInfoDlg->Close();
		delete pInfoDlg;
		pInfoDlg = 0;
	}

	ClosePort();
}

void CKermit::Encode(char a, char next)
{
	int a7, b8;
	if (nRepeatFlag) {
		if (a == next) {
			if (++nRepeat < 94) {
				return;
			} 
			else if (nRepeat == 94) {
				Data[nSize++] = nRepeatQ;
				Data[nSize++] = ToChar(nRepeat);
				nRepeat = 0;
			}
		}
		else if (nRepeat == 1) {
			nRepeat = 0;
			Encode(a, -1);
			if (nSize <= nMaxDataLen)
				nOSize = nSize;
			nRepeat = 0;
			Encode(a, -1);
			return;
		}
		else if (nRepeat > 1) {
			Data[nSize++] = nRepeatQ;
			Data[nSize++] = ToChar(++nRepeat);
			nRepeat = 0;
		}
	
	}

	a7 = a & 127;
	b8 = a & 128;

	if (bEbqFlag && b8) {
		Data[nSize++] = nEbq;
		a = a7;
	}

	if (a7 < 32 || a7 == 127) {
		Data[nSize++] = cSctlq;
		a = Ctl(a);
	}
	else if (a7 == cSctlq)
		Data[nSize++] = cSctlq;
	else if ((bEbqFlag && a7) == nEbq)
		Data[nSize++] = cSctlq;
	else if (nRepeatFlag && a7 == nRepeatQ)
		Data[nSize++] = cSctlq;

	Data[nSize++] = a;
	Data[nSize] = '\0';


}


void CKermit::Decode()
{
	int a7, b8;
	char a;

	TRACE("Decoding pRecvData: %s\n", pRecvData);

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
			if (a7 > 62 && a7 < 96) {
				a = Ctl(a);
			}
		}

		a |= b8;

		for(; nRepeat > 0; nRepeat--) {
			PutCharToFile(a);
			nCount++;
		}
	}

	//Notify(CProgressDlg::FILEPOS, &nCount);
}


int CKermit::Ackl(char * s)
{
	int x = SendPacket('Y', strlen(s), s);
	NextPacket();
	return x;
}



int CKermit::GetPacket(int nMaxLen)
{
	int i;
	char next;
	static char c;
	static char remain[6] = {'\0', '\0', '\0', '\0', '\0', '\0', };

	if (nFirst == 1) {
		nFirst = 0;
		*remain = '\0';
		if (!GetCharFromFile(c)) {
			nFirst = -1;
			return (nSize = 0);
		}
		nCount++;
	}
	else if (nFirst == -1) {
		return (nSize = 0);
	}

	for (nSize = 0; (Data[nSize] = remain[nSize]) != '\0'; nSize++);
	*remain = '\0';

	nRepeat = 0;
	while (nFirst > -1) {
		if (!GetCharFromFile(next)) 
			nFirst = -1;
		nCount++;
		nOSize = nSize;
		Encode(c, next);
		c = next;

		if (nSize == nMaxLen)
			return nSize;

		if (nSize > nMaxLen) {
			for (i = 0; (remain[i] = Data[nOSize + i]) != '\0'; i++);
			nSize = nOSize;
			Data[nSize] = '\0';
			return nSize;
		}

	}
	
	return nSize;
}


char * CKermit::ReqPar()
{
	Data[1] = ToChar(nMaxRecvLen);
	Data[2] = ToChar(nReqTimeOut);
	Data[3] = ToChar(nRpad);
	Data[4] = Ctl(cRpad);
	Data[5] = ToChar('\r');
	Data[6] = '#';
	switch (nRecvQuot) {
		case -1:
		case 1: if (nParity) nEbq = nSentQuot = '&'; break;
		case 0:
		case 2: break;
	}
	Data[7] = nSentQuot;

	nChk = nReqBlockChkType;
	Data[8] = nChk + '0';

	Data[9] = '\0';
	return (Data+1);
}

BOOL CKermit::SendInit()
{
	char *s;
	s = ReqPar();
	nSeq = 0;
	return (SendPacket('S', strlen(s), s));
}

void CKermit::SendEOF()
{
	NextPacket();
	SendPacket('Z', 0, "");
}

void CKermit::SendEOT()
{
	NextPacket();
	SendPacket('B', 0, "");
}

void CKermit::SendFiles()
{
	nBlockChkType = 1;

	bSkipFile = false;
	bCancel = false;

	BOOL bStart = TRUE;
	char type;
	CString sName;
	nCount = 0;
	SendInit();
	state = sendfile;
	nSeqAll = 0;
	nErrors = -1;

	SetTimeout(1);

	try {
		PacketType(type);
	}
	catch (CProtException * pEx) {
		switch (pEx->error) {
		case CProtException::CANCELLED:
			Cancel();
		default:
			ThrowProtException(pEx);
		}
	}

	if (type == 'Y') {
		if (bStart) {
			SetParam(pRecvData);
			bStart = FALSE;
		}
		nFirst = 1;
		nMaxDataLen = nMaxSendLen - (nReqBlockChkType + 3);
		nBlockChkType = nReqBlockChkType;
		
		SendFile();
	}

	// All files are done. Send EndOfTransmission.
	SendEOT();
	
	PacketType(type);

	nSeq = 0;
}

void CKermit::SendFile()
{
	static int nFileNo = 0;

	CVD vd(sCurPCPath);
	if (!OpenFile()) {
		DWORD dwError = GetLastError();
		char type;
		// Nak and tell the calc that there's noting to send.
		Nak();
		// The calc will send an error packet and we catch
		// the exception here
		try {
			PacketType(type);
		}
		catch (CProtException * pEx) {
			if (pEx->error != CProtException::ERRORPACKET) {
				// Another error occured. Throw the exception again.
				ThrowProtException(pEx);
			}
			pEx->Delete();
			// The expected error packet arrived.
			// We will process normally which will send a FILEERROR exception...
		}

		// Check if the directory exists...
		

		if (!PathExists(sCurPCPath)) {
			ThrowProtException(CProtException::DIRERROR, dwError, sReqFileName);
			return;
		}

		// If the file is the HP38DIR.CUR we handle this separately
		if (vd.IsTheCurFile(sReqFileName)) {
			nBlockChkType = 1;
			nSeq = 0;
			ThrowProtException(CProtException::VDINIT, dwError, sReqFileName);
		}
		else if (vd.IsTheDirFile(sReqFileName)) {
			// This also happens if the current dir was not a VD
			// and the calc already initialized it with HP38DIR.CUR
			// If we naked the request for this file the calc also will
			// send it, so we just return.
			nBlockChkType = 1;
			nSeq = 0;
			ThrowProtException(CProtException::VDINIT, dwError, sReqFileName);
			return;
		}
		else {
			nBlockChkType = 1;
			nSeq = 0;
			ThrowProtException(CProtException::FILEERROR, dwError, sReqFileName);
			return;
		}
	}

	// FIXME: Need to make sure that directory handling is correct.

	unsigned int size = GetFileSize();

	CString sFileName;
	GetFileName(sFileName);

	Notify(sFileName, false);

	int x = EncodeString((LPTSTR)(LPCTSTR)sFileName);
	NextPacket();
	bool bFPack = true;
	SendPacket('F', x, Data);
	nFirst = 1;
	nCount = 0;
	
	char type;

	while (true) {
		try {
			PacketType(type);
		}
		catch (CProtException * pEx) {
			switch (pEx->error) {
			case CProtException::CANCELLED:
				Cancel();
			default:
				ThrowProtException(pEx);
			}
		}
		// The first Ack of the calc contains the file name the
		// calc uses.
		if (bFPack && type == 'Y') {
			// Copy the filename to a CString because it won't be valid
			// when the next packet arrives.
			CString sRecvAs = pRecvData;

			bFPack = false;
		}

		if (!SendData())
			break;
	}

	SendEOF();

	PacketType(type);
	
	CloseFile();
}

BOOL CKermit::PathExists(CString sPath)
{
	// Check if the directory still exists...
	CFileFind finder;
	if (sPath.Right(1) != "\\") {
		sPath += "\\";
	}
	sPath += "*.*";
	BOOL bFound = finder.FindFile(sPath);
	if (bFound) {
		// This must be called before calling IsDirectory()
		finder.FindNextFile();
	}

	finder.Close();

	return bFound;
}

int CKermit::EncodeString(char * s)
{
	int i, next;
	static int c;
	static char remain[6] = {'\0', '\0', '\0', '\0', '\0', '\0', };

	if (nFirst == 1) {
		nFirst = 0;
		*remain = '\0';
		c = *s++;
		if (c == '\0') {
			nFirst = -1;
			return (nSize = 0);
		}
	}
	else if (nFirst == -1) {
		return (nSize = 0);
	}

	for (nSize = 0; (Data[nSize] = remain[nSize]) != '\0'; nSize++);
	*remain = '\0';

	nRepeat = 0;
	while (nFirst > -1) {
		next = *s++;
		if (next == '\0') 
			nFirst = -1;
		nOSize = nSize;
		Encode(c, next);
		c = next;

		if (nSize == nMaxDataLen)
			return nSize;

		if (nSize > nMaxDataLen) {
			for (i = 0; (remain[i] = Data[nOSize + i]) != '\0'; i++);
			nSize = nOSize;
			Data[nSize] = '\0';
			return nSize;
		}

	}
	
	return nSize;
}

BOOL CKermit::SendData()
{
	int x;
	if ((x = GetPacket(nMaxDataLen)) == 0)
		return FALSE;
	NextPacket();
	//Notify(CProgressDlg::FILEPOS, &nCount);
	//Notify(sReqFileName, FALSE, nSeqAll);
	return SendPacket('D', x, Data);
}

void CKermit::RecvFiles()
{
	// FIXME: Need to put code in to save and restore the initial
	// directory, since RecvFile will change it according to the dir where
	// that entry is.

	char type;
	char *s;
	int nFileNo = 0;
	CString sName;

	nBufLen = 0;

	bSpecialNames = false;

	nCount = 0;
	state = recvfile;

	s = ReqPar();

	if (!bClientRequest) {
		nSeq = 0;
	}

	nSeqAll = 0;
	nErrors = -1;
	
	while (true) {
		try {
			PacketType(type);
		}
		catch (CProtException * pEx) {
			switch (pEx->error) {
			case CProtException::CANCELLED:
				Cancel();
			default:
				ThrowProtException(pEx);
			}
		}

		if (type == 'Q') {
			Nak();
		}
		else if (type == 'Y' && state == recvfile) {
			SetParam(pRecvData);
			nSeq = 0;
			SendPacket('R', sName.GetLength(), (LPTSTR)(LPCTSTR)sName);
			state = recvdata;

			unsigned int size = GetFileSize();

			Notify(sName, true);
		}
		
		else if (type == 'Y' && state == recvdata) {
			Ack();
		}

		else if (type == 'S') {
			SetParam(pRecvData);
			Ackl(ReqPar());
			nBlockChkType = nReqBlockChkType;
		}
		else if (type == 'D') {
			Ack();
			Decode();
		}
		else if (type == 'X') {
			Ack();
		}
		// The current file is finished.
		else if (type == 'Z') {
			Ack();

			CloseFile();

		}
		else if (type == 'B') {
			Ack();
			nBlockChkType = 1;
			nSeq = 0;
			break;
		}
		else if (type == 'F') {
			Ack();
			DecodeString();
			sReqFileName = pRecvData;

			Notify(sReqFileName, true);
			
			if (!CreateFile()) {
				// End the transaction...
				Nak();
				// Throw an exception.
				ThrowProtException(CProtException::FILEERROR, GetLastError(), sReqFileName);
			}
		}
	}

}

int CKermit::Ack()
{
	int x = SendPacket('Y', 0, "");
	NextPacket();
	return x;
}

int CKermit::CheckSum(char *p)
{
	unsigned int m;
	long s;
	m = (nParity) ? 0177 : 0377;
	for (s = 0; *p != '\0'; *p++)
		s += *p & m;

	return (s & 07777);
}

LONG CKermit::CHK3(char *s)
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

int CKermit::Chkl(char *packet)
{
	int s, t;
	s = CheckSum(packet);
	t = (((s & 192) >> 6) + s) & 63;
	return t;
}

char CKermit::Ctl(char c)
{
	return (c ^ 64);
}

void CKermit::DecodeString()
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

void CKermit::Finish()
{
	nBlockChkType = 1;
	nSeq = 0;
	SendPacket('G', 1, "F");
}

int CKermit::Nak()
{
	int x = SendPacket('N', 0, "");
	NextPacket();
	return x;
}

void CKermit::NextPacket()
{
	nSeq = (nSeq + 1) & 63;
	nSeqAll++;
}

int CKermit::RecvPacket(bool bFirstPacket /* = false */)
{
	int i, j, x, rlnpos;
	char type, pbc[4];

	nRecvSeq = -1;
	nRecvPackLength = -1;

	*RecvPack = '\0';

	if (bCancel) {
		ThrowProtException(CProtException::CANCELLED);
	}

	if (bSkipFile) {
		return CProtException::SKIPPED;
	}
	
	j = ReadLineFromCom();

	if (j < 0)
		return ('T');

	// When we read the first packet we have to add a '\0' at the beginning
	// of the packet, cause this '\0' was read by ClientRequest to determine
	// if it was a screenshot or a kermit-packet.
	if (bFirstPacket) {
		char temp[1031];
		memset(temp, 0, 1031);
		memcpy(temp + 2, RecvPack, j);
		temp[0] = cFirstChar;
		temp[1] = cSecondChar;
        j += 2;

        memcpy(RecvPack, temp, j);
        // !@#$% Do something with the second charactor
	}
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

	//Notify(sReqFileName, false, 1);

	return (type);
}

bool CKermit::RecvStack()
{
	char type;

	// Don't try to notify...
	bNotify = false;

	sStack.Empty();

	while (true) {
		PacketType(type);

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

	TRACE("Stack received\n");
	bNotify = true;

	return true;
}

int CKermit::Resend()
{
	int x;
	if (*SendPack) {
		//Sende Pack
		x = WriteBufferToCom(SendPack, strlen(SendPack));
	}
	else
		x = Nak();
	return x;
}

bool CKermit::SendPacket(char cType, int len, char *d)
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

	// Send the packet
	WriteBufferToCom(SendPack, strlen(SendPack));

	TRACE("Sent no.:     %d Type: %c SendPack: %s\n", nSeq, cType, SendPack);

	return true; //oder False
}

void CKermit::SetParam(char *s)
{
	int x;

	s--;
	
	x = (nRecvPackLength >= 1) ? UnChar(s[1]) : 80;
	nMaxSendLen = (x < 10) ? 80 : x;

	x = (nRecvPackLength >= 2) ? UnChar(s[2]) : 5;
	int nTimeOut = (x < 0) ? 5 : x;

	SetTimeout((int)(nTimeOut / 5));

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

char CKermit::ToChar(int x)
{
	return (x + 32);
}

int CKermit::UnChar(char c)
{
	return (c - 32);
}

void CKermit::PacketType(char &type, bool bFirstPacket /* = false */)
{
	int tries;

	type = RecvPacket(bFirstPacket);
	TRACE("Received no.: %d Type: %c RecvPack: %s\n", nRecvSeq, type, RecvPack);
	for (tries = 0; nRecvSeq != nSeq || !strchr("FXDZBYSIR", type); tries++) {

		if (tries > 5) {
			TRACE("Throwing RETRIES Exception");
			ThrowProtException(CProtException::RETRIES);
		}

		if (type == 'N' && nRecvSeq == ((nSeq + 1) & 63)) {
			type = 'Y';
			return;
		}

		else if (type == 'E') {
			CString sError = pRecvData;
			if (sError == "Invalid Syntax") {
				// Let the use know that the file he tried to send was not a valid one.
				//MessageBox(NULL, "Invalid Syntax\n\nThe file you tried to send does not\ncontain valid data or a valid program.", "Error Message from the calculator", MB_OK | MB_ICONSTOP);
				ThrowProtException(CProtException::SYNTAX);
			}
			else if ((sError.Find("Low Battery") != -1) || (sError.Find("Low battery") != - 1)) {
				// If the battery has low battery, we have to prevent HPComm do anything
				// more, because is causes a lot of trouble :-(
				ThrowProtException(CProtException::LOWBATTERY);
			}
			else if (sError == "Insufficient memory") {
				ThrowProtException(CProtException::MEMORY);
			}
			else {
				// Another error occured which we can't process.
				ThrowProtException(CProtException::ERRORPACKET, 0, sError);
			}
		}
		else {
			TRACE("Recived an Error-Packet\n");
			//TRACE("nSeq: %d nRecvSeq %d RecvPack %s:\n", nSeq, nRecvSeq, RecvPack);
			nErrors++;
			//Notify(CProgressDlg::ERRORS, &nErrors);
			Resend();
		}

		type = RecvPacket();
		TRACE("Received after error: %d, %c\n", nSeq, type);
	}

	return;
}

int CKermit::ReadLineFromCom()
{
	int i;
	char ch;

	if (bCancel) {
		ThrowProtException(CProtException::CANCELLED);
	}
	for (i = 0; i < MAXRP; i++) {
		if (!ReadCharFromCom(&ch)) {
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

void CKermit::Transfer(bool bRecv /* = false */)
{
	//CWaitCursor wait;
	bCancel = false;
	bSkipFile = false;
	
	unsigned int nSize = 0;
	threadinfo ti;

	ti.pEx = NULL;

	if (pInfoDlg == 0) {
		pInfoDlg = new CTransferInfo;
		pInfoDlg->m_sPath = sCurPCPath;
		pInfoDlg->Create(IDD_TRANSFERINFO, AfxGetMainWnd());
	}

	pInfoDlg->ShowInitScreen();

	HWND * hInfoWnd = &pInfoDlg->m_hWnd;

	ProtCallback = &(pInfoDlg->UpdateDlg);

	// This method also can be called with a Null-ptr to IOReq from within
	// the method ClientRequest.
	ti.write = bRecv;

	ti.pKermit = this;
	DWORD exit;
	CWinThread * pThread = AfxBeginThread(TransferThreadProc, &ti, 0, CREATE_SUSPENDED, NULL);
	pThread->m_bAutoDelete = false;
	pThread->ResumeThread();

	HANDLE * handleArray = new HANDLE[2];

	handleArray[0] = pThread->m_hThread;
	handleArray[1] = m_hStop;

	m_bTransfering = TRUE;

	// Hmm, not sure but here could occur a "deadlock". If the Window gets not messages
	// any more and we call Pump at the wrong time, Pump will wait will a new
	// message arrives. Very unlikely, but possible.
	DWORD dwSigHandle;
	while (dwSigHandle = WaitForMultipleObjects(2, handleArray, FALSE, 50)) {
		if (dwSigHandle == WAIT_TIMEOUT) {
			pInfoDlg->Pump();
			continue;
		}
		else if (dwSigHandle - WAIT_OBJECT_0 == 1) {
			bCancel = true;
			continue;
		}
		else if (dwSigHandle - WAIT_OBJECT_0 == 0) {
			break;
		}
	}

	WaitForSingleObject(pThread->m_hThread, INFINITE);

	delete [] handleArray;

	m_bTransfering = FALSE;

	if (!::GetExitCodeThread(pThread->m_hThread, &exit)) {
		ThrowProtException(CProtException::UNKNOWN, GetLastError());
	}
	
	delete pThread;

	if(!IsWindow(*hInfoWnd)) {
		pInfoDlg = 0;
	}

	if ((bTransactionFinished || bSendingOnly) && pInfoDlg) {
		pInfoDlg->Close();
		delete pInfoDlg;
		pInfoDlg = 0;
	}
	else if (bVDFilesSent && pInfoDlg) {
		pInfoDlg->ShowUserInfo();
	}

	// If the thread exited successfully, it returns SUCCESS;	
	
	CProtException::ERRCODE errcode = (CProtException::ERRCODE)exit;
	if (errcode != CProtException::SUCCESS) {
		// If the thread ended without success we check if we
		// have a non NULL exception pointer.
		// If yes, throw this exception, if not, throw one with
		// the corresponding error code.
		if (ti.pEx != NULL) {
			ThrowProtException(ti.pEx);
		}
		else {
			ThrowProtException(errcode);
		}
	}
}

void CKermit::Notify(CString &sFileName, bool bRecv, int nPackets)
{
	if (ProtCallback != 0) {
		ProtCallback(sFileName, bRecv, nPackets);
	}
}

void CKermit::ClientRequest(char cFirstChar, char cSecondChar, CString &sCurPath)
{
	bClientRequest = true;

	Reset(false);

	sCurPCPath = sCurPath;

    this->cFirstChar = cFirstChar;
    this->cSecondChar = cSecondChar;
	bool bFirstPacket = true;
    char type;
	while (true) {
		try {
			PacketType(type, bFirstPacket);
			bFirstPacket = false;
		}
		catch (CProtException * pEx) {
			CloseFile();
			if (pEx->error == CProtException::ERRORPACKET) {
				// We got an error message. This can be the case if the 38G
				// Wants to create the VD-files. We just listen to what will follow.
				nBlockChkType = 1;
				nSeq = nSeqAll = 0;
				pEx->Delete();
				break;
			}
			else {
				ThrowProtException(pEx);
			}
		}

		TRACE("Server Packet %d %c %s\n", nSeq, type, pRecvData);

		if (type == 'Q') {
			Nak();
		}
		else if (type == 'B') {
			Ack();
			break;
		}
		else if (type == 'I') {
			SetParam(pRecvData);
			Ackl(ReqPar());
			nSeq = 0;
			nSeqAll = 0;
		}
		else if (type == 'R') {
			DecodeString();
			sReqFileName = pRecvData;

			try {
				Transfer(true);
			}
			catch (CProtException * pEx) {
				if (pEx->error == CProtException::ERRORPACKET) {
					// We got an error message. This can be the case if the 38G
					// Wants to create the VD-files. We just listen to what will follow.
					pEx->Delete();
					nBlockChkType = 1;
					nSeq = nSeqAll = 0;
					break;
				}
				else
					ThrowProtException(pEx);
			}

			nSeq = 0;
			nBlockChkType = 1;
			break;
		}
		else if (type == 'S') {
			// TRACE("Client sends a File\n");
			if (!PathExists(sCurPCPath)) {
				SendError("Directory does not exist");
				ThrowProtException(CProtException::DIRERROR, -1, sReqFileName);
				return;
			}
			SetParam(pRecvData);
			nSeq = 0;
			Ackl(ReqPar());
			nBlockChkType = nReqBlockChkType;
			Transfer(false);
			break;
		}
		else if (type == 'X') {
			Nak();
		}
		else if (type == 'G') {
			// We don't support generic commands at the moment.
			Nak();
		}
	}

	bClientRequest = false;

}


void CKermit::Reset(bool bCloseInfoDlg /* = true */)
{
	CloseFile();

	bCancel = false;

	if (bCloseInfoDlg && (pInfoDlg != 0)) {
		pInfoDlg->Close();
		delete pInfoDlg;
		pInfoDlg = 0;
	}

	bTransactionFinished = false;
	bVDFilesSent = false;

	nBlockChkType = 1;
	nSeq = 0;
	nSeqAll = 0;

}

void CKermit::StopAll() {
	if (m_bTransfering) {
		SetEvent(m_hStop);
	}
	else if (pInfoDlg) {
		pInfoDlg->Close();
		delete pInfoDlg;
		pInfoDlg = 0;
	}
}

UINT CKermit::TransferThreadProc(LPVOID lParam)
{
	threadinfo * ti = (threadinfo *)lParam;
	try {
		if (ti->write) {
			ti->pKermit->SendFiles();
		}
		else {
			ti->pKermit->RecvFiles();	
		}
	}
	catch (CProtException * pEx) {
		ti->pEx = pEx;
		CProtException::ERRCODE error = pEx->error;
		return error;
	}

	return CProtException::SUCCESS;

}

void CKermit::SendError(CString sError)
{
	NextPacket();
	try {
		SendPacket('E', sError.GetLength(), (LPTSTR)(LPCTSTR)sError);
	}
	catch (CProtException * pEx) {
		pEx->sMessage;
		pEx->Delete();
	}
}

void CKermit::Cancel()
{
	char type;
	TRACE("CANCELLING...\r\n");
	bCancel = false;
	SendPacket('Z', 1, "D");
	type = RecvPacket();
	TRACE("AFTER DISCARD: %c\r\n", type);
	SendError("TRANSFER CANCELLED ON PC");
	type = RecvPacket();
	TRACE("AFTER ERRORPACKET: %c\r\n", type);
	ThrowProtException(CProtException::CANCELLED);
}
