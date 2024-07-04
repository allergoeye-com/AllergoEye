/*
 * ABmp.h
 *
 *  Created on: Aug 27, 2014
 *      Author: alex
 */

#ifndef MBMP_H_
#define MBMP_H_

#include "MemImage.h"

#ifndef WIN32

#include "minstance.h"

#endif

#include "WinBmp.h"

class ADIBSECTION;

#pragma pack(push)
#pragma pack(8)
EXPORTNIX class EXPMDC tagABITMAP : public tagMBITMAP {
public:
    tagABITMAP(tagGDC *);

    tagABITMAP(tagGDC *, int x, int y);

    tagABITMAP(tagGDC *, MArray<DWORD> &data, int x, int y); //data returned empty
    virtual ~tagABITMAP();

    tagABITMAP &Copy(const tagABITMAP &b);

    virtual BOOL SetSize(int cx, int cy, int n = 0);

    virtual DWORD *SetAccess();

    virtual BOOL ReleaseAccess();

private:
    DWORD *m_hBmp;

};
typedef tagABITMAP *ABITMAP;
EXPORTNIX class EXPMDC ABmp : protected WinBmp, public tagGDC {
public:
    ABmp();

    ABmp(int x, int y, APalette *pal = 0);

    ABmp(ABmp &bmp);

    virtual ~ABmp();

    operator ABITMAP()
    { return dynamic_cast<ABITMAP>(surface); }

    int IsHandle()
    { return surface != 0 && type != NONE; }

    HANDLE Handle()
    { return (HANDLE) surface; }

    void Inverce();

    int GetHeight();

    int GetWidth();

    BOOL Load(const char *filename);

    BOOL Save(const char *filename);

    HGLOBAL GetDIB(BOOL b = false)
    { return WinBmp::GetDIB(b); }

    BOOL Load(char *pData, ULONG size)
    { return WinBmp::Load(pData, size); }

    BOOL
    Load(DWORD *pal, int wNumColors, char *lpDibBits, int dwWidth, int dwHeight, int pitch = -1)
    { return WinBmp::Load(pal, wNumColors, lpDibBits, dwWidth, dwHeight, pitch); }

    BOOL LoadDib(char *dib)
    { return WinBmp::CreateBmp(dib); }


    APalette *GetPalette()
    { return WinBmp::GetPalette(); }

    BOOL GetRGB(char *&lpBits, int &len);

    BOOL SetRGB(char *lpBits, int len);

    BOOL SetBitmapBits(char *lpBits, int len)
    { return WinBmp::SetBitmapBits(lpBits, len); }

    BOOL GetBitmapBits(char *&lpBits, int len)
    { return WinBmp::GetBitmapBits(lpBits, len); }

    BOOL Create(int x, int y);

    BOOL SetSize(int x, int y);

    int GetNumColors()
    { return palette && !palette->IsDef() ? palette->GetNumColors() : 0; }

    BOOL Attach(ABITMAP g);

    BOOL Detach();

    virtual void DeletePicture()
    { DeleteImage(); }

    void SetPixelsPerMeter(int)
    {}

public:
    virtual BOOL SetAccess();

    virtual BOOL ReleaseAccess();

protected:
    void DeleteImage();

    virtual DWORD *AttachData(int x, int y, ImageInfo &info);

    virtual BOOL DetachData();

    virtual DWORD *AttachData(ImageInfo &info);

    virtual bool IsAttachedData();

protected:

    int width;
    int height;
    bool fInverse;

    friend class ADIBSECTION;

    friend BOOL GetObject(ABmp *hbitmap, int, ADIBSECTION *);

    friend BOOL GetObject(ABmp *hbitmap, int, BITMAP *);

    friend int
    GetDIBits(void *hdc, ABmp *hbm, int start, int cLines, LPVOID lpvBits, LPBITMAPINFO lpbmi,
              UINT);


};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(2)

EXPORTNIX class EXPMDC ADIBSECTION {
public:
    BITMAP dsBm;
    BITMAPINFOHEADER dsBmih;

    ADIBSECTION()
    { memset(this, 0, sizeof(ADIBSECTION)); }

    ADIBSECTION(ADIBSECTION &b)
    {
        memset(this, 0, sizeof(ADIBSECTION));
        *this = b;
    }

    ADIBSECTION &operator=(ADIBSECTION &b);

    ~ADIBSECTION();

    void UpdateBitmap();

    void Release();

private:
    HGLOBAL dib;
    ABmp *pBmp;

    friend BOOL GetObject(ABmp *hbitmap, int, ADIBSECTION *);
};
#pragma pack(pop)

#ifndef DIB_RGB_COLORS
#define DIB_RGB_COLORS      0 /* color table in RGBs */
#endif
EXPORTNIX int
GetDIBits(void *hdc, ABmp *hbm, int start, int cLines, LPVOID lpvBits, LPBITMAPINFO lpbmi, UINT);

EXPORTNIX BOOL GetObject(ABmp *hbitmap, int, ADIBSECTION *);
EXPORTNIX BOOL GetObject(ABmp *hbitmap, int, BITMAP *db);
#ifndef WIN32
typedef ADIBSECTION DIBSECTION;
#endif
#endif /* MBMP_H_ */
