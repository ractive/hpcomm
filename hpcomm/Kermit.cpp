// Kermit.cpp: implementation of the CKermit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HPComm.h"
#include "Kermit.h"
#include "HPCommDoc.h"
#include "Settings.h"
#include "MainFrame.h"

//Very bad to include the whole thing. Only the defs are needed.
#include "ProgressDlg.h"  

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKermit::CKermit(CSettings * settings)
: CProt(settings)
{
}

CKermit::~CKermit()
{
}

bool CKermit::Connect(int nComPort)
{
	bool res;
	if (pfConnectNewCb != 0)
		pfConnectNewCb(nComPort);
	
	// The old while trick.
	while (1) {
		nBlockChkType = 1;
		res = CProt::Connect(nComPort);

		if (pfConnectBumpCb != 0)
			pfConnectBumpCb(TRYOPENPORT, res);

		if (res == false)
			break;

		res = SendPathRequest();

		if (pfConnectBumpCb != 0)
			pfConnectBumpCb(CONNECTTOCALC, res);
		
		if (res == false) {
			bConnected = false;
			break;
		}

		res = CheckMode();
		if (res == false) {
			bConnected = false;
			break;		
		}

		if (pfConnectBumpCb != 0)
			pfConnectBumpCb(GETTINGCALCINFO, res);
		
		break;
	}

	return res;
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


BOOL CKermit::Decode()
{
	int a7, b8;
	char a;

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
			PutCharToBuffer(a);
		}
	}
	Notify(CProgressDlg::FILEPOS, &nCount);

	return TRUE;
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
		if (!GetCharFromBuffer(c)) {
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
		if (!GetCharFromBuffer(next)) 
			nFirst = -1;
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

	pSettings->nChk = nReqBlockChkType;
	Data[8] = pSettings->nChk + '0';

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

CProt::ERRCODE CKermit::SendFiles()
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

	POSITION pos = pIOReq->list.GetHeadPosition();
	ERRCODE code;
	while (pos != 0) {
		code = PacketType(type);
		CFiler::IOBuffer *buff = pIOReq->list.GetAt(pos);
		buff->success = false;
		
		if (code == SKIPPED) {
			NextPacket();
			SendPacket('Z', 1, "D");
			// Set the filepos so as if the whole file had been
			// transmitted that the "All files" progress bar is shown
			// correctly.
			unsigned long filepos = pIOReq->list.GetAt(pos)->entry->Size();
			Notify(CProgressDlg::FILEPOS, &filepos);
			state = sendfile;
		}
		else if (code == CANCELLED) {
			NextPacket();
			SendPacket('Z', 1, "D");
			Sleep(1000);
			SendEOT();
		}

		else if (code != SUCCESS) {
			return code;
		}

		if (type == 'Y') {
			if (bStart) {
				SetParam(pRecvData);
				bStart = FALSE;
			}
			nFirst = 1;
			nMaxDataLen = nMaxSendLen - (nReqBlockChkType + 3);
			nBlockChkType = nReqBlockChkType;
			// Set up the bufferlength so that GetCharFromBuffer can count
			// and terminate.
			nBufLen = buff->entry->Size();
			// Set the right buffer pointer
			cursor = buf = buff->buffer;
			CProt::ERRCODE code = SendFile(buff->entry);
			// These error are quite bad and we cannot process another file.
			if (code != SUCCESS) {
				return code;
			}
			buff->success = true;
		}

		else
			return DEVICE;
	
		pIOReq->list.GetNext(pos);
	}

	if (bCancel) {
		return CANCELLED;
	}

	// All files are done. Send EndOfTransmission.
	SendEOT();
	
	code = PacketType(type);

	if ((code == SUCCESS) && (type == 'Y')) {
		nSeq = 0;
		return SUCCESS;
	}
	else {
		return code;
	}
}

CProt::ERRCODE CKermit::SendFile(CFilerEntry * pEntry)
{
	static int nFileNo = 0;

	// FIXME: Need to make sure that directory handling is correct.

	unsigned int size = pEntry->Size();
	Notify(CProgressDlg::RANGE, &size);
	CString name = pEntry->Name();
	Notify(CProgressDlg::FILENAME, &name);

	int x = EncodeString((LPTSTR)(LPCTSTR)pEntry->Name());
	NextPacket();
	bool bFPack = true;
	SendPacket('F', x, Data);
	nFirst = 1;
	nCount = 0;
	
	char type;
	ERRCODE code;
	while (true) {
		code = PacketType(type);
		// The first Ack of the calc contains the file name the
		// calc uses.
		if (bFPack && type == 'Y') {
			// Copy the filename to a CString because it won't be valid
			// when the next packet arrives.
			CString sRecvAs = pRecvData;
			Notify(CProgressDlg::HPFILENAME, &sRecvAs);
			// Rename the entry.
			pEntry->Name(sRecvAs);
			bFPack = false;
		}
		if (code == SKIPPED) {
			NextPacket();
			SendPacket('Z', 1, "D");
			// Set the filepos so as if the whole file had been
			// transmitted that the "All files" progress bar is shown
			// correctly.
			unsigned long filepos = pEntry->Size();
			Notify(CProgressDlg::FILEPOS, &filepos);
			// Take care that the next file is not skipped, too.
			bSkipFile = false;
			return SKIPPED;
		}
		else if (code == CANCELLED) {
			NextPacket();
			SendPacket('Z', 1, "D");
			Sleep(1000);
			SendEOT();
		}
		
		else if (code != SUCCESS) {
			return code;
		}

		if (!SendData())
			break;
	}
	SendEOF();

	code = PacketType(type);

	return code;
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
	Notify(CProgressDlg::FILEPOS, &nCount);
	Notify(CProgressDlg::PACKETNO, &nSeqAll);
	return SendPacket('D', x, Data);
}



/*
void CKermit::UpDir()
{
	
	sCurCalcDir = sCurCalcDir.Left(sCurCalcDir.ReverseFind('\\'));
	sCurCalcDir = sCurCalcDir.Left(sCurCalcDir.ReverseFind('\\') + 1);
    nSeq = 0;
    //SendPacket('C', 5, "UPDIR");
	//RecvStack();
}
*/
CProt::ERRCODE CKermit::RecvFiles()
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

	bNotify = false;
	HandleSpecialNames();
	bNotify = true;

	nBufLen = 0;

	POSITION pos = pIOReq->list.GetHeadPosition();
	CFiler::IOBuffer *buff = pIOReq->list.GetNext(pos);
	buff->success = false;

	nCount = 0;
	state = recvfile;
	CString sEntryName = buff->entry->Name();
	sName = GetMappedName(sEntryName);

	s = ReqPar();

	nSeq = 0;
	nSeqAll = 0;
	nErrors = -1;
	
	// The I packet only has to be sent if we request a recive from
	// scratch.  The archive operation starts the sending of the file
	// itself, so we don't need it.
	if (!bArchive)
		SendPacket('I', strlen(s), s);

	ERRCODE code;
	while (true) {
		code = PacketType(type);
		if (code == CANCELLED) {
			SendPacket('E', 15, "FILES CANCELLED");
		}

		if (code != SUCCESS) {
			return code;
		}
		if (type == 'Q') {
			Nak();
		}
		else if (type == 'Y' && state == recvfile) {
			SetParam(pRecvData);
			nSeq = 0;
			SendPacket('R', sName.GetLength(), (LPTSTR)(LPCTSTR)sName);
			state = recvdata;
			Notify(CProgressDlg::FILENAME, &sName);
			unsigned int size = buff->entry->Size();
			Notify(CProgressDlg::RANGE, &size);
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

			ReAllocateBuffer(buf, nBufLen);
			buff->entry->Size(nBufLen);
			buff->buffer = buf;
			buff->success = true;

			// Was that the last file?
			if (pos != NULL) {
				// We have some files left.
				buff = pIOReq->list.GetNext(pos);
				buff->success = false;
				CString sEntryName = buff->entry->Name();
				sName = GetMappedName(sEntryName);
				nSeq = 0;
				SendPacket('R', sName.GetLength(), (LPTSTR)(LPCTSTR)sName);
				nCount = 0;
				nBufLen = 0;
				Notify(CProgressDlg::FILENAME, &sName);
				unsigned int size = buff->entry->Size();
				Notify(CProgressDlg::RANGE, &size);
			}
		}
		else if (type == 'B') {
			Ack();
			nBlockChkType = 1;
			break;
		}
		else if (type == 'F') {
			Ack();
			DecodeString();
			cursor = buf = AllocateBuffer(pIOReq->biggestObject); // Size!!!!!!

		}
	}

	// Now we have to rename the files back to the original names.

	DeleteMapList();

	if (bSpecialNames) {
		CString sCommand = "\"RENAME BACK\" rmHelper";
		nSeq = 0;
		nBlockChkType = 1;
		SendPacket('C', sCommand.GetLength(), (LPTSTR)(LPCTSTR)sCommand);
		RecvStack();
		bSpecialNames = false;
	}

	return SUCCESS;
}


void CKermit::Archive(CString sArchive)
{
	bArchive = TRUE;
	nBlockChkType = 1;
	nSeq = 0;
	CString sProgLimiter1, sProgLimiter2;
	sProgLimiter1 = "«";
	sProgLimiter2 = "»";
	if (pSettings->nTransIO == 3) {
		sProgLimiter1 = "\\<<";
		sProgLimiter2 = "\\>>";
	}
	sArchive = ":IO:" + sArchive + " " + sProgLimiter1 + "ARCHIVE SERVER" + sProgLimiter2;

	SendPacket('C', sArchive.GetLength(), (LPTSTR)(LPCTSTR)sArchive);
	RecvStack();
	Sleep(700);
	FlushFileBuffers(hComPort);
	Finish();
}

bool CKermit::SendPathRequest()
{
	CWaitCursor wait;
	CString sTransIO;
	char send[] = "-35 FS? IOPAR 5 GET IOPAR 6 GET";
	nSeq = 0;
	SendPacket('C', strlen(send), send);
	SetTimeout(2);
	if (!RecvStack())
		return false;
	else
		return true;
}

bool CKermit::CheckMode()
{
	sStack.TrimRight();
	sscanf(sStack.Right(1), "%d", &pSettings->nTransIO);
	sStack = sStack.Left(sStack.ReverseFind('\n') - 1);
	sscanf(sStack.Right(1), "%d", &pSettings->nChk);
	sStack = sStack.Left(sStack.ReverseFind('\n') + 1);
	pSettings->bBinary = (sStack.Right(3) == "1\r\n");
	nSeq = 0;
	SendPacket('C', 7, "3 DROPN");
	return RecvStack();
}


bool CKermit::_SetBinary(bool bBinary)
{
    char s[8] = "-35 ";

	if (bBinary)
		strcat(s, "SF");
	else
		strcat(s, "CF");

	nSeq = 0;
	SendPacket('C', strlen(s), s);
	return RecvStack();
}

void CKermit::Restore(const CString &sName)
{
	CString sFileName = "'" + sName + "'" + " RCL RESTORE";
	nBlockChkType = 1;
	nSeq = 0;
	Sleep(300);
	SendPacket('C', sFileName.GetLength(), (LPTSTR)(LPCTSTR)sFileName);
}

CProt::ERRCODE CKermit::readRemoteDir(CString &sRemoteDir)
{
	ProtCallback = 0;
	//CString sRead = "Reading " + pSettings->sCurCalcDir + " ...";
	//CString sRemoteDir;
	nBlockChkType = 1;
	SetTimeout(1);
	nSeq = 0;
	SendPacket('G', 1, "D");
	
	char type;
	ERRCODE code;
	while (true) {
		if ((code = PacketType(type)) != SUCCESS) {
			return code;
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
			sRemoteDir = sRemoteDir + pRecvData;
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
		else
			return UNKNOWN;
	}

	return SUCCESS;
}

bool CKermit::cp(bool removeAfterCopy, CFiler * fromFiler, CFilerEntryList *fromList, CFilerEntry *to)
{
	CFiler::IORequest IOReq;
	CFiler::IOBuffer buff;

	CString sDest = fromFiler->toString(to);

	sDest = "{" + sDest + "}";

	CString sList;

	CString sCurDir = "{" + fromFiler->toString(fromFiler->currentDir()) + "}"; 

	sList = "%%HP: T(3)A(D)F(.);\\<< PATH ";
	sList = sList + "{";
	POSITION pos = fromList->GetHeadPosition();
	while (pos) {
		sList = sList + fromList->GetNext(pos)->Name() + " ";	
	}
	sList = sList + "} ";
	sList = sList + sDest + " " + sCurDir + " \\-> L D P \\<<P EVAL L SIZE 1 FOR A L A GET DUP ";
	if (removeAfterCopy)
		sList += "DUP ";
	
	sList += "RCL ";
	
	if (removeAfterCopy)
		sList += "SWAP IFERR PURGE THEN PGDIR END";

	sList += " D EVAL SWAP STO P EVAL -1 STEP \\>> EVAL \\>>";

	buff.buffer = sList.GetBuffer(sList.GetLength());

	const char * tmpfilename = "hpcpxxxx.tmp";
	
	CFilerEntry cptmp(tmpfilename, false, sList.GetLength());
	buff.entry = &cptmp;
	IOReq.list.AddHead(&buff);
	IOReq.write = false;
	pIOReq = &IOReq;
	SendFiles();
	sList.Empty();

	sList = tmpfilename;
	sList += " '";
	sList += tmpfilename;
	sList += "' PURGE";

	nBlockChkType = 1;
    nSeq = 0;
	nSeqAll = 0;
    SendPacket('C', sList.GetLength(), (LPTSTR)(LPCTSTR)sList);
	RecvStack();
	sList.ReleaseBuffer();
    return true;	
}

bool CKermit::hasEightbitChar(CString &sName, CString& sNewName)
{
	bool bRet = false;
	for (int i = 0; i < sName.GetLength(); i++) {
		unsigned char ch;
		if ((ch = (unsigned char) sName[i]) >= 127) {
			// This is a 8bit char.
			bRet = true;
		}
		else {
			// The 7 bit chars are taken for the new name.
			sNewName += ch;
		}
	}

	return bRet;
}

CProt::ERRCODE CKermit::HandleSpecialNames()
{
	POSITION pos = pIOReq->list.GetHeadPosition();
	int cnt = 0;
	while (pos) {
		CFiler::IOBuffer * buff = pIOReq->list.GetNext(pos);
		CString sName = buff->entry->Name();
		NameMap * map = new NameMap;
		map->sOrigName = sName;
		if (hasEightbitChar(sName, map->sRenamed)) {
			bSpecialNames = true;
			CString sExt;
			sExt.Format(".tmp%d", cnt++);
			map->sRenamed += sExt;
		}

		MapList.AddHead(map);
	}
	
	CString sCommand;

	if (bSpecialNames) {
		// There were files with an invalid filename. Now we process them.

		// The problem is, that if you try to send a Kermit-packet containing
		// such an invalid name, the protocol cannot handle it.
		// In this case, we have to tell the calc the name in a secure way, so that
		// the calc can temporary rename the file, so that it can be sent.

		// We just send the calc a list of all filenames to be renamed. The name itself is
		// sent as a list of ints, representing the single char values.

		CFiler::IORequest ioreq;
		ioreq.biggestObject = pIOReq->biggestObject;
		ioreq.filer = pIOReq->filer;
		

		sCommand = "%%HP: T(3)A(D)F(.);\\<< \\-> C \\<< IFERR 'ren.list' RCL THEN {";
		CString sName;
		pos = MapList.GetHeadPosition();
		while (pos) {
			NameMap * map = MapList.GetNext(pos);
			sName = map->sOrigName;
			sCommand += "{";
			int chr;
			for (int x = 0; x < sName.GetLength(); x++) {
				chr = sName[x]; 
				if (chr < 0) {
					// This is a special char.
					chr += 256; // The calc wants unsigned ints.
				}
				CString sNumber;
				sNumber.Format("%d ", chr);
				sCommand += sNumber;
			}

			sCommand += "} " + map->sRenamed;
		}
		CString sHelperName = "rmHelper";
		sCommand += "} \\-> L \\<< 1 L SIZE FOR X \"\" L X 1 + GET \\-> FROM TO \\<<L X GET DUP SIZE 1 FOR P DUP P GET CHR FROM + 'FROM' STO -1 STEP DROP TO \"'\" FROM \"'\" + + OBJ\\-> \\>> 2 STEP L SIZE \\->LIST DUP 'ren.list' STO \\>> ELSE DROP END ";
		sCommand += "ren.list SIZE 1 FOR F ren.list F GET ren.list F 1 - GET IF C \"RENAME\" == THEN SWAP END DUP RCL SWAP IFERR PURGE THEN PGDIR END SWAP STO -2 STEP IF C \"RENAME\" == NOT THEN 'ren.list' PURGE '";
		sCommand += sHelperName + "' PURGE ELSE DROP2 END \\>> \\>>";
		SendHelperProg(sHelperName, (LPTSTR)(LPCTSTR)sCommand, sCommand.GetLength());

		sCommand = "\"RENAME\" rmHelper";
		nSeq = 0;
		nBlockChkType = 1;
		SendPacket('C', sCommand.GetLength(), (LPTSTR)(LPCTSTR)sCommand);
		RecvStack();
	}
	
	return SUCCESS;
}

CProt::ERRCODE CKermit::SendHelperProg(CString &sName, char *buffer, int len)
{
	BOOL bStart = TRUE;
	char type;
	int x, nFileNo = 0;
	bool bDone = false;
	buf = cursor = buffer;
	nBufLen = len;
	nCount = 0;
	SendInit();
	state = sendfile;
	ERRCODE code;
	while (true) {
		code = PacketType(type);
		if (type == 'Y' && state == sendfile) {
			if (bDone) {
				SendEOT();
				code = PacketType(type);

				if ((code == SUCCESS) && (type == 'Y')) {
					nSeq = 0;
					return SUCCESS;
				}
				else {
					return code;
				}
			}
			if (bStart) {
				SetParam(pRecvData);
				bStart = FALSE;
			}
			nFirst = 1;
			nMaxDataLen = nMaxSendLen - (nReqBlockChkType + 3);
			x = EncodeString((LPTSTR)(LPCTSTR)sName);
			nBlockChkType = nReqBlockChkType;
			NextPacket();
			SendPacket('F', x, Data);
			nCount = 0; //nSeqAll = 0;
			state = senddata;
			nFirst = 1;
		}
		else if (type == 'Y' && state == senddata) {
			if (!SendData()) {
				SendEOF();
				bDone = true;
				state = sendfile;
			}
		}
	}

	return SUCCESS;
}

CString& CKermit::GetMappedName(CString &sName)
{
	POSITION pos = MapList.GetHeadPosition();
	NameMap * map;
	while (pos) {
		// Find the entry with the corresponding name.
		map = MapList.GetNext(pos);
		if (map->sOrigName == sName)
			break;
	}

	return map->sRenamed;
}

void CKermit::DeleteMapList()
{
	POSITION pos = MapList.GetHeadPosition();
	while (pos) {
		NameMap * map = MapList.GetNext(pos);
		delete map;
	}
	MapList.RemoveAll();
}

