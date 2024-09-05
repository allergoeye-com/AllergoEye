/*
 * AClip.h
 *
 *  Created on: Sep 4, 2014
 *      Author: alex
 */

#ifndef ACLIP_H_
#define ACLIP_H_
#pragma pack(push)
#pragma pack(8)

template<class T, class _TYPE>
class DWClipLine {
public:
    _TYPE left;
    _TYPE top;
    _TYPE right;
    _TYPE bottom;

    void Init(IRect &clip, int dop = 0)
    {
        left = clip.left - dop;
        right = clip.right + dop;
        top = clip.top - dop;
        bottom = clip.bottom + dop;
    }

    bool ClipBarsk(T &p1, T &p2);

    bool ClipBarsk(_TYPE &x1, _TYPE &y1, _TYPE &x2, _TYPE &y2);

    bool PtIn(int x, int y)
    { return x >= left && y >= top && x <= right && y <= bottom; };
protected:
    bool TestBarsk(float p, float q, float *u1, float *u2);

};

template<class T, class _TYPE>
inline bool DWClipLine<T, _TYPE>::TestBarsk(float p, float q, float *u1, float *u2)
{
    float r;
    if (p < 0.0f)
    {
        if ((r = q / p) > *u2)
            return 0;
        else if (r > *u1)
            *u1 = r;
    } else if (p > 0.0f)
    {
        if ((r = q / p) < *u1)
            return 0;
        else if (r < *u2)
            *u2 = r;
    } else if (q < 0.0f)
        return 0;
    return 1;
}

template<class T, class _TYPE>
inline bool DWClipLine<T, _TYPE>::ClipBarsk(T &p1, T &p2)
{
    float u1 = 0.0f, u2 = 1.0f, dx = (float) (p2.x - p1.x), dy;
    if (TestBarsk(-dx, (float) p1.x - left, &u1, &u2))
        if (TestBarsk(dx, right - (float) p1.x, &u1, &u2))
        {
            dy = (float) (p2.y - p1.y);
            if (TestBarsk(-dy, (float) p1.y - top, &u1, &u2))
                if (TestBarsk(dy, bottom - (float) p1.y, &u1, &u2))
                {
                    if (u2 < 1.0)
                    {
                        p2.x = (_TYPE) (p1.x + u2 * dx);
                        p2.y = (_TYPE) (p1.y + u2 * dy);
                    }
                    if (u1 > 0.0)
                    {
                        p1.x = (_TYPE) (p1.x + u1 * dx);
                        p1.y = (_TYPE) (p1.y + u1 * dy);
                    }
                    return 1;
                }
        }
    return 0;
}

template<class T, class _TYPE>
inline bool DWClipLine<T, _TYPE>::ClipBarsk(_TYPE &x1, _TYPE &y1, _TYPE &x2, _TYPE &y2)
{
    float u1 = 0.0f, u2 = 1.0f, dx = (float) (x2 - x1), dy;
    if (TestBarsk(-dx, x1 - left, &u1, &u2))
        if (TestBarsk(dx, right - x1, &u1, &u2))
        {
            dy = y2 - y1;
            if (TestBarsk(-dy, y1 - top, &u1, &u2))
                if (TestBarsk(dy, bottom - y1, &u1, &u2))
                {
                    if (u2 < 1.0)
                    {
                        x2 = (_TYPE) (x1 + u2 * dx);
                        y2 = (_TYPE) (y1 + u2 * dy);
                    }
                    if (u1 > 0.0)
                    {
                        x1 = (_TYPE) (x1 + u1 * dx);
                        y1 = (_TYPE) (y1 + u1 * dy);
                    }
                    return 1;
                }
        }
    return 0;
}

template<class T, class _TYPE>
class DWClipPolygon {

    struct _stack {
        int b;
        T p;
        bool fCh;
    };

    enum Orient {
        Left, Right, Top, Bottom
    };
public:
    void Init(IRect &clip, int ext = 0)
    {
        offsetx = offsety = 0;
        left = clip.left - ext;
        right = clip.right - 1 + ext;
        top = clip.top - ext;
        bottom = clip.bottom + ext - 1;
    }

    bool Clip(T *src, int n, T *dst, int &num, IRect *mrc);

    bool PtIn(int x, int y)
    { return x >= left && y >= top && x <= right && y <= bottom; };
protected:
    bool CheckOrient(T &p, int b);

    void Intersect(T &p1, T &p2, int b, T &res);

    void CheckPoint(T &p, int b, T *dst, int &num, T *first, T *last, bool *f_first);

    void Close(T *dst, int &num, T *first, T *last, bool *f_first);

    _stack stack[4];
    _TYPE offsetx;
    _TYPE offsety;
public:
    _TYPE left;
    _TYPE top;
    _TYPE right;
    _TYPE bottom;

};

template<class T, class _TYPE>
inline bool DWClipPolygon<T, _TYPE>::CheckOrient(T &p, int b)
{

    return (b == Left && p.x >= left) || (b == Top && p.y >= top) ||
           (b == Right && p.x <= right) || (b == Bottom && p.y <= bottom);
}

template<class T, class _TYPE>
inline void DWClipPolygon<T, _TYPE>::Intersect(T &p1, T &p2, int b, T &res)
{
    float m = p1.x != p2.x ? (float) (p1.y - p2.y) / (float) (p1.x - p2.x) : 0.0f;
    if (b == Left)
    {
        res.x = left;
        res.y = (_TYPE) (p2.y + (left - p2.x) * m);
    } else if (b == Top)
    {
        res.y = top;
        res.x = (_TYPE) (m != 0.0 ? p2.x + (top - p2.y) / m : p2.x);
    } else if (b == Right)
    {
        res.x = right;
        res.y = (_TYPE) (p2.y + (right - p2.x) * m);
    } else if (b == Bottom)
    {
        res.y = bottom;
        res.x = (_TYPE) (m != 0.0f ? p2.x + (bottom - p2.y) / m : p2.x);
    }

}

template<class T, class _TYPE>
void
DWClipPolygon<T, _TYPE>::CheckPoint(T &p, int b, T *dst, int &num, T *first, T *last, bool *f_first)
{
    int is = 0;
    T pt;
    bool fCh;
    while_true:
    fCh = CheckOrient(p, b);
    if (!*(f_first + b))
    {
        *(first + b) = p;
        *(f_first + b) = 1;
    } else if (fCh != CheckOrient(*(last + b), b))
    {
        Intersect(p, *(last + b), b, pt);
        if (b < Bottom)
        {
            (stack + is)->b = b;
            (stack + is)->p = p;
            (stack + is)->fCh = fCh;

            ++is;
            p = pt;
            ++b;
            goto while_true;
        } else
        {
            (dst + num)->x = pt.x;
            (dst + num)->y = pt.y;
            if (!(num && (dst + num)->x == (dst + num - 1)->x &&
                  (dst + num)->y == (dst + num - 1)->y))
                ++num;
        }
    }
    go_to :
    *(last + b) = p;
    if (fCh)
    {
        if (b < Bottom)
        {
            ++b;
            goto while_true;
        } else
        {
            (dst + num)->x = p.x;
            (dst + num)->y = p.y;
            if (!(num && (dst + num)->x == (dst + num - 1)->x &&
                  (dst + num)->y == (dst + num - 1)->y))
                ++num;
        }
    }
    if (is)
    {
        --is;
        b = (stack + is)->b;
        p = (stack + is)->p;
        fCh = (stack + is)->fCh;
        goto go_to;


    }
}

template<class T, class _TYPE>
void DWClipPolygon<T, _TYPE>::Close(T *dst, int &num, T *first, T *last, bool *f_first)
{

    T pt;
    BYTE b;
    for (b = Left; b <= Bottom; ++b)
    {
        if (*(f_first + b) && CheckOrient(*(last + b), b) != CheckOrient(*(first + b), b))
        {
            Intersect(*(last + b), *(first + b), b, pt);
            if (b < Bottom)
                CheckPoint(pt, b + 1, dst, num, first, last, f_first);
            else
            {
                (dst + num)->x = pt.x;
                (dst + num)->y = pt.y;
                if (!(num && (dst + num)->x == (dst + num - 1)->x &&
                      (dst + num)->y == (dst + num - 1)->y))
                    ++num;
            }

        }
    }
}

template<class T, class _TYPE>
bool DWClipPolygon<T, _TYPE>::Clip(T *src, int n, T *dst, int &num, IRect *mrc)
{
    T first[4], last[4];
    bool f_first[4] = {0, 0, 0, 0};
    num = 0;
    T *dr_ptr = src;

    BYTE fContinue = 0, prev_b = 0, next_b = 0, cur_b = (((BYTE) ((dr_ptr->x < left) << 3) & 8) |
                                                         ((BYTE) ((dr_ptr->y < top) << 2) & 4) |
                                                         ((BYTE) ((dr_ptr->x > right) << 1) & 2) |
                                                         ((BYTE) ((dr_ptr->y > bottom)) & 1));


    bool fFirst = 1;
    while (n--)
    {
        if (n)
        {
            next_b = ((BYTE) (((dr_ptr + 1)->x < left) << 3) & 8) |
                     ((BYTE) (((dr_ptr + 1)->y < top) << 2) & 4) |
                     ((BYTE) (((dr_ptr + 1)->x > right) << 1) & 2) |
                     ((BYTE) (((dr_ptr + 1)->y > bottom)) & 1);
            if (prev_b == cur_b)
                fContinue = cur_b & next_b;
        }

        if (!fContinue)
        {
            if (!fFirst && !prev_b && !cur_b && !next_b)
            {
                (dst + num)->x = dr_ptr->x;
                (dst + num)->y = dr_ptr->y;
                if (!(num && (dst + num)->x == (dst + num - 1)->x &&
                      (dst + num)->y == (dst + num - 1)->y))
                    ++num;
            } else
                CheckPoint(*dr_ptr, Left, dst, num, first, last, f_first);
        }
        if (fFirst) fFirst = 0;
        prev_b = cur_b;
        cur_b = next_b;
        fContinue = 0;
        next_b = 64;
        ++dr_ptr;
    }
    Close(dst, num, first, last, f_first);
    if (num > 2 && (dst->x != dst[num - 1].x || dst->y != dst[num - 1].y))
        dst[num++] = *dst;
    if (num > 2)
    {
        mrc->SetRect(dst->x + offsetx, dst->y + offsety, dst->x + offsetx, dst->y + offsety);
        n = num;
        while (n--)
        {
            dst->x += offsetx;
            dst->y += offsety;
            mrc->left = min(dst->x, mrc->left);
            mrc->right = max(dst->x, mrc->right);
            mrc->top = min(dst->y, mrc->top);
            mrc->bottom = max(dst->y, mrc->bottom);
            ++dst;
        }
        return 1;
    }
    return 0;
}

#include "mstack.h"

struct AFill {
    enum {
        markB, markP, markT, markC
    };

    AFill(BYTE *_buff, int _width, int _height) : buff(_buff), width(_width), height(_height),
                                                  _Next(0)
    {
        memset(buff, 0xff, width * height);
    }

    void Ellipse(IRect &rc);

    void Polygon(IPoint *p, int num);

    void PolygonConvex(IPoint *p, int num, int mark = markB);


    enum TYPE_LINE {
        VERT, HORZ, NHORZ, NVERT
    };

    void Fill(MFifo<IPoint> &fifo, int _x, int _y, IRect &clip, BYTE mark = 3);

    void LineEllipse(int x, int y);

    void Line(int x1, int y1, int x2, int y2, BYTE mark = 1)
    {
        InitLine(x1, y1, x2, y2);
        int k, e;
        switch (type)
        {
            case VERT:
                k = cY * width + cX;
                e = YEnd * width + cX;
                for (; k <= e; k += width)
                    buff[k] = mark;
                break;
            case HORZ:
                k = cY * width;
                e = k + XEnd;
                k += cX;
                for (; k <= e; ++k)
                    buff[k] = mark;
                break;
            default:
                do
                    if (cX < width && cY < height)
                        buff[cX + cY * width] = mark;
                while ((this->*_Next)());
                buff[cX + cY * width] = mark;
        }

        /*
        do
        {
            int _x = cX;
            int _y = cY;
            if (_x < 0)
                _x = 0;
            if (_x >= width)
                _x = width - 1;
            if (_y < 0)
                _y = 0;
            if (_y >= height)
                _y = height - 1;
            buff[_x + _y * width] = markP;
        }
        while ((this->*_Next)());
        int _x = cX;
        int _y = cY;
        if (_x < 0)
            _x = 0;
        if (_x >= width)
            _x = width - 1;
        if (_y < 0)
            _y = 0;
        if (_y >= height)
            _y = height - 1;
            buff[_x + _y * width] = markP;
*/
    }

    void InitLine(int x1, int y1, int x2, int y2);

    BOOL NextForX();

    BOOL NextForY();

    typedef BOOL (AFill::*NextPoint)();

    NextPoint _Next;
    int width;
    int height;
    int X, Y;
    double dDx;
    BYTE *buff;

    int cX, cY;
    int sX, sY;
    int XEnd, YEnd;
    int Dx, Dy;
    int Dx2, Dy2;
    int dX, dY;
    int Ex, Ey;
    int Ex0, Ey0;

    int type;

};

inline BOOL AFill::NextForX()
{
    cX += dX;
    Ey += Dy2;
    if (Ey >= 0)
    {
        Ey -= Dx2;
        cY += dY;
    }
    return cX != XEnd;
}

inline BOOL AFill::NextForY()
{
    cY += dY;
    Ex += Dx2;
    if (Ex >= 0)
    {
        Ex -= Dy2;
        cX += dX;
    }
    return cY != YEnd;
}

#pragma pack(pop)


#endif /* ACLIP_H_ */
