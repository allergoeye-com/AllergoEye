/*
 * GDIPath.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: alex
 */
#include "stdafx.h"
#include "GDIPath.h"
#include "MemImage.h"
#include "AObject.h"
#include "ADC.h"
GDIPath::RECORD_STRETCH_JBMP::RECORD_STRETCH_JBMP() : RECORD(STRETCH_JBMP)
{
    data = 0;
}

GDIPath::RECORD_STRETCH_JBMP::~RECORD_STRETCH_JBMP()
{
}
//------------------------------------
//
//------------------------------------
GDIPath::GDIPath() : AHandle(AGDIPATH)
{
}

//------------------------------------
//
//------------------------------------
GDIPath::~GDIPath()
{
    for (int i = 0, l = path.GetLen(); i < l; ++i)
        delete path[i];
}

//------------------------------------
//
//------------------------------------
void GDIPath::SelectObject(AHandle *h)
{
    if (!h) return;
    switch (h->type)
    {
        case AHandle::HBGCOLOR:
        case AHandle::BGCOLOR:
            b = *(GBRUSH) h;
            b.a = ((GBRUSH) h)->a;
            break;
        case AHandle::HFGCOLOR:
        case AHandle::FGCOLOR:
            p = *(GPEN) h;
            break;
        case AHandle::MFONT:
            ((GFONT) h)->GetLogFont(&f);

        case AHandle::NONE:
        default:
            break;


    }
}

//------------------------------------
//
//------------------------------------
void GDIPath::LineTo(int x, int y)
{
    RECORD_LINETO *l = new RECORD_LINETO(x, y);
    l->p = p;
    path.Add() = l;
}

//------------------------------------
//
//------------------------------------
void GDIPath::MoveTo(int x, int y)
{
    RECORD_MOVETO *l = new RECORD_MOVETO(x, y);
    path.Add() = l;
}

//------------------------------------
//
//------------------------------------
void GDIPath::SetPixel(int x, int y, COLORREF c)
{
    RECORD_SETPIXEL *l = new RECORD_SETPIXEL(x, y, c);
    path.Add() = l;
}

//------------------------------------
//
//------------------------------------
void GDIPath::Ellipse(int left, int top, int right, int bottom)
{
    RECORD_ELLIPSE *l = new RECORD_ELLIPSE(left, top, right, bottom);
    l->b = b;
    l->p = p;
    path.Add() = l;

}

//------------------------------------
//
//------------------------------------
void GDIPath::Polygon(IPoint *pp, int len)
{
    RECORD_POLYGON *l = new RECORD_POLYGON(pp, len);
    l->b = b;
    l->p = p;
    path.Add() = l;

}

void GDIPath::Path(IPoint *pp, int len)
{
    RECORD_PATH *l = new RECORD_PATH(pp, len);
    l->b = b;
    l->p = p;
    path.Add() = l;

}

void GDIPath::OutString(WString &text, int x, int y, COLORREF fg, IRect &clip)
{
    RECORD_TEXTOUT *l = new RECORD_TEXTOUT(text, x, y, fg, clip);
    l->font = f;
    path.Add() = l;
}

void GDIPath::Stretch(MemImage *src, IRect &_rc_src, IRect &_rc_dst)
{
    RECORD_STRETCH_BMP *l = new RECORD_STRETCH_BMP();
    l->rc_src = _rc_src;
    l->rc_dst = _rc_dst;
    l->data.Copy(src->ptr, src->info.width * src->info.height);
    l->info = src->info;
    path.Add() = l;
}

void GDIPath::Stretch(AObject *src, IRect &_rc_src, IRect &_rc_dst)
{
    RECORD_STRETCH_JBMP *l = new RECORD_STRETCH_JBMP();
    l->rc_src = _rc_src;
    l->rc_dst = _rc_dst;
    l->data = src;
    path.Add() = l;
}

//------------------------------------
//
//------------------------------------
void GDIPath::PolygonConvex(IPoint *pp, int len)
{
    RECORD_POLYGON_CONVEX *l = new RECORD_POLYGON_CONVEX(pp, len);
    l->b = b;
    l->p = p;
    path.Add() = l;

}

//------------------------------------
//
//------------------------------------
void GDIPath::PolyLine(IPoint *pp, int len)
{
    RECORD_POLYLINE *l = new RECORD_POLYLINE(pp, len);
    l->b = b;
    l->p = p;
    path.Add() = l;

}

//------------------------------------
//
//------------------------------------
void GDIPath::Rectangle(int left, int top, int right, int bottom)
{
    RECORD_RECTANGLE *l = new RECORD_RECTANGLE(left, top, right, bottom);
    l->b = b;
    l->b.a = b.a;
    l->p = p;
    path.Add() = l;

}

//------------------------------------
//
//------------------------------------
BOOL GDIPath::StrokePath(MemImage *dc)
{
    RECORD **rec = path.GetPtr();
    AFont *font = 0;
    for (int i = 0, l = path.GetLen(); i < l; ++i)
    {
        switch (rec[i]->type)
        {
            case TEXTOUT:
            {
                RECORD_TEXTOUT *l = (RECORD_TEXTOUT *) rec[i];
                if (!font)
                    font = new AFont();
                font->CreateFontInderect(&l->font);
                AFONT old = (AFONT) dc->SelectObject(font);
                font->OutString(l->text, l->x, l->y, l->color, dc->GetPtr(), dc->info.width,
                                dc->info.height, l->clip);
                dc->SelectObject(old);
                break;

            }
            case SETPIXEL:
            {
                RECORD_SETPIXEL *l = (RECORD_SETPIXEL *) rec[i];
                dc->SetPixel(l->pt.x, l->pt.y, l->c);
                break;
            }
            case LINETO:
            {
                RECORD_LINETO *l = (RECORD_LINETO *) rec[i];
                GPEN old = dc->SelectObject(&l->p);
                dc->LineTo(l->pt.x, l->pt.y);
                dc->SelectObject(old);
                break;
            }
            case MOVETO:
            {
                RECORD_MOVETO *l = (RECORD_MOVETO *) rec[i];
                dc->MoveToEx(l->pt.x, l->pt.y, 0);
                break;
            }
            case RECTANGLE:
            {
                RECORD_RECTANGLE *l = (RECORD_RECTANGLE *) rec[i];
                GPEN old = dc->SelectObject(&l->p);
                dc->Rectangle(l->rect, false);
                dc->SelectObject(old);
                break;
            }
            case ELLIPSE:
            {
                RECORD_ELLIPSE *l = (RECORD_ELLIPSE *) rec[i];
                GPEN old = dc->SelectObject(&l->p);
                dc->Ellipse(l->rect, false);
                dc->SelectObject(old);
                break;
            }
            case PATH:
            case POLYGON_CONVEX:
            case POLYGON:
            {
                RECORD_POLYGON *l = (RECORD_POLYGON *) rec[i];
                GPEN old = dc->SelectObject(&l->p);
                dc->PolyLine(l->pt.GetPtr(), l->pt.GetLen());
                dc->SelectObject(old);

                break;
            }
            case POLYLINE:
            {
                RECORD_POLYLINE *l = (RECORD_POLYLINE *) rec[i];
                GPEN old = dc->SelectObject(&l->p);
                dc->PolyLine(l->pt.GetPtr(), l->pt.GetLen());
                dc->SelectObject(old);
                break;
            }
            case STRETCH_BMP:
            default:;


        }
    }
    return true;
}

//------------------------------------
//
//------------------------------------
BOOL GDIPath::FillPath(MemImage *dc)
{
    RECORD **rec = path.GetPtr();

    RECORD_PATH *lPath = 0;
    MArray<COLORREF> colors;
    AFont *font = 0;
    for (int i = 0, len = path.GetLen(); i < len; ++i)
    {
        switch (rec[i]->type)
        {
            case TEXTOUT:
            {
                RECORD_TEXTOUT *l = (RECORD_TEXTOUT *) rec[i];
                if (!font)
                    font = new AFont();
                font->CreateFontInderect(&l->font);
                AFONT old = (AFONT) dc->SelectObject(font);
                font->OutString(l->text, l->x, l->y, l->color, dc->GetPtr(), dc->info.width,
                                dc->info.height, l->clip);
                dc->SelectObject(old);
                break;

            }
            case SETPIXEL:
            {
                RECORD_SETPIXEL *l = (RECORD_SETPIXEL *) rec[i];
                dc->SetPixel(l->pt.x, l->pt.y, l->c);
                break;
            }
            case RECTANGLE:
            {
                RECORD_RECTANGLE *l = (RECORD_RECTANGLE *) rec[i];
                if (l)
                    dc->FillRect(l->rect, l->b);
                break;
            }
            case ELLIPSE:
            {
                RECORD_ELLIPSE *l = (RECORD_ELLIPSE *) rec[i];
                GBRUSH old = dc->SelectObject(&l->b);
                dc->Ellipse(l->rect, true);
                dc->SelectObject(old);
                break;
            }
            case POLYGON_CONVEX:
            {
                RECORD_POLYGON_CONVEX *l = (RECORD_POLYGON_CONVEX *) rec[i];
                GBRUSH old = dc->SelectObject(&l->b);
                dc->PolygonConvex(l->pt.GetPtr(), l->pt.GetLen(), true);
                dc->SelectObject(old);

                break;
            }
            case POLYGON:
            {
                RECORD_POLYGON *l = (RECORD_POLYGON *) rec[i];
                GBRUSH old = dc->SelectObject(&l->b);
                dc->Polygon(l->pt.GetPtr(), l->pt.GetLen(), true);
                dc->SelectObject(old);

                break;
            }
            case PATH:
            {
                lPath = (RECORD_PATH *) rec[i];
                colors.Add(lPath->b.color);
                dc->FillSurface(lPath->pt.GetPtr(), lPath->pt.GetLen(), 0, colors.GetLen());


                break;
            }

            case POLYLINE:
            {
                RECORD_POLYLINE *l = (RECORD_POLYLINE *) rec[i];
                GBRUSH old = dc->SelectObject(&l->b);
                dc->Polygon(l->pt.GetPtr(), l->pt.GetLen(), true);
                dc->SelectObject(old);
                break;
            }
            case STRETCH_BMP:
            {
                RECORD_STRETCH_BMP *l = (RECORD_STRETCH_BMP *) rec[i];
                JBitmap bmp;
                bmp.Create(l->rc_dst.Width(), l->rc_dst.Height(), AObject::TJLOCAL);
                if (bmp.IsValid())
                {
                    MemImage src;
                    src.Init(l->data.GetPtr(), l->info);
                    dc->Stretch(src, l->rc_src, l->rc_dst);
                }
                break;
            }

            default:;

        }
    }
    if (lPath)
    {
        GBRUSH oldb = dc->SelectObject(&lPath->b);
        dc->FillSurface(0, 0, &colors, 255);
        dc->SelectObject(oldb);
    }
    return true;
}

//------------------------------------
//
//------------------------------------
BOOL GDIPath::StrokeAndFillPath(MemImage *dc)
{
    RECORD **rec = path.GetPtr();
    MArray<COLORREF> colors;
    RECORD_PATH *lPath = 0;
    AFont *font = 0;

    for (int i = 0, len = path.GetLen(); i < len; ++i)
    {
        switch (rec[i]->type)
        {
            case TEXTOUT:
            {
                RECORD_TEXTOUT *l = (RECORD_TEXTOUT *) rec[i];
                if (!font)
                    font = new AFont();
                font->CreateFontInderect(&l->font);
                AFONT old = (AFONT) dc->SelectObject(font);
                font->OutString(l->text, l->x, l->y, l->color, dc->GetPtr(), dc->info.width,
                                dc->info.height, l->clip);
                dc->SelectObject(old);
                break;

            }

            case SETPIXEL:
            {
                RECORD_SETPIXEL *l = (RECORD_SETPIXEL *) rec[i];
                dc->SetPixel(l->pt.x, l->pt.y, l->c);
                break;
            }
            case LINETO:
            {
                RECORD_LINETO *l = (RECORD_LINETO *) rec[i];
                GPEN old = dc->SelectObject(&l->p);
                dc->LineTo(l->pt.x, l->pt.y);
                dc->SelectObject(old);
                break;
            }
            case MOVETO:
            {
                RECORD_MOVETO *l = (RECORD_MOVETO *) rec[i];
                dc->MoveToEx(l->pt.x, l->pt.y, 0);
                break;
            }
            case RECTANGLE:
            {
                RECORD_RECTANGLE *l = (RECORD_RECTANGLE *) rec[i];
                GPEN old = dc->SelectObject(&l->p);
                dc->Rectangle(l->rect);
                dc->SelectObject(old);
                break;
            }
            case ELLIPSE:
            {
                RECORD_ELLIPSE *l = (RECORD_ELLIPSE *) rec[i];
                GPEN old = dc->SelectObject(&l->p);
                dc->Ellipse(l->rect);
                dc->SelectObject(old);
                break;
            }
            case POLYGON_CONVEX:
            {
                RECORD_POLYGON_CONVEX *l = (RECORD_POLYGON_CONVEX *) rec[i];
                GPEN old = dc->SelectObject(&l->p);
                GBRUSH oldb = dc->SelectObject(&l->b);
                dc->PolygonConvex(l->pt.GetPtr(), l->pt.GetLen());
                dc->SelectObject(old);
                dc->SelectObject(oldb);

                break;
            }

            case POLYGON:
            {
                RECORD_POLYGON *l = (RECORD_POLYGON *) rec[i];
                GPEN old = dc->SelectObject(&l->p);
                GBRUSH oldb = dc->SelectObject(&l->b);
                dc->Polygon(l->pt.GetPtr(), l->pt.GetLen());
                dc->SelectObject(old);
                dc->SelectObject(oldb);

                break;
            }
            case PATH:
            {
                RECORD_PATH *_lPath = lPath;
                lPath = (RECORD_PATH *) rec[i];
                if (lPath->b.a)
                {
                    colors.Add(lPath->b.color);
                    dc->FillSurface(lPath->pt.GetPtr(), lPath->pt.GetLen(), 0, colors.GetLen());
                }
                GPEN old = dc->SelectObject(&lPath->p);
                dc->PolyLine(lPath->pt.GetPtr(), lPath->pt.GetLen());
                dc->SelectObject(old);
                if (!lPath->b.a) lPath = _lPath;
            }
                break;

            case POLYLINE:
            {
                RECORD_POLYLINE *l = (RECORD_POLYLINE *) rec[i];
                GPEN old = dc->SelectObject(&l->p);
                GBRUSH oldb = dc->SelectObject(&l->b);
                dc->Polygon(l->pt.GetPtr(), l->pt.GetLen());
                dc->SelectObject(old);
                dc->SelectObject(oldb);
                break;
            }
            case STRETCH_BMP:
            {
                RECORD_STRETCH_BMP *l = (RECORD_STRETCH_BMP *) rec[i];
                JBitmap bmp;
                bmp.Create(l->rc_dst.Width(), l->rc_dst.Height(), AObject::TJLOCAL);
                if (bmp.IsValid())
                {
                    MemImage src;
                    src.Init(l->data.GetPtr(), l->info);
                    dc->Stretch(src, l->rc_src, l->rc_dst);
                }
                break;
            }


        }
    }
    if (lPath)
    {
        GBRUSH oldb = dc->SelectObject(&lPath->b);
        dc->FillSurface(0, 0, &colors, 255);
        dc->SelectObject(oldb);
    }
    if (font)
    {

        delete font;
    }
    return true;
}

//------------------------------------
//
//------------------------------------
BOOL
GDIPath::StrokeAndFillPath(ADC *gdc, AObject *dc, AObject *fill, AObject *stroke, AObject *dash)
{
    RECORD **rec = path.GetPtr();
    MArray<COLORREF> colors;
    RECORD_PATH *lPath = 0;
    JVariable param[5];


    for (int i = 0, len = path.GetLen(); i < len; ++i)
    {
        switch (rec[i]->type)
        {
            case TEXTOUT:
            {
                RECORD_TEXTOUT *l = (RECORD_TEXTOUT *) rec[i];
                ColorRef c;
                c.color = l->color;
                BYTE r = c.r;
                c.r = c.b;
                c.b = r;
                c.a = 255;
                param[0] = (int) c.color;
                fill->Run("setColor", param, 1);
                double size = l->font.lfHeight < 0 ? abs(l->font.lfHeight) : MulDiv(
                        (int) abs(l->font.lfHeight), (int) 72,
                        (int) gdc->GetDeviceCaps(LOGPIXELSY));
                size *= 0.75;
                param[0] = (float) size;
                fill->Run("setTextSize", param, 1);
                wchar_t *p = l->text.GetPtr();
                MArray<jchar> b(l->text.GetLen());
                for (int i = 0, len = l->text.GetLen(); i < len; ++i)
                    b[i] = (jchar) p[i];
                param[0].MakeWString(b.GetPtr(), l->text.StrLen());

                param[1] = (float) (l->x + 21 + size);
                param[2] = (float) (l->y + 23 + size);
                param[3].Attach(fill->obj, fill->spec);

                dc->Run("drawText", param, 4);
                param[3].Detach();
                //canvas.drawText("Some Text", 10, 25, paint);
                break;

            }

            case RECTANGLE:
            {
                RECORD_RECTANGLE *l = (RECORD_RECTANGLE *) rec[i];
                {
                    AObject rect;
                    param[0] = (float) (l->rect.left + 22);
                    param[1] = (float) (l->rect.top + 22);
                    param[2] = (float) (l->rect.right + 22);
                    param[3] = (float) (l->rect.bottom + 22);
                    rect.New("android.graphics.RectF", param, 4, AObject::TJLOCAL);
                    ColorRef c;

                    if (l->b.a && l->b.color != 0xFFFFFFFF)
                        c.color = l->b.color;
                    else
                        c.color = l->p.color;

                    BYTE r = c.r;
                    c.r = c.b;
                    c.b = r;
                    c.a = 255;
                    param[0] = (int) c.color;
                    if (l->b.a && l->b.color != 0xFFFFFFFF)
                    {
                        fill->Run("setColor", param, 1);
                        param[0] = (float) (l->p.width);;
                        stroke->Run("setStrokeWidth", param, 1);
                    } else
                    {

                        stroke->Run("setColor", param, 1);
                        param[0] = (float) (l->p.width);;
                        stroke->Run("setStrokeWidth", param, 1);
                    }

                    param[0].Attach(rect.obj, rect.spec);
                    if (l->b.a && l->b.color != 0xFFFFFFFF)
                        param[1].Attach(fill->obj, fill->spec);
                    else
                        param[1].Attach(stroke->obj, stroke->spec);

                    dc->Run("drawRect", param, 2);
                    param[0].Detach();
                    param[1].Detach();

                }


                break;
            }
            case ELLIPSE:
            {
                RECORD_ELLIPSE *l = (RECORD_ELLIPSE *) rec[i];
                AObject rect;
                param[0] = (float) (l->rect.left + 22);
                param[1] = (float) (l->rect.top + 22);
                param[2] = (float) (l->rect.right + 22);
                param[3] = (float) (l->rect.bottom + 22);
                rect.New("android.graphics.RectF", param, 4, AObject::TJLOCAL);
                ColorRef c;

                if (l->b.a && l->b.color != 0xFFFFFFFF)
                    c.color = l->b.color;
                else
                    c.color = l->p.color;

                BYTE r = c.r;
                c.r = c.b;
                c.b = r;
                c.a = 255;
                param[0] = (int) c.color;
                if (l->b.a && l->b.color != 0xFFFFFFFF)
                {
                    fill->Run("setColor", param, 1);
                    param[0] = (float) (l->p.width);;
                    stroke->Run("setStrokeWidth", param, 1);
                } else
                {

                    stroke->Run("setColor", param, 1);
                    param[0] = (float) (l->p.width);;
                    stroke->Run("setStrokeWidth", param, 1);
                }

                param[0].Attach(rect.obj, rect.spec);
                if (l->b.a && l->b.color != 0xFFFFFFFF)
                    param[1].Attach(fill->obj, fill->spec);
                else
                    param[1].Attach(stroke->obj, stroke->spec);

                dc->Run("drawOval", param, 2);
                param[0].Detach();
                param[1].Detach();


                //      GPEN old = dc->SelectObject(&l->p);
                //    dc->Ellipse(l->rect, false);
                //  dc->SelectObject(old);
                break;
            }


            case POLYLINE:
            {
                RECORD_POLYLINE *l = (RECORD_POLYLINE *) rec[i];
                ColorRef c;
                c.color = l->p.color;
                BYTE r = c.r;
                c.r = c.b;
                c.b = r;
                c.a = 255;
                AObject *prm = stroke;
                if (l->p.attrib != PS_SOLID)
                    prm = dash;
                param[0] = (int) c.color;
                prm->Run("setColor", param, 1);
                param[0] = (float) (l->p.width);
                prm->Run("setStrokeWidth", param, 1);
                IPoint *p = l->pt.GetPtr();
                int len = l->pt.GetLen();
                MArray<float> fl(len * 4);
                int k = 0;
                for (int i = 0; i < len; ++i)
                {
                    fl[k++] = p[i].x + 22;
                    fl[k++] = p[i].y + 22;
                    if (i && i < len - 1)
                    {
                        fl[k++] = p[i].x + 22;
                        fl[k++] = p[i].y + 22;

                    }

                }
                fl.SetLen(k);
                if (len == 2)
                {
                    param[0] = fl[0];
                    param[1] = fl[1];
                    param[2] = fl[2];
                    param[3] = fl[3];
                    param[4].Attach(prm->obj, prm->spec);
                    dc->Run("drawLine", param, 5);
                    param[4].Detach();

                } else
                {
                    param[0].Set(fl.GetPtr(), fl.GetLen());
                    param[1].Attach(prm->obj, prm->spec);
                    dc->Run("drawLines", param, 2);
                    param[1].Detach();
                }


                break;
            }
            case STRETCH_BMP:
            {
                RECORD_STRETCH_BMP *l = (RECORD_STRETCH_BMP *) rec[i];
                //l->rc_src = _rc_src;
                // l->rc_dst = _rc_dst;
                //l->data.Copy(src->ptr, src->info.width * src->info.height);
                //l->info = src->info;
                JBitmap bmp;
                bmp.Create(l->rc_dst.Width(), l->rc_dst.Height(), AObject::TJLOCAL);
                if (bmp.IsValid())
                {

                    MemImage dst;
                    MemImage src;
                    src.Init(l->data.GetPtr(), l->info);
                    ImageInfo info(l->rc_dst.Width(), l->rc_dst.Height(), l->rc_dst.Width());
                    dst.Init(bmp.SetAccess(), info);
                    IRect rc(0, 0, l->rc_dst.Width(), l->rc_dst.Height());
                    dst.Stretch(src, l->rc_src, rc, SRCCOPY);
                    param[0].Attach(bmp.Get_jobject(), "android.graphics.Bitmap");
                    param[1] = (float) (l->rc_dst.left + 22);
                    param[2] = (float) (l->rc_dst.top + 22);
                    param[3].Attach(0, fill->spec);
                    dc->Run("drawBitmap", param, 4);
                    //param[3].Detach();
                    param[0].Detach();

                }
                break;
            }
            case STRETCH_JBMP:
                {
                    RECORD_STRETCH_JBMP *l = (RECORD_STRETCH_JBMP *) rec[i];

                    if (l->data)
                    {
                        param[0] = (int)l->rc_dst.left + 22;
                        param[1] = (int)l->rc_dst.top + 22;
                        param[2] = (int)(l->rc_dst.right + 22);
                        param[3] =  (int)l->rc_dst.bottom + 22;
                        l->data->Run("setBounds", param, 4);
                        param[0].Attach(dc->obj, "android.graphics.Canvas");
                        l->data->Run("draw", param, 1);
                        param[0].Detach();
                    }
                    break;
                }
        }
    }

    return true;
}
