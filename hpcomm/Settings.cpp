// Settings.cpp: implementation of the CSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Settings.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSettings::CSettings()
{
	dwBaud = 9600;
	nComPort = 0;

	nByte = 8;
	nParity = NOPARITY;
	nStop = ONESTOPBIT;

	nTransIO = 1;		// Translation mode
	bBinary = TRUE;       // True for binary, false for ASCII.
	nChk = 1;			// Checksum type
	nCalcType = 1;		// Type of the calculator
}

CSettings::~CSettings()
{

}
