// CalcComm.cpp: implementation of the CCalcComm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CalcComm.h"
#include "Kermit.h"
#include "CalcTypes.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCalcComm::CCalcComm()
{
	m_pKermit = new CKermit;
	ZeroMemory(m_sRemoteDir, sizeof(m_sRemoteDir));
	m_pKermit->OpenPort(1);
}

CCalcComm::~CCalcComm()
{
	m_pKermit->ClosePort();
	delete m_pKermit;
}

BOOL CCalcComm::ReadRemoteDir()
{
	ZeroMemory(m_sRemoteDir, sizeof(m_sRemoteDir));
	try {
		m_pKermit->readRemoteDir(m_sRemoteDir);
	}
	catch (CProtException * pEx) {
		switch (pEx->error) {
		case CProtException::RETRIES:
			break;
		default:
			break;
		}
		delete pEx;
		return FALSE;
	}

	TCHAR * m_pCurPos = m_sRemoteDir;
	if (*m_pCurPos == '{') {
		m_pCurPos = strchr(m_pCurPos, '\n');
		m_pCurPos++;
	}

	if (m_pCurPos == NULL) {
		return FALSE;
	}

	/* Establish string and get the first token: */
	m_sEntryToken = strtok(m_pCurPos, "\n");

	return TRUE;
}

LPCALCITEM CCalcComm::FindNextFile()
{
	if (m_sEntryToken == NULL) {
		return NULL;
	}

	LPCALCITEM pItem = ParseLine(m_sEntryToken);
	/* Get next token: */
	m_sEntryToken = strtok(NULL, "\n");
	
	return pItem;
}

LPCALCITEM CCalcComm::ParseLine(TCHAR * line)
{
	// Unfortunately the type info can contain a whitespace
	// (e.g. "Real Number") which makes things messy.

	// Extract a line of the directory-string.
	TCHAR sName[50];
	UINT nSize = 0;
	// Get the name and the size
	sscanf(line, "%s %d", sName, &nSize);

	// Walk behind the size string
	line = strchr(line, ' ');
	line++;
	line = strchr(line, ' ');
	line++;

	TCHAR * sCRC = strrchr(line, ' ');
	UINT nCRC = 0;

	TCHAR sType[40] = "";
	TCHAR * p = sType;
	TCHAR * pOld = sType;

	sscanf(sCRC, "%d", &nCRC);
	while(IsCharAlpha(*line) || *line == ' ') {
		pOld = p;
		*p++ = *line++;
	}
	*pOld = '\0';

	LPCALCITEM pItem = new CALCITEM;
	pItem->cb = sizeof(CALCITEM);
	pItem->nSize = nSize;
	pItem->nCRC = nCRC;
	CCalcTypes * ct = CCalcTypes::instance();
	pItem->bIsDir = ct->isADirectory(sType);
	pItem->type = ct->icon(sType);
	lstrcpy(pItem->sType, sType);
	lstrcpy(pItem->sName, sName);
	return pItem;
}

BOOL CCalcComm::FindFirstFile()
{
	return ReadRemoteDir();
}

BOOL CCalcComm::ChangeDir(LPCTSTR sDir)
{
	m_pKermit->nSeq = 0;
	m_pKermit->SendPacket('C', strlen(sDir), (LPTSTR)sDir);

	return m_pKermit->RecvStack();

}

BOOL CCalcComm::GoHome()
{
	m_pKermit->nSeq = 0;
	m_pKermit->SendPacket('C', strlen("HOME"), "HOME");

	return m_pKermit->RecvStack();
}
