/*
 * MemImage.h
 *
 *  Created on: Aug 28, 2014
 *      Author: alex
 */

#ifndef MEMIMAGE_H_
#define MEMIMAGE_H_

#include "AColor.h"
#include "mstack.h"
#include "APalette.h"
#include "AFont.h"
#include "ILine.h"

class GDIPath;

#ifdef WIN32
struct WIN32HDC;
#endif
#define WIDTHBYTES(bits)      ((((bits) + 31)>>5)<<2)
#define DIB_HEADER_MARKER   ((unsigned) ('M' << 8) | 'B')

#pragma pack(push)
#pragma pack(8)

EXPORTNIX class EXPMDC MemImage : public AHandle {
public:
    typedef void (MemImage::*ptLineOP)(DWORD *, DWORD *, DWORD);

    typedef void (MemImage::*bmpLineOP)(DWORD *, DWORD *, DWORD, int);

    typedef void (MemImage::*patLineOP)(DWORD *, DWORD, int);

    typedef void (MemImage::*patOP)(DWORD *, DWORD);

    MemImage();

#ifdef WIN32
    HDC CreateHDC();
    void ReleaseHDC();
    HBITMAP CreateHBITMAP();
#endif

    virtual ~MemImage()
    {}

    void GetClientRect(IRect *p)
    { p->Set(0, 0, info.width, info.height); }

    MemImage(DWORD *p, ImageInfo &_info, GPEN fg = 0, GBRUSH bg = 0);

    MemImage(DWORD *p, AndroidBitmapInfo &_info, GPEN fg = 0, GBRUSH bg = 0);

    void Init(DWORD *p, AndroidBitmapInfo &_info, GPEN fg = 0, GBRUSH bg = 0);

    void Init(DWORD *p, ImageInfo &_info, GPEN fg = 0, GBRUSH bg = 0);

    DWORD *GetPtr()
    { return ptr; }

    BOOL Attach(MemImage *img);

    void Detach();

    void Release()
    { Detach(); }

    BOOL GetRGB(MArray<char> &buff, bool fBmp = true, bool Reverse = true);

    BOOL SetRGB(MArray<char> &buff, bool fBmp = true);

    BOOL PtIn(int x, int y)
    { return clip.PtInRegion(curOrg.x + x, curOrg.x + y); }

    ColorRef *operator()(int _x, int _y)
    {
        return ptr ? C(curOrg.x + _x, curOrg.y + _y) : 0;
    }
    ColorRef* Color(int _x, int _y)
    {
        return _x < info.width&& _x > 0 && _y < info.height&& _y >= 0 ? clrs + _y * info.wline + _x : 0;
    }

    IRect GetRect()
    { return IRect(curOrg.x, curOrg.y, curOrg.x + info.width, curOrg.y + info.height); }

    IPoint GetCurrentPosition() const
    { return curPos; }

    void SetClipBox(IRect *rc)
    { if (rc) clip = *rc; else clip.Set(0, 0, info.width, info.height); }

    IRect GetClipBox() const
    { return clip; }

public :
    void MedianSmooth(int w);

    void FillRects(MArray<POINT> &org, int w, int h, COLORREF clr);

    bool DrawText(LPCSTR lpchText, int cchText, IRect *lprc, UINT format, MVector<MArray<IPoint> >* out = 0);

    bool TextOut(int x, int y, LPCSTR lpString, int len, MVector<MArray<IPoint> >* out = 0);

    IRect GetStringRect(LPCSTR str, int len = 0);

    void TileWithMask(int *mask, IRect &rc, int offset);

    void Frame(IPoint *pp1, int len1, IPoint *pp2, int len2);

    bool DrawText(const wchar_t *lpchText, int cchText, IRect *lprc, UINT format, MVector<MArray<IPoint> >* out = 0);

    bool TextOut(int x, int y, const wchar_t *lpString, int len, MVector<MArray<IPoint> >* out = 0);

    IRect GetStringRect(const wchar_t *str, int len = 0);

    CSize GetTextExtent(LPCSTR s)
    {
        IRect rc = GetStringRect(s);
        return CSize(rc.Width(), rc.Height());
    }

    void Draw3dRect(IRect *lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight, int level = 1);

    COLORREF SetTextColor(COLORREF c)
    {
        COLORREF old = text_color & 0x00FFFFFF;
        text_color = c | 0xFF000000;
        return old;
    }

    DWORD
    GetGlyphOutline(UINT uChar, UINT fuFormat, LPGLYPHMETRICS lpgm, DWORD cjBuffer, LPVOID pvBuffer,
                    MAT2 *lpmat)
    {
        return font ? font->getGlyphOutline(uChar, fuFormat, lpgm, cjBuffer, pvBuffer, lpmat) : -1;
    }

    DWORD GetKerningPairs(DWORD cPairs, KERNINGPAIR *kern_pair)
    {
        return font ? font->GetKerningPairs(cPairs, kern_pair) : -1;

    }

    void MedianSmoothLine(int w, MArray<POINT> &arr);

    AFont *CurrentFont()
    { return font; }

public :
    int SetROP2(int rop)
    {
        int r = rop2;
        rop2 = rop;
        iPixel.SetRop(rop2);
        return r;
    }

    int GetROP2()
    { return rop2; }

    int SetBkMode(int bk)
    {
        int b = bkmode;
        bkmode = bk;
        return b;
    }

    COLORREF SetBkColor(COLORREF bk)
    {
        int b = bkcolor;
        bkcolor = bkmode == TRANSPARENT ? (bk & 0xFF000000) : (bk | 0xFF000000);
        return (b & 0x00FFFFFF);
    }

    int SetPolyFillMode(int flg)
    { return flg; }

public :
    GPEN SelectObject(GPEN h);

    AFONT SelectObject(AFONT h);

    AHandle *SelectObject(AHandle *h);

    GPEN SetPen(COLORREF c, int w, int at)
    {
        GPEN r = pen;
        t_pen.SetPen(c, w, at);
        pen = &t_pen;
        return r;
    }

    GPEN SetFGColor(COLORREF c)
    {
        GPEN r = pen;
        t_pen.SetColor(c);
        pen = &t_pen;
        return r;
    }

    GPEN GetCurPen()
    { return pen; }

    GBRUSH SelectObject(GBRUSH h);

    GBRUSH SetBgColor(COLORREF c)
    {
        GBRUSH r = brush;
        t_brush.SetColor(c);
        brush = &t_brush;
        return r;
    }

    GBRUSH SetBgColor(COLORREF c, int alpha)
    {
        GBRUSH r = brush;
        t_brush.SetColor(c, alpha);
        brush = &t_brush;
        return r;
    }

    GBRUSH GetCurBrush()
    { return brush; }

    void UnSelect(AHandle *h);

public :
    //***********************************************************************************************************
    // size min(rc_dst1.Height() rc_dst2.Height()) size min(rc_dst1.Width() rc_dst2.Width())
    void Stretch(MemImage &src1, MemImage &src2, IRect &rc_src, IRect &rc_dst, IPoint &dst_org1,
                 IPoint &dst_org2);
    //***********************************************************************************************************

    void Show(MemImage &i, int x, int y, int rop = SRCCOPY);

    void Show(MemImage &i, IRect &rc_dst, int rop = SRCCOPY);

    void Show(MemImage &i, IRect &rc_src, IRect &rc_dst, int rop = SRCCOPY);

    void Stretch(MemImage &i, IRect &rc_src, IRect &rc_dst, int rop = SRCCOPY);

    void Stretch(MemImage &src, IRect &rc_dst, int rop = SRCCOPY);
    void Stretch(AObject *i, IRect &rc_src, IRect &rc_dst, int rop = SRCCOPY);

    void SetTransparentColor(COLORREF clr);

    void Show(MemImage &i, int x, int y, IRect *rc_src, int rop = SRCCOPY)
    {
        if (rc_src)
        {
            IRect rc_dst(x, y, info.width, info.height);
            Show(i, *rc_src, rc_dst, rop);
        } else
            Show(i, x, y, rop);

    }

    void TransparentBlt(MemImage &i, int x, int y, COLORREF crTransparent)
    {
        IRect rc_src(0, 0, i.info.width, i.info.height);
        IRect rc_dst(x, y, info.width, info.height);
        TransparentBlt(i, rc_src, rc_dst, crTransparent);

    }

    void FillSurface(IPoint *pp, int len, MArray<COLORREF> *dst, int number);

    BOOL TransparentBlt(MemImage &i, IRect &rc_src, IRect &rc_dst, COLORREF crTransparent);

    BOOL ShowWithMask(MemImage &i, MemImage &mask, IRect &_rc_src, IRect &_rc_dst, COLORREF iMask,
                      COLORREF src, COLORREF dst);

    void Scroll(int dx, int dy);

    void Line(int x1, int y1, int x2, int y2);

    void PolyLine(IPoint *p, int len);

    void Clear();

    void Clear(COLORREF clr);

    void FillRect(IRect &rc, ABGColor &bg);

    void FillSolidRect(IRect &rc, COLORREF bg)
    {
        ABGColor b(bg | 0xFF000000);
        FillRect(rc, b);
    }

    void FillSolidRect(int left, int top, int width, int height, COLORREF bg)
    {
        ABGColor b(bg | 0xFF000000);
        IRect rc(left, top, left + width, top + height);
        FillRect(rc, b);
    }

    void Bar(int x1, int y1, int x2, int y2)
    {
        IRect rc(x1, y1, x2, y2);
        FillRect(rc, *brush);
    }

    void Rectangle(MArray<IRect> &rc);

    void Rectangle(IRect &rc, bool fFill = true);

    void Rectangle(int left, int top, int right, int bottom, bool fFill = true)
    {
        IRect r(left, top, right, bottom);
        Rectangle(r, fFill);
    }

    void Rectangles(IRect *rc, int num);

    void SetPixel(int x, int y, COLORREF);

    void SetPixel(int x, int y, ColorRef &c);

    COLORREF GetPixel(int x, int y);

    void MoveToEx(int x, int y, IPoint *oldposPOLYGON);

    void LineTo(int x, int y)
    {
        Line(curPos.x, curPos.y, x, y);
        curPos.Set(x, y);
    }

    void Ellipse(int left, int top, int right, int bottom, bool fFill = true)
    {
        IRect r(left, top, right, bottom);
        Ellipse(r, fFill);
    }

    void Ellipse(IRect &rc, bool fFill = true);

    void Polygon(IPoint *p, int len, bool fFill = false);

    void PolygonConvex(IPoint *pp, int len, bool fFill = false);

    void PatBlt(IRect &rc, int rop);

    void Rect(int x1, int y1, int x2, int y2);

    void Rect(IRect &r)
    { Rect(r.left, r.top, r.right, r.bottom); }

    void PolyBezier(IPoint *p, int count, bool fFill = false);

    void PolyPolygon(int n, int *count, IPoint *points);

    BOOL SetViewportOrg(int x, int y, IPoint *old)
    {
        if (old) old->Set(curOrg.x, curOrg.x);
        curOrg.Set(x, y);
        return true;
    }

    void RoundRect(int x, int y, int width, int height, int d);

    int Fill(int x, int y, COLORREF c, IRect *rc = 0);

    void FillSurfaceBorder(int x, int y, COLORREF c);

public :
    BOOL BeginPath();

    BOOL EndPath();

    BOOL StrokePath();

    BOOL StrokeAndFillPath();

    BOOL FillPath();

    BOOL AttachPath(GDIPath *p);

    GDIPath *DetachPath();

    BOOL SetPrintPath(GDIPath *p);

    virtual BOOL SetAccess()
    { return ptr != 0; };

    virtual BOOL ReleaseAccess()
    { return ptr != 0; };

    void _Ellipse(IRect &rc, MArray<IPoint> &cp);

    void SetNumberOfProcessors(int i)
    { numberOfProcessors = i; }

protected:
    void PathBezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4,
                    MArray<IPoint> &pt);

    void Tile(IRect &rc, int offset);

    void _RoundRect(int x, int y, int width, int height, int radius, MArray<IPoint> &cp);

    void PutText(int left, int top, int right, int bottom, char **list, int num_str, int flg, MVector<MArray<IPoint> >* out);

    void PutText(int left, int top, int right, int bottom, wchar_t **list, int num_str, int flg, MVector<MArray<IPoint> >* out);

    int Fill(MFifo<IPoint> &fifo, int _x, int _y, COLORREF srcColor, COLORREF dstColor, IRect *rc = 0);

    void _FillRect(IRect &rc, ABGColor &bg);

    int NumberOfProcessors();


//	void Fill(IRect &rc, MFifo<IPoint> &fifo, BYTE *ord, int x, int y);
    void SetBmpRop(UINT p);

    void bmpBLACKNESSLine(DWORD *dst, DWORD *src, DWORD brush, int len);

    void bmpDSTINVERTLine(DWORD *dst, DWORD *src, DWORD brush, int len);

    void bmpMERGECOPYLine(DWORD *dst, DWORD *src, DWORD brush, int len);

    void bmpMERGEPAINTLine(DWORD *dst, DWORD *src, DWORD brush, int len);

    void bmpNOTSRCCOPYLine(DWORD *dst, DWORD *src, DWORD brush, int len);

    void bmpNOTSRCERASELine(DWORD *dst, DWORD *src, DWORD brush, int len);

    void bmpPATCOPYLine(DWORD *dst, DWORD *src, DWORD brush, int len);

    void bmpPATINVERTLine(DWORD *dst, DWORD *src, DWORD brush, int len);

    void bmpPATPAINTLine(DWORD *dst, DWORD *src, DWORD brush, int len);

    void bmpSRCANDLine(DWORD *dst, DWORD *src, DWORD brush, int len);

    void bmpSRCCOPYLine(DWORD *dst, DWORD *src, DWORD brush, int len);

    void bmpSRCERASELine(DWORD *dst, DWORD *src, DWORD brush, int len);

    void bmpSRCINVERTLine(DWORD *dst, DWORD *src, DWORD brush, int len);

    void bmpSRCPAINTLine(DWORD *dst, DWORD *src, DWORD brush, int len);

    void bmpWHITENESSLine(DWORD *dst, DWORD *src, DWORD brush, int len);

    void bmpBLACKNESS(DWORD *dst, DWORD *src, DWORD brush);

    void bmpDSTINVERT(DWORD *dst, DWORD *src, DWORD brush);

    void bmpMERGECOPY(DWORD *dst, DWORD *src, DWORD brush);

    void bmpMERGEPAINT(DWORD *dst, DWORD *src, DWORD brush);

    void bmpNOTSRCCOPY(DWORD *dst, DWORD *src, DWORD brush);

    void bmpNOTSRCERASE(DWORD *dst, DWORD *src, DWORD brush);

    void bmpPATCOPY(DWORD *dst, DWORD *src, DWORD brush);

    void bmpPATINVERT(DWORD *dst, DWORD *src, DWORD brush);

    void bmpPATPAINT(DWORD *dst, DWORD *src, DWORD brush);

    void bmpSRCAND(DWORD *dst, DWORD *src, DWORD brush);

    void bmpSRCCOPY(DWORD *dst, DWORD *src, DWORD brush);

    void bmpSRCERASE(DWORD *dst, DWORD *src, DWORD brush);

    void bmpSRCINVERT(DWORD *dst, DWORD *src, DWORD brush);

    void bmpSRCPAINT(DWORD *dst, DWORD *src, DWORD brush);

    void bmpWHITENESS(DWORD *dst, DWORD *src, DWORD brush);


    void patBLACKNESSLine(DWORD *dst, DWORD brush, int len);

    void patDSTINVERTLine(DWORD *dst, DWORD brush, int len);

    void patPATCOPYLine(DWORD *dst, DWORD brush, int len);

    void patPATINVERTLine(DWORD *dst, DWORD brush, int len);

    void patWHITENESSLine(DWORD *dst, DWORD brush, int len);

    void patBLACKNESS(DWORD *dst, DWORD);

    void patDSTINVERT(DWORD *dst, DWORD);

    void patWHITENESS(DWORD *dst, DWORD);

    void patPATINVERT(DWORD *dst, DWORD brush);

    void patPATCOPY(DWORD *dst, DWORD brush);

    void _PolyLine(IPoint *pt, int len);

    void __PolyLine(IPoint *pt, int len);

public:
    BOOL IsAccess()
    { return ptr != 0; }

    ColorRef *Image(int _x, int _y)
    {
        if (_x >= info.width) _x = info.width - 1;
        if (_x < 0) _x = 0;
        if (_y >= info.height) _y = info.height - 1;
        if (_y < 0) _y = 0;
        return clrs ? clrs + _y * info.wline + _x : 0;
    }

protected:
    ColorRef *C(int _x, int _y)
    {
        if (_x >= info.width) _x = info.width - 1;
        if (_x < 0) _x = 0;
        if (_y >= info.height) _y = info.height - 1;
        if (_y < 0) _y = 0;
        return clrs + _y * info.wline + _x;
    }

public:
    ImageInfo info;
    union {
        DWORD *ptr;
        ColorRef *clrs;
    };
protected:
    int numberOfProcessors;
    IPoint curPos;
    IPoint curOrg;
    IPixel iPixel;
    GPEN pen;
    GDIPath *pGDIPath, *pGDIPrint;
    GBRUSH brush;
    tagGPEN t_pen;
    tagGBRUSH t_brush;
    COLORREF text_color;
    AFONT font;
    IRect clip;
    MArray<BYTE> mask;
#ifdef WIN32
    WIN32HDC *hDC;
#endif

    DWORD rop2;
    int bkmode;
    DWORD bkcolor;

    bmpLineOP _bmpLineOP;
    patLineOP _patLineOP;
    ptLineOP _ptLineOP;
    patOP _patOP;
    union Color4Side {
        struct {
            ColorRef s0;
            ColorRef s1;
            ColorRef s2;
            ColorRef s3;
        };
        ColorRef s[4];
    };

    /**************
    0 ------ 1
    |        |
    |        |
    |        |
    2 ------ 3
    ************/

    struct LinearBox {
        Color4Side Val;
        IPoint p[4];
        Vector2F minPoint;
        Vector2F maxPoint;

        LinearBox()
        {
            p[0].x = -1;
            p[0].y = -1;
            p[1].x = 1;
            p[1].y = -1;
            p[2].x = -1;
            p[2].y = 1;
            p[3].x = 1;
            p[3].y = 1;
        }
    };

    void Stretch1(Transform2F<Vector2F, IRect, float> &trans, ptLineOP op, DWORD *dst,
                  ImageInfo &info_dst, IRect &rc_dst, DWORD *src, ImageInfo &info_src,
                  IRect &rc_src, int start, int num);

    void Stretch2(Transform2F<Vector2F, IRect, float> &trans, ptLineOP op, DWORD *dst,
                  ImageInfo &info_dst, IRect &rc_dst, DWORD *src, ImageInfo &info_src,
                  IRect &rc_src, int start, int num);

    void Paste(bmpLineOP op, DWORD *dst, ImageInfo &info_dst, IRect &rc_dst, DWORD *src,
               ImageInfo &info_src, IRect &rc_src, GBRUSH bg, int start, int num);

    void Clear(DWORD *dst, DWORD *line, ImageInfo &info, int start, int num);

    static void
    Stretch_1(Transform2F<Vector2F, IRect, float> &trans, DWORD *dst, IPoint &org1, IPoint &org2,
              ImageInfo &info_dst, IRect &rc_dst, DWORD *src1, DWORD *src2, ImageInfo &info_src,
              IRect &rc_src, int start, int num);

    static void
    Stretch_2(Transform2F<Vector2F, IRect, float> &trans, DWORD *dst, IPoint &org1, IPoint &org2,
              ImageInfo &info_dst, IRect &rc_dst, DWORD *src1, DWORD *src2, ImageInfo &info_src,
              IRect &rc_src, int start, int num);

    friend void *ThreadStretch1(void *p);

    friend void *ThreadStretch2(void *p);

    friend void *ThreadStretch_1(void *p);

    friend void *ThreadStretch_2(void *p);

    friend void *ThreadPaste(void *p);

    friend void *ThreadClear(void *p);

    friend void *ThreadFillRect(void *p);

    friend void *ThreadPolyLine(void *p);


    friend struct tagMBITMAP;
    friend struct ThreadParam;


};
#ifndef GDC
EXPORTNIX class EXPMDC tagGDC : public MemImage {
public :
    tagGDC()
    {
        fTemp = 0;
        type = AGDC;
        surface = 0;
    }

    virtual ~tagGDC()
    {
        if (surface) surface->OnDeleteContext();
        surface = 0;
    }

    virtual void OnDeleteSurface()
    { surface = 0; }

    virtual BOOL SetAccess()
    { return surface ? surface->SetAccess() != 0 : MemImage::SetAccess(); };

    virtual BOOL ReleaseAccess()
    {
        if (surface)
        {
            surface->ReleaseAccess();
            if (fTemp && !surface->count) delete this;
            return true;
        }

        return MemImage::ReleaseAccess();


    };

    MBITMAP surface;
    bool fTemp;
};

#include "GDIPath.h"

typedef tagGDC *GDC;
typedef GDIPath *GENHMETAFILE;
#ifndef WIN32
#define HENHMETAFILE GENHMETAFILE
#else
struct WIN32HDC{
    HDC hDC, hOldDC;
    HBITMAP hbmp, hOldBmp;
    MArray<char> data;
    HRGN rgn;
    BOOL fKillRgn;
    MemImage *memImage;
    WIN32HDC(MemImage *in, HRGN r = 0);
    ~WIN32HDC();

};
#endif

inline GDC WINAPI CreateEnhMetaFile(GDC a, LPCSTR, IRect *rc, IN LPCSTR)
{
    tagGDC *dc = new tagGDC();
    GDIPath *pGDIPath = new GDIPath();
    pGDIPath->init_rc = *rc;
    dc->AttachPath(pGDIPath);
    return dc;

}

inline GENHMETAFILE WINAPI CloseEnhMetaFile(GDC dc)
{
    if (dc)
    {
        GDIPath *pGDIPath = dc->DetachPath();
        delete dc;
        return pGDIPath;
    }
    return 0;
}

inline BOOL  WINAPI DeleteEnhMetaFile(GENHMETAFILE a)
{
    if (a)
        delete a;
    return true;
}

#endif
#pragma pack(pop)

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpBLACKNESSLine(DWORD *dst, DWORD *src, DWORD brush, int len)
{
    while (len--)
    {
        *dst &= 0xFF000000;
        ++dst;
    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpDSTINVERTLine(DWORD *dst, DWORD *src, DWORD brush, int len)
{
    while (len--)
    {
        *dst = ~(*dst & 0x00FFFFFF) | (*dst & 0xFF000000);
        ++dst;

    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpMERGECOPYLine(DWORD *dst, DWORD *src, DWORD brush, int len)
{
    while (len--)
    {
        *dst = (*dst & 0xFF000000) | ((*src & 0x00FFFFFF) & brush);
        ++dst;
        ++src;

    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpMERGEPAINTLine(DWORD *dst, DWORD *src, DWORD brush, int len)
{
    while (len--)
    {
        *dst = (*dst & 0xFF000000) | ((*src & 0x00FFFFFF) | brush);
        ++dst;
        ++src;

    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpNOTSRCCOPYLine(DWORD *dst, DWORD *src, DWORD brush, int len)
{
    while (len--)
    {
        *dst = (*dst & 0xFF000000) | ~(*src & 0x00FFFFFF);
        ++dst;
        ++src;

    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpNOTSRCERASELine(DWORD *dst, DWORD *src, DWORD brush, int len)
{
    while (len--)
    {
        *dst = ~((*src & 0x00FFFFFF) | brush) | (*dst & 0xFF000000);
        ++dst;
        ++src;

    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpPATCOPYLine(DWORD *dst, DWORD *src, DWORD brush, int len)
{
    while (len--)
    {
        *dst = brush;
        ++dst;

    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpPATINVERTLine(DWORD *dst, DWORD *src, DWORD brush, int len)
{
    while (len--)
    {
        *dst = ((*dst ^ brush) & 0x00FFFFFF) | (*dst & 0xFF000000);
        ++dst;

    }

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpPATPAINTLine(DWORD *dst, DWORD *src, DWORD brush, int len)
{
    while (len--)
    {
        *dst |= (~(*src) & 0x00FFFFFF) | brush;
        ++dst;
        ++src;

    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpSRCANDLine(DWORD *dst, DWORD *src, DWORD brush, int len)
{
    while (len--)
    {
        *dst &= (*src & 0x00FFFFFF) | (*dst & 0xFF000000);
        ++dst;
        ++src;

    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpSRCCOPYLine(DWORD *dst, DWORD *src, DWORD brush, int len)
{
    memcpy(dst, src, len * sizeof(DWORD));
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpSRCERASELine(DWORD *dst, DWORD *src, DWORD brush, int len)
{
    while (len--)
    {
        *dst = ((*src & 0x00FFFFFF) & ~(*dst & 0x00FFFFFF)) | (*dst & 0xFF000000);
        ++dst;
        ++src;

    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpSRCINVERTLine(DWORD *dst, DWORD *src, DWORD brush, int len)
{
    while (len--)
    {
        *dst = ((*src & 0x00FFFFFF) ^ (*dst & 0x00FFFFFF)) | (*dst & 0xFF000000);
        ++dst;
        ++src;

    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpSRCPAINTLine(DWORD *dst, DWORD *src, DWORD brush, int len)
{
    while (len--)
    {
        *dst = ((*src & 0x00FFFFFF) | (*dst & 0x00FFFFFF)) | (*dst & 0xFF000000);
        ++dst;
        ++src;

    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpWHITENESSLine(DWORD *dst, DWORD *src, DWORD brush, int len)
{
    while (len--)
    {
        *dst |= 0xFFFFFFFF;
        ++dst;
    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::patBLACKNESSLine(DWORD *dst, DWORD brush, int len)
{
    if ((brush & 0xFF000000))
        while (len--)
        {
            *dst &= 0xFF000000;
            ++dst;
        }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::patDSTINVERTLine(DWORD *dst, DWORD brush, int len)
{

    if ((brush & 0xFF000000))
        while (len--)
        {
            *dst = ((~*dst) & 0x00FFFFFF) | (*dst & 0xFF000000);
            ++dst;

        }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::patWHITENESSLine(DWORD *dst, DWORD brush, int len)
{
    if ((brush & 0xFF000000))
        while (len--)
        {
            *dst |= 0xFFFFFFFF;
            ++dst;
        }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::patPATINVERTLine(DWORD *dst, DWORD brush, int len)
{
    if ((brush & 0xFF000000))
        while (len--)
        {
            *dst = ((*dst ^ brush) & 0x00FFFFFF) | (*dst & 0xFF000000);
            ++dst;
        }

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::patPATCOPYLine(DWORD *dst, DWORD brush, int len)
{
    if ((brush & 0xFF000000))
        while (len--)
        {
            *dst = (brush & 0x00FFFFFF) | (*dst & 0xFF000000);
            ++dst;

        }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::patBLACKNESS(DWORD *dst, DWORD brush)
{
    if ((brush & 0xFF000000))
        *dst &= 0xFF000000;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::patDSTINVERT(DWORD *dst, DWORD brush)
{
    if ((brush & 0xFF000000))
        *dst = ((~*dst) & 0x00FFFFFF) | (*dst & 0xFF000000);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::patWHITENESS(DWORD *dst, DWORD brush)
{
    if ((brush & 0xFF000000))
        *dst |= 0xFFFFFFFF;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::patPATINVERT(DWORD *dst, DWORD brush)
{
    if ((brush & 0xFF000000))
        *dst = ((*dst ^ brush) & 0x00FFFFFF) | (*dst & 0xFF000000);

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::patPATCOPY(DWORD *dst, DWORD brush)
{
    if ((brush & 0xFF000000))
        *dst = (brush & 0x00FFFFFF) | (*dst & 0xFF000000);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpBLACKNESS(DWORD *dst, DWORD *src, DWORD)
{
    *dst &= 0xFF000000;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpDSTINVERT(DWORD *dst, DWORD *src, DWORD)
{
    *dst = ((~*dst) & 0x00FFFFFF) | (*dst & 0xFF000000);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpMERGECOPY(DWORD *dst, DWORD *src, DWORD brush)
{
    *dst = (*dst & 0xFF000000) | ((*src & 0x00FFFFFF) & brush);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpMERGEPAINT(DWORD *dst, DWORD *src, DWORD brush)
{
    *dst = (*dst & 0xFF000000) | ((*src & 0x00FFFFFF) | brush);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpNOTSRCCOPY(DWORD *dst, DWORD *src, DWORD brush)
{
    *dst = (*dst & 0xFF000000) | ~(*src & 0x00FFFFFF);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpNOTSRCERASE(DWORD *dst, DWORD *src, DWORD brush)
{
    *dst = ~((*src & 0x00FFFFFF) | brush) | (*dst & 0xFF000000);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpPATCOPY(DWORD *dst, DWORD *src, DWORD brush)
{
    *dst = brush | (*dst & 0xFF000000);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpPATINVERT(DWORD *dst, DWORD *src, DWORD brush)
{
    *dst = ((*dst ^ brush) & 0x00FFFFFF) | (*dst & 0xFF000000);

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpPATPAINT(DWORD *dst, DWORD *src, DWORD brush)
{
    *dst |= (~(*src) & 0x00FFFFFF) | brush;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpSRCAND(DWORD *dst, DWORD *src, DWORD brush)
{
    *dst &= (*src & 0x00FFFFFF) | (*dst & 0xFF000000);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpSRCCOPY(DWORD *dst, DWORD *src, DWORD brush)
{
    *dst = *src;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpSRCERASE(DWORD *dst, DWORD *src, DWORD brush)
{
    *dst = ((*src & 0x00FFFFFF) & ~(*dst & 0x00FFFFFF)) | (*dst & 0xFF000000);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpSRCINVERT(DWORD *dst, DWORD *src, DWORD brush)
{
    *dst = ((*src & 0x00FFFFFF) ^ (*dst & 0x00FFFFFF)) | (*dst & 0xFF000000);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpSRCPAINT(DWORD *dst, DWORD *src, DWORD brush)
{
    *dst = ((*src & 0x00FFFFFF) | (*dst & 0x00FFFFFF)) | (*dst & 0xFF000000);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
inline void MemImage::bmpWHITENESS(DWORD *dst, DWORD *src, DWORD brush)
{
    *dst |= 0xFFFFFFFF;
}

#endif /* MEMIMAGE_H_ */
