// FilerPC.h: interface for the CFilerPC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILERPC_H__F55E4299_1682_11D3_98A5_0060B0A2563C__INCLUDED_)
#define AFX_FILERPC_H__F55E4299_1682_11D3_98A5_0060B0A2563C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Filer.h"

// This class implements methods for the PC filespace.  Method comments
// are as for the CFiler class, unless noted.
class CFilerPC : public CFiler  
{
public:
    CFilerPC();
    virtual ~CFilerPC();

	bool isValidDropTarget(CFilerEntry *pEntry);
	bool canBeDragged(CFilerEntry *pEntry);

	virtual bool isAvailable();
    virtual bool isLocal();
	virtual bool reconnect();

	// Suggests a name for a new directory.  The name of the new directory
	// will be chosen not to clash with any existing directory.
	virtual CString suggestFolderName(CFilerEntry *parent);
    // Creates a new directory, and returns the CFilerEntry*.
	virtual CFilerEntry *newFolder(CFilerEntry *pParent, CString sName);

    virtual CFilerEntry *createArchive(CFiler *toFiler, CFilerEntry *toParent,
									   CString fname);
    virtual bool restoreFromArchive(CFiler *fromFiler, CFilerEntry *fromfe);

    virtual bool hasDateTime();
    virtual bool getDateTime(CFilerEntry *pEntry, SYSTEMTIME &st);

    virtual bool hasCRC();
    virtual bool getCRC(CFilerEntry *pEntry, unsigned int &crc);

    virtual bool hasFlash();
    virtual bool updateFlash(const CString &sFlashFilename);

    virtual const CString &rootName();
	virtual char separator();

protected:
	virtual bool _rm(CFilerEntryList *pVictims);
	virtual bool _rename(CFilerEntry *pEntry, const CString &sNewName);
    virtual bool _cp(bool removeAfterCopy, CFiler *fromFiler,
                     CFilerEntryList *fromList, CFilerEntry *to);

    // Read entries.  This method works recursively.
    virtual bool readDir(CFilerDir *parent, bool reRead = false);
    // Read the root entries.  These are the drive letters.  This method
	// is a helper for readDir.
    bool readRoot(CFilerDir *root);

	virtual void calcIcons(CFilerEntry *entry);
	virtual int getDirIcon();

	// This method is called by cp when doing cross-filespace copies, so
	// that ridiculous transfers aren't attempted.  On a PC, this returns
	// 0, which means no limit.
	virtual unsigned int biggestObject(CFilerEntry *where);

    virtual bool transfer(CFiler::IORequest *IOReq);

	virtual bool validFilename(const CString &name);
	virtual bool overwrite(CFilerEntry *parent, const CString &name);

private:
    static const CString myComputer;

};

#endif // !defined(AFX_FILERPC_H__F55E4299_1682_11D3_98A5_0060B0A2563C__INCLUDED_)
