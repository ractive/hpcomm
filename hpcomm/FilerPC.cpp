// FilerPC.cpp: implementation of the CFilerPC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FilerEntry.h"
#include "FilerPC.h"
#include "FilerDir.h"
#include "Overwrite.h"

const CString CFilerPC::myComputer("My Computer");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilerPC::
CFilerPC()
{
}

CFilerPC::
~CFilerPC()
{
}


bool CFilerPC::
isValidDropTarget(CFilerEntry *pEntry)
{
	// The root must not be a drop target.
	return isRoot(pEntry) == false;
}


bool CFilerPC::
canBeDragged(CFilerEntry *pEntry)
{
	// Don't allow that the root entry and the drives can be dragged.
	return pEntry != 0 && !isRoot(pEntry) && !isRoot(pEntry->Parent());
}


bool CFilerPC::
isAvailable()
{
    return true;
}


bool CFilerPC::
isLocal()
{
    return true;
}


bool CFilerPC::
reconnect()
{
	return true;
}


bool CFilerPC::
transfer(CFiler::IORequest *IOReq)
{
	bool res = true;

	VERIFY(IOReq!=0);
	if (IOReq->list.GetCount()==0)
		return 0;

	POSITION pos = IOReq->list.GetHeadPosition();
	while (pos)
	{
		IOBuffer *buff = IOReq->list.GetNext(pos);
		buff->success = false;
		CString fName = toString(buff->entry);
		CFile file;

		if (IOReq->write == true && (buff->buffer != 0))
		{
			UINT flags = CFile::modeWrite;
			// FIXME: Put code here to handle overwriting files.
			if (true) {
				// modeCreate will truncate an existing file to length 0
				flags |= CFile::modeCreate;
			}
			if (file.Open(fName, flags) == false) {
				buff->success = false;
				res = false;
				continue;
			}
			
			// FIXME: Exception handling
			file.Write(buff->buffer, buff->entry->Size());
			
			// Close the file.
			file.Close();

			buff->success = true;
		}
		else
		{
			buff->entry->Size(0);
			DWORD filesize;
			CFileFind finder;
			if (finder.FindFile(fName)) {
				finder.FindNextFile();
				filesize = finder.GetLength();
			}
			if (file.Open(fName, CFile::modeRead) == false) {
				buff->success = false;
				res = false;
				continue;
			}

			// FIXME: What happens if the read fails?
			buff->entry->Size(filesize);
			// Allocate a new buffer
			buff->buffer = new char[filesize];
			buff->success = (filesize == file.Read(buff->buffer, filesize));
			file.Close();
		}
	}

    return res;
}


bool CFilerPC::
_rm(CFilerEntryList *pVictims)
{
	// Do nothing if the list is empty
	if (pVictims->IsEmpty())
		return true;
	
	// FIXME: Replace this para with something that allocates only as much
	// memory as needed.

	// Allocate a string for all the names of the files that will
	// be deleted. It's a double zero terminated string.
	char *pFrom = new char[pVictims->GetCount() * (_MAX_PATH + 1) + 1];
	// Set it to 0
	memset(pFrom, '\0', pVictims->GetCount() * (_MAX_PATH + 1) + 1);
	char *cursor = pFrom;

	POSITION pos;
	pos = pVictims->GetHeadPosition();
	// Iterate through the list with the CFileEntries that will be deleted.
	while (pos != 0)
    {
		CFilerEntry * fe = pVictims->GetNext(pos);

		// We don't support deleting the root.
		if (fe == 0)
			return false;

		// Get the filename of the CFileEntry
		CString sPath = toString(fe);
		char elem[_MAX_PATH];
		strcpy(elem, sPath);
		// Copy the filename to pFrom
		strcpy(cursor, elem);
		cursor += strlen(elem) + 1;
	}

	// Set up the struct to tell Windows to delete the files.
	SHFILEOPSTRUCT shFile;
	shFile.hwnd = 0;
	shFile.wFunc = FO_DELETE;
	shFile.pFrom = pFrom;
	shFile.pTo = NULL;
	shFile.fFlags = FOF_ALLOWUNDO;
	shFile.fAnyOperationsAborted = FALSE;

	bool bRet = true;

	// Now we delete the files.
    bRet = SHFileOperation(&shFile) == 0;

	delete pFrom;

    // BUG: Windows will ask before deleting a .exe.  If the user
    // cancels the delete, fAnyOperationsAborted is NOT set to TRUE.
	if (shFile.fAnyOperationsAborted == TRUE)
		bRet = false;

	return bRet;
}


bool CFilerPC::
_rename(CFilerEntry *pEntry, const CString &sNewName)
{
	CString sOld, sNew;

	// The the current name.
	sOld = toString(pEntry);
	
	// Replace the last part of the path with the new name.
	sNew = sOld.Left(sOld.ReverseFind(separator()) + 1) + sNewName;

	SHFILEOPSTRUCT shFile;
	shFile.hwnd = NULL;
	shFile.wFunc = FO_RENAME;
	
	char *pFrom, *pTo;

	// Make a double zero terminated string out of CString
	pFrom = new char[_MAX_PATH + 2];
	memset(pFrom, '\0', _MAX_PATH + 2);
	pTo = new char[_MAX_PATH + 2];
	memset(pTo, '\0', _MAX_PATH + 2);

	// pFrom == the current file-name
	strcpy(pFrom, sOld);
	// pTo == the new file name.
	strcpy(pTo, sNew);
		
	shFile.pFrom = pFrom;
	shFile.pTo = pTo;

	shFile.fFlags = FOF_ALLOWUNDO;
	shFile.fAnyOperationsAborted = FALSE;

	BOOL bRet;
	// Do the renaming.
	bRet = SHFileOperation(&shFile);

	delete pFrom;
	delete pTo;

	if (bRet != 0)
		return false;
	if (shFile.fAnyOperationsAborted == TRUE)
		return false;
	
	return true;
}


bool CFilerPC::
_cp(bool removeAfterCopy, CFiler *fromFiler, 
    CFilerEntryList *fromList, CFilerEntry *to)
{
     if (fromFiler != this)
		 return false;

#if 1
    TRACE("CFilerPC::_cp: to=%s, removeAfterCopy=%d\n", toString(to), removeAfterCopy);
	POSITION p;
	p = fromList->GetHeadPosition();
	// Iterate through the list with the CFileEntries.
	while (p != 0)
    {
        CFilerEntry * fe = fromList->GetNext(p);
        TRACE("CFilerPC::_cp: fe=%s\n", toString(fe));
	}
#endif
	// First we create the double-zero terminated file-list from fromList.

	// FIXME: Replace this para with something that allocates only as much
	// memory as needed.

	CString sTo = toString(to);

	// Allocate a string for all the names of the files.
	char *pFrom = new char[fromList->GetCount() * (_MAX_PATH + 1) + 1];
	// Set it to 0
	memset(pFrom, '\0', fromList->GetCount() * (_MAX_PATH + 1) + 1);
	char *cursor = pFrom;

	POSITION pos;
	pos = fromList->GetHeadPosition();
	// Iterate through the list with the CFilerEntries.
	while (pos != 0)
    {
		CFilerEntry *fe = fromList->GetNext(pos);

		// Get the filename of the CFileEntry
		CString sPath = toString(fe);
		char elem[_MAX_PATH];
		strcpy(elem, sPath);
		// Copy the filename to pFrom
		strcpy(cursor, elem);
		cursor += strlen(elem) + 1;
	}

	// Now we set up the struct to tell Windows what operation on what files to perform.
	SHFILEOPSTRUCT shFile;
	shFile.hwnd = 0;
    shFile.wFunc = removeAfterCopy ? FO_MOVE : FO_COPY;
	shFile.pFrom = pFrom;
    shFile.pTo = NULL;
    shFile.pTo = sTo.GetBuffer(sTo.GetLength());
	shFile.fFlags = FOF_ALLOWUNDO;
	shFile.fAnyOperationsAborted = FALSE;

	// Note that if the user aborted the copy, some of the files
	// might still exist.  If that's the case, we return false to indicate
	// to the client that it may want to consider refreshing.
	
#if 0
    // BUG: Windows will ask before deleting a .exe.  If the user cancels
    // the delete, fAnyOperationsAborted is NOT set to TRUE.
#endif

	bool bRet = true;

	// Now we copy or move the files.
    if (SHFileOperation(&shFile) != 0)
		bRet = false;
	if (shFile.fAnyOperationsAborted == TRUE)
		bRet = false;

	delete pFrom;

	return bRet;
}


CString CFilerPC::
suggestFolderName(CFilerEntry *parent)
{
	// A new directory is called "New folder" (or an incremented variant)
	// on the PC.
	
	// First we have to check if such a dir already exists.

	CString name("New folder");
	CFilerEntry *fe;
	int i = 1;

	// Does the name exist?  If so, just keep incrementing it.
	while ((fe = CFilerDir::CAST(parent)->findEntry(name)) != 0)
		name.Format("New Folder (%d)", i++);

	return name;
}


CFilerEntry *CFilerPC::
newFolder(CFilerEntry *pParent, CString sName)
{
	CFilerDir *parentDir = CFilerDir::CAST(pParent);
	// Does it already exist?
	CFilerEntry *fe = parentDir->findEntry(sName);
	if (fe != 0)
	{
		// If it exists and is a directory, return that dir.
		if (fe->Directory() == true)
			return fe;

		// If it exists and is a file, it's an error.
		return 0;
	}

	fe = newCFilerEntry(sName, true, CFilerEntry::NA);
	parentDir->add(fe);

	sName = toString(fe, true);
	if (CreateDirectory(sName, 0) == 0)
	{
		deleteCFilerEntry(fe);
		return 0;
	}

	// We know that the new dir is empty, so we add a 0 entry to
	// "initialise" the directory.
	CFilerDir *newdir = CFilerDir::CAST(fe);
	newdir->add(0);
	
	return fe;
}


bool CFilerPC::
hasDateTime()
{
    return true;
}


bool CFilerPC::
getDateTime(CFilerEntry *pEntry, SYSTEMTIME &st)
{
    if (pEntry == 0)
        // What's the date/time of "My Computer"??
        return false;

	// Don't show dates for drives.
	if (isRoot(pEntry->Parent()))
		return false;

    CFileFind finder;
    if (finder.FindFile(toString(pEntry))) {
		// If it is found, get the creation time.
		// Before we can do that, we must call FindNextFile.
        finder.FindNextFile();
		FILETIME ftutc, ft;
        bool bRet = (finder.GetLastWriteTime(&ftutc) == TRUE);
		FileTimeToLocalFileTime(&ftutc, &ft);
		FileTimeToSystemTime(&ft, &st);
		return bRet;
    }

    return false;
}


bool CFilerPC::
hasCRC()
{
    // Not supported at the moment.
    return false;
}


bool CFilerPC::
getCRC(CFilerEntry *pEntry, unsigned int &crc)
{
    // Not supported at the moment.
    return false;
}


bool CFilerPC::
hasFlash()
{
	return true;
}


bool CFilerPC::
updateFlash(const CString &sFlashFilename)
{
    HMODULE downloaddll = LoadLibrary("FDownload.dll");

    if (downloaddll == 0)
    {
        AfxMessageBox("Could not load flash downloading DLL.", MB_ICONERROR);

        return false;
    }

    typedef bool (__cdecl *DownloadFuncType)(char *);
    DownloadFuncType download;
    download = (DownloadFuncType)GetProcAddress(downloaddll, "Download");

    if (download == 0)
    {
        AfxMessageBox("Could not find download DLL function.", MB_ICONERROR);
        FreeLibrary(downloaddll);

        return false;
    }

    char fname[_MAX_PATH];
    strcpy(fname, sFlashFilename);
    bool res = download(fname);
    if (res == false)
    {
        AfxMessageBox("Could not start flash download program.", MB_ICONERROR);

        FreeLibrary(downloaddll);

        return false;
    }

    FreeLibrary(downloaddll);

    return true;
}


// Calling this method ensures that all the PC dirs from here up exist.
// If they don't, it creates them.
bool CFilerPC::
readDir(CFilerDir *dir, bool reRead /* = false */)
{
	// Are we passed a directory?
	VERIFY(dir->Directory());

#if 0 && defined(_DEBUG)
	TRACE("CFilerPC::readDir: dir=%s\n", toString(dir));
#endif

	// Have we already read this directory?
	if (!reRead && hasBeenRead(dir))
		return true;

	if (reRead)
	{
		// We re-read the directory contents, but don't delete the
		// CFilerEntries.  We just update existing ones and add new ones.
	}

    // Is it the the root case?
    if (isRoot(dir))
        return readRoot(dir);

    // No, it's not the root.

    // First we ensure that the parent is read in.  This is recursive.
    bool res = readDir(CFilerDir::CAST(dir->Parent()));
	if (res == false)
		return false;

    // Now we read this directory in.
    CString sDir(toString(dir));
    sDir += separator();
	sDir += "*.*";

    CFileFind ff;
    // Grr, FindFile returns BOOL instead of bool....
    BOOL doFileFind = ff.FindFile(sDir);
    if (doFileFind == FALSE)
    {
        DWORD error = GetLastError();
        // If we're reading from a device like a floppy, and
        // there's no floppy, we get an error.  Ignore it.
        if (error == ERROR_NOT_READY)
            TRACE("CFilerPC::readDir: Device '%s\\' not ready.  Ignoring.\n",
                  toString(dir));
        else
		{
			res = false;
			TRACE("CFilerPC::readDir: error=%d\n");
		}
    }

	CStringArray sa;

	bool dirIsBlank = true;
    // This code iterates over the directory.
    while(doFileFind == TRUE)
    {
        // Get info on the next file.
        doFileFind = ff.FindNextFile();

        if (ff.IsDots())
            continue;

        // Extract the info for the CFilerEntry.
        CString name(ff.GetFileName());
		sa.Add(name);
        bool isDir = true;
        unsigned int size = CFilerEntry::NA;

        if (ff.IsDirectory() == FALSE)
        {
            isDir = false;
            size = ff.GetLength();
        }

		CFilerEntry * fe;
		if (reRead && ((fe = dir->findEntry(name)) != 0))
		{
			// The dir already contains this entry, so we update it.
			fe->Name(name);
			fe->Directory(isDir);
			fe->Size(size);
			calcIcons(fe);
		}
		else {
			// Create the new CFilerEntry.
			fe = newCFilerEntry(name, isDir, size);
	        // Add it to the dir.
	        dir->add(fe);
		}

		dirIsBlank = false;
    }

	if (reRead)
		cleanUp(&sa, dir);

	// If there were no entries, we still need to force the
	// list to exist.
	if (dirIsBlank == true)
		dir->add(0);

    return true;
}


bool CFilerPC::
readRoot(CFilerDir *root)
{
	VERIFY(root!=0);

    char drives[500], *ptr;
	// TODO: Check the result of this call.  Return false if it fails.
    GetLogicalDriveStrings(500, drives);
    ptr = drives;
    while (*ptr) {
        CString s(ptr);
        // Remove the trailing backslash.
        if (s[2] == separator())
            s.Delete(2, 1);
        s.MakeUpper();
        // Create a CFilerEntry for it.
        CFilerEntry *pEntry = newCFilerEntry(s, true, CFilerEntry::NA);

        // Add it to the root collection.
        root->add(pEntry);

		// If it's the system directory, make it the current dir.
		if (s == "C:" && currentDir() == 0)
			currentDir(pEntry);

        ptr += strlen(ptr) + 1;
    }

	return true;
}


void CFilerPC::
calcIcons(CFilerEntry *entry)
{
	// This next line avoids calculating the icon if it's been done
	// before.  If the icon has changed, then invalidate() should be
	// called.
	if (entry->IconIndex(false) != -1 && entry->IconIndex(true) != -1)
		return;

    if (isRoot(entry))
    {
		// These are the icons for "My Computer".
	    SHFILEINFO shinfo;
		ITEMIDLIST *idl;
		SHGetSpecialFolderLocation((HWND)AfxGetMainWnd(), CSIDL_DRIVES, &idl);
		SHGetFileInfo((char *)idl, 0, &shinfo, sizeof(shinfo), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_PIDL);
		entry->IconIndex(false, shinfo.iIcon);
		SHGetFileInfo((char *)idl, 0, &shinfo, sizeof(shinfo), SHGFI_SYSICONINDEX | SHGFI_PIDL);
		entry->IconIndex(true,  shinfo.iIcon);

		return;
    }


	CString entryName(toString(entry));
	// Are they the drives?  If so, Windows wants a trailing slash.
	if (entry->Directory() && isRoot(entry->Parent()))
	{
		entryName += separator();
	}

    SHFILEINFO shinfo;

    // We get the index of the item from the ImageList.

    // WARNING: These calls are hideously expensive in terms of time!!!
    SHGetFileInfo((LPCTSTR)entryName, 0, &shinfo, sizeof(shinfo), 
                  SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

    entry->IconIndex(false, shinfo.iIcon);

    SHGetFileInfo((LPCTSTR)entryName, 0, &shinfo, sizeof(shinfo),
                  SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_OPENICON);

    entry->IconIndex(true, shinfo.iIcon);
}


int CFilerPC::getDirIcon()
{
	char sysName[_MAX_PATH];
	GetSystemDirectory(sysName, _MAX_PATH);
    SHFILEINFO shinfo;

    // WARNING: These calls are hideously expensive in terms of time!!!
    SHGetFileInfo(sysName, 0, &shinfo, sizeof(shinfo), 
                  SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

	return shinfo.iIcon;
}


// This method is called by cp when doing cross-filespace copies, so
// that ridiculous transfers aren't attempted.  On a PC, this returns
// 0, which means no limit.
unsigned int CFilerPC::
biggestObject(CFilerEntry *where)
{
    return 128*1024;
}


const CString &CFilerPC::
rootName()
{
	return myComputer;
}


char CFilerPC::
separator()
{
	return '\\';
}


bool CFilerPC::
validFilename(const CString &name)
{
    if (name.GetLength() > 127 || name.GetLength() == 0)
        return false;

	// See http://msdn.microsoft.com/LIBRARY/DDKDOC/WIN95DDK/STORAGE_60Q8.HTM

    if (name.FindOneOf("/\\^\"*?|<>") != -1)
        return false;

	return true;
}


CFilerEntry *CFilerPC::
createArchive(CFiler *toFiler, CFilerEntry *toParent, CString fname)
{
	return 0;
}


bool CFilerPC::
restoreFromArchive(CFiler *fromFiler, CFilerEntry *fromfe)
{
	return false;
}


bool CFilerPC::
overwrite(CFilerEntry *parent, const CString &name)
{
#if 0
	static bool bAll = false;
	static bool prev;
	if (bAll) {
		return prev;
	}
	COverwrite dlg;
	dlg.sDirname = parent->Name();
	dlg.sFilename = name;
	int res = dlg.DoModal();

	if (res == IDYES) {
		bAll = false;
		return true;
	}

	if (res == IDNO) {
		bAll = false;
		return false;
	}
	
	if (res == IDYESALL) {
		bAll = true;
		prev = true;
		return true;
	}

	if (res == IDNOALL) {
		bAll = true;
		prev = false;
		return false;
	}
#else
	COverwrite dlg;
	dlg.sDirname = parent->Name();
	dlg.sFilename = name;
	int res = dlg.DoModal();
#endif
	if (res == IDYES)
		return true;

	if (res == IDNO)
		return false;

	// Cause a fault if anything else came back.
	VERIFY(res==IDYES);

	return true;
}
