///////////////////////////////////////////////////
// box3d.h
// GRAPHICS Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////
#ifndef __BOX3D__
#define __BOX3D__
struct Box3D
{
 
    Box3D(const Vector3D &v1, const Vector3D &v2) { Set(v1, v2); }
    void Set(const Box3D &b) { minPoint = b.minPoint; maxPoint = b.maxPoint; }
	Box3D() { Reset (); }
	bool IsEmpty() { return maxPoint.x  == LONG_MIN; }
	double Width() { return maxPoint.x - minPoint.x; }
	double Height() { return maxPoint.y - minPoint.y; }
	double Depth() { return maxPoint.z - minPoint.z; }
	void MakeMax(); 
    void Set(const Vector3D &v1, const Vector3D &v2);
    void Update(const Vector3D &v);
	void Update(Vector3D *v, int num);
    int Dir(Plane3D &plane);
	double Distance(Plane3D &plane);
    bool PtInBox(const Vector3D &pos);
    bool PtInBox(double x, double y, double z);
	bool PtInBox(const Vector3F &pos);
	bool Intersect(Box3D &box);
	void Reset();
    Vector3D operator [] (int index) const ;
    double Volume() ;
	void Plane (Plane3D::PL i, Plane3D &p);
	Vector3D CenterPoint()	{ return (minPoint + maxPoint)/2.0; }
    Box3D &operator |= (Box3D &box);const 
    bool operator == (Box3D &box) { return minPoint == box.minPoint &&  maxPoint == box.maxPoint; }
    bool operator != (Box3D &box) { return minPoint != box.minPoint ||  maxPoint != box.maxPoint; }
    
    Box3D &operator &= (Box3D &box);
	CROSS Cross (Box3D &box);
	bool IsCross(Vector3D &vs, Vector3D &n);
	void Inflate(Vector3D &delta);
	void Inflate(double delta);
	double	Distance(Vector3D &org, Vector3D &dir);
	void Move(Vector3D &v);
	void Transform(MFORM3D &m);
	void Transform(MFORM3D &m, Box3D &res);

    Vector3D minPoint;
    Vector3D maxPoint;

};

//------------------------------------
//
//------------------------------------
inline void Box3D::MakeMax() 
{ 
	minPoint.x = LONG_MIN;
	minPoint.y = LONG_MIN;
	minPoint.z = LONG_MIN;
	maxPoint.x = LONG_MAX;
	maxPoint.y = LONG_MAX;
	maxPoint.z = LONG_MAX;
}
//------------------------------------
//
//------------------------------------
inline void Box3D::Set(const Vector3D &v1, const Vector3D &v2)
{
	minPoint.x = min(v1.x, v2.x);
	minPoint.y = min(v1.y, v2.y);
	minPoint.z = min(v1.z, v2.z);
	maxPoint.x = max(v1.x, v2.x);
	maxPoint.y = max(v1.y, v2.y);
	maxPoint.z = max(v1.z, v2.z);
}
//------------------------------------
//
//------------------------------------
inline void Box3D::Update(const Vector3D &v)
{
	minPoint.x = min(v.x, minPoint.x);
	minPoint.y = min(v.y, minPoint.y);
	minPoint.z = min(v.z, minPoint.z);
	maxPoint.x = max(v.x, maxPoint.x);
	maxPoint.y = max(v.y, maxPoint.y);
	maxPoint.z = max(v.z, maxPoint.z);
}

//------------------------------------
//
//------------------------------------
inline void Box3D::Update(Vector3D *v, int num)
{
	while (num--)
	{
		minPoint.x = min(v->x, minPoint.x);
		minPoint.y = min(v->y, minPoint.y);
		minPoint.z = min(v->z, minPoint.z);
		maxPoint.x = max(v->x, maxPoint.x);
		maxPoint.y = max(v->y, maxPoint.y);
		maxPoint.z = max(v->z, maxPoint.z);
		++v;
	}
}

//------------------------------------
//
//------------------------------------
inline int Box3D::Dir(Plane3D &plane)
{
  if (plane.n.x * (plane.mask & Vector3D::YZ ? maxPoint.x : minPoint.x) +
       plane.n.y * (plane.mask & Vector3D::XZ ? maxPoint.y : minPoint.y) +
       plane.n.z * (plane.mask & Vector3D::XY ? maxPoint.z : minPoint.z) + plane.dist > PRECISION)
		return Plane3D::FRONT;
    if (plane.n.x * (plane.mask & Vector3D::YZ ? minPoint.x : maxPoint.x) +
        plane.n.y * (plane.mask & Vector3D::XZ ? minPoint.y : maxPoint.y) +
		plane.n.z * (plane.mask & Vector3D::XY ? minPoint.z : maxPoint.z) + plane.dist < -PRECISION)
        return Plane3D::BACK;
    return Plane3D::BOTH;
}

//------------------------------------
//
//------------------------------------
inline double Box3D::Distance(Plane3D &plane)
{
 double dist;
 if (
	((dist = plane.n.x * (plane.mask & Vector3D::YZ ? maxPoint.x : minPoint.x) +
       plane.n.y * (plane.mask & Vector3D::XZ ? maxPoint.y : minPoint.y) +
       plane.n.z * (plane.mask & Vector3D::XY ? maxPoint.z : minPoint.z) + plane.dist) > PRECISION) ||
    ((dist = plane.n.x * (plane.mask & Vector3D::YZ ? minPoint.x : maxPoint.x) +
        plane.n.y * (plane.mask & Vector3D::XZ ? minPoint.y : maxPoint.y) +
		plane.n.z * (plane.mask & Vector3D::XY ? minPoint.z : maxPoint.z) + plane.dist) < -PRECISION))
        return dist;
    return 0.0;
}
//------------------------------------
//
//------------------------------------
inline bool Box3D::PtInBox(const Vector3D &pos)
{
	return PtInBox(pos.x, pos.y, pos.z);
}
//------------------------------------
//
//------------------------------------
inline bool Box3D::PtInBox(double x, double y, double z)
{
    return x - minPoint.x > -PRECISION && maxPoint.x - x > -PRECISION  &&
           y - minPoint.y > -PRECISION && maxPoint.y - y > -PRECISION &&
           z - minPoint.z > -PRECISION && maxPoint.z - z > -PRECISION;
}
//------------------------------------
//
//------------------------------------
inline bool Box3D::PtInBox(const Vector3F &pos)
{
	return PtInBox((double)pos.x, (double)pos.y, (double)pos.z);
}
//------------------------------------
//
//------------------------------------
inline bool Box3D::Intersect(Box3D &box)
{
	return !(maxPoint.x < box.minPoint.x || minPoint.x > box.maxPoint.x ||
			maxPoint.y < box.minPoint.y || minPoint.y > box.maxPoint.y ||
			maxPoint.z < box.minPoint.z || minPoint.z > box.maxPoint.z);
}

//------------------------------------
//
//------------------------------------
inline void Box3D::Reset()
{
	maxPoint.x = LONG_MIN;
	maxPoint.y = LONG_MIN;
	maxPoint.z = LONG_MIN;
	minPoint.x = LONG_MAX;
	minPoint.y = LONG_MAX;
	minPoint.z = LONG_MAX;
}

//------------------------------------
//
//------------------------------------
inline Vector3D Box3D::operator [] (int index) const 
{
/****************************
 Y	        
/2-------3	 
/ |	    /|	 
6-------7 |	 
|  |    | | 	 
|  0----|-1 X	 
| /     |/ 	 
4-------5		 
Z     
0 minPoint.x, minPoint.y, minPoint.z
1 maxPoint.x, minPoint.y minPoint.z
2 minPoint.x, maxPoint.y minPoint.z
3 maxPoint.x, maxPoint.y minPoint.z
4 minPoint.x, minPoint.y maxPoint.z
5 maxPoint.x, minPoint.y maxPoint.z
6 minPoint.x, maxPoint.y maxPoint.z
7 maxPoint.x, maxPoint.y maxPoint.z

		 
*******************************/

    return Vector3D (index & 1 ? maxPoint.x : minPoint.x,
                      index & 2 ? maxPoint.y : minPoint.y,
                      index & 4 ? maxPoint.z : minPoint.z);
}
//------------------------------------
//
//------------------------------------
inline double Box3D::Volume() 
{
	Vector3D org = (*this)[0];
	return fabs(((*this)[4] - org) * Mul((*this)[1] - org, (*this)[2] - org));		
}
//------------------------------------
//
//------------------------------------
inline void Box3D::Plane (Plane3D::PL i, Plane3D &p)
{
	const int ind[6][3] = {
		{6, 4, 5}, // front
		{2, 0, 1}, // back
		{2, 0, 4}, // left
		{7, 5, 1}, // right
		{2, 6, 7}, // top
		{4, 0, 1}, // bottom
	};
	p.Init(operator [] (ind[i][0]), operator [] (ind[i][1]), operator [] (ind[i][2]));
	Vector3D center = (minPoint + maxPoint)/2.0;
	if (p.n.x * center.x + p.n.y * center.y + p.n.z * center.z + p.dist < -PRECISION)
		p.Flip();

}

//------------------------------------
//
//------------------------------------
inline Box3D &Box3D::operator |= (Box3D &box)
{
	minPoint.x = min(box.minPoint.x, minPoint.x);
	minPoint.y = min(box.minPoint.y, minPoint.y);
	minPoint.z = min(box.minPoint.z, minPoint.z);
	maxPoint.x = max(box.maxPoint.x, maxPoint.x);
	maxPoint.y = max(box.maxPoint.y, maxPoint.y);
	maxPoint.z = max(box.maxPoint.z, maxPoint.z);
	return *this;
}
//------------------------------------
//
//------------------------------------
inline Box3D &Box3D::operator &= (Box3D &box)
{
	if ((maxPoint.x < box.minPoint.x || minPoint.x > box.maxPoint.x ||
			maxPoint.y < box.minPoint.y || minPoint.y > box.maxPoint.y ||
			maxPoint.z < box.minPoint.z || minPoint.z > box.maxPoint.z))
			Reset();
	else
	{
		minPoint.x = max(box.minPoint.x, minPoint.x);
		minPoint.y = max(box.minPoint.y, minPoint.y);
		minPoint.z = max(box.minPoint.z, minPoint.z);
		maxPoint.x = min(box.maxPoint.x, maxPoint.x);
		maxPoint.y = min(box.maxPoint.y, maxPoint.y);
		maxPoint.z = min(box.maxPoint.z, maxPoint.z);
	}
	return *this;
}
//------------------------------------
//
//------------------------------------
inline CROSS Box3D::Cross (Box3D &box)
{
	if ((maxPoint.x < box.minPoint.x || minPoint.x > box.maxPoint.x ||
			maxPoint.y < box.minPoint.y || minPoint.y > box.maxPoint.y ||
			maxPoint.z < box.minPoint.z || minPoint.z > box.maxPoint.z))
			return EMPTY;
	Box3D t; 
	t.minPoint.x = max(box.minPoint.x, minPoint.x);
	t.minPoint.y = max(box.minPoint.y, minPoint.y);
	t.minPoint.z = max(box.minPoint.z, minPoint.z);
	t.maxPoint.x = min(box.maxPoint.x, maxPoint.x);
	t.maxPoint.y = min(box.maxPoint.y, maxPoint.y);
	t.maxPoint.z = min(box.maxPoint.z, maxPoint.z);
	return box.maxPoint != t.maxPoint || t.minPoint != box.minPoint ? INTERSECT : ENCLOSURED;

}
//------------------------------------
//
//------------------------------------
inline bool Box3D::IsCross(Vector3D &vs, Vector3D &n)
{ 
	if (!PtInBox(vs))
	{
		double t;
		Plane3D bp;
		Vector3D pos;
		for (int i = 0; i < 6; ++i)
		{
			Plane ((Plane3D::PL)i, bp);
			if (bp.Intersect(vs, n, t))
			{
				pos = vs + t * n;
				if (pos.x >= minPoint.x && pos.x <= maxPoint.x &&
				   pos.y >= minPoint.y && pos.y <= maxPoint.y &&
				   pos.z >= minPoint.z && pos.z <= maxPoint.z)
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
inline void Box3D::Inflate(Vector3D &delta)
{
	minPoint -= delta;
	maxPoint += delta;
}
//------------------------------------
//
//------------------------------------
inline void Box3D::Inflate(double delta)
{
	minPoint.x -= delta;
	minPoint.y -= delta;
	minPoint.z -= delta;
	maxPoint.x += delta;
	maxPoint.y += delta;
	maxPoint.z += delta;
}
//------------------------------------
//
//------------------------------------
inline double Box3D::Distance(Vector3D &org, Vector3D &dir)
{
	return ((*this)[dir.Orient()] - org) * dir;
}

//------------------------------------
//
//------------------------------------
inline void Box3D::Move(Vector3D &v)
{
	minPoint += v;
	maxPoint += v;
}

//------------------------------------
//
//------------------------------------
inline void Box3D::Transform(MFORM3D &m)
{
	Vector3D v[8];
	int i;
	for (i = 0; i < 8; ++i)
    {
        Vector3D vv = operator [] (i);
        v [i] = m * vv;
    }
	Reset();
	for (i = 0; i < 8; ++i)
		Update(v[i]);

}
//------------------------------------
//
//------------------------------------
inline void Box3D::Transform(MFORM3D &m, Box3D &res)
{
	res.Reset();
	for (int i = 0; i < 8; ++i)
		res.Update(m * (*this)[i]);
}

//------------------------------------
//
//------------------------------------
inline Box3D operator | (Box3D &a, Box3D &box)
{
	Box3D b;	
	b.minPoint.x = min(box.minPoint.x, a.minPoint.x);
	b.minPoint.y = min(box.minPoint.y, a.minPoint.y);
	b.minPoint.z = min(box.minPoint.z, a.minPoint.z);
	b.maxPoint.x = max(box.maxPoint.x, a.maxPoint.x);
	b.maxPoint.y = max(box.maxPoint.y, a.maxPoint.y);
	b.maxPoint.z = max(box.maxPoint.z, a.maxPoint.z);
	return b;
}
//------------------------------------
//
//------------------------------------
inline Box3D operator & (Box3D &a, Box3D &box)
{
	Box3D b;
	if (!(a.maxPoint.x < box.minPoint.x || a.minPoint.x > box.maxPoint.x ||
				a.maxPoint.y < box.minPoint.y || a.minPoint.y > box.maxPoint.y ||
				a.maxPoint.z < box.minPoint.z || a.minPoint.z > box.maxPoint.z))
	{	
		b.minPoint.x = max(box.minPoint.x, a.minPoint.x);
		b.minPoint.y = max(box.minPoint.y, a.minPoint.y);
		b.minPoint.z = max(box.minPoint.z, a.minPoint.z);
		b.maxPoint.x = min(box.maxPoint.x, a.maxPoint.x);
		b.maxPoint.y = min(box.maxPoint.y, a.maxPoint.y);
		b.maxPoint.z = min(box.maxPoint.z, a.maxPoint.z);
	}
	return b;
}
#endif
