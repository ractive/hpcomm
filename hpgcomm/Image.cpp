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


// Image.cpp: implementation of the CImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "afxole.h"
#include "Image.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define BORDERTHICK 6
#define BORDERTHIN 2

CImage::CImage()
{
	hDIB = 0;
	bBorder = false;
	bBorderThin = true;
	bBorderBlack = true;
}

CImage::~CImage()
{
	bitmap.DeleteObject();
	DeleteObject(origBitmap);
	DeleteObject(origSize);
	Free();
}

BOOL CImage::LoadBMP( LPCTSTR sBMPFile)
{
	CFile file;
	if( !file.Open( sBMPFile, CFile::modeRead) )
		return FALSE;

	BITMAPFILEHEADER bmfHeader;

	// Read file header
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		return FALSE;

	// File type should be 'BM'
	if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
		return FALSE;

	// Get length of the remainder of the file and allocate memory
	DWORD nPackedDIBLen = file.GetLength() - sizeof(BITMAPFILEHEADER);
	if (!Alloc(nPackedDIBLen))
		return FALSE;

	if (file.Read(phDIB, nPackedDIBLen) != nPackedDIBLen )
	{
		Free();
		return FALSE;
	}
	
	// This is the original bitmap we load.
	// Keep this handle to make modifications starting from this one.
	origBitmap = CreateBitmapHandle();
	origSize = origBitmap;
	BOOL res = Update(origBitmap);

	// Fill the imageInfo struct;
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)phDIB;

	imageInfo.colors = lpbi->biClrUsed;
	imageInfo.height = lpbi->biHeight;
	imageInfo.width = lpbi->biWidth;

	if (imageInfo.width % 8)
		imageInfo.bytesPerLine = (imageInfo.width / 8 + 1);
	else
		imageInfo.bytesPerLine = imageInfo.width / 8; 

	imageInfo.fourBytes = (((imageInfo.bytesPerLine - 1) / 4) + 1) * 4;
	imageInfo.size = imageInfo.fourBytes * imageInfo.height * (imageInfo.colors == 2 ? 1 : 4);

	return res;
}

HBITMAP CImage::GetHandle()
{
	return (HBITMAP)bitmap.GetSafeHandle();
}

void CImage::CopyToClipboard( const CWnd *pWnd)
{
	// When the content of the clipboard is updated, Windows calls the
	// standard release procedures on the object which is currently in the clipboard.
	// If there's a bitmap in it DeleteObject() is called. Cause of this,
	// we have to make a copy of our bitmap so that the one that is show won't be deleted.

	CDC sourceDC, destDC;

	// Create a memory DC compatible with the display
	sourceDC.CreateCompatibleDC( NULL );
	destDC.CreateCompatibleDC( NULL );

	// Get logical coordinates
	BITMAP bm;
	::GetObject( (HBITMAP)bitmap, sizeof( bm ), &bm );

	// Create a bitmap to hold the result
	HBITMAP hbmResult = ::CreateCompatibleBitmap(CClientDC(NULL), bm.bmWidth, bm.bmHeight);

	// Select bitmaps into the DCs
	HBITMAP hbmOldSource = (HBITMAP)::SelectObject( sourceDC.m_hDC, (HBITMAP)bitmap);
	HBITMAP hbmOldDest = (HBITMAP)::SelectObject( destDC.m_hDC, hbmResult );

	// Copy the content to the new bitmap.
	destDC.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &sourceDC, 0, 0, SRCCOPY);

	// Reselect the old bitmaps
	::SelectObject( sourceDC.m_hDC, hbmOldSource );
	::SelectObject( destDC.m_hDC, hbmOldDest );

	// Put the bitmap data onto the clipboard
	if (!::OpenClipboard(pWnd->GetSafeHwnd())) {
		MessageBox(pWnd->GetSafeHwnd(), "The clipboard could not be opened!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	if (!::EmptyClipboard()) {
		MessageBox(pWnd->GetSafeHwnd(), "The clipboard could not be emptied!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	if (!::SetClipboardData(CF_BITMAP, hbmResult)) {
		MessageBox(pWnd->GetSafeHwnd(), "The picture could not be copied to the clipboard!", "Error", MB_OK | MB_ICONEXCLAMATION);	
		// Dont't return. We have to (try to) close the clipboard.
	}
	if (!::CloseClipboard()) {
		MessageBox(pWnd->GetSafeHwnd(), "The clipboard could not be closed!", "Error", MB_OK | MB_ICONEXCLAMATION);
	}
}

CSize CImage::GetSize()
{
	BITMAP bmp;
	bitmap.GetBitmap(&bmp);
	return CSize(bmp.bmWidth, bmp.bmHeight);

}

BOOL CImage::LoadGROB(LPCTSTR sGROBFile, int nPlanes /* = 0 */)
{
	CWaitCursor wait;
	CFile file;
	if( !file.Open( sGROBFile, CFile::modeRead) )
		return FALSE;

	// Read file header
	char hp[7];
	BYTE ch1, ch2, head[8], nib[3];
	file.Read(hp, 7);
	file.Read(&ch1, 1);
	file.Read(&ch1, 1);
	file.Read(&ch2, 1);
	if (hp != "HPHP48-" && ch1 != 0x1E && ch2 != 0x2B) {
		MessageBox(NULL, "No valid GROB file", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	file.Read(head, 8);
	nib[2] = head[0] >> 4;
	nib[2] = nib[2] + ((head[1] & 0x0F) << 4);
	nib[1] = head[1] >> 4;
	nib[1] = nib[1] + ((head[2] & 0x0F) << 4);
	nib[0] = head[2] >> 4;
	imageInfo.size = (nib[2] + (nib[1] << 8) + (nib[0] << 16)) - 15;

	nib[2] = head[3] & 0x0F;
	nib[2] = nib[2] + (head[3] & 0xF0);
	nib[1] = head[4] & 0x0F;
	nib[1] = nib[1] + (head[4] & 0xF0);
	nib[0] = head[5] & 0x0F;
	imageInfo.height = nib[2] + (nib[1] << 8) + (nib[0] << 16);

	nib[2] = head[5] >> 4;
	nib[2] = nib[2] + ((head[6] & 0x0F) << 4);
	nib[1] = head[6] >> 4;
	nib[1] = nib[1] + ((head[7] & 0x0F) << 4);
	nib[0] = head[7] >> 4;
	imageInfo.width = nib[2] + (nib[1] << 8) + (nib[0] << 16);
	unsigned int nibs = imageInfo.width / 4;

	if (imageInfo.width % 4)
		nibs++;

	if (nibs % 2)
		nibs++;
	
	if (nPlanes != 0) {
		planes = nPlanes;
		// More planes will be supported maybe later.
		if (planes > 2) {
			planes = 2;
		}
		if (planes == 2) {
			if (nibs * imageInfo.height == imageInfo.size ) {
				// This is a old-style grayscale GROB where the height
				// is the sum of all planes' heights.
				imageInfo.height = imageInfo.height / planes;
			}
		
		}
	}
	else {
		if (nibs * imageInfo.height != imageInfo.size / 2) {
			// We have an unknown size compared to the dimesions.
			// Display it as a B/W image.
			planes = 1;
		}
		else {
			// It's a Vger grayscale GROB.
			planes = 2;
		}
	}
	
	imageInfo.colors = planes > 1 ? 4 : 2;

	if (imageInfo.width % 8)
		imageInfo.bytesPerLine = (imageInfo.width / 8 + 1);
	else
		imageInfo.bytesPerLine = imageInfo.width / 8; 

	imageInfo.fourBytes = (((imageInfo.bytesPerLine - 1) / 4) + 1) * 4;

	BITMAPINFOHEADER bmiHeader;
	FillHeaderStruct(bmiHeader);

	// Alloc the memory for the image.
	if (!Alloc(imageInfo.size + sizeof(BITMAPINFOHEADER) + imageInfo.colors * sizeof(RGBQUAD)))
		return FALSE;
	memcpy(phDIB, &bmiHeader, sizeof(BITMAPINFOHEADER));
	BYTE * cursor = phDIB + sizeof(BITMAPINFOHEADER);
	CreateGrayMap(bmiHeader.biClrUsed, cursor);

	BYTE *Image = new BYTE[imageInfo.size / 2];
	file.Read(Image, imageInfo.size / 2);

	ConvertGROBToBMP(cursor, Image);

	delete [] Image;

#ifdef _DEBUG

	// If bmiHeader.biClrUsed is zero we have to infer the number
	// of colors from the number of bits used to specify it.
	int nColors = bmiHeader.biClrUsed ? bmiHeader.biClrUsed : 
						1 << bmiHeader.biBitCount;



	BITMAPINFO &bmInfo = *(LPBITMAPINFO)phDIB;
	BITMAPFILEHEADER bmFileHeader;
	bmFileHeader.bfOffBits = (DWORD) (sizeof(bmFileHeader) + bmInfo.bmiHeader.biSize +
						nColors * sizeof(RGBQUAD));
	bmFileHeader.bfReserved1 = 0;
	bmFileHeader.bfReserved2 = 0;
	bmFileHeader.bfSize = sizeof(bmFileHeader);
	bmFileHeader.bfType = 0x4D42;

	CFile test;
	test.Open("C:\\testbmp.bmp", CFile::modeCreate | CFile::modeWrite | CFile::modeRead);
	test.Write(&bmFileHeader, sizeof(bmFileHeader));
	test.Write(&bmInfo, imageInfo.size + sizeof(BITMAPINFO) + nColors * sizeof(RGBQUAD));
	test.Close();
#endif


	origBitmap = CreateBitmapHandle();
	origSize = origBitmap;
	
	return Update(origBitmap);
}

BYTE CImage::TurnNibble(BYTE value)
{
	switch (value) {
	case 1: return 8;
	case 2: return 4;
	case 3: return 12;
	case 4: return 2;
	case 5: return 10;
	case 7: return 14;
	case 8: return 1;
	case 10: return 5;
	case 11: return 13;
	case 12: return 3;
	case 13: return 11;
	case 14: return 7;
	default: return value;
	}

}


// This code is from CodeGuru:
// See http://www.codeguru.com/bitmap/bitmap_to_file.shtml

BOOL CImage::SaveBMP(CString *pFileName)
{

	// Get the handle to the DIB.
	// The memory is allocated in DDBToDIB. But we have to free it at the end.
	HANDLE hDIB = DDBToDIB(bitmap, 0);
	
	
	BITMAPFILEHEADER	hdr;
	LPBITMAPINFOHEADER	lpbi;

	if (!hDIB)
		return FALSE;

	CFile file;
	if( !file.Open(*pFileName, CFile::modeWrite|CFile::modeCreate) )
		return FALSE;

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	int nColors = 1 << lpbi->biBitCount;
	// Without this line, it's not possible to save BMP files.
	if (lpbi->biBitCount > 8)
		nColors = 0;

	// Fill in the fields of the file header 
	hdr.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
	hdr.bfSize		= GlobalSize (hDIB) + sizeof( hdr );
	hdr.bfReserved1 	= 0;
	hdr.bfReserved2 	= 0;
	hdr.bfOffBits		= (DWORD) (sizeof( hdr ) + lpbi->biSize +
						nColors * sizeof(RGBQUAD));

	// Write the file header 
	file.Write( &hdr, sizeof(hdr) );

	// Write the DIB header and the bits 
	file.Write( lpbi, GlobalSize(hDIB) );
	
	// Free the memory allocated in DDBToDIB
	GlobalFree(hDIB);

	return TRUE;
}

BOOL CImage::SaveGROB(CString *pFileName)
{
	// Get the handle to the DIB.
	// The memory is allocated in DDBToDIB. But we have to free it at the end.
	HANDLE hDIB = DDBToDIB(bitmap, 0);
	
	
	LPBITMAPINFOHEADER	lpbi;

	if (!hDIB)
		return FALSE;

	CFile file;
	if( !file.Open(*pFileName, CFile::modeWrite | CFile::modeCreate) )
		return FALSE;

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	int nColors = 1 << lpbi->biBitCount;

	
	// Free the memory allocated in DDBToDIB
	GlobalFree(hDIB);

	return TRUE;
}

void CImage::MirrorBitmap(BOOL bLateral)
{
	HBITMAP hBitmap = (HBITMAP)bitmap;
	// Create a memory DC compatible with the display
	CDC sourceDC, destDC;
	sourceDC.CreateCompatibleDC( NULL );
	destDC.CreateCompatibleDC( NULL );

	// Get logical coordinates
	BITMAP bm;
	::GetObject( hBitmap, sizeof( bm ), &bm );

	// Create a bitmap to hold the result
	HBITMAP hbmResult = ::CreateCompatibleBitmap(CClientDC(NULL), 
						bm.bmWidth, bm.bmHeight);

	// Select bitmaps into the DCs
	HBITMAP hbmOldSource = (HBITMAP)::SelectObject( sourceDC.m_hDC, hBitmap );
	HBITMAP hbmOldDest = (HBITMAP)::SelectObject( destDC.m_hDC, hbmResult );
	
	if( bLateral )
		destDC.StretchBlt( 0, 0, bm.bmWidth, bm.bmHeight, &sourceDC, 
				bm.bmWidth-1, 0, -bm.bmWidth, bm.bmHeight, SRCCOPY );
	else
		destDC.StretchBlt( 0, 0, bm.bmWidth, bm.bmHeight, &sourceDC, 
				0, bm.bmHeight-1, bm.bmWidth, -bm.bmHeight, SRCCOPY );

	// Reselect the old bitmaps
	::SelectObject( sourceDC.m_hDC, hbmOldSource );
	::SelectObject( destDC.m_hDC, hbmOldDest );

	Update(hbmResult);
	origSize = hbmResult;
}

void CImage::InvertBitmap()
{
	CDC sourceDC, destDC;

	// Create a memory DC compatible with the display
	sourceDC.CreateCompatibleDC( NULL );
	destDC.CreateCompatibleDC( NULL );

	// Get logical coordinates
	BITMAP bm;
	::GetObject( (HBITMAP)bitmap, sizeof( bm ), &bm );

	// Create a bitmap to hold the result
	HBITMAP hbmResult = ::CreateCompatibleBitmap(CClientDC(NULL), bm.bmWidth, bm.bmHeight);

	// Select bitmaps into the DCs
	HBITMAP hbmOldSource = (HBITMAP)::SelectObject( sourceDC.m_hDC, (HBITMAP)bitmap);
	HBITMAP hbmOldDest = (HBITMAP)::SelectObject( destDC.m_hDC, hbmResult );

	// Draw the background of the destination DC black, so that the XOR operation
	// of BitBlt will take effect.
	HBRUSH hbrBack = CreateSolidBrush(0x00FFFFFF);
	HBRUSH hbrOld = (HBRUSH)::SelectObject( destDC.m_hDC, hbrBack );
	destDC.PatBlt( 0, 0, bm.bmWidth, bm.bmHeight, PATCOPY );
	::DeleteObject( ::SelectObject( destDC.m_hDC, hbrOld ) );

	// Do the inversion. SRCINVERT simply XOR s bit for bit
	destDC.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &sourceDC, 0, 0, SRCINVERT);

	// Reselect the old bitmaps
	::SelectObject( sourceDC.m_hDC, hbmOldSource );
	::SelectObject( destDC.m_hDC, hbmOldDest );

	Update(hbmResult);
	origSize = hbmResult;
}

void CImage::Rotate()
{
	CDC sourceDC, destDC;
	sourceDC.CreateCompatibleDC( NULL );
	destDC.CreateCompatibleDC( NULL );

	// Get logical coordinates
	BITMAP bm;
	::GetObject( (HBITMAP)bitmap, sizeof( bm ), &bm );

	// First we determine the dimensions of the new bitmap.
	// width = height and heigth = width

	int w = bm.bmHeight;
	int h = bm.bmWidth;


	// Create a bitmap to hold the result
	HBITMAP hbmResult = ::CreateCompatibleBitmap(CClientDC(NULL), w, h);

	HBITMAP hbmOldSource = (HBITMAP)::SelectObject( sourceDC.m_hDC, (HBITMAP)bitmap );
	HBITMAP hbmOldDest = (HBITMAP)::SelectObject( destDC.m_hDC, hbmResult );

	// Draw the background color before we change mapping mode
	HBRUSH hbrBack = CreateSolidBrush(0x00FFFFFF);
	HBRUSH hbrOld = (HBRUSH)::SelectObject( destDC.m_hDC, hbrBack );
	destDC.PatBlt( 0, 0, w, h, PATCOPY );
	::DeleteObject( ::SelectObject( destDC.m_hDC, hbrOld ) );


	// Now do the actual rotating - a pixel at a time
	// Computing the destination point for each source point
	// will leave a few pixels that do not get covered
	// So we use a reverse transform - e.i. compute the source point
	// for each destination point

	for( int y = 0; y < bm.bmHeight; y++ )
	{
		for( int x = 0; x < bm.bmWidth; x++ )
		{
			destDC.SetPixel(y, h - x - 1, sourceDC.GetPixel(x,y));
		}
	}

	// Restore DCs
	::SelectObject( sourceDC.m_hDC, hbmOldSource );
	::SelectObject( destDC.m_hDC, hbmOldDest );

	Update(hbmResult);
	origSize = hbmResult;
}

void CImage::Reset()
{
	//DeleteObject(origSize);
	origSize = origBitmap;
	Update(origBitmap);
}

HANDLE CImage::DDBToDIB(CBitmap &bitmap, DWORD dwCompression)
{
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;

	HANDLE res = bitmap.GetSafeHandle();
	VERIFY(res);

	// The function has no arg for bitfields
	if( dwCompression == BI_BITFIELDS )
		return NULL;

	// Get bitmap information
	bitmap.GetObject(sizeof(bm),(LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize		= sizeof(BITMAPINFOHEADER);
	bi.biWidth		= bm.bmWidth;
	bi.biHeight 		= bm.bmHeight;
	bi.biPlanes 		= 1;
	bi.biBitCount		= bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression	= dwCompression;
	bi.biSizeImage		= 0;
	bi.biXPelsPerMeter	= 0;
	bi.biYPelsPerMeter	= 0;
	bi.biClrUsed		= 0;
	bi.biClrImportant	= 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if( nColors > 256 ) 
		nColors = 0;
	dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = GetDC(NULL);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
			(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0){
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) 
						* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else {
		GlobalFree(hDIB);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, (HBITMAP)bitmap.GetSafeHandle(),
				0L,				// Start scan line
				(DWORD)bi.biHeight,		// # of scan lines
				(LPBYTE)lpbi 			// address for bitmap bits
				+ (bi.biSize + nColors * sizeof(RGBQUAD)),
				(LPBITMAPINFO)lpbi,		// address of bitmapinfo
				(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

	if( !bGotBits )
	{
		GlobalFree(hDIB);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	ReleaseDC(NULL,hDC);
	return hDIB;

}

void CImage::SetBit(int nBit, BYTE &byte, BOOL bSet)
{
	BYTE mask = 0;
	switch(nBit % 4) {
	case 0: mask = 0x1; break;
	case 1: mask = 0x2; break;
	case 2: mask = 0x4; break;
	case 3: mask = 0xA; break;
	}
	if (nBit > 3) {
		mask = (mask << 4);
	}
	if (bSet) {
		byte |= mask;
	}
	else {
		byte &= ~mask;
	}

}

BYTE CImage::TurnByte(BYTE byte)
{
	return TurnNibble((byte >> 4) & 0x0F) | (TurnNibble(byte & 0x0F) << 4);
}

bool CImage::LoadScreen(unsigned char *screendata)
{
	if (screendata == 0)
		return false;

	const int rows = 64;
	const int columns = 131;

	if (*screendata == 27 || *screendata == 'W') {
		imageInfo.colors = 2;
	}
	else {
		imageInfo.colors = 4; 
	}

	imageInfo.height = rows;
	imageInfo.width = columns;

	// These "magic numbers" result form the fact, that the width of a screendump
	// is always 131.
	// lBytesPerLine = ceil(131 / 8)
	imageInfo.bytesPerLine = 17;

	// Bitmaps are DWORD aligned.
	imageInfo.fourBytes = 20;

	imageInfo.size = imageInfo.fourBytes * imageInfo.height * (imageInfo.colors == 2 ? 1 : 4);

	BITMAPINFOHEADER bmiHeader;
	FillHeaderStruct(bmiHeader);

	if (!Alloc(imageInfo.size + sizeof(BITMAPINFOHEADER) + imageInfo.colors * sizeof(RGBQUAD)))
		return FALSE;

	memcpy(phDIB, &bmiHeader, sizeof(BITMAPINFOHEADER));
	BYTE * cursor = phDIB + sizeof(BITMAPINFOHEADER);
	CreateGrayMap(imageInfo.colors, cursor);

	if (*screendata == 27) {

		// HP48 screendata format.

		// The data is structured like this:
		// 3 bytes escape character stuff for the printer (that we can ignore)
		// Then 131 bytes of data. These bytes contains infomation about the
		// first 8 pixel rows, so that the first byte contains the info about (0,0) till (0, 7).
		// 4 bytes escape char.... 131 bytes info.....a.s.o.
		
		screendata += 2; // Skip the escape sequence.

		for (int block = 0; block < 8; block++) {
			for (int col = 0; col < columns; col++) {
				for (int nbit = 0; nbit < 8; nbit++) {
					SetBit(7 - (col % 8), cursor[col / 8 + imageInfo.size - (block * 8 + 1) * imageInfo.fourBytes - nbit * imageInfo.fourBytes], !IsBitSet(*screendata, nbit));
				}
				screendata++;
			}
			screendata += 4;  // Another escape sequence.
		}
	}

	else {

		// HP49 screendata format (like a 131 * 64 GROB)

		if (*screendata++ == 'W')
			planes = 1;
		else 
			planes = 2;

		ConvertGROBToBMP(cursor, screendata);
	}

	origBitmap = CreateBitmapHandle();
	origSize = origBitmap;	
	Update(origBitmap);

	return TRUE;


	return true;
}

bool CImage::IsBitSet(BYTE val, int nBit)
{
	return ((val >> nBit) & 0x1) == 1;

}

bool CImage::isGrayscale()
{
	return (planes == 2);
}

void CImage::Magnify(BOOL bMagnify /* = TRUE*/)
{
	// Get the size of the bitmap and magnify or shrink it.
	CSize size = GetSize();

	if (bMagnify) {
		size.cx = size.cx * 2;
		size.cy = size.cy * 2;
	}
	else {
		size.cx = size.cx / 2;
		size.cy = size.cy / 2;
	}
	
	// Do the resize
	Resize(size);
}

void CImage::Resize(CSize size)
{
	// First check if the image becomes bigger than the screen resolution.
	int xScreen = GetSystemMetrics(SM_CXSCREEN);
	int yScreen = GetSystemMetrics(SM_CYSCREEN);
	
	if ((size.cx > xScreen) || (size.cy > yScreen)) {
		// Yes, it's bigger
		// Ask the user if he wants to resize it so that it fits in...
		if (MessageBox(NULL, "The image is larger than the screen.\n\nDo you want to size it to the maximum visible size?", "Error", MB_YESNO | MB_ICONQUESTION) == IDNO) {
			return;
		}
		size.cx = xScreen - 25;
		size.cy = yScreen - GetSystemMetrics(SM_CYMENU) - 5;
	}

	// Create a memory DC compatible with the display
	CDC sourceDC, destDC;
	sourceDC.CreateCompatibleDC( NULL );
	destDC.CreateCompatibleDC( NULL );

	// Get logical coordinates
	BITMAP bm;
	::GetObject(origSize, sizeof(bm), &bm );

	// Create a bitmap to hold the result
	HBITMAP hbmResult = ::CreateCompatibleBitmap(CClientDC(NULL), size.cx, size.cy);

	// Select bitmaps into the DCs
	HBITMAP hbmOldSource = (HBITMAP)::SelectObject( sourceDC.m_hDC, origSize);
	HBITMAP hbmOldDest = (HBITMAP)::SelectObject( destDC.m_hDC, hbmResult );
	destDC.SetStretchBltMode(HALFTONE);
	SetBrushOrgEx(destDC, 0, 0, NULL);
	destDC.StretchBlt( 0, 0, size.cx, size.cy, &sourceDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY );

	// Reselect the old bitmaps
	::SelectObject( sourceDC.m_hDC, hbmOldSource );
	::SelectObject( destDC.m_hDC, hbmOldDest );

	Update(hbmResult);
}

void CImage::ConvertToMono()
{
	HBITMAP hBitmap = (HBITMAP)bitmap;
	// Create a memory DC compatible with the display
	CDC sourceDC, destDC;
	sourceDC.CreateCompatibleDC( NULL );
	destDC.CreateCompatibleDC( NULL );

	// Get logical coordinates
	BITMAP bm;
	::GetObject( hBitmap, sizeof( bm ), &bm );

	// Create a bitmap to hold the result
	HBITMAP hbmResult = ::CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

	// Select bitmaps into the DCs
	HBITMAP hbmOldSource = (HBITMAP)::SelectObject( sourceDC.m_hDC, hBitmap );
	HBITMAP hbmOldDest = (HBITMAP)::SelectObject( destDC.m_hDC, hbmResult );
	destDC.SetStretchBltMode(HALFTONE);
	SetBrushOrgEx(destDC, 0, 0, NULL);
	BOOL b = destDC.StretchBlt( 0, 0, bm.bmWidth, bm.bmHeight, &sourceDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY );

	// Reselect the old bitmaps
	::SelectObject(sourceDC.m_hDC, hbmOldSource);
	::SelectObject(destDC.m_hDC, hbmOldDest);

	Update(hbmResult);
}

bool 
CImage::ConvertToGray()
{
	BITMAP bm;
	::GetObject(bitmap, sizeof(bm), &bm);
	LPBITMAPINFO lpbi = (LPBITMAPINFO)&bm;

	CSize size = GetSize();
	HBITMAP hGrayBM = CreateBitmap(size.cx, size.cy, 1, 4, NULL);
	BITMAPINFOHEADER bmiHeaderGray;
	tagImageInfo grayInfo;
	grayInfo = imageInfo;
	grayInfo.colors = 4;

	if (grayInfo.width % 8)
		grayInfo.bytesPerLine = (grayInfo.width / 2 + 1);
	else
		grayInfo.bytesPerLine = grayInfo.width / 2; 

	grayInfo.fourBytes = (((grayInfo.bytesPerLine - 1) / 4) + 1) * 4;

	FillHeaderStruct(bmiHeaderGray, &grayInfo);

	grayInfo.size = imageInfo.fourBytes * imageInfo.height * bmiHeaderGray.biBitCount;


	HGLOBAL hGray = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, grayInfo.size + sizeof(BITMAPINFOHEADER) + grayInfo.colors * sizeof(RGBQUAD));
	BYTE * pGray = (BYTE *)hGray;
	if (pGray == 0)
		return false;

	memcpy(pGray, &bmiHeaderGray, sizeof(bmiHeaderGray));

	BYTE * graybits = pGray + sizeof(BITMAPINFOHEADER);
	BYTE * cursor = graybits;
	CreateGrayMap(grayInfo.colors, cursor);

	BITMAPINFO &bmInfoGray = *(LPBITMAPINFO)pGray;
	
	BYTE * bits = GetBits();
	int nBitCount = GetBitCount();
	unsigned long imagesize = grayInfo.height * grayInfo.width / 2;

	int nGray1, nGray2;

	if (nBitCount == 8) {
		int nColorIndex1, nColorIndex2;
		unsigned long size = grayInfo.height * grayInfo.fourBytes;
		int widthcounter = 0;

		int lFourByte = (((grayInfo.width - 1) / 4) + 1) * 4;

		while (size--) {
			nColorIndex1 = *bits++;
			if (++widthcounter >= lpbi->bmiHeader.biWidth) {
				*cursor++ = ((nGray1 / 64) << 4) & 0xF0;
				bits += lFourByte - widthcounter;
				cursor += (grayInfo.fourBytes * 2 - widthcounter) / 2;
				widthcounter = 0;
				continue;
			}
			nColorIndex2 = *bits++;
			RGBQUAD rgb1 = lpbi->bmiColors[nColorIndex1];
			RGBQUAD rgb2 = lpbi->bmiColors[nColorIndex2];
			nGray1 = (rgb1.rgbRed * 30 + rgb1.rgbGreen * 59 + rgb1.rgbBlue * 11) / 100;
			nGray2 = (rgb2.rgbRed * 30 + rgb2.rgbGreen * 59 + rgb2.rgbBlue * 11) / 100;
			*cursor++ = ((((nGray1 / 64) << 4) & 0xF0) | (((nGray2 / 64)) & 0x0F));
			if (++widthcounter >= lpbi->bmiHeader.biWidth) {
				bits += lFourByte - widthcounter;
				cursor += (grayInfo.fourBytes * 2 - widthcounter) / 2;
				widthcounter = 0;
			}

		}
	}
	else if (nBitCount >= 24) {
		cursor--;
		int widthcounter = 0;
		while (imagesize--) {
			nGray1 = (bits[0] * 30 + bits[1] * 59 + bits[2] * 11) / 100;
			if (++widthcounter >= grayInfo.width) {
				*cursor++ = ((nGray1 / 64) << 4) & 0xF0;
				cursor += (grayInfo.fourBytes * 2 - widthcounter) / 2;
				widthcounter = 0;
				bits += 4;
				continue;
				
			}
			nGray2 = (bits[4] * 30 + bits[5] * 59 + bits[6] * 11) / 100;
			*cursor++ = ((((nGray1 / 64) << 4) & 0xF0) | (((nGray2 / 64)) & 0x0F));
			bits += 8;
			if (++widthcounter >= grayInfo.width) {
				cursor += (grayInfo.fourBytes * 2 - widthcounter) / 2;
				widthcounter = 0;
			}
		}
	}
	else if (nBitCount == 16) {
	/*
            WORD *dst=(WORD*)lpDIBBits;
            int Size=nWidth*nHeight;

            while ( Size-- )
            {
                BYTE b = (BYTE)((*dst)&(0x1F));
                BYTE g = (BYTE)(((*dst)>>5)&(0x1F));
                BYTE r = (BYTE)(((*dst)>>10)&(0x1F));

                int nGrey = Grey( r, g, b );

                *dst++ = ((WORD)(((BYTE)(nGrey)|((WORD)((BYTE)(nGrey))<<5))|(((DWORD)(BYTE)(nGrey))<<10)));
            }
      */  	
	}
#ifdef _DEBUG
	BITMAPINFO * bmi = (LPBITMAPINFO)graybits;

	BITMAPFILEHEADER BMPFileHead;
	BMPFileHead.bfOffBits = (DWORD) (sizeof(BMPFileHead) + bmInfoGray.bmiHeader.biSize +
						bmi->bmiHeader.biClrUsed * sizeof(RGBQUAD));
	BMPFileHead.bfReserved1 = 0;
	BMPFileHead.bfReserved2 = 0;
	BMPFileHead.bfSize = sizeof(BMPFileHead);
	BMPFileHead.bfType = 0x4D42;
	HANDLE hFile = CreateFile("C:\\testbmp.bmp", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	DWORD written;
	WriteFile(hFile, &BMPFileHead, sizeof(BMPFileHead), &written, NULL);
	WriteFile(hFile, bmi, bmInfoGray.bmiHeader.biSizeImage + sizeof(BITMAPINFO) + grayInfo.colors * sizeof(RGBQUAD), &written, NULL);
	CloseHandle(hFile);
#endif	

	return (Update(CreateBitmapHandle(pGray)) == TRUE);



  }

bool CImage::Alloc(DWORD size)
{
	if ((hDIB != 0) && (GlobalSize(hDIB) != size)) {
		// We alread have an allocated mem block.
		// Reallocate it to the new size.
		hDIB = GlobalReAlloc(hDIB, size, GMEM_ZEROINIT);
		// We must fill the mem with 0s to wipe out existing data.
		memset((BYTE *)hDIB, 0, size);
	}
	else {
		hDIB = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, size);
	}

	if (hDIB == NULL)
		return false;
	
	// In Win32 (relating to the MSDN) the handle value returned by
	// GlobalAlloc and GlobalReAlloc is the pointer to the mem block.
	phDIB = (BYTE *)hDIB;
	
	return (phDIB != NULL);
}

void CImage::Free()
{
	GlobalFree(hDIB);
	hDIB = 0;
}

HBITMAP CImage::CreateBitmapHandle(BYTE * pIm /* = 0 */)
{
	if (pIm == 0)
		pIm = phDIB;

	BITMAPINFOHEADER &bmiHeader = *(LPBITMAPINFOHEADER)pIm;
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)pIm;

	// If bmiHeader.biClrUsed is zero we have to infer the number
	// of colors from the number of bits used to specify it.
	int nColors = bmiHeader.biClrUsed ? bmiHeader.biClrUsed : 
						1 << bmiHeader.biBitCount;

	LPVOID lpDIBBits;
	if( bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + bmiHeader.biClrUsed) + 
			((bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);

	CClientDC dc(NULL);
	return CreateDIBitmap( dc.m_hDC,		// handle to device context 
				&bmiHeader,	// pointer to bitmap size and format data 
				CBM_INIT,	// initialization flag 
				lpDIBBits,	// pointer to initialization data 
				&bmInfo,	// pointer to bitmap color-format data 
				DIB_RGB_COLORS);		// color-data usage 

}

void CImage::CreateGrayMap(int nColors, BYTE *& cursor)
{
	for (int i = 0; i < nColors; i++) {
		*cursor++ = (int)(i * (0xFF / (nColors - 1)));
		*cursor++ = (int)(i * (0xFF / (nColors - 1)));
		*cursor++ = (int)(i * (0xFF / (nColors - 1)));
		*cursor++ = 0;
	}
}

void CImage::FillHeaderStruct(BITMAPINFOHEADER &bmiHeader, tagImageInfo * pInfo /* = 0 */)
{
	if (pInfo == 0)
		pInfo = &imageInfo;

	bmiHeader.biBitCount = pInfo->colors > 2 ? 4 : 1;
	bmiHeader.biClrImportant = pInfo->colors;
	bmiHeader.biClrUsed = pInfo->colors;
	bmiHeader.biCompression = 0;
	bmiHeader.biPlanes = 1;
	bmiHeader.biHeight = pInfo->height;
	bmiHeader.biWidth = pInfo->width;
	bmiHeader.biSize = sizeof(bmiHeader);
	bmiHeader.biSizeImage = pInfo->size;
	bmiHeader.biXPelsPerMeter = 0x0B12;
	bmiHeader.biYPelsPerMeter = 0x0B12;
}

BOOL CImage::Update(HBITMAP hBM)
{
	HGDIOBJ hOld;
	BOOL ret = TRUE;
	// Keep the origBitmap handle.
	// Delete the GDI object only if it is not the original one.
	if (((hOld = bitmap.Detach()) != origBitmap) && (hOld != origSize)) {
		if (hOld != 0)
			ret &= DeleteObject(hOld);
	}

	// Attach the new bimap handle to the bitmap object.
	ret &= bitmap.Attach(hBM);
	CSize size = GetSize();
	imageInfo.height = size.cy;
	imageInfo.width = size.cx;
	if (imageInfo.width % 8)
		imageInfo.bytesPerLine = (imageInfo.width / 8 + 1);
	else
		imageInfo.bytesPerLine = imageInfo.width / 8; 

	imageInfo.fourBytes = (((imageInfo.bytesPerLine - 1) / 4) + 1) * 4;

	return ret;
}

bool CImage::Border(bool bBlack, bool bThin, bool bAdd)
{
	if (bBorder == false && bAdd == false) {
		// There is nothing to do;
		return false;
	}

	if (bBlack == bBorderBlack && bThin == bBorderThin && bBorder == bAdd) {
		// Everything is the same as before.
		return false;
	}

	CDC sourceDC, destDC;

	// Create a memory DC compatible with the display
	sourceDC.CreateCompatibleDC( NULL );
	destDC.CreateCompatibleDC( NULL );

	// Get logical coordinates
	BITMAP bm;
	::GetObject( (HBITMAP)bitmap, sizeof( bm ), &bm );

	// Create a bitmap to hold the result
	int nNewBorder = bThin ? BORDERTHIN : BORDERTHICK;
	int nOldBorder = bBorderThin ? BORDERTHIN : BORDERTHICK;
	
	int newWidth, newHeight;
	int imageWidth, imageHeight;
	int indent;

	if (bBorder) {
		// We already have a border.
		indent = nOldBorder / 2;
		imageWidth = bm.bmWidth - nOldBorder;
		imageHeight = bm.bmHeight - nOldBorder;
		if (bAdd) {
			// The border is updated
			if (bBorderThin != bThin) {
				// The size of the border changed.
				if (bBorderThin) {
					// We had a thin border till now, but the user wants a thick one
					newWidth = bm.bmWidth - BORDERTHIN + BORDERTHICK;
					newHeight = bm.bmHeight - BORDERTHIN + BORDERTHICK;
				}
				else {
					// We had a thick border till now, but the user wants a thin one
					newWidth = bm.bmWidth - BORDERTHICK + BORDERTHIN;
					newHeight = bm.bmHeight - BORDERTHICK + BORDERTHIN;
				}
			}
			if (bBorderBlack != bBlack) {
				// The color of the border changed, but the size stays the same.
				newWidth = bm.bmWidth;
				newHeight = bm.bmHeight;
			}
			
		}
		else {
			// The border has to be removed
			newWidth = bm.bmWidth - nOldBorder;
			newHeight = bm.bmHeight - nOldBorder;
			nNewBorder = 0;
			bBorder = false;
		}
	}
	else {
		// We don't have a border yet.
		indent = 0;
		imageWidth = bm.bmWidth;
		imageHeight = bm.bmHeight;
		newWidth = bm.bmWidth + nNewBorder;
		newHeight = bm.bmHeight + nNewBorder;
		bBorder = true;
	}

	HBITMAP hbmResult = ::CreateCompatibleBitmap(CClientDC(NULL), newWidth, newHeight);

	// Select bitmaps into the DCs
	HBITMAP hbmOldSource = (HBITMAP)::SelectObject( sourceDC.m_hDC, (HBITMAP)bitmap);
	HBITMAP hbmOldDest = (HBITMAP)::SelectObject( destDC.m_hDC, hbmResult );

	// Draw the background of the destination DC with the border color

	COLORREF col;
	if (!bBlack)
		col = 0x00FFFFFF;	// Black
	else
		col = 0;			// White
	
	HBRUSH hbrBack = CreateSolidBrush(col);
	HBRUSH hbrOld = (HBRUSH)::SelectObject( destDC.m_hDC, hbrBack );
	destDC.PatBlt( 0, 0, newWidth, newHeight, PATCOPY );
	::DeleteObject( ::SelectObject( destDC.m_hDC, hbrOld ) );

	// Copy the image from the source to the dest DC
	destDC.BitBlt(nNewBorder / 2, nNewBorder / 2, imageWidth, imageHeight, &sourceDC, indent, indent, SRCCOPY);

	// Reselect the old bitmaps
	::SelectObject( sourceDC.m_hDC, hbmOldSource );
	::SelectObject( destDC.m_hDC, hbmOldDest );

	bBorderBlack = bBlack;
	bBorderThin = bThin;

	return (Update(hbmResult) == TRUE);
}

void CImage::ConvertGROBToBMP(BYTE *&bmp, BYTE *grob)
{
	BYTE byte1 = 0, byte2 = 0;

	long scans;
	
	if (planes > 1) {

		BYTE * planesbytes = new BYTE[planes];
		for (scans = imageInfo.height; scans >= 1; scans--) {
			for (int i = 0; i < imageInfo.bytesPerLine; i++) {
				// Read in the current bytes of all the planes. We will merge them
				// afterwards.
				for (int p = 0; p < planes; p++) {
					planesbytes[p] = grob[((scans - 1) * imageInfo.bytesPerLine) + i + p * imageInfo.size / 2 / 2];
					planesbytes[p] = ~(planesbytes[p]);
				}

				// Which bit has to be written in the current byte.
				int bmpbit = 0;

				// Because every byte contains information of 8 pixels we must process
				// every bit of the current byte.
				for (int bit = 0; bit < 8; bit++) {
					if (bit % 2 == 1) {
						// Every second bit is located at a new "nibble"
						bmpbit = 4;
					}
					for (p = planes - 1; p >= 0; p--) {
						// Set the bits for all planes. 
						SetBit(bmpbit + p, *bmp, (planesbytes[planes - p - 1] >> bit) & 0x1 == 1);
					}
					if (bit % 2 == 1) {
						// Turn the two nibbles.
						*bmp = ((*bmp >> 4) & 0x0F) | (*bmp << 4);
						// Go to the next byte.
						bmp++;
						bmpbit = 0;
					}
				}

			}
		}
		delete [] planesbytes;
	}
	else {
		for (scans = imageInfo.height; scans >= 1; scans--) {
			for (int i = 0; i < imageInfo.bytesPerLine; i++) {
				byte1 = grob[((scans - 1) * imageInfo.bytesPerLine) + i];
				byte1 = ~TurnByte(byte1);
				if (i == imageInfo.bytesPerLine - 1) {
					byte1 &= ~(0xFF >> (imageInfo.width % 8));
					*bmp++ = byte1;
					bmp += (imageInfo.fourBytes - imageInfo.bytesPerLine);
				}
				else {
					*bmp++ = byte1;
				}

			}
		}
	}
}

BYTE * CImage::GetBits()
{
	HANDLE hDIB = DDBToDIB(bitmap, 0);
	BYTE * pDib = (BYTE *)GlobalLock(hDIB);
	GlobalUnlock(hDIB);
	BITMAPINFO &bmInfoDIB = *(LPBITMAPINFO)pDib;

	int nColors = bmInfoDIB.bmiHeader.biClrUsed ? bmInfoDIB.bmiHeader.biClrUsed : 
						1 << bmInfoDIB.bmiHeader.biBitCount;

	if (bmInfoDIB.bmiHeader.biBitCount >= 16)
		nColors = 0;

	LPVOID lpDIBBits;
	if(bmInfoDIB.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfoDIB.bmiColors + bmInfoDIB.bmiHeader.biClrUsed) + 
			((bmInfoDIB.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfoDIB.bmiColors + nColors);

	return (BYTE *) lpDIBBits;

}

int CImage::GetBitCount()
{
	HANDLE hDIB = DDBToDIB(bitmap, 0);
	BYTE * pDib = (BYTE *)GlobalLock(hDIB);
	GlobalUnlock(hDIB);
	BITMAPINFO &bmInfoDIB = *(LPBITMAPINFO)pDib;
	return bmInfoDIB.bmiHeader.biBitCount;
}
