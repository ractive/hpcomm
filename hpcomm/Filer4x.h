// Filer4x.h: interface for the CFiler4x class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILER4X_H__F55E4299_1682_11D3_98A5_0060B0A2563C__INCLUDED_)
#define AFX_FILER4X_H__F55E4299_1682_11D3_98A5_0060B0A2563C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Filer.h"

class CProt;

// This class implements methods for the HP48 and HP49G calculator
// filespaces.  Method comments are as for the CFiler class, unless noted.

// This class is a sibling of CFilerPC.

class CFiler4x : public CFiler  
{
public:
    CFiler4x(CProt *prot);
    virtual ~CFiler4x();

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

	virtual const CString &rootName();
	virtual char separator();

	void setProtocol(CProt *pProt);

protected:
	virtual bool _rm(CFilerEntryList *pVictims);
	virtual bool _rename(CFilerEntry *pEntry, const CString &sNewName);
    virtual bool _cp(bool removeAfterCopy, CFiler *fromFiler,
                     CFilerEntryList *fromList, CFilerEntry *to);

    // Read entries.  This method works recursively.
    virtual bool readDir(CFilerDir *parent, bool reRead = false);

	// Calculate the icon indices of an entry
	virtual void calcIcons(CFilerEntry *entry);
	virtual int getDirIcon();

	// This method is called by cp when doing cross-filespace copies, so
	// that ridiculous transfers aren't attempted.  Here, this is 128k
	// for a HP48, and 256k for an HP49G.
	virtual unsigned int biggestObject(CFilerEntry *where);

    virtual bool transfer(CFiler::IORequest *IOReq);

	virtual bool validFilename(const CString &name);
	virtual bool overwrite(CFilerEntry *parent, const CString &name);

private:
	// We store the real current calculator directory here.
	// This leaves currentDir free for the client.
	// Note, this autochanges entry to the root if 0 is passed.
	bool currentCalcDir(CFilerDir *&dir);
	CFilerDir *currentCalcDir();

	// Set the calculator to the top directory.
	bool home();
	// Change to a given directory.
	bool changeDir(CString * sDir);
	// ???
	// void changeDir();

#if 0
	// Ensure that the calc dir is set to our dir.
	void syncCurDir();
#endif

	CFilerDir *currentCalcDir_;

    static const CString home_;

	class CProt *prot_;
};

#endif // !defined(AFX_FILER4X_H__F55E4299_1682_11D3_98A5_0060B0A2563C__INCLUDED_)
