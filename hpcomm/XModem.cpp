// XModem.cpp: implementation of the XModem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HPComm.h"
#include "XModem.h"
#include "HPCommDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XModem::XModem(CSettings * settings)
: CProt(settings)
{
	SOH = '\1';
	STX = '\2';
	ACK = 6;
	EOT = 4;
	NAK = 0x15;
	CAN = 0x18;
	CRC = 'C';

	PADCHAR = '\0';
	bTimedOut = FALSE;
}

XModem::~XModem()
{

}

int XModem::SendFile()
{
	int i, chksum;
	char ans = 0, ch = 0;
	char bno = 1;
	int nCount = 1;
	BOOL bEOF = FALSE;

	FlushFileBuffers(hComPort);
	tries = 0;
	SetTimeout(60);

	if (!GetCharFromCom(&ch)) {
		MessageBox(NULL, "No response from HP48!\nHit the EVAL key to start the transaction.", "NO RESPONSE", MB_OK | MB_ICONEXCLAMATION);
		return TIMEOUT;
	}
	bCRC = (ch == 'D');
	nBufLen = (bCRC ? 1024:128);
	FlushFileBuffers(hComPort);
	SetTimeout(10);
	while (tries < 10 && !bEOF && ans != CAN) {
		if (bCancel || bSkipFile) {
			PutCharToCom(CAN);
			return CANCELLED;
		}
	
        memset(SendPack, PADCHAR, 1031);
#ifdef XMODEM_FIXME
		if ((len = GetBufferFromFile(SendPack, nBufLen)) < nBufLen)
			bEOF = TRUE;
		if (len == 0)
			break;
#endif
		chksum = 0;
		
		PutCharToCom(bCRC ? STX : SOH);
		PutCharToCom(bno);
		PutCharToCom(~bno);
		for (i = 0; i < nBufLen; i++) {
			PutCharToCom(SendPack[i]);
			chksum += SendPack[i];
		}

		nCount++;

		if (bCRC) {
			WORD CRC = CalcHPCRC(SendPack);
			PutCharToCom(CRC & 0xFF);
			PutCharToCom((CRC >> 8) & 0xFF);

		}
		else
			PutCharToCom(chksum);

		if (!GetCharFromCom(&ans))
			bTimedOut = TRUE;

		if (ans == ACK) {
			bno++;
			tries = 0;
		}
		else if (ans == NAK) {
			bEOF = FALSE;
			tries++;
			Resend();
		}
	}
	if (bEOF) {
		PutCharToCom(EOT);
		if (!GetCharFromCom(&ch) && ch == ACK) {
			PutCharToCom(EOT);
			return SUCCESS;
		}

	}
	if (bTimedOut) {
		bTimedOut = FALSE;
		return TIMEOUT;
	}

	
	return SUCCESS;

}

void XModem::Resend()
{
/*
	if (0xFFFFFFFF == SetFilePointer(hSendFile, -nBufLen, NULL, FILE_CURRENT))
		SetFilePointer(hSendFile, 0L, NULL, FILE_BEGIN);
*/
}

CProt::ERRCODE XModem::SendFiles()
{
	MessageBox(NULL, "XModem", "Xmodem", MB_OK);
/*
	SetTimeout(5);
	::PostMessage(hTransWnd, WM_MSGFROMTHREAD, BYTES, 0);
	for (int i = 0; i < pDoc->aFiles.GetSize(); i++) {
		if (!OpenSendFile(pDoc->aFiles.GetFileName(i))) {
			CString s = "Could not open file: " + pDoc->aFiles.GetFileName(i);
			MessageBox(NULL,  s, "ERROR", MB_OK | MB_ICONSTOP);
			return FILEERROR;
		}
		::PostMessage(hTransWnd, WM_MSGFROMTHREAD, FILENO, i);
		
		if (pDoc->calc == HP49) 
			SendFileEx(pDoc->aFiles.GetFileName(i), pDoc->aFiles.GetFileSize(i));
		else
			if (SendFile() != SUCCESS)
				continue;	// makes that sense??????

		CloseSendFile();

		if (IsCancelled())
			return CANCELLED;
	}
*/
	return SUCCESS;
}

CProt::ERRCODE XModem::RecvFiles()
{

	SetTimeout(5);
/*	::PostMessage(hTransWnd, WM_MSGFROMTHREAD, BYTES, 0);
	for (int i = 0; i < pDoc->aFiles.GetSize(); i++) {
		OpenRecvFile(pDoc->aFiles.GetFileName(i));
		::PostMessage(hTransWnd, WM_MSGFROMTHREAD, FILENO, i);
		int nRet = RecvFile();
		CloseRecvFile();
		if(nRet == FALSE) {
			DeleteFile(pDoc->aFiles.GetFileName(i));
			return UNKNOWN;
		}
		if (IsCancelled())
			return CANCELLED;
	}
*/
	return SUCCESS;
}

BOOL XModem::RecvFile()
{
	int blk = 0, i;
	char soh = 0, bn, nbn, ch, low, high;
	unsigned cs, chksum;
	BOOL crcin = TRUE, bFirst = TRUE;
	tries = 0;
	SetTimeout(3);
	bCRC = FALSE;
#ifdef FIXME_XMODEM
	for (i = 0; ( (i < 3) && !bCRC) && (pDoc->prot != XMODEM); i++) {
		if (pDoc->prot == XMODEMHP)
			PutCharToCom('D');
		else if (pDoc->prot == XMODEM1K)
			PutCharToCom('C');
		bCRC = (GetCharFromCom(&soh) && (soh == SOH || soh == STX));
	}
#endif
	if (!bCRC) {
		PutCharToCom(NAK);
		if (!GetCharFromCom(&soh) && (soh == SOH || soh == STX)) {
			bTimedOut = TRUE;
			MessageBox(NULL, "No response from HP48!\nHit the EVAL key to start the transaction.", "NO RESPONSE", MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}
	}

	tries = 0;
	while (tries < 10) {
		if (bCancel) {
			break;
		}
		if (!bFirst) {
			if (!GetCharFromCom(&soh)) {
				Nak();
				continue;
			}
			if (soh == CAN)
				break;
			if (soh == EOT) {
				Ack();
				break;
			}
		}
		if (soh == SOH)
			nBufLen = 128;
		else if (soh == STX)
			nBufLen = 1024;
		bFirst = FALSE;
		SetTimeout(1);
		GetCharFromCom(&bn);
		GetCharFromCom(&nbn);
		chksum = 0;
		memset(RecvPack, 1031, PADCHAR);
		for (i = 0; i < nBufLen; i++) {
			if (!GetCharFromCom(&ch)) {
				bTimedOut = TRUE;
				break;
			}
			RecvPack[i] = ch;
			chksum = (chksum +  (ch & 255)) & 255;
		}
		if (bTimedOut)
			break;
		if (!GetCharFromCom(&low)) {
			bTimedOut = TRUE;
			break;
		}
		if (bCRC) {
			if (!GetCharFromCom(&high)) {
				bTimedOut = TRUE;
				break;
			}
		}

		cs = (unsigned)low;
		cs &= 255;

		if (bn == blk) {
/*
			if (0xFFFFFFFF == SetFilePointer(hRecvFile, -128, NULL, FILE_CURRENT))
				SetFilePointer(hRecvFile, 0L, NULL, FILE_BEGIN);
*/
		}
		else if (bn != blk + 1) {
			Nak();
			break;
		}
		blk = bn;
		if ((nbn & 255) != (~blk & 255)) {
			Nak();
			continue;
		}
		if (bCRC) {
			if ((low & (high >> 8)) != CalcHPCRC(RecvPack)) {
				Nak();
				continue;
			}
		}
		else if (cs != chksum) {
			Nak();
			continue;
		}
		soh = bn = nbn = cs = 0;
		tries = 0;
#ifdef XMODEM_FIXME
		PutBufferToFile(RecvPack, nBufLen);
#endif
		if (bCancel)
			break;
		Ack();
	

	}
	if (bTimedOut) {
		Nak();
	}
	if (soh == EOT) {
		bTimedOut = FALSE;
		return TRUE;
		//FERTIG
	}
	else {
		MessageBox(NULL, "Transfer was aborted", "ERROR", MB_OK | MB_ICONSTOP);
	}

	bTimedOut = FALSE;
	return FALSE;


}

void XModem::Ack()
{
	PutCharToCom(ACK);
}

void XModem::Nak()
{
	tries++;
	PutCharToCom(NAK);
}

void XModem::TimedOut()
{
	MessageBox(NULL, "No response from HP", "ERROR", MB_OK | MB_ICONSTOP);
}

WORD XModem::CalcHPCRC(char * s)
{
	int a, b;
	WORD res = 0;
	for (unsigned int i = 1; i < strlen(s); i++) {
		a = (res ^ (s[i] & 0xF) & 0xF ) * 0x1081;
		b = res >> 4;
		res = (a ^ b ) & 0xFFFF;
		a = (res ^ ((s[i] >> 4) & 0xF) & 0xF ) * 0x1081;
		b = res >> 4;
		res = (a ^ b ) & 0xFFFF;
	}
	return res;
}

BOOL XModem::SendFileEx(CString sFileName, UINT uSize /* = 0*/)
{
	BOOL bRet;
	int nLen = sFileName.GetLength();
	if (uSize == 0) {
		bRet = SendCommand('G', sFileName, nLen);
	}
	else {
		CString sName = sFileName;
		char *s = sFileName.GetBuffer(nLen + 3);
		s[0] = uSize & 0xFF;
		s[1] = (uSize >> 8) & 0xFF;
		s[2] = (uSize >> 16) & 0xFF;
		memcpy(s + 3, sName, nLen);
		bRet = SendCommand('P', s, nLen + 3);
	}
	
	if (bRet) {
		return SendFile();
	}

	return FALSE;
}

BOOL XModem::ChangeDir(CString sDirName)
{
	return SendCommand('C', sDirName, sDirName.GetLength());
}

BOOL XModem::SendCommand(char cType, const char *data, int nLen)
{
	// First char is the command-type
	SendPack[0] = cType;

	char * senddata = SendPack + 1;
	int nCheckSum = 0;
	// copy the data and calculate checksum
	for (int i = 0; i < nLen; i++) {
		*senddata++ = *data;
		nCheckSum += *data++;
	}
	// add checksum to the senddata-packet
	*senddata++ = (nCheckSum & 0xFF);
	*senddata = '\0'; //Necessary ??
	// Send Packet
	// Packet length is nLen + cType + nCheckSum = nLen + 2
	PutBufferToCom(SendPack, nLen + 2);
	char res;
	if (!GetCharFromCom(&res)) {
		// No answer....
	}
	return (res == ACK);

	
}

BOOL XModem::ExecCommand(CString sCommand)
{
	return SendCommand('E', sCommand, sCommand.GetLength());
}

int XModem::GetFreeMem()
{
	if (SendCommand('M', "", 0)) {
		char space[20];
		int n;
		GetBufferFromCom(space, 20);
		sscanf("%d", space, &n);
		return n;
	}
	else
		return -1;
}

void XModem::Server()
{
	char soh;
	while (!bCancel) {
		GetCharFromCom(&soh);
		if (soh == SOH) {
			
		}
	}
}

BOOL XModem::ReadRemoteDir()
{
	if (SendCommand('L', "", 0)) {
			
	}
	return TRUE;
}

bool XModem::_SetBinary(bool bBinary)
{
	// XModem only transfers files in binary modes, so we do nothing.
	return false;
}

CProt::ERRCODE XModem::readRemoteDir(CString &sRemoteDir)
{
	return UNKNOWN;
}
