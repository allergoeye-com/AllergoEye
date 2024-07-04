//==============================================================
//
// File MSWIN.H
// is a part of the MDC - Windows oriented graphic library,
// intended to use with Pluk Interpreter.
// version 1.3.
// Copyright (C) 1996 Alexander L. Kalaidzidis. 
//
//==============================================================
#ifndef __MSWIN_TYPE__
#define __MSWIN_TYPE__
#ifdef __PLUK_LINUX__

#include "std.h"
#include "afx.h"

#ifndef COLORREF
typedef uint32_t COLORREF;
#endif
#define RGBA(r, g, b, a)((COLORREF)(( ((DWORD)(BYTE)(r) & 0xff)  | (((DWORD)((BYTE)(g))<<8) & 0xff00) | ((((DWORD)(BYTE)(b))<<16) & 0xff0000) | ((((DWORD)(BYTE)(a))<<24) & 0xff000000) )))
#define RGB(r, g, b)  RGBA(r,g,b, 0)
#define GetRValue(rgb)  ((BYTE)(rgb))
#define GetGValue(rgb)  ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)  ((BYTE)((rgb)>>16))


#pragma pack(push) //NADA
#pragma pack(2)

typedef struct tagPALETTEENTRY {
    BYTE peRed;
    BYTE peGreen;
    BYTE peBlue;
    BYTE peFlags;

    BYTE *Serialize(BYTE *p)
    {
        if (p)
        {
            *p++ = peRed;
            *p++ = peGreen;
            *p++ = peBlue;
            *p++ = peFlags;
        }
        return p;
    }

    BYTE *UnSerialize(BYTE *p)
    {
        if (p)
        {

            peRed = *p++;
            peGreen = *p++;
            peBlue = *p++;
            peFlags = *p++;
        }
        return p;
    }

} PALETTEENTRY;
typedef struct tagLOGPALETTE {
    WORD palVersion;
    WORD palNumEntries;
    PALETTEENTRY palPalEntry[1];
} LOGPALETTE;
typedef tagLOGPALETTE *LPLOGPALETTE;

typedef struct tagBITMAP {
    INT bmType;
    INT bmWidth;
    INT bmHeight;
    INT bmWidthBytes;
    BYTE bmPlanes;
    BYTE bmBitsPixel;
    void *bmBits;
} BITMAP;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    INT biWidth;
    INT biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    INT biXPelsPerMeter;
    INT biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;

    BYTE *Serialize(BYTE *p)
    {
        if (p)
        {

            DWord2Mem(biSize, p);
            p += sizeof(DWORD);
            Int2Mem(biWidth, p);
            p += sizeof(DWORD);
            Int2Mem(biHeight, p);
            p += sizeof(DWORD);
            Short2Mem(biPlanes, p);
            p += sizeof(SHORT);
            Short2Mem(biBitCount, p);
            p += sizeof(SHORT);
            DWord2Mem(biCompression, p);
            p += sizeof(DWORD);
            DWord2Mem(biSizeImage, p);
            p += sizeof(DWORD);
            Int2Mem(biXPelsPerMeter, p);
            p += sizeof(DWORD);
            Int2Mem(biYPelsPerMeter, p);
            p += sizeof(DWORD);
            DWord2Mem(biClrUsed, p);
            p += sizeof(DWORD);
            DWord2Mem(biClrImportant, p);
            p += sizeof(DWORD);
        }
        return p;

    }

    BYTE *UnSerialize(BYTE *p)
    {
        if (p)
        {

            biSize = Mem2DWord(p);
            p += sizeof(DWORD);
            biWidth = Mem2Int(p);
            p += sizeof(DWORD);
            biHeight = Mem2Int(p);
            p += sizeof(DWORD);
            biPlanes = Mem2Short(p);
            p += sizeof(SHORT);
            biBitCount = Mem2Short(p);
            p += sizeof(SHORT);
            biCompression = Mem2DWord(p);
            p += sizeof(DWORD);
            biSizeImage = Mem2DWord(p);
            p += sizeof(DWORD);
            biXPelsPerMeter = Mem2Int(p);
            p += sizeof(DWORD);
            biYPelsPerMeter = Mem2Int(p);
            p += sizeof(DWORD);
            biClrUsed = Mem2DWord(p);
            p += sizeof(DWORD);
            biClrImportant = Mem2DWord(p);
            p += sizeof(DWORD);
        }
        return p;
    }

} BITMAPINFOHEADER;
typedef struct tagBITMAPINFOHEADER *LPBITMAPINFOHEADER;
typedef struct tagBITMAPCOREHEADER {
    DWORD bcSize;                 /* used to get to color table */
    WORD bcWidth;
    WORD bcHeight;
    WORD bcPlanes;
    WORD bcBitCount;

    BYTE *Serialize(BYTE *p)
    {
        if (p)
        {
            DWord2Mem(bcSize, p);
            p += sizeof(DWORD);
            Short2Mem(bcWidth, p);
            p += sizeof(WORD);
            Short2Mem(bcHeight, p);
            p += sizeof(WORD);
            Short2Mem(bcPlanes, p);
            p += sizeof(WORD);
            Short2Mem(bcBitCount, p);
            p += sizeof(WORD);
        }
        return p;
    }

    BYTE *UnSerialize(BYTE *p)
    {
        if (p)
        {
            bcSize = Mem2DWord(p);
            p += sizeof(DWORD);
            bcWidth = Mem2Short(p);
            p += sizeof(WORD);
            bcHeight = Mem2Short(p);
            p += sizeof(WORD);
            bcPlanes = Mem2Short(p);
            p += sizeof(WORD);
            bcBitCount = Mem2Short(p);
            p += sizeof(WORD);
        }
        return p;

    }

} BITMAPCOREHEADER, *LPBITMAPCOREHEADER, *PBITMAPCOREHEADER;

struct RGBQUAD {
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;

    BYTE *Serialize(BYTE *p)
    {
        if (p)
        {
            *p++ = rgbBlue;
            *p++ = rgbGreen;
            *p++ = rgbRed;
            *p++ = rgbReserved;
        }
        return p;
    }

    BYTE *UnSerialize(BYTE *p)
    {
        if (p)
        {

            rgbBlue = *p++;
            rgbGreen = *p++;
            rgbRed = *p++;
            rgbReserved = *p++;
        }
        return p;
    }

};

#define sizeofRGBTRIPLE 3

struct RGBTRIPLE {
    BYTE rgbtBlue;
    BYTE rgbtGreen;
    BYTE rgbtRed;

    BYTE *Serialize(BYTE *p)
    {
        if (p)
        {

            *p++ = rgbtBlue;
            *p++ = rgbtGreen;
            *p++ = rgbtRed;
        }
        return p;
    }

    BYTE *UnSerialize(BYTE *p)
    {
        if (p)
        {
            rgbtBlue = *p++;
            rgbtGreen = *p++;
            rgbtRed = *p++;
        }
        return p;
    }
};

typedef struct tagBITMAPFILEHEADER {
    USHORT bfType;
    DWORD bfSize;
    SHORT bfReserved1;
    SHORT bfReserved2;
    DWORD bfOffBits;

    BYTE *Serialize(BYTE *p)
    {
        if (p)
        {
            Short2Mem(bfType, p);
            p += sizeof(SHORT);
            DWord2Mem(bfSize, p);
            p += sizeof(DWORD);
            Short2Mem(bfReserved1, p);
            p += sizeof(SHORT);
            Short2Mem(bfReserved2, p);
            p += sizeof(SHORT);
            DWord2Mem(bfOffBits, p);
            p += sizeof(DWORD);
        }
        return p;

    }

    BYTE *UnSerialize(BYTE *p)
    {
        if (p)
        {
            bfType = Mem2Short(p);
            p += sizeof(SHORT);
            bfSize = Mem2DWord(p);
            p += sizeof(DWORD);
            bfReserved1 = Mem2Short(p);
            p += sizeof(SHORT);
            bfReserved2 = Mem2Short(p);
            p += sizeof(SHORT);
            bfOffBits = Mem2DWord(p);
            p += sizeof(DWORD);
        }
        return p;

    }

} BITMAPFILEHEADER;

typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD bmiColors[1];
} BITMAPINFO;
typedef BITMAPINFO *LPBITMAPINFO;
typedef struct tagBITMAPCOREINFO {
    BITMAPCOREHEADER bmciHeader;
    RGBTRIPLE bmciColors[1];
} BITMAPCOREINFO;
#define sizeofICONDIRENTRY 16

struct ICONDIRENTRY {
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD bReserved1;
    WORD bReserved2;
    DWORD dwBytesInRes;
    DWORD dwImageOffset;

    BYTE *Serialize(BYTE *p)
    {
        if (p)
        {
            *p++ = bWidth;
            *p++ = bHeight;
            *p++ = bColorCount;
            *p++ = bReserved;
            Short2Mem(bReserved1, p);
            p += sizeof(USHORT);
            Short2Mem(bReserved2, p);
            p += sizeof(USHORT);
            DWord2Mem(dwBytesInRes, p);
            p += sizeof(DWORD);
            DWord2Mem(dwImageOffset, p);
            p += sizeof(DWORD);
        }
        return p;
    }

    BYTE *UnSerialize(BYTE *p)
    {
        if (p)
        {
            bWidth = *p++;
            bHeight = *p++;
            bColorCount = *p++;
            bReserved = *p++;
            bReserved1 = Mem2Short(p);
            p += sizeof(USHORT);
            bReserved2 = Mem2Short(p);
            p += sizeof(USHORT);
            dwBytesInRes = Mem2DWord(p);
            p += sizeof(DWORD);
            dwImageOffset = Mem2DWord(p);
            p += sizeof(DWORD);
        }
        return p;
    }
};

#define sizeofCURSORDIRENTRY 16

struct CURSORDIRENTRY {
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD wXHotSpot;
    WORD wYHotSpot;
    DWORD dwBytesInRes;
    DWORD dwImageOffset;

    BYTE *Serialize(BYTE *p)
    {
        if (p)
        {
            *p++ = bWidth;
            *p++ = bHeight;
            *p++ = bColorCount;
            *p++ = bReserved;
            Short2Mem(wXHotSpot, p);
            p += sizeof(USHORT);
            Short2Mem(wYHotSpot, p);
            p += sizeof(USHORT);
            DWord2Mem(dwBytesInRes, p);
            p += sizeof(DWORD);
            DWord2Mem(dwImageOffset, p);
            p += sizeof(DWORD);
        }
        return p;
    }

    BYTE *UnSerialize(BYTE *p)
    {
        if (p)
        {
            bWidth = *p++;
            bHeight = *p++;
            bColorCount = *p++;
            bReserved = *p++;
            wXHotSpot = Mem2Short(p);
            p += sizeof(USHORT);
            wYHotSpot = Mem2Short(p);
            p += sizeof(USHORT);
            dwBytesInRes = Mem2DWord(p);
            p += sizeof(DWORD);
            dwImageOffset = Mem2DWord(p);
            p += sizeof(DWORD);
        }
        return p;
    }
};

#pragma pack(pop)
#ifndef RED
#define    BLACK            RGB(0, 0, 0)
#define    BLUE            RGB(0, 0, 128)
#define    GREEN            RGB(0, 128, 0)
#define    CYAN            RGB(0, 128, 128)
#define    RED            RGB(200, 0, 0 )
#define    MAGENTA        RGB(128, 0, 128)
#define    BROWN            RGB(128, 0, 0)
#define    LIGHTGRAY        RGB(192,192,192)
#define    DARKGRAY        RGB(130,130,130)
#define    LIGHTBLUE        RGB(0,0,0xff)
#define    LIGHTGREEN        RGB(0, 0xff, 0)
#define    LIGHTCYAN        RGB(0,0xff,0xff)
#define    LIGHTRED        RGB(0xff,0, 0)
#define    LIGHTMAGENTA    RGB(0xff,0,0xff)
#define    YELLOW            RGB(0xff, 0xff, 0)
#define    WHITE            RGB(0xff, 0xff, 0xff)
#define LGRAY        0x00c0c0c0L
#define LRED         0x000000ffL
#define LGREEN       0x0000ff00L
#define LMAGENTA     0x00ff00ffL
#define LCYAN        0x00ffff00L
#define LBLUE        0x00ff0000L
#define GRAY         0x007f7f7fL

inline BYTE GetBlue(DWORD c)
{
    return (BYTE) (c >> 16);
}

inline BYTE GetGreen(DWORD c)
{
    return (BYTE) (((WORD) c) >> 8);
}

inline BYTE GetRed(DWORD c)
{
    return (BYTE) c;
}

#endif
typedef BITMAPINFOHEADER ABITMAPINFOHEADER;
typedef RGBQUAD ARGBQUAD;
typedef BITMAPFILEHEADER ABITMAPFILEHEADER;

#else
#include <windows.h>
#include "buffer.h"

#define	BLACK 			RGB(0, 0, 0)
#define	BLUE  			RGB(0, 0, 128)
#define	GREEN 			RGB(0, 128, 0)
#define	CYAN 			RGB(0, 128, 128)
#define	RED 			RGB(200, 0, 0 )
#define	MAGENTA 		RGB(128, 0, 128)
#define	BROWN 			RGB(128, 0, 0)
#define	LIGHTGRAY 		RGB(192,192,192)
#define	DARKGRAY 		RGB(130,130,130)
#define	LIGHTBLUE 		RGB(0,0,0xff)
#define	LIGHTGREEN 		RGB(0, 0xff, 0)
#define	LIGHTCYAN 		RGB(0,0xff,0xff)
#define	LIGHTRED 		RGB(0xff,0, 0)
#define	LIGHTMAGENTA 	RGB(0xff,0,0xff)
#define	YELLOW 			RGB(0xff, 0xff, 0)
#define	WHITE  			RGB(0xff, 0xff, 0xff)
#define LGRAY        0x00c0c0c0L
#define LRED         0x000000ffL
#define LGREEN       0x0000ff00L
#define LMAGENTA     0x00ff00ffL
#define LCYAN        0x00ffff00L
#define LBLUE        0x00ff0000L
#define GRAY         0x007f7f7fL
struct ABITMAPINFOHEADER : public BITMAPINFOHEADER{
      BYTE *Serialize (BYTE *p)
        {            if (p)
            {

    DWord2Mem(biSize, p);
        p += sizeof(DWORD);
    Int2Mem(biWidth, p);
        p += sizeof(DWORD);
    Int2Mem(biHeight, p);
        p += sizeof(DWORD);
    Short2Mem(biPlanes, p);
        p += sizeof(SHORT);
    Short2Mem(biBitCount, p);
        p += sizeof(SHORT);
    DWord2Mem(biCompression, p);
        p += sizeof(DWORD);
    DWord2Mem(biSizeImage, p);
        p += sizeof(DWORD);
    Int2Mem(biXPelsPerMeter, p);
        p += sizeof(DWORD);
    Int2Mem(biYPelsPerMeter, p);
        p += sizeof(DWORD);
    DWord2Mem(biClrUsed, p);
        p += sizeof(DWORD);
    DWord2Mem(biClrImportant, p);
        p += sizeof(DWORD);
        }
        return p;

        }
        BYTE *UnSerialize (BYTE *p)
        {
                        if (p)
            {

    biSize = Mem2DWord(p);
        p += sizeof(DWORD);
    biWidth = Mem2Int(p);
        p += sizeof(DWORD);
    biHeight = Mem2Int(p);
        p += sizeof(DWORD);
    biPlanes = Mem2Short(p);
        p += sizeof(SHORT);
    biBitCount = Mem2Short(p);
        p += sizeof(SHORT);
    biCompression = Mem2DWord(p);
        p += sizeof(DWORD);
    biSizeImage = Mem2DWord(p);
        p += sizeof(DWORD);
    biXPelsPerMeter = Mem2Int(p);
        p += sizeof(DWORD);
    biYPelsPerMeter = Mem2Int(p);
        p += sizeof(DWORD);
    biClrUsed = Mem2DWord(p);
        p += sizeof(DWORD);
    biClrImportant = Mem2DWord(p);
        p += sizeof(DWORD);
    }
        return p;
        }
 };
 struct ARGBQUAD : public RGBQUAD {
     BYTE *Serialize(BYTE *p)
    {
            if (p)
            {
    *p++= rgbBlue;
    *p++= rgbGreen;
    *p++= rgbRed;
    *p++= rgbReserved;
            }
    return p;
    }
    BYTE *UnSerialize(BYTE *p)
    {
                    if (p)
            {

    rgbBlue = *p++;
    rgbGreen = *p++;
    rgbRed = *p++;
    rgbReserved = *p++;
                    }
    return p;
    }


 };
struct ABITMAPFILEHEADER : public  BITMAPFILEHEADER {
   BYTE *Serialize(BYTE *p)
    {
        if (p)
            {
    Short2Mem(bfType, p);
    p += sizeof(SHORT);
    DWord2Mem(bfSize, p);
    p += sizeof(DWORD);
    Short2Mem(bfReserved1, p);
    p += sizeof(SHORT);
    Short2Mem(bfReserved2, p);
    p += sizeof(SHORT);
    DWord2Mem(bfOffBits, p);
    p += sizeof(DWORD);
        }
    return p;

    }
    BYTE *UnSerialize(BYTE *p)
    {
        if (p)
            {
    bfType = Mem2Short(p);
    p += sizeof(SHORT);
    bfSize = Mem2DWord(p);
    p += sizeof(DWORD);
    bfReserved1 = Mem2Short(p);
    p += sizeof(SHORT);
    bfReserved2 = Mem2Short(p);
    p += sizeof(SHORT);
    bfOffBits = Mem2DWord(p);
    p += sizeof(DWORD);
        }
    return p;
        
    }
};
#endif
#define BYTE_FROM_RGBTRIPLE 3
#define BYTE_FROM_RGBQUAD 4
#define BYTE_FROM_BITMAPINFOHEADER 40
#define BYTE_FROM_BITMAPFILEHEADER 14
#define BYTE_FROM_BITMAPCOREHEADER 12
#define sizeofBITMAPINFOHEADER BYTE_FROM_BITMAPINFOHEADER
#define sizeofBITMAPFILEHEADER BYTE_FROM_BITMAPFILEHEADER
#define sizeofBITMAPCOREHEADER BYTE_FROM_BITMAPCOREHEADER
#define sizeofRGBQUAD 4


#endif
