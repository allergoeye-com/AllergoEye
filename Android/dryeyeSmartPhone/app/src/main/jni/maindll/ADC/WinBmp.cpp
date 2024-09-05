/*
 * WinBmp.cpp
 *
 *  Created on: Sep 5, 2014
 *      Author: alex
 */
#include "stdafx.h"
#include "APalette.h"
#include "WinBmp.h"
#include "mfile.h"
#include "MemImage.h"

#define IntToMem Long2Mem

inline void MemToInt(BYTE *a, int &b)
{ b = Mem2Long(a); }

#define LongToMem Long2Mem

inline void MemToLong(BYTE *a, int &b)
{ b = Mem2Long(a); }


//-----------------------------------------------------
//
//-----------------------------------------------------
WinBmp::WinBmp()
{
    palette = 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
WinBmp::~WinBmp()
{
    if (palette) delete palette;
    palette = 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL WinBmp::Load(const char *filename)
{
    DeletePicture();
    MFILE file(filename);
    int size;
    char *point;

    if (!file.Open(false))
        return 0;
    size = file.GetSize();

    point = (char *) HMALLOC(size);
    char *hp = point;
    int ul = size;
    int BYTES_PER_READ = 1024 * 1024 * 10;
    while (ul > BYTES_PER_READ)
    {
        if (!file.Read(-1, (char *) hp, BYTES_PER_READ))
        {
            return 0;
        }

        ul -= BYTES_PER_READ;
        hp += BYTES_PER_READ;
    }
    if (ul && !file.Read(-1, (char *) hp, ul))
    {
        HFREE((void *) point);
        return 0;
    }

    file.Close();

    char ret = Load(point, size);
    HFREE((void *) point);

    if (ret == FALSE)
        DeletePicture();
    return ret;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL WinBmp::Load(char *pData, ULONG size)
{
    DeletePicture();

//	BITMAPFILEHEADER header;
    if (size <= sizeofBITMAPFILEHEADER)
        return FALSE;
    short int wrem = *(short int *) pData;
    if (wrem != DIB_HEADER_MARKER)
        return FALSE;
    pData += BYTE_FROM_BITMAPFILEHEADER;
    char ret = CreateBmp(pData);

    if (ret == FALSE)
        DeletePicture();
    return ret;

}

BOOL WinBmp::Load(DWORD *pal, int wNumColors, char *lpDibBits, int dwWidth, int dwHeight, int pitch)
{
//	int Width = dwWidth;
//	int Height = dwHeight;
    DeletePicture();
    palette = new APalette(pal, wNumColors);

    ImageInfo info;
    BYTE *new_ptr = (BYTE *) AttachData(dwWidth, dwHeight, info);
    if (!new_ptr)
        return false;
    ConvertPalTo32((BYTE *) lpDibBits, info.width, info.height, new_ptr, info.wline * sizeof(DWORD),
                   true, palette, 0, pitch);
    DetachData();

    return new_ptr != 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL WinBmp::CreateBmp(char *point)
{
    if (!CreateDIBPalette(point) || !DIBToBitmap(point))
        return FALSE;
    return TRUE;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL WinBmp::CreateDIBPalette(char *lpbi)
{
    char bWinStyleDIB = (char) is_win30_dib(lpbi);
    int wNumColors = (int) DIBNumColors(lpbi);
    if (wNumColors)
    {
        LOGPALETTE *lpPal = (LOGPALETTE *) HMALLOC(sizeof(LOGPALETTE) +
                                                   sizeof(PALETTEENTRY) * wNumColors);

        lpPal->palVersion = PALVERSION;
        lpPal->palNumEntries = (WORD) wNumColors;

        char *pnt;
        if (bWinStyleDIB)
            pnt = lpbi + BYTE_FROM_BITMAPINFOHEADER; //- BYTE_FROM_RGBQUAD;
        else
            pnt = lpbi + BYTE_FROM_BITMAPCOREHEADER;// - BYTE_FROM_RGBTRIPLE;

        for (int i = 0; i < wNumColors; i++)
        {
            lpPal->palPalEntry[i].peBlue = *pnt;
            lpPal->palPalEntry[i].peGreen = pnt[1];
            lpPal->palPalEntry[i].peRed = pnt[2];
            lpPal->palPalEntry[i].peFlags = 0;
            if (bWinStyleDIB)
                pnt += BYTE_FROM_RGBQUAD;
            else
                pnt += BYTE_FROM_RGBTRIPLE;
        }

        palette = new APalette(lpPal);
        HFREE((void *) lpPal);
    } else
        palette = new APalette(RGB_PAL);
    return TRUE;
}

int WinBmp::BitCount()
{
    int biBitCount = palette->GetNumColors();
    if (palette->IsDef())
        biBitCount = 24;
    else if (biBitCount <= 2)
        biBitCount = 1;
    else if (biBitCount <= 16)
        biBitCount = 4;
    else if (biBitCount <= 256)
        biBitCount = 8;
    else
        biBitCount = 24;
    return biBitCount;

}

HGLOBAL WinBmp::BitmapToDIB(BOOL flg)
{

    ABITMAPINFOHEADER bmInfoHdr;
    ImageInfo info;
    BOOL fAt = IsAttachedData();

    BYTE *src = (BYTE *) AttachData(info);
    if (!src)
        return 0;
    APalette *pal = 0;
    APalette *_palette = palette;

    if (flg && _palette->IsDef())
    {
        MemImage img((DWORD *) src, info);
        MArray<char> data;
        img.GetRGB(data, FALSE);
        _palette = new APalette((BYTE *) data.GetPtr(), info);
        if (flg == 2)
            palette = _palette;
        else
            pal = _palette;
    }

    InitBitmapInfoHeader(&bmInfoHdr,
                         info.width,
                         info.height,
                         _palette->GetNumColors(), _palette->IsDef());

    int sz_all = sizeofBITMAPINFOHEADER +
                 PaletteSize((char *) &bmInfoHdr) + bmInfoHdr.biSizeImage;
    HGLOBAL hDIB = GlobalAlloc(GHND, sz_all);
    if (!hDIB)
    {
        if (!fAt) DetachData();

        return 0;
    }
    BYTE *ptr = (BYTE *) GlobalLock (hDIB);
    if (ptr == 0L)
    {
        GlobalFree(hDIB);
        if (!fAt) DetachData();

        return 0;
    }
    bmInfoHdr.Serialize(ptr);
    BYTE *lpBits = (BYTE *) FindDIBBits((char *) ptr);
    //   char *tmp = (char *)lpBits;
    int clr = _palette->GetNumColors();
    if (clr)
    {
        ColorRef *pal = (ColorRef *) HMALLOC(sizeof(COLORREF) * clr);
        memset((char *) pal, 0, sizeof(COLORREF) * clr);
        _palette->GetRGBPalette((COLORREF *) pal, clr);
        BYTE *pp = (BYTE *) (ptr + sizeofBITMAPINFOHEADER);
        for (int i = 0; i < clr; ++i)
        {

            ARGBQUAD tmp;
            tmp.rgbBlue = pal[i].b;
            tmp.rgbRed = pal[i].r;
            tmp.rgbGreen = pal[i].g;
            tmp.rgbReserved = 0;
            tmp.Serialize(pp + i * sizeofRGBQUAD);

        }
        HFREE(pal);
    }

    Convert32ToPal(src, info.width, info.height, lpBits, info.wline * sizeof(DWORD), _palette,
                   true);

    GlobalUnlock (hDIB);
    if (pal)
        delete pal;
    if (!fAt) DetachData();

    return hDIB;


}

BOOL WinBmp::Save(const char *filename)
{
    HGLOBAL hdib = BitmapToDIB(false);
    if (!hdib) return FALSE;
    BOOL ret = SaveDIB(filename, hdib);
    GlobalFree(hdib);
    return ret;
}

BOOL WinBmp::SaveDIB(const char *FileName, HGLOBAL hdib)
{

    ABITMAPFILEHEADER hdr;
    ABITMAPINFOHEADER lpbi;

    if (!hdib)
        return FALSE;
    MFILE file(FileName);
    if (!file.Open(true)) return 0;
    BYTE *bbi = (BYTE *) GlobalLock(hdib);
    if (!bbi)
        return FALSE;
    lpbi.UnSerialize(bbi);
    hdr.bfType = DIB_HEADER_MARKER;
    hdr.bfSize = (DWORD) (GlobalSize(hdib) + sizeofBITMAPFILEHEADER);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;
    hdr.bfOffBits = (DWORD) sizeofBITMAPFILEHEADER + lpbi.biSize +
                    PaletteSize((LPSTR) bbi);
    DWORD len = (DWORD) GlobalSize(hdib);
    BYTE bhdr[sizeofBITMAPFILEHEADER];
    hdr.Serialize(bhdr);
    if (!file.Write(-1, (char *) bhdr, sizeofBITMAPFILEHEADER))
    {
        GlobalUnlock (hdib);
        return FALSE;
    }
    if (!file.Write(-1, (char *) bbi, (unsigned) len))
    {
        //DWORD dw = GetLastError();
        GlobalUnlock (hdib);
        return FALSE;
    }
    GlobalUnlock (hdib);
    return TRUE;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL WinBmp::DIBToBitmap(char *lpbi)
{

    BYTE *bmpData = (BYTE *) FindDIBBits(lpbi);
    int biWidth, biHeight;
    INT biBitCount;
    MemToLong((BYTE *) (lpbi + 4), biWidth);
    MemToLong((BYTE *) (lpbi + 8), biHeight);
    MemToInt((BYTE *) (lpbi + 14), biBitCount);
    ImageInfo info;
    BYTE *new_ptr = (BYTE *) AttachData(biWidth, biHeight, info);
    if (!new_ptr)
        return false;
    ConvertPalTo32(bmpData, info.width, info.height, new_ptr, info.wline * sizeof(DWORD), true,
                   palette, biBitCount);
    DetachData();
    return true;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
int WinBmp::DIBNumColors(char *lpbi)
{
    INT wBitCount;

    if (is_win30_dib(lpbi))
    {
        int dwClrUsed;
        MemToLong((BYTE *) (lpbi + 32), dwClrUsed);

        if (dwClrUsed)
            return (unsigned) dwClrUsed;
    }
    if (is_win30_dib(lpbi))
        MemToInt((BYTE *) (lpbi + 14), wBitCount);
    else
        MemToInt((BYTE *) (lpbi + 10), wBitCount);

    switch (wBitCount)
    {
        case 1:
            return 2;
        case 4:
            return 16;
        case 8:
            return 256;
        default:
            return 0;
    }
}

//-----------------------------------------------------
//
//-----------------------------------------------------
char *WinBmp::FindDIBBits(char *lpbi)
{
    int s;
    MemToLong((BYTE *) lpbi, s);
    return (lpbi + (DWORD) s + PaletteSize(lpbi));
}

//-----------------------------------------------------
//
//-----------------------------------------------------
unsigned WinBmp::PaletteSize(char *lpbi)
{
    if (is_win30_dib(lpbi))
        return (DIBNumColors(lpbi) * BYTE_FROM_RGBQUAD);
    return (DIBNumColors(lpbi) * BYTE_FROM_RGBTRIPLE);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void WinBmp::InitBitmapInfoHeader(ABITMAPINFOHEADER *lpBmInfoHdr, int dwWidth,
                                  int dwHeight, int numColors, BOOL flg)
{
    int NumBitsPix = 24;

    memset(lpBmInfoHdr, 0, sizeof(ABITMAPINFOHEADER));
    lpBmInfoHdr->biSize = sizeof(ABITMAPINFOHEADER);
    lpBmInfoHdr->biWidth = dwWidth;
    lpBmInfoHdr->biHeight = dwHeight;
    lpBmInfoHdr->biPlanes = 1;
    if (!flg)
        switch (numColors)
        {
            case 256:
                NumBitsPix = 8;
                break;
            case 16:
                NumBitsPix = 4;
                break;
            case 2:
                NumBitsPix = 1;
                break;
        }

    lpBmInfoHdr->biBitCount = (WORD) NumBitsPix;
    lpBmInfoHdr->biSizeImage = WIDTHBYTES (dwWidth * NumBitsPix) * dwHeight;
    lpBmInfoHdr->biXPelsPerMeter = 3780;
    lpBmInfoHdr->biYPelsPerMeter = 3780;

}

void WinBmp::Convert32ToPal(BYTE *ptr, int iWidth, int iHeight, BYTE *&new_ptr, int i_width,
                            APalette *pal, bool fInverse)
{
    COLORREF rgb;
    int j;
    BYTE t;

    int biBitCount = pal->GetNumColors();
    if (pal->IsDef())
        biBitCount = 24;
    else if (biBitCount <= 2)
        biBitCount = 1;
    else if (biBitCount <= 16)
        biBitCount = 4;
    else if (biBitCount <= 256)
        biBitCount = 8;
    else
        biBitCount = 24;

    int r_width = WIDTHBYTES(iWidth * biBitCount);

    if (!new_ptr) new_ptr = (BYTE *) HMALLOC(r_width * iHeight);
    BYTE *tmp = new_ptr;
    if (!new_ptr) return;
    if (fInverse)
    {
        new_ptr += r_width * (iHeight - 1);
        r_width = -r_width;
    }
    for (int k, i = 0; i < iHeight; ++i)
    {

        for (k = j = 0; j < iWidth;)
        {
            rgb = RGB(ptr[j * 4], ptr[j * 4 + 1], ptr[j * 4 + 2]);
            switch (biBitCount)
            {
                case 24:
                    new_ptr[j * 3 + 2] = ptr[j * 4];
                    new_ptr[j * 3 + 1] = ptr[j * 4 + 1];
                    new_ptr[j * 3] = ptr[j * 4 + 2];
                    ++j;
                    break;
                case 1:
                {
                    if ((j % 8) == 0)
                        new_ptr[k] = 0;
                    new_ptr[k] |= (rgb != 0) << (7 - (j & 7));
                    if ((j & 7) == 7)
                        ++k;
                    ++j;
                }
                    break;
                case 4:

                    t = pal->GetIndex(rgb);
                    new_ptr[k] = t << 4;

                    if (j < iWidth - 1)
                    {
                        ++j;
                        rgb = RGB(ptr[j * 4], ptr[j * 4 + 1], ptr[j * 4 + 2]);
                        t = pal->GetIndex(rgb);
                        new_ptr[k] |= t;

                    }
                    ++j;
                    ++k;
                    break;
                case 8:
                    new_ptr[j] = pal->GetIndex(rgb);
                    ++j;
                    break;
            }
        }
        new_ptr += r_width;
        ptr += i_width;
    }
    new_ptr = tmp;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
void WinBmp::ConvertPalTo32(BYTE *ptr, int iWidth, int iHeight, BYTE *&new_ptr, int r_width,
                            bool fInverse, APalette *pal, int _biBitCount, int pitch)
{
    COLORREF rgb;
    int j;


    int biBitCount = _biBitCount;
    if (_biBitCount == 0)
    {

        biBitCount = pal->GetNumColors();
        if (pal->IsDef())
            biBitCount = 24;
        else if (biBitCount <= 2)
            biBitCount = 1;
        else if (biBitCount <= 16)
            biBitCount = 4;
        else if (biBitCount <= 256)
            biBitCount = 8;
        else
            biBitCount = 24;

    }
    int i_width = pitch == -1 ? WIDTHBYTES(iWidth * biBitCount) : pitch;
    int biK = biBitCount / 8;
//if (!new_ptr) new_ptr = (BYTE *)HMALLOC(r_width * iHeight);
    if (!new_ptr) return;
    BYTE *tmp = new_ptr;
    if (fInverse)
    {
        new_ptr += r_width * (iHeight - 1);
        r_width = -r_width;
    }

    for (int n, index, k, i = 0; i < iHeight; ++i)
    {

        for (k = j = 0; j < iWidth; ++j)
        {

            switch (biBitCount)
            {
                case 16:
                {
                    USHORT t = (USHORT) ptr[j * biK] | (((USHORT) ptr[j * biK + 1] << 8) & 0xFF00);
                    new_ptr[j * 4 + 2] = (BYTE) (t & 0x1f) << 3;
                    new_ptr[j * 4 + 1] = (BYTE) ((t >> 5) & 0x1f) << 3;
                    new_ptr[j * 4] = (BYTE) ((t >> 10) & 0x1f) << 3;
                }

                    break;

                case 32:
                case 24:
                    new_ptr[j * 4 + 2] = ptr[j * biK];
                    new_ptr[j * 4 + 1] = ptr[j * biK + 1];
                    new_ptr[j * 4] = ptr[j * biK + 2];

                    break;

                case 1:
                {
                    BYTE b = ptr[k];
                    for (int n = 0; n < 8; ++n)
                    {
                        index = ((b >> (7 - ((n) & 7))) & 1);
                        memset(new_ptr + j * 4, index ? 255 : 0, 3);
                        if (n < 7)
                            ++j;

                        if (j + 1 >= iWidth)
                            break;
                    }
                    ++k;
                }
                    break;
                case 4:
                    n = (ptr[k] >> 4) & 0x0f;
                    rgb = pal->GetRGB(n);
                    new_ptr[j * 4] = GetRValue(rgb);
                    new_ptr[j * 4 + 1] = GetGValue(rgb);
                    new_ptr[j * 4 + 2] = GetBValue(rgb);
                    new_ptr[j * 4 + 3] = 255;
                    n = ptr[k] & 0x0f;
                    ++j;
                    if (j < iWidth)
                    {
                        rgb = pal->GetRGB(n);
                        new_ptr[j * 4] = GetRValue(rgb);
                        new_ptr[j * 4 + 1] = GetGValue(rgb);
                        new_ptr[j * 4 + 2] = GetBValue(rgb);

                    }

                    ++k;
                    break;
                case 8:

                    rgb = pal->GetRGB(ptr[k]);
                    new_ptr[j * 4 + 0] = GetRValue(rgb);
                    new_ptr[j * 4 + 1] = GetGValue(rgb);
                    new_ptr[j * 4 + 2] = GetBValue(rgb);


                    ++k;
            }
            if (j < iWidth)
                new_ptr[j * 4 + 3] = 255;


        }
        new_ptr += r_width;
        ptr += i_width;
    }
    new_ptr = tmp;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
int WinBmp::InversImage(char *bits, int w, int h, int bpp)
{
    int LineLen;
    switch (bpp)
    {
        case 1 :
            LineLen = w % 8 ? w / 8 + 1 : w / 8;
            break;
        case 4 :
            LineLen = w % 2 ? w / 2 + 1 : w / 2;
            break;
        case 8 :
            LineLen = w;
            break;
        case 24 :
            LineLen = w * 3;
            break;
        default :
            return w;
    }
    LineLen = LineLen % 4 ? (LineLen / 4 + 1) * 4 : LineLen;
    char *buff = new char[LineLen];
    if (buff)
    {
        memset(buff, 0, LineLen);
        for (int i = 0; i < h / 2; ++i)
        {
            memcpy(buff, bits + (int) LineLen * (int) i, LineLen);
            memcpy(bits + (int) LineLen * (int) i, bits + (int) LineLen * (int) (h - 1 - i),
                   LineLen);
            memcpy(bits + (int) LineLen * (int) (h - 1 - i), buff, LineLen);
        }
        delete[] buff;
    }
    return LineLen;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void WinBmp::DoInverse(ABITMAPINFOHEADER *bmiInfo, char *bits)
{
    bmiInfo->biHeight = -bmiInfo->biHeight;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
BOOL WinBmp::is_win30_dib(char *lpbi)
{
    int wrem;
    MemToLong((BYTE *) lpbi, wrem);
    return wrem == BYTE_FROM_BITMAPINFOHEADER;
}

//-------------------------------------
//
//-------------------------------------
BOOL WinBmp::GetBitmapBits(char *&lpBits, int &len)
{
    ImageInfo info;
    BOOL fAt = IsAttachedData();
    BYTE *src = (BYTE *) AttachData(info);
    if (src)
    {
        if (!len || !lpBits)
            len = WIDTHBYTES(info.width * BitCount()) * info.height;
        if (!lpBits) lpBits = (char *) HMALLOC(len);
        Convert32ToPal(src, info.width, info.height, (BYTE *&) lpBits, info.wline * sizeof(DWORD),
                       palette, false);
        if (!fAt) DetachData();
        return true;
    }
    return false;
}

//-------------------------------------
//
//-------------------------------------
BOOL WinBmp::SetBitmapBits(char *lpBits, int len)
{
    ImageInfo info;
    BOOL fAt = IsAttachedData();
    BYTE *new_ptr = (BYTE *) AttachData(info);
    if (!new_ptr)
        return false;
    ConvertPalTo32((BYTE *) lpBits, info.width, info.height, new_ptr, info.wline * sizeof(DWORD),
                   true, palette, 0);
    if (!fAt) DetachData();
    return true;

}


