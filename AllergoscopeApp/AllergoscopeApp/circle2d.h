///////////////////////////////////////////////////
// Circle2D.h
// GRAPHICS Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////
#ifndef __Circle2D__
#define __Circle2D__
#pragma pack(push)
#pragma pack(8)
struct Circle2D
{
 	Circle2D():radius(0),radiusQ(0) {}
 	Circle2D(Circle2D &s):radius(s.radius),radiusQ(s.radiusQ),center(s.center) {}

	Circle2D(Vector2D &c, double r): center (c), radius (r), radiusQ(r * r) {}
	Circle2D(Box2D &b): center (b.CenterPoint()) { radius = ((b.maxPoint - b.minPoint) * 0.5).Len(); radiusQ = radius * radius; }

	Box2D Box();
	BOOL operator > (const Circle2D &a) const { return fabs(radius - a.radius) > PRECISION ? (radius > a.radius) : (center > a.center); }
	BOOL operator < (const Circle2D &a) const { return fabs(radius - a.radius) > PRECISION ? (radius < a.radius) : (center < a.center); }
	BOOL operator == (const Circle2D &a) const { return fabs(radius - a.radius) < PRECISION && center == a.center; }
	BOOL operator != (const Circle2D &a) const { return !operator == (a); }
	operator Vector2D() const { return center; } 
	void Init(Vector2D &c, double r) { center = c; radius = r; radiusQ = r * r; }
	void Init(Box2D &b) { center = b.CenterPoint(); radius = ((b.maxPoint - b.minPoint) * 0.5).Len(); radiusQ = radius * radius; }
 	void Init(Circle2D &s) { radius = s.radius; radiusQ = s.radiusQ; center = s.center; }
 	double Dist(Vector2D &b);
	void Reset() { radius = radiusQ = 0; }
	void MakeBox(Box2D &box, double r);
	void Update(Vector2D &p);
	Circle2D &operator |= (Box2D &b);
	Circle2D &operator |= (Circle2D &s);
	bool PtInCircle2D (Vector2D &p);
	bool PtInCircle2D (Vector3F &p);
	CROSS Cross (Circle2D &sh);

	bool IsCross(Vector2D &ray, Vector2D &n);
	int	Dir(Line2D &line);
	bool IsCross(Line2D &line);
	bool Intersect(Vector2D &ray, Vector2D &n, double &t0, double &t1);
	bool Clip(Vector2D &org, Vector2D &dest);
	bool IsClip(Vector2D &org, Vector2D &dest);

	Vector2D center;
	double radius;
	double radiusQ;
};
#pragma pack(pop)
inline bool Circle2D::IsClip(Vector2D &org, Vector2D &dest)
{
	if (dest == org) return PtInCircle2D(org);
	Vector2D v = org - center;
	double f1 = v.LenQ() <= radiusQ;
	double f2 = (dest - center).LenQ() <= radiusQ;
	if (f2 && f1)
		return true;
	Vector2D n = dest - org;
	double A = n * n;
	double B = 2 * (n * v);
	double C = (v * v) - radiusQ;
	double D = B * B - 4.0 * A * C;
	A *= 2.0;
    if(D < 0.0)
          return FALSE;
	return true;
}
inline bool Circle2D::Clip(Vector2D &org, Vector2D &dest) 
{
	if (dest == org) return PtInCircle2D(org);
	Vector2D v = org - center;
	double f1 = v.LenQ() <= radiusQ;
	double f2 = (dest - center).LenQ() <= radiusQ;
	if (f2 && f1)
		return true;
	Vector2D n = dest - org;
	double A = n * n;
	double B = 2 * (n * v);
	double C = (v * v) - radiusQ;  
	double D = B * B - 4.0 * A * C;
	A *= 2.0;
    if(D < 0.0)
          return FALSE;
    D = sqrt(D);
    double  t0 = (-B - D) / A;
    double t1 = (-B + D) / A;
	if (t0 > 1.0) t0 = -1.0;
	if (t1 > 1.0) t1 = -2.0;
    if (t0 >= 0.0 || t1 >= 0.0)
    {
		if (t1 < 0.0f)
			org = org + t0 * n;
		else
		if (t0 < 0.0f)
			dest = org + t1 * n;
		else
		if (f1 != f2)
		{
			if (!f1)
				dest = org + min(t0, t1) * n;
			else
				org = org + max(t0, t1) * n;		
		}
		else
		{
			dest = org + max(t0, t1) * n;
			org += min(t0, t1) * n;
		}
   		return TRUE;
    }
	return 0;
}   

//------------------------------------
//
//------------------------------------
inline bool Circle2D::Intersect(Vector2D &ray, Vector2D &n, double &t0, double &t1)
{
	Vector2D v = center - ray;
	double A = n * n;
	double B = 2 * (n * v);
	double C = v * v - radiusQ;  
	double D = B * B - 4.0 * A * C;
	A *= 2;
    if(D < 0.0)
          return false;
     D = sqrt(D);
    t0 = (-B - D) / A;
    t1 = (-B + D) / A;
    return t0 >= 0.0 || t1 >= 0.0;
}


//------------------------------------
//
//------------------------------------
inline bool Circle2D::IsCross(Vector2D &ray, Vector2D &n)
{
	Vector2D d = center - ray;
	double lenQ = d * d;
	 if (lenQ > radiusQ)
	 {
 		double _prj = d * n;
		Vector2D projection(n);
		projection *= _prj;
		d -= projection;
		return fabs(radiusQ - (d * d)) > -PRECISION;
	}
	return true;
}
//------------------------------------
//
//------------------------------------
inline Box2D Circle2D::Box() 
{
	Box2D box; 
	MakeBox(box, radius);
	return box; 
}

//------------------------------------
//
//------------------------------------
inline void Circle2D::MakeBox(Box2D &box, double r)
{
	box.Reset();
	Vector2D p;
	p = center;
	p -= r;
	box.Update(p);
	p += r * 2;
	box.Update(p);

}		

//------------------------------------
//
//------------------------------------
inline void Circle2D::Update(Vector2D &p)
{
	double rQ = (p - center).LenQ();

	if (rQ > radiusQ)
	{
		radiusQ = rQ;
		radius = sqrt(rQ);
	}
}
//------------------------------------
//
//------------------------------------
inline Circle2D &Circle2D::operator |= (Box2D &b)
{
	Circle2D s(b);
	return operator |= (s);  
	
	
}
//------------------------------------
//
//------------------------------------
inline Circle2D &Circle2D::operator |= (Circle2D &s)
{
	Vector2D dc(s.center - center);
	double lc = dc.Len();
	dc *= ((lc + (s.radius - radius))/(2 * lc));
	radius = (radius + s.radius + lc)/2;
	center = center + dc;
	radiusQ = radius * radius;
	return *this;
}
//------------------------------------
//
//------------------------------------
inline bool Circle2D::PtInCircle2D (Vector2D &p)
{
	return (p - center).LenQ() <= radiusQ;
}
//------------------------------------
//
//------------------------------------
inline double Circle2D::Dist(Vector2D &p)
{
	return (p - center).Len() - radius;
}
//------------------------------------
//
//------------------------------------
inline CROSS Circle2D::Cross (Circle2D &sh)
{
	double d = (sh.center - center).LenQ();
	if (d + sh.radiusQ <= radiusQ) return ENCLOSURED;
	if (d <= radiusQ + sh.radiusQ) return INTERSECT;
	return EMPTY;
}

//------------------------------------
//
//------------------------------------
inline int	Circle2D::Dir(Line2D &line)
{
	double d = line.Distance(center);
	int dir;
	if (d > PRECISION)
        dir = Line2D::FRONT;
    else
    if (d < -PRECISION)
        dir = Line2D::BACK;
    else
		dir = Line2D::LINE;
	if (d <= radius && d >= -radius) 
		dir |= Line2D::INTERSECT;
	return dir;
}
//------------------------------------
//
//------------------------------------
inline bool Circle2D::IsCross(Line2D &line)
{
	double d = line.Distance(center);
	return d > radius || d < -radius; 
}

#endif
