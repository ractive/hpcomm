// Prot.cpp: implementation of the CProt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Prot.h"
#include "VD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProt::CProt()
{
	pos = NULL;

	bUseBuffer = false;

	bNewFileCreated = false;

	bTransactionFinished = false;

	bSendingOnly = false;

	bVDFilesSent = false;

	hRecvFile = INVALID_HANDLE_VALUE;

	hSendFile = INVALID_HANDLE_VALUE;

	bRecv = true;
}

CProt::~CProt()
{

}

unsigned int CProt::GetFileSize()
{
	HANDLE hFile;
	if (bRecv) {
		hFile = hRecvFile;
	}
	else {
		hFile = hSendFile;
	}
	CFile file((int)hFile);
	return file.GetLength();
}

bool CProt::GetFileName(CString &sFileName)
{
	sFileName = sReqFileName;
	return true;
}

bool CProt::GetCharFromFile(char &ch)
{
	DWORD dwRead;
	ReadFile(hSendFile, &ch, 1, &dwRead, NULL);
	return dwRead == 1;
}

bool CProt::PutCharToFile(char ch)
{
	DWORD dwWritten;
	WriteFile(hRecvFile, &ch, 1, &dwWritten, NULL);
	return dwWritten == 1;
}

bool CProt::CreateFile()
{
	bRecv = true;

	CString sFilePath = sCurPCPath + "\\" + sReqFileName;

	CFileFind finder;
	bool bNewFileCreated = !finder.FindFile(sFilePath);
	finder.Close();

    CVD vd(sCurPCPath);
    if (vd.IsTheDirFile(sReqFileName)) {
		// The calc now writes a directory file to the PC which
		// means the transaction is finished.
		bTransactionFinished = true;
	}

	bWasReadonly = false;

	// We check if the file has the read-only attribute set, but only if it exists
	// already.
	DWORD dwFileAttributes = GetFileAttributes(sFilePath);
	if (!bNewFileCreated && (dwFileAttributes & FILE_ATTRIBUTE_READONLY)) {
		// Some ApLets you download from the internet have the read-only flag set to 
		// the VD files HP38DIR.CUR and HP38DIR.000
		// If this is true, we remove it temporaly to let the calc update this file.
		if (vd.IsTheDirFile(sReqFileName) == false) {
			// For all other files we raise an error message and ask the user if
			// he wants to overwrite the file anyway.
			CString sMsg = "The file " + sReqFileName + " is read-only, so it can't be overwritten.\n\nDo you want to overwrite it anyway?";
			if (MessageBox(NULL, sMsg, "Warning", MB_YESNO | MB_ICONINFORMATION) == IDNO) {
				return false;
			}
		}

		// Try to remove the read-only attribute.
		if (!SetFileAttributes(sFilePath, (dwFileAttributes & ~FILE_ATTRIBUTE_READONLY))) {
			MessageBox(NULL, "The read-only attribute could not removed!", "Error", MB_OK | MB_ICONSTOP);
			return false;
		}
		
		bWasReadonly = true;
	}

	hRecvFile = ::CreateFile(sCurPCPath + "\\" + sReqFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwError = GetLastError();
	return hRecvFile != INVALID_HANDLE_VALUE;
}

bool CProt::CloseFile()
{
	bool bRet = false;
	if (bRecv) {
		// If we received a file, we have to close the file we created.
		if (hRecvFile != INVALID_HANDLE_VALUE) {
			bRet = CloseHandle(hRecvFile) == TRUE;
			hRecvFile = INVALID_HANDLE_VALUE;
            CVD vd(sCurPCPath);
			// If the file was read-only we set it back to that attribute.
			if (bWasReadonly || vd.IsTheDirFile(sReqFileName)) {
				CString sFilePath = sCurPCPath + "\\" + sReqFileName;
				// Get the other file-attributes and add the read-only flag.
				if (!SetFileAttributes(sFilePath, GetFileAttributes(sFilePath) | FILE_ATTRIBUTE_READONLY)) {
					MessageBox(NULL, "It was not possible to make " + sReqFileName + " read-only.", "Error", MB_OK | MB_ICONEXCLAMATION);
					return false;
				}
			}
			return bRet;
		}
		else
			return false;
	}
	else {
		// If we sent a file, se have to close the send file.
		if (hSendFile != INVALID_HANDLE_VALUE) {
			bRet = CloseHandle(hSendFile) == TRUE;
			hSendFile = INVALID_HANDLE_VALUE;
			return bRet;
		}
		else
			return false;
	}
}

bool CProt::OpenFile()
{
	bRecv = false;

    CVD vd(sCurPCPath);
	// If a file is opened which is not a VD-file we know that
	// it's only a send-only transaction.
	bSendingOnly = !vd.IsAVDFile(sReqFileName);
	bVDFilesSent = vd.IsTheDirFile(sReqFileName);

	hSendFile = ::CreateFile(sCurPCPath + "\\" + sReqFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);		
	return hSendFile != INVALID_HANDLE_VALUE;
}
