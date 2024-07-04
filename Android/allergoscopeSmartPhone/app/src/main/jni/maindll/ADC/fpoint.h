///////////////////////////////////////////////////
// fpoint.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////
#ifndef ___M__POINT__
#define ___M__POINT__
#include "mvect.h"

#define FPoint Vector2D
#pragma pack(push)
#pragma pack(2)
struct SHPoint 	{
	short x;
	short y;
};
struct CHPoint 	{
	char x;
	char y;
};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(8)

union MPointOrient {
	struct {
	BYTE left : 1;
	BYTE top : 1;
	BYTE right : 1;
	BYTE bottom : 1;
	BYTE reserv : 4;
	} bit;
	BYTE byte;
	MPointOrient() { byte = 0; };
};

#define SetPointOrient(p, rc, ret) \
{									\
	ret.bit.left = p.x < rc.left; \
	ret.bit.top = p.y < rc.top;	  \
	ret.bit.right = p.x > rc.right;	\
	ret.bit.bottom = p.y > rc.bottom; \
}
#define SetPointOrientB(p, rc, ret) \
{ \
	ret = 0; \
	ret = (((p.x < rc.left) << 3) & 0xF7) | (((p.y < rc.top) << 2) & 0xFb) | (((p.x > rc.right) << 1) & 0xfd) | (((p.y > rc.bottom)) & 0xfe); \
}
template <class T>
struct TRect
{
	enum CROSS {EMPTY, ENCLOSURED_R2This = 1, ENCLOSURED_This2R = 2, INTERSECT = 4, EQUAL = 8 };

	TRect() { left = top = right = bottom = 0; }
	TRect(T l, T t, T r, T b) { left = l; top = t; right = r; bottom = b; }
	TRect(TRect &rc) { left = rc.left; top = rc.top; right = rc.right; bottom = rc.bottom; }
	TRect(RECT rc) { left = rc.left; top = rc.top; right = rc.right; bottom = rc.bottom; }
	FPoint CenterPoint() { return FPoint((left + right)/2, (top + bottom)/2); }
	T Width() const { return right - left; }
	T Height() const { return bottom - top; }
	FPoint LT() const { return FPoint(left, top); }
	FPoint RT() const { return FPoint(right, top); }
	FPoint LB() const { return FPoint(left, bottom); }
	FPoint RB() const { return FPoint(right, bottom); }
	// 0 LT
	// 1 RT
	// 2 LB
	// 3 RB
	FPoint operator [] (int index) {
	        return FPoint (index & 1 ? right : left,
                      index & 2 ? bottom : top); }
	
	TRect &Set(T l, T t, T r, T b) { left = l; top = t; right = r; bottom = b; return *this; }
	void SetRect(T l, T t, T r, T b) { left = l; top = t; right = r; bottom = b; }
	BOOL PtInRect(const FPoint &p) { return p.x >= left && p.y >= top && p.x <= right && p.y <= bottom; };
	bool PtInRect(T x, T y) const { return x >= left && y >= top && x <= right && y <= bottom; };
	TRect operator & (const TRect &r);
	TRect operator | (const TRect &r);
	TRect operator * (const T&);
	bool operator < (const TRect &rc)const;
	bool operator > (const TRect &rc)const;
	double Weight() const { return fabs(Width() * Height()); }
	TRect &operator = (const TRect &rc) { left = rc.left; top = rc.top; right = rc.right; bottom = rc.bottom; return *this; }
	void Offset(T x, T y) { left += x; top += y; right += x; bottom += y; }
	void Inflate(T x, T y)  { left -= x; top -= y; right += x; bottom += y; }
	TRect &operator &= (const TRect &r);
	TRect &operator |= (const TRect &r);
	TRect &operator /= (T r) { if (r) { left /= r; top /= r; right /= r; bottom /= r; } return *this; }
	TRect &operator *= (T r) { left *= r; top *= r; right *= r; bottom *= r; return *this; }
	TRect &operator += (const TRect r) { left -= r.left; top -= r.top; right += r.right; bottom += r.bottom; return *this; }
	TRect &operator += (const T &r) { left -= r; top -= r; right += r; bottom += r; return *this; }
	BOOL IsEmpty() {return fabs(left) <= PRECISION && fabs(top) <= PRECISION &&
			fabs(right) <= PRECISION && fabs(bottom) <= PRECISION; }
	T fabs(const T &t) const { return t >= 0 ? t : -t; } 

	CROSS IsCross(TRect &rc)
	{
		T l = max(rc.left, left);
		T r = min(rc.right, right);
		T t = max(rc.top, top);
		T b = min(rc.bottom, bottom);
		CROSS ret = EMPTY;
		if (l <= r && t <= b)
		{
			if (fabs(left - rc.left) <= PRECISION && fabs(top - rc.top) <= PRECISION &&
				fabs(right - rc.right) <= PRECISION && fabs(bottom - rc.bottom) <= PRECISION)
					ret = EQUAL;
			else
			if (rc.left == l && rc.top == t && rc.right == r && rc.bottom == b)
				ret = ENCLOSURED_R2This;
			else
			if (left == l && top == t && right == r && bottom == b)
				ret = ENCLOSURED_This2R;
			else
				ret = INTERSECT;
		}
		return ret;

	}

	bool IsIntersect(TRect &r);
	BOOL operator == (const TRect &r) const;
	BOOL operator != (const TRect &r)const { return !(operator == (r)); }
	operator RECT () { RECT r; ::SetRect(&r, (int)left, (int)top, (int)(right + 0.5), (int)(bottom + 0.5)); return r; }
	T left;
	T bottom;
	T right;
	T top;
};
#pragma pack(pop)
template <class T>
inline TRect<T> TRect<T>::operator & (const TRect &r) 
{
	TRect newr;
	newr.left = max(left, r.left);
	newr.top = max(top, r.top);
	newr.right = min(right, r.right);
	newr.bottom = min(bottom, r.bottom);
	return newr.left <= newr.right && newr.top <= newr.bottom ? newr : newr.Set(0, 0, 0, 0);
}
template <class T>
inline bool TRect<T>::IsIntersect(TRect &r) 
{
	return max(left, r.left) <= min(right, r.right) && 
		max(top, r.top) <= min(bottom, r.bottom) ? true : false;
}

template <class T>
inline TRect<T> TRect<T>::operator | (const TRect &r)  
{
	return TRect(min(left, r.left), min(top, r.top), max(right, r.right), max(bottom, r.bottom)); 
}

template <class T>
inline TRect<T> &TRect<T>::operator &= (const TRect &r) 
{
	left = max(left, r.left);
	top = max(top, r.top);
	right = min(right, r.right);
	bottom = min(bottom, r.bottom);
	if (left > right || top > bottom)
		Set(0, 0, 0, 0);
	return *this;
}
template <class T>
inline TRect<T> TRect<T>::operator * (const T &n) 
{
	return TRect(left * n, top * n, right * n, bottom * n);
}

template <class T>
inline TRect<T> &TRect<T>::operator |= (const TRect &r)  
{
	left = min(left, r.left);
	top = min(top, r.top), 
	right = max(right, r.right);
	bottom = max(bottom, r.bottom); 
	return *this;
}

template <class T>
inline BOOL TRect<T>::operator == (const TRect<T> &r) const 
{
	return fabs(left - r.left) <= PRECISION && fabs(top - r.top) <= PRECISION &&
		fabs(right - r.right) <= PRECISION && fabs(bottom - r.bottom) <= PRECISION;
}

template <class T>
inline bool TRect<T>::operator < (const TRect<T> &rc) const
{
	return rc.Width() * rc.Height() > Width() * Height();
}
template <class T>
inline bool TRect<T>::operator > (const TRect<T> &rc) const
{
	return rc.Width() * rc.Height() < Width() * Height();
}


typedef TRect< float > FRect;

//typedef TRect< int > IRect;

#define DPoint Vector2D
#define DRect FRect

#endif
