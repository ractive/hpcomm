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

class CKermit : public CProt  
{
public:
	struct NameMap {
		CString sOrigName;
		CString sRenamed;
	};

	typedef CTypedPtrList<CPtrList, NameMap*> NameMapList;

	bool cp(bool removeAfterCopy, CFiler * fromFiler, CFilerEntryList *fromList, CFilerEntry *to);
	CProt::ERRCODE readRemoteDir(CString &sRemoteDir);
	void Archive(CString sArchive);
	void Restore(const CString &sName);

	CKermit(CSettings * settings);
	virtual ~CKermit();
	// Try and connect, or reconnect.  Does nothing if already connected.
	// Call isConnected to find out if it worked.  0 indicates current port.
	virtual bool Connect(int nComPort=0);
	ERRCODE RecvFiles();
	void Delete(CFilerEntryList *pVictims);
	void DeleteDir(CFilerEntry * pDir);

protected:
	bool SendPathRequest();
	bool CheckMode();
	CProt::ERRCODE SendFile(CFilerEntry * pEntry);
	BOOL SendData();
	int EncodeString(char * s);
	void SendEOT();
	void SendEOF();
	BOOL SendInit();
	char * ReqPar();
	int GetPacket(int nMaxLen);
	int Ackl(char * s);
	BOOL Decode();
	void Encode(char a, char next);
	ERRCODE SendFiles();

    virtual bool _SetBinary(bool bBinary);

    enum _state {
		start, sendfile, senddata, done, recvfile, recvdata
	} state;

	int nMaxDataLen;

	int nSize;
	int nOSize;

	char Data[110];

private:
	bool bSpecialNames;
	void DeleteMapList();
	CString& GetMappedName(CString& sName);
	NameMapList MapList;
	ERRCODE SendHelperProg(CString& sName, char * buffer, int len);
	CProt::ERRCODE HandleSpecialNames();
	bool hasEightbitChar(CString& sName, CString& sNewName);
};

#endif // !defined(AFX_KERMIT_H__E0221B82_07D0_11D3_AE2A_0000E8627120__INCLUDED_)
