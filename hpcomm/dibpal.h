/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998 by Jörg König
// All rights reserved
//
// When used within HPComm, this code is licensed under the GPL, by
// written arrangement with the author.
//
// See http://www.codeguru.com/dialog/BmpDlg.shtml for details.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc., and
// I'll try to keep a version up to date.  I can be reached as follows:
//    J.Koenig@adg.de                 (company site)
//    Joerg.Koenig@rhein-neckar.de    (private site)
/////////////////////////////////////////////////////////////////////////////


#ifndef DIBPAL_h
#define DIBPAL_h

class CBmpPalette : public CPalette
{
public:
    CBmpPalette( CDIBitmap* pBmp );
};

#endif	// DIBPAL_h