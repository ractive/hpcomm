//////////////////////////////////////////////////////////////////////
/////////////////////              HPComm            /////////////////
//////////////////////////////////////////////////////////////////////




// Kermit.h: interface for the CKermit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KERMIT_H__E0221B82_07D0_11D3_AE2A_0000E8627120__INCLUDED_)
#define AFX_KERMIT_H__E0221B82_07D0_11D3_AE2A_0000E8627120__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Prot.h"
#include "HPComm38.h"
#include "TransferInfo.h"

// Threadfunctiondeclaration.
//UINT transferthread(LPVOID lParam);

class CKermit : public CProt
{
public:
	void Reset(bool bCloseInfoDlg = true);
	void ClientRequest(char cFirstChar, char cSecondChar, CString &sCurPath);
	void Notify(CString &sFileName, bool bRecv);

	void (*ProtCallback)(CString &sFileName, bool bRecv);

	void Transfer(bool bRecv = false);
	void Finish();


	CKermit();
	virtual ~CKermit();
	void RecvFiles();
	//void Delete(CFilerEntryList *pVictims);
	//void DeleteDir(CFilerEntry * pDir);

//protected:

	char* pRecvData;

	int nSeq;
	int nSpad;
	char cSpad;

	char sMark;
	int nBlockChkType;
	// FIXME: Is this needed?
	// int nSendPackLength;
	int nRecvPackLength;
	int nRecvSeq;
	char rMark;
	CString sStack;
	int nParity;
	int nMaxSendLen;
	char cRctlq;
	int nRecvQuot;
	// int nQuotFlag;
	BOOL bEbqFlag;
	int nEbq;
	int nSentQuot;
	int nReqBlockChkType;
	int nRepeatQ;
	int nRepeatFlag;
	int nRepeat;

	int UnChar(char c);
	char ToChar(int x);
	void SetParam(char * s);
	bool SendPacket(char cType, int len, char *d);
	int Resend();
	bool RecvStack();
	int RecvPacket(bool bFirstPacket = false);
	void NextPacket();
	int Nak();
	void DecodeString();
	char Ctl(char c);
	int Chkl(char * packet);
	LONG CHK3(char * s);
	int CheckSum(char * p);
	int Ack();
	void SendFile();
	BOOL SendData();
	int EncodeString(char * s);
	void SendEOT();
	void SendEOF();
	BOOL SendInit();
	char * ReqPar();
	int GetPacket(int nMaxLen);
	int Ackl(char * s);
	void Decode();
	void Encode(char a, char next);
	void SendFiles();


    enum _state {
		start, sendfile, senddata, done, recvfile, recvdata
	} state;

	int nMaxDataLen;

	int nSize;
	int nOSize;

	char Data[110];
	unsigned int nBufLen;
	//char * buf;
	//char * cursor;
	bool bCancel;
	bool bSkipFile;
	int nErrors;
	BOOL bCRC;
	HWND hTransWnd;
	HANDLE hCancel;
	HANDLE hDiscardFile;
	// True if we're SURE a calc is on the other end.
	bool bConnected;

	// Children should call this when they are sure they have a connection.

	unsigned long nCount;
	int nSeqAll;
	int MAXRP;
	int MAXSP;
	char cREOL;
	char cSEOL;

	char cSctlq;
	char cRpad;
	int nRpad;
	int nFirst;
	int nReqTimeOut;
	int nMaxRecvLen;

	char SendPack[1031];
	char RecvPack[1031];
	CTransferInfo * pInfoDlg;

private:
	static UINT TransferThreadProc(LPVOID lParam);
	char cFirstChar;
    char cSecondChar;
	int nChk;
	//CString sCurPCDir;
	CFile recvFile;
	CFile sendFile;
	bool bSpecialNames;
protected:
	bool bClientRequest;
	int ReadLineFromCom();
	bool bNotify;
	void PacketType(char& type, bool bFirstPacket = false);
};

struct threadinfo {
	CProtException * pEx;
	bool write;
	CKermit * pKermit;
}; // I always forget this ";"


#endif // !defined(AFX_KERMIT_H__E0221B82_07D0_11D3_AE2A_0000E8627120__INCLUDED_)
