// Filer4x.cpp: implementation of the CFiler4x class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FilerEntry.h"
#include "Filer4x.h"
#include "FilerDir.h"
#include "Prot.h"
#include "ProgressDlg.h"
#include "ProtException.h"
#include "CalcTypes.h"

const CString CFiler4x::home_("HOME");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFiler4x::
CFiler4x(CProt *prot)
{
	VERIFY(prot!=0);
	prot_ = prot;

	// Digging through subdirs is an expensive operation, so we
	// pretend that every dir entry has subdirs.
	lieAboutSubDirs_ = true;	
	currentCalcDir_ = 0;
}

CFiler4x::
~CFiler4x()
{
}


bool CFiler4x::
isValidDropTarget(CFilerEntry *pEntry)
{
	// Every item can be a drop target on the calculator.
	return true;
}


bool CFiler4x::
canBeDragged(CFilerEntry *pEntry)
{
	// Don't allow the root item to be dragged.
	return pEntry != 0 && !isRoot(pEntry);
}


bool CFiler4x::
isAvailable()
{
	return prot_->IsConnected();
}


bool CFiler4x::
isLocal()
{
    return false;
}


bool CFiler4x::
reconnect()
{
	return prot_->Connect();
}


bool CFiler4x::
transfer(CFiler::IORequest *IOReq)
{
	// Change the directory so entries get put in the right place.
	// FIXME: This hack assumes that the directory of the first
	// element is the directory of the rest.
	POSITION pos = IOReq->list.GetHeadPosition();
	if (pos == 0)
		return false;

	IOBuffer *iob = IOReq->list.GetAt(pos);
	CFilerEntry *fe = iob->entry;
	CFilerDir *dir = CFilerDir::CAST(fe->Parent());
	
	if (currentCalcDir(dir) == false)
		return false;
	
	return (prot_->Transfer(IOReq) == CProt::SUCCESS);
}


bool CFiler4x::
_rm(CFilerEntryList *pVictims)
{
	bool bRes = true;

	// Do nothing if the list is empty
	if (pVictims->IsEmpty())
		return true;

	if (prot_->IsConnected() == false)
		return false;
	
	// Make a list of the files to delete
	// The string should look like this at the end: "{file1 file2} PURGE"
    bool ActuallyDelete = false;
	CString sList = "{";
	CFilerEntry *fe;
	POSITION pos = pVictims->GetHeadPosition();
	while (pos)
	{
		fe = pVictims->GetNext(pos);
		// Ask the user if he really wants to delete dirs.
		if (fe->Directory()) {
			CString msg = "Are you sure you want to delete the \"" + fe->Name() + "\" directory?";
			int ans = AfxMessageBox(msg, MB_YESNOCANCEL | MB_ICONQUESTION);
			if (ans == IDYES) {
				// The user really wants to delete the dir.
				// His wish is my order :-)

				// Make sure the calc changes to the parent of the dir to be deleted.
				CFilerEntry * curdirfe = fe->Parent();
				CFilerDir * curdir = CFilerDir::CAST(curdirfe);
				if (currentCalcDir(curdir) == false)
					return false;

				CString sDir = "'";
				sDir += fe->Name() + "' PGDIR";
				prot_->nSeq = 0;
				prot_->SendPacket('C', sDir.GetLength(), (LPTSTR)(LPCTSTR)sDir);
				prot_->RecvStack();

				continue;
			}
			else if (ans == IDNO) {
				// No, he doesn't want to delete it.
				continue;
			}
			else if (ans == IDCANCEL) {
				// Stop the whole thing.
				return false;
			}

		}
        else {
            CString msg = "Are you sure you want to delete the \"" + fe->Name() + "\" file?";
            int ans = AfxMessageBox(msg, MB_YESNOCANCEL | MB_ICONQUESTION);
			if (ans == IDCANCEL) {
				// Stop the whole thing.
				return false;
			}
            else if (ans == IDNO) 
                continue;
            else 
                ActuallyDelete = true;
        }
		// Split it in different packets if the length is bigger than a packetlength.
		if (sList.GetLength() + fe->Name().GetLength() > 80) {
			sList += "} PURGE";
			prot_->nSeq = 0;
			prot_->SendPacket('C', sList.GetLength(), (LPTSTR)(LPCTSTR)sList);
			if (prot_->RecvStack() == TRUE) {
				sList = "{";
			}
			// Could not be processed.
			else {
				return false;
			}
			
		}
		sList += fe->Name() + " ";
	}
	// Complete the HP command
    if (ActuallyDelete == false)
        return true;
	
    sList += "} PURGE";
    prot_->nSeq = 0;
    prot_->SendPacket('C', sList.GetLength(), (LPTSTR)(LPCTSTR)sList);
    return (prot_->RecvStack() == TRUE);
}


bool CFiler4x::
_rename(CFilerEntry *pEntry, const CString &sNewName)
{
	if (isRoot(pEntry))
		return false;
	CFilerDir * parent = CFilerDir::CAST(pEntry->Parent());
	if (currentCalcDir(parent) == false)
		return false;

	bool ret = prot_->rename(pEntry->Name(), sNewName);

	if (ret == false)
		return false;

	// If it was successful, rename the entry...
	pEntry->Name(sNewName);

	// Note, we DONT force the recalculation of the icons
	// because we only know this when the calc tell us.
	// (And the type is unlikely to change because of a rename...
	// pEntry->IconIndex(true, -1);
	// pEntry->IconIndex(false, -1);

	return true;

}


bool CFiler4x::
_cp(bool removeAfterCopy, CFiler *fromFiler, 
    CFilerEntryList *fromList, CFilerEntry *to)
{
	if (fromFiler != this)
		return false;
	
	bool res = prot_->cp(removeAfterCopy, this, fromList, to);

	return res;
}


CString CFiler4x::
suggestFolderName(CFilerEntry *parent)
{
	// A new directory is called "NEW" (or an incremented variant)
	// on the calc.
	CString name("NEW");
	CFilerEntry *fe;
	int i = 1;

	// Does the name exist?  If so, just keep incrementing it.
	while ((fe = CFilerDir::CAST(parent)->findEntry(name)) != 0)
		name.Format("NEW%d", i++);

	return name;
}


CFilerEntry *CFiler4x::
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
	sName = fe->Name();

	currentCalcDir(parentDir);
	if (prot_->newFolder(sName) == false) 
	{
		deleteCFilerEntry(fe);

		return 0;
	}

    CCalcTypes *ct = CCalcTypes::instance();
	fe->user1(0);
	fe->IconIndex(false, ct->directoryIcon());
	fe->IconIndex(true, ct->openDirectoryIcon());

	// We know that the new dir is empty, so we add a 0 entry to
	// "initialise" the directory.
	CFilerDir *newdir = CFilerDir::CAST(fe);
	newdir->add(0);

	return fe;
}


bool CFiler4x::
hasDateTime()
{
    return false;
}


bool CFiler4x::
getDateTime(CFilerEntry *pEntry, SYSTEMTIME &st)
{
    return false;
}


bool CFiler4x::
hasCRC()
{
    // Not supported at the moment.
    return true;
}


bool CFiler4x::
getCRC(CFilerEntry *pEntry, unsigned int &crc)
{
    crc = pEntry->user1();

	return true;
}


// Calling this method ensures that all the PC dirs from here up exist.
// If they don't, it creates them.
bool CFiler4x::
readDir(CFilerDir *dir, bool reRead /* = false */)
{
	// Are we passed a directory?
	VERIFY(dir->Directory());

#if 0 && defined(_DEBUG)
	TRACE("CFiler4x::readDir: dir=%s\n", toString(dir));
#endif

    CCalcTypes *ct = CCalcTypes::instance();

	// Have we already read this directory?
	if (!reRead && hasBeenRead(dir))
		return true;

	bool res;

	if (reRead) {
		// If we re-read a directory, we have to make sure that the directory on the calc
		// is set correctly (i.e. if the user was browsing "offline" and makes a refresh.
		currentCalcDir(dir);
	}

    // First we ensure that the parent is read in.  This is recursive.
	if (isRoot(dir) == false)
	{
	    // but only if we don't have the root item.
		res = readDir(CFilerDir::CAST(dir->Parent()));

		if (res == false)
			return false;
	}
	else
    {
        // We're reading the root.  Set the directory on the calc if we've never done it.
        if (currentDir() == 0)
            currentDir(dir);

        // These are the icons for the HP HOME dir.
        int icon = ct->icon("HOME");
        dir->IconIndex(false, icon);
        dir->IconIndex(true, icon);
    }


    // Now we read this directory in.
	if (currentCalcDir(dir) == false)
		return false;

	CStringArray sa;

	CString sRemoteDir;
	CProt::ERRCODE code = prot_->readRemoteDir(sRemoteDir);

#if 0
    TRACE("%s\n", showString(sRemoteDir));
    CFiler::showDir(dir);
#endif

	if (code != CProt::SUCCESS)
		return false;

	if (sRemoteDir.IsEmpty()) {
		dir->add(0);
		if (reRead) {
			cleanUp(&sa, dir);
		}
		return true;
	}

	// The HP48GX sends the current dir with the directory content, whereas
	// the HP48SX does not.  So we just remove the current dir to handle both the same
	// way.
	if (sRemoteDir.Left(1) == "{") {
		sRemoteDir = sRemoteDir.Mid(sRemoteDir.Find('\r') + 2);
	}

	char name[50], sType[30]; // What's the max. filename length on the HP???
	int nSize;
	unsigned int nCheckSum;

	bool dirIsBlank = true;


	while ((sRemoteDir.Find('\n') != -1))
	{
		// Unfortunately the type info can contain a whitespace
		// (e.g. "Real Number") which makes things messy.

		// Extract a line of the directory-string.

		CString sLine = sRemoteDir.Left(sRemoteDir.Find('\n'));
		sscanf(sRemoteDir, "%s %d", name, &nSize);
		sLine = sLine.Mid(sLine.Find(' ') + 1);
		sLine = sLine.Mid(sLine.Find(' ') + 1);
		strcpy(sType, sLine.Left(sLine.ReverseFind(' ')));
		sLine = sLine.Mid(sLine.ReverseFind(' ') + 1);
		sscanf(sLine, "%u", &nCheckSum);

		sa.Add(name);

		CFilerEntry *fe;
		if (reRead && ((fe = dir->findEntry((CString)name)) != 0))
		{
			// The dir already contains this entry, so we update it.
            bool b = ct->known(sType);
            if (b == false)
            {
                CString msg;
                msg.Format("CFiler4x: Entry \"%s\" has unknown type \"%s\".  Please report this!\n",
                    name, sType);
                AfxMessageBox(msg, MB_ICONEXCLAMATION | MB_OK);
                // By default leave b as false, to indicate it's not a dir.
            }
            else
                b = ct->isADirectory(sType);

            // Check if the file was a directory before.
			if (fe->Directory() != b) {
				if (fe->Directory()) {
					// The entry changed from a directory to a file, so we
					// have to make a CFilerEntry from the CFilerDir
					CFilerEntry * newfe = newCFilerEntry(fe->Name(), false, nSize);
					CFilerEntryList list;
					list.AddHead(fe);
					rmEntries(&list);
					fe = newfe;
					dir->add(fe);
					
				}
				else {
					// A directory was transferred to the calc, but as for the PC
					// the dir is just a normal file, it is also transferd as such a file.
					// We have to make a CFilerDir out of the CFilerEntry.
					CFilerEntry * newdir = newCFilerEntry(fe->Name(), true, nSize);
					CFilerEntryList list;
					list.AddHead(fe);
					rmEntries(&list);
					fe = newdir;
					dir->add(fe);
				}
			}
			else {
				fe->Name(name);
				fe->Size(nSize);
				calcIcons(fe);
			}
		}
		else
		{
			// Create the new CFilerEntry.
			fe = newCFilerEntry(name, ct->isADirectory(sType), nSize);
			// Add it to the dir.
			dir->add(fe);
		}

		fe->user1(nCheckSum);

		// Now we calculate the icons.
		while (1)  // This trick is so we can use break instead of if-else-if.
		{
			if (ct->isADirectory(sType))
			{
				// If it's a directory we know that this subdirectory
				// can't be read in yet.
				fe->IconIndex(false, ct->icon("Unvisited Directory"));
				fe->IconIndex(true, ct->openDirectoryIcon());
				break;
			}

			int icon = ct->icon(sType);
			fe->IconIndex(false, icon);
			fe->IconIndex(true, icon);

			break;
		}

		// Move to the next entry.
		sRemoteDir = sRemoteDir.Mid(sRemoteDir.Find('\n') + 1);
		dirIsBlank = false;
    }

	if (!isRoot(dir))
	{
		// The dir is read, but isn't the root. Update the icons for this
		// directory.
		dir->IconIndex(false, ct->directoryIcon());
		dir->IconIndex(true, ct->openDirectoryIcon());
	}
    else
    {
        // These are the icons for the HP HOME dir.
        int icon = ct->icon("HOME");
        dir->IconIndex(false, icon);
        dir->IconIndex(true, icon);
    }

	if (reRead)
		cleanUp(&sa, dir);

	// If there were no entries, we still need to force the
	// list to exist.
	if (dirIsBlank == true)
		dir->add(0);

    return true;
}


// Convert an object type string into an icon index.
void CFiler4x::
calcIcons(CFilerEntry *entry)
{
	// This next line avoids calculating the icon if it's been done
	// before.  If the icon has changed, then invalidate() should be
	// called.
	if (entry->IconIndex(false) != -1 && entry->IconIndex(true) != -1)
		return;

    // Note, this function doesn't do much, because the icons
    // are computed when we read the dir in.
}


int CFiler4x::
getDirIcon()
{
    return CCalcTypes::instance()->directoryIcon();
}


// This method is called by cp when doing cross-filespace copies, so
// that ridiculous transfers aren't attempted.  Here, this is 128k
// for a HP48, and 256k for an HP49G.
unsigned int CFiler4x::
biggestObject(CFilerEntry *where)
{
	// TODO: Put code in here to return 256k for the HP49G.
    return 128*1024;
}


const CString &CFiler4x::
rootName()
{
	// Just like E.T.!!!!
	return home_;
}


char CFiler4x::
separator()
{
	return ' ';
}


CFilerDir *CFiler4x::
currentCalcDir()
{
	return currentCalcDir_;
}


bool CFiler4x::
currentCalcDir(CFilerDir *&dir)
{
    // If we're not connected, pretend we were never called.
    if (prot_->IsConnected() == false)
        return false;

	// Set the calculator to home if this the first time.
	if (currentCalcDir_ == 0)
	{
		if (home() == false)
			return false;
	}

	// Only change the directory if we need to.
	if (currentCalcDir_ != dir)
	{
		if (changeDir(&toString(dir)) == false)
			return false;
	}

	currentCalcDir_ = dir;

	return true;
}

bool CFiler4x::
home()
{
	if (prot_->IsConnected() == false)
		return false;

	prot_->nBlockChkType = 1;	
	prot_->nSeq = 0;
	prot_->SendPacket('C', 4, "HOME");

	return prot_->RecvStack();
}


bool CFiler4x::
changeDir(CString *sDir)
{
	prot_->nSeq = 0;
	prot_->SendPacket('C', sDir->GetLength(), (LPTSTR)(LPCTSTR)*sDir);

	return prot_->RecvStack();
}


void CFiler4x::
setProtocol(CProt *pProt)
{
	VERIFY(pProt!=0);
	prot_ = pProt;
}


bool CFiler4x::
validFilename(const CString &name)
{
    if (name.GetLength() > 127 || name.GetLength() == 0)
        return false;

    // From section 5-5 of the HP48G Series User's Guide.
	// Note, apparently we _can_ use periods in the middle of filenames.
    if (name.FindOneOf(" ,@;") != -1)
        return false;
    if (name.FindOneOf("#[]\"'{}()\253\273:_") != -1)
        return false;
    if (name.FindOneOf("!+-*/^\203\204=<>\210\211\212\213") != -1)
        return false;
    CString first(name[0]);
    if (first.FindOneOf("0123456789") != -1)
        return false;
    // FIXME: It's also not valid to have names of existing HP commands.

    // HP49G
    if (name.FindOneOf("\206") != -1)
        return false;

	return true;
}


CFilerEntry *CFiler4x::
createArchive(CFiler *toFiler, CFilerEntry *toParent, CString fname)
{
	// Create a temporary CFilerEntry in the root of the src.
	CFilerEntry *srcfe = newCFilerEntry("hpcommarch", false, CFilerEntry::NA);
	// Add if needed.
	CFilerDir *rootdir = CFilerDir::CAST(root());
	rootdir->add(srcfe);

	// Create an IOBuffer for the archive.
	IOBuffer archBuffer;
	archBuffer.entry = srcfe;
	archBuffer.buffer = 0;
	archBuffer.success = false;

	// Create an IORequest for fetching the archive.
	IORequest archRequest;
	archRequest.filer = this;
	archRequest.biggestObject = biggestObject(root());
	archRequest.write = false;
	archRequest.list.AddTail(&archBuffer);

	// Now fetch the archive.
	bool res;
	prot_->bArchive = true;
	try {
		prot_->Archive(srcfe->Name());
		res = transfer(&archRequest);
	}
	catch (CProtException *pEx) {
		// Do our cleanup to date.
		deleteCFilerEntry(srcfe);
		if (archBuffer.buffer != 0)
			delete [] archBuffer.buffer;
		// Rethrow the exception.
		throw pEx;
	}

	prot_->bArchive = false;

	if (res == false || archBuffer.buffer == 0)
	{
		deleteCFilerEntry(srcfe);
		if (archBuffer.buffer != 0)
			delete [] archBuffer.buffer;

		return 0;
	}

	// Change the name to our name.
	srcfe->Name(fname);
	
	// Ok, we've read the archive in.  Now create a CFilerEntry* for it in
	// the other filer.
	bool existed;
	CFilerDir *todir = CFilerDir::CAST(toParent);
	CFilerEntry *destfe = toFiler->findOrCreate(todir, srcfe, &existed);

	// Now we can get rid of the source CFilerEntry*.
	deleteCFilerEntry(srcfe);

	// Did they decline to overwrite an existing one?
	if (destfe == 0)
	{
		deleteCFilerEntry(srcfe);

		return 0;
	}

	// Write it out to the other filer.  First, create the new IOBuffer.
	archBuffer.entry = destfe;
	archBuffer.success = false;

	// Then make the IORequest.
	archRequest.filer = toFiler;
	archRequest.biggestObject = toFiler->biggestObject(root());
	archRequest.write = true;

	// And write it.  FilerPC does not raise exceptions.
	res = toFiler->transfer(&archRequest);

	if (res == 0)
	{
		if (existed == false)
			toFiler->deleteCFilerEntry(destfe);

		destfe = 0;
	}

	if (archBuffer.buffer != 0)
		delete [] archBuffer.buffer;

	return destfe;
}


bool CFiler4x::
restoreFromArchive(CFiler *fromFiler, CFilerEntry *fromfe)
{
	CFilerEntry *fe = newCFilerEntry("hpcommarch", false, CFilerEntry::NA);
	CFilerDir *rootdir = CFilerDir::CAST(root());
	rootdir->add(fe);

	// Copy the file that contains the archive to restore.
	bool res;
	try {
		res = cp(false, fromFiler, fromfe, fe);
		prot_->Restore(fe->Name());
	}
	catch (CProtException *pEx) {
		deleteCFilerEntry(fe);
		throw pEx;
	}

	deleteCFilerEntry(fe);
	
	return res;
}


bool CFiler4x::
overwrite(CFilerEntry *parent, const CString &name)
{
	// TODO: Put code here to work out what to do on the calc re overwriting.
	return true;
}
