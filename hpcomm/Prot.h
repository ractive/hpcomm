//////////////////////////////////////////////////////////////////////
/////////////////////              HPComm            /////////////////
//////////////////////////////////////////////////////////////////////




// Prot.h: interface for the CProt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROT_H__E0221B81_07D0_11D3_AE2A_0000E8627120__INCLUDED_)
#define AFX_PROT_H__E0221B81_07D0_11D3_AE2A_0000E8627120__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HPComm.h"	// Added by ClassView

class CSettings;

// Here's a description of the ERRCODE errors.
//
//  SUCCESS		There were no errors
//  OPENPORT	The com-port could not be opened
//  RETRIES		There were too many retries
//  DEVICE		There is probaly not a HP-calc connected (i.e. Modem)
//  TIMEOUT		Makes this sense, together with RETRIES??????
//  CANCELLED	The transfer was cancelled
//  FILEERROR	A file could not be opened or closed
//  UNKNOWN		There was an error that could not be specified

class CProt
{
public:
	void Finish();
	// Error codes that are returned after a transfer.
	typedef enum {SUCCESS, OPENPORT, RETRIES, DEVICE, TIMEOUT,
				  CANCELLED, SKIPPED, FILEERROR, ERRORPACKET, UNKNOWN,
				  LOWBATTERY, SYNTAX, MEMORY} ERRCODE;

	void ThrowProtException(ERRCODE errcode);
	bool newFolder(CString sName);
	bool rename(CString from, CString to);
	virtual bool cp(bool removeAfterCopy, CFiler * fromFiler, CFilerEntryList *fromList, CFilerEntry *to) = 0;
	virtual ERRCODE readRemoteDir(CString &sRemoteDir) = 0;
	virtual void Restore(const CString &sName) = 0;
	virtual void Archive(CString sArchive) = 0;

	bool bArchive;

	CProt(CSettings * settings);
	virtual ~CProt();
	// Ptrs we use for the callbacks when establishing a connection.
	void (*pfConnectNewCb)(int nComPort=0);
	void (*pfConnectBumpCb)(int stage, bool bSuccess = true);
	void (*pfConnectDeleteCb)();

	bool ReadScreenData();
	void Notify(int nType, void* param);

	void ReAllocateBuffer(char *&buf, unsigned int nNewLen);
	char * AllocateBuffer(unsigned int nLen);
	virtual ERRCODE RecvFiles() =0;
	virtual ERRCODE SendFiles() =0;

	// Try and connect, or reconnect.  Does nothing if already connected.
	// Call isConnected to find out if it worked.  0 indicates current port.
	virtual bool Connect(int nComPort=0);
    // Are we currently connected?
    bool IsConnected();
	
	void SetHP(int nTransIO, int nCheckSum);
    bool SetBinary(bool bBinary);
    bool IsBinary();

	char * cursor;
    CSettings * pSettings;
	//clipInfo_ * pClipInfo;
	DWORD hThread;
	CFiler::IORequest * pIOReq;
	CProt::ERRCODE Transfer(CFiler::IORequest * io);
	void (*ProtCallback)(int nType, void* param);
	void CloseComPort(bool bDisConnect = true);
	HANDLE hComPort;
	bool SetTimeout(int nTime);

	char* pRecvData;

	bool SendPacket(char cType, int len, char * d);
	int RecvPacket();
	bool RecvStack();
	char ToChar(int x);
	int Chkl(char * packet);
	int CheckSum(char * p);
	LONG CHK3(char * s);
	int UnChar(char c);
	ERRCODE PacketType(char& type);
	int Ack();
	int Nak();
	void SetParam(char * s);
	void DecodeString();
	int Resend();
	void NextPacket();
	char Ctl(char c);

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
	bool OpenPort();

protected:
	bool bNotify;

	unsigned int nBufLen;
	BOOL GetCharFromBuffer(char &ch);
	char * buf;
	void PutCharToBuffer(char ch);
	bool bCancel;
	bool bSkipFile;
	int nErrors;
	BOOL bCRC;
	int GetLineFromCom();
	BOOL PutPacketToCom(char * s);
	HWND hTransWnd;
	HANDLE hCancel;
	HANDLE hDiscardFile;
	BOOL GetCharFromCom(char * ch);
	unsigned long GetBufferFromCom(char * s, unsigned long len);
	BOOL PutBufferToCom(char * s, unsigned long len);
	BOOL PutCharToCom(char ch);
	// True if we're SURE a calc is on the other end.
	bool bConnected;
	// Children should call this when they are sure they have a connection.
	void SetConnected();
    virtual bool _SetBinary(bool bBinary)=0;

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
private:
};

#endif // !defined(AFX_PROT_H__E0221B81_07D0_11D3_AE2A_0000E8627120__INCLUDED_)
