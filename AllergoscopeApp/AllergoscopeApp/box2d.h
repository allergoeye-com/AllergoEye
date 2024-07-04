///////////////////////////////////////////////////
// box2d.h
// GRAPHICS Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////
#ifndef __BOX2D__
#define __BOX2D__
#include "line2d.h"
struct Box2D
{
 
    Box2D(const Vector2D &v1, const Vector2D &v2) { Set(v1, v2); }
    Box2D(int left,  int top, int right, int bottom) { Set(Vector2D(left, top), Vector2D(right, bottom)); }
    void Set(const Box2D &b) { minPoint = b.minPoint; maxPoint = b.maxPoint; }
	Box2D() { Reset (); }
	bool IsEmpty() { return maxPoint.x  == LONG_MIN; }
	double Width() { return maxPoint.x - minPoint.x; }
	double Height() { return maxPoint.y - minPoint.y; }
	void MakeMax(); 
    void Set(const Vector2D &v1, const Vector2D &v2);
    void Update(const Vector2D &v);
    void Update(const Vector2F &v);
	void Update(Vector2D *v, int num);
    bool PtInBox(const Vector2D &pos);
    bool PtInBox(double x, double y);
	bool Intersect(Box2D &box);
	void Reset();
    Vector2D operator [] (int index) const ;
	void Line(Line2D::PL i, Line2D &p);
	void Line(Line2D::PL i, Segment2D &p);
	Vector2D CenterPoint()	{ return (minPoint + maxPoint)/2.0; }
    Box2D &operator |= (Box2D &box);const 
    bool operator == (Box2D &box) { return minPoint == box.minPoint &&  maxPoint == box.maxPoint; }
    bool operator != (Box2D &box) { return minPoint != box.minPoint ||  maxPoint != box.maxPoint; }
    
    Box2D &operator &= (Box2D &box);
	CROSS Cross (Box2D &box);
	bool IsCross(Vector2D &vs, Vector2D &n);
	bool ClipSection(Vector2D &p1, Vector2D &p2);
	bool TestBarsk(double p, double q, double * u1,double * u2);
	void Inflate(Vector2D &delta);
	void Inflate(double delta);
	void Move(Vector2D &v);

    Vector2D minPoint;
    Vector2D maxPoint;

};
inline bool Box2D::TestBarsk(double p, double q, double * u1,double * u2)
{
	double r;
	if (p < 0.0) 
	{
		if ((r = q/p) > *u2)
			return 0;
		else
		if (r > *u1)
			*u1 = r; 
	}
	else
	if (p > 0.0) 
	{
		if ((r = q/p) < *u1)
			return 0;
		else 
		if (r < *u2)
			*u2 = r;
	}
	else
	if (q < 0.0)
		return 0;
	return 1;
}

inline bool Box2D::ClipSection(Vector2D &p1, Vector2D &p2)
{
	double u1 = 0.0, u2 = 1.0, dx = p2.x - p1.x, dy;
	if (TestBarsk(-dx, p1.x - minPoint.x, &u1, &u2))
		if (TestBarsk(dx, maxPoint.x - p1.x, &u1, &u2))
		{
			dy = p2.y - p1.y;
			if (TestBarsk(-dy, p1.y - minPoint.y, &u1, &u2))
				if (TestBarsk(dy, maxPoint.x - p1.y, &u1, &u2))
				{
					if (u2 < 1.0) 
					{
						p2.x = (p1.x + u2 * dx);
						p2.y = (p1.y + u2 * dy);
					}
					if (u1 > 0.0) 
					{
						p1.x = (p1.x + u1 * dx);
						p1.y = (p1.y + u1 * dy);
					}
					return 1;
				}
		}
	return 0;
}

//------------------------------------
//
//------------------------------------
inline void Box2D::MakeMax() 
{ 
	minPoint.x = LONG_MIN;
	minPoint.y = LONG_MIN;
	maxPoint.x = LONG_MAX;
	maxPoint.y = LONG_MAX;
}
//------------------------------------
//
//------------------------------------
inline void Box2D::Set(const Vector2D &v1, const Vector2D &v2)
{
	minPoint.x = min(v1.x, v2.x);
	minPoint.y = min(v1.y, v2.y);
	maxPoint.x = max(v1.x, v2.x);
	maxPoint.y = max(v1.y, v2.y);
}
//------------------------------------
//
//------------------------------------
inline void Box2D::Update(const Vector2D &v)
{
	minPoint.x = min(v.x, minPoint.x);
	minPoint.y = min(v.y, minPoint.y);
	maxPoint.x = max(v.x, maxPoint.x);
	maxPoint.y = max(v.y, maxPoint.y);
}
inline void Box2D::Update(const Vector2F &v)
{
	minPoint.x = min((double)v.x, minPoint.x);
	minPoint.y = min((double)v.y, minPoint.y);
	maxPoint.x = max((double)v.x, maxPoint.x);
	maxPoint.y = max((double)v.y, maxPoint.y);
}

//------------------------------------
//
//------------------------------------
inline void Box2D::Update(Vector2D *v, int num)
{
	while (num--)
	{
		minPoint.x = min(v->x, minPoint.x);
		minPoint.y = min(v->y, minPoint.y);
		maxPoint.x = max(v->x, maxPoint.x);
		maxPoint.y = max(v->y, maxPoint.y);
		++v;
	}
}

//------------------------------------
//
//------------------------------------
inline bool Box2D::PtInBox(const Vector2D &pos)
{
	return PtInBox(pos.x, pos.y);
}
//------------------------------------
//
//------------------------------------
inline bool Box2D::PtInBox(double x, double y)
{
    return x - minPoint.x > -PRECISION && maxPoint.x - x > -PRECISION  &&
           y - minPoint.y > -PRECISION && maxPoint.y - y > -PRECISION;
}
//------------------------------------
//
//------------------------------------
inline bool Box2D::Intersect(Box2D &box)
{
	return !(maxPoint.x < box.minPoint.x || minPoint.x > box.maxPoint.x ||
			maxPoint.y < box.minPoint.y || minPoint.y > box.maxPoint.y);
}

//------------------------------------
//
//------------------------------------
inline void Box2D::Reset()
{
	maxPoint.x = LONG_MIN;
	maxPoint.y = LONG_MIN;
	minPoint.x = LONG_MAX;
	minPoint.y = LONG_MAX;
}

//------------------------------------
//
//------------------------------------
inline Vector2D Box2D::operator [] (int index) const 
{
/****************************

 Y	        
0-------1	 
|	    |	 
|       |	 
|       |
2-------3 X	 	 
0 minPoint.x, minPoint.y
1 maxPoint.x, minPoint.y
2 minPoint.x, maxPoint.y
3 maxPoint.x, maxPoint.y

		 
*******************************/

    return Vector2D (index & 1 ? maxPoint.x : minPoint.x,
                      index & 2 ? maxPoint.y : minPoint.y);
}
//------------------------------------
//
//------------------------------------
inline void Box2D::Line(Line2D::PL i, Line2D &p)
{
	const int ind[4][2] = {
		{2, 0}, // left
		{0, 1}, // top
		{1, 3}, // right
		{3, 2}, // bottom
	};
	p.Init(operator [] (ind[i][0]), operator [] (ind[i][1]));
	Vector2D center = (minPoint + maxPoint)/2.0;
	if (p.n.x * center.x + p.n.y * center.y + p.dist < -PRECISION)
		p.Flip();

}
inline void Box2D::Line(Line2D::PL i, Segment2D &p)
{
	const int ind[4][2] = {
		{2, 0}, // left
		{0, 1}, // top
		{1, 3}, // right
		{3, 2}, // bottom
	};
	p.Set(operator [] (ind[i][0]), operator [] (ind[i][1]));
	Vector2D center = (minPoint + maxPoint)/2.0;
	if (p.Dir(center) == Line2D::BACK)
		p.Flip();

}

//------------------------------------
//
//------------------------------------
inline Box2D &Box2D::operator |= (Box2D &box)
{
	minPoint.x = min(box.minPoint.x, minPoint.x);
	minPoint.y = min(box.minPoint.y, minPoint.y);
	maxPoint.x = max(box.maxPoint.x, maxPoint.x);
	maxPoint.y = max(box.maxPoint.y, maxPoint.y);
	return *this;
}
//------------------------------------
//
//------------------------------------
inline Box2D &Box2D::operator &= (Box2D &box)
{
	if ((maxPoint.x < box.minPoint.x || minPoint.x > box.maxPoint.x ||
			maxPoint.y < box.minPoint.y || minPoint.y > box.maxPoint.y))
			Reset();
	else
	{
		minPoint.x = max(box.minPoint.x, minPoint.x);
		minPoint.y = max(box.minPoint.y, minPoint.y);
		maxPoint.x = min(box.maxPoint.x, maxPoint.x);
		maxPoint.y = min(box.maxPoint.y, maxPoint.y);
	}
	return *this;
}
//------------------------------------
//
//------------------------------------
inline CROSS Box2D::Cross (Box2D &box)
{
	if ((maxPoint.x < box.minPoint.x || minPoint.x > box.maxPoint.x ||
			maxPoint.y < box.minPoint.y || minPoint.y > box.maxPoint.y))
			return EMPTY;
	Box2D t; 
	t.minPoint.x = max(box.minPoint.x, minPoint.x);
	t.minPoint.y = max(box.minPoint.y, minPoint.y);
	t.maxPoint.x = min(box.maxPoint.x, maxPoint.x);
	t.maxPoint.y = min(box.maxPoint.y, maxPoint.y);
	return box.maxPoint != t.maxPoint || t.minPoint != box.minPoint ? INTERSECT : ENCLOSURED;

}
//------------------------------------
//
//------------------------------------
inline bool Box2D::IsCross(Vector2D &vs, Vector2D &n)
{ 
	if (!PtInBox(vs))
	{
		double t;
		Line2D bp;
		Vector2D pos;
		for (int i = 0; i < 4; ++i)
		{
			Line((Line2D::PL)i, bp);
			if (bp.Intersect(vs, n, t))
			{
				pos = vs + t * n;
				if (pos.x >= minPoint.x && pos.x <= maxPoint.x &&
				   pos.y >= minPoint.y && pos.y <= maxPoint.y)
					return true;
			}
		}
		return false;
	}
	return true;
}
//------------------------------------
//
//------------------------------------
inline void Box2D::Inflate(Vector2D &delta)
{
	minPoint -= delta;
	maxPoint += delta;
}
//------------------------------------
//
//------------------------------------
inline void Box2D::Inflate(double delta)
{
	minPoint.x -= delta;
	minPoint.y -= delta;
	maxPoint.x += delta;
	maxPoint.y += delta;
}

//------------------------------------
//
//------------------------------------
inline void Box2D::Move(Vector2D &v)
{
	minPoint += v;
	maxPoint += v;
}


//------------------------------------
//
//------------------------------------
inline Box2D operator | (Box2D &a, Box2D &box)
{
	Box2D b;	
	b.minPoint.x = min(box.minPoint.x, a.minPoint.x);
	b.minPoint.y = min(box.minPoint.y, a.minPoint.y);
	b.maxPoint.x = max(box.maxPoint.x, a.maxPoint.x);
	b.maxPoint.y = max(box.maxPoint.y, a.maxPoint.y);
	return b;
}
//------------------------------------
//
//------------------------------------
inline Box2D operator & (Box2D &a, Box2D &box)
{
	Box2D b;
	if (!(a.maxPoint.x < box.minPoint.x || a.minPoint.x > box.maxPoint.x ||
				a.maxPoint.y < box.minPoint.y || a.minPoint.y > box.maxPoint.y))
	{	
		b.minPoint.x = max(box.minPoint.x, a.minPoint.x);
		b.minPoint.y = max(box.minPoint.y, a.minPoint.y);
		b.maxPoint.x = min(box.maxPoint.x, a.maxPoint.x);
		b.maxPoint.y = min(box.maxPoint.y, a.maxPoint.y);
		
	}
	return b;
}
#endif
