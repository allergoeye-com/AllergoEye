/*
 * MemImage.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: alex
 */

#include "stdafx.h"
#include "MemImage.h"
#include "ILine.h"
#include "Vector2F.h"
#include "ABmp.h"
#include "mstack.h"
#include "AClip.h"
#include "GDIPath.h"
#include "ADC.h"
#include "MultiThread.h"


extern MultiThreadS *pMultiThreads;

#define INFLATE_RECT_FONT 0

struct ThreadParam {
    DWORD *src, *src1;
    ImageInfo info_src;
    DWORD *dst, *dst2;
    ImageInfo info_dst;
    IRect rc_src;
    IRect rc_dst;
    IRect clip;
    MemImage *img;
    GBRUSH bg;
    GPEN fg;
    int start;
    int num;
    IPoint org1, org2;
    MemImage::bmpLineOP _bmpLineOP;
    MemImage::patLineOP _patLineOP;
    MemImage::ptLineOP _ptLineOP;
    Transform2F<Vector2F, IRect, float> trans;

    //---------------------------------------------------------------
    //
    //---------------------------------------------------------------
    void InitStrech(Transform2F<Vector2F, IRect, float> &_trans, MemImage *_dst, IRect &_rc_dst,
                    MemImage *_src, IRect &_rc_src, int _start, int end)
    {
        trans = _trans;
        img = _dst;
        _ptLineOP = _dst->_ptLineOP;
        dst = _dst->ptr;
        info_dst = _dst->info;
        rc_dst = _rc_dst;
        rc_src = _rc_src;
        src = _src->ptr;
        info_src = _src->info;
        bg = _dst->brush;
        start = _start;
        num = end;
    }

    //---------------------------------------------------------------
    //
    //---------------------------------------------------------------
    void InitStrech(Transform2F<Vector2F, IRect, float> &_trans, MemImage *_dst, IPoint &_org1,
                    IPoint &_org2, IRect &_rc_dst, MemImage *_src1, MemImage *_src2, IRect &_rc_src,
                    int _start, int end)
    {
        trans = _trans;
        img = 0;
        org1 = _org1;
        org2 = _org2;
        dst = _dst->ptr;

        info_dst = _dst->info;
        rc_dst = _rc_dst;
        rc_src = _rc_src;
        src = _src1->ptr;
        src1 = _src2->ptr;
        info_src = _src1->info;
        start = _start;
        num = end;
    }

    //---------------------------------------------------------------
    //
    //---------------------------------------------------------------
    void
    InitPaste(MemImage *_dst, IRect &_rc_dst, MemImage *_src, IRect &_rc_src, int _start, int end)
    {
        img = _dst;
        _bmpLineOP = _dst->_bmpLineOP;
        dst = _dst->ptr;
        info_dst = _dst->info;
        rc_dst = _rc_dst;
        rc_src = _rc_src;
        src = _src->ptr;
        info_src = _src->info;
        bg = _dst->brush;
        start = _start;
        num = end - start;
    }

    //---------------------------------------------------------------
    //
    //---------------------------------------------------------------
    void InitClear(MemImage *_dst, DWORD *line, int _start, int end)
    {
        img = _dst;
        _bmpLineOP = _dst->_bmpLineOP;
        dst = _dst->ptr;
        src = line;
        info_dst = _dst->info;
        start = _start;
        num = end - start;
    }

    //---------------------------------------------------------------
    //
    //---------------------------------------------------------------
    void InitFillRect(MemImage *_dst, IRect &rc, ABGColor *b)
    {
        img = _dst;
        info_dst = _dst->info;
        rc_dst = rc;
        bg = b;
    }

};

//---------------------------------------------------------------
//
//---------------------------------------------------------------
struct ParamPolyLine {
    IPoint *pp;
    int len;
    MemImage *img;

    void Init(MemImage *_img, IPoint *_pp, int _len)
    {
        img = _img;
        pp = _pp;
        len = _len;
    }
};

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
void *ThreadPolyLine(void *p)
{
    ParamPolyLine *param = (ParamPolyLine *) p;
    param->img->__PolyLine(param->pp, param->len);
    return 0;
}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
void *ThreadFillRect(void *p)
{
    ThreadParam *param = (ThreadParam *) p;
    param->img->_FillRect(param->rc_dst, *param->bg);
    return 0;
}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
void *ThreadPaste(void *p)
{
    ThreadParam *param = (ThreadParam *) p;
    param->img->Paste(param->_bmpLineOP, param->dst, param->info_dst, param->rc_dst, param->src,
                      param->info_src, param->rc_src, param->bg, param->start, param->num);
    return 0;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void *ThreadStretch2(void *p)
{
    ThreadParam *param = (ThreadParam *) p;
    param->img->Stretch2(param->trans, param->_ptLineOP, param->dst, param->info_dst, param->rc_dst,
                         param->src, param->info_src, param->rc_src, param->start, param->num);
    return 0;

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void *ThreadStretch1(void *p)
{
    ThreadParam *param = (ThreadParam *) p;
    param->img->Stretch1(param->trans, param->_ptLineOP, param->dst, param->info_dst, param->rc_dst,
                         param->src, param->info_src, param->rc_src, param->start, param->num);
    return 0;

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void *ThreadStretch_2(void *p)
{
    ThreadParam *param = (ThreadParam *) p;
    if (!param->img)
        MemImage::Stretch_2(param->trans, param->dst, param->org1, param->org2, param->info_dst,
                            param->rc_dst, param->src, param->src1, param->info_src, param->rc_src,
                            param->start, param->num);
    return 0;

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void *ThreadStretch_1(void *p)
{
    ThreadParam *param = (ThreadParam *) p;
    if (!param->img)
        MemImage::Stretch_1(param->trans, param->dst, param->org1, param->org2, param->info_dst,
                            param->rc_dst, param->src, param->src1, param->info_src, param->rc_src,
                            param->start, param->num);
    return 0;

}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
void *ThreadClear(void *p)
{
    ThreadParam *param = (ThreadParam *) p;
    param->img->Clear(param->dst, param->src, param->info_dst, param->start, param->num);
    return 0;
}

MemImage::MemImage() : AHandle(MEMIMAGE)
{
    numberOfProcessors = -1;
    text_color = 0xFF000000;
    font = 0;
    bkmode = OPAQUE;
    pGDIPath = 0;
    pGDIPrint = 0;
    _patOP = &MemImage::patPATCOPY;
    _ptLineOP = &MemImage::bmpSRCCOPY;
    _bmpLineOP = &MemImage::bmpSRCCOPYLine;
    _patLineOP = &MemImage::patPATCOPYLine;
    ptr = 0;
    pen = &t_pen;
    brush = &t_brush;
    rop2 = R2_COPYPEN;
    bkcolor = 0xFFFFFFFF;
    iPixel.SetRop(rop2);
    clip.Set(0, 0, 0, 0);
    SetBmpRop(SRCCOPY);
    //font = (AFont *)GetStockObject(SYSTEM_FONT);
    handle = (HANDLE) this;
#ifdef WIN32
    hDC = 0;
#endif

}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
MemImage::MemImage(DWORD *p, ImageInfo &_info, GPEN fg, GBRUSH bg) : AHandle(AGDC)
{
#ifdef WIN32
    hDC = 0;
#endif
    numberOfProcessors = -1;
    text_color = 0xFF000000;
    pGDIPath = pGDIPrint = 0;
    text_color = 0;
    SetBmpRop(SRCCOPY);
    font = (AFont *) GetStockObject(SYSTEM_FONT);
    info = _info;
    ptr = p;
    pen = &t_pen;
    brush = &t_brush;
    if (fg) *pen = *fg;
    if (bg) *brush = *bg;
    rop2 = R2_COPYPEN;
    bkcolor = 0xFFFFFFFF;
    clip.Set(0, 0, info.width, info.height);
    handle = (HANDLE) this;
    iPixel.SetRop(rop2);
}

//----------------------------------------------------------------
//
//----------------------------------------------------------------
MemImage::MemImage(DWORD *p, AndroidBitmapInfo &_info, GPEN fg, GBRUSH bg) : AHandle(AGDC)
{
#ifdef WIN32
    hDC = 0;
#endif
    numberOfProcessors = -1;

    text_color = 0xFF000000;
    font = (AFont *) GetStockObject(SYSTEM_FONT);
    pGDIPath = pGDIPrint = 0;
    SetBmpRop(SRCCOPY);
    info = _info;
    ptr = p;
    pen = &t_pen;
    brush = &t_brush;
    if (fg) *pen = *fg;
    if (bg) *brush = *bg;
    rop2 = R2_COPYPEN;
    bkcolor = 0xFFFFFFFF;
    clip.Set(0, 0, info.width, info.height);
    handle = (HANDLE) this;
    iPixel.SetRop(rop2);
}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
void MemImage::Init(DWORD *p, AndroidBitmapInfo &_info, GPEN fg, GBRUSH bg)
{
    SetBmpRop(SRCCOPY);
    info = _info;
    font = (AFont *) GetStockObject(SYSTEM_FONT);

    ptr = p;
    if (fg) *pen = *fg;
    if (bg) *brush = *bg;
    rop2 = R2_COPYPEN;
    bkcolor = 0xFFFFFFFF;
    clip.Set(0, 0, info.width, info.height);
}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
void MemImage::Init(DWORD *p, ImageInfo &_info, GPEN fg, GBRUSH bg)
{
    SetBmpRop(SRCCOPY);
    font = (AFont *) GetStockObject(SYSTEM_FONT);

    info = _info;
    ptr = p;
    if (fg) *pen = *fg;
    if (bg) *brush = *bg;
    rop2 = R2_COPYPEN;
    bkcolor = 0xFFFFFFFF;
    clip.Set(0, 0, info.width, info.height);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
BOOL MemImage::Attach(MemImage *img)
{
    if (!img->SetAccess()) return FALSE;
    info = img->info;
    ptr = img->ptr;
    if (img->pen) *pen = *img->pen;
    if (img->brush) *brush = *img->brush;
    rop2 = R2_COPYPEN;
    bkcolor = 0xFFFFFFFF;
    clip = img->clip;
    font = img->font;
    return TRUE;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Detach()
{
    ptr = 0;
    font = (AFont *) GetStockObject(SYSTEM_FONT);
    pen = &t_pen;
    brush = &t_brush;
    rop2 = R2_COPYPEN;
    bkcolor = 0xFFFFFFFF;
}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
void MemImage::Paste(bmpLineOP op, DWORD *dst, ImageInfo &info_dst, IRect &rc_dst, DWORD *src,
                     ImageInfo &info_src, IRect &rc_src, GBRUSH bg, int start, int num)
{
    if (!ptr) return;

    DWORD *_src = src + info_src.wline * (rc_src.top + start) + rc_src.left;
    DWORD *_dst = dst + info_dst.wline * (rc_dst.top + start) + rc_dst.left;
    int _w = min(rc_src.Width(), rc_dst.Width());
    DWORD clr = bg->a ? bg->color & 0xFFFFFFFF : 0x00FFFFFF;
    while (num--)
    {
        (this->*op)(_dst, _src, clr, _w);
        _src += info_src.wline;
        _dst += info_dst.wline;
    }
}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
void MemImage::Clear(DWORD *dst, DWORD *line, ImageInfo &info, int start, int num)
{
    dst += info.wline * start;
    while (num--)
    {
        memcpy(dst, line, sizeof(DWORD) * info.width);
        dst += info.wline;
    }
}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
void MemImage::Show(MemImage &i, int x, int y, int rop)
{
    if (!ptr) return;
    if (!i.SetAccess()) return;

    IRect rc_dst(x, y, info.width, info.height);
    rc_dst &= clip;
    IRect rc_src(0, 0, i.info.width, i.info.height);
    Show(i, rc_src, rc_dst, rop);
    i.ReleaseAccess();
}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
void MemImage::Show(MemImage &i, IRect &rc_dst, int rop)
{
    if (!ptr) return;
    i.ReleaseAccess();

    rc_dst &= clip;
    IRect rc_src(0, 0, min(rc_dst.Width(), i.info.width), min(rc_dst.Height(), i.info.height));
    Show(i, rc_src, rc_dst, rop);
    i.ReleaseAccess();
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::SetBmpRop(UINT p)
{
    switch (p)
    {
        case BLACKNESS:
            _ptLineOP = &MemImage::bmpBLACKNESS;
            _bmpLineOP = &MemImage::bmpBLACKNESSLine;
            _patLineOP = &MemImage::patBLACKNESSLine;
            _patOP = &MemImage::patBLACKNESS;

            break;
        case DSTINVERT:
            _ptLineOP = &MemImage::bmpDSTINVERT;
            _bmpLineOP = &MemImage::bmpDSTINVERTLine;
            _patLineOP = &MemImage::patDSTINVERTLine;
            _patOP = &MemImage::patDSTINVERT;
            break;
        case PATCOPY:
            _ptLineOP = &MemImage::bmpPATCOPY;
            _bmpLineOP = &MemImage::bmpPATCOPYLine;
            _patLineOP = &MemImage::patPATCOPYLine;
            _patLineOP = &MemImage::patPATCOPYLine;
            break;
        case PATINVERT:
            _ptLineOP = &MemImage::bmpPATINVERT;
            _bmpLineOP = &MemImage::bmpPATINVERTLine;
            _patLineOP = &MemImage::patPATINVERTLine;
            _patOP = &MemImage::patPATINVERT;
            break;
        case WHITENESS:
            _ptLineOP = &MemImage::bmpWHITENESS;
            _bmpLineOP = &MemImage::bmpWHITENESSLine;
            _patLineOP = &MemImage::patWHITENESSLine;
            _patOP = &MemImage::patWHITENESS;
            break;
        case MERGECOPY:
            _ptLineOP = &MemImage::bmpMERGECOPY;
            _bmpLineOP = &MemImage::bmpMERGECOPYLine;
            _patLineOP = &MemImage::patPATCOPYLine;
            _patOP = &MemImage::patPATCOPY;
            break;
        case MERGEPAINT:
            _ptLineOP = &MemImage::bmpMERGEPAINT;
            _bmpLineOP = &MemImage::bmpMERGEPAINTLine;
            _patLineOP = &MemImage::patPATCOPYLine;
            _patOP = &MemImage::patPATCOPY;
            break;
        case NOTSRCCOPY:
            _ptLineOP = &MemImage::bmpNOTSRCCOPY;
            _bmpLineOP = &MemImage::bmpNOTSRCCOPYLine;
            _patLineOP = &MemImage::patPATCOPYLine;
            _patOP = &MemImage::patPATCOPY;
            break;
        case NOTSRCERASE:
            _ptLineOP = &MemImage::bmpNOTSRCERASE;
            _bmpLineOP = &MemImage::bmpNOTSRCERASELine;
            _patLineOP = &MemImage::patPATCOPYLine;
            _patOP = &MemImage::patPATCOPY;
            break;
        case PATPAINT:
            _ptLineOP = &MemImage::bmpPATPAINT;
            _bmpLineOP = &MemImage::bmpPATPAINTLine;
            _patLineOP = &MemImage::patPATCOPYLine;
            _patOP = &MemImage::patPATCOPY;
            break;
        case SRCAND:
            _ptLineOP = &MemImage::bmpSRCAND;
            _bmpLineOP = &MemImage::bmpSRCANDLine;
            _patLineOP = &MemImage::patPATCOPYLine;
            _patOP = &MemImage::patPATCOPY;
            break;
        case SRCCOPY:
            _ptLineOP = &MemImage::bmpSRCCOPY;
            _bmpLineOP = &MemImage::bmpSRCCOPYLine;
            _patLineOP = &MemImage::patPATCOPYLine;
            _patOP = &MemImage::patPATCOPY;
            break;
        case SRCERASE:
            _ptLineOP = &MemImage::bmpSRCERASE;
            _bmpLineOP = &MemImage::bmpSRCERASELine;
            _patLineOP = &MemImage::patPATCOPYLine;
            _patOP = &MemImage::patPATCOPY;
            break;
        case SRCINVERT:
            _ptLineOP = &MemImage::bmpSRCINVERT;
            _bmpLineOP = &MemImage::bmpSRCINVERTLine;
            _patLineOP = &MemImage::patPATCOPYLine;
            _patOP = &MemImage::patPATCOPY;
            break;
        case SRCPAINT:
            _ptLineOP = &MemImage::bmpSRCPAINT;
            _bmpLineOP = &MemImage::bmpSRCPAINTLine;
            _patLineOP = &MemImage::patPATCOPYLine;
            _patOP = &MemImage::patPATCOPY;
            break;
        default :
            _ptLineOP = &MemImage::bmpSRCCOPY;
            _bmpLineOP = &MemImage::bmpSRCCOPYLine;
            _patLineOP = &MemImage::patPATCOPYLine;
            _patOP = &MemImage::patPATCOPY;
            break;

    }

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
BOOL
MemImage::ShowWithMask(MemImage &i, MemImage &mask, IRect &_rc_src, IRect &_rc_dst, COLORREF iMask,
                       COLORREF csrc, COLORREF cdst)
{
    if (!ptr) return FALSE;
    if (!i.SetAccess()) return FALSE;
    if (!mask.SetAccess())
    {
        i.ReleaseAccess();
        return FALSE;
    }

    IRect rc_src(0, 0, i.info.width, i.info.height);
    _rc_src &= rc_src;
    IRect rs = _rc_src;
    IRect rd = _rc_dst;
    rd.left += curOrg.x;
    rd.top += curOrg.y;
    int offx = rd.left - rs.left;
    int offy = rd.top - rs.top;
    rs.OffsetRect(offx, offy);
    rs &= rd;
    rs &= clip;
    rs.OffsetRect(-offx, -offy);
    rd &= clip;
    int j = min(rs.Height(), rd.Height());

    DWORD *_src = i.ptr + i.info.wline * rs.top + rs.left;
    DWORD *_mask = mask.ptr + mask.info.wline * rs.top + rs.left;

    DWORD *_dst = ptr + info.wline * rd.top + rd.left;
    int _w = min(rs.Width(), rd.Width());

    while (j--)
    {
        for (int j = 0; j < _w; ++j)
            if (_mask[j] == iMask)
                _dst[j] = _src[j] == csrc ? cdst : _src[j];
        _src += i.info.wline;
        _mask += i.info.wline;
        _dst += info.wline;
    }
    i.ReleaseAccess();
    mask.ReleaseAccess();
    return TRUE;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
BOOL MemImage::TransparentBlt(MemImage &i, IRect &_rc_src, IRect &_rc_dst, COLORREF crTransparent)
{
    if (!ptr) return FALSE;
    if (!i.SetAccess()) return FALSE;

    IRect rc_src(0, 0, i.info.width, i.info.height);
    _rc_src &= rc_src;
    IRect rs = _rc_src;
    IRect rd = _rc_dst;
    rd.left += curOrg.x;
    rd.top += curOrg.y;
    int offx = rd.left - rs.left;
    int offy = rd.top - rs.top;
    rs.OffsetRect(offx, offy);
    rs &= rd;
    rs &= clip;
    rs.OffsetRect(-offx, -offy);
    rd &= clip;
    int j = min(rs.Height(), rd.Height());

    DWORD *_src = i.ptr + i.info.wline * rs.top + rs.left;
    DWORD *_dst = ptr + info.wline * rd.top + rd.left;
    int _w = min(rs.Width(), rd.Width());
    ColorRef *pclr = (ColorRef *) &crTransparent;
    pclr->a = 0xFF;

    while (j--)
    {
        for (int j = 0; j < _w; ++j)
            if (crTransparent != _src[j])
                _dst[j] = _src[j];
        _src += i.info.wline;
        _dst += info.wline;
    }
    i.ReleaseAccess();
    return TRUE;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
int MemImage::NumberOfProcessors()
{
    return numberOfProcessors != -1 ? numberOfProcessors : pMultiThreads->NumberOfProcessors();

}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
void MemImage::Show(MemImage &i, IRect &_rc_src, IRect &_rc_dst, int rop)
{
    if (!ptr) return;
    if (!i.SetAccess()) return;
    IRect rc_src(0, 0, i.info.width, i.info.height);
    _rc_src &= rc_src;
    IRect rs = _rc_src;
    IRect rd = _rc_dst;
    rd.left += curOrg.x;
    rd.top += curOrg.y;
    int offx = rd.left - rs.left;
    int offy = rd.top - rs.top;
    rs.OffsetRect(offx, offy);
    rs &= rd;
    rs &= clip;
    rs.OffsetRect(-offx, -offy);
    rd &= clip;
    int j = min(rs.Height(), rd.Height());

    SetBmpRop(rop);
    int N = NumberOfProcessors();
    if (j / N > 10 && N > 1 && pMultiThreads->TryLock())
    {
        int n = j / N;
        ThreadParam *params = (ThreadParam *) _alloca(sizeof(ThreadParam) * (N + 1));
        for (int k = 0; k < N; ++k)
        {
            params[k].InitPaste(this, rd, &i, rs, k * n, k == N - 1 ? j : (k + 1) * n);
            pMultiThreads->Run(k, (AFX_THREADPROC) ThreadPaste, params + k);
        }
        pMultiThreads->Wait();
        pMultiThreads->Unlock();

    } else
        Paste(_bmpLineOP, ptr, info, rd, i.ptr, i.info, rs, brush, 0, j);
    SetBmpRop(SRCCOPY);
    i.ReleaseAccess();

}

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
void MemImage::Scroll(int offsetX, int offsetY)
{
    if (!ptr) return;
    int j;
    IRect rc1, rc(0, 0, info.width, info.height);
    rc1 = rc;
    MArray<IRect> fill;
    rc1.OffsetRect(offsetX, offsetY);
    rc.Minus(rc1, fill);
    rc1 &= rc;

    if (rc.bottom + offsetY < 0 || rc.top + offsetY > info.height ||
        rc.right + offsetX < 0 || rc.left + offsetX > info.width)
        return;
    IPoint tprg = curOrg;
    IRect rc_dst(max(0, rc.left + offsetX),
                 max(0, rc.top + offsetY),
                 min(info.width, rc.right + offsetX),
                 min(info.height, rc.bottom + offsetY));
    curOrg.Set(0, 0);

    IRect rc_src;
    rc_src.left = max(0, rc.left + offsetX < 0 ? -offsetX : rc.left);
    rc_src.top = max(0, rc.top + offsetY < 0 ? -offsetY : rc.top);
    rc_src.right = min(info.width, rc_src.left + rc_dst.Width());
    rc_src.bottom = min(info.height, rc_src.top + rc_dst.Height());
    int w, _w = min(rc_src.Width(), rc_dst.Width());
    j = min(rc_src.Height(), rc_dst.Height());
    DWORD *src = ptr;
    DWORD *dst = ptr;
    if (offsetY <= 0)
    {
        DWORD *_src = src + (info.wline * rc_src.top + rc_src.left);
        DWORD *_dst = dst + (info.wline * rc_dst.top + rc_dst.left);
        while (j--)
        {
            if (rc_src.left < rc_dst.left)
            {
                w = _w;
                src = _src + w - 1;
                dst = _dst + w - 1;
                while (w--)
                    *dst-- = *src--;

            } else
                memcpy(_dst, _src, _w * sizeof(DWORD));
            _src += info.wline;
            _dst += info.wline;
        }
    } else
    {

        DWORD *_src = src + (info.wline * (rc_src.bottom - 1) + rc_src.left);
        DWORD *_dst = dst + (info.wline * (rc_dst.bottom - 1) + rc_dst.left);

        while (j--)
        {
            if (rc_src.left < rc_dst.left)
            {
                w = _w;
                src = _src + w - 1;
                dst = _dst + w - 1;
                while (w--)
                    *dst-- = *src--;

            } else
                memcpy(_dst, _src, _w * sizeof(DWORD));
            _src -= info.wline;
            _dst -= info.wline;
        }

    }
    for (int i = 0, l = fill.GetLen(); i < l; ++i)
    {
        fill[i].InflateRect(1, 1);
        fill[i] &= rc;
        FillRect(fill[i], *brush);
    }
    curOrg = tprg;

}


//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Line(int x1, int y1, int x2, int y2)
{
    if (!ptr) return;

    IPoint p[2];
    p[0].SetPoint(x1, y1);
    p[1].SetPoint(x2, y2);
    PolyLine(p, 2);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Tile(IRect &rc, int offset)
{
    if (!ptr) return;
    IPoint size(brush->pattern->width, brush->pattern->height);

    int width = rc.Width();
    rc.OffsetRect(offset, offset);
    int x_size = rc.Width();
    int y_size = rc.Height();
    int endx = rc.left + size.x;
    int endy = rc.top + size.y;
    int startx = rc.left;
    int starty = rc.top;
    IRect rect;
    ImageInfo2 *pat = brush->pattern;
    DWORD *src = pat->image.GetPtr();
    BYTE *_mask = mask.GetPtr();
    while (starty < rc.top + y_size)
    {
        endx = rc.left + size.x;
        startx = rc.left;
        while (startx < rc.left + x_size)
        {
            if (endx > rc.left + x_size)
                endx = rc.left + x_size;
            if (endy > rc.top + y_size)
                endy = rc.top + y_size;
            rect.SetRect(startx, starty, endx, endy);
            DWORD *_src = src;
            BYTE *ms = _mask + width * (rect.top - rc.top) + (rect.left - rc.left);
            DWORD *_dst = ptr + info.wline * rect.top + rect.left;
            int _w = rect.Width();
            int num = rect.Height();
            while (num--)
            {
                for (int i = 0; i < _w; ++i)
                    if (!ms[i])
                        _dst[i] = _src[i];
                _src += pat->wline;
                _dst += info.wline;
                ms += width;
            }
            startx = endx;
            endx = startx + size.x;
        }
        starty = endy;
        endy = starty + size.y;
    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Clear()
{
    Clear(brush->color);
}

void MemImage::Clear(COLORREF clr)
{
    if (!ptr) return;

    DWORD *p = ptr;
    DWORD *h = p;
    int j = info.width;
    while (j--)
        *p++ = clr;
    p = h + info.wline;
    int N = NumberOfProcessors();
    if (N > 1 && pMultiThreads->TryLock())
    {

        int n = (info.height - 1) / N;
        ThreadParam *params = (ThreadParam *) _alloca(sizeof(ThreadParam) * (N + 1));
        for (int k = 0; k < N; ++k)
        {
            params[k].InitClear(this, h, k * n, k == N - 1 ? (info.height) : (k + 1) * n);
            pMultiThreads->Run(k, (AFX_THREADPROC) ThreadClear, params + k);
        }
        pMultiThreads->Wait();
        pMultiThreads->Unlock();

    } else
        Clear(p, h, info, 0, info.height - 1);
/*	{
		for (int i = 1; i < info.height; ++i, p += info.wline)
		memcpy(p, h, sizeof(DWORD) * info.width);
	}
*/
}

AFONT MemImage::SelectObject(AFONT h)
{
    AFONT old = font;
    font = h;
    if (pGDIPath)
        pGDIPath->SelectObject(h);
    if (pGDIPrint)
        pGDIPrint->SelectObject(h);
    return old;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
GPEN MemImage::SelectObject(GPEN h)
{
    GPEN old = pen;
    if (!h)
        h = &t_pen;
    if (pGDIPath)
        pGDIPath->SelectObject(h);
    if (pGDIPrint)
        pGDIPrint->SelectObject(h);

    pen = h;
    return old;

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
GBRUSH MemImage::SelectObject(GBRUSH h)
{
    GBRUSH old = brush;
    if (!h)
        h = &t_brush;

    if (pGDIPath)
        pGDIPath->SelectObject(h);
    if (pGDIPrint)
        pGDIPrint->SelectObject(h);
    brush = h;
    return old;

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::UnSelect(AHandle *h)
{
    if (!h) return;
    if (h->type == BGCOLOR)
        SelectObject((GBRUSH) 0);

    if (h->type == FGCOLOR)
        SelectObject((GPEN) 0);


}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
AHandle *MemImage::SelectObject(AHandle *h)
{
    if (!h) return 0;
    if (h->type == BGCOLOR || h->type == HBGCOLOR)
        return SelectObject((GBRUSH) h);

    if (h->type == FGCOLOR || h->type == HFGCOLOR)
        return SelectObject((GPEN) h);

    return 0;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::MoveToEx(int x, int y, IPoint *oldpos)
{
    if (!ptr) return;
    if (oldpos) *oldpos = curPos;
    curPos.Set(x, y);
    if (pGDIPath)
    {
        pGDIPath->MoveTo(x, y);
        return;
    }
    if (pGDIPrint)
        pGDIPrint->MoveTo(x, y);


}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Rectangle(IRect &rc, bool fFill)
{
    if (pGDIPath)
    {
        pGDIPath->Rectangle(curOrg.x + rc.left, curOrg.y + rc.top, curOrg.x + rc.right,
                            curOrg.y + rc.bottom);
        return;
    }
    if (pGDIPrint)
    {
        pGDIPrint->Rectangle(curOrg.x + rc.left, curOrg.y + rc.top, curOrg.x + rc.right,
                             curOrg.y + rc.bottom);
        return;
    }
    if (!ptr) return;

    if (!fFill || !brush->a)
    {
        Rect(rc);
        return;
    }
    if (fFill && brush->a)
        FillRect(rc, *(ABGColor *) brush);
    if (pen->a)
        Rect(rc);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::_PolyLine(IPoint *pp, int len)
{
    int N = NumberOfProcessors();
    if (0) //len > N * 10 && N > 1 && pMultiThreads->TryLock())
    {
        int n = len / N;
        ParamPolyLine *params = (ParamPolyLine *) _alloca(sizeof(ParamPolyLine) * (N + 1));
        int count = n, pos = 1;
        for (int k = 0; k < N; ++k)
        {
            params[k].Init(this, pp + pos - 1, count);
            pos += count;
            if (pos + n >= len - 1)
                count = len - pos;
            else
                count = n;
            pMultiThreads->Run(k, (AFX_THREADPROC) ThreadPolyLine, params + k);
        }
        pMultiThreads->Wait();
        pMultiThreads->Unlock();

    } else
        __PolyLine(pp, len);

}

void MemImage::__PolyLine(IPoint *pp, int len)
{
    if (!ptr || !len) return;

    if (pGDIPrint)
    {
        pGDIPrint->PolyLine(pp, len);
        return;
    }
    ILine line;
    if (rop2 != R2_COPYPEN && pp[0].y >= 0 && pp[0].y < info.height && pp[0].x >= 0 &&
        pp[0].x < info.width)
    {
        line.SetRop(rop2);
        line.Rop(ptr + pp[0].y * info.wline + pp[0].x, pen->color);
    }
    bool f_next = false;
    for (int i = 1; i < len; ++i)
    {
        if (pp[i - 1].x == pp[i].x && pp[i - 1].y == pp[i].y)
        {
            line.SetRop(rop2);
            if (pp[i].y >= 0 && pp[i].y < info.height && pp[i].x >= 0 && pp[i].x < info.width)
                line.Rop(ptr + pp[i].y * info.wline + pp[i].x, pen->color);
            f_next = false;
        } else if (abs(pp[i - 1].x - pp[i].x) <= 1 && abs(pp[i - 1].y - pp[i].y) <= 1)
        {
            line.SetRop(rop2);
            if (i == 1)
                if (pp[0].y >= 0 && pp[0].y < info.height && pp[0].x >= 0 && pp[0].x < info.width)
                    line.Rop(ptr + pp[0].y * info.wline + pp[0].x, pen->color);
            if (pp[i].y >= 0 && pp[i].y < info.height && pp[i].x >= 0 && pp[i].x < info.width)
                line.Rop(ptr + pp[i].y * info.wline + pp[i].x, pen->color);
            f_next = false;

        } else
        {
            line.Init(pp[i - 1].x, pp[i - 1].y, pp[i].x, pp[i].y);
            if (f_next)
                line.Next();
            line.Draw(ptr, info, pen->width, pen->color, bkcolor, bkmode, rop2, pen->attrib);
            f_next = true;
        }
    }
}
//---------------------------------------------------------------
//
//---------------------------------------------------------------
#if (0)
                                                                                                                        void MemImage::PolyLine(IPoint *pt, int len)
{
	if (pGDIPath && pt)
		{
			pGDIPath->PolyLine(pt, len);
			return;
		}

	if (!ptr || !pt) return;
	IRect rc(0, 0, info.height, info.width);

	if (len < 2)
	{
		SetPixel(pt->x + curOrg.x, pt->y + curOrg.x, pen->color);
		return;
	}
	if((rc & clip).IsEmpty())
		return ;
	MArray<IPoint> apt;
	apt.Copy(pt, len);
	pt = apt.GetPtr();
	IRect rc1(pt[0].x + curOrg.x, pt[0].y + curOrg.y, pt[0].x + curOrg.x, pt[0].y + curOrg.y);
	for (int i = 0; i < len; ++i)
	{
		pt[i].x += curOrg.x;
		pt[i].y += curOrg.y;
		rc1.Update(pt[i].x, pt[i].y);

	}
	if ((rc1 & clip) == rc1)
	{
		_PolyLine(pt, len);
		return;
	}
	DWClipLine<IPoint, int> clipL;
	clipL.Init(clip, 1);
//	--clipL.right;
//	--clipL.bottom;

	int n, num = 0;
	IPoint dst[3], tmp[2];

	BYTE fContinue = 0, prev_b = 0, next_b = 0, cur_b = (((BYTE)((pt->x < clipL.left) << 3) & 8) | ((BYTE)((pt->y < clipL.top) << 2) & 4) |
								((BYTE)((pt->x > clipL.right) << 1) & 2) | ((BYTE)((pt->y > clipL.bottom)) & 1));
	bool fFirst = 1;

	while (len--)
	{
		if (len)
		{
			next_b = ((BYTE)(((pt + 1)->x < clipL.left) << 3) & 8) | ((BYTE)(((pt + 1)->y < clipL.top) << 2) & 4) |
						((BYTE)(((pt + 1)->x > clipL.right) << 1) & 2) | ((BYTE)(((pt + 1)->y > clipL.bottom)) & 1);
			if (prev_b == cur_b)
				fContinue =  cur_b & next_b;
		}
		if (!fContinue)
		{
			if (!prev_b && !cur_b && !next_b)
			{
				*(dst + num) = *pt;
				//if (!(num &&  (dst + num)->x == (dst + num - 1)->x && (dst + num)->y == (dst + num - 1)->y))
					++num;
			}
			else
			{
				if (!fFirst)
				{

					if (!prev_b && !cur_b)
					{
						*(dst + num) = *pt;
					//	if (!(num &&  (dst + num)->x == (dst + num - 1)->x && (dst + num)->y == (dst + num - 1)->y))
							++num;

					}
					else
					{
						if (!(prev_b & cur_b))
						{
							if (num == 1)
								num = 0;
							tmp[0] = *(pt - 1);
							tmp[1] = *pt;
							if (clipL.ClipBarsk (tmp[0], tmp[1]))
							{
								*(dst + num) = *tmp;
								++num;
								*(dst + num) = *(tmp + 1);
								++num;

							}

						}
					}
					if (num > 1)
					{
						_PolyLine(dst, num);
						n = num;
						if ((num = (!cur_b && len)))
						{
							dst[0] = dst[n - 1];
						}
					}

				}
			}
		}
		if (num > 1)
		{
			_PolyLine(dst, num);
			dst[0] = dst[num - 1];
			num = 1;
		}
		fFirst = 0;
		prev_b = cur_b;
		cur_b = next_b;
		fContinue = 0;
		next_b = 64;
		++pt;
		while(*pt == *(pt - 1) && len)
		{
			--len;
			++pt;
		}

	}

}
#else

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::PolyLine(IPoint *pt, int len)
{
    if (pGDIPath && pt)
    {
        pGDIPath->PolyLine(pt, len);
        return;
    }

    if (!ptr || !pt) return;
    IRect rc(0, 0, info.height, info.width);

    if (len < 2)
    {
        SetPixel(pt->x + curOrg.x, pt->y + curOrg.x, pen->color);
        return;
    }
    if ((rc & clip).IsEmpty())
        return;
    MArray<IPoint> apt;
    apt.Copy(pt, len);
    pt = apt.GetPtr();
    IRect rc1(pt[0].x + curOrg.x, pt[0].y + curOrg.y, pt[0].x + curOrg.x, pt[0].y + curOrg.y);
    for (int i = 0; i < len; ++i)
    {
        pt[i].x += curOrg.x;
        pt[i].y += curOrg.y;
        rc1.Update(pt[i].x, pt[i].y);

    }
    if ((rc1 & clip) == rc1)
    {
        _PolyLine(pt, len);
        return;
    }
    DWClipLine<IPoint, int> clipL;
    clipL.Init(clip, 1);
//	--clipL.right;
//	--clipL.bottom;

    int n, num = 0;
    IPoint dst[3], tmp[2];
    MArray<IPoint> line;
    line.SetAddedRealSize(128);
    BYTE fContinue = 0, prev_b = 0, next_b = 0, cur_b = (((BYTE) ((pt->x < clipL.left) << 3) & 8) |
                                                         ((BYTE) ((pt->y < clipL.top) << 2) & 4) |
                                                         ((BYTE) ((pt->x > clipL.right) << 1) & 2) |
                                                         ((BYTE) ((pt->y > clipL.bottom)) & 1));
    bool fFirst = 1;
    bool fClip = false;
    while (len--)
    {
        fClip = false;
        if (len)
        {
            next_b = ((BYTE) (((pt + 1)->x < clipL.left) << 3) & 8) |
                     ((BYTE) (((pt + 1)->y < clipL.top) << 2) & 4) |
                     ((BYTE) (((pt + 1)->x > clipL.right) << 1) & 2) |
                     ((BYTE) (((pt + 1)->y > clipL.bottom)) & 1);
            if (prev_b == cur_b)
                fContinue = cur_b & next_b;
        }
        if (!fContinue)
        {
            if (!prev_b && !cur_b && !next_b)
            {
                if (!(num && pt->x == (dst + num - 1)->x && pt->y == (dst + num - 1)->y))
                {
                    *(dst + num) = *pt;
                    ++num;
                }
            } else
            {
                if (!fFirst)
                {

                    if (!prev_b && !cur_b)
                    {

                        if (!(num && pt->x == (dst + num - 1)->x && pt->y == (dst + num - 1)->y))
                        {
                            *(dst + num) = *pt;
                            ++num;
                        }

                    } else
                    {
                        if (!(prev_b & cur_b))
                        {
                            if (num == 1)
                                num = 0;
                            tmp[0] = *(pt - 1);
                            tmp[1] = *pt;
                            if (clipL.ClipBarsk(tmp[0], tmp[1]))
                            {
                                *(dst + num) = *tmp;
                                ++num;
                                *(dst + num) = *(tmp + 1);
                                ++num;

                            }
                            fClip = true;

                        }
                    }
                    if (num > 1 && fClip)
                    {
                        if (line.GetLen())
                            _PolyLine(line.GetPtr(), line.GetLen());
                        line.Copy(dst, num);
                        n = num;
                        if ((num = (*pt == dst[n - 1])))
                        {
                            dst[0] = dst[n - 1];
                        }
                    }

                }
            }
        }
        if (num > 1)
        {
            //_PolyLine(dst, num);
            //dst[0] = dst[num - 1];
            //num = 1;
            BOOL fAdd = 0;
            if (line.GetLen())
            {
                IPoint &p1 = line[line.GetLen() - 1];
                IPoint &p2 = dst[0];
                if (p1.x == p2.x && p1.y == p2.y)
                {
                    line.Append(dst + 1, num - 1);
                    fAdd = true;
                }
            }
            if (!fAdd)
            {
                if (line.GetLen())
                    _PolyLine(line.GetPtr(), line.GetLen());
                line.Copy(dst, num);
            }
            dst[0] = dst[num - 1];
            num = 1;

        }
        fFirst = 0;
        prev_b = cur_b;
        cur_b = next_b;
        fContinue = 0;
        next_b = 64;
        ++pt;
        while (*pt == *(pt - 1) && len)
        {
            --len;
            ++pt;
        }

    }


    if (line.GetLen())
        _PolyLine(line.GetPtr(), line.GetLen());

}

#endif

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::SetTransparentColor(COLORREF clr)
{
    if (!ptr) return;
    DWORD *c, *_c = ptr;
    int h = info.height;
    int w;
    clr = clr | 0xFF000000;
    while (h--)
    {
        c = _c;
        w = info.width;
        while (w--)
        {
            if (*c == clr)
                *c &= 0x00FFFFFF;
            ++c;
        }
        _c += info.wline;
    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Draw3dRect(IRect *lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight, int level)
{
    if (!ptr) return;

    int cx = lpRect->right - lpRect->left;
    int cy = lpRect->bottom - lpRect->top;
    FillSolidRect(lpRect->left, lpRect->top, cx - level, level, clrTopLeft);
    FillSolidRect(lpRect->left, lpRect->top, level, cy - level, clrTopLeft);
    FillSolidRect(lpRect->left + cx, lpRect->top, -level, cy, clrBottomRight);
    FillSolidRect(lpRect->left, lpRect->top + cy, cx, -level, clrBottomRight);

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::FillRect(IRect &_rc, ABGColor &clr)
{
    if (!ptr || !clr.a) return;

    IRect rc = clip;
    IRect rc1 = _rc;
    rc1.OffsetRect(curOrg.x, curOrg.y);
    rc &= rc1;
    if (rc.IsEmpty()) return;
    int N = NumberOfProcessors();

    if (10 < rc.Height() / N && N > 1 && pMultiThreads->TryLock())
    {
        int n;
        int nn = rc.Height();
        if (!clr.pattern)
            n = rc.Height() / N;
        else
        {
            int k = rc.Height() / clr.pattern->height;
            k /= N;
            n = k * clr.pattern->height;
        }
        ThreadParam *params = (ThreadParam *) _alloca(sizeof(ThreadParam) * (N + 1));
        IRect r = rc;

        for (int k = 0; k < N; ++k)
        {
            r.top = n * k + rc.top;
            r.bottom = (k == N - 1 ? nn : (k + 1) * n) + rc.top;

            params[k].InitFillRect(this, r, &clr);
            pMultiThreads->Run(k, (AFX_THREADPROC) ThreadFillRect, params + k);
        }
        pMultiThreads->Wait();
        pMultiThreads->Unlock();

    } else
        _FillRect(rc, clr);

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Rectangle(MArray<IRect> &arc)
{
    if (!ptr) return;

    IRect *prc = arc.GetPtr();
    ILine line;
    line.SetRop(rop2);
    for (int j, i, k = 0, l = arc.GetLen(); k < l; ++k)
    {
        IRect rc = prc[k] & clip;
        //rc &= clip;
        if (!rc.IsEmpty())
        {
            DWORD *_dst = ptr + (info.wline * rc.top + rc.left);
            j = rc.bottom - rc.top;
            int w = rc.right - rc.left;
            if (j && w)
            {
                DWORD *h = _dst;
                for (i = 0; i < w; ++i)
                    h[i] = brush->color;
                w *= sizeof(DWORD);
                while (j--)
                {
                    memcpy(_dst, h, w);
                    _dst += info.wline;
                }
                if (pen->a)
                {
                    line.Init(rc.left, rc.top, rc.left, rc.bottom);
                    line.Draw(ptr, info, pen->width, pen->color, bkcolor, bkmode, rop2,
                              pen->attrib);
                    line.Init(rc.left + 1, rc.bottom, rc.right, rc.bottom);
                    line.Draw(ptr, info, pen->width, pen->color, bkcolor, bkmode, rop2,
                              pen->attrib);
                    line.Init(rc.right, rc.bottom - 1, rc.right, rc.top);
                    line.Draw(ptr, info, pen->width, pen->color, bkcolor, bkmode, rop2,
                              pen->attrib);
                    line.Init(rc.right - 1, rc.top, rc.left + 1, rc.top);
                    line.Draw(ptr, info, pen->width, pen->color, bkcolor, bkmode, rop2,
                              pen->attrib);
                }
            }
        }
    }

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::_FillRect(IRect &rc, ABGColor &clr)
{
    if (!ptr) return;
    int i, j;
    if (!clr.a) return;
    if (!rc.IsEmpty())
    {
        if (!clr.pattern)
        {
            DWORD *_dst = ptr + (info.wline * rc.top + rc.left);
            j = rc.bottom - rc.top;
            int w = rc.right - rc.left;
            if (w && j)
            {
                DWORD *h = _dst;
                for (i = 0; i < w; ++i)
                    h[i] = clr.color;
                w *= sizeof(DWORD);
                while (j--)
                {
                    memcpy(_dst, h, w);
                    _dst += info.wline;
                }
            }
        } else if (clr.pattern && clr.typeBrush == tagGBRUSH::BG_IMG)
        {
            IPoint size(clr.pattern->width - 1, clr.pattern->height - 1);
            int width = rc.Width();
            int x_size = rc.Width();
            int y_size = rc.Height();
            int endx = rc.left + size.x;
            int endy = rc.top + size.y;
            int startx = rc.left;
            int starty = rc.top;
            IRect rect;

            ImageInfo2 *pat = clr.pattern;
            DWORD *src = pat->image.GetPtr();
            while (starty < rc.top + y_size)
            {
                endx = rc.left + size.x;
                startx = rc.left;
                while (startx < rc.left + x_size)
                {
                    if (endx > rc.left + x_size)
                        endx = rc.left + x_size;
                    if (endy > rc.top + y_size)
                        endy = rc.top + y_size;
                    rect.SetRect(startx, starty, endx, endy);
                    DWORD *_src = src;
                    DWORD *_dst = ptr + info.wline * rect.top + rect.left;
                    int _w = rect.Width();
                    int num = rect.Height();
                    while (num--)
                    {
                        for (int i = 0; i < _w; ++i)
                            _dst[i] = _src[i];
                        _src += pat->wline;
                        _dst += info.wline;
                    }
                    startx = endx;
                    endx = startx + size.x;
                }
                starty = endy;
                endy = starty + size.y;
            }
        }
    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Rect(int x1, int y1, int x2, int y2)
{
    IPoint p[5];
    p[0].SetPoint(x1, y1);
    p[1].SetPoint(x1, y2);
    p[2].SetPoint(x2, y2);
    p[3].SetPoint(x2, y1);
    p[4].SetPoint(x1, y1);
    PolyLine(p, 5);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Rectangles(IRect *rc, int num)
{
    if (!ptr) return;


    while (num--)
    {
        Rectangle(*rc);

        ++rc;
    }

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Stretch(MemImage &src, IRect &rc_dst, int rop)
{
    if (!ptr) return;

    if (!src.SetAccess()) return;
    IRect rc_src(0, 0, src.info.width, src.info.height);
    Stretch(src, rc_src, rc_dst, rop);
    src.ReleaseAccess();

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Stretch2(Transform2F<Vector2F, IRect, float> &trans, ptLineOP op, DWORD *dst,
                        ImageInfo &info_dst, IRect &rc_dst, DWORD *src, ImageInfo &info_src,
                        IRect &rc_src, int start, int end)
{
    if (!ptr) return;

    try
    {

        ColorRef rs;
        rs.a = 255;
        LinearBox bx;
        Vector2F pos, dmin, dmax, t;
        float _x, _y, t1x, t1y;
        int ix, iy;
        int r, g, b, a;
        float scale = trans.form.m11ScaleX;
        int X = info_src.width - 1;
        int X1 = info_src.width;
        int Y = info_src.height - 1;

        for (int y = start; y < end; ++y)
        {
            _y = trans.invert.m22ScaleY * ((float) y + trans.invert.m23Dy);
            bx.minPoint.y = (float) ((int) _y);
            bx.maxPoint.y = (float) ((int) _y + 1);
            for (int x = rc_dst.left; x < rc_dst.right; ++x)
            {

                _x = trans.invert.m11ScaleX * ((float) x + trans.invert.m13Dx);
                //trans.DPtoLP(_x, _y);
                bx.minPoint.x = (float) ((int) _x);
                bx.maxPoint.x = (float) ((int) _x + 1);
                pos.x = _x;
                pos.y = _y;
                for (int i = 0; i < 4; ++i)
                {
                    ix = (int) (i & 1 ? bx.maxPoint.x : bx.minPoint.x);
                    if (ix < 0) ix = 0;
                    iy = (int) (i & 2 ? bx.maxPoint.y : bx.minPoint.y);
                    if (iy < 0) iy = 0;
                    if (ix > X)
                        ix = X;
                    if (iy > Y)
                        iy = Y;
                    bx.Val.s[i].color = src[ix + iy * X1];
                }
                dmin.x = pos.x - bx.minPoint.x;
                dmin.y = pos.y - bx.minPoint.y;
                dmax.x = bx.maxPoint.x - bx.minPoint.x;
                dmax.y = bx.maxPoint.y - bx.minPoint.y;

                t.x = dmax.x == 0.0 ? 0.0f : (float) (dmin.x / dmax.x);
                t.y = dmax.y == 0.0 ? 0.0f : (float) (dmin.y / dmax.y);
                t1x = 1.0f - t.x;
                t1y = 1.0f - t.y;

                r = (int) ((bx.Val.s0.r * t1x + bx.Val.s1.r * t.x) * t1y +
                           (bx.Val.s2.r * t1x + bx.Val.s3.r * t.x) * t.y);
                g = (int) ((bx.Val.s0.g * t1x + bx.Val.s1.g * t.x) * t1y +
                           (bx.Val.s2.g * t1x + bx.Val.s3.g * t.x) * t.y);
                b = (int) ((bx.Val.s0.b * t1x + bx.Val.s1.b * t.x) * t1y +
                           (bx.Val.s2.b * t1x + bx.Val.s3.b * t.x) * t.y);
                a = (int) ((bx.Val.s0.a * t1x + bx.Val.s1.a * t.x) * t1y +
                           (bx.Val.s2.a * t1x + bx.Val.s3.a * t.x) * t.y);
                rs.r = (BYTE) (r < 255 ? r : 255);
                rs.g = (BYTE) (g < 255 ? g : 255);
                rs.b = (BYTE) (b < 255 ? b : 255);
                rs.a = (BYTE) (a < 255 ? a : 255);
                if (!op)
                    dst[x + info_dst.wline * y] = rs.color;
                else
                    (this->*op)(dst + x + info_dst.wline * y, &rs.color, brush->color);
            }
        }
    } catch (std::exception &e)
    {
        TRACE(e.what());

    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Stretch_2(Transform2F<Vector2F, IRect, float> &trans, DWORD *dst, IPoint &org1,
                         IPoint &org2, ImageInfo &info_dst, IRect &rc_dst, DWORD *src1, DWORD *src2,
                         ImageInfo &info_src, IRect &rc_src, int start, int end)
{

    try
    {

        ColorRef rs;
        rs.a = 255;
        LinearBox bx;
        Color4Side Val1, Val2;
        Vector2F pos, dmin, dmax, t;
        float _x, _y, t1x, t1y;
        int ix, iy;
        int r, g, b, a;
        float scale = trans.form.m11ScaleX;
        int X = info_src.width - 1;
        int X1 = info_src.width;
        int Y = info_src.height - 1;

        for (int y = start; y < end; ++y)
        {
            _y = trans.invert.m22ScaleY * ((float) y + trans.invert.m23Dy);
            bx.minPoint.y = (float) ((int) _y);
            bx.maxPoint.y = (float) ((int) _y + 1);
            for (int x = rc_dst.left; x < rc_dst.right; ++x)
            {

                _x = trans.invert.m11ScaleX * ((float) x + trans.invert.m13Dx);
                //trans.DPtoLP(_x, _y);
                bx.minPoint.x = (float) ((int) _x);
                bx.maxPoint.x = (float) ((int) _x + 1);
                pos.x = _x;
                pos.y = _y;
                for (int i = 0; i < 4; ++i)
                {
                    ix = int(i & 1 ? bx.maxPoint.x : bx.minPoint.x);
                    if (ix < 0) ix = 0;
                    iy = int(i & 2 ? bx.maxPoint.y : bx.minPoint.y);
                    if (iy < 0) iy = 0;
                    if (ix > X)
                        ix = X;
                    if (iy > Y)
                        iy = Y;
                    Val1.s[i].color = src1[ix + iy * X1];
                    Val2.s[i].color = src2[ix + iy * X1];
                }
                dmin.x = pos.x - bx.minPoint.x;
                dmin.y = pos.y - bx.minPoint.y;
                dmax.x = bx.maxPoint.x - bx.minPoint.x;
                dmax.y = bx.maxPoint.y - bx.minPoint.y;

                t.x = dmax.x == 0.0 ? 0.0f : (float) (dmin.x / dmax.x);
                t.y = dmax.y == 0.0 ? 0.0f : (float) (dmin.y / dmax.y);
                t1x = 1.0f - t.x;
                t1y = 1.0f - t.y;

                r = (int) ((Val1.s0.r * t1x + Val1.s1.r * t.x) * t1y +
                           (Val1.s2.r * t1x + Val1.s3.r * t.x) * t.y);
                g = (int) ((Val1.s0.g * t1x + Val1.s1.g * t.x) * t1y +
                           (Val1.s2.g * t1x + Val1.s3.g * t.x) * t.y);
                b = (int) ((Val1.s0.b * t1x + Val1.s1.b * t.x) * t1y +
                           (Val1.s2.b * t1x + Val1.s3.b * t.x) * t.y);
                a = (int) ((Val1.s0.a * t1x + Val1.s1.a * t.x) * t1y +
                           (Val1.s2.a * t1x + Val1.s3.a * t.x) * t.y);
                rs.r = (BYTE) (r < 255 ? r : 255);
                rs.g = (BYTE) (g < 255 ? g : 255);
                rs.b = (BYTE) (b < 255 ? b : 255);
                rs.a = (BYTE) (a < 255 ? a : 255);
                dst[x + org1.x + info_dst.wline * (y + org2.y)] = rs.color;
                r = (int) ((Val2.s0.r * t1x + Val2.s1.r * t.x) * t1y +
                           (Val2.s2.r * t1x + Val2.s3.r * t.x) * t.y);
                g = (int) ((Val2.s0.g * t1x + Val2.s1.g * t.x) * t1y +
                           (Val2.s2.g * t1x + Val2.s3.g * t.x) * t.y);
                b = (int) ((Val2.s0.b * t1x + Val2.s1.b * t.x) * t1y +
                           (Val2.s2.b * t1x + Val2.s3.b * t.x) * t.y);
                a = (int) ((Val2.s0.a * t1x + Val2.s1.a * t.x) * t1y +
                           (Val2.s2.a * t1x + Val2.s3.a * t.x) * t.y);
                rs.r = (BYTE) (r < 255 ? r : 255);
                rs.g = (BYTE) (g < 255 ? g : 255);
                rs.b = (BYTE) (b < 255 ? b : 255);
                rs.a = (BYTE) (a < 255 ? a : 255);
                dst[x + org2.x + info_dst.wline * (y + org2.y)] = rs.color;
            }
        }
    } catch (std::exception &e)
    {
        TRACE(e.what());

    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Stretch1(Transform2F<Vector2F, IRect, float>& trans, ptLineOP op, DWORD* dst,
    ImageInfo& info_dst, IRect& rc_dst, DWORD* src, ImageInfo& info_src,
    IRect& rc_src, int start, int end)
{
    if (!ptr) return;

    try
    {
        ColorRef rs;
        rs.a = 255;
        float s_x = 1.0f / trans.form.m11ScaleX;
        float s_y = 1.0f / trans.form.m22ScaleY;
        float sc_x = s_x / 2;
        float sc_y = s_y / 2;
        float ms = max(s_x, s_y);
        ColorRef *tmp = (ColorRef *) _alloca(sizeof(ColorRef) * int(ms + 4) * int(ms + 4));
        int minx, limx, limy, miny;
        int ni, r, g, b, a;
        float x1, y1;
        ColorRef* clrs = (ColorRef*)src;
        if (rc_dst.left < 0)
            rc_dst.left = 0;

        for (int y = start; y < end; ++y)
        {
            y1 = trans.invert.m22ScaleY * ((float)y + trans.invert.m23Dy);
            limy = min(int(y1 + sc_y + 1.5f), rc_src.bottom) * info_src.wline;
            miny = max(int(y1 - sc_y + 0.5f), rc_src.top) * info_src.wline;
            for (int x = rc_dst.left; x < rc_dst.right; ++x)
            {
                ni = r = g = b = a = 0;
                //x1 = (float)x;
                x1 = trans.invert.m11ScaleX * ((float)x + trans.invert.m13Dx);
                //y1 = (float)y;
                //trans.DPtoLP(x1, y1);
                minx = max(int(x1 - sc_x + 0.5f), rc_src.left);
                limx = min(int(x1 + sc_x + 1.5f), rc_src.right);
                for (int iy = miny; iy < limy; iy += info_src.wline)
                    for (int ix = minx; ix < limx; ++ix, ++ni)
                        tmp[ni].color = clrs[ix + iy].color;
                for (int j = 0; j < ni; ++j)
                {
                    r += tmp[j].r;
                    g += tmp[j].g;
                    b += tmp[j].b;
                    a += tmp[j].a;
                }
                if (ni)
                {
                    rs.Set(r / ni, g / ni, b / ni, a / ni);
                    if (!op)
                        dst[x + info_dst.wline * y] = rs.color;
                    else
                        (this->*op)(dst + x + info_dst.wline * y, &rs.color, brush->color);
                }

            }
        }
    } catch (std::exception &e)
    {
        TRACE(e.what());

    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Stretch_1(Transform2F<Vector2F, IRect, float> &trans, DWORD *dst, IPoint &org1,
                         IPoint &org2, ImageInfo &info_dst, IRect &rc_dst, DWORD *src1, DWORD *src2,
                         ImageInfo &info_src, IRect &rc_src, int start, int end)
{

    try
    {

        ColorRef rs;
        rs.a = 255;
        float s_x = 1.0f / trans.form.m11ScaleX;
        float s_y = 1.0f / trans.form.m22ScaleY;
        float sc_x = s_x / 2;
        float sc_y = s_y / 2;
        float ms = max(s_x, s_y);
        ColorRef *tmp1 = (ColorRef *) _alloca(sizeof(ColorRef) * int(ms + 4) * int(ms + 4));
        ColorRef *tmp2 = (ColorRef *) _alloca(sizeof(ColorRef) * int(ms + 4) * int(ms + 4));

        int minx, limx, limy, miny;
        int ni, r1, g1, b1, a1, r2, g2, b2, a2;
        float x1, y1;
        ColorRef *clrs1 = (ColorRef *) src1;
        ColorRef *clrs2 = (ColorRef *) src2;

        if (rc_dst.left < 0)
            rc_dst.left = 0;

        for (int y = start; y < end; ++y)
        {
            y1 = trans.invert.m22ScaleY * ((float) y + trans.invert.m23Dy);
            limy = min(int(y1 + sc_y + 1.5f), rc_src.bottom) * info_src.wline;
            miny = max(int(y1 - sc_y + 0.5f), rc_src.top) * info_src.wline;
            for (int x = rc_dst.left; x < rc_dst.right; ++x)
            {
                ni = r1 = g1 = b1 = r2 = g2 = b2 = a1 = a2 = 0;
                //x1 = (float)x;
                x1 = trans.invert.m11ScaleX * ((float) x + trans.invert.m13Dx);
                //y1 = (float)y;
                //trans.DPtoLP(x1, y1);
                minx = max(int(x1 - sc_x + 0.5f), rc_src.left);
                limx = min(int(x1 + sc_x + 1.5f), rc_src.right);
                for (int iy = miny; iy < limy; iy += info_src.wline)
                    for (int ix = minx; ix < limx; ++ix, ++ni)
                    {
                        tmp1[ni].color = clrs1[ix + iy].color;
                        tmp2[ni].color = clrs2[ix + iy].color;
                    }
                for (int j = 0; j < ni; ++j)
                {
                    r1 += tmp1[j].r;
                    g1 += tmp1[j].g;
                    b1 += tmp1[j].b;
                    a1 += tmp1[j].a;
                    r2 += tmp2[j].r;
                    g2 += tmp2[j].g;
                    b2 += tmp2[j].b;
                    a2 += tmp2[j].a;

                }
                if (ni)
                {
                    rs.Set(r1 / ni, g1 / ni, b1 / ni, a1 / ni);
                    dst[x + org1.x + info_dst.wline * (y + org1.y)] = rs.color;
                    rs.Set(r2 / ni, g2 / ni, b2 / ni, a2 / ni);
                    dst[x + org2.x + info_dst.wline * (y + org2.y)] = rs.color;
                }

            }
        }
    } catch (std::exception &e)
    {
        TRACE(e.what());

    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
//
void MemImage::Stretch(MemImage &src1, MemImage &src2, IRect &_rc_src, IRect &_rc_dst, IPoint &org1,
                       IPoint &org2)
{
    if (!ptr) return;
    IRect rc_src = _rc_src;
    IRect rtest, rc_dst = _rc_dst;
    if (!src1.SetAccess() || !src2.SetAccess()) return;

    Transform2F<Vector2F, IRect, float> trans;
    trans.Init(rc_dst, rc_src, false);

    IRect tst(0, 0, src1.info.width, src1.info.height);
    rc_src &= src1.clip;

    tst &= rc_src;

    if (tst.IsEmpty()) return;

    ColorRef rs;
    rs.a = 255;
    rc_dst &= clip;

    if (rc_dst.left < 0)
        rc_dst.left = 0;
    if (rc_dst.IsEmpty()) return;
    int j = rc_dst.bottom - rc_dst.top;
    int N = NumberOfProcessors();

    if (tst.IsEmpty()) return;


    if (rc_src.Width() >= rc_dst.Width())
    {
        if (N < j * 2 && N > 1 && pMultiThreads->TryLock())
        {
            int n = j / N;
            ThreadParam *params = (ThreadParam *) _alloca(sizeof(ThreadParam) * (N + 1));
            for (int k = 0; k < N; ++k)
            {
                params[k].InitStrech(trans, this, org1, org2, rc_dst, &src1, &src2, rc_src,
                                     rc_dst.top + k * n,
                                     k == N - 1 ? rc_dst.bottom : rc_dst.top + (k + 1) * n);
                pMultiThreads->Run(k, (AFX_THREADPROC) ThreadStretch_1, params + k);
            }
            pMultiThreads->Wait();
            pMultiThreads->Unlock();

        } else
            Stretch_1(trans, ptr, org1, org2, info, rc_dst, src1.ptr, src2.ptr, src1.info, rc_src,
                      rc_dst.top, rc_dst.bottom);

    } else
    {
        if (N < j * 2 && N > 1 && pMultiThreads->TryLock())
        {
            int n = j / N;
            ThreadParam *params = (ThreadParam *) _alloca(sizeof(ThreadParam) * (N + 1));
            for (int k = 0; k < N; ++k)
            {
                params[k].InitStrech(trans, this, org1, org2, rc_dst, &src1, &src2, rc_src,
                                     rc_dst.top + k * n,
                                     k == N - 1 ? rc_dst.bottom : rc_dst.top + (k + 1) * n);
                pMultiThreads->Run(k, (AFX_THREADPROC) ThreadStretch_2, params + k);
            }
            pMultiThreads->Wait();
            pMultiThreads->Unlock();

        } else
            Stretch_2(trans, ptr, org1, org2, info, rc_dst, src1.ptr, src2.ptr, src1.info, rc_src,
                      rc_dst.top, rc_dst.bottom);

    }
    SetBmpRop(SRCCOPY);
    src1.ReleaseAccess();
    src2.ReleaseAccess();

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
#if (1)
void MemImage::Stretch(AObject *src, IRect &_rc_src, IRect &_rc_dst, int rop)
{

    if (!ptr) return;
    _rc_dst.OffsetRect(curOrg.x, curOrg.y);
    if (pGDIPrint)
        pGDIPrint->Stretch(src, _rc_src, _rc_dst);
}
void MemImage::Stretch(MemImage &src, IRect &_rc_src, IRect &_rc_dst, int rop)
{

    if (!ptr) return;
    IRect rc_src = _rc_src;
    IRect rtest, rc_dst = _rc_dst;
    if (!src.SetAccess()) return;

    rc_dst.OffsetRect(curOrg.x, curOrg.y);
    if (pGDIPrint)
    {
        pGDIPrint->Stretch(&src, _rc_src, _rc_dst);
        return;
    }
    Transform2F<Vector2F, IRect, float> trans;
    trans.Init(rc_dst, rc_src, false);

    IRect tst(0, 0, src.info.width, src.info.height);
    SetBmpRop(rop);
    rc_src &= src.clip;

    tst &= rc_src;

    if (tst.IsEmpty()) return;

    ColorRef rs;
    rs.a = 255;
    rc_dst &= clip;

    if (rc_dst.left < 0)
        rc_dst.left = 0;
    if (rc_dst.IsEmpty()) return;
    int j = rc_dst.bottom - rc_dst.top;
    int N = NumberOfProcessors();
    if (rc_src.Width() >= rc_dst.Width())
    {
        if (N < j * 2 && N > 1 && pMultiThreads->TryLock())
        {
            int n = j / N;
            ThreadParam *params = (ThreadParam *) _alloca(sizeof(ThreadParam) * (N + 1));
            for (int k = 0; k < N; ++k)
            {
                //void InitStrech(Transform2F<Vector2F, IRect, float> &_trans, MemImage *_dst, IRect &_rc_dst, MemImage *_src, IRect &_rc_src, int _start, int end)

                params[k].InitStrech(trans, this, rc_dst, &src, rc_src, rc_dst.top + k * n,
                                     k == N - 1 ? rc_dst.bottom : rc_dst.top + (k + 1) * n);
                params[k]._ptLineOP = rop == SRCCOPY ? 0 : _ptLineOP;
                pMultiThreads->Run(k, (AFX_THREADPROC) ThreadStretch1, params + k);
            }
            pMultiThreads->Wait();
            pMultiThreads->Unlock();

        } else
            Stretch1(trans, _ptLineOP, ptr, info, rc_dst, src.ptr, src.info, rc_src, rc_dst.top,
                     rc_dst.bottom);

    } else
    {
        if (N < j * 2 && N > 1 && pMultiThreads->TryLock())
        {
            int n = j / N;
            ThreadParam *params = (ThreadParam *) _alloca(sizeof(ThreadParam) * (N + 1));
            for (int k = 0; k < N; ++k)
            {
                //void InitStrech(Transform2F<Vector2F, IRect, float> &_trans, MemImage *_dst, IRect &_rc_dst, MemImage *_src, IRect &_rc_src, int _start, int end)

                params[k].InitStrech(trans, this, rc_dst, &src, rc_src, rc_dst.top + k * n,
                                     k == N - 1 ? rc_dst.bottom : rc_dst.top + (k + 1) * n);
                params[k]._ptLineOP = rop == SRCCOPY ? 0 : _ptLineOP;
                pMultiThreads->Run(k, (AFX_THREADPROC) ThreadStretch2, params + k);
            }
            pMultiThreads->Wait();
            pMultiThreads->Unlock();

        } else
            Stretch2(trans, _ptLineOP, ptr, info, rc_dst, src.ptr, src.info, rc_src, rc_dst.top,
                     rc_dst.bottom);

    }
    SetBmpRop(SRCCOPY);
    src.ReleaseAccess();

}

#else
void MemImage::Stretch(MemImage &src, IRect &_rc_src, IRect &_rc_dst, int rop)
{
	if (!ptr) return;
	IRect rc_src = _rc_src;
	IRect rtest, rc_dst = _rc_dst;
	if (!src.SetAccess()) return;

	rc_dst.OffsetRect(curOrg.x, curOrg.y);
	Transform2F<Vector2F, IRect, float> trans;
	trans.Init(rc_dst, rc_src, false);

	IRect tst(0, 0, src.info.width, src.info.height);
	SetBmpRop(rop);
	rc_src &= src.clip;

	tst &= rc_src;

	if (tst.IsEmpty()) return;

	float left = tst.left;
	float right = tst.right;
	float top = tst.top;
	float bottom = tst.bottom;

	trans.LPtoDP(left, top);
	trans.LPtoDP(right, bottom);
	rtest.Set(int(left + 0.5f), int(top + 0.5f), int(right + 0.5f), int(bottom + 0.5f));
	ColorRef rs;
	rs.a = 255;
	rc_dst &= clip;
	rc_dst &= rtest;
	if (rc_dst.IsEmpty()) return;
	if (rc_src.Width() >= rc_dst.Width())
	{
		float s_x = 1.0f/trans.form.m11ScaleX;
		float s_y = 1.0f/trans.form.m22ScaleY;
		float sc_x = s_x / 2;
		float sc_y = s_y / 2;
		float ms = max(s_x, s_y);
		ColorRef *tmp = (ColorRef *)_alloca(sizeof(ColorRef) * int(ms + 2) * int(ms + 2));
		int minx, limx, limy, miny;
		int ni, r, g, b;
		float x1, y1;

		for (int y = rc_dst.top; y <  rc_dst.bottom; ++y)
		{
			for (int x = rc_dst.left; x < rc_dst.right; ++x)
			{
				ni = r = g = b = 0;
				x1 = (float)x;
				y1 = (float)y;
				trans.DPtoLP(x1, y1);
				minx = max(int(x1 - sc_x + 0.5f), rc_src.left);
				limx = min(int(x1 + sc_x + 1.5f), rc_src.right);
				limy = min(int(y1 + sc_y + 1.5f), rc_src.bottom) * src.info.wline;
				miny = max(int(y1 - sc_y + 0.5f), rc_src.top) * src.info.wline;
				for (int iy = miny;iy < limy; iy +=  src.info.wline)
					for (int ix = minx;ix < limx; ++ix, ++ni)
							tmp[ni].color = src.clrs[ix + iy].color;
				for (int j = 0; j < ni; ++j)
				{
					r += tmp[j].r;
					g += tmp[j].g;
					b += tmp[j].b;
				}
				if (ni)
				{
					rs.Set(r/ni, g/ni, b/ni);
					(this->*_ptLineOP)(ptr + x + info.wline * y, &rs.color, brush->color);
				}

			}
		}
	}
	else
	{
		LinearBox bx;
		Vector2F pos, dmin, dmax, t;
        float _x, _y, t1x, t1y;
        int ix, iy;
        int r, g, b;
        float scale = trans.form.m11ScaleX;
		int X = src.info.width - 1;
		int X1 = src.info.width;
		int Y = src.info.height - 1;
        for (int y = rc_dst.top; y <  rc_dst.bottom; ++y)
		{
			_y = trans.invert.m22ScaleY * ((float)y + trans.invert.m23Dy);
			for (int x = rc_dst.left; x < rc_dst.right; ++x)
			{

				_x = trans.invert.m11ScaleX * ((float)x +  trans.invert.m13Dx);
				//trans.DPtoLP(_x, _y);
				bx.minPoint.x = (int)_x;
				bx.minPoint.y = (int)_y;
				bx.maxPoint.x = (int)_x + 1;
				bx.maxPoint.y = (int)_y + 1;
				pos.x = _x;
				pos.y = _y;
				for (int i = 0; i < 4; ++i)
				{
					ix = i & 1 ? bx.maxPoint.x : bx.minPoint.x;
					if (ix < 0) ix = 0;
					iy = i & 2 ? bx.maxPoint.y : bx.minPoint.y;
					if (iy < 0) iy = 0;
					if (ix > X)
						ix = X;
					if (iy > Y)
						iy = Y;
					bx.Val.s[i].color = src.ptr[ix + iy * X1];
				}
				dmin.x = pos.x - bx.minPoint.x;
				dmin.y = pos.y - bx.minPoint.y;
				dmax.x = bx.maxPoint.x - bx.minPoint.x;
				dmax.y = bx.maxPoint.y - bx.minPoint.y;

                t.x = (float)dmin.x/(float)dmax.x;
                t.y = (float)dmin.y/(float)dmax.y;
				t1x = 1.0f - t.x;
				t1y = 1.0f - t.y;

				r = (int)((bx.Val.s0.r * t1x + bx.Val.s1.r * t.x) * t1y + (bx.Val.s2.r * t1x + bx.Val.s3.r * t.x) * t.y);
				g = (int)((bx.Val.s0.g * t1x + bx.Val.s1.g * t.x) * t1y + (bx.Val.s2.g * t1x + bx.Val.s3.g * t.x) * t.y);
				b = (int)((bx.Val.s0.b * t1x + bx.Val.s1.b * t.x) * t1y + (bx.Val.s2.b * t1x + bx.Val.s3.b * t.x) * t.y);
				rs.r = (BYTE)(r < 255 ? r : 255);
				rs.g = (BYTE)(g < 255 ? g : 255);
				rs.b = (BYTE)(b < 255 ? b : 255);
				if (rop == SRCCOPY)
					ptr[x + info.wline * y] = rs.color;
				else
				(this->*_ptLineOP)(ptr + x + info.wline * y, &rs.color, brush->color);
			}
		}
	}
	SetBmpRop(SRCCOPY);
	src.ReleaseAccess();

}
#endif

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::SetPixel(int x, int y, COLORREF c)
{

    if (!ptr) return;
    x += curOrg.x;
    y += curOrg.y;
    if (!(c & 0xFF000000))
        c |= 0xFF000000;

    if (pGDIPath)
    {
        pGDIPath->SetPixel(x, y, c);
        return;
    }
    if (pGDIPrint)
    {
        pGDIPrint->SetPixel(x, y, c);
        return;
    }
    if (y >= 0 && y < info.height && x >= 0 && x < info.width)
        iPixel.Rop(ptr + y * info.wline + x, c);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::SetPixel(int x, int y, ColorRef &c)
{

    if (!ptr) return;
    x += curOrg.x;
    y += curOrg.y;

    if (pGDIPath)
    {
        pGDIPath->SetPixel(x, y, c);
        return;
    }
    if (pGDIPrint)
    {
        pGDIPrint->SetPixel(x, y, c);
        return;
    }
    if (y >= 0 && y < info.height && x >= 0 && x < info.width)
        iPixel.Rop(ptr + y * info.wline + x, c.color);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
COLORREF MemImage::GetPixel(int x, int y)
{
    if (!ptr) return 0;
    x += curOrg.x;
    y += curOrg.y;
    ColorRef *c = C(x, y);
    return c ? c->color & 0x00FFFFFF : 0;

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Ellipse(IRect &rc, bool fFill)
{
    if (pGDIPath)
    {
        pGDIPath->Ellipse(curOrg.x + rc.left, curOrg.y + rc.top, curOrg.x + rc.right,
                          curOrg.y + rc.bottom);
        return;
    }
    if (pGDIPrint)
    {
        pGDIPrint->Ellipse(curOrg.x + rc.left, curOrg.y + rc.top, curOrg.x + rc.right,
                           curOrg.y + rc.bottom);;
        return;
    }

    if (!ptr) return;
    MArray<IPoint> arr;
    _Ellipse(rc, arr);
    PolygonConvex(arr.GetPtr(), arr.GetLen(), fFill);

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
int MemImage::Fill(int x, int y, COLORREF c, IRect* rc)
{
    x += curOrg.x;
    y += curOrg.y;
    MFifo<IPoint> fifo;

    if (!clip.PtInRegion(x, y)) return 0;
    ColorRef *pc = Image(x, y);
    return pc ? Fill(fifo, x, y, pc->color, c, rc) : 0;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::FillSurfaceBorder(int x, int y, COLORREF c)
{
    if (!ptr) return;
    int j = info.height;
    int w = info.width;

    COLORREF src = Image(x, y)->color;


    if (w && j)
    {
        DWORD *top = ptr;
        MFifo<IPoint> fifo;
        DWORD *right = ptr + info.width - 1;
        DWORD *bottom = ptr + info.wline * (info.height - 1);
        int i;
        for (i = 0; i < w; ++i)
        {
            if (bottom[i] == src)
                Fill(fifo, i, info.height - 1, src, c);
            if (top[i] == src)
                Fill(fifo, i, 0, src, c);
        }
        int k = j * info.wline;
        for (i = 0; i < k; i += info.wline)
        {
            if (top[i] == src)
                Fill(fifo, 0, i / info.wline, src, c);

            if (right[i] == src)
                Fill(fifo, info.width - 1, i / info.wline, src, c);

        }
    }

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::FillSurface(IPoint *pp, int len, MArray<COLORREF> *dst, int number)
{
    if (!ptr) return;
    int J;

    if (!dst)
    {

        int i;
        IRect rc;
        DWClipPolygon<IPoint, int> cc;
        cc.Init(clip);
        MArray<IPoint> poly(len * 2);
        int num = 0;
        MArray<IPoint> apt;
        apt.Copy(pp, len);
        pp = apt.GetPtr();
        for (i = 0; i < len; ++i)
        {
            pp[i].x += curOrg.x;
            pp[i].y += curOrg.y;
        }
        if (pp[len - 1] != pp[0])
            apt.Add(pp[0]);
        pp = apt.GetPtr();
        cc.Clip(pp, len, poly.GetPtr(), num, &rc);
        //PolyLine(poly.GetPtr(), num);
        if (rc.IsEmpty()) return;
        int j = rc.bottom - rc.top;
        int w = rc.right - rc.left;

        IRect nrec = rc;
        nrec.InflateRect(4, 4);
        mask.SetLen(nrec.Width() * nrec.Height() * 2);
        mask.Set(0xff);
        BYTE *_mask = mask.GetPtr();
        AFill test(mask.GetPtr(), nrec.Width(), nrec.Height());
        int nw = nrec.Width();
        IPoint *pt = poly.GetPtr();
        for (i = 0; i < num; ++i)
        {
            pt[i].x -= rc.left - 2;
            pt[i].y -= rc.top - 2;

        }
        test.Polygon(pt, num);
        for (i = 0; i < num; ++i)
        {
            pt[i].x += rc.left - 2;
            pt[i].y += rc.top - 2;

        }
        ColorRef *top = (ColorRef *) ptr + (info.wline * rc.top + rc.left);
        _mask += nw * 2 + 2;
        J = j;
        while (j--)
        {
            for (i = 0; i < w; ++i)
                if (!_mask[i])
                {
                    BYTE &_top_a = top[i].a;
                    if (_top_a == 255)
                        _top_a = number;
                    else
                        _top_a = 255;
                }
            top += info.wline;
            _mask += nw;
        }
    }
    if (dst)
    {
        ColorRef *top = (ColorRef *) ptr;
        if (brush->pattern && brush->typeBrush == tagGBRUSH::BG_IMG)
        {
            IPoint size(brush->pattern->width, brush->pattern->height);
            CRect rc(0, 0, info.width, info.height);
            int width = rc.Width();
            int x_size = rc.Width();
            int y_size = rc.Height();
            int endx = rc.left + size.x;
            int endy = rc.top + size.y;
            int startx = rc.left;
            int starty = rc.top;
            IRect rect;
            ImageInfo2 *pat = brush->pattern;
            DWORD *src = pat->image.GetPtr();
            while (starty < rc.top + y_size)
            {
                endx = rc.left + size.x;
                startx = rc.left;
                while (startx < rc.left + x_size)
                {
                    if (endx > rc.left + x_size)
                        endx = rc.left + x_size;
                    if (endy > rc.top + y_size)
                        endy = rc.top + y_size;
                    rect.SetRect(startx, starty, endx, endy);
                    DWORD *_src = src;
                    DWORD *_dst = ptr + info.wline * rect.top + rect.left;
                    top = (ColorRef *) dst;
                    int _w = rect.Width();
                    int num = rect.Height();
                    while (num--)
                    {
                        for (int i = 0; i < _w; ++i)
                            if (top[i].a != 255)
                                _dst[i] = _src[i];
                        _src += pat->wline;
                        _dst += info.wline;
                    }
                    startx = endx;
                    endx = startx + size.x;
                }
                starty = endy;
                endy = starty + size.y;
            }
        } else
        {
            COLORREF *_dst = dst->GetPtr();
            int L = info.wline * info.height;
            for (int i = 0; i < L; ++i, ++top)
            {
                BYTE a = top->a;
                if (a != 255)
                {
                    if (_dst[a - 1])
                        top->color = _dst[a - 1];
                    else
                        top->a = 255;
                }
            }
        }

    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
int MemImage::Fill(MFifo<IPoint> &fifo, int _x, int _y, COLORREF srcColor, COLORREF dstColor, IRect *rc)
{
    if (!ptr) return 0;
    IPoint p;
    fifo.Push(IPoint(_x, _y));
    bool fBorder = true;
    int n = 1;
    ptr[_x + _y * info.wline] = dstColor;
    if (rc)
        rc->Set(_x, _y, _x, _y);

    while (!fifo.IsEmpty())
    {

        p = fifo.Pop();
        if (p.x + 1 < info.width && ptr[p.x + 1 + p.y * info.wline] == srcColor)
        {
            fifo.Push().Set(p.x + 1, p.y);
            ptr[p.x + 1 + p.y * info.wline] = dstColor;
            if (rc)
                rc->Update(p.x + 1, p.y);
            ++n;
        }
        if (p.x - 1 >= 0 && ptr[p.x - 1 + p.y * info.wline] == srcColor)
        {
            fifo.Push().Set(p.x - 1, p.y);
            ptr[p.x - 1 + p.y * info.wline] = dstColor;
            if (rc)
                rc->Update(p.x - 1, p.y);
            ++n;
        }
        if (p.y + 1 < info.height && ptr[p.x + (p.y + 1) * info.wline] == srcColor)
        {
            fifo.Push().Set(p.x, p.y + 1);
            ptr[p.x + (p.y + 1) * info.wline] = dstColor;
            if (rc)
                rc->Update(p.x, p.y + 1);
            ++n;
        }
        if (p.y - 1 >= 0 && ptr[p.x + (p.y - 1) * info.wline] == srcColor)
        {
            fifo.Push().Set(p.x, p.y - 1);
            ptr[p.x + (p.y - 1) * info.wline] = dstColor;
            if (rc)
                rc->Update(p.x, p.y - 1);
            ++n;
        }

    }
    return n;

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::PolygonConvex(IPoint *pp, int len, bool fFill)
{
    if (!ptr || !pp) return;

    if (pGDIPath)
    {
        pGDIPath->PolygonConvex(pp, len);
        return;
    }
    if (!fFill)
    {
        PolyLine(pp, len);
        return;
    }

    if (!(brush->color & 0xFF000000))
    {
        if (!fFill)
            PolyLine(pp, len);
        return;
    }
    IRect rc;
    DWClipPolygon<IPoint, int> cc;
    cc.Init(clip);
    MArray<IPoint> poly(len * 2);
    int num = 0;
    MArray<IPoint> apt;
    apt.Copy(pp, len);
    if (apt[len - 1] != apt[0])
    {
        apt.Add() = apt[0];
        ++len;
    }
    pp = apt.GetPtr();
    int i;
    for (i = 0; i < len; ++i)
    {
        pp[i].x += curOrg.x;
        pp[i].y += curOrg.y;
    }
    cc.Clip(pp, len, poly.GetPtr(), num, &rc);
    //PolyLine(poly.GetPtr(), num);
    if (rc.IsEmpty()) return;
    int j = rc.bottom - rc.top;
    int w = rc.right - rc.left;

    IRect nrec = rc;
    nrec.InflateRect(4, 4);
    mask.SetLen(nrec.Width() * nrec.Height() * 2);
    mask.Set(0xff);
    BYTE *_mask = mask.GetPtr();
    AFill test(mask.GetPtr(), nrec.Width(), nrec.Height());
    int nw = nrec.Width();
    IPoint *pt = poly.GetPtr();
    for (i = 0; i < num; ++i)
    {
        pt[i].x -= rc.left - 2;
        pt[i].y -= rc.top - 2;

    }
    test.PolygonConvex(pt, num);
    for (i = 0; i < num; ++i)
    {
        pt[i].x += rc.left - 2;
        pt[i].y += rc.top - 2;

    }
    if (brush->pattern && brush->typeBrush == tagGBRUSH::BG_IMG)
        Tile(nrec, 2);
    else
    {
        ColorRef *top = (ColorRef *) ptr + (info.wline * rc.top + rc.left);
        _mask += nw * 2 + 2;
        while (j--)
        {
            for (i = 0; i < w; ++i)
                if (!_mask[i])
                    top[i].color = brush->color;
            top += info.wline;
            _mask += nw;
        }
    }
//	if (!fFill)
    if (pen->color != brush->color)
    {
        IPoint p = curOrg;
        curOrg.Set(0, 0);
        PolyLine(pt, num);
        curOrg = p;
    }

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Polygon(IPoint *pp, int len, bool fFill)
{
    if (!ptr) return;

    if (pGDIPath && pp)
    {
        pGDIPath->Polygon(pp, len);
        return;
    }
    if (!ptr || !pp) return;
    if (!(brush->color & 0xFF000000) || !fFill)
    {

       // if (!fFill)
        {
            PolyLine(pp, len);
            return;
        }
    }
    IRect rc;
    DWClipPolygon<IPoint, int> cc;
    cc.Init(clip);
    MArray<IPoint> poly(len * 2);
    int num = 0;
    MArray<IPoint> apt;
    apt.Copy(pp, len);
    pp = apt.GetPtr();
    int i;
    for (i = 0; i < len; ++i)
    {
        pp[i].x += curOrg.x;
        pp[i].y += curOrg.y;
    }
    if (pp[len - 1] != pp[0])
        apt.Add(pp[0]);
    pp = apt.GetPtr();
    cc.Clip(pp, len, poly.GetPtr(), num, &rc);
    //PolyLine(poly.GetPtr(), num);
    if (rc.IsEmpty()) return;
    int j = rc.bottom - rc.top;
    int w = rc.right - rc.left;

    IRect nrec = rc;
    nrec.InflateRect(4, 4);
    mask.SetLen(nrec.Width() * nrec.Height() * 2);
    mask.Set(0xff);
    BYTE *_mask = mask.GetPtr();
    AFill test(mask.GetPtr(), nrec.Width(), nrec.Height());
    int nw = nrec.Width();
    IPoint *pt = poly.GetPtr();
    for (i = 0; i < num; ++i)
    {
        pt[i].x -= rc.left - 2;
        pt[i].y -= rc.top - 2;

    }
    test.Polygon(pt, num);
    for (i = 0; i < num; ++i)
    {
        pt[i].x += rc.left - 2;
        pt[i].y += rc.top - 2;

    }
    if (brush->pattern && brush->typeBrush == tagGBRUSH::BG_IMG)
        Tile(nrec, 2);
    else
    {
        ColorRef *top = (ColorRef *) ptr + (info.wline * rc.top + rc.left);
        _mask += nw * 2 + 2;
        while (j--)
        {
            for (i = 0; i < w; ++i)
                if (!_mask[i])
                    top[i].color = brush->color;
            top += info.wline;
            _mask += nw;
        }
    }
    if (!fFill)
        if (pen->color & 0xFF000000)
        {
            IPoint p = curOrg;
            curOrg.Set(0, 0);
            PolyLine(pt, num);
            curOrg = p;
        }

/*
	ILine line;
	IRect rc(INT_MAX, INT_MAX, -INT_MAX, -INT_MAX);
	rc.Update(pp[0].x, pp[0].y);
	int i;
	DWClipPolygon<IPoint, int> cc;
	cc.Init(clip, -10);
	MArray<IPoint> p22(len * 2);

	cc.Clip(pp, len, p22.GetPtr(), len, &rc);
	_PolyLine(p22.GetPtr(), len);
	return;
	for(i=1; i < len; ++i)
	{
		rc.Update(pp[i].x, pp[i].y);
		line.Init(pp[i - 1].x, pp[i - 1].y, pp[i].x, pp[i].y);
		line.DrawRestrict((ColorRef *)ptr, info, 0x1, 0xff);
	}
	--i;
	if (pp[0].x != pp[i].x || pp[0].y != pp[i].y)
	{
		line.Init(pp[i].x, pp[i].y, pp[0].x, pp[0].y);
		line.DrawRestrict((ColorRef *)ptr, info, 0x1, 0xff);
	}
	rc.InflateRect(2, 2);
	rc &= clip;
	if (rc.IsEmpty()) return;
	int j = rc.bottom - rc.top;
	int w = rc.right - rc.left;
	if (w && j)
	{

		ColorRef *top = (ColorRef *)ptr + (info.wline * rc.top + rc.left);
		MFifo<IPoint> fifo;
		mask.SetLen(rc.Width() * rc.Height());
		BYTE *ord = mask.GetPtr();
		ColorRef *right = (ColorRef *)ptr + (info.wline * rc.top + rc.right - 1);
		ColorRef *bottom = (ColorRef *)ptr + (info.wline * (rc.bottom - 1) + rc.left);
		Fill(rc, fifo, ord, rc.left, rc.top);

		for (i = 0; i < w; ++i)
		{
			if (bottom[i].a > 2)
				Fill(rc, fifo, ord, rc.left + i, rc.bottom - 1);

			if (top[i].a != 1)
				Fill(rc, fifo, ord, rc.left + i, rc.top);
		}
		int k = j * info.wline;
		for (i = 0; i < k; i += info.wline)
		{
			if (top[i].a > 2)
				Fill(rc, fifo, ord, rc.left, rc.top + i/info.wline);

			if (right[i].a > 2)
				Fill(rc, fifo, ord, rc.right - 1, rc.top + i/info.wline);

		}

		line.SetRop(rop2);
		int y = j;
		while (j--)
		{
			for (i = 0; i < w; ++i)
				if (top[i].a == 2)
					top[i].a = 255;
				else
				if (top[i].a == 1)
				{
					if (!fFill)
						line.Rop((DWORD *)top + i, pen->color);
					else
						top[i].color = brush->color;

				}
				else
				if (top[i].a == 255)
				{
					top[i].color = brush->color;
				}
			top += info.wline;
					IRect rc;
	DWClipPolygon<IPoint, int> cc;
	cc.Init(clip);
	MArray<IPoint> poly(len * 2);
	int num = 0;
	MArray<IPoint> apt;
	apt.Copy(pp, len);
	pp = apt.GetPtr();
	for (int i = 0; i < len; ++i)
	{
		pp[i].x += curOrg.x;
		pp[i].y += curOrg.y;
	}
	if (pp[len - 1] != pp[0])
	apt.Add(pp[0]);
	pp = apt.GetPtr();
	cc.Clip(pp, len, poly.GetPtr(), num, &rc);
	//PolyLine(poly.GetPtr(), num);
	if (rc.IsEmpty()) return;
	int j = rc.bottom - rc.top;
	int w = rc.right - rc.left;

	IRect nrec = rc;
	nrec.InflateRect(4 , 4);
	mask.SetLen(nrec.Width() * nrec.Height() * 2);
	mask.Set(0xff);
	BYTE *_mask = mask.GetPtr();
	AFill test(mask.GetPtr(), nrec.Width(), nrec.Height());
	int nw = nrec.Width();
	IPoint *pt = poly.GetPtr();
	int i;
	for (i = 0; i < num; ++i)
	{
		pt[i].x -= rc.left - 2;
		pt[i].y -= rc.top - 2;

	}
	test.Polygon(pt, num);
	for (i = 0; i < num; ++i)
	{
		pt[i].x += rc.left - 2;
		pt[i].y += rc.top - 2;

	}
	if (brush->pattern && brush->typeBrush == tagGBRUSH::BG_IMG)
		Tile(nrec, 2);
	else
	{
		ColorRef *top = (ColorRef *)ptr + (info.wline * rc.top + rc.left);
		_mask += nw * 2 + 2;
		while (j--)
		{
			for (i = 0; i < w; ++i)
				if (!_mask[i])
					top[i].color = brush->color;
			top += info.wline;
			_mask += nw;
		}
	}
	if (!fFill)
	if (pen->a && pen->color != brush->color)
	{
		IPoint p = curOrg;
		curOrg.Set(0, 0);
		PolyLine(pt, num);
		curOrg = p;
	}
		if (brush->pattern && brush->typ
		}

	}
*/
}

#include "RuningPolygon.h"

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::Frame(IPoint *pp, int len, IPoint *pp2, int len2)
{

    if (!ptr || !pp) return;
    RuningPoygon p1, p2;
    MArray<IPoint> apt, apt2;
    apt.Copy(pp, len);
    pp = apt.GetPtr();
    IRect rc(pp[0].x + curOrg.x, pp[0].y + curOrg.y, pp[0].x + curOrg.x, pp[0].y + curOrg.y);
    int i;
    for (i = 0; i < len; ++i)
    {
        rc.left = min(pp[i].x + curOrg.x, rc.left);
        rc.top = min(pp[i].y + curOrg.y, rc.top);
        rc.right = max(pp[i].x + curOrg.x, rc.right);
        rc.bottom = max(pp[i].y + curOrg.y, rc.bottom);
        pp[i].x += curOrg.x;
        pp[i].y += curOrg.y;
    }
    apt2.Copy(pp2, len);
    pp2 = apt2.GetPtr();
    IRect rc2(pp2[0].x + curOrg.x, pp2[0].y + curOrg.y, pp2[0].x + curOrg.x, pp2[0].y + curOrg.y);
    for (i = 0; i < len2; ++i)
    {
        rc2.left = min(pp2[i].x + curOrg.x, rc2.left);
        rc2.top = min(pp2[i].y + curOrg.y, rc2.top);
        rc2.right = max(pp2[i].x + curOrg.x, rc2.right);
        rc2.bottom = max(pp2[i].y + curOrg.y, rc2.bottom);
        pp2[i].x += curOrg.x;
        pp2[i].y += curOrg.y;
    }
    if ((rc & rc2) == rc)
    {
        int l = len2;
        len2 = len;
        IPoint *p = pp2;
        pp2 = pp;
        pp = p;
    }

    rc = clip & rc;
    if (rc.IsEmpty()) return;
    p2.Init(pp2, len);
    Vector2D i1[2];
    Vector2D i2[2];
    apt.SetLen(0);
    i1[0].Set(rc.left, rc.top);
    i1[1].Set(rc.right, rc.bottom);
    if (p2.PtIn(i1[0]) && p2.PtIn(i1[1]))
        return;
    p1.Init(pp, len);
    Segment2D cl;
    cl.org.x = rc.left;
    cl.dest.x = rc.right;
    int n1 = 1, n2 = 1;
    for (i = rc.top; i < rc.bottom; ++i)
    {
        cl.org.y = i;
        cl.dest.y = i;
        if ((n1 = p1.IsClipXRay(cl, i1)))
        {
            if ((n2 = p2.IsClipXRay(cl, i2)))
            {
                if (cl.org == i2[0])
                {
                    if (i2[1].x  < i1[1].x && n2 > 1 && n1 > 1)
                    {
                        if (cl.dest == i2[1])

                        {
                            continue;
                        }
                        apt.Add().Set((int) (i2[1].x + 0.5), i);
                        apt.Add().Set((int) (i1[1].x + 0.5), i);
                    }
                    else
                    {
                        ++n1;
                    }
                    //apt.Add().Set(-1, i);
                    //apt.Add().Set(-1, i);

                } else
                {
                    if ((int)i1[0].x >= rc.left && (int)i2[0].x <= rc.right && i1[0].x  < i2[0].x)
                    {
                        apt.Add().Set((int) (i1[0].x + 0.5), i);
                        apt.Add().Set((int) (i2[0].x + 0.5), i);
                    }
                    else
                    {
                        ++n1;
                    }
                    if ((int)i2[1].x >= rc.left && (int)i1[1].x  <= rc.right &&  i2[1].x  < i1[1].x)
                        {
                            apt.Add().Set((int) (i2[1].x + 0.5), i);
                            apt.Add().Set((int) (i1[1].x + 0.5), i);
                        }
                    else
                    {
                        ++n1;
                    }
                }

            } else
        //    if (i1[0].x <= i1[1].x)
            {
                //if (i1[0].x == i1[1].x)
                    if (p1.PtIn(cl.dest))
                            i1[1] = cl.dest;

                if (i1[0].x >= rc.left && (int)i1[1].x <= rc.right && i1[0].x < i1[1].x)
                {
                apt.Add().Set((int) (i1[0].x + 0.5), i);
                apt.Add().Set((int) (i1[1].x + 0.5), i);
                }
                //	apt.Add().Set(-1, i);
                //apt.Add().Set(-1, i);
            }
        }
      

    }
    if (brush->pattern && brush->typeBrush == tagGBRUSH::BG_IMG)
    {
        ImageInfo2 *pat = brush->pattern;
        DWORD *__src, *_src, *src = pat->image.GetPtr();
        int w = pat->width;
        pp = apt.GetPtr();
        for (int i = 0, l = apt.GetLen(); i < l; i += 2)
        {

            DWORD *_dst = ptr + info.wline * pp[i].y + pp[i].x;
            int _w = pp[i + 1].x - pp[i].x;
            int h = ((pp[i].y - rc.top) % pat->height);
            _src = src + pat->wline * h;
            __src = _src;
            for (int k = (pp[i].x - rc.left) % pat->width, j = 0; j < _w; ++j, ++k)
            {
                if (k == w)
                {
                    k = 0;
                    _src = __src;
                }
                _dst[j] = _src[k];
                if ((_dst[j] & 0xFF000000) != 0xFF000000)
                {
                    _dst[j] |= 0xFF000000;
                }
            }
        }
    } else
    {
        pp = apt.GetPtr();
        for (int j, i = 0, l = apt.GetLen(); i < l; i += 2)
        {
            if (pp[i].x < 0 || pp[i].y < 0) continue;
            ColorRef *top = (ColorRef *) ptr + (info.wline * pp[i].y + pp[i].x);
            int w = pp[i + 1].x - pp[i].x;
            for (j = 0; j < w; ++j)
            {
                top[j].color = brush->color;
            }
        }
    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::FillRects(MArray<POINT> &org, int _w, int _h, COLORREF clr)
{
    if (!ptr) return;

    POINT *pt = org.GetPtr();
    IRect rc;
    clr |= 0xff000000;
    for (int i = 0, l = org.GetLen(); i < l; ++i)
    {
        rc.Set(curOrg.x + pt[i].x, curOrg.y + pt[i].y, curOrg.x + pt[i].x + _w,
               curOrg.y + pt[i].y + _h);
        rc &= clip;
        if (!rc.IsEmpty())
        {
            DWORD *_dst, *dst = ptr + (info.wline * rc.top + rc.left);
            int j = rc.bottom - rc.top;
            int w, _w = rc.right - rc.left;
            if (_w && j)
            {
                while (j--)
                {
                    _dst = dst;
                    w = _w;
                    while (w--)
                    {
                        *_dst = clr;
                        ++_dst;

                    }
                    dst += info.wline;

                }
            }
        }
    }

}


//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::PatBlt(IRect &_rc, int rop)
{
    if (!ptr) return;
    if (rop == PATCOPY)
    {
        FillRect(_rc, *brush);
        return;
    }
    int j;
    SetBmpRop(rop);

    IRect rc = clip;
    IRect rc1 = _rc;
    rc1.OffsetRect(curOrg.x, curOrg.y);
    rc &= rc1;
    if (rc.IsEmpty()) return;
    if (!brush->pattern)
    {
        if (!rc.IsEmpty())
        {
            DWORD *_dst = ptr + (info.wline * rc.top + rc.left);
            j = rc.bottom - rc.top;
            int w = rc.right - rc.left;
            if (w && j)
            {
                while (j--)
                {
                    (this->*_patLineOP)(_dst, brush->color, w);
                    _dst += info.wline;
                }
            }
        }
    } else if (brush->typeBrush == tagGBRUSH::BG_IMG)
    {
        IPoint size(brush->pattern->width, brush->pattern->height);
        int width = rc.Width();
        int x_size = rc.Width();
        int y_size = rc.Height();
        int endx = rc.left + size.x;
        int endy = rc.top + size.y;
        int startx = rc.left;
        int starty = rc.top;
        IRect rect;
        DWORD clr = 0xFFFFFFFF;
        ImageInfo2 *pat = brush->pattern;
        DWORD *src = pat->image.GetPtr();
        while (starty < rc.top + y_size)
        {
            endx = rc.left + size.x;
            startx = rc.left;
            while (startx < rc.left + x_size)
            {
                if (endx > rc.left + x_size)
                    endx = rc.left + x_size;
                if (endy > rc.top + y_size)
                    endy = rc.top + y_size;
                rect.SetRect(startx, starty, endx, endy);
                DWORD *_src = src;
                DWORD *_dst = ptr + info.wline * rect.top + rect.left;
                int _w = rect.Width();
                int num = rect.Height();
                while (num--)
                {
                    (this->*_bmpLineOP)(_dst, _src, clr, _w);
                    _src += pat->wline;
                    _dst += info.wline;
                }
                startx = endx;
                endx = startx + size.x;
            }
            starty = endy;
            endy = starty + size.y;
        }

    }
    SetBmpRop(SRCCOPY);


}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void
MemImage::PathBezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4,
                     MArray<IPoint> &pt)
{
    float A = y4 - y1;
    float B = x1 - x4;
    float C = -B * y1 - x1 * A;
    float AB = A * A + B * B;
    if (AB <= 1.0e-6)
    {
        IPoint &p = pt.Add();
        p.x = (int) (x4);
        p.y = (int) (y4);
        return;
    }
    float f1 = A * x2 + B * y2 + C;
    if ((f1 * f1) < AB)
    {
        f1 = A * x3 + B * y3 + C;
        if ((f1 * f1) < AB)
        {
            IPoint &p = pt.Add();

            p.x = (int) (x4);
            p.y = (int) (y4);
            return;
        }
    }
    float x12 = x1 + x2;
    float y12 = y1 + y2;
    float x23 = x2 + x3;
    float y23 = y2 + y3;
    float x34 = x3 + x4;
    float y34 = y3 + y4;

    float x1223 = x12 + x23;
    float y1223 = y12 + y23;
    float x2334 = x23 + x34;
    float y2334 = y23 + y34;

    float x = x1223 + x2334;
    float y = y1223 + y2334;

    PathBezier(x1, y1, x12 / 2.0f, y12 / 2.0f, x1223 / 4.0f, y1223 / 4.0f, x / 8.0f, y / 8.0f, pt);
    PathBezier(x / 8.0f, y / 8.0f, x2334 / 4.0f, y2334 / 4.0f, x34 / 2.0f, y34 / 2.0f, x4, y4, pt);
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::PolyBezier(IPoint *p, int count, bool fFill)
{
    if (count < 4) return;
    MArray<IPoint> pt;
    pt.SetAddedRealSize(-1);
    pt.Add(*p);
    for (int i = 1; i + 2 < count; i += 3)
    {
        int n = pt.GetLen() - 1;
        PathBezier((float) pt[n].x, (float) pt[n].y, (float) p[i].x, (float) p[i].y,
                   (float) p[i + 1].x, (float) p[i + 1].y, (float) p[i + 2].x, (float) p[i + 2].y,
                   pt);
    }
    if (pGDIPath)
    {
        pGDIPath->Path(pt.GetPtr(), pt.GetLen());
        return;
    }
    if (pGDIPrint)
    {
        pGDIPrint->Path(pt.GetPtr(), pt.GetLen());
        return;
    }
    if (!fFill)
        PolyLine(pt.GetPtr(), pt.GetLen());
    else
        Polygon(pt.GetPtr(), pt.GetLen());
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::PolyPolygon(int n, int *count, IPoint *points)
{
    int l;
    for (int i = 0; i < n; ++i)
    {
        l = count[i];
        Polygon(points, l);
        points += l;
    }


}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
BOOL MemImage::GetRGB(MArray<char> &buff, bool fBmp, bool fReverse)
{
    if (!ptr) return 0;

    BOOL ret = 0;
    char *src = (char *) ptr;
    if (src)
    {

        int _width = fBmp ? WIDTHBYTES(info.width * 24) : info.width * 3;
        int y = info.height;
        if (buff.GetLen() != _width * y)
            buff.SetLen(_width * y);
        char *lpBits = buff.GetPtr();
        char *dst = fReverse ? lpBits + (y - 1) * _width : lpBits;
        
        int w = info.width;
        int _w = info.wline * sizeof(DWORD);
        if (!fReverse) _width = -_width;

        while (y--)
        {
            for (int i = 0; i < w; ++i)
            {
                dst[i * 3 + 2] = src[i * 4];
                dst[i * 3 + 1] = src[i * 4 + 1];
                dst[i * 3] = src[i * 4 + 2];
            }
            src += _w;
            dst -= _width;
        }

        ret = TRUE;
    }
    return ret;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
BOOL MemImage::SetRGB(MArray<char> &buff, bool fBmp)
{
    if (!ptr) return 0;

    BOOL ret = 0;
    char *dst = (char *) ptr;
    if (dst)
    {

        int _width = fBmp ? WIDTHBYTES(info.width * 24) : info.width * 3;
        int y = info.height;
        if (buff.GetLen() < y * _width)
            return false;
        char *lpBits = buff.GetPtr();
        char *src = lpBits + (y - 1) * _width;
        int w = info.width;
        int _w = info.wline * sizeof(DWORD);

        while (y--)
        {
            for (int i = 0; i < w; ++i)
            {
                dst[i * 4 + 3] = (BYTE) 0xff;
                dst[i * 4 + 2] = src[i * 3];
                dst[i * 4 + 1] = src[i * 3 + 1];
                dst[i * 4] = src[i * 3 + 2];

            }
            dst += _w;
            src -= _width;
        }

        ret = TRUE;
    }
    return ret;
}

#ifdef WIN32
WIN32HDC::WIN32HDC(MemImage *in, HRGN r)
	{
		memImage = 0;
		rgn = 0;
		hOldDC = hDC = 0;
		hbmp = 0;
		fKillRgn = 0;
		if (!in) return;

		memImage = in;

		BITMAPINFO bmInfoHdrRGB;
		ABITMAPINFOHEADER *head = (ABITMAPINFOHEADER *)&bmInfoHdrRGB;
		WinBmp::InitBitmapInfoHeader (head, in->info.width, in->info.height, 0, 1);
		bmInfoHdrRGB.bmiColors[0].rgbBlue = 0;
		bmInfoHdrRGB.bmiColors[0].rgbGreen = 0;
		bmInfoHdrRGB.bmiColors[0].rgbRed = 0;
		bmInfoHdrRGB.bmiColors[0].rgbReserved = 0;

		hDC = ::GetDC ((HWND)0);
		void *pBits;
		hbmp = ::CreateDIBSection(hDC, &bmInfoHdrRGB, DIB_RGB_COLORS, &pBits, 0, 0);
		data.Attach((char *)pBits, head->biSizeImage);
		if (!in->GetRGB(data, true))
		{
			data.Detach();
			ReleaseDC (0, hDC);
			::DeleteObject(hbmp);
			hDC = 0;
			return;
		}
		ReleaseDC (0, hDC);
		hOldDC  = ::GetDC ((HWND)0);
	    hDC = ::CreateCompatibleDC( hOldDC);
		hOldBmp = (HBITMAP)SelectObject(hDC, hbmp);
		fKillRgn = r == 0;
		if (!fKillRgn)
		{
			IRect rc = in->GetClipBox();
			rgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
		}
		else
			rgn = r;
	    SelectClipRgn(hDC, rgn);
	}
	WIN32HDC::~WIN32HDC()
	{
	   if (!hDC) return;
		memImage->SetRGB(data, true);
		SelectObject(hDC, hOldBmp);
		DeleteDC(hDC);
   		ReleaseDC(0, hOldDC);
		::DeleteObject(hbmp);
		data.Detach();
		::DeleteObject(rgn);
	}
	HDC MemImage::CreateHDC()
	{
		if (!hDC)
			hDC = new WIN32HDC(this);
		if (hDC->hDC)
			return hDC->hDC;
		else
			delete hDC;
		return 0;
	}
	void MemImage::ReleaseHDC()
	{
		if (hDC)
			delete hDC;
		hDC = 0;

	}

	HBITMAP MemImage::CreateHBITMAP()
	{
		HDC hOldDC = 0, hDC = 0;
		HBITMAP hbmp = 0;
		BITMAPINFO bmInfoHdrRGB;
		ABITMAPINFOHEADER *head = (ABITMAPINFOHEADER *)&bmInfoHdrRGB;
		WinBmp::InitBitmapInfoHeader (head, info.width, info.height, 0, 1);
		bmInfoHdrRGB.bmiColors[0].rgbBlue = 0;
		bmInfoHdrRGB.bmiColors[0].rgbGreen = 0;
		bmInfoHdrRGB.bmiColors[0].rgbRed = 0;
		bmInfoHdrRGB.bmiColors[0].rgbReserved = 0;

		hDC = ::GetDC ((HWND)0);
		void *pBits;
		MArray<char> data;
		hbmp = ::CreateDIBSection(hDC, &bmInfoHdrRGB, DIB_RGB_COLORS, &pBits, 0, 0);
		data.Attach((char *)pBits, head->biSizeImage);
		if (!GetRGB(data, true))
		{
			data.Detach();
			ReleaseDC (0, hDC);
			::DeleteObject(hbmp);
			return 0;
		}
		data.Detach();
		ReleaseDC (0, hDC);
		return hbmp;

	}

#endif

//---------------------------------------------------------------
//
//---------------------------------------------------------------
BOOL MemImage::BeginPath()
{
    if (pGDIPath)
        return FALSE;
    pGDIPath = new GDIPath();
    pGDIPath->b = *brush;
    pGDIPath->p = *pen;
    return TRUE;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
BOOL MemImage::AttachPath(GDIPath *p)
{
    if (pGDIPath)
        delete pGDIPath;
    pGDIPath = p;
    pGDIPath->b = *brush;
    pGDIPath->p = *pen;
    return TRUE;
}

BOOL MemImage::SetPrintPath(GDIPath *p)
{

    pGDIPrint = p;
    if (p)
    {
        pGDIPrint->b = *brush;
        pGDIPrint->p = *pen;
    }
    return TRUE;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
GDIPath *MemImage::DetachPath()
{
    GDIPath *p = pGDIPath;
    pGDIPath = 0;
    return p;

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
BOOL MemImage::EndPath()
{
    if (!pGDIPath) return FALSE;
    return TRUE;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
BOOL MemImage::StrokePath()
{
    if (!pGDIPath) return FALSE;
    GDIPath *p = pGDIPath;
    pGDIPath = 0;
    BOOL ret = p->StrokePath(this);
    delete p;
    return ret;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
BOOL MemImage::FillPath()
{
    if (!pGDIPath) return FALSE;
    GDIPath *p = pGDIPath;
    pGDIPath = 0;
    BOOL ret = p->FillPath(this);
    delete p;
    return ret;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
BOOL MemImage::StrokeAndFillPath()
{
    if (!pGDIPath) return FALSE;
    GDIPath *p = pGDIPath;
    pGDIPath = 0;
    BOOL ret = p->StrokeAndFillPath(this);
    delete p;
    return ret;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::RoundRect(int x, int y, int width, int height, int radius)
{
    MArray<IPoint> arr;
    _RoundRect(x, y, width, height, radius, arr);
    Polygon(arr.GetPtr(), arr.GetLen(), true);

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::_RoundRect(int x, int y, int width, int height, int radius, MArray<IPoint> &cp)
{
    if (!ptr) return;

    if (width < 0)
    {
        width = -width;
        x = x - width;
    }
    if (height < 0)
    {
        height = -height;
        y = y - height;
    }
    if (radius == 0)
    {
        cp.SetLen(5);
        cp[0].SetPoint(x, y);
        cp[1].SetPoint(x + width, y);
        cp[2].SetPoint(x + width, y + height);
        cp[3].SetPoint(x, y + height);
        cp[4].SetPoint(x, y);

        return;
    }
    if (width == 0 || height == 0) return;
    //1
    MArray<IPoint> arc, arc2;
    arc.SetAddedRealSize(-1);
    arc2.SetAddedRealSize(-1);

    int xx = 0;
    int yy = (int) radius;
    int d = 1 - yy;
    int delta1 = 3;
    int delta2 = -2 * yy + 5;

    arc2.Add().Set(xx, yy);
    while (yy > xx)
    {
        if (d < 0)
        {

            d += delta1;
            delta1 += 2;
            delta2 += 2;
            ++xx;
        } else
        {
            d += delta2;
            delta1 += 2;
            delta2 += 4;

            ++xx;
            --yy;
        }
        arc2.Add().Set(xx, yy);
        arc.Add().Set(yy, xx);
    }
    arc2.Swap();
    arc += arc2;
    --width;
    --height;
    int cx = x + radius;
    int cy = y + radius;
    int i = 0, l = arc.GetLen();
    cp.Add().Set(x, y + height - radius);
    cp.Add().Set(x, y + radius);
    IPoint *parc = arc.GetPtr();

    for (i = 0; i < l; ++i)
        cp.Add().Set(cx - parc[i].x, cy - parc[i].y);

    cp.Add().Set(x + radius, y);
    cp.Add().Set(x + width - radius, y);

    cx = x + width - radius;
    cy = y + radius;
    for (i = 0; i < l; ++i)
        cp.Add().Set(cx + parc[i].y, cy - parc[i].x);
    cp.Add().Set(x + width, y + radius);
    cp.Add().Set(x + width, y + height - radius);
    cx = x + width - radius;
    cy = y + height - radius;
    for (i = l - 1; i >= 0; --i)
        cp.Add().Set(cx + parc[i].y, cy + parc[i].x);
    cp.Add().Set(x + width - radius - 1, y + height);
    cp.Add().Set(x + radius, y + height);
    cx = x + radius;
    cy = y + height - radius;
    for (i = l - 1; i >= 0; --i)
        cp.Add().Set(cx - parc[i].x, cy + parc[i].y);
    cp.Add().Set(x, y + height - radius);

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::_Ellipse(IRect &rc, MArray<IPoint> &cp)
{
    MArray<IPoint> pt;
    MArray<IPoint> pt2;
    pt2.SetAddedRealSize(-1);
    pt.SetAddedRealSize(-1);
    cp.SetAddedRealSize(-1);
    int dX = rc.right - rc.left;
    int dY = rc.bottom - rc.top;
    int X = (rc.right + rc.left) / 2;
    int Y = (rc.bottom + rc.top) / 2;
    dX >>= 1;
    dY >>= 1;

    int xr2 = (dX * dX) << 1;
    int yr2 = (dY * dY) << 1;
    if (xr2 < 0 || yr2 < 0)
        return;
    int x = 0;
    int y = dY;
    int d = (yr2 - xr2 * y) + (xr2 >> 1);
    int xm = (int) (xr2 / sqrt(float((xr2 + yr2) << 1)) - 1);
    pt.Add().SetPoint(x, y);
    while (x < xm)
    {
        if (y > 0)
        {
            if (d > 0)
            {
                --y;
                d -= xr2 * (y << 1);
            }
        }
        d += yr2 * ((x << 1) + 3);
        ++x;
        pt.Add().SetPoint(x, y);
    }
    d = (xr2 - yr2 * dX) + (yr2 >> 1);
    x = dX;
    y = 0;
    pt2.Add().SetPoint(x, y);


    xm += 2;

    bool fD;
    while (x >= 0 && x >= xm)
    {
        if ((fD = (d > 0)))
        {
            --x;
            d -= yr2 * x * 2;
        }
        d += xr2 * ((y << 1) + 3);
        ++y;
        pt2.Add().SetPoint(x, y);
    }
    pt2.Swap();
    pt += pt2;
    int i = 0;
    IPoint *ppt = pt.GetPtr();
    int l = pt.GetLen();
    for (i = 0; i < l; ++i)
        cp.Add().SetPoint(X - ppt[i].x, Y - ppt[i].y);
    for (i = l - 1; i >= 0; --i)
        cp.Add().SetPoint(X - ppt[i].x, Y + ppt[i].y);

    for (i = 0; i < l; ++i)
        cp.Add().SetPoint(X + ppt[i].x, Y + ppt[i].y);
    for (i = l - 1; i >= 0; --i)
        cp.Add().SetPoint(X + ppt[i].x, Y - ppt[i].y);

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
bool MemImage::DrawText(LPCSTR lpchText, int cchText, IRect *lprc, UINT flg, MVector<MArray<IPoint> >* out)
{
    if (!ptr || !font || !lpchText) return 0;
    WString s = lpchText;
    return DrawText(s.GetPtr(), cchText, lprc, flg, out);

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
bool MemImage::DrawText(const wchar_t *lpchText, int cchText, IRect *lprc, UINT flg, MVector<MArray<IPoint> >* out)
{
    if (!ptr || !font || !lpchText || !wcslen(lpchText)) return 0;
    WString s = lpchText;
    if (cchText && wcslen(lpchText) != cchText)
        s[cchText] = 0;

    wchar_t *str = s.GetPtr();
    IRect old_clip = clip;
    IRect rgn = *lprc;
    if (!(flg & MAT_CALCRECT))
    {
        rgn.OffsetRect(curOrg.x, curOrg.y);
        clip = clip & rgn;
    }
    if ((flg & MAT_CALCRECT) || !clip.IsRectNull())
    {
        int oldbk = bkmode;
        bkmode = TRANSPARENT;
        wchar_t *wrem = new wchar_t[wcslen(str) + 1];
        wchar_t **st = new wchar_t *[wcslen(str)];
        size_t i = 0, num_str = 0, j = 0;
        for (size_t l = wcslen(str); i < l; ++i)
        {
            if (str[i] == '\r')
                continue;
            if (str[i] == '\t')
                wrem[j++] = ' ';
            else
                wrem[j++] = str[i];
            if (str[i] == '\n')
            {
                --j;
                wrem[j] = 0;
                st[num_str] = new wchar_t[j + 1];
                memmove(st[num_str++], wrem, sizeof(wchar_t) * (j + 1));
                j = 0;
                continue;
            }
        }
        wrem[j] = 0;
        if (wcslen(wrem) > 0)
        {
            st[num_str] = new wchar_t[j + 1];
            memmove(st[num_str++], wrem, sizeof(wchar_t) * (j + 1));
        }
        delete[] wrem;
        if (num_str)
        {
            if (!(flg & MAT_CALCRECT))
            {
                if (oldbk != TRANSPARENT)
                {
                    tagGBRUSH b(bkcolor);
                    FillRect(*lprc, b);
                }
                PutText(rgn.left, rgn.top, rgn.right, rgn.bottom, st, (int) num_str, flg, out);
            } else
            {
                int i = 0, w = 0, h = 0;
                IRect r;
                for (; i < (int) num_str; ++i)
                {
                    if (wcslen(st[i]))
                        r = font->GetStringRect(st[i]);
                    else
                        r = font->GetStringRect(L"Wy");
                    r.InflateRect(INFLATE_RECT_FONT, INFLATE_RECT_FONT);
                    w = max(w, r.Width());
                    h += r.Height();
                }
                lprc->Set(0, 0, w, h);
            }


        }
        for (i = 0; i < num_str; ++i)
            delete[] st[i];
        delete[] st;
        bkmode = oldbk;
    }
    clip = old_clip;
    return 0;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
bool MemImage::TextOut(int x, int y, LPCSTR str, int len, MVector<MArray<IPoint> >* out)
{
    if (!ptr || !font || !str) return 0;
    x += curOrg.x;
    y += curOrg.y;

    if (str)
    {
        WString s(str);
        return TextOut(x, y, s.GetPtr(), len, out);

    }
    return 0;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
bool MemImage::TextOut(int x, int y, const wchar_t *str, int len, MVector<MArray<IPoint> > *out)
{
    if (!ptr || !font || !str) return 0;
    x += curOrg.x;
    y += curOrg.y;

    if (str)
    {
        WString s(str);
        if (len != wcslen(str))
            s[len] = 0;
        if (bkmode != TRANSPARENT)
        {
            IRect rc = font->GetStringRect(s);
            if (rc.IsRectNull()) return FALSE;

            rc.InflateRect(INFLATE_RECT_FONT, INFLATE_RECT_FONT);
            if (rc.left)
                rc.right -= rc.left;
            if (rc.top)
                rc.bottom -= rc.top;

            rc.top = rc.left = 0;

            rc.OffsetRect(x, y);
            rc &= clip;
            DWORD *_dst = ptr + (info.wline * rc.top + rc.left);
            int j = rc.bottom - rc.top;
            int w = rc.right - rc.left;
            if (w && j)
            {
                DWORD *h = _dst;
                for (int i = 0; i < w; ++i)
                    h[i] = bkcolor;
                w *= sizeof(DWORD);
                while (j--)
                {
                    memcpy(_dst, h, w);
                    _dst += info.wline;
                }
            }
        }
        if (pGDIPrint)
        {
            pGDIPrint->OutString(s, x, y, text_color, clip);
            return 1;
        }
        if (out)
        font->OutString(s, x, y, *out);
        font->OutString(s, x, y, text_color, ptr, info.width, info.height, &clip);
        return 1;
    }
    return 0;
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::PutText(int left, int top, int right, int bottom, char **list, int num_str, int flg, MVector<MArray<IPoint> >* out)
{
    int h1 = bottom - top;
    IRect r1, r;
    r1.Set(left, top, right, bottom);
    int i = 0, h2 = 0;
    for (; i < num_str; ++i)
    {
        if (strlen(list[i]))
            r = GetStringRect(list[i]);
        else
            r = GetStringRect("W");
        h2 += r.bottom;
    }

    if (h2 < h1)
    {
        if (flg & MAT_VCENTER)
        {
            if ((h1 - h2) >= 0)
                r1.top += (r1.bottom - r1.top - h2) / 2;
        } else if (flg & MAT_BOTTOM)
        {
            if ((h1 - h2) >= 0)
                r1.top += (h1 - h2);
        }
    }
    h1 = r.bottom;
    int x, y, k = 0;
    for (i = 0; i < num_str; i++)
    {
        if (*(list[i]))
            r = GetStringRect(list[i]);
        else
            r.right = r.bottom = 0;

        if (flg & MAT_RIGHT)
        {
            x = r1.right - (r.right + 1);
            y = r1.top;
        } else if (flg & MAT_CENTER)
        {
            x = r1.left + (r1.right - r1.left) / 2 - r.right / 2;
            y = r1.top;
        } else
        {
            x = r1.left;
            y = r1.top;
        }
        if (*(list[i]))
            TextOut(x - curOrg.x, y - curOrg.y, list[i], (int) strlen(list[i]), out);
        r1.top += h1;
    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void
MemImage::PutText(int left, int top, int right, int bottom, wchar_t **list, int num_str, int flg, MVector<MArray<IPoint> >* out)
{
    int h1 = bottom - top;
    IRect r1, r;
    r1.Set(left, top, right, bottom);
    int i = 0, h2 = 0;
    for (; i < num_str; ++i)
    {
        if (wcslen(list[i]))
            r = GetStringRect(list[i]);
        else
            r = GetStringRect(L"Wy");
        h2 += r.bottom;
    }

    if (h2 <= h1)
    {
        if (flg & MAT_VCENTER)
        {
            if ((h1 - h2) >= 0)
                r1.top += (r1.bottom - r1.top - h2) / 2;
        } else if (flg & MAT_BOTTOM)
        {
            if ((h1 - h2) >= 0)
                r1.top += (h1 - h2);
        }
    }
    h1 = r.bottom;
    int x, y, k = 0;
    for (i = 0; i < num_str; i++)
    {
        if (*(list[i]))
            r = GetStringRect(list[i]);
        else
            r.right = r.bottom = 0;
        h1 = r.bottom;

        if (flg & MAT_RIGHT)
        {
            x = r1.right - (r.right + 1);
            y = r1.top;
        } else if (flg & MAT_CENTER)
        {
            x = r1.left + (r1.right - r1.left) / 2 - r.right / 2;
            y = r1.top;
        } else
        {
            x = r1.left;
            y = r1.top;
        }
        if (*(list[i]))
            TextOut(x - curOrg.x, y - curOrg.y, list[i], (int) wcslen(list[i]), out);
        r1.top += h1;
    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
IRect MemImage::GetStringRect(LPCSTR str, int len)
{
    IRect rc;
    if (!ptr || !font) return rc;

    if (str)
    {
        if (len && len != strlen(str))
        {
            WString s(str);
            s[len] = 0;
            rc = font->GetStringRect(s);
        } else
            rc = font->GetStringRect(str);

        if (!rc.IsRectNull())
        {
            rc.InflateRect(INFLATE_RECT_FONT, INFLATE_RECT_FONT);
            if (rc.left)
                rc.right -= rc.left;
            if (rc.top)
                rc.bottom -= rc.top;

            rc.top = rc.left = 0;
        }
    }
    return rc;

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
IRect MemImage::GetStringRect(const wchar_t *str, int len)
{
    IRect rc;
    if (!ptr || !font) return rc;
    if (str)
    {
        if (len && len != wcslen(str))
        {
            WString s(str);
            s[len] = 0;
            rc = font->GetStringRect(s);
        } else
            rc = font->GetStringRect(str);

        if (!rc.IsRectNull())
        {
            rc.InflateRect(INFLATE_RECT_FONT, INFLATE_RECT_FONT);
            if (rc.left)
                rc.right -= rc.left;
            if (rc.top)
                rc.bottom -= rc.top;

            rc.top = rc.left = 0;
        }
    }
    return rc;

}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::TileWithMask(int *mask, IRect &rc, int offset)
{
    if (!ptr) return;

    IPoint size(brush->pattern->width, brush->pattern->height);

    int width = rc.Width();
    rc.OffsetRect(offset, offset);
    int x_size = rc.Width();
    int y_size = rc.Height();
    int endx = rc.left + size.x;
    int endy = rc.top + size.y;
    int startx = rc.left;
    int starty = rc.top;
    IRect rect;
    ImageInfo2 *pat = brush->pattern;
    DWORD *src = pat->image.GetPtr();
    int *_mask = mask;
    while (starty < rc.top + y_size)
    {
        endx = rc.left + size.x;
        startx = rc.left;
        while (startx < rc.left + x_size)
        {
            if (endx > rc.left + x_size)
                endx = rc.left + x_size;
            if (endy > rc.top + y_size)
                endy = rc.top + y_size;
            rect.SetRect(startx, starty, endx, endy);
            DWORD *_src = src;
            int *ms = _mask + width * (rect.top - rc.top) + (rect.left - rc.left);
            DWORD *_dst = ptr + info.wline * rect.top + rect.left;
            int _w = rect.Width();
            int num = rect.Height();
            while (num--)
            {
                for (int i = 0; i < _w; ++i)
                    if (!ms[i])
                        _dst[i] = _src[i];
                _src += pat->wline;
                _dst += info.wline;
                ms += width;
            }
            startx = endx;
            endx = startx + size.x;
        }
        starty = endy;
        endy = starty + size.y;
    }
}

extern int __cdecl CompRgb2(const void *elem1, const void *elem2);

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::MedianSmooth(int w)
{
    if (!ptr) return;

    int X = info.width;
    int Y = info.height;

    MArray<ColorRef> _data(Y * X);
    ColorRef *data = _data.GetPtr();
    int L = 2 * w + 1;
    ColorRef *buff = (ColorRef *) _alloca(L * L * sizeof(ColorRef));
    if (data && buff)
    {
        int X = info.width;
        int Y = info.height;
        for (int j = 0; j < Y; ++j)
        {
            int line = j * X;
            for (int i = 0; i < X; ++i)
            {
                int n = 0;
                for (int m = max(j - w, 0) * X, lim_m = min(Y - 1, j + w) * X; m <= lim_m; m += X)
                {
                    for (int k = max(i - w, 0), lim_k = min(X - 1, i + w); k <= lim_k; ++k)
                    {
                        buff[n].color = ptr[k + m];
                        ++n;
                    }
                }
                if (n > 0)
                {
                    qsort(buff, n, sizeof(INT32), CompRgb2);
                    if (n % 2 == 1)
                        data[i + line] = buff[n / 2].color;
                    else
                    {
                        int k = n / 2;
                        data[i + line].r = ((int) buff[k].r + (int) buff[k - 1].r) / 2;
                        data[i + line].g = ((int) buff[k].g + (int) buff[k - 1].g) / 2;
                        data[i + line].b = ((int) buff[k].b + (int) buff[k - 1].b) / 2;
                    }
                }
            }
        }
        memcpy(ptr, data, Y * X * sizeof(DWORD));
    }
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void MemImage::MedianSmoothLine(int w, MArray<POINT> &arr)
{
    if (!ptr) return;

    int X = info.width;
    int Y = info.height;

    MArray<ColorRef> _data(arr.GetLen());
    ColorRef *data = _data.GetPtr();
    int L = 2 * w + 1;
    ColorRef *buff = (ColorRef *) _alloca(L * L * sizeof(ColorRef));
    int j;
    if (data && buff)
    {
        int X = info.width;
        int len, Y = info.height;
        for (j = 0, len = arr.GetLen(); j < len; ++j)
        {
            int line = arr[j].y * X;
            int n = 0;
            for (int m = max(arr[j].y - w, 0) * X, lim_m = min(Y - 1, arr[j].y + w) * X;
                 m <= lim_m; m += X)
            {
                for (int k = max(arr[j].x - w, 0), lim_k = min(X - 1, arr[j].x + w);
                     k <= lim_k; ++k)
                {
                    buff[n].color = ptr[k + m];
                    ++n;
                }
            }
            if (n > 0)
            {
                qsort(buff, n, sizeof(INT32), CompRgb2);
                if (n % 2 == 1)
                    data[j] = buff[n / 2].color;
                else
                {
                    int k = n / 2;
                    data[j].r = ((int) buff[k].r + (int) buff[k - 1].r) / 2;
                    data[j].g = ((int) buff[k].g + (int) buff[k - 1].g) / 2;
                    data[j].b = ((int) buff[k].b + (int) buff[k - 1].b) / 2;
                }
            }
        }
        for (j = 0, len = arr.GetLen(); j < len; ++j)
        {
            ptr[arr[j].x + arr[j].y * info.width] = data[j];
        }

    }
}


