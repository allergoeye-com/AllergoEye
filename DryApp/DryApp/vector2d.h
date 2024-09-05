///////////////////////////////////////////////////
// Vector2D.h
// GRAPHICS Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////
#ifndef __VECTOR2D__
#define __VECTOR2D__
#include "Vector2F.h"
#pragma pack(push)
#pragma pack(8)

struct Vector2D {
	enum ORIENT { LEFT, RIGHT, BEYOND/*do*/, BEHIND/*posle*/, BETWEEN/*mezhdu*/, ORG, DEST };

	double x;
	double y;

	Vector2D(double ix = 0, double iy = 0):x(ix), y(iy) { }
	Vector2D(const Vector2D &p) : x(p.x), y(p.y) {}
	Vector2D(const Vector2F &p) : x(p.x), y(p.y) {}

	Vector2D(const POINT &p) : x(p.x), y(p.y) {}
#ifndef ANDROID_NDK
	Vector2D(const POINTL &p) : x(p.x), y(p.y) {}
#endif
	void Set(double ix, double iy) { x = ix; y = iy; }
	Vector2D &operator = (const Vector2F &p) { x = p.x; y = p.y;  return *this; }
#ifndef ANDROID_NDK
	Vector2D &operator = (const POINTL &p) { x = p.x; y = p.y;  return *this; }
#endif
	Vector2D &operator = (const POINT &p) { x = p.x; y = p.y; return *this;}
	Vector2D &operator += (const Vector2D &p)  { x += p.x, y += p.y; return *this; } 
	Vector2D &operator -= (const Vector2D &p) { x -= p.x, y -= p.y; return *this; }
	Vector2D &operator /= (double d)  { x /= d, y /= d; return *this; }
	Vector2D &operator *= (double d)  { x *= d, y *= d; return *this; }
	Vector2D operator * (double d)  {Vector2D v(*this); v.x *= d, v.y *= d; return v; }
	Vector2D operator / (double d)  {Vector2D v(*this); v.x /= d, v.y /= d; return v; }
	Vector2D operator - () const { return Vector2D (-x, -y); }	
	operator Vector2F () const  { return Vector2F ((float)x, (float)y); }	
	double operator [] (int i) const { return !i ? x : y; }
	double &operator [] (int i) { return !i ? x : y; }
	BOOL operator == (const Vector2D &p) const { return fabs(x - p.x) < PRECISION && fabs(y - p.y) < PRECISION; }
	BOOL operator != (const Vector2D &p) const  { return !operator == (p); }
	operator CPoint () const { return CPoint((int)(x + 0.5), (int)(y + 0.5)); }
#ifndef ANDROID_NDK
	operator POINTL ()const { POINTL t; t.x = (int)(x + 0.5); t.y = (int)(y + 0.5); return t; }
#endif
	operator POINT () const{ POINT t; t.x = (int)(x + 0.5); t.y = (int)(y + 0.5); return t; } 
	double Len() { return sqrt (x*x + y*y); }
	double LenQ() { return x*x + y*y; }
	double Modul ()  { return Len(); }
	Vector2D Ort() { double l = Len(); return l ? Vector2D(x/l, y/l) : Vector2D(0, 0); }
	Vector2D Perp() { return Vector2D(-y, x); }
	void Normalize() { double l = Len(); if (l > 0.0) { x /= l; y /= l; } }
	double PolarAngle() const;
	double Angle(const Vector2D &p2);
	ORIENT Orient(const Vector2D &org, const Vector2D &dest);
	bool operator < (const Vector2D &p) const { if (fabs(x - p.x) > PRECISION)  return x < p.x;
													if (fabs(y - p.y) > PRECISION) return y < p.y;
													return false;
												}
	
	bool operator > (const Vector2D &p) const { if (fabs(x - p.x) > PRECISION)  return x > p.x;
													if (fabs(y - p.y) > PRECISION) return y > p.y;
													return false;
												}

};
#pragma pack(pop)

 
//------------------------------------
//
//------------------------------------
inline double operator * (const Vector2D &p, const Vector2D &p1) { return p.x * p1.x + p.y * p1.y; } 
//------------------------------------
//
//------------------------------------
inline Vector2D operator + (const Vector2D &p, const Vector2D &p1) { return Vector2D(p.x + p1.x, p.y + p1.y); } 
//------------------------------------
//
//------------------------------------
inline Vector2D operator - (const Vector2D &p, const Vector2D &p1) { return Vector2D(p.x - p1.x, p.y - p1.y); }
//------------------------------------
//
//------------------------------------
inline Vector2D operator * (double i, const Vector2D &p) { return Vector2D(p.x * i, p.y * i); }
//------------------------------------
//
//------------------------------------
inline double Vector2D::Angle(const Vector2D &p2) {
	double a = PolarAngle() - p2.PolarAngle();
	if (a < 0) a += 360.0f;
	if (a > 180) a -= 360.0f;
	return a;
}
//------------------------------------
//
//------------------------------------
inline double Vector2D::PolarAngle() const
{
	double a1 = x ? atan2(y, x): (y > 0 ? PI/2.0 : -PI/2.0);
	return a1 * 180.0/PI;
}
//------------------------------------
//
//------------------------------------
inline double PolarAngle(const Vector2D &p1, const Vector2D &p2,  const Vector2D &p3)
{
	return (p2 - p1).Angle(p3 - p1);
}
//------------------------------------
//
//------------------------------------
inline double PolarAngle(const Vector2D &p1, const Vector2D &p2,  const Vector2D &p3, const Vector2D &p4)
{
	return (p2 - p1).Angle(p4 - p3);
}
//------------------------------------
//
//------------------------------------
inline int direct (Vector2D &p0, Vector2D &p1, Vector2D &p2)
{
	Vector2D a = p1 - p0;
	Vector2D b = p2 - p0;
	double sa = a.x * b.y - b.x * a.y;
	if (sa > 0.0)
		return 1;
	if (sa < 0.0)
		return -1;
	return 0; 
}
//------------------------------------
//
//------------------------------------
inline Vector2D::ORIENT Vector2D::Orient(const Vector2D &org, const Vector2D &dest)
{
	if (org == *this)
		return ORG;
	if (dest == *this)
		return DEST;
	Vector2D a = dest - org;
	Vector2D b = *this - org;
	double sa = a.x * b.x;
	double sb = b.y * a.y;
	double sab = a.x * b.y - b.x * a.y;
	if (sab > PRECISION)
		return LEFT;
	if (sab < -PRECISION)
		return RIGHT;
 
	if (sa < -PRECISION || sb < -PRECISION)
		return BEHIND;
	sa = a.x * a.x + a.y * a.y;
	sb = b.x * b.x + b.y * b.y;

	if (sa < sb)
		return BEYOND;
	return BETWEEN;

}

#endif
