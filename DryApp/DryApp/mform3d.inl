///////////////////////////////////////////////////
// MFORM3D.inl
// GRAPHICS Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////
//------------------------------------
//
//------------------------------------
inline MFORM3D::MFORM3D()
{
	memset(dwM, 0, sizeof(dwM));
	M.m44 = M.m11ScaleX = M.m22ScaleY = M.m33ScaleZ = 1;
}
//------------------------------------
//
//------------------------------------
inline Vector3D operator * (const MFORM3D &form, const Vector3D &p)
{
	return Vector3D(form.M.m11ScaleX * p.x + form.M.m12ShiftX * p.y + form.M.m13Shift *  p.z + form.M.m14Dx,
		form.M.m21ShiftY * p.x + form.M.m22ScaleY * p.y + form.M.m23Shift *  p.z + form.M.m24Dy,
		form.M.m31ShiftZ * p.x + form.M.m32Shift * p.y + form.M.m33ScaleZ * p.z + form.M.m34Dz);
}
//------------------------------------
//
//------------------------------------
inline Vector3D operator * (const Vector3D &p, const MFORM3D &form)
{
	return Vector3D(form.M.m11ScaleX * p.x + form.M.m12ShiftX * p.y + form.M.m13Shift *  p.z + form.M.m14Dx,
		form.M.m21ShiftY * p.x + form.M.m22ScaleY * p.y + form.M.m23Shift *  p.z + form.M.m24Dy,
		form.M.m31ShiftZ * p.x + form.M.m32Shift * p.y + form.M.m33ScaleZ * p.z + form.M.m34Dz);
}

//------------------------------------
//
//------------------------------------
inline Vector3D MFORM3D::Dir(Vector3D &p)
{
	return Vector3D(M.m11ScaleX * p.x + M.m12ShiftX * p.y + M.m13Shift *  p.z,
		M.m21ShiftY * p.x + M.m22ScaleY * p.y + M.m23Shift *  p.z,
		M.m31ShiftZ * p.x + M.m32Shift * p.y + M.m33ScaleZ * p.z);
}
//------------------------------------
//
//------------------------------------
inline Vector3D MFORM3D::Translation ()
{
	return Vector3D(M.m14Dx, M.m24Dy, M.m34Dz);
}
//------------------------------------
//
//------------------------------------
inline void MFORM3D::Reset()
{
	memset(dwM, 0, sizeof(dwM));
	M.m44 = M.m11ScaleX = M.m22ScaleY = M.m33ScaleZ = 1;

}
//------------------------------------
//
//------------------------------------
inline void MFORM3D::Scale(double x, double y, double z)
{
	MFORM3D m;
	m.M.m11ScaleX = x;
	m.M.m22ScaleY = y;
	m.M.m33ScaleZ = z;
	operator *= (m);
}
//------------------------------------
//
//------------------------------------
inline MFORM3D &MFORM3D::MulV(MFORM3D &m)
{
	double tmp[4][4];
	memset(tmp, 0, sizeof(tmp));
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 4; ++k)
				tmp[i][j] += dwM[i][k] * m.dwM[k][j];
		}
	memcpy (dwM, tmp, sizeof(MFORM3D));
	return *this;
}
//------------------------------------
//
//------------------------------------
inline MFORM3D &MFORM3D::operator *= (MFORM3D &m)
{

	double tmp[4][4];
	memset(tmp, 0, sizeof(tmp));
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 4; ++k)
				tmp[i][j] += dwM[k][j] * m.dwM[i][k];
				
		}
	memcpy (dwM, tmp, sizeof(MFORM3D));
	return *this;
}
//------------------------------------
//
//------------------------------------
inline MFORM3D operator * (MFORM3D &a, MFORM3D &b)
{
	MFORM3D res;
	memset(res.dwM, 0, sizeof(MFORM3D));
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 4; ++k)
				res.dwM[i][j] += a.dwM[k][j] * b.dwM[i][k];
				
		}
	return res;
}
//------------------------------------
//
//------------------------------------
inline MFORM3D &MFORM3D::operator += (MFORM3D &m)
{
	double tmp[4][4];
	memset(tmp, 0, sizeof(tmp));
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
				dwM[i][j] += m.dwM[i][i];
	return *this;
}

//------------------------------------
//
//------------------------------------
inline void MFORM3D::RotateX(double angle_degree)
{
	Vector3D os(1, 0, 0);
	Rotate(angle_degree, &os);
}
//------------------------------------
//
//------------------------------------
inline void MFORM3D::RotateY(double angle_degree)
{
	Vector3D os(0, 1, 0);
	Rotate(angle_degree, &os);

}
//------------------------------------
//
//------------------------------------
inline void MFORM3D::RotateZ(double angle_degree)
{
	Vector3D os(0, 0, 1);
	Rotate(angle_degree, &os);
}
//------------------------------------
//
//------------------------------------
inline void MFORM3D::Rotate(double angle_degree, Vector3D *os /*= 0*/)
{
	Vector3D o(1, 1, 1);
	if (!os) os = &o; 
	os->Normalize();
	 double angle = -angle_degree * PI/180.0;
	 MFORM3D m;
	 double c = cos(angle);
	 if (fabs(c) < PRECISION)
		c = 0.0;
	 double s = sin(angle);
	 if (fabs(s) < PRECISION)
		s = 0.0;
	Rotate(s, c, os);	
}
inline void MFORM3D::Rotate(double s, double c, Vector3D *os)
{
	os->Normalize();
	 MFORM3D m;
	 m.dwM[0][0] = c + (1 - c) * os->x * os->x;
	 m.dwM[0][1] = (1 - c) * os->x * os->y - s * os->z;
	 m.dwM[0][2] = (1 - c) * os->x * os->z + s * os->y;

	 m.dwM[1][0] = (1 - c) * os->x * os->y + s * os->z;
	 m.dwM[1][1] = c + (1 - c) * os->y * os->y;
	 m.dwM[1][2] = (1 - c) * os->y * os->z - s * os->x;

	 m.dwM[2][0] = (1 - c) * os->x * os->z - s * os->y;
	 m.dwM[2][1] = (1 - c) * os->y * os->z + s * os->x;
	 m.dwM[2][2] = c + (1 - c) * os->z * os->z;
	operator *= (m);
}

//------------------------------------
//
//------------------------------------
inline void MFORM3D::Ortho(double left, double top, double right, double bottom, double Near, double Far)
{
	MFORM3D m;
	m.M.m11ScaleX = 2.0/(right - left);
	m.M.m22ScaleY = 2.0/(top - bottom);
	m.M.m33ScaleZ = -2.0/(Far - Near);
	m.M.m14Dx = -(right + left)/(right - left);
	m.M.m24Dy = -(top + bottom)/(top - bottom);
	m.M.m34Dz = -(Far + Near)/(Far - Near);
	m.M.m44	= 1;
	operator *= (m);

} 
//------------------------------------
//
//------------------------------------
inline void MFORM3D::SetInv(Vector3D &offset, Vector3D &rotate, Vector3D &scale) 
{
	Reset();

	MFORM3D r, n;
	Scale(scale.x, scale.y, scale.z);
	Move(offset.x, offset.y, 0);
	r.RotateZ(rotate.z);

	r.RotateY(rotate.y);
	r.RotateX(rotate.x);
	MFORM3D::operator *= (r);
	Move(0, 0, offset.z);

}

//------------------------------------
//
//------------------------------------
inline void MFORM3D::Move(double x, double y, double z)
{
	MFORM3D m;
	m.M.m14Dx = x;
	m.M.m24Dy = y;
	m.M.m34Dz = z;

	operator *= (m);

}
//------------------------------------
//
//------------------------------------
inline void MFORM3D::Move(Vector3D &v) 
{ 
	MFORM3D f; 
	f.M.m14Dx = v.x; 
	f.M.m24Dy = v.y;
	f.M.m34Dz = v.z; 
	operator *= (f); 
}
//------------------------------------
//
//------------------------------------
inline bool MFORM3D::Invert(MFORM3D &res)
{
    int i, j, k, swap;
    double t;
	res.Reset();
    MFORM3D temp = *this;
    for (i = 0; i < 4; i++) 
	{
		swap = i;
		for (j = i + 1; j < 4; ++j) 
		{
			if (fabs(temp.dwM[j][i]) > fabs(temp.dwM[i][i])) 
				swap = j;
		}

		if (swap != i) 
		{
		    for (k = 0; k < 4; ++k) 
			{
				t = temp.dwM[i][k];
				temp.dwM[i][k] = temp.dwM[swap][k];
				temp.dwM[swap][k] = t;

				t = res.dwM[i][k];
				res.dwM[i][k] = res.dwM[swap][k];
				res.dwM[swap][k] = t;
			}
		}

		if (temp.dwM[i][i]  == 0.0) 
			return 0;
		t = temp.dwM[i][i];
		for (k = 0; k < 4; ++k) 
		{
			temp.dwM[i][k] /= t;
			res.dwM[i][k] /= t;
		}
		for (j = 0; j < 4; j++) 
		{
			if (j != i) 
			{
				t = temp.dwM[j][i];
				for (k = 0; k < 4; k++) 
				{
					temp.dwM[j][k] -= temp.dwM[i][k] * t;
					res.dwM[j][k] -= res.dwM[i][k] * t;
				}
			}
		}
    }

    return 1;
}

//------------------------------------
//
//------------------------------------
inline bool MFORM3D::operator == (MFORM3D &f) 
{ 
	double *a = (double *)this, *b = (double *)&f; 
	for (int i = 0; i < 16; ++i) 
		if (fabs(a[i] - b[i]) > 1.0e-6) 
			return 0; 
	return 1; 
}

//------------------------------------
//
//------------------------------------
inline void MFORM3D::Frustrum(double left, double top, double right, double bottom, double dbNear, double dbFar)
{
	MFORM3D m;
	m.dwM[0][0] = 2 * dbNear/(right - left);
	m.dwM[1][1] = 2 * dbNear/(top - bottom);

	m.dwM[2][0] = (right + left)/(right - left);
	m.dwM[2][1] = (top + bottom)/(top - bottom);
	m.dwM[2][2] = -(dbFar + dbNear)/(dbFar - dbNear);
	m.dwM[2][3] = -1;
	m.dwM[3][2] = -2.0 * dbFar * dbNear/(dbFar - dbNear);
	m.dwM[3][3] = 0;
	operator *= (m);
	
}
//------------------------------------
//
//------------------------------------
inline void MFORM3D::Perspective(double angle, double aspect, double dbNear, double dbFar)
{
	MFORM3D m;
	double f = 1.0/tan(angle * PI/360.0);
	m.dwM[0][0] = f/aspect;
	m.dwM[1][1] = f;
	m.dwM[2][2] = -(dbFar + dbNear)/(dbFar - dbNear);
	m.dwM[2][3] = -1;
	m.dwM[3][2] = -2.0 * dbFar * dbNear/(dbFar - dbNear);
	m.dwM[3][3] = 0;
	operator *= (m);
	
}


//------------------------------------
//
//------------------------------------
inline void MFORM3D::Perspective(double angle, double width, double height, double dbNear, double dbFar)
{
	Perspective(angle, width/height, dbNear, dbFar);
}
inline void MFORM3D::LookAt2(Vector3D &eye, Vector3D &center, Vector3D &up)
{
    Vector3D forward, side;
    MFORM3D m;

    forward  = center - eye;
	forward.Normalize();
	side = Mul(forward, up);
    side.Normalize();
	up = Mul(side, forward);
    m.dwM[0][0] = side.x;
    m.dwM[1][0] = side.y;
    m.dwM[2][0] = side.z;

    m.dwM[0][1] = up.x;
    m.dwM[1][1] = up.y;
    m.dwM[2][1] = up.z;

    m.dwM[0][2] = -forward.x;
    m.dwM[1][2] = -forward.y;
    m.dwM[2][2] = -forward.z;
	operator *= (m);
    Move(-eye.x, -eye.y, -eye.z);
}

//------------------------------------
//
//------------------------------------
inline void MFORM3D::LookAt(Vector3D &eye, Vector3D &look, Vector3D &up)
{
	Vector3D n = eye - look;
	Vector3D u = up.Normal(n);
	Vector3D vv(0, 0, 0);
	Vector3D v = Normal(n, u, vv);
	u.Normalize();
	v.Normalize();
	n.Normalize();
	Vector3D d((-1 * eye)*u, (-1 * eye)*v, (-1 * eye)*n);
	MFORM3D m;
	m.dwM[0][0] = u.x;
	m.dwM[1][0] = u.y;
	m.dwM[2][0] = u.z;
	m.dwM[3][0] = d.x;
	

	m.dwM[0][1] = v.x;
	m.dwM[1][1] = v.y;
	m.dwM[2][1] = v.z;
	m.dwM[3][1] = d.y;

	m.dwM[0][2] = n.x;
	m.dwM[1][2] = n.y;
	m.dwM[2][2] = n.z;
	m.dwM[3][2] = d.z;
	operator *= (m);
}
//------------------------------------
//
//------------------------------------
inline Vector3D operator * (MFORM3D &m, double v[4])
{
	double tmp[4];
	for (int j = 0; j < 4; ++j)
	{
		tmp[j] = 0;
		for (int k = 0; k < 4; ++k)
			tmp[j] += m.dwM[k][j] * v[k];
		
	}
	return Vector3D(tmp[0], tmp[1], tmp[2]);	
}
