// FilerDir.cpp: implementation of the CFilerDir class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hpcomm.h"
#include "FilerDir.h"
#include "FilerEntry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilerDir::
CFilerDir(const CString &name, bool dir, unsigned int size)
	: CFilerEntry(name, dir, size), 
	  entries_(0)
{
}


CFilerDir::
~CFilerDir()
{
	if (entries_ != 0)
		delete entries_;
}


CFilerDir *CFilerDir::
CAST(CFilerEntry *entry)
{
	VERIFY(entry!=0);
	VERIFY(entry->Directory());

	return (CFilerDir *)entry;
}


void CFilerDir::
add(CFilerEntry *entry)
{
	// NOTE: This accepts an argument of zero, which means
	// just create the list.  It does NOT mean add the root.

	// TODO: Put in a check to prevent duplicate ptrs.
	// TODO: Put in a check to prevent duplicate names.
	if (entries_ == 0)
		entries_ = new CFilerEntryList;

	if (entry == 0)
		return;

	entry->Parent(this);
	entries_->AddTail(entry);
}


bool CFilerDir::
containsEntry(CFilerEntry *entry)
{
    VERIFY(entry!=0);

	if (entries_ == 0)
		return false;

    POSITION pos = entries_->Find(entry);

	return pos != 0;
}


CFilerEntry *CFilerDir::
findEntry(CString &sName)
{
	if (entries_ == 0)
		return false;

	POSITION pos = entries_->GetHeadPosition();
	while (pos)
	{
		CFilerEntry *fe = entries_->GetNext(pos);
		// Check if an entry with the same name exists (only one can).
		if (fe->Name() == sName)
			return fe;
	}

	// No luck. Return 0.
	return 0;
}
