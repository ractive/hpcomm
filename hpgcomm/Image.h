/*************************************************************************
  HPGComm
**************************************************************************

Copyright (C) January 2001 by HP and Jean-Pierre Bergamin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
***************************************************************************/


// Image.h: interface for the CImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGE_H__7B01B634_27D4_11D3_81A7_0000B4A265C3__INCLUDED_)
#define AFX_IMAGE_H__7B01B634_27D4_11D3_81A7_0000B4A265C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CImage  
{	
public:
	typedef struct {
		LONG height;	// Height of the image
		LONG width;		// Width of the image
		WORD planes;	// GROB planes
		int colors;	// Number of colors in the image.
		DWORD size;		// Size of the image
		LONG bytesPerLine;	// How many bytes are in one BMP scan line with valid data
		LONG fourBytes;		// How many bytes are in one BMP scan line (DWORD alignement)
	} tagImageInfo; 

	CImage();
	virtual ~CImage();

    int GetBitCount();
	BYTE * GetBits();
	CSize GetSize();
	bool isGrayscale();
	bool IsBitSet(BYTE val, int nBit);

	bool Alloc(DWORD size);
    void Free();
    void FillHeaderStruct(BITMAPINFOHEADER &bmiHeader, tagImageInfo * pInfo = 0);
	void CreateGrayMap(int nColors, BYTE *& cursor);
	HBITMAP CreateBitmapHandle(BYTE * pIm = 0 );
	HBITMAP GetHandle();
	
    bool LoadScreen(unsigned char * screendata);
	BOOL LoadBMP( LPCTSTR sBMPFile);
	BOOL LoadGROB(LPCTSTR sGROBFile, int nPlanes = 0);
	BOOL SaveGROB(CString * pFileName);
	BOOL SaveBMP(CString * pFileName);
    BOOL Update(HBITMAP hBM);
    bool Border(bool bBlack, bool bThin, bool bAdd);
	void CopyToClipboard( const CWnd *pWnd);
	void Reset();
	void Magnify(BOOL bMagnify = TRUE);
	void Rotate();
	void InvertBitmap();
	void MirrorBitmap(BOOL bLateral);
	bool ConvertToGray();
	void ConvertToMono();
	void Resize(CSize size);

	HBITMAP origSize;
	bool bBorder;
	bool bBorderThin;
	bool bBorderBlack;
	HBITMAP origBitmap;
	BYTE * phDIB;

private:
	BYTE TurnByte(BYTE byte);
	void SetBit(int nBit, BYTE &byte, BOOL bFalse);
	HANDLE DDBToDIB( CBitmap& bitmap, DWORD dwCompression);
	BYTE TurnNibble(BYTE value);

    tagImageInfo imageInfo;
	HGLOBAL hDIB;
	//BITMAPINFO * bmInfo;
	//BITMAPFILEHEADER bmFileHeader;
	CBitmap bitmap;
	int planes;

protected:
	void ConvertGROBToBMP(BYTE *&bmp, BYTE * grob);
};

#endif // !defined(AFX_IMAGE_H__7B01B634_27D4_11D3_81A7_0000B4A265C3__INCLUDED_)
