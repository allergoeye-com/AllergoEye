///////////////////////////////////////////////////
// MFORM3D.h
// GRAPHICS Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////
#ifndef __MFORM3D__
#define __MFORM3D__
#pragma pack(push)
#pragma pack(8)
struct MFORM3D {
union {
	struct {
	double m11ScaleX;
	double m21ShiftY;
	double m31ShiftZ;
	double m41;

	double m12ShiftX;
	double m22ScaleY;
	double m32Shift;
	double m42;

	double m13Shift;
	double m23Shift;
	double m33ScaleZ;
	double m43;

	double m14Dx;
	double m24Dy;
	double m34Dz;
	double m44;
	} M;
	double arM[16];
	double dwM[4][4];
};
	MFORM3D();
	MFORM3D(const MFORM3D &m) { memcpy(dwM, m.dwM, sizeof(dwM)); }
	void Move(double x, double y, double z);
	bool operator == (MFORM3D &f);
	const MFORM3D &operator =(const MFORM3D &m) { memcpy(dwM, m.dwM, sizeof(dwM)); return *this; }
	MFORM3D &MulV(MFORM3D &m);
	void Clear0() { for (int i = 0; i < 16; ++i) if (fabs(arM[i]) < PRECISION)arM[i] = 0.0; }
	MFORM3D &operator += (MFORM3D &f);
	void RotateX(double angle_degree);
	void RotateY(double angle_degree);
	void RotateZ(double angle_degree);
	void Rotate(double angle_degree, Vector3D *os = 0);
	void Rotate(double s, double c, Vector3D *os);

	void Scale(double x, double y, double z);
	MFORM3D &operator *= (MFORM3D &m);
	void LookAt(Vector3D &eye, Vector3D &look, Vector3D &up);
	void LookAt2(Vector3D &eye, Vector3D &center, Vector3D &up);
	void Ortho(double left, double top, double right, double bottom, double near, double far);
	void Frustrum(double left, double top, double right, double bottom, double dbFar, double dbNear);
	void Perspective(double angle, double width, double height, double dbNear, double dbFar);
	void Perspective(double angle, double aspect, double dbNear, double dbFar);
	void Reset();
	void Move(Vector3D &v);
	bool Invert(MFORM3D &res);
	Vector3D Dir(Vector3D &p);
	Vector3D Translation ();
	void SetInv(Vector3D &offset, Vector3D &rotate, Vector3D &scale);


};
#pragma pack(pop)

#endif
