// FilerEntry.cpp: implementation of the CFilerEntry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FilerEntry.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilerEntry::
CFilerEntry(const CString &name, bool dir, unsigned int size)
{
    dir_ = dir;
    name_ = name;
    parent_ = 0;
    size_ = size;
    iconIndex_ = -1;
    iconIndexSelected_ = -1;
    user1_ = 0;
}


CFilerEntry::
~CFilerEntry()
{
}


void CFilerEntry::
Directory(bool dir)
{
    dir_ = dir;
}


bool CFilerEntry::
Directory()
{
    return dir_;
}


void CFilerEntry::
Name(const CString name)
{
    name_ = name;
}


const CString &CFilerEntry::
Name()
{
    return name_;
}


void CFilerEntry::
Parent(CFilerEntry *parent)
{
    parent_ = parent;
}


CFilerEntry *CFilerEntry::
Parent()
{
    return parent_;
}


void CFilerEntry::
Size(unsigned int size)
{
    size_ = size;
}


unsigned int CFilerEntry::
Size()
{
    return size_;
}


void CFilerEntry::
IconIndex(bool selected, int iconIndex)
{
    if (selected)
		iconIndexSelected_ = iconIndex;
    else
		iconIndex_ = iconIndex;
}


int CFilerEntry::
IconIndex(bool selected)
{
    return selected ? iconIndexSelected_ : iconIndex_;
}


// Get and set the user field.
unsigned int CFilerEntry::
user1()
{
    return user1_;
}


void CFilerEntry::
user1(unsigned int user1)
{
    user1_ = user1;
}


bool CFilerEntry::
descendedFrom(CFilerEntry *possibleParent)
{
    // Handle the top-most case.
    if (possibleParent == 0 && parent_ == 0)
        return true;

    // Follow our parents up the tree.
    CFilerEntry *parent = parent_;
    while (parent != 0)
    {
        if (parent == possibleParent)
            return true;

        parent = parent->parent_;
    }

    return false;
}
