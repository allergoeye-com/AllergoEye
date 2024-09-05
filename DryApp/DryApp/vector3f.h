#ifndef __Vector3F__
#define __Vector3F__
#pragma pack(push)
#pragma pack(4)

struct Vector3FC;

struct Vector3F {
	enum AXIS { X, Y, Z };
	union {
		struct {
			float x;
			float y;
			float z;
		};
		struct {
			float r;
			float g;
			float b;
		};
		float axis[3];
	};

	Vector3F(float ix = 0, float iy = 0, float iz = 0):x(ix), y(iy),z(iz) { }
	Vector3F(float s[3]):x(s[0]), y(s[1]),z(s[2]) { }
	void Reset() { x = y = z = 0; }
	Vector3F(const Vector3F &p) : x(p.x), y(p.y), z(p.z) {}
	operator Vector3FC() const;

	void Set(float ix, float iy, float iz = 0) { x = ix; y = iy; z = iz; }
	void Set(int ix, int iy, int iz = 0) { x = (float)ix; y = (float)iy; z = (float)iz; }
	#ifdef _VER_PLK_64
		void Set(DINT ix, DINT iy, DINT iz = 0) { x = (float)ix; y = (float)iy; z = (float)iz; }
	#endif
	void Set(size_t ix, size_t iy, size_t iz = 0) { x = (float)ix; y = (float)iy; z = (float)iz; }
	Vector3F &operator += (const Vector3F &p) { x += p.x, y += p.y; z += p.z; return *this; } 
	Vector3F &operator -= (const Vector3F &p) { x -= p.x, y -= p.y; z -= p.z;  return *this; }
	Vector3F operator - () const { return Vector3F (-x, -y, -z); }	
	Vector3F &operator *= (float c) { x *= c, y *= c; z *= c; return *this; }
	float Cos(const Vector3F &p) const;
	Vector3F &operator /= (float i) { x /= i, y /= i; z /= i; return *this; }
	Vector3F &operator /= (int i) { x /= (float)i, y /= (float)i; z /= (float)i; return *this; }
	Vector3F &operator /= (double i) { x /= (float)i, y /= (float)i; z /= (float)i; return *this; }
	Vector3F &operator -= (float i) { x -= i, y -= i; z -= i; return *this; }
	Vector3F &operator += (float i) { x += i, y += i; z += i; return *this; }
	Vector3F &operator = (float i) { x = i, y = i; z = i; return *this; }
	Vector3F &operator = (const Vector3F &c) { x = c.x, y = c.y; z = c.z; return *this; }
	
	float &operator [] (int i) { return i < 3 ? axis[i] : z; }
	float operator [] (int i) const { return i < 3 ? axis[i] : z; }
	BOOL PreciseEqual(const Vector3F &p) { return x == p.x && y == p.y && z == p.z; }
	BOOL operator == (const Vector3F &p) const {return fabs(x - p.x) <= PRECISION_F && fabs(y - p.y) <= PRECISION_F && fabs(z - p.z) <= PRECISION_F; }
	bool operator <= (const Vector3F &p) const {  return operator ==(p) || operator < (p); }
	bool operator >= (const Vector3F &p) const {  return operator ==(p) || operator > (p); }
	bool operator < (const Vector3F &p) const { 
						if (fabs(x - p.x) > PRECISION_F) 
							return x < p.x;
						else
						if (fabs(y - p.y) > PRECISION_F) 
							return y < p.y;
						else
						if (fabs(z - p.z) > PRECISION_F) 
							return z < p.z;
						return false;
					};
	bool operator > (const Vector3F &p) const { 
						if (fabs(x - p.x) > PRECISION_F) 
							return x > p.x;
						if (fabs(y - p.y) > PRECISION_F) 
							return y > p.y;
						if (fabs(z - p.z) > PRECISION_F) 
							return z > p.z;
						return false;
					};

	BOOL operator != (const Vector3F &p)  const { return !operator == (p); }
	float Len() const{ return (float)sqrt (x*x + y*y + z*z); }
	float Mod() const { return (float)sqrt (x*x + y*y + z*z); }
	float Mod2() const { return x*x + y*y + z*z; }
	float LenQ() const { return x*x + y*y + z*z; }
	float Sum() { return x + y + z; }
	Vector3F Int () const { return Vector3F((float)floor(x), (float)floor(y), (float)floor(z)); }
	Vector3F Abs () const { return Vector3F((float)fabs(x), (float)fabs(y), (float)fabs(z)); }
	
	Vector3F Q () const { return Vector3F(x*x, y*y, z*z); }
	float Max() const { return max(max(x, y), z); }
	float Modul() const { return Len(); }
	Vector3F Ort() { float l = Len(); return l ? Vector3F(x/l, y/l, z/l) : Vector3F(0, 0, 0); }
	void NormalAndLen(Vector3F &n, float &l) { l = (float)sqrt (x*x + y*y + z*z); n.x = x / l; n.y = y / l; n.z = z / l; }

	Vector3F &Normalize() { float l = (float)sqrt (x*x + y*y + z*z); if (l !=  0.0f) {x /= l; y /= l; z /= l;} return *this; }
	Vector3F Normal(Vector3F &d);
	float Angle(Vector3F &p2);
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
	enum ORIENT { XYZ = 0, YZ = 1, XZ = 2, XY = 4 };

	int	Orient () //������������ �������������� 
	{
		if (x > PRECISION_F)
		{
			if (y > PRECISION_F)
				return z > PRECISION_F ? XYZ : XY;
			return z > PRECISION_F ? XZ : (XZ | XY);
		}
		if (y > PRECISION_F)
			return z > PRECISION_F ? YZ : (YZ | XY);
  		return z > PRECISION_F ? (XZ | YZ) : (YZ | XZ | XY);
	}
	void Round()
	{
		if (fabs(z) < PRECISION_F)
			z = 0;
		if (fabs(y) < PRECISION_F)
			y = 0;
		if (fabs(x) < PRECISION_F)
			x = 0;
	}
};
inline float Vector3F::Cos(const Vector3F &p) const
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
		return (float)c;
	} 
	else
		return 0.0f;
}
inline void Swap(Vector3F &a, Vector3F &b)
{
	Vector3F temp;
	memcpy(&temp, &a, sizeof(Vector3F));
	memcpy(&a, &b, sizeof(Vector3F));
	memcpy(&b, &temp, sizeof(Vector3F));
}
#pragma pack(pop)
//
//------------------------------------
inline Vector3F Normal(Vector3F &a, Vector3F &b, Vector3F &org) 
{  
	return Vector3F((a.y - org.y) * (b.z - org.z) - (b.y - org.y) * (a.z - org.z),
		(a.z - org.z) * (b.x - org.x) - (b.z - org.z) * (a.x - org.x),
		(a.x - org.x) * (b.y - org.y) - (b.x - org.x) * (a.y - org.y)); 
}
//------------------------------------
//
//------------------------------------
inline Vector3F Mul (const Vector3F &u, const Vector3F &v) 
{ 
	return Vector3F (u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x); 
}
//------------------------------------
//
//------------------------------------
inline void Mul (const Vector3F &u, const Vector3F &v, Vector3F &res) 
{ 
	res.Set(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x); 
}
//------------------------------------
//
//------------------------------------
inline Vector3F operator + (const Vector3F &p, const Vector3F &p1)  { return Vector3F(p.x + p1.x, p.y + p1.y, p.z + p1.z); } 
//------------------------------------
//
//------------------------------------
inline Vector3F operator - (const Vector3F &p, const Vector3F &p1) { return Vector3F(p.x - p1.x, p.y - p1.y, p.z - p1.z); }
//------------------------------------
//
//------------------------------------
inline float operator * (const Vector3F &p,  const Vector3F &p1)  { return p.x * p1.x + p.y * p1.y + p.z * p1.z; } 
//------------------------------------
//
//------------------------------------
inline Vector3F operator / (const Vector3F &p, const Vector3F &p1)  { return Vector3F(fabs(p1.x) > PRECISION_F ? p.x/p1.x : 0.0f, fabs(p1.y) > PRECISION_F ? p.y/p1.y : 0.0f, fabs(p1.z) > PRECISION_F ? p.z/p1.z : 0.0f); } 
//------------------------------------
//
//------------------------------------
inline Vector3F operator + (const float d, const Vector3F &p)  { return Vector3F(p.x + d, p.y + d, p.z + d); } 

//------------------------------------
//
//------------------------------------
inline Vector3F operator - (const Vector3F &p, float d) { return Vector3F(p.x - d, p.y - d, p.z - d); }

//------------------------------------
//
//------------------------------------
inline Vector3F operator * (float i, const Vector3F &p){ return Vector3F(p.x * i, p.y * i, p.z * i); }
//------------------------------------
//
//------------------------------------
inline Vector3F operator * (const Vector3F &p, float i) { return Vector3F(p.x * i, p.y * i, p.z * i); }
//------------------------------------
//
//------------------------------------
inline Vector3F operator / (const Vector3F &p, float i)  { return Vector3F(p.x / i, p.y / i, p.z / i); }
//------------------------------------
//
//------------------------------------
inline Vector3F Vector3F::Normal(Vector3F &d) { Vector3F v(0, 0, 0); return ::Normal(*this, d, v); }   
//------------------------------------
//
//------------------------------------
inline float Vector3F::Angle(Vector3F &p2) {  return (float)(acos(Ort() * p2.Ort()) * 180.0 / PI); }
//------------------------------------
//
//------------------------------------
inline float Square(const Vector3F &a, const Vector3F &b, const Vector3F &c)
{
    return Mul(c - a, b - a).Len();
}
struct Vector3FC : public Vector3F {
		INT32 color;
		Vector3FC(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f, INT32 icolor = 0) : Vector3F(ix, iy, iz) { color = icolor; }
		Vector3FC(float s[3]): Vector3F(s) { color = 0; }
		Vector3FC(const Vector3F &s): Vector3F(s) { color = 0; }
		Vector3FC(const Vector3FC &s): Vector3F(s) { color = s.color; }
		void Reset() { x = y = z = 0.0f; color = 0; }
	//	operator Vector3F() const { return Vector3F(x, y, z); }
		Vector3FC &operator = (const Vector3F &c) { x = c.x; y = c.y; z = c.z; return *this; }
		Vector3FC &operator = (const Vector3FC &c) { x = c.x; y = c.y; z = c.z; color = c.color; return *this; }
	};

inline Vector3F::operator Vector3FC() const { return Vector3FC(x, y, z, 0); }
inline void Swap(Vector3FC &a, Vector3FC &b)
{
	Vector3FC temp;
	memcpy(&temp, &a, sizeof(Vector3FC));
	memcpy(&a, &b, sizeof(Vector3FC));
	memcpy(&b, &temp, sizeof(Vector3FC));
}
inline float RoundTo(float x, float Scale)
{
	int ix = (int)(x * Scale);
	switch (ix % 10)
	{
	case 1 :
	case -9 :
		ix -= 1;
		break;
	case 2 :
	case -8 :
		ix -= 2;
		break;
	case -1 :
	case 9 :
		ix += 1;
		break;
	case -2 :
	case 8 :
		ix += 2;
		break;
	default :
		break;
	}
	return (float)ix / Scale;
}
inline void RoundTo(Vector3F &a, float Scale)
{
	a.x = RoundTo(a.x, Scale);
	a.y = RoundTo(a.y, Scale);
	a.z = RoundTo(a.z, Scale);
}
#endif
