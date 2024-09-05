///////////////////////////////////////////////////
// Line2D.h
// GRAPHICS Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2015
////////////////////////////////////////////////////
#ifndef __LINE2D__
#define __LINE2D__
#pragma pack(push)
#pragma pack(8)
struct Line2D {
	enum {LINE, FRONT = 1, BACK = 2, INTERSECT = 4 };
	enum PL { PL_LEFT, PL_TOP, PL_RIGHT, PL_BOTTOM };
	enum ORIENT { COLLINEAR = 1, PARALLEL, CROSS };

	Line2D() { Init(Vector2D(1.0, 1.0), 0.0); }
	Line2D(const Vector2D &v, double d) { Init(v, d); }
	Line2D(const Vector2D &org, const Vector2D &dest) { Init(org, dest); }
	Line2D(const Line2D &p) { n = p.n; dist = p.dist; }
    void Init(const Vector2D &org, const Vector2D &dest);
	void Init(const Vector2D &v, double d);
	Vector2D Dir() { return Vector2D(-n.y, n.x); }
    Vector2D Point () { return n * -dist; }
	void MoveTo(const Vector2D &v)  { dist = -(n.x * v.x + n.y * v.y); }
	double Distance (Vector2D &v) {return fabs(v.x * n.x + v.y * n.y + dist); }
	int	Dir(const Vector2D &v);
	int Dir(double x, double y);
	BOOL PtIn(double x, double y)  { return Dir(x, y) == LINE; }
	BOOL PtIn(Vector2D &v)  { return Dir(v) == LINE; }

	double PerpPoint (Vector2D &p, Vector2D &res);
	void Flip() { n  = -n; dist = -dist; }
	bool operator < (const Line2D &a)  const { return operator != (a) && (n < a.n || (n == a.n && dist < a.dist)); }
	bool operator > (const Line2D &a)  const { return (operator != (a)) && (n > a.n || (n == a.n && dist > a.dist)); }       
	bool operator == (const Line2D &a) const  { double b = (a.n * n) - 1.0; 
        return b >= 0 && b < PRECISION && (dist == a.dist || 
                                (fabs(n.x) > PRECISION && a.n.x/n.x == a.dist/dist) ||
                                (fabs(n.y) > PRECISION && a.n.y/n.y == a.dist/dist)); }
	bool operator != (const Line2D &a) const { return !(operator == (a)); }    
	bool IsIntersect(Line2D &p) { double b = fabs((p.n * n)) - 1.0; return b < -PRECISION || b > PRECISION; }
	ORIENT Intersect(Vector2D &org, Vector2D &dir, double &t);
	ORIENT Orient(Line2D &d);
	bool Intersect(Line2D &d, Vector2D &res);
	bool Intersect(Vector2D &org, Vector2D &dir, Vector2D &res);

	double dist;
	Vector2D n;
};
#pragma pack(pop)
//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
inline void Line2D::Init(const Vector2D &org, const Vector2D &dest)
{
	double l = (dest - org).LenQ(); 
	if (l < PRECISION) return;  
	
	n.x = org.y - dest.y;
	n.y = dest.x - org.x;
	dist =  org.x * dest.y - dest.x * org.y;
	double d = n.Len(); 
	dist /= d;
	n /= d;
}
//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
inline void Line2D::Init(const Vector2D &v, double d)
{
	n = v;
	n.Normalize();
	dist = d;
}

//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
inline int	Line2D::Dir(const Vector2D &v)
{
	double d = n.x * v.x + n.y * v.y + dist;
	if (d > PRECISION)
		return FRONT;
	else
	if (d < -PRECISION)
		return BACK;

	return LINE;
}
inline int Line2D::Dir(double x, double y)
{
	double d = n.x * x + n.y * y + dist;
	if (d > PRECISION)
		return FRONT;
	else
	if (d < -PRECISION)
		return BACK;

	return LINE;
}

//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
inline double Line2D::PerpPoint (Vector2D &p, Vector2D &res)
{
	double d = -dist - (p * n);
	res = p + (d * n);
	return d;
}

//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
inline bool Line2D::Intersect(Line2D &d, Vector2D &res)
{
	
	Vector2D org = d.Point();
	Vector2D dir = d.Dir();
	double t;
	if (Intersect(org, dir, t) != Line2D::CROSS)
		return false;
	res = org + t * dir;
	
	return true;
}
//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
inline bool Line2D::Intersect(Vector2D &org, Vector2D &dir, Vector2D &res)
{
	double t;
	if (Intersect(org, dir, t))
	{
		res = org + t * dir;
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
inline Line2D::ORIENT Line2D::Orient(Line2D &d)
{
	double ds = n * d.Dir();
	if (fabs(ds) < PRECISION)
	{
		Vector2D org2 = Point();
		Vector2D org = d.Point();
		Vector2D::ORIENT direct = org.Orient(org2, org2 + Dir());
		return direct == Vector2D::LEFT || direct == Vector2D::RIGHT ? PARALLEL : COLLINEAR;
			
	}
	return CROSS;
}
//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------

inline Line2D::ORIENT Line2D::Intersect(Vector2D &org, Vector2D &dir, double &t)
{
	double denom = dir * n;
	if (fabs (denom) < PRECISION)
	{
		Vector2D org2 = Point();
		Vector2D::ORIENT direct = org.Orient(org2, org2 + Dir());
		if (direct == Vector2D::LEFT || direct == Vector2D::RIGHT)
			return PARALLEL;
		return COLLINEAR;
			
	}
	double numer = -(dist + (org * n));
	t = numer/denom;
	return CROSS;
}

#include "Segment2d.h"
#endif
