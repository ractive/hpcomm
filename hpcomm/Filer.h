// Filer.h: interface for the CFiler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILER_H__F55E4294_1682_11D3_98A5_0060B0A2563C__INCLUDED_)
#define AFX_FILER_H__F55E4294_1682_11D3_98A5_0060B0A2563C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FilerEntry.h"
#include "FilerDir.h"

// CFiler is an abstract class which represent a filespace, and all
// the operations that can be performed on entries within that filespace.

// This class is not reentrant, because many methods have hidden
// state.  This class needs a way of returning detailed error codes.
// At the moment, methods return false or 0 to indicate a problem.

// This class makes no promise about when an underlying operation will
// take place.  The class might also internally read more than asked
// (eg, FindFirst), or do anything it feels like.

// The intention is that three other classes will be derived from
// this.  The first is CFilerPC, which represents the PC filespace.
// The second is CFiler4x, which represents the filespace on an HP4x
// series calculator.  The third is CFiler38, which represents the
// filespace contained within an HP38 virtual directory on the PC.  Note
// this latter class will have its own specialised ctor so the VD
// location can be set.

// This class maintains an internal data structure which represent a
// model of the filesystem they are representing.  Returned pointers
// are to the nodes in these internal filesystems.  Pointers will then
// be valid until invalidate is called, or the instance is destroyed.

// TODO: Provide some mechanism so that callers can determine
// error types.

// TODO: Define exceptions.

// TODO: Many of these methods need a callback so that the caller
// (which may be a UI class) can be kept notified of progress.

// ISSUE: Derived classes will have to be careful that helpers such
// as protocols are designed and used in such a way so that changes they
// make to the filespace are reflected in the data structures.

class CFiler
{
	// Strange but true...
	friend class CFiler4x;
public:
    CFiler();
    virtual ~CFiler();

	// This struct is used to hold data while being transferred between
	// filespaces.
	struct IOBuffer
	{
		// The entry for this buffer.  The Size() property is crucial.
		class CFilerEntry *entry;
		// When reading, if this ptr!=0, it is deallocated.  Memory for the
		// new data is then allocated using new[], and data is placed in
		// this buffer.  When writing, the data to be written lives in this
		// buffer.
		char *buffer;
		bool success;
	};

	typedef CTypedPtrList<CPtrList, IOBuffer*> IOBufferList;
	
	struct IORequest
	{
		// All entries come from this filer.
		CFiler *filer;
		// True for writing, false for reading.
		bool write;
		IOBufferList list;
		// When reading, this tells the protocol the upper bound of read
		// objects.
		unsigned int biggestObject;
	};

	// Re-read a directory.
	bool reRead(CFilerEntry *fe);

	// Call this method if a entry is a valid drop target.
	virtual bool isValidDropTarget(CFilerEntry *pEntry)=0;

	// Check if this entry can be dragged.
	// I.e. drives and the root items, can't be dragged.
	virtual bool canBeDragged(CFilerEntry *pEntry) = 0;

    // This method can be called to find out if a filer is ready to go.
    // For remote filesystems, this should return whether or not the
    // filer is connected.
	virtual bool isAvailable()=0;

    // This method can be called to find out if this filesystem resides on
    // the PC.
    virtual bool isLocal()=0;

	// Calling this tells the filer to try to reconnect.
	// It doesn't invalidate any entries.
	virtual bool reconnect()=0;

    // This method should be called when it is suspected that the
    // underlying filespace has changed without CFiler being aware of
    // it.  This terminates any finds, and causes the current dir to
    // become invalid.  If an argument is given, then this is taken
    // as a request to invalidate any entries below this point.
    // CFilerEntry*s above this point will still be valid, but not
    // below.
    virtual bool invalidate(CFilerEntry *startingAt);

	// This returns true if it belongs to us and it's still valid.
	bool isValid(CFilerEntry *entry);

    // If this class has any unwritten data, calling this will ensure
    // it's been written.  Returns true if all went well.
    virtual bool sync();

    // Call this method to get the root handle.
    CFilerEntry* root();
	// Returns true if this is the root entry.
	bool isRoot(CFilerEntry *entry) {return entry == root_; };

    // These two methods can be used to iterate over the objects at a
    // certain level of the filespace.
    virtual bool findFile(CFilerEntry *pParent);
    virtual CFilerEntry *findFileNext();
    
    // Set the current directory.  This current directory is not the real
    // system directory, just a tool for toString.
    // Returns false if not valid.
    virtual bool currentDir(CFilerEntry *cd);
    // Get the current directory.  Returns 0 if not set or not valid.
    virtual CFilerEntry *currentDir();
    
    // General entry manipulation.  Finds are invalid after all of these.
    bool rm(CFilerEntry *pVictim);
    bool rm(CFilerEntryList *pVictims);
    bool rename(CFilerEntry *pEntry, const CString &sNewName);

    // TODO: Put in a callback for notification of overwrites.
    bool cp(bool removeAfterCopy, CFiler *fromFiler, CFilerEntry *from,
            CFilerEntry *to);
    bool cp(bool removeAfterCopy, CFiler *fromFiler, CFilerEntryList *fromList,
            CFilerEntry *to);
    bool cp(bool removeAfterCopy, HDROP hDrop, CFiler * fromFiler, 
			CFilerEntry *to);

	// Suggests a name for a new directory.  The name of the new directory
	// will be chosen not to clash with any existing directory.
	virtual CString suggestFolderName(CFilerEntry *parent)=0;
    // Creates a new directory, and returns the CFilerEntry*.
	virtual CFilerEntry *newFolder(CFilerEntry *pParent, CString sName)=0;

	// FIXME: Rewrite these comments.
    // Not sure about how to write the methods for these.  They will
    // return false or 0 when called on an instance of CFilerPC.  
	// The arguments
    // are the full names of PC files.
	// Note, invalidate() should be called, because entries may
	// have been created...
    virtual CFilerEntry *createArchive(CFiler *toFiler, CFilerEntry *toParent,
									   CString fname)=0;
    virtual bool restoreFromArchive(CFiler *fromFiler, CFilerEntry *fromfe)=0;
    
    // Convert strings to and from CFilerEntries.
    // fromString converts a string.  It can be absolute, or relative to
    // the current directory.
    // 0 is returned if it is not possible to convert it.
    virtual CFilerEntry *fromString(CString sName);
    // toString converts to a CString.  If the conversion is not
    // possible, an empty string is returned.  If bAbsolute is false,
    // the string is relative to the current directory if possible.
	// Directories are not followed by the appropriate separator for
	// the filespace.
    virtual CString toString(CFilerEntry *pEntry, bool bAbsolute=true);

    // Get the icon indexes associated with this entry.  A return of false
    // means that the icon could not be retrieved, or is not supported
    // in this filespace.
    virtual bool getIcons(CFilerEntry *pEntry, int &icon, int &selIcon);

	// Find the icon number of a directory.  Needed when creating a new directory.
	virtual int getDirIcon()=0;

    // TODO: Put in bool feature test methods.

    virtual bool hasDateTime()=0;
    virtual bool getDateTime(CFilerEntry *pEntry, SYSTEMTIME &st)=0;

    virtual bool hasCRC()=0;
    virtual bool getCRC(CFilerEntry *pEntry, unsigned int &nCRC)=0;

    // This returns false except on the HP49G, where it will call
    // a separate program.
    virtual bool hasFlash();
    virtual bool updateFlash(const CString &sFlashFilename);
    
    // any others?  Permission setting/getting?

	// This method determines if the entry has subdirectories or not.
	virtual bool hasSubDir(CFilerEntry *pEntry);
	// This method says whether findFile has been called on this entry.
	// Always returns true if called with a file.  Returns false if
	// called on a device like a floppy which didn't read before.
	bool hasBeenRead(CFilerEntry *entry);

	// Remainder need to be overridden in a derived class.
	// This is the char used as a separator between filespace entries
	// in a pathname for this filesystem, eg, '\\' for the PC filespace.
	virtual char separator()=0;

	// Return the name of the root entry, eg, "My Computer" for FilerPC.
	virtual const CString &rootName()=0;

	// If Windows sees a network drive mounted on a Unix box, and that
	// Unix box contains a symbolic link which loops, a recursive
	// program will never stop.  To avoid this, the filer can tell you
	// how many levels deep a particular CFilerEntry is.  You can use
	// this to escape from loops.  Passing the root will return 0,
	// passing the top-level entries (such as drive letters) will return
	// 1, etc.
	unsigned int level(CFilerEntry *entry);

protected:
    CFilerEntry *newCFilerEntry(const CString &name, bool dir,
								unsigned int size);
	void deleteCFilerEntry(CFilerEntry *victim);

    // Transfer a bunch of data from buffers to files, or v-v.
	// TODO: Error codes?
	virtual bool transfer(IORequest *IOReq)=0;

    // This method takes the name from fe, and looks for a corresponding fe
    // in dir.  If it exists, that one is returned, otherwise a new one is
    // created.  The fe that's passed in doesn't have to be in the same dir
    // as "dir" - it will look for the fe's name in the dir.  If the
	// directory has not been read yet, or something else nasty happens,
	// it returns 0.
    CFilerEntry* findOrCreate(CFilerEntry *dir, CFilerEntry *fe,
							  bool *existed = 0);

	void cleanUp(CStringArray *sa, CFilerDir *dir);

	// Used by hasSubDirs to find out whether it should force a read
	// of the contents of a subdir, or just pretend that there are.
	// This should be set to true if finding out about subdirs is an
	// expensive operation. 
	bool lieAboutSubDirs_;

	// Removes the CFilerEntries from the entry pool, but does NOT delete
	// the files it refers to.
	void rmEntries(CFilerEntryList * pVictims);

    // All these methods need to be overridden in a derived class.

    // Find the icons associated with a particular entry, and put them
	// into the CFilerEntry.  Should be overridden for each class.
	virtual void calcIcons(CFilerEntry *entry)=0;

    // Read entries from the directory.  This method works recursively.
    virtual bool readDir(CFilerDir *parent, bool reRead = false)=0;

	virtual bool _rm(CFilerEntryList *pVictims)=0;
	virtual bool _rename(CFilerEntry *pEntry, const CString &sNewName)=0;
    virtual bool _cp(bool removeAfterCopy, CFiler *fromFiler,
                     CFilerEntryList *fromList, CFilerEntry *to)=0;

	// This method is called by cp when doing cross-filespace copies, so
	// that ridiculous transfers aren't attempted.  On a PC, this returns
	// 0, which means no limit.
	virtual unsigned int biggestObject(CFilerEntry *where)=0;

	// Derived classes should implement this function to say whether a
	// particular name is a valid filename in that filespace.
    virtual bool validFilename(const CString &name)=0;

	// Returns true or false depending on whether the user wants to
	// overwrite a file.
	virtual bool overwrite(CFilerEntry *parent, const CString &name)=0;

#ifdef _DEBUG
	// This method can be used while debugging to show the children of
	// a CFilerDir.  Example, put "showDir(dir)" in your watch window.
    static CString showString(CString s);
    static void showDir(CFilerDir *fd);
#endif

private:
	// Current position when doing a findFile().
    CFilerDir *ff_;
	CFilerDir *root_;
    POSITION findPos_;
    bool finishedSearch_;

	// Current directory pointer.
    CFilerEntry *currentDir_;

    CFilerEntryList entryPool_;

    // Methods to delete CFilerEntries from the pool.
    // After this, all affected CFilerEntry*s will be invalid.
	void deleteEntryPool();

	// Deletes all the allocated buffers we need for a file transfer.
	void deleteIOBuffers(IORequest * IOReq);

	// This method allows the user to edit a filename until it's right.
	// returns false if the user wants to skip this file.
	// TODO: Make it so the user can skip all remaining files.
    typedef enum {OK, SKIP, CANCEL} editReturn;
	editReturn editFilename(CString &name);
};

#endif // !defined(AFX_FILER_H__F55E4294_1682_11D3_98A5_0060B0A2563C__INCLUDED_)
