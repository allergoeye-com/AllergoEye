///////////////////////////////////////////////////
// Plane3D.h
// GRAPHICS Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////
#ifndef __PLANE3D__
#define __PLANE3D__
#pragma pack(push)
#pragma pack(8)

struct Plane3D {
    enum PL {
        PL_FRONT, PL_FAR, PL_LEFT, PL_RIGHT, PL_TOP, PL_BOTTOM
    };
    enum {
        PLANE, FRONT = 1, BACK = 2, BOTH = 4, INTERSECT = 8
    };

    Plane3D()
    { Init(Vector3D(0, 0, 1), 1); };

    Plane3D(const Plane3D &p)
    {
        n = p.n;
        dist = p.dist;
        mask = p.mask;
    }

    Plane3D(const Vector3D v, double d)
    { Init(v, d); }

    Plane3D(const Vector3D &normal, const Vector3D &v)
    { Init(normal, v); }

    Plane3D(const Vector3D &p1, const Vector3D &p2, const Vector3D &p3)
    { Init(p1, p2, p3); }

    bool operator<(const Plane3D &a) const
    {
        return (operator!=(a)) && (n < a.n || (n == a.n && dist < a.dist));
    }

    bool operator>(const Plane3D &a) const
    { return (operator!=(a)) && (n > a.n || (n == a.n && dist > a.dist)); }

    bool operator==(const Plane3D &a) const
    {
        double b = (a.n * n) - 1;
        return b >= 0 && b < PRECISION && (dist == a.dist ||
                                           (fabs(n.x) > PRECISION &&
                                            a.n.x / n.x == a.dist / dist) ||
                                           (fabs(n.y) > PRECISION &&
                                            a.n.y / n.y == a.dist / dist) ||
                                           (fabs(n.y) > PRECISION && a.n.z / n.z == a.dist / dist));
    }

    bool operator!=(const Plane3D &a) const
    { return !(operator==(a)); }

    void Init(Plane3D &d)
    {
        n = d.n;
        dist = d.dist;
        mask = d.mask;
    }

    void Init(const Vector3D v, double d);

    void Init(const Vector3D &normal, const Vector3D &v);

    void Init(const Vector3D &p1, const Vector3D &p2, const Vector3D &p3);

    void MoveTo(const Vector3D &v);

    bool IsIntersect(Plane3D &p)
    {
        double b = fabs((p.n * n)) - 1;
        return b < -PRECISION || b > PRECISION;
    }

    double Distance(Vector3D &v)
    { return v.x * n.x + v.y * n.y + v.z * n.z + dist; }

    Vector3D Point()
    { return n * -dist; }

    int Dir(const Vector3D &v);

    int Dir(const Vector3F &v);

    void Flip();

    double PerpPoint(Vector3D &p, Vector3D &res);

    bool Intersect(Vector3D &org, Vector3D &dir, double &t);

    bool Intersect(Line3D &line, Vector3D &res);

    bool Intersect(Vector3D &org, Vector3D &dir, Vector3D &res);

    Vector3D n;
    double dist;
    int mask;
};

#pragma pack(pop)

//-----------------------------------------------
//
//-----------------------------------------------
inline void Plane3D::Init(const Vector3D v, double d)
{
    n = v;
    mask = n.Orient();
    n.Normalize();
    dist = d;

}

//-----------------------------------------------
//
//-----------------------------------------------
inline void Plane3D::Init(const Vector3D &normal, const Vector3D &v)
{
    n = normal;
    n.Normalize();
    dist = -v.x * n.x - v.y * n.y - v.z * n.z;
    mask = n.Orient();

}

//-----------------------------------------------
//
//-----------------------------------------------
inline void Plane3D::Init(const Vector3D &p1, const Vector3D &p2, const Vector3D &p3)
{
    n = Mul((p2 - p1), (p3 - p1));
    n.Normalize();
    dist = -p1.x * n.x - p1.y * n.y - p1.z * n.z;
    mask = n.Orient();

}

inline void Plane3D::MoveTo(const Vector3D &v)
{
    dist = -(n.x * v.x + n.y * v.y + n.z * v.z);
}

//-----------------------------------------------
//
//-----------------------------------------------
inline int Plane3D::Dir(const Vector3D &v)
{
    double d = n.x * v.x + n.y * v.y + n.z * v.z + dist;
    if (d > PRECISION)
        return FRONT;
    else if (d < -PRECISION)
        return BACK;

    return PLANE;
}

//-----------------------------------------------
//
//-----------------------------------------------
inline int Plane3D::Dir(const Vector3F &v)
{
    double d = n.x * v.x + n.y * v.y + n.z * v.z + dist;
    if (d > PRECISION_F)
        return FRONT;
    else if (d < -PRECISION_F)
        return BACK;

    return PLANE;
}

//-----------------------------------------------
//
//-----------------------------------------------
inline void Plane3D::Flip()
{
    n = -n;
    dist = -dist;
    mask = n.Orient();
}

//-----------------------------------------------
//
//-----------------------------------------------
inline double Plane3D::PerpPoint(Vector3D &p, Vector3D &res)
{
    double d = -dist - (p * n);
    res = p + (d * n);
    return d;
}

//-----------------------------------------------
//
//-----------------------------------------------
inline bool Plane3D::Intersect(Vector3D &org, Vector3D &dir, double &t)
{
    double numer = -(dist + (org * n));
    double denom = dir * n;
    if (fabs(denom) < PRECISION)
        return false;
    t = numer / denom;

    return true;
}

inline bool Plane3D::Intersect(Vector3D &org, Vector3D &dir, Vector3D &res)
{
    double t;
    if (Intersect(org, dir, t))
    {
        res = org + t * dir;
        return true;
    }
    return false;
}

//-----------------------------------------------
//
//-----------------------------------------------
inline bool Plane3D::Intersect(Line3D &line, Vector3D &res)
{

    double numer = -(dist + (line.org * n));
    Vector3D dir = line.Dir();
    double d = dir * n;
    if (fabs(d) > PRECISION)
    {
        double t = numer / d;
        if (t - 1.0 < PRECISION && t > -PRECISION)
        {
            res = line.org + t * dir;
            return 1;
        }
    }
    return false;

}

//-----------------------------------------------
//
//-----------------------------------------------
inline bool Intersect(Plane3D &p1, Plane3D &p2, Plane3D &p3, Vector3D &res)
{
    //double det = Mul(p1.n , p2.n) * p3.n;
    MFORM2D f;
    f.SetRows(p1.n, p2.n, p3.n);
    double det = f.Det();
    if (fabs(det) < PRECISION) return false;
    res.Set(f.Det1(-p1.dist, -p2.dist, -p3.dist) / det,
            f.Det2(-p1.dist, -p2.dist, -p3.dist) / det,
            f.Det3(-p1.dist, -p2.dist, -p3.dist) / det);

    return true;
}
//======================================================
//
//======================================================
#pragma pack(push)
#pragma pack(8)

struct Plane3F {
    enum PL {
        PL_FRONT, PL_FAR, PL_LEFT, PL_RIGHT, PL_TOP, PL_BOTTOM
    };
    enum {
        PLANE, FRONT = 1, BACK = 2, BOTH = 4, INTERSECT = 8
    };

    Plane3F()
    { Init(Vector3F(0, 0, 1), 1); };

    Plane3F(const Plane3F &p)
    {
        n = p.n;
        dist = p.dist;
        mask = p.mask;
    }

    Plane3F(const Vector3F v, float d)
    { Init(v, d); }

    Plane3F(const Vector3F &normal, const Vector3F &v)
    { Init(normal, v); }

    Plane3F(const Vector3F &p1, const Vector3F &p2, const Vector3F &p3)
    { Init(p1, p2, p3); }

    bool operator<(const Plane3F &a) const
    {
        return (operator!=(a)) && (n < a.n || (n == a.n && dist < a.dist));
    }

    bool operator>(const Plane3F &a) const
    {
        return (operator!=(a)) && (n > a.n || (n == a.n && dist > a.dist));
    }

    bool operator==(const Plane3F &a) const
    {
        float b = (a.n * n) - 1;
        return b >= 0 && b < PRECISION_F && (dist == a.dist ||
                                             (fabs(n.x) > PRECISION_F &&
                                              a.n.x / n.x == a.dist / dist) ||
                                             (fabs(n.y) > PRECISION_F &&
                                              a.n.y / n.y == a.dist / dist) ||
                                             (fabs(n.y) > PRECISION_F &&
                                              a.n.z / n.z == a.dist / dist));
    }

    bool operator!=(const Plane3F &a) const
    { return !(operator==(a)); }

    void Init(Plane3F &d)
    {
        n = d.n;
        dist = d.dist;
        mask = d.mask;
    }

    void Init(const Vector3F v, float d);

    void Init(const Vector3F &normal, const Vector3F &v);

    void Init(const Vector3F &p1, const Vector3F &p2, const Vector3F &p3);

    void MoveTo(const Vector3F &v);

    bool IsIntersect(Plane3F &p)
    {
        float b = (float) fabs((p.n * n)) - 1;
        return b < -PRECISION_F || b > PRECISION_F;
    }

    float Distance(Vector3F &v)
    { return v.x * n.x + v.y * n.y + v.z * n.z + dist; }

    Vector3F Point()
    { return n * -dist; }

    int Dir(const Vector3F &v);

    void Flip();

    float PerpPoint(Vector3F &p, Vector3F &res);

    bool Intersect(Vector3F &org, Vector3F &dir, float &t);

    bool Intersect(Vector3F &org, Vector3F &dir, Vector3F &res);

    Vector3F n;
    float dist;
    int mask;
};

#pragma pack(pop)

//-----------------------------------------------
//
//-----------------------------------------------
inline void Plane3F::Init(const Vector3F v, float d)
{
    n = v;
    mask = n.Orient();
    n.Normalize();
    dist = d;

}

//-----------------------------------------------
//
//-----------------------------------------------
inline void Plane3F::Init(const Vector3F &normal, const Vector3F &v)
{
    n = normal;
    n.Normalize();
    dist = -v.x * n.x - v.y * n.y - v.z * n.z;
    mask = n.Orient();

}

//-----------------------------------------------
//
//-----------------------------------------------
inline void Plane3F::Init(const Vector3F &p1, const Vector3F &p2, const Vector3F &p3)
{
    n = Mul((p2 - p1), (p3 - p1));
    n.Normalize();
    dist = -p1.x * n.x - p1.y * n.y - p1.z * n.z;
    mask = n.Orient();

}

inline void Plane3F::MoveTo(const Vector3F &v)
{
    dist = -(n.x * v.x + n.y * v.y + n.z * v.z);
}

//-----------------------------------------------
//
//-----------------------------------------------
inline int Plane3F::Dir(const Vector3F &v)
{
    float d = n.x * v.x + n.y * v.y + n.z * v.z + dist;
    if (d > PRECISION_F)
        return Plane3D::FRONT;
    else if (d < -PRECISION_F)
        return Plane3D::BACK;

    return Plane3D::PLANE;
}

//-----------------------------------------------
//
//-----------------------------------------------
inline void Plane3F::Flip()
{
    n = -n;
    dist = -dist;
    mask = n.Orient();
}

//-----------------------------------------------
//
//-----------------------------------------------
inline float Plane3F::PerpPoint(Vector3F &p, Vector3F &res)
{
    float d = -dist - (p * n);
    res = p + (d * n);
    return d;
}

//-----------------------------------------------
//
//-----------------------------------------------
inline bool Plane3F::Intersect(Vector3F &org, Vector3F &dir, float &t)
{
    float numer = -(dist + (org * n));
    float denom = dir * n;
    if (fabs(denom) < PRECISION_F)
        return false;
    t = numer / denom;

    return true;
}

inline bool Plane3F::Intersect(Vector3F &org, Vector3F &dir, Vector3F &res)
{
    float t;
    if (Intersect(org, dir, t))
    {
        res = org + t * dir;
        RoundTo(res, 100.0f);
        return true;
    }
    return false;
}


#endif
