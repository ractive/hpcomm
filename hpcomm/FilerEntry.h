/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// FilerEntry.h: interface for the CFilerEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILERENTRY_H__F55E4293_1682_11D3_98A5_0060B0A2563C__INCLUDED_)
#define AFX_FILERENTRY_H__F55E4293_1682_11D3_98A5_0060B0A2563C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include <afxtempl.h>

class CFiler;

// The basic filer type.
class CFilerEntry
{
	friend class CFilerDir;
    friend class CFiler;
	friend class CFilerPC;
	friend class CFiler4x;
	// FIXME: temporary.  Should be CProt.
	friend class CKermit;

public:
    typedef enum {NA = (unsigned int)-1};

    // Public methods for testing the attributes of a CFilerEntry.

    // Whether this entry is a directory.
    bool Directory();

    // Name of this object, using the PC charset.
    const CString &Name();

    // 0 if this is in the root space.
    CFilerEntry *Parent();

    // NA if a directory.
    unsigned int Size();

    // Get and set the user field.
    unsigned int user1();
    void user1(unsigned int user1);

protected:
    CFilerEntry(const CString &name, bool dir, unsigned int size=NA);
    virtual ~CFilerEntry();

    // The four attributes of a CFilerEntry.
    bool dir_;
    CString name_;
    CFilerEntry *parent_;
    unsigned int size_;
    // This int is used as an index into the appropriate icon type.
    int iconIndex_;
    int iconIndexSelected_;

    unsigned int user1_;

    // Private methods for setting the above.
    void Directory(bool dir);
    void Name(const CString name);
    void Parent(CFilerEntry *parent);
    void Size(unsigned int size);
    // This int is used as an index into the appropriate icon type.
    int IconIndex(bool selected);
    void IconIndex(bool selected, int iconIndex);

    bool descendedFrom(CFilerEntry *possibleParent);
};

// Templated collection type to represent a bunch of these.
typedef CTypedPtrList<CPtrList, CFilerEntry*> CFilerEntryList;

#endif // !defined(AFX_FILERENTRY_H__F55E4293_1682_11D3_98A5_0060B0A2563C__INCLUDED_)
