// FilerDir.h: interface for the CFilerDir class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILERDIR_H__CC187F44_1972_11D3_98A7_0060B0A2563C__INCLUDED_)
#define AFX_FILERDIR_H__CC187F44_1972_11D3_98A7_0060B0A2563C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FilerEntry.h"

class CFilerDir: public CFilerEntry
{
    friend class CFiler;
	friend class CFilerPC;
	friend class CFiler4x;

private:
    CFilerDir(const CString &name, bool dir, unsigned int size=NA);
    virtual ~CFilerDir();

	// Downcast a CFilerEntry to a CFilerDir.  Only for the use of
	// CFilers.
	static CFilerDir *CAST(CFilerEntry *entry);

	// Add an entry.  Note that adding 0 means force creation of the list.
	// It does NOT mean add the root.  This is a hack fix for a bug where
	// an empty directory wasn't causing the list to be created.
    void add(CFilerEntry *entry);
	
    bool containsEntry(CFilerEntry *entry);
	CFilerEntry *findEntry(CString &sName);

	// A collection holding the subentries.  If this is 0, it
	// means that subentries have not been read yet.  It is
	// valid for it to be allocated, but hold no entries, as
	// this indicates a terminal empty subdir.
    CFilerEntryList *entries_;
};

#endif // !defined(AFX_FILERDIR_H__CC187F44_1972_11D3_98A7_0060B0A2563C__INCLUDED_)
