/*
 * GDIPath.h
 *
 *  Created on: Aug 26, 2014
 *      Author: alex
 */
#ifndef GDIPATH_H_
#define GDIPATH_H_

#include "AColor.h"

class MemImage;

class AObject;

class ADC;

#pragma pack(push)
#pragma pack(8)

EXPORTNIX class EXPMDC  GDIPath : public AHandle {

public:
    enum TYPE_RECORD {
        SETPIXEL,
        LINETO,
        MOVETO,
        RECTANGLE,
        ELLIPSE,
        POLYGON,
        POLYGON_CONVEX,
        POLYLINE,
        PATH,
        TEXTOUT,
        STRETCH_BMP,
        STRETCH_JBMP
    };

    struct RECORD {
        RECORD(TYPE_RECORD _type)
        { type = _type; }

        virtual ~RECORD()
        {}

        TYPE_RECORD type;
    };

    struct RECORD_SETPIXEL : public RECORD {
        RECORD_SETPIXEL(int _x, int _y, COLORREF clr) : RECORD(SETPIXEL)
        {
            pt.x = _x;
            pt.y = _y;
            c = clr;
        }

        virtual ~RECORD_SETPIXEL()
        {}

        IPoint pt;
        COLORREF c;
    };

    struct RECORD_LINETO : public RECORD {
        RECORD_LINETO(int _x, int _y) : RECORD(LINETO)
        {
            pt.x = _x;
            pt.y = _y;
        }

        virtual ~RECORD_LINETO()
        {}

        IPoint pt;
        tagGPEN p;
    };

    struct RECORD_MOVETO : public RECORD {
        RECORD_MOVETO(int _x, int _y) : RECORD(MOVETO)
        {
            pt.x = _x;
            pt.y = _y;
        }

        virtual ~RECORD_MOVETO()
        {}

        IPoint pt;
    };

    struct RECORD_RECTANGLE : public RECORD {
        RECORD_RECTANGLE(int left, int top, int right, int bottom) : RECORD(RECTANGLE)
        { rect.Set(left, top, right, bottom); }

        virtual ~RECORD_RECTANGLE()
        {}

        tagGBRUSH b;
        tagGPEN p;
        IRect rect;

    };

    struct RECORD_POLYGON_CONVEX : public RECORD {
        RECORD_POLYGON_CONVEX(IPoint *p, int len) : RECORD(POLYGON_CONVEX)
        { pt.Copy(p, len); }

        virtual ~RECORD_POLYGON_CONVEX()
        {}

        tagGBRUSH b;
        tagGPEN p;
        MArray<IPoint> pt;
    };

    struct RECORD_POLYGON : public RECORD {
        RECORD_POLYGON(IPoint *p, int len) : RECORD(POLYGON)
        { pt.Copy(p, len); }

        virtual ~RECORD_POLYGON()
        {}

        tagGBRUSH b;
        tagGPEN p;
        MArray<IPoint> pt;
    };

    struct RECORD_PATH : public RECORD {
        RECORD_PATH(IPoint *p, int len) : RECORD(PATH)
        { pt.Copy(p, len); }

        virtual ~RECORD_PATH()
        {}

        tagGBRUSH b;
        tagGPEN p;
        MArray<IPoint> pt;
    };

    struct RECORD_POLYLINE : public RECORD {
        RECORD_POLYLINE(IPoint *p, int len) : RECORD(POLYLINE)
        { pt.Copy(p, len); }

        virtual ~RECORD_POLYLINE()
        {}

        tagGBRUSH b;
        tagGPEN p;
        MArray<IPoint> pt;
    };

    struct RECORD_STRETCH_BMP : public RECORD {
        RECORD_STRETCH_BMP() : RECORD(STRETCH_BMP)
        {}

        virtual ~RECORD_STRETCH_BMP()
        {};
        IRect rc_src;
        IRect rc_dst;
        MArray<DWORD> data;
        ImageInfo info;
    };
    struct RECORD_STRETCH_JBMP : public RECORD {
        RECORD_STRETCH_JBMP();

        virtual ~RECORD_STRETCH_JBMP();
        IRect rc_src;
        IRect rc_dst;
        AObject *data;
        ImageInfo info;
    };

    struct RECORD_ELLIPSE : public RECORD {
        RECORD_ELLIPSE(int left, int top, int right, int bottom) : RECORD(ELLIPSE)
        { rect.Set(left, top, right, bottom); }

        virtual ~RECORD_ELLIPSE()
        {};
        IRect rect;
        tagGBRUSH b;
        tagGPEN p;
    };

    struct RECORD_TEXTOUT : public RECORD {
        RECORD_TEXTOUT(WString &_text, int _x, int _y, COLORREF fg, IRect &_clip) : RECORD(TEXTOUT)
        {
            text = _text;
            color = fg;
            clip = _clip;
            x = _x;
            y = _y;

        }

        WString text;
        COLORREF color;
        IRect clip;
        int x;
        int y;
        LOGFONT font;
    };

//    void OutString(WString &text, int x, int y, COLORREF fg, DWORD *ptr, int width, int height, IRect *clip);
    GDIPath();

    virtual ~GDIPath();

    void SelectObject(AHandle *h);

    void LineTo(int x, int y);

    void MoveTo(int x, int y);

    void Ellipse(int left, int top, int right, int bottom);

    void Rectangle(int left, int top, int right, int bottom);

    void SetPixel(int x, int y, COLORREF c);

    void Polygon(IPoint *pp, int len);

    void PolygonConvex(IPoint *pp, int len);

    void PolyLine(IPoint *pp, int len);

    void Path(IPoint *pp, int len);

    void OutString(WString &text, int x, int y, COLORREF fg, IRect &clip);

    void Stretch(MemImage *src, IRect &_rc_src, IRect &_rc_dst);
    void Stretch(AObject *src, IRect &_rc_src, IRect &_rc_dst);

    BOOL StrokePath(MemImage *);

    BOOL StrokeAndFillPath(MemImage *);

    BOOL FillPath(MemImage *);

    BOOL
    StrokeAndFillPath(ADC *gdc, AObject *canvas, AObject *fill, AObject *stroke, AObject *dash);

    void Clear()
    {
        RECORD **rec = path.GetPtr();
        for (int i = 0, l = path.GetLen(); i < l; ++i)
            delete rec[i];
        path.Clear();
    }

    IRect init_rc;
    tagGBRUSH b;
    tagGPEN p;
    LOGFONT f;

    MArray<RECORD *> path;
};
#pragma pack(pop)

#endif /* GDIPATH_H_ */
