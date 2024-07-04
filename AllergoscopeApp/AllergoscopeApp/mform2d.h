///////////////////////////////////////////////////
// MFORM2D.h
// GRAPHICS Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////
#ifndef __MFORM2D__
#define __MFORM2D__

#ifndef WIN32
#ifndef ANDROID_NDK
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
#endif
#pragma pack(push)
#pragma pack(8)
struct MFORM2D {
union {
	struct {
		double m11ScaleX;
		double m21ShiftY;
		double m31;
		
		double m12ShiftX;
		double m22ScaleY;
		double m32;
		
		double m13Dx;
		double m23Dy;
		double m33;
	} M;
	double arM[9];
	double dwM[3][3];
};
	MFORM2D()
	{
		memset(dwM, 0, sizeof(dwM));
		M.m33 = M.m11ScaleX = M.m22ScaleY = M.m33 = 1;
	}
	MFORM2D(const MFORM2D &m) { memcpy(dwM, m.dwM, sizeof(dwM)); }
	MFORM2D(XFORM &m);
	void Init();
	operator XFORM() const;
	double Det();
	double Det1(double a1, double b1, double c1);
	double Det2(double a2, double b2, double c2);
	double Det3(double a3, double b3, double c3);
	void SetCol(int i, const Vector3D &p);
	void SetRow(int i, const Vector3D &p);
	void SetRows(Vector3D &a, Vector3D &b, Vector3D &c);
	void SubMat3(Vector3D &v); 
	void SubMat2(Vector3D &v); 
	void SubMat1(Vector3D &v); 
	void Move(double x, double y);
	void Rotate (double angle_degree);
	void Rotate(double angle_degree, int x_org, int y_org);
	void Shift (double x, double y, double xref = 0, double yref = 0);
	void Scale(double x, double y);
	void Scale(double x, double y, int x_org, int y_org);
	MFORM2D &operator *= (MFORM2D &m);
	void Decompose (MFORM2D &shift, MFORM2D &scale, MFORM2D &rotate, Vector2D &offset);

};
#pragma pack(pop)

#endif
