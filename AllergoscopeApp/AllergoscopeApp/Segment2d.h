#ifndef __SEGMENT2D__
#define __SEGMENT2D__
#pragma pack(push)
#pragma pack(8)
struct Segment2D {
	Vector2D org, dest;
	Segment2D() {};
	Segment2D(const Vector2D &_org, const Vector2D &_dest): org(_org), dest(_dest) {};
	Segment2D(const POINT &_org, const POINT &_dest) : org(_org), dest(_dest) {  };
#ifndef ANDROID_NDK
	Segment2D(POINTL &_org, POINTL &_dest) : org(_org), dest(_dest) { };
#endif
	Segment2D(Segment2D &e) : org(e.org), dest(e.dest) { }
	void Init(const Vector2D &_org, const Vector2D &_dest) { org = _org; dest = _dest;  }
	void Init(const POINT &_org, const POINT &_dest) { org = _org; dest = _dest;  }

	void Set(const Vector2D &_org, const Vector2D &_dest) { org = _org; dest = _dest;  }
	Segment2D &Flip() { Vector2D tmp = org; org = dest; dest = tmp; return *this; }
	Vector2D Point(double t);
	void Point(double t, Vector2D &res);
	BOOL Intersect(Segment2D &d, Vector2D &res, double _PRECISION = PRECISION);
	Line2D::ORIENT Intersect(Segment2D &d, double &t, double _PRECISION = PRECISION);
	Line2D::ORIENT Intersect(Line2D &d, double &t, double _PRECISION = PRECISION);
	BOOL Intersect(Line2D &d, Vector2D &res, double _PRECISION = PRECISION);
	
	BOOL Cross(Segment2D &d, double &t);
	BOOL PtInSegment(Vector2D &v, double _PRECISION = PRECISION);
	Vector2D Min() { return Vector2D(min(org.x, dest.x), min(org.y, dest.y)); }
	Vector2D Max() { return Vector2D(max(org.x, dest.x), max(org.y, dest.y)); }
	double Len() { return (dest - org).Len(); }
	double LenQ() { return (dest - org).LenQ(); }
	int	Dir(const Vector2D &v, double _PRECISION = PRECISION);
	bool Expand(const Vector2D &v, double _PRECISION = PRECISION);

	double PerpDistance (const Vector2D &v);
	void Distance(const Vector2D &pt, double &dx, double &dy);
	void PerpPoint (Vector2D &p, Vector2D &res);
	Line2D::ORIENT Orient(Line2D &d);
	Line2D::ORIENT Orient(Segment2D &d);
	Line2D::ORIENT Orient(Vector2D &dorg, Vector2D &ddest);

	Vector2D::ORIENT Orient(Vector2D &p) { return p.Orient(org, dest); }
	int Sgn(Segment2D &);
};
#pragma pack(pop)
inline int Segment2D::Sgn(Segment2D &p) { 
	Vector2D n1(org.y - dest.y, dest.x - org.x);
	Vector2D n2(p.org.y - p.dest.y, p.dest.x - p.org.x);
	return SGN(n2.x * n1.y - n1.x * n2.y);
} 

inline Line2D::ORIENT Segment2D::Orient(Line2D &d)
{
	Vector2D n(org.y - dest.y, dest.x - org.x);
	double ds = n * d.Dir();
	if (fabs(ds) < PRECISION)
	{
		Vector2D::ORIENT direct = d.Point().Orient(org, dest);
		return direct == Vector2D::LEFT || direct == Vector2D::RIGHT ? Line2D::PARALLEL : Line2D::COLLINEAR;
			
	}
	return Line2D::CROSS;
}
inline Line2D::ORIENT Segment2D::Orient(Segment2D &d)
{
	Vector2D n(org.y - dest.y, dest.x - org.x);
	double ds = n * (d.dest - d.org);
	if (fabs(ds) < PRECISION)
	{
		Vector2D::ORIENT direct = d.org.Orient(org, dest);
		return direct == Vector2D::LEFT || direct == Vector2D::RIGHT ? Line2D::PARALLEL : Line2D::COLLINEAR;
			
	}
	return Line2D::CROSS;
}
inline Line2D::ORIENT Segment2D::Orient(Vector2D &dorg, Vector2D &ddest)
{
	Vector2D n(org.y - dest.y, dest.x - org.x);
	double ds = n * (ddest - dorg);
	if (fabs(ds) < PRECISION)
	{
		Vector2D::ORIENT direct = dorg.Orient(org, dest);
		return direct == Vector2D::LEFT || direct == Vector2D::RIGHT ? Line2D::PARALLEL : Line2D::COLLINEAR;

	}
	return Line2D::CROSS;
}

//------------------------------------
//
//------------------------------------
inline void Segment2D::PerpPoint (Vector2D &v, Vector2D &res)
{
	Vector2D n(org.y - dest.y, dest.x - org.x);
	double dist = org.x * dest.y - dest.x * org.y;
	double d = n.Len(); 
	if (d != 0.0)
	{	
		dist /= d;
		n /= d;
		d = -dist - (v * n);
		res = v + (d * n);
	}
	else
		res = org;

}

//------------------------------------
//
//------------------------------------
inline double Segment2D::PerpDistance (const Vector2D &v) 
{
	Vector2D n(org.y - dest.y, dest.x - org.x);
	double dist = org.x * dest.y - dest.x * org.y;
	double d = n.Len(); 
	if (d != 0.0)
	{
		dist /= d;
		n /= d;
	
		return v.x * n.x + v.y * n.y + dist;
	}
	return 0;
}
//------------------------------------
//
//------------------------------------
inline void Segment2D::Distance(const Vector2D &pt, double &dx, double &dy)
{
	double dx_l = dest.x - org.x;
	double dy_l = dest.y - org.y;
	if (fabs(dx_l) < PRECISION && fabs(dy_l) < PRECISION)
	{
		dx = pt.x - org.x;
		dy = pt.y - org.y;
	}
	else
	{
		double dx_t = pt.x - org.x;
		double dy_t = pt.y - org.y;
		if (fabs(dx_t) < PRECISION && fabs(dy_t) < PRECISION)
		{
			dx = 0.0;
			dy = 0.0;
		}
		else
		{
			double cosa = (dx_t * dx_l + dy_t * dy_l) / (dx_l * dx_l + dy_l * dy_l);
			dx = -dx_t + dx_l * cosa;
			dy = -dy_t + dy_l * cosa;
		}
	}
	double x_x = pt.x + dx;
	double y_x = pt.y + dy;
	double dx_x = x_x - org.x;
	double dy_x = y_x - org.y;
	if (dx_x * dx_l + dy_x * dy_l < 0)
	{
		dx = org.x - pt.x;
		dy = org.y - pt.y;
	}
	else
	{
		dx_x = x_x - dest.x;
		dy_x = y_x - dest.y;
		if (-dx_x * dx_l - dy_x * dy_l < 0.0)
		{
			dx = dest.x - pt.x;
			dy = dest.y - pt.y;
		}
	}
	
}
 
//------------------------------------
//
//------------------------------------
inline int Segment2D::Dir(const Vector2D &v, double _PRECISION)
{
	if (v != org && dest != v)
	{
		double nx = org.y - dest.y;
		double ny = dest.x - org.x;
		double dist = org.x * dest.y - dest.x * org.y;
		double d = nx * v.x + ny * v.y + dist;
		if (d > _PRECISION)
			return Line2D::FRONT;
		else
		if (d < -_PRECISION)
			return Line2D::BACK;
	}
	return Line2D::LINE;
}
//------------------------------------
//
//------------------------------------
inline bool Segment2D::Expand(const Vector2D &v, double _PRECISION)
{
	if (v == org || dest == v) return true;
	Vector2D a = dest - org;
	Vector2D b = v - org;
		double sa = a.x * b.x;
	double sb = b.y * a.y;
	double sab = a.x * b.y - b.x * a.y;

	if (sab > _PRECISION || sab < -_PRECISION) return false;
	if (sa < -_PRECISION || sb < -_PRECISION)
		org = v;
	else
	if (a.x * a.x + a.y * a.y < b.x * b.x + b.y * b.y)
		dest = v;
	return true;
}

//------------------------------------
//
//------------------------------------
inline BOOL Segment2D::PtInSegment(Vector2D &v, double _PRECISION)
{ 
	if (org == v || dest == v) return true;
	Vector2D a = dest - org;
	Vector2D b = v - org;
	double sab = a.x * b.y - b.x * a.y;
	if (sab > _PRECISION || sab < -_PRECISION) return false;
 
	if (a.x * b.x < -_PRECISION || b.y * a.y < -_PRECISION) return false;
	double sa = a.x * a.x + a.y * a.y;
	double sb = b.x * b.x + b.y * b.y ;
	return fabs(sa - sb) < _PRECISION || sa > sb;
}	

//------------------------------------
//
//------------------------------------
inline Vector2D Segment2D::Point(double t) { return Vector2D(org  + t *(dest - org)); }
//------------------------------------
//
//------------------------------------
inline void Segment2D::Point(double t, Vector2D &res) { res = org + t *(dest - org); }

//------------------------------------
//
//------------------------------------
inline Line2D::ORIENT Segment2D::Intersect(Segment2D &d, double &t, double _PRECISION) 
{ 
	Vector2D n(d.dest.y - d.org.y, d.org.x - d.dest.x);
	double ds = n * (dest - org);
	if (fabs(ds) < _PRECISION)
	{
		Vector2D::ORIENT direct = org.Orient(d.org, d.dest);
		if (direct == Vector2D::LEFT || direct == Vector2D::RIGHT)
			return Line2D::PARALLEL;
		return Line2D::COLLINEAR;
			
	}
	double u = n * (org - d.org);
	t = -u/ds;
	return Line2D::CROSS;
}
//------------------------------------
//
//------------------------------------
inline Line2D::ORIENT Segment2D::Intersect(Line2D &d, double &t, double _PRECISION) 
{ 
	double ds = d.n * (dest - org);
	if (fabs(ds) < _PRECISION)
	{
		Vector2D org2 = d.Point();
		Vector2D::ORIENT direct = org.Orient(org2, org2 + d.Dir());
		if (direct == Vector2D::LEFT || direct == Vector2D::RIGHT)
			return Line2D::PARALLEL;
		return Line2D::COLLINEAR;
			
	}
	double u = -(d.dist + (org * d.n));
	t = u/ds;
	return Line2D::CROSS;
}
//------------------------------------
//
//------------------------------------
inline BOOL Segment2D::Intersect(Line2D &d, Vector2D &res, double _PRECISION) 
{ 
	double t;
	if (Intersect(d, t, _PRECISION) == Line2D::CROSS)
	{
		if (t < 0.0 || t > 1.0)    
			return FALSE;
		res = org + t * (dest - org);
		return true; 
	}
	return 0;
}

//------------------------------------
//
//------------------------------------
inline BOOL Segment2D::Intersect(Segment2D &d, Vector2D &res, double _PRECISION) 
{ 
	double t;
	if (Intersect(d, t, _PRECISION) == Line2D::CROSS)
	{
		if (t < 0.0 || t > 1.0)    
			return FALSE;
		Point(t, res);
		
		return d.PtInSegment(res,_PRECISION); 
	}
	return 0;
}

#endif
