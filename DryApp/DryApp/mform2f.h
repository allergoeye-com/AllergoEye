#ifndef __MFORM2F__
#define __MFORM2F__
#ifndef WIN32

typedef struct  tagXFORM
  {
    FLOAT   eM11;
    FLOAT   eM12;
    FLOAT   eM21;
    FLOAT   eM22;
    FLOAT   eDx;
    FLOAT   eDy;
  } XFORM;
#endif
#pragma pack(push)
#pragma pack(4)
struct MFORM2F {
union {
	struct {
		float m11ScaleX;
		float m21ShiftY;
		float m31;
		
		float m12ShiftX;
		float m22ScaleY;
		float m32;
		
		float m13Dx;
		float m23Dy;
		float m33;
	};
	float arM[9];
	float dwM[3][3];
};
	MFORM2F()
	{
		memset(dwM, 0, sizeof(dwM));
		m33 = m11ScaleX = m22ScaleY = m33 = 1;
	}
	MFORM2F(const MFORM2F &m) { memcpy(dwM, m.dwM, sizeof(dwM)); }
	MFORM2F(XFORM &m);
	void Init();
	operator XFORM() const;
	float Det();
	float Det1(float a1, float b1, float c1);
	float Det2(float a2, float b2, float c2);
	float Det3(float a3, float b3, float c3);
	void Move(float x, float y);
	void Rotate (float angle_degree);
	void Rotate(float angle_degree, int x_org, int y_org);
	void Shift (float x, float y, float xref = 0, float yref = 0);
	void Scale(float x, float y);
	void Scale(float x, float y, int x_org, int y_org);
	MFORM2F &operator *= (MFORM2F &m);
	void Decompose (MFORM2F &shift, MFORM2F &scale, MFORM2F &rotate, Vector2F &offset);

};
#pragma pack(pop)

//------------------------------------
//
//------------------------------------
inline float MFORM2F::Det() 
{
	return arM[0] * (arM[4] * arM[8] - arM[7] * arM[5]) + 
	       arM[1] * (arM[6] * arM[5] - arM[3] * arM[8]) + 
	       arM[2] * (arM[3] * arM[7] - arM[6] * arM[4]);
}

//------------------------------------
//
//------------------------------------
inline float MFORM2F::Det1(float a1, float b1, float c1) 
{
	return a1 * (arM[4] * arM[8] - arM[7] * arM[5]) + 
	       b1 * (arM[6] * arM[5] - arM[3] * arM[8]) + 
	       c1 * (arM[3] * arM[7] - arM[6] * arM[4]);
}
//------------------------------------
//
//------------------------------------
inline float MFORM2F::Det2(float a2, float b2, float c2) 
{
	return a2 * (arM[2] * arM[7] - arM[8] * arM[1]) +
			b2 * (arM[0] * arM[8] - arM[6] * arM[2]) +  
			c2 * (arM[1] * arM[6] - arM[7] * arM[0]);
}

//------------------------------------
//
//------------------------------------
inline float MFORM2F::Det3(float a3, float b3, float c3) 
{
	return a3 * (arM[1] * arM[5] - arM[4] * arM[2]) +  
			b3 * (arM[2] * arM[3] - arM[5] * arM[0]) +
			c3 * (arM[0] * arM[4]  - arM[3] * arM[1]);
}

//------------------------------------
//
//------------------------------------
inline MFORM2F::MFORM2F(XFORM &m)
{
	m11ScaleX = m.eM11;
	m12ShiftX = m.eM12;
	m13Dx = m.eDx;
	m21ShiftY = m.eM21;
	m22ScaleY = m.eM22;
	m23Dy = m.eDy;
	m31 = m32 = 0;
	m33 = 1;

}
//------------------------------------
//
//------------------------------------
inline void MFORM2F::Init()
{
	memset(dwM, 0, sizeof(dwM));
	m33 = m11ScaleX = m22ScaleY = m33 = 1;
}

//------------------------------------
//
//------------------------------------
inline MFORM2F::operator XFORM() const
{
	XFORM m;
	m.eM11 = (FLOAT)m11ScaleX;
	m.eM12 = (FLOAT)m12ShiftX;
	m.eDx = (FLOAT)m13Dx;
	m.eM21 = (FLOAT)m21ShiftY;
	m.eM22 = (FLOAT)m22ScaleY;
	m.eDy = (FLOAT)m23Dy;
	return m;
}

//------------------------------------
//
//------------------------------------
inline MFORM2F &MFORM2F::operator *= (MFORM2F &m)
{
	float tmp[3][3];
	memset(tmp, 0, sizeof(tmp));
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
		{
			for (int k = 0; k < 3; ++k)
				tmp[i][j] += dwM[k][j] * m.dwM[i][k];
				
		}
	memcpy (dwM, tmp, sizeof(tmp));
	return *this;
}

//------------------------------------
//
//------------------------------------
inline void MFORM2F::Shift(float x, float y, float xref, float yref)
{
	MFORM2F m;
	m.m12ShiftX = x;
	m.m21ShiftY = y;
	m.m13Dx = -x * yref;
	m.m23Dy = -y * xref;

	operator *= (m);
}
//------------------------------------
//
//------------------------------------
inline void MFORM2F::Scale(float x, float y)
{
	MFORM2F m;
	m.m11ScaleX = x;
	m.m22ScaleY = y;
	
	operator *= (m);
}

//------------------------------------
//
//------------------------------------
inline void MFORM2F::Rotate(float angle_degree)
{
	float angle = angle_degree * (float)(PI/180.0);
	MFORM2F m;
	m.m22ScaleY = m.m11ScaleX = (float)cos(angle);
	m.m12ShiftX = (float)-sin(angle);
	m.m21ShiftY = -m.m12ShiftX;
	operator *= (m);
}
//------------------------------------
//
//------------------------------------
inline void MFORM2F::Rotate(float angle_degree, int x_org, int y_org)
{
	Move((float)-x_org, (float)-y_org);
	Rotate(angle_degree);
	Move((float)x_org, (float)y_org);
}
//------------------------------------
//
//------------------------------------
inline void MFORM2F::Scale(float x, float y, int x_org, int y_org)
{
	Move((float)-x_org, (float)-y_org);
	Scale(x, y);
	Move((float)x_org, (float)y_org);
}

//------------------------------------
//
//------------------------------------
inline void MFORM2F::Move (float x, float y)
{
	MFORM2F m;
	m.m13Dx = x;
	m.m23Dy = y;
	operator *= (m);
}
//------------------------------------
//
//------------------------------------
inline void MFORM2F::Decompose (MFORM2F &shift, MFORM2F &scale, MFORM2F &rotate, Vector2F &offset)
{
	offset.x = m13Dx;
	offset.y = m23Dy;
	float R = (float)sqrt(m11ScaleX * m11ScaleX + m12ShiftX * m12ShiftX);
	shift.Init();
	shift.m21ShiftY = R ? (m11ScaleX * m21ShiftY + m12ShiftX * m22ScaleY)/(R*R) : 0;
	scale.Init();
	scale.m11ScaleX = R;
	scale.m22ScaleY = R ? (m11ScaleX * m22ScaleY - m12ShiftX * m21ShiftY)/R : 0;
	rotate.Init();
	rotate.m11ScaleX = R ? m11ScaleX/R : 0;
	rotate.m21ShiftY = R ? -m12ShiftX/R : 0;
	rotate.m12ShiftX = -rotate.m21ShiftY;
	rotate.m22ScaleY = rotate.m11ScaleX;
}
#endif
