// Filer.cpp: implementation of the CFiler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hpcomm.h"
#include "Filer.h"
#include "NewFilename.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFiler::
CFiler()
{
    ff_ = 0;
    findPos_ = 0;
	finishedSearch_ = true;
    currentDir_ = 0;
	root_ = 0;
	lieAboutSubDirs_ = false;
}

CFiler::
~CFiler()
{
    // Delete all the CFilerEntrys we created.
    deleteEntryPool();
}


bool CFiler::
invalidate(CFilerEntry *startingAt)
{
	VERIFY(isValid(startingAt));

	// Do they want to invalidate the whole tree?
	if (isRoot(startingAt)) 
		deleteEntryPool();
	else
		// No, delete a part.  This can be done with deleteCFilerEntry.
		deleteCFilerEntry(startingAt);

	// If any of the CFiler attributes are affected, reset them.
	if (isValid(ff_) == false)
	{
		ff_ = 0;
		findPos_ = 0;
		finishedSearch_ = true;
	}

	// Did they toast the root?
	if (isValid(root_) == false)
	{
		// Yes.  Force a reread.
		root();
		currentDir_ = root_;
	}
	else if (isValid(currentDir_) == false)
		currentDir_ = startingAt->Parent();

    return true;
}


// Does this entry appear in our valid list?
bool CFiler::isValid(CFilerEntry *entry)
{
	if (entry == 0)
		return false;

	POSITION pos = entryPool_.Find(entry);
	
	return pos != 0;

}


bool CFiler::
sync()
{
    return true;
}


bool CFiler::
findFile(CFilerEntry *pParent)
{
	VERIFY(isValid(pParent));

	if (pParent->Directory() == false)
		return false;

	CFilerDir *dir = CFilerDir::CAST(pParent);

	bool res = readDir(dir);

    if (res == false)
        return false;

	ff_ = dir;
    findPos_ = 0;

	CFilerEntryList *list = ff_->entries_;

    if (list == 0 || list->GetCount() == 0)
        finishedSearch_ = true;
    else
        finishedSearch_ = false;

    return true;
}


CFilerEntry *CFiler::
findFileNext()
{
    // Assert if someone calls us without first calling findFile.
    VERIFY(ff_!=0);

    if (finishedSearch_ == true)
        return 0;

    // Note, we'll never get here if entries_ is empty, so calling
    // GetHeadPosition will always be valid.
	CFilerEntryList *list = ff_->entries_;
	VERIFY(list!=0);

    if (findPos_ == 0)
		findPos_ = list->GetHeadPosition();

    CFilerEntry *ret = list->GetNext(findPos_);
	VERIFY(ret!=0);

    if (findPos_ == 0)
        finishedSearch_ = true;

    return ret;
}


bool CFiler::
currentDir(CFilerEntry *cd)
{
	if (!isValid(cd))
		return false;
	
    currentDir_ = cd;

    return true;
}


CFilerEntry *CFiler::
currentDir()
{
	return currentDir_;
}


bool CFiler::
rm(CFilerEntry* pVictim)
{
	// Create a list of one item, then
	// pass it to the listed rm.
	CFilerEntryList victimList;

    victimList.AddTail(pVictim);

    return rm(&victimList);
}


bool CFiler::
rm(CFilerEntryList *pVictims)
{
	VERIFY(pVictims!=0);

    if (pVictims->GetCount() == 0)
        return true;

	bool res = _rm(pVictims);
	
    // Now fix up the CFilerEntries.
    CFilerEntryList rereadList;
    POSITION pos = pVictims->GetHeadPosition();
    while (pos)
    {
        CFilerEntry *fe = pVictims->GetNext(pos);
        fe = fe->Parent();
		// Is it already in the list?
		if (isValid(fe) && (rereadList.Find(fe) == 0))
		{
			// No, so add it.
			rereadList.AddTail(fe);
		}
    }
    
    // Take the reread list, and reread each directory.
    pos = rereadList.GetHeadPosition();
    while (pos)
    {
        CFilerEntry *fe = rereadList.GetNext(pos);
        
        reRead(fe);
    }
    
    return res;
}


bool CFiler::
rename(CFilerEntry *pEntry, const CString &sNewName)
{
	VERIFY(isValid(pEntry));

    if (validFilename(sNewName) == false)
    {
        CString s;
        s.Format("\"%s\" is not a legal name.", sNewName);
        AfxMessageBox(s, MB_ICONEXCLAMATION | MB_OK);

        return false;
    }

	bool res = _rename(pEntry, sNewName);

	if (res == true) {
		// If it was successful, rename the entry...
		pEntry->Name(sNewName);

		// ...and recalculate the icon.
		pEntry->IconIndex(true, -1);
		pEntry->IconIndex(false, -1);
		calcIcons(pEntry);
	}

	return res;
}


bool CFiler::
cp(bool removeAfterCopy, CFiler *fromFiler, CFilerEntry *from, CFilerEntry *to)
{
	// Create a list of one item, then
	// pass it to the listed rm.
	CFilerEntryList fromList;

    fromList.AddTail(from);

    return cp(removeAfterCopy, fromFiler, &fromList, to);
}


bool CFiler::
cp(bool removeAfterCopy, CFiler *fromFiler, 
   CFilerEntryList *fromList, CFilerEntry *to)
{
	POSITION pos;
	CWaitCursor wait;

    VERIFY(fromFiler!=0);
    VERIFY(fromList!=0);
	VERIFY(isValid(to));

    // An empty copy or move always succeeds.
    if (fromList->GetCount() == 0)
        return true;

    // A copy of more than one item to a file fails, regardless
    // of the source types.
    if (fromList->GetCount() > 1 && to->Directory() == false)
        return false;

    // Don't allow the copy of a directory to a file.
    CFilerEntry *first = fromList->GetHead();
    if (to->Directory() == false &&
        fromList->GetCount() == 1 &&
        first->Directory() == true)
        return false;

    // Hmm, should be alright now.
    CString fromFilerType(fromFiler->rootName());
    CString toFilerType(this->rootName());

    // Are they on the same filesystem?  If so, do it the quick way.
    if (fromFilerType == toFilerType)
    {
        bool res = _cp(removeAfterCopy, fromFiler, fromList, to);

		// Now fix up the CFilerEntries.  We'll always want to reread the
		// destination.
		CFilerEntryList rereadList;
		CFilerEntry *fe = to;
		if (fe->Directory() == false)
			fe = fe->Parent();
		rereadList.AddTail(fe);

		// If it was a move, reread the source directories.
		if (removeAfterCopy)
		{
			pos = fromList->GetHeadPosition();
			while (pos)
			{
				CFilerEntry *fe = fromList->GetNext(pos);
				if (fe->Directory() == false)
					fe = fe->Parent();
				// Is it already in the list?
				if (rereadList.Find(fe) == 0)
				{
					// No, so add it.
					rereadList.AddTail(fe);
				}
			}
		}

		// Take the reread list, and reread each directory.
		pos = rereadList.GetHeadPosition();
		while (pos)
		{
			CFilerEntry *fe = rereadList.GetNext(pos);
		
			reRead(fe);
		}

        return res;
    }

	// No, they are on different filesystems.  So we need to copy the files
	// using an IORequest of IOBuffers.
	
	// The first step is to determine whether the filenames are valid on this filer.
	// Don't do the check if we're copying to a file.
	int bad=0;
	int total=0;
	pos = fromList->GetHeadPosition();
	while (to->Directory() && pos) {
		total++;
		CFilerEntry *fromFE = fromList->GetNext(pos);
		CString fromName = fromFE->Name();
		if (validFilename(fromName) == false)
			bad++;
	}

	// If there's more than one bad entry, should we continue?
	if (total > 1 && bad > 0)
	{
		CString s;
		if (bad == total)
			s.Format("All %d filenames will need changing.  Continue?", total);
		else
			s.Format("%d out of %d filenames will need changing.  Continue?", bad, total);
		int res = AfxMessageBox(s, MB_OKCANCEL);
		if (res == IDCANCEL)
			// No, quit.
			return false;
	}

	// Build the IORequest.
    IORequest IOReq;
    IORequest deleteList;

	pos = fromList->GetHeadPosition();
	while (pos) {
		// FIXME: Is the size acceptable?

		// Fill a IOBuffer List with the files we want to copy.
		IOBuffer * buf = new IOBuffer;
		buf->success = false;
		buf->buffer = 0; // If this becomes non-zero, it needs to be deleted.
		buf->entry = fromList->GetNext(pos);
		IOReq.list.AddHead(buf);
	}

	IOReq.filer = fromFiler;
	IOReq.write = false;
	IOReq.biggestObject = biggestObject(root_);

	// Read the files from the source filer

	bool res = fromFiler->transfer(&IOReq);

    // Now write them to our filer.

	// See if all the requested files could be read in.
	// If transfer returned true, all files were transferred correctly.
	pos = IOReq.list.GetHeadPosition();
	while (res && pos)
    {
		// Keep this position in case we need to delete it.
		POSITION thispos = pos;
		IOBuffer *iobuff = IOReq.list.GetNext(pos);
        if (iobuff->success == false)
        {
            // Oops, the read didn't work for this one.  Drop it from the list.
			// Remember this spot
            deleteList.list.AddTail(iobuff);
			// Drop the one we remembered.
            IOReq.list.RemoveAt(thispos);
        }
	}

	if (to->Directory() == false)
	{
		IOBuffer *iobuff = IOReq.list.GetHead();
		to->Size(iobuff->entry->Size());
		iobuff->entry = to;
		iobuff->success = false;
	}
	else
	{
		// Ok, now generate CFilerEntry*s for the new items.
		pos = IOReq.list.GetHeadPosition();
		// This is set true if the user selects cancel.
		bool autoKill = false;
		while (pos)
		{
            editReturn res;
			POSITION thispos = pos;
			IOBuffer *iobuff = IOReq.list.GetNext(pos);
			// Yeah, we read it.  Now to see if the name is suitable for
			// the new filespace.
			if (autoKill == false)
			{
				CFilerEntry *fromfe = iobuff->entry;
				CString name = fromfe->Name();

                bool hadToChangeFilename = false;
                CString oldName = name;
				res = editFilename(name);
                if (oldName != name)
                    hadToChangeFilename = true;
				if (res == OK)
				{
					// get a CFilerEntry* for the destination.
					CFilerEntry *fe = findOrCreate(to, fromfe);
					// Did they decline to overwrite an existing one?
					if (fe == 0)
						// Yeah, so fake doing a skip.
						res = SKIP;
					else
						iobuff->entry = fe;

                    // Support renaming the file because of invalid chars.
                    if (hadToChangeFilename)
                        fe->Name(name);
				}
					
                // Clear the flag, for the writing process.
                iobuff->success = false;
			}

			if (autoKill == true || res == SKIP || res == CANCEL)
			{
				// Ok, the user wants to skip this filename.  Drop it from the list.
				deleteList.list.AddTail(iobuff);
				// Find the next iobuff.
				iobuff = 0;
				if (pos != 0)
					iobuff = IOReq.list.GetNext(pos);
				// Then drop the one we remembered.
				IOReq.list.RemoveAt(thispos);
			}

			if (res == CANCEL)
				autoKill = true;
		}
	}

	// Any files left?
    if (IOReq.list.GetCount() == 0)
        return false;

	// Write it to the receiving filer.
	IOReq.write = true;
	res = transfer(&IOReq);

	// Check if all files could be written.  Make a list of those that couldn't.
	CFilerEntryList list;
	if (!res) {
		pos = IOReq.list.GetHeadPosition();
		while (pos) {
			// Check the success flag
			IOBuffer *iobuff = IOReq.list.GetNext(pos);
			if (!iobuff->success) {
				list.AddHead(iobuff->entry);
			}
		}
	}

	// Delete the entries that could not be written.
	rmEntries(&list);

	deleteIOBuffers(&IOReq);
    deleteIOBuffers(&deleteList);

	// Re read the dir.
	CFilerDir *todir;
	if (to->Directory() == true)
		todir = CFilerDir::CAST(to);
	else
		todir = CFilerDir::CAST(to->Parent());
	readDir(todir, true);

    return res;
}


bool CFiler::
cp(bool removeAfterCopy, HDROP hDrop, CFiler *fromFiler, CFilerEntry *to)
{
	// Make new filer entries form the files string (double zero
	// terminated)
	UINT n = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
	char sFile[_MAX_PATH];
	CFilerEntryList fromList;
	for (UINT i = 0; i < n; i++) 
	{
		DragQueryFile(hDrop, i, sFile, _MAX_PATH);
		CFilerEntry * fe = fromFiler->fromString(sFile);
		fromList.AddTail(fe);
	}
	cp(false, fromFiler, &fromList, to);

	return false;
}


CFilerEntry *CFiler::
fromString(CString sName)
{
	CFilerEntry * fe = root();

	CString sDir;
	// This makes the while loop simpler
	sName += separator(); 
	while (!sName.IsEmpty())
	{
		// Get the name of the current dir-level
		sDir = sName.Left(sName.Find(separator()));
		// Remove the name of the dir-level
		sName = sName.Mid(sName.Find(separator()) + 1);
		// Init the find procedure
		if (!findFile(fe))
			return 0;

		// Find the item
		bool found = false;
		while (fe = findFileNext()) {
			if (fe->Name() == sDir)
			{
				found = true;
				break;
			}
		}
		if (found == false)
			return 0;
		// fe should now contain the CFilerEntry for this level	
	}

    return fe;
}


CString CFiler::
toString(CFilerEntry *pEntry, bool bAbsolute)
{
	VERIFY(isValid(pEntry));

    if (isRoot(pEntry))
        return rootName();

    CString path;
    
    while (pEntry != 0 && isRoot(pEntry) == false)
    {
        // If this is not the first element, insert the separator.
        if (path.IsEmpty() == false)
            path = separator() + path;
        
        path = pEntry->Name() + path;

        // Loop for the next element.
        pEntry = pEntry->Parent();

        // Can we use our current directory?
        if (bAbsolute == false && currentDir_ != 0 && pEntry == currentDir_)
            break;
    }

    return path;
}


bool CFiler::
getIcons(CFilerEntry *pEntry, int &icon, int &selIcon)
{
	VERIFY(isValid(pEntry));

	calcIcons(pEntry);

	icon = pEntry->IconIndex(false);
	selIcon = pEntry->IconIndex(true);

    return true;
}


bool CFiler::
hasFlash()
{
	return false;
}


bool CFiler::
updateFlash(const CString &sFlashFilename)
{
	return false;
}


bool CFiler::
hasSubDir(CFilerEntry *pEntry)
{
	VERIFY(isValid(pEntry));

    // Is it a file?  Files cannot have subdirectories.
    if (pEntry->Directory() == false)
        return false;

	// If this is set, reading dirs is expensive.
	// If the dir has not been read in yet, just return yes so that
	// the subdirs are not read.
	if (lieAboutSubDirs_ == true && hasBeenRead(pEntry) == false)
		return true;

    // First, make sure we have this directory-entry.
	// This will make sure it's read in, and that we have our FilerDir*.
    CFilerDir *pDir = CFilerDir::CAST(pEntry);
    
	if (readDir(pDir) == false)
		return false;

    // Now we look for subdirectories...
	CFilerEntryList *list = pDir->entries_;
	POSITION pos = list->GetHeadPosition();
    while (pos != 0)
	{
		CFilerEntry *fe = list->GetNext(pos);
        // ...till we find one...
        if (fe->Directory() == true)
            return true;
    }

	// ... or not.
    return false;
}


// This method says whether findFile has been called on this entry.
// Always returns true if called with a file.  Returns false if
// called on a device like a floppy which didn't read before.
bool CFiler::
hasBeenRead(CFilerEntry *entry)
{
	VERIFY(isValid(entry));

    // Is it a file?  We say they're always in a has-been-read state.
    if (entry->Directory() == false)
        return true;

    // First, make sure we have this directory-entry.
	// This will make sure it's read in, and that we have our FilerDir*.
    CFilerDir *dir = CFilerDir::CAST(entry);
	VERIFY(dir!=0);
    
	return dir->entries_ != 0;
}


unsigned int CFiler::level(CFilerEntry *entry)
{
	VERIFY(isValid(entry));

	unsigned int ret = 0;

    while (isRoot(entry) == false)
    {
		ret++;
        // Loop for the next element.
        entry = entry->Parent();
    }
	
    return ret;
}


CFilerEntry *CFiler::
root()
{
	if (root_ == 0)
		root_ = (CFilerDir *)newCFilerEntry(rootName(), true, CFilerEntry::NA);

	return root_;
}


// These routines manage the CFilerEntry pool.
CFilerEntry *CFiler::
newCFilerEntry(const CString &name, bool dir, unsigned int size)
{
    CFilerEntry *entry;
    if (dir == true)
		entry = new CFilerDir(name, dir, size);
    else
		entry = new CFilerEntry(name, dir, size);

    // Add it to the allocation pool.
    entryPool_.AddTail(entry);

#if 0 && defined(_DEBUG)
    if (root_ != 0)
		TRACE("CFiler::newCFilerEntry: filer=%p, entry=%p, name=%s\n", this, entry, toString(entry));
	else
		TRACE("Creating Root entry: %p\n", entry);
#endif

    return entry;
}


void CFiler::
deleteEntryPool()
{
    // Remove every item from the allocation pool.
    POSITION pos = entryPool_.GetHeadPosition();
    while (pos != 0)
    {
        CFilerEntry *fe = entryPool_.GetNext(pos);

        delete fe;
    }

    entryPool_.RemoveAll();

	root_ = 0;
}


void CFiler::
deleteCFilerEntry(CFilerEntry *victim)
{
	VERIFY(isValid(victim));

	// If it's a directory, zap all its children.
    if (victim->Directory() == true)
    {
		CFilerDir *dir = CFilerDir::CAST(victim);
		CFilerEntryList *list = dir->entries_;
		// The list will be 0 if this directory hasn't been read yet.
		if (list != 0)
		{
			POSITION pos = list->GetHeadPosition();
			while (pos != 0)
			{
				CFilerEntry *fe = list->GetNext(pos);
				deleteCFilerEntry(fe);
			}
			list->RemoveAll();
			delete dir->entries_;
			dir->entries_ = 0;
		}
	}

	// Remove it from the directory that contains it.
	CFilerEntry *parent = victim->Parent();
	if (parent != 0)
	{
		CFilerDir *dir = CFilerDir::CAST(parent);
		POSITION pos = dir->entries_->Find(victim);
		VERIFY(pos!=0);
		// Double check this is the entry.
		VERIFY(dir->entries_->GetAt(pos) == victim);
		// Ok it is, so remove it.
		dir->entries_->RemoveAt(pos);
	}

	// Now remove the entry from the entry pool.
	POSITION pos = entryPool_.Find(victim);
	VERIFY(pos);
	entryPool_.RemoveAt(pos);
	
	// And finally delete the entry itself.
	delete victim;

	return;
}


void CFiler::deleteIOBuffers(IORequest *IOReq)
{
	POSITION pos = IOReq->list.GetHeadPosition();

	// Iterate through the list, delete the "file"-buffers and the
	// IOBuffer entries
	while (pos) {
		IOBuffer * iobuff = IOReq->list.GetNext(pos);
		if (iobuff->buffer != 0)
			delete [] iobuff->buffer;
		delete iobuff;
	}
	// Remove the entries from the list.
	IOReq->list.RemoveAll();
}


void CFiler::
rmEntries(CFilerEntryList *pVictims)
{
	POSITION pos = pVictims->GetHeadPosition();
	while (pos != 0)
	{
		CFilerEntry *fe = pVictims->GetNext(pos);
		if (isValid(fe))
			invalidate(fe);
	}
}

bool CFiler::reRead(CFilerEntry *fe)
{
	if (!isAvailable())
		return false;

	// Cast to a dir
	CFilerDir * dir = CFilerDir::CAST(fe);
	if (dir != 0)
	{
		// Do the reread.
		return readDir(dir, true);
	}
	
	return false;
}


void CFiler::cleanUp(CStringArray *psa, CFilerDir *dir)
{
	// Compare every name from the string array (which contains the
	// correct dir entries) with the filer entries.
	
	// Store the entries which have to be deleted.
	CFilerEntryList victims;

	POSITION pos = dir->entries_->GetHeadPosition();
	int size = psa->GetSize();
	int i = 0;
	while (pos)
	{
		bool bFound = false;
		// We don't have to start from the beginning of the array every time.
		// Start there, where the last item has been found.  This is likely
		// to speed up operations where entries are stored in more or less
		// the right order.
		for (i = 0; i < size; i++)
		{
			CString sEntryName = dir->entries_->GetAt(pos)->Name();
			CString sArrayName = psa->GetAt(i);
			bFound = (sEntryName == sArrayName);
			if (bFound)
				break;
		}
		if (!bFound)
			// This is a file we must delete, because it's not here any more.
			victims.AddTail(dir->entries_->GetAt(pos));

		dir->entries_->GetNext(pos);
	}

	rmEntries(&victims);
}


CFilerEntry* CFiler::
findOrCreate(CFilerEntry *dir, CFilerEntry *fe, bool *existed /* = 0 */)
{
    if (fe == 0 || dir == 0)
        return 0;

    VERIFY(dir->Directory());

	if (existed != 0)
		*existed = true;

    // Does it already live here?
    if (fe->Parent() == dir)
        return fe;

    // Get the details we'll need to look for it.
    CString entryName = fe->Name();
    CFilerDir *ddir = CFilerDir::CAST(dir);

    CFilerEntry *foundfe = ddir->findEntry(entryName);
    if (foundfe != 0)
	{
		// Oops, it exists.  Find out if the user wants to overwrite it.
		bool res = overwrite(dir, entryName);

		if (res == false)
			return 0;

        return foundfe;
	}

	if (existed != 0)
		*existed = false;

    // Not found, so create it.
    foundfe = newCFilerEntry(entryName, fe->Directory(), fe->Size());

    // Add it to the dir.
    ddir->add(foundfe);

    return foundfe;
}


CFiler::editReturn CFiler::
editFilename(CString &name)
{
    if (validFilename(name) == true)
        return OK;

    CNewFilename dlg;
	dlg.sOldFilename = name;
	dlg.sNewFilename = dlg.sOldFilename;
	while (1)
	{
		int res = dlg.DoModal();
		if (res == IDOK && (validFilename(dlg.sNewFilename) == true))
        {
            name = dlg.sNewFilename;

			return OK;
        }

		if (res == IDIGNORE)
			return SKIP;

        if (res == IDCANCEL)
            return CANCEL;

        // Not valid, but user pressed ok.  Go around again.
		dlg.sOldFilename = dlg.sNewFilename;
/*
		case IDOK: sRes = "OK"; break; // "OK"
		case IDCANCEL: sRes = "Cancel"; break; // "Cancel"
		case IDIGNORE: return false; break; // "Skip"
*/
	}
}


#ifdef _DEBUG
CString CFiler::
showString(CString s)
{
    CString s1 = s;
    CString ret;

    while (s1.GetLength() > 0)
    {
        char x=s1[0];
        if (isascii(x))
            ret += x;
        else
        {
            CString p;
            p.Format("#%3u", (unsigned int)(unsigned char)x);
            ret += p;
        }
        s1.Delete(0, 1);
    }

    return ret;
}

void CFiler::
showDir(CFilerDir *fd)
{

	CFilerEntryList *fel = fd->entries_;

    if (fel == 0)
    {
        TRACE("No entries\n");
        
        return;
    }

    POSITION pos = fel->GetHeadPosition();
    int i=0;
    while (pos != 0)
    {
        CFilerEntry *fe = fel->GetNext(pos);

        TRACE("%d: %s%s\n", ++i, showString(fe->Name()), fe->Directory() ? " (dir)" : "");
    }
}
#endif
