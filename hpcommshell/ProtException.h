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


// ProtException.h: interface for the CProtException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROTEXCEPTION_H__429F0FA2_6C66_11D3_82D5_0000B4A265C3__INCLUDED_)
#define AFX_PROTEXCEPTION_H__429F0FA2_6C66_11D3_82D5_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_MSG 100

class CProtException
{
public:
	typedef enum {SUCCESS, OPENPORT, RETRIES, DEVICE, TIMEOUT,
				  CANCELLED, SKIPPED, FILEERROR, DIRERROR, ERRORPACKET, UNKNOWN,
				  LOWBATTERY, SYNTAX, MEMORY, VDINIT} ERRCODE;

	CProtException(ERRCODE errcode, DWORD dwSysErrcode = 0, TCHAR * sMsg = NULL);
	virtual ~CProtException();
	TCHAR sMessage[MAX_MSG];
	ERRCODE error;
	DWORD dwError;
};

#endif // !defined(AFX_PROTEXCEPTION_H__429F0FA2_6C66_11D3_82D5_0000B4A265C3__INCLUDED_)
