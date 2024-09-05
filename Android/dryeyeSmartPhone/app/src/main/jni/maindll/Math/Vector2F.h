#ifndef __VECTOR2F__
#define __VECTOR2F__
#pragma pack(push)
#pragma pack(4)
#ifndef PRECISION_F
#define PRECISION_F 1.0e-4f
#endif
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif
#ifndef SGN
#define SGN(x)  ( (x) >= 0 ? 1 : -1)
#endif

struct Vector2F {
    enum ORIENT {
        LEFT, RIGHT, BEYOND/*do*/, BEHIND/*posle*/, BETWEEN/*mezhdu*/, ORG, DEST
    };
    union {
        struct {
            float x;
            float y;
        };
        float axis[2];
    };

    Vector2F(float ix = 0, float iy = 0) : x(ix), y(iy)
    {}

    Vector2F(int ix, int iy) : x((float) ix), y((float) iy)
    {}

    Vector2F(const Vector2F &p) : x(p.x), y(p.y)
    {}

    Vector2F(const POINT &p) : x((float) p.x), y((float) p.y)
    {}
    
    void Set(float ix, float iy)
    {
        x = ix;
        y = iy;
    }

    void Set(int ix, int iy)
    {
        x = (float) ix;
        y = (float) iy;
    }

    Vector2F &operator=(const POINT &p)
    {
        x = (float) p.x;
        y = (float) p.y;
        return *this;
    }

    Vector2F &operator=(const CPoint &p)
    {
        x = (float) p.x;
        y = (float) p.y;
        return *this;
    }

    Vector2F &operator+=(const Vector2F &p)
    {
        x += p.x, y += p.y;
        return *this;
    }

    Vector2F &operator-=(const Vector2F &p)
    {
        x -= p.x, y -= p.y;
        return *this;
    }

    Vector2F &operator/=(float d)
    {
        x /= d, y /= d;
        return *this;
    }

    Vector2F &operator*=(float d)
    {
        x *= d, y *= d;
        return *this;
    }

    Vector2F operator*(float d)
    {
        Vector2F v(*this);
        v *= d;
        return v;
    }

    Vector2F operator/(float d)
    {
        Vector2F v(*this);
        v /= d;
        return v;
    }

    Vector2F &WeightedSum(const Vector2F &a, float wa, const Vector2F &b, float wb)
    {
        float s = wa + wb;
        x = (a.x * wa + b.x * wb) / s;
        y = (a.y * wa + b.y * wb) / s;
        return *this;
    }

    Vector2F &operator/=(int d)
    {
        x /= (float) d, y /= (float) d;
        return *this;
    }

    Vector2F &operator*=(int d)
    {
        x *= (float) d, y *= (float) d;
        return *this;
    }

    Vector2F operator*(int d)
    {
        Vector2F v(x *= (float) d, y * (float) d);
        return v;
    }

    Vector2F operator/(int d)
    {
        Vector2F v(x / (float) d, y / (float) d);
        return v;
    }

    Vector2F operator/(double d)
    {
        Vector2F v(x / (float) d, y / (float) d);
        return v;
    }

    void Shift(float dx, float dy)
    {
        x += dx;
        y += dy;
    }

    float operator[](int i) const
    { return !i ? x : y; }

    float &operator[](int i)
    { return !i ? x : y; }

    BOOL operator==(const Vector2F &p) const
    { return fabs(x - p.x) <= PRECISION_F && fabs(y - p.y) <= PRECISION_F; }

    BOOL operator!=(const Vector2F &p) const
    { return !operator==(p); }

    operator CPoint() const
    { return CPoint((int) (x + 0.5), (int) (y + 0.5)); }

    operator POINTL() const
    {
        POINTL t;
        t.x = (int) (x + 0.5);
        t.y = (int) (y + 0.5);
        return t;
    }

    operator POINT() const
    {
        POINT t;
        t.x = (int) (x + 0.5);
        t.y = (int) (y + 0.5);
        return t;
    }

    float Len()
    { return (float) sqrt(x * x + y * y); }

    float Norm()
    { return (float) sqrt(x * x + y * y); }

    float LenQ()
    { return x * x + y * y; }

    float Modul()
    { return Len(); }

    Vector2F Ort()
    {
        float l = Len();
        return l ? Vector2F(x / l, y / l) : Vector2F(0, 0);
    }

    Vector2F Perp()
    { return Vector2F(-y, x); }

    void Normalize()
    {
        float l = Len();
        if (l > 0.0)
        {
            x /= l;
            y /= l;
        }
    }

    float PolarAngle() const;

    float Angle(const Vector2F &p2) const;

    ORIENT Orient(Vector2F &org, Vector2F &dest);

/*
	bool operator < (const Vector2F &p) const { if (fabs(x - p.x) > PRECISION_F)  return x < p.x;
													if (fabs(y - p.y) > PRECISION_F) return y < p.y;
													return false;
												}
	
	bool operator > (const Vector2F &p) const { if (fabs(x - p.x) > PRECISION_F)  return x > p.x;
													if (fabs(y - p.y) > PRECISION_F) return y > p.y;
													return false;
												}
*/
    BOOL operator<(const Vector2F &p) const
    {
        if (fabs(x - p.x) > PRECISION_F)
            return x < p.x;
        else if (fabs(y - p.y) > PRECISION_F)
            return y < p.y;
        return false;
    };

    BOOL operator>(const Vector2F &p) const
    {
        if (fabs(x - p.x) > PRECISION_F)
            return x > p.x;
        if (fabs(y - p.y) > PRECISION_F)
            return y > p.y;
        return false;
    };

};

#pragma pack(pop)

//------------------------------------
//
//------------------------------------
inline float operator*(const Vector2F &p, const Vector2F &p1)
{ return p.x * p1.x + p.y * p1.y; }

//------------------------------------
//
//------------------------------------
inline Vector2F operator+(const Vector2F &p, const Vector2F &p1)
{ return Vector2F(p.x + p1.x, p.y + p1.y); }

//------------------------------------
//
//------------------------------------
inline Vector2F operator-(const Vector2F &p, const Vector2F &p1)
{ return Vector2F(p.x - p1.x, p.y - p1.y); }

//------------------------------------
//
//------------------------------------
inline Vector2F operator*(float i, const Vector2F &p)
{ return Vector2F(p.x * i, p.y * i); }

//------------------------------------
//
//------------------------------------
inline Vector2F operator*(int i, const Vector2F &p)
{ return Vector2F(p.x * (float) i, p.y * (float) i); }

inline Vector2F operator/(int i, const Vector2F &p)
{ return Vector2F(p.x / (float) i, p.y / (float) i); }

inline Vector2F operator+(int i, const Vector2F &p)
{ return Vector2F(p.x + (float) i, p.y + (float) i); }

inline Vector2F operator-(int i, const Vector2F &p)
{ return Vector2F(p.x - (float) i, p.y - (float) i); }

//------------------------------------
//
//------------------------------------
inline float Vector2F::Angle(const Vector2F &p2) const
{
    float a = PolarAngle() - p2.PolarAngle();
    if (a < 0) a += 360.0f;
    if (a > 180) a -= 360.0f;
    return a;
}

//------------------------------------
//
//------------------------------------
inline float Vector2F::PolarAngle() const
{
    float a1 = x ? (float) atan2((float) y, (float) x) : (float) (y > 0 ? PI / 2.0 : -PI / 2.0);
    return (float) (a1 * 180.0 / PI);
}

//------------------------------------
//
//------------------------------------
inline float PolarAngle(const Vector2F &p1, const Vector2F &p2, const Vector2F &p3)
{
    return (p2 - p1).Angle(p3 - p1);
}

//------------------------------------
//
//------------------------------------
inline float
PolarAngle(const Vector2F &p1, const Vector2F &p2, const Vector2F &p3, const Vector2F &p4)
{
    return (p2 - p1).Angle(p4 - p3);
}

//------------------------------------
//
//------------------------------------
inline int direct(Vector2F &p0, Vector2F &p1, Vector2F &p2)
{
    Vector2F a = p1 - p0;
    Vector2F b = p2 - p0;
    float sa = a.x * b.y - b.x * a.y;
    if (sa > PRECISION_F)
        return 1;
    if (sa < -PRECISION_F)
        return -1;
    return 0;
}

//------------------------------------
//
//------------------------------------
inline Vector2F::ORIENT Vector2F::Orient(Vector2F &org, Vector2F &dest)
{
    if (org == *this)
        return ORG;
    if (dest == *this)
        return DEST;
    Vector2F a = dest - org;
    Vector2F b = *this - org;
    float sa = a.x * b.x;
    float sb = b.y * a.y;
    float sab = a.x * b.y - b.x * a.y;
    if (sab > PRECISION_F)
        return LEFT;
    if (sab < -PRECISION_F)
        return RIGHT;

    if (sa < -PRECISION_F || sb < -PRECISION_F)
        return BEHIND;
    if (a.x * a.x + a.y * a.y < b.x * b.x + b.y * b.y)
        return BEYOND;
    return BETWEEN;

}

inline void Swap(Vector2F &a, Vector2F &b)
{
    Vector2F temp;
    memcpy(&temp, &a, sizeof(Vector2F));
    memcpy(&a, &b, sizeof(Vector2F));
    memcpy(&b, &temp, sizeof(Vector2F));
}

inline int __cdecl CmpVector2F(const void *a, const void *b)
{
    Vector2F *pa = (Vector2F *) a;
    Vector2F *pb = (Vector2F *) b;
    if (*pa > *pb) return 1;
    if (*pa < *pb) return -1;
    return 0;
}

#endif
