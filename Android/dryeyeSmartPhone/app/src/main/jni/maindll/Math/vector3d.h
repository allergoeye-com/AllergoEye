///////////////////////////////////////////////////
// Vector3D.h
// GRAPHICS Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////
#ifndef __VECTOR3D__
#define __VECTOR3D__
#pragma pack(push)
#pragma pack(8)

#include "vector3f.h"

struct Vector3D {
    enum AXIS {
        X, Y, Z
    };
    union {
        struct {
            double x;
            double y;
            double z;
        };
        double axis[3];
    };

    Vector3D(double ix = 0, double iy = 0, double iz = 0) : x(ix), y(iy), z(iz)
    {}

    Vector3D(float s[3]) : x(s[0]), y(s[1]), z(s[2])
    {}

    Vector3D(double s[3]) : x(s[0]), y(s[1]), z(s[2])
    {}

    Vector3D(int s[3]) : x(s[0]), y(s[1]), z(s[2])
    {}

    void Reset()
    { x = y = z = 0; }

    Vector3D(const Vector3F &p) : x(p.x), y(p.y), z(p.z)
    {}

    Vector3D(const Vector3D &p) : x(p.x), y(p.y), z(p.z)
    {}

    Vector3D(const POINT &p) : x(p.x), y(p.y), z(0)
    {}

    Vector3D(const POINTL &p) : x(p.x), y(p.y), z(0)
    {}

    void Set(double ix, double iy, double iz = 0)
    {
        x = ix;
        y = iy;
        z = iz;
    }

    Vector3D &operator=(const POINTL &p)
    {
        x = p.x;
        y = p.y;
        z = 0;
        return *this;
    }

    Vector3D &operator=(const POINT &p)
    {
        x = p.x;
        y = p.y;
        z = 0;
        return *this;
    }

    Vector3D &operator=(const Vector3F &p)
    {
        x = p.x;
        y = p.y;
        z = p.z;
        return *this;
    }

    Vector3D &operator+=(const Vector3D &p)
    {
        x += p.x, y += p.y;
        z += p.z;
        return *this;
    }

    Vector3D &operator-=(const Vector3D &p)
    {
        x -= p.x, y -= p.y;
        z -= p.z;
        return *this;
    }

    Vector3D operator-() const
    { return Vector3D(-x, -y, -z); }

    Vector3D &operator*=(double i)
    {
        x *= i, y *= i;
        z *= i;
        return *this;
    }

    Vector3D &operator-=(double i)
    {
        x -= i, y -= i;
        z -= i;
        return *this;
    }

    Vector3D &operator+=(double i)
    {
        x += i, y += i;
        z += i;
        return *this;
    }

    Vector3D &operator=(double i)
    {
        x = i, y = i;
        z = i;
        return *this;
    }

    double operator*=(const Vector3D &p)
    { return p.x * x + p.y * y + p.z * z; }

    double Cos(const Vector3D &p) const;

    operator Vector3F() const
    { return Vector3F((float) x, (float) y, (float) z); }

    operator Vector2D() const;

    Vector3D &operator/=(double i)
    {
        x /= i, y /= i;
        z /= i;
        return *this;
    }

    double &operator[](int i)
    { return i < 3 ? axis[i] : z; }

    double operator[](int i) const
    { return i < 3 ? axis[i] : z; }

    BOOL IsEqual(const Vector3D &p, double iPRECISION) const
    {
        return fabs(x - p.x) <= iPRECISION && fabs(y - p.y) <= iPRECISION &&
               fabs(z - p.z) <= iPRECISION;
    }

//	BOOL operator == (const Vector3D &p) const {return fabs(x - p.x) <= PRECISION && fabs(y - p.y) <= PRECISION && fabs(z - p.z) <= PRECISION; }
//	BOOL operator != (const Vector3D &p)  const { return !operator == (p); }
    BOOL operator==(const Vector3D &b) const
    { return x == b.x && y == b.y && z == b.z; }

    BOOL operator!=(const Vector3D &b) const
    { return x != b.x || y != b.y || z != b.z; }

    BOOL operator<=(const Vector3D &p) const
    { return operator==(p) || operator<(p); }

    BOOL operator>=(const Vector3D &p) const
    { return operator==(p) || operator>(p); }

/*
	bool operator < (const Vector3D &p) const { 
						if (fabs(x - p.x) > PRECISION)  return x < p.x;
						if (fabs(y - p.y) > PRECISION) return y < p.y;
						if (fabs(z - p.z) > PRECISION) return z < p.z;
						return false;
						};
	bool operator > (const Vector3D &p) const { 
						if (fabs(x - p.x) > PRECISION)  return x > p.x;
						if (fabs(y - p.y) > PRECISION) return y > p.y;
						if (fabs(z - p.z) > PRECISION) return z > p.z;
						return false;
						};
*/
    BOOL operator<(const Vector3D &p) const
    {
        if (x != p.x) return x < p.x;
        if (y != p.y) return y < p.y;
        if (z != p.z) return z < p.z;
        return false;
    };

    BOOL operator>(const Vector3D &p) const
    {
        if (x != p.x) return x > p.x;
        if (y != p.y) return y > p.y;
        if (z != p.z) return z > p.z;
        return false;
    };

    double Len() const
    { return sqrt(x * x + y * y + z * z); }

    double Mod() const
    { return sqrt(x * x + y * y + z * z); }

    double Mod2() const
    { return x * x + y * y + z * z; }

    double LenQ() const
    { return x * x + y * y + z * z; }

    double Sum()
    { return x + y + z; }

    Vector3D Int() const
    { return Vector3D(int(x), int(y), int(z)); }

    Vector3D Abs() const
    { return Vector3D(fabs(x), fabs(y), fabs(z)); }

    Vector3D Q() const
    { return Vector3D(x * x, y * y, z * z); }

    double Max() const
    { return max(max(x, y), z); }

    double Modul() const
    { return Len(); }

    Vector3D Ort()
    {
        double l = Len();
        return l ? Vector3D(x / l, y / l, z / l) : Vector3D(0, 0, 0);
    }

    void NormalAndLen(Vector3D &n, double &l)
    {
        l = sqrt(x * x + y * y + z * z);
        n.x = x / l;
        n.y = y / l;
        n.z = z / l;
    }

    Vector3D &Normalize()
    {
        double l = sqrt(x * x + y * y + z * z);
        if (l > PRECISION)
        {
            x /= l;
            y /= l;
            z /= l;
        }
        return *this;
    }

    Vector3D Normal(Vector3D &d);

    double Angle(Vector3D &p2);

/****************************
	Y	         
    /|-------/	 
   / |	    /|	 
  /-------/	 |	 
  | 1|  4 |  | 	 
  |  |----|-/ X	 
  | /  2  |/ 	 
  |-------|		 
  Z        		 
*******************************/
    enum ORIENT {
        XYZ = 0, YZ = 1, XZ = 2, XY = 4
    };

    int Orient() //������������ ��������������
    {
        if (x > PRECISION)
        {
            if (y > PRECISION)
                return z > PRECISION ? XYZ : XY;
            return z > PRECISION ? XZ : (XZ | XY);
        }
        if (y > PRECISION)
            return z > PRECISION ? YZ : (YZ | XY);
        return z > PRECISION ? (XZ | YZ) : (YZ | XZ | XY);
    }

    void Round()
    {
        if (fabs(z) < PRECISION)
            z = 0;
        if (fabs(y) < PRECISION)
            y = 0;
        if (fabs(x) < PRECISION)
            x = 0;
    }
};

inline double Vector3D::Cos(const Vector3D &p) const
{
    double p_l2 = p.LenQ();
    double l2 = LenQ();
    double nom = p_l2 * l2;
    if (nom != 0.0)
    {
        double c = (p.x * x + p.y * y + p.z * z) / sqrt(nom);
        double mod = fabs(c);
        if (mod > 1.0)
            c /= mod;
        return c;
    } else
        return 0.0;
}

inline void Swap(Vector3D &a, Vector3D &b)
{
    Vector3D temp;
    memcpy(&temp, &a, sizeof(Vector3D));
    memcpy(&a, &b, sizeof(Vector3D));
    memcpy(&b, &temp, sizeof(Vector3D));
}

#pragma pack(pop)

#endif
