// XModem.cpp: implementation of the CXModem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hpcomm38.h"
#include "XModem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXModem::CXModem()
{
	SOH = '\1';
	STX = '\2';
	ACK = '\6';
	EOT = '\4';
	NAK = 0x15;
	CAN = 0x18;
	CRC = 'C';

	PADCHAR = '\0';

	m_bIsXModem = TRUE;
}

CXModem::~CXModem()
{

}

BOOL CXModem::SendFile()
{
	CWaitCursor wait;
	m_ulBytesSent = 0;

	int i, chksum;
	char ans = 0, ch = 0;
	char bno = 1;
	int nCount = 1;
	BOOL bEOF = FALSE;
	BOOL bCRC;
	int len = 0;
	unsigned long nBytes = 0;

	// Ask the user for the name of the new file.
	OPENFILENAME openfilename;
	openfilename.lStructSize = sizeof(OPENFILENAME);
	openfilename.hwndOwner = AfxGetMainWnd()->m_hWnd;
	openfilename.hInstance = AfxGetInstanceHandle();
	openfilename.lpstrFilter = NULL;
	openfilename.lpstrCustomFilter = NULL;
	openfilename.nMaxCustFilter = 0;
	openfilename.nFilterIndex = 0;
	openfilename.lpstrFile = sReqFileName.GetBuffer(_MAX_FNAME);;
	openfilename.nMaxFile = MAX_PATH;
	openfilename.lpstrFileTitle = NULL;
	openfilename.nMaxFileTitle = 0;
	openfilename.lpstrInitialDir = m_sCurPCPath /*.GetBuffer(_MAX_DIR)*/;
	openfilename.lpstrTitle = "Send file with XModem...";
	openfilename.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	openfilename.nFileOffset = 0;
	openfilename.nFileExtension = 0;
	openfilename.lpstrDefExt = NULL;
	openfilename.lCustData = 0;
	openfilename.lpfnHook = NULL;
	openfilename.lpTemplateName = NULL;
	BOOL bRet = GetOpenFileName(&openfilename);
    
	sReqFileName.ReleaseBuffer();
	//m_sDropTarget.ReleaseBuffer();
	
	if (bRet != IDOK) {
		Cancel(); // FIXME!!!
	}
    
	m_sCurPCPath = sReqFileName.Left(sReqFileName.ReverseFind('\\'));
	sReqFileName = sReqFileName.Mid(sReqFileName.ReverseFind('\\') + 1);
	
	FlushFileBuffers(hComPort);
	m_nTries = 0;
	SetTimeout(60);
	/*sCurPCPath = "D:\\Eigene Dateien\\HP38\\39bivarodd";
	sReqFileName = "BIVARODD.APT";*/
	
	if (!OpenFile()) {
		ThrowProtException(CProtException::FILEERROR, GetLastError());
	}

	unsigned int size = GetFileSize();
	//if (SetRange) {
		SetRange(size);
	//}

	FileInfo(sReqFileName, FALSE);

	if (!ReadCharFromCom(&ch)) {
		ThrowProtException(CProtException::TIMEOUT);
	}
	bCRC = (ch == 'D');
	m_nBufLen = (bCRC ? 1024:128);
	FlushFileBuffers(hComPort);
	SetTimeout(10);
	while (m_nTries < 10 && !bEOF && ans != CAN) {
		if (m_bCancel || m_bSkipFile) {
			Cancel();
			return FALSE;
		}
	
        memset(SendPack, PADCHAR, 1031);

		if ((len = GetBufferFromFile(SendPack, m_nBufLen)) < m_nBufLen)
			bEOF = TRUE;

		if (len == 0)
			break;

		chksum = 0;
		
		SendChar(bCRC ? STX : SOH);
		SendChar(bno);
		SendChar(~bno);
		for (i = 0; i < m_nBufLen; i++) {
			SendChar(SendPack[i]);
			nBytes++;
			chksum += SendPack[i];
		}

		nCount++;

		if (bCRC) {
			WORD CRC = CalcHPCRC(SendPack);
			SendChar(CRC & 0xFF);
			SendChar((CRC >> 8) & 0xFF);

		}
		else {
			SendChar(chksum);
		}

		if (!ReadCharFromCom(&ans)) {
			ThrowProtException(CProtException::TIMEOUT);
		}

		if (ans == ACK) {
			//if (SetPos) {
				SetPos(nBytes, m_ulBytesSent);
			//}
			bno++;
			m_nTries = 0;
		}
		else if (ans == NAK) {
			bEOF = FALSE;
			m_nTries++;
			nBytes -= m_nBufLen;
			Resend();
		}
	}

	if (ans == CAN) {
		ThrowProtException(CProtException::CANCELLED);
	}

	//if (SetPos) {
		SetPos(nBytes, m_ulBytesSent);
	//}

	if (bEOF) {
		SendChar(EOT);
		if (!ReadCharFromCom(&ch) && ch == ACK) {
			SendChar(EOT);
			return TRUE;
		}

	}

	return TRUE;
}

WORD CXModem::CalcHPCRC(char *s)
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

void CXModem::Ack()
{
	SendChar(ACK);
}

void CXModem::Nak()
{
	m_nTries++;
	SendChar(NAK);
}

void CXModem::Resend()
{
	SetPointerBack(m_nBufLen);
}

void CXModem::SendFiles()
{
	SendFile();
}

void CXModem::RecvFiles()
{
	// Ask the user for the name of the new file.
	OPENFILENAME openfilename;
	openfilename.lStructSize = sizeof(OPENFILENAME);
	openfilename.hwndOwner = AfxGetMainWnd()->m_hWnd;
	openfilename.hInstance = AfxGetInstanceHandle();
	openfilename.lpstrFilter = NULL;
	openfilename.lpstrCustomFilter = NULL;
	openfilename.nMaxCustFilter = 0;
	openfilename.nFilterIndex = 0;
	openfilename.lpstrFile = sReqFileName.GetBuffer(_MAX_FNAME);;
	openfilename.nMaxFile = MAX_PATH;
	openfilename.lpstrFileTitle = NULL;
	openfilename.nMaxFileTitle = 0;
	openfilename.lpstrInitialDir = m_sCurPCPath /*.GetBuffer(_MAX_DIR)*/;
	openfilename.lpstrTitle = "Get file with XModem...";
	openfilename.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	openfilename.nFileOffset = 0;
	openfilename.nFileExtension = 0;
	openfilename.lpstrDefExt = NULL;
	openfilename.lCustData = 0;
	openfilename.lpfnHook = NULL;
	openfilename.lpTemplateName = NULL;
	BOOL bRet = GetSaveFileName(&openfilename);

	sReqFileName.ReleaseBuffer();
	//m_sDropTarget.ReleaseBuffer();
	
	if (bRet != IDOK) {
		Cancel(); // FIXME!!!
	}
    
	m_sCurPCPath = sReqFileName.Left(sReqFileName.ReverseFind('\\'));
	sReqFileName = sReqFileName.Mid(sReqFileName.ReverseFind('\\') + 1);
	if (!CreateFile()) {
		ThrowProtException(CProtException::FILEERROR, GetLastError());
	}


	int blk = 0, i;
	char soh = 0, bn, nbn, ch, low, high;
	unsigned cs, chksum;
	BOOL crcin = TRUE, bFirst = TRUE;
	m_nTries = 0;
	SetTimeout(60);
	BOOL bCRC = FALSE;
	char RecvPack[2048];
	// Only use "Checksum" for now...
	/*for (i = 0; ( (i < 3) && !bCRC) && (pDoc->prot != XMODEM); i++) {
		if (pDoc->prot == XMODEMHP)
			PutCharToCom('D');
		else if (pDoc->prot == XMODEM1K)
			PutCharToCom('C');
		bCRC = (GetCharFromCom(&soh) && (soh == SOH || soh == STX));
	}*/

	FlushComPort();

	for (i = 0; i < 10; i++) {
		SendChar(NAK);
		if (!ReadCharFromCom(&soh) || (soh != SOH)) {
			continue;
		}
		else {
			break;
		}
	}

	while (m_nTries < 10) {
		if (m_bCancel) {
			break;
		}
		if (!bFirst) {
			if (!ReadCharFromCom(&soh)) {
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
			m_nBufLen = 128;
		else if (soh == STX)
			m_nBufLen = 1024;
		
		bFirst = FALSE;
		SetTimeout(1);
		ReadCharFromCom(&bn);
		ReadCharFromCom(&nbn);
		chksum = 0;
		memset(RecvPack, 1031, PADCHAR);
		for (i = 0; i < m_nBufLen; i++) {
			if (!ReadCharFromCom(&ch)) {
				ThrowProtException(CProtException::TIMEOUT, 0);
			}
			RecvPack[i] = ch;
			chksum = (chksum +  (ch & 255)) & 255;
		}
		
		if (!ReadCharFromCom(&low)) {
			ThrowProtException(CProtException::TIMEOUT, 0);
		}

		if (bCRC) {
			if (!ReadCharFromCom(&high)) {
				ThrowProtException(CProtException::TIMEOUT, 0);
			}
		}

		cs = (unsigned)low;
		cs &= 255;

		if (bn == blk) {
			SetPointerBack(m_nBufLen, FALSE);
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
		m_nTries = 0;

		if (PutBufferToFile(RecvPack, m_nBufLen) != m_nBufLen) {
			ThrowProtException(CProtException::FILEERROR, GetLastError());
		}

		if (m_bCancel) {
			ThrowProtException(CProtException::CANCELLED, 0);
		}

		Ack();
	

	}

	if (soh == EOT) {
		return;
		//FERTIG
	}

	ThrowProtException(CProtException::UNKNOWN, 0);
}

void CXModem::Cancel()
{
	TRACE("CANCELLING...\r\n");
	m_bCancel = FALSE;
	SendChar(CAN);
	SendChar(CAN);
	ThrowProtException(CProtException::CANCELLED);
}

void CXModem::Reset(bool bCloseInfoDlg /* = true */)
{
	CloseFile();

	m_bCancel = false;

	CloseTransferWnd();

	m_bTransactionFinished = false;
	m_bVDFilesSent = false;

}

void CXModem::ClientRequest(char cFirstChar, char cSecondChar, CString &sCurPath)
{
	Reset(false);

	m_sCurPCPath = sCurPath;

	SendFile();
}

