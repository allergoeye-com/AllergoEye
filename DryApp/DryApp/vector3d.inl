///////////////////////////////////////////////////
// Vector3D.h
// GRAPHICS Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////
inline Vector3D::operator Vector2D () const { return Vector2D(x, y); }

//------------------------------------
//
//------------------------------------
inline Vector3D Normal(const Vector3D &a, const Vector3D &b, const Vector3D &org) 
{  
	return Vector3D((a.y - org.y) * (b.z - org.z) - (b.y - org.y) * (a.z - org.z),
		(a.z - org.z) * (b.x - org.x) - (b.z - org.z) * (a.x - org.x),
		(a.x - org.x) * (b.y - org.y) - (b.x - org.x) * (a.y - org.y)); 
}
//------------------------------------
//
//------------------------------------
inline Vector3D Mul (const Vector3D &u, const Vector3D &v) { return Vector3D (u.y * v.z - u.z * v.y, u.z * v.x-u.x * v.z, u.x * v.y - u.y*v.x); }
inline void Mul (const Vector3D &u, const Vector3D &v, Vector3D &res) { res.Set(u.y * v.z - u.z * v.y, u.z * v.x-u.x * v.z, u.x * v.y - u.y*v.x); }

//------------------------------------
//
//------------------------------------
inline double operator * (const Vector3D &p,  const Vector3D &p1)  { return p.x * p1.x + p.y * p1.y + p.z * p1.z; } 
//------------------------------------
//
//------------------------------------
inline Vector3D operator + (const Vector3D &p, const Vector3D &p1)  { return Vector3D(p.x + p1.x, p.y + p1.y, p.z + p1.z); } 

//------------------------------------
//
//------------------------------------
inline Vector3D operator / (const Vector3D &p, const Vector3D &p1)  { return Vector3D(fabs(p1.x) > PRECISION ? p.x/p1.x : 0, fabs(p1.y) > PRECISION ? p.y/p1.y : 0.0, fabs(p1.z) > PRECISION ? p.z/p1.z : 0); } 
//------------------------------------
//
//------------------------------------
inline Vector3D operator + (const Vector3D &p, double d)  { return Vector3D(p.x + d, p.y + d, p.z + d); } 

//------------------------------------
//
//------------------------------------
inline Vector3D operator - (const Vector3D &p, const Vector3D &p1) { return Vector3D(p.x - p1.x, p.y - p1.y, p.z - p1.z); }
//------------------------------------
//
//------------------------------------
inline Vector3D operator - (const Vector3D &p, double d) { return Vector3D(p.x - d, p.y - d, p.z - d); }

//------------------------------------
//
//------------------------------------
inline Vector3D operator * (double i, Vector3D &p){ return Vector3D(p.x * i, p.y * i, p.z * i); }
//------------------------------------
//
//------------------------------------
inline Vector3D operator * (const Vector3D &p, double i) { return Vector3D(p.x * i, p.y * i, p.z * i); }
//------------------------------------
//
//------------------------------------
inline Vector3D operator / (const Vector3D &p, double i)  { return Vector3D(p.x / i, p.y / i, p.z / i); }
//------------------------------------
//
//------------------------------------
inline Vector3D Vector3D::Normal(Vector3D &d) { Vector3D v(0, 0, 0); return ::Normal(*this, d, v); }   
//------------------------------------
//
//------------------------------------
inline double Vector3D::Angle(Vector3D &p2) {  return acos(Ort() * p2.Ort()) * 180.0 / PI; }
//------------------------------------
//
//------------------------------------
inline double Square(const Vector3D &a, const Vector3D &b, const Vector3D &c)
{
    return Mul(c - a, b - a).Len();
}
inline double Distance(const Vector3D &a, const Vector3D &v1, const Vector3D &v2)
{
	Vector3D v = v2 - v1;
	Vector3D w = a - v1;
	double r1, r2;
	if ((r1 = w * v) < PRECISION)
		return (a - v1).Len();
	if ( (r2 = v * v) - r1 < PRECISION)
		return (a - v2).Len();
	r1 /= r2;
	return (a - (v1 + (r1 * v))).Len();
}
