//
// Created by alex on 19.12.2017.
//

#ifndef FUTURA_POINT_RECT_H_H
#define FUTURA_POINT_RECT_H_H
#include "buffer.h"
#pragma pack(push)
#pragma pack(4)
#ifndef int32
typedef int int32;
#endif
struct iPOINT {
    int32 x, y;
};
struct IPOINT : public iPOINT {
    IPOINT() {x = y = 0; }
    IPOINT(int initX, int initY){ x = initX; y = initY; }
    IPOINT(DWORD dwPoint)
    {
        x = (short)LOWORD(dwPoint);
        y = (short)HIWORD(dwPoint);
    }
    void Set(int _x, int _y) {x = _x; y = _y; }
    void SetPoint(int _x, int _y) {x = _x; y = _y; }
    void Offset(int xOffset, int yOffset) { x += xOffset; y += yOffset; }
    void Offset(IPOINT point) { x += point.x; y += point.y; }
    BOOL operator == (const IPOINT &point) const { return (x == point.x && y == point.y); }
    BOOL operator != (const IPOINT &point) const	{ return (x != point.x || y != point.y); }
    void operator += (const IPOINT &point)	{ x += point.x; y += point.y; }
    void operator -= (const IPOINT &point)	{ x -= point.x; y -= point.y; }
    void operator += (int i) { x += i; y += i; }
    void operator -= (int i) { x -= i; y -= i; }
    operator IPOINT * () { return this; }

    IPOINT &operator /= (double d)  { x = int(x / d + 0.5); y = int(y / d + 0.5); return *this; }
    IPOINT &operator *= (double d)  { x = int(x / d + 0.5); y = int(y / d + 0.5); return *this; }
    IPOINT operator * (double d)  {IPOINT v(*this); v.x = int(v.x * d + 0.5); v.y = int(v.y * d + 0.5); return *this; }
    IPOINT operator / (double d)  {IPOINT v(*this); v.x = int(v.x / d + 0.5); v.y = int(v.y / d + 0.5); return *this; }
    int &operator [] (int i) { return !i ? x : y; }
    IPOINT operator - () const	{ return IPOINT(-x, -y); }
    IPOINT operator + (const IPOINT &point) const { return IPOINT(x + point.x, y + point.y); }
    IPOINT operator - (const IPOINT &point) const { return IPOINT(x - point.x, y - point.y); }
    IPOINT operator + (int i) const { return IPOINT(x + i, y + 1); }
    IPOINT operator - (int i) const { return IPOINT(x - i, y - 1); }

    bool operator < (const IPOINT &b) const { if (x != b.x) return x < b.x;
        if (y != b.y) return y < b.y;
        return false;
    }

    bool operator > (const IPOINT &b) const { if (x != b.x) return x > b.x;
        if (y != b.y) return y > b.y;
        return false;
    }

};
#ifndef WIN32
typedef IPOINT * LPPOINT;
#define POINT IPOINT
struct POINTL {
    int32 x, y;
};
#endif
struct ISIZE {
    int32 cx, cy;
    ISIZE(int x, int y) { cx = x; cy = y; }
    ISIZE(const ISIZE &sz) { cx = sz.cx; cy = sz.cy; }
    ISIZE() {cx = cy = 0; }
    ISIZE &operator += (const ISIZE &sz)   {cx += sz.cx; cy += sz.cy; return *this; }
    ISIZE &operator -= (const ISIZE &sz)  {cx -= sz.cx; cy -= sz.cy; return *this; }
};
#ifndef WIN32
#define CSize ISIZE
#define SIZE ISIZE
#endif

struct IRECT {
    int32 left;
    int32 top;
    int32 right;
    int32 bottom;
    IRECT() { left = top = right = bottom = 0; }
    IRECT(int32 l, int32 t, int32 r, int32 b) { left = l; top = t; right = r; bottom = b; }
    IRECT(const IRECT * lpSrcRect) {memcpy(this, lpSrcRect, sizeof (IRECT)); }
    IRECT(const IRECT &rc) { left = rc.left; top = rc.top; right = rc.right; bottom = rc.bottom; }
    IRECT(const IPOINT &lt, const IPOINT &rb) { left = lt.x; top = lt.y; right = rb.x; bottom = rb.y; }
    IRECT(IPOINT topLeft, IPOINT bottomRight) { left = topLeft.x; top = topLeft.y; right = bottomRight.x; bottom = bottomRight.y; }
    IRECT(IPOINT topLeft, ISIZE sz) { left = topLeft.x; top = topLeft.y; right = left + sz.cx; bottom = top + sz.cy; }
    IRECT(const IPOINT &topLeft, const SIZE &sz) { left = topLeft.x; top = topLeft.y; right = left + sz.cx; bottom = top + sz.cy; }

    //operator IRECT() const { return *(IRECT *)this; }
    IPOINT &TopLeft() { return *((IPOINT*)this); }
    IPOINT &BottomRight() { return *((IPOINT *)this+1); }
    IPOINT CenterPoint() const { return IPOINT((left+right)/2, (top+bottom)/2); }

    IRECT &operator = (const IRECT &rc) { left = rc.left; top = rc.top; right = rc.right; bottom = rc.bottom; return *this; }
    bool operator == (const IRECT &r)  { return r.left == left && r.right == right && r.top == top && r.bottom == bottom; }
    bool operator != (const IRECT &r) { return !(operator == (r)); }
    IRECT &Set(int32 l, int32 t, int32 r, int32 b) { left = l; top = t; right = r; bottom = b; return *this; }
    void Set(const IPOINT &lt, const IPOINT &rb) { left = lt.x; top = lt.y; right = rb.x; bottom = rb.y; }
    IRECT &SetRect(int32 l, int32 t, int32 r, int32 b) { left = l; top = t; right = r; bottom = b; return *this; }
    IRECT &SetRect(POINT topLeft, POINT bottomRight) { left = topLeft.x; top = topLeft.y; right = bottomRight.x; bottom = bottomRight.y; return *this; }

    int Width() const { return right - left; }
    int Height() const { return bottom - top; }
    void Update() { int l = min(left, right), t = min(top, bottom),  r = max(left, right), b = max(top, bottom); Set(l, t, r, b); }
    void SetWidth(int n) { right = left + n; Update(); }
    void SetHeight(int n) { top = bottom + n; Update(); }
    void Move(int x, int y) { right += x - left; bottom += y - top; top = y; left = x; }
    BOOL IsEmpty() const {return (left == 0 && right == 0 && top == 0 && bottom == 0); }
    BOOL IsRectEmpty() const {return (left == 0 && right == 0 && top == 0 && bottom == 0); }
    BOOL IsRectNull()  const {return (left == 0 && right == 0 && top == 0 && bottom == 0); }

    void SetRectEmpty() { left = top = right = bottom = 0; }
    void Clear() {left = right = top = bottom = 0; }

#ifndef _CONSOLE_PROG_
    #ifdef __WXGTK__

	operator wxRect () const { return wxRect(left, top, right - left, bottom - top); }
    IRECT &operator = (const wxRect &rc)  { left = rc.x; top = rc.y; right = rc.width + rc.x; bottom = rc.height + rc.y; return *this; }
#endif
#endif
public:
    operator IRECT * () { return this; }
    void SwapLeftRight() {SwapLeftRight(this); }
    void SwapLeftRight(IRECT *lpRect)
    { int temp = lpRect->left; lpRect->left = lpRect->right; lpRect->right = temp; }
    BOOL PtInRect(IPOINT p) { return p.x >= left && p.y >= top && p.x <= right && p.y <= bottom; };
    BOOL PtInRect(IPOINT *p) { return p->x >= left && p->y >= top && p->x <= right && p->y <= bottom; };
    BOOL PtIn(int x, int y) { return x >= left && y >= top && x <= right && y <= bottom; };
    BOOL PtInRegion(int x, int y) { return x >= left && y >= top && x < right && y < bottom; };
    void InflateRect(int x, int y)  { left -= x; top -= y; right += x; bottom += y; }
    void OffsetRect(int x, int y)  { left += x; top += y; right += x; bottom += y; }
    void Normalize()
    {
        int nTemp;
        if (left > right)
        {
            nTemp = left;
            left = right;
            right = nTemp;
        }
        if (top > bottom)
        {
            nTemp = top;
            top = bottom;
            bottom = nTemp;
        }
    }
    IRECT &operator |= (const IRECT &r)
    {
        left = min(left, r.left);
        top = min(top, r.top),
                right = max(right, r.right);
        bottom = max(bottom, r.bottom);
        return *this;
    }
    IRECT &operator &= (const IRECT &r)
    {
        left = max(left, r.left);
        top = max(top, r.top);
        right = min(right, r.right);
        bottom = min(bottom, r.bottom);
        if (left >= right || top >= bottom)
            Clear();
        return *this;
    }
    void Update(int x, int y)
    {
        left = min(x, left);
        top = min(y, top);
        right = max(x, right);
        bottom = max(y, bottom);
    }
    bool Minus(const IRECT &a, MArray<IRECT> &ret)
    {
        int len = ret.GetLen();
        IRECT newr;
        newr.left = max(a.left, left);
        newr.top = max(a.top, top);
        newr.right = min(a.right, right);
        newr.bottom = min(a.bottom, bottom);
        if (newr.left <= newr.right && newr.top <= newr.bottom)
        {
            if (newr.left > left)
                ret.Add().Set(left, top, newr.left,bottom);
            if (newr.right < right)
                ret.Add().Set(newr.right, top, right, bottom);
            if (newr.top > top)
                ret.Add().Set(newr.left, top, newr.right, newr.top);
            if (newr.bottom < bottom)
                ret.Add().Set(newr.left, newr.bottom, newr.right, bottom);
        }
        return ret.GetLen() == len;

    }
    void CopyRect(const IRECT *lpSrcRect) {memcpy(this, lpSrcRect, sizeof(IRECT)); }
    BOOL EqualRect(const IRECT *lpRect) { return lpRect->left == left && lpRect->right == right && lpRect->top == top && lpRect->bottom == bottom; }
    void InflateRect(const IRECT *lpRect)  { left -= lpRect->left; top -= lpRect->top; right += lpRect->right; bottom += lpRect->bottom; }
    void InflateRect(int l, int t, int r, int b)  { left -= l; top -= t; right += r; bottom += b; }
    void DeflateRect(int x, int y)  { left += x; top += y; right -= x; bottom -= y; }
    void DeflateRect(const IRECT *lpRect)  { left += lpRect->left; top += lpRect->top; right -= lpRect->right; bottom -= lpRect->bottom; }
    void DeflateRect(int l, int t, int r, int b)  { left += l; top += t; right -= r; bottom -= b; }
#ifndef _CONSOLE_PROG_
    #ifdef __WXGTK__
    IRECT &operator = (const wxRect &rc)  { left = rc.x; top = rc.y; right = rc.width + rc.x; bottom = rc.height + rc.y; return *this; }
#endif
#endif
    void OffsetRect(IPOINT point) { left += point.x; top += point.y; right += point.x; bottom += point.y; }
    BOOL IntersectRect(const IRECT *lpRect1, const IRECT *lpRect2) {

        left = max(lpRect1->left, lpRect2->left);
        top = max(lpRect1->top, lpRect2->top);
        right = min(lpRect1->right, lpRect2->right);
        bottom = min(lpRect1->bottom, lpRect2->bottom);
        return left <= right && top <= bottom ? TRUE : FALSE;
    }
    void NormalizeRect() { IRECT::Normalize(); }
    BOOL UnionRect(const IRECT *lpRect1, const IRECT *lpRect2)
    {
        SetRect(min(lpRect1->left, lpRect2->left), min(lpRect1->top, lpRect2->top), max(lpRect1->right, lpRect2->right), max(lpRect1->bottom, lpRect2->bottom));
        return !IsRectNull();

    }
    IRECT MulDiv(int nMultiplier, int nDivisor) const
    {
        IRECT r(
                ::MulDiv(left, nMultiplier, nDivisor),
                ::MulDiv(top, nMultiplier, nDivisor),
                ::MulDiv(right, nMultiplier, nDivisor),
                ::MulDiv(bottom, nMultiplier, nDivisor));
        return r;
    }


};
inline void CopyRect(IRECT *a, IRECT *b) { *a = *b; }
#pragma pack(pop)

inline void InflateRect(IRECT *rc, int x, int y)  { rc->InflateRect(x, y); }

inline IRECT operator | (const  IRECT &a, const IRECT &r)
{
    return IRECT(min(a.left, r.left), min(a.top, r.top), max(a.right, r.right), max(a.bottom, r.bottom));
}
inline IRECT operator & (const IRECT &a,  const IRECT &r)
{
    IRECT newr;
    newr.left = max(a.left, r.left);
    newr.top = max(a.top, r.top);
    newr.right = min(a.right, r.right);
    newr.bottom = min(a.bottom, r.bottom);
    return newr.left <= newr.right && newr.top <= newr.bottom ? newr : newr.Set(0, 0, 0, 0);
}
inline void SetRect(IRECT *r, int32 left, int32 top, int32 right, int32 bottom)
{
    r->left = left;
    r->top = top;
    r->right = right;
    r->bottom = bottom;
}
inline BOOL PtInRect(IRECT *rc,IPOINT *p)
{

    return rc && p ? p->x >= rc->left && p->y >= rc->top && p->x <= rc->right && p->y <= rc->bottom : 0;

}
inline BOOL PtInRect(IRECT *rc, const IPOINT &p)
{

    return rc ? p.x >= rc->left && p.y >= rc->top && p.x <= rc->right && p.y <= rc->bottom : 0;

}

#ifndef WIN32
EXPORTNIX struct EXPMDC tagPTR {
        tagPTR() {}
        virtual ~tagPTR() {}
};
typedef IRECT RECT;
typedef const RECT * LPCRECT;
typedef RECT * LPRECT;
#endif
typedef IRECT IRect;
typedef IPOINT IPoint;

#endif //FUTURA_POINT_RECT_H_H
