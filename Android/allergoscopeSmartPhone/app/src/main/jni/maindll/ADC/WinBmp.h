/*
 * WinBmp.h
 *
 *  Created on: Sep 5, 2014
 *      Author: alex
 */

#ifndef WINBMP_H_
#define WINBMP_H_

class APalette;

#pragma pack(push)
#pragma pack(8)

EXPORTNIX class EXPMDC WinBmp {
public:
    WinBmp();

    virtual ~WinBmp();

    virtual void DeletePicture() = 0;

    virtual DWORD *AttachData(int x, int y, ImageInfo &info) = 0;

    virtual DWORD *AttachData(ImageInfo &info) = 0;

    virtual bool IsAttachedData() = 0;

    virtual BOOL DetachData() = 0;

    BOOL DIBToBitmap(char *lpbi);

    BOOL Load(const char *filename);

    BOOL Save(const char *filename);

    BOOL Load(char *pData, ULONG size);

    BOOL
    Load(DWORD *pal, int wNumColors, char *lpDibBits, int dwWidth, int dwHeight, int pitch = -1);

    HGLOBAL GetDIB(BOOL flg)
    { return BitmapToDIB(flg); }

    BOOL GetDibBits(char *&lpBits, int &len)
    { return GetBitmapBits(lpBits, len); }

    BOOL GetBitmapBits(char *&lpBits, int &len);

    BOOL SetBitmapBits(char *lpBits, int len);

    int BitCount();

    HGLOBAL BitmapToDIB(BOOL flg);

    APalette *GetPalette()
    { return palette; }

    static void InitBitmapInfoHeader(ABITMAPINFOHEADER *lpBmInfoHdr, int dwWidth,
                                     int dwHeight, int numColors, BOOL flg);

protected:
    BOOL SaveDIB(const char *FileName, HGLOBAL hdib);

    BOOL CreateDIBPalette(char *lpbi);

    BOOL CreateBmp(char *point);

    int DIBNumColors(char *lpbi);

    char *FindDIBBits(char *lpbi);

    unsigned PaletteSize(char *lpbi);

    int InversImage(char *bits, int w, int h, int bpp);

    void DoInverse(ABITMAPINFOHEADER *bmiInfo, char *bits);

    BOOL is_win30_dib(char *lpbi);

    void
    ConvertPalTo32(BYTE *ptr, int iWidth, int iHeight, BYTE *&new_ptr, int r_width, bool fInverse,
                   APalette *pal, int _biBitCount, int pitch = -1);

    void
    Convert32ToPal(BYTE *ptr, int iWidth, int iHeight, BYTE *&new_ptr, int i_width, APalette *_pal,
                   bool fInverse);

protected:
    APalette *palette;

};
#pragma pack(pop)
#endif /* WINBMP_H_ */
