// CalcTypes.h: interface for the CCalcTypes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALCTYPES_H__767721E5_F951_11D3_9901_0060B0A2563C__INCLUDED_)
#define AFX_CALCTYPES_H__767721E5_F951_11D3_9901_0060B0A2563C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCalcType  
{
    // Grr, if I make this private and CCalcTypes a friend, I can't use
    // static initialisation.... :-(
public:
    enum {langs_ = 3};

	int iconIndex_;
	char *msg_[langs_];
};


class CCalcTypes
{
public:
	static void Destroy();
    static CCalcTypes *instance();

    // const char *lookup(const char *key);
    bool isA(const char *key, const char *candidate);
    bool known(const char *key);
    const int icon(const char *key);

    // These entries are just for directories, since they're most common.
    bool isADirectory(const char *candidate);
    const int directoryIcon();
    const int openDirectoryIcon();

private:
    CCalcTypes();

    CCalcType *find(const char *key);

    static CCalcTypes *instance_;

    static int msgs_;
    static CCalcType msg_[];
    // TODO: Hits will eventually be used to guess the right language based on
    // past lookups for when entries exist in more than one language.
    // int hits_[CCalcType::langs_];

    // Stores a pointer to the most recently accessed item, to speed lookups.
    CCalcType *mra_;
    // Stores the index of the most recently used language.  This will be
    // checked first.
    int mrl_;

    CCalcType *directory_;
    int directoryIcon_;
    int openDirectoryIcon_;
};

#endif // !defined(AFX_CALCTYPES_H__767721E5_F951_11D3_9901_0060B0A2563C__INCLUDED_)
