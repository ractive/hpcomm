// Settings.h: interface for the CSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTINGS_H__83AA8B81_33F5_11D3_81C7_0000B4A265C3__INCLUDED_)
#define AFX_SETTINGS_H__83AA8B81_33F5_11D3_81C7_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSettings  
{
    friend class CProt;
    friend class CKermit;
public:
	CSettings();
	virtual ~CSettings();

	DWORD dwBaud;		// Baudrate
	int nComPort;		// Com-port
	int nByte;			// Data bytes
	int nParity;		// Parity setting
	int nStop;			// Stop bits
//	int nTimeOut;		// Timeout in secs
	int nTransIO;		// Translation mode
	int nChk;			// Checksum type
	int nCalcType;		// Type of the calculator
private:
	bool bBinary;       // True for binary, false for ASCII.
};

#endif // !defined(AFX_SETTINGS_H__83AA8B81_33F5_11D3_81C7_0000B4A265C3__INCLUDED_)
