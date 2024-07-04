///////////////////////////////////////////////////
// Sphera3D.h
// GRAPHICS Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////
#ifndef __SPHERA3D__
#define __SPHERA3D__
#pragma pack(push)
#pragma pack(8)

struct Sphera3D {
    Sphera3D() : radius(0), radiusQ(0)
    {}

    Sphera3D(Sphera3D &s) : radius(s.radius), radiusQ(s.radiusQ), center(s.center)
    {}

    Sphera3D(Vector3D &c, double r) : center(c), radius(r), radiusQ(r * r)
    {}

    Sphera3D(Box3D &b) : center(b.CenterPoint())
    {
        radius = ((b.maxPoint - b.minPoint) * 0.5).Len();
        radiusQ = radius * radius;
    }

    Box3D Box();

    BOOL operator>(const Sphera3D &a) const
    { return fabs(radius - a.radius) > PRECISION ? (radius > a.radius) : (center > a.center); }

    BOOL operator<(const Sphera3D &a) const
    { return fabs(radius - a.radius) > PRECISION ? (radius < a.radius) : (center < a.center); }

    BOOL operator==(const Sphera3D &a) const
    { return fabs(radius - a.radius) < PRECISION && center == a.center; }

    BOOL operator!=(const Sphera3D &a) const
    { return !operator==(a); }

    operator Vector3D() const
    { return center; }

    void Init(Vector3D &c, double r)
    {
        center = c;
        radius = r;
        radiusQ = r * r;
    }

    void Init(Box3D &b)
    {
        center = b.CenterPoint();
        radius = ((b.maxPoint - b.minPoint) * 0.5).Len();
        radiusQ = radius * radius;
    }

    void Init(Sphera3D &s)
    {
        radius = s.radius;
        radiusQ = s.radiusQ;
        center = s.center;
    }

    void Reset()
    { radius = radiusQ = 0; }

    void MakeBox(MFORM3D &m, Box3D &res);

    void MakeBox(Box3D &box, double r);

    void Update(Vector3D &p);

    Sphera3D &operator|=(Box3D &b);

    Sphera3D &operator|=(Sphera3D &s);

    bool PtInSphera3D(Vector3D &p);

    bool PtInSphera3D(Vector3F &p);

    CROSS Cross(Sphera3D &sh);

    bool IsCross(Vector3D &ray, Vector3D &n);

    int Dir(Plane3D &plane);

    bool IsCross(Plane3D &plane);

    bool Intersect(Vector3D &ray, Vector3D &n, double &t0, double &t1);

    bool Clip(Vector3D &org, Vector3D &dest);

    Vector3D center;
    double radius;
    double radiusQ;
};

#pragma pack(pop)

inline bool Sphera3D::Clip(Vector3D &org, Vector3D &dest)
{
    if (dest == org) return PtInSphera3D(org);
    Vector3D v = org - center;
    double f1 = v.LenQ() <= radiusQ;
    double f2 = (dest - center).LenQ() <= radiusQ;
    if (f2 && f1)
        return true;
    Vector3D n = dest - org;
    double A = n * n;
    double B = 2 * (n * v);
    double C = (v * v) - radiusQ;
    double D = B * B - 4.0 * A * C;
    A *= 2.0;
    if (D < 0.0)
        return FALSE;
    D = sqrt(D);
    double t0 = (-B - D) / A;
    double t1 = (-B + D) / A;
    if (t0 > 1.0) t0 = -1.0;
    if (t1 > 1.0) t1 = -2.0;
    if (t0 >= 0.0 || t1 >= 0.0)
    {
        if (t1 < 0.0f)
            org = org + t0 * n;
        else if (t0 < 0.0f)
            dest = org + t1 * n;
        else if (f1 != f2)
        {
            if (!f1)
                dest = org + min(t0, t1) * n;
            else
                org = org + max(t0, t1) * n;
        } else
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
inline bool Sphera3D::Intersect(Vector3D &ray, Vector3D &n, double &t0, double &t1)
{
    Vector3D v = center - ray;
    double A = n * n;
    double B = 2 * (n * v);
    double C = v * v - radiusQ;
    double D = B * B - 4.0 * A * C;
    A *= 2;
    if (D < 0.0)
        return false;
    D = sqrt(D);
    t0 = (-B - D) / A;
    t1 = (-B + D) / A;
    return t0 >= 0.0 || t1 >= 0.0;
}


//------------------------------------
//
//------------------------------------
inline bool Sphera3D::IsCross(Vector3D &ray, Vector3D &n)
{
    Vector3D d = center - ray;
    double lenQ = d * d;
    if (lenQ > radiusQ)
    {
        double _prj = d * n;
        Vector3D projection(n);
        projection *= _prj;
        d -= projection;
        return fabs(radiusQ - (d * d)) > -PRECISION;
    }
    return true;
}

//------------------------------------
//
//------------------------------------
inline Box3D Sphera3D::Box()
{
    Box3D box;
    MakeBox(box, radius);
    return box;
}

//------------------------------------
//
//------------------------------------
inline void Sphera3D::MakeBox(MFORM3D &m, Box3D &res)
{
    res.Reset();
    MakeBox(res, radius);
    res.Transform(m);
}

//------------------------------------
//
//------------------------------------
inline void Sphera3D::MakeBox(Box3D &box, double r)
{
    box.Reset();
    Vector3D p;
    p = center;
    p -= r;
    box.Update(p);
    p += r * 2;
    box.Update(p);

}

//------------------------------------
//
//------------------------------------
inline void Sphera3D::Update(Vector3D &p)
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
inline Sphera3D &Sphera3D::operator|=(Box3D &b)
{
    Sphera3D s(b);
    return operator|=(s);


}

//------------------------------------
//
//------------------------------------
inline Sphera3D &Sphera3D::operator|=(Sphera3D &s)
{
    Vector3D dc(s.center - center);
    double lc = dc.Len();
    dc *= ((lc + (s.radius - radius)) / (2 * lc));
    radius = (radius + s.radius + lc) / 2;
    center = center + dc;
    radiusQ = radius * radius;
    return *this;
}

//------------------------------------
//
//------------------------------------
inline bool Sphera3D::PtInSphera3D(Vector3D &p)
{
    return (p - center).LenQ() <= radiusQ;
}

//------------------------------------
//
//------------------------------------
inline bool Sphera3D::PtInSphera3D(Vector3F &b)
{
    double x = (double) b.x - center.x;
    double y = (double) b.y - center.y;
    double z = (double) b.z - center.z;
    return (x * x + y * y + z * z) <= radiusQ;
}

//------------------------------------
//
//------------------------------------
inline CROSS Sphera3D::Cross(Sphera3D &sh)
{
    double d = (sh.center - center).LenQ();
    if (d + sh.radiusQ <= radiusQ) return ENCLOSURED;
    if (d <= radiusQ + sh.radiusQ) return INTERSECT;
    return EMPTY;
}

//------------------------------------
//
//------------------------------------
inline int Sphera3D::Dir(Plane3D &plane)
{
    double d = plane.Distance(center);
    int dir;
    if (d > PRECISION)
        dir = Plane3D::FRONT;
    else if (d < -PRECISION)
        dir = Plane3D::BACK;
    else
        dir = Plane3D::PLANE;
    if (d <= radius && d >= -radius)
        dir |= Plane3D::INTERSECT;
    return dir;
}

//------------------------------------
//
//------------------------------------
inline bool Sphera3D::IsCross(Plane3D &plane)
{
    double d = plane.Distance(center);
    return d > radius || d < -radius;
}

#endif
