///////////////////////////////////////////////////
// frustrum.h
// GRAPHICS Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////
#ifndef __MFRUSTRUM__
#define __MFRUSTRUM__
//#include "../include/sinclude/mtree.h"
#pragma pack(push)
#pragma pack(8)

struct MFrustrum {
public:
    MFrustrum()
    {
        iNum = 0;
        planes = 0;
    }

    MFrustrum(MFORM3D &trans, MFORM3D &prj)
    {
        iNum = 0;
        planes = 0;
        Init(trans, prj);
    }

    MFrustrum(const MFrustrum &p)
    {
        planes = new Plane3D[p.iNum];
        iNum = p.iNum;
        memcpy(planes, p.planes, sizeof(Plane3D) * iNum);
    }

    ~MFrustrum()
    { Reset(); }

    bool IsValid()
    { return planes != 0; }

    void Reset()
    {
        if (planes) delete[] planes;
        iNum = 0;
        planes = 0;
    }

    void Init(MFORM3D &trans, MFORM3D &prj)
    {
        MFORM3D m = prj * trans;
        Init(m);
    }

    MFrustrum &operator=(const MFrustrum &p)
    {
        Reset();
        planes = new Plane3D[p.iNum];
        iNum = p.iNum;
        memcpy(planes, p.planes, sizeof(Plane3D) * iNum);
        return *this;
    }

    void Init(MFORM3D &m);

    void Init(Box3D &box);

    bool PtIn(Vector3D &v);

    bool PtIn(Vector3F &v);

    CROSS Cross(Box3D &box);

    CROSS Cross(Sphera3D &s);

    bool IsCross(Box3D &box);

    CROSS Cross(Box3D &box, MArray<Plane3D *> &res);

    bool ClipLine(Vector3D &v1, Vector3D &v2);

protected:
    bool ClipPoint(Vector3D &v1, Vector3D &v2);

    void InitPlane(Plane3D &plane, Vector3D n, double d);

public:

    Plane3D *planes;
    int iNum;

};

#pragma pack(pop)

//------------------------------------
//
//------------------------------------
inline void MFrustrum::Init(MFORM3D &m)
{
    Reset();
    iNum = 6;
    planes = new Plane3D[iNum];
    InitPlane(planes[Plane3D::PL_FRONT],
              Vector3D(m.arM[3] + m.arM[2], m.arM[7] + m.arM[6], m.arM[11] + m.arM[10]),
              m.arM[15] + m.arM[14]);
    InitPlane(planes[Plane3D::PL_FAR],
              Vector3D(m.arM[3] - m.arM[2], m.arM[7] - m.arM[6], m.arM[11] - m.arM[10]),
              m.arM[15] - m.arM[14]);
    InitPlane(planes[Plane3D::PL_LEFT],
              Vector3D(m.arM[3] + m.arM[0], m.arM[7] + m.arM[4], m.arM[11] + m.arM[8]),
              m.arM[15] + m.arM[12]);
    InitPlane(planes[Plane3D::PL_RIGHT],
              Vector3D(m.arM[3] - m.arM[0], m.arM[7] - m.arM[4], m.arM[11] - m.arM[8]),
              m.arM[15] - m.arM[12]);
    InitPlane(planes[Plane3D::PL_TOP],
              Vector3D(m.arM[3] - m.arM[1], m.arM[7] - m.arM[5], m.arM[11] - m.arM[9]),
              m.arM[15] - m.arM[13]);
    InitPlane(planes[Plane3D::PL_BOTTOM],
              Vector3D(m.arM[3] + m.arM[1], m.arM[7] + m.arM[5], m.arM[11] + m.arM[9]),
              m.arM[15] + m.arM[13]);

}

//------------------------------------
//
//------------------------------------
inline void MFrustrum::Init(Box3D &box)
{
    Reset();
    iNum = 6;
    planes = new Plane3D[iNum];
    box.Plane(Plane3D::PL_FRONT, planes[Plane3D::PL_FRONT]);
    box.Plane(Plane3D::PL_FAR, planes[Plane3D::PL_FAR]);
    box.Plane(Plane3D::PL_LEFT, planes[Plane3D::PL_LEFT]);
    box.Plane(Plane3D::PL_RIGHT, planes[Plane3D::PL_RIGHT]);
    box.Plane(Plane3D::PL_TOP, planes[Plane3D::PL_TOP]);
    box.Plane(Plane3D::PL_BOTTOM, planes[Plane3D::PL_BOTTOM]);
    Vector3D v = box.CenterPoint();
    for (int i = 0; i < iNum; ++i)
    {
        if (planes[i].n.x * v.x + planes[i].n.y * v.y + planes[i].n.z * v.z + planes[i].dist <
            -PRECISION)
            planes[i].Flip();
    }

}

//------------------------------------
//
//------------------------------------
inline void MFrustrum::InitPlane(Plane3D &plane, Vector3D n, double d)
{
    d /= n.Len();
    n.Normalize();
    plane.Init(n, d);

}

//------------------------------------
//
//------------------------------------
inline bool MFrustrum::PtIn(Vector3D &v)
{
    for (int i = 0; i < iNum; ++i)
        if (planes[i].n.x * v.x + planes[i].n.y * v.y + planes[i].n.z * v.z + planes[i].dist <
            -PRECISION)
            return false;
    return true;
}

//------------------------------------
//
//------------------------------------
inline bool MFrustrum::PtIn(Vector3F &v)
{
    for (int i = 0; i < iNum; ++i)
        if (planes[i].n.x * v.x + planes[i].n.y * v.y + planes[i].n.z * v.z + planes[i].dist <
            -PRECISION_F)
            return false;
    return true;
}


//------------------------------------
//
//------------------------------------
inline bool MFrustrum::ClipPoint(Vector3D &v1, Vector3D &v2)
{
    Vector3D dir;
    bool fRet = true;
    double numer, d, t;
    for (int i = 0; i < iNum; ++i)
    {
        numer = -(planes[i].n.x * v1.x + planes[i].n.y * v1.y + planes[i].n.z * v1.z +
                  planes[i].dist);
        if (numer > PRECISION)
        {
            dir = v2 - v1;
            d = dir * planes[i].n;
            if (fabs(d) > PRECISION)
            {
                t = numer / d;
                if (t <= 1.0 && t >= 0.0)
                {
                    dir = v1 + t * dir;
                    if (PtIn(dir))
                    {
                        v1 = dir;
                        return true;
                    }
                }
            }
            fRet = false;
        }
    }
    return fRet;
}

inline bool MFrustrum::ClipLine(Vector3D &v1, Vector3D &v2)
{
    return ClipPoint(v1, v2) && ClipPoint(v2, v1);
}

//------------------------------------
//
//------------------------------------
inline CROSS MFrustrum::Cross(Box3D &box)
{
    BYTE a, test = 0;
    for (int i = 0; i < iNum; ++i)
    {
        if ((a = box.Dir(planes[i])) == Plane3D::BACK)
            return EMPTY;
        test += a == Plane3D::FRONT;
    }
    return test == iNum ? ENCLOSURED : INTERSECT;
}

//------------------------------------
//
//------------------------------------
inline CROSS MFrustrum::Cross(Sphera3D &s)
{
    BYTE a, test = 0;
    for (int i = 0; i < iNum; ++i)
    {
        if ((a = s.Dir(planes[i])) & Plane3D::BACK)
            return EMPTY;
        test += (a & Plane3D::FRONT) != 0;
    }
    return test == iNum ? ENCLOSURED : INTERSECT;
}

//------------------------------------
//
//------------------------------------
inline bool MFrustrum::IsCross(Box3D &box)
{
    for (int i = 0; i < iNum; ++i)
    {
        if (planes[i].n.x * (planes[i].mask & Vector3D::YZ ? box.minPoint.x : box.maxPoint.x) +
            planes[i].n.y * (planes[i].mask & Vector3D::XZ ? box.minPoint.y : box.maxPoint.y) +
            planes[i].n.z * (planes[i].mask & Vector3D::XY ? box.minPoint.z : box.maxPoint.z) <
            -PRECISION)
            return false;
    }
    return true;

}

//------------------------------------
//
//------------------------------------
inline CROSS MFrustrum::Cross(Box3D &box, MArray<Plane3D *> &res)
{
    BYTE a, test = 0;
    for (int i = 0; i < iNum; ++i)
    {
        if ((a = box.Dir(planes[i])) == Plane3D::BACK)
            return EMPTY;
        if (a == Plane3D::FRONT)
            ++test;
        else
            res.Add() = planes + i;
    }
    return test == iNum ? ENCLOSURED : INTERSECT;
}

#endif
