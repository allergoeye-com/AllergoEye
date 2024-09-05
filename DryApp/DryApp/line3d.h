///////////////////////////////////////////////////
// Line3D.h
// GRAPHICS Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////
#ifndef __LINE3D__
#define __LINE3D__
#pragma pack(push)
#pragma pack(8)
struct Line3D {
	Vector3D org, dest;
	Line3D() {};
	Line3D(Vector3D &_org, Vector3D &_dest) : org(_org), dest(_dest) {};
	Line3D(POINT &_org, POINT &_dest) : org(_org), dest(_dest) {};
	Line3D(POINTL &_org, POINTL &_dest) : org(_org), dest(_dest) {};
	Line3D(Line3D &e) : org(e.org), dest(e.dest) {};
	Line3D &Flip();
	Vector3D Point(double t);
	Vector3D Dir();
	operator Vector3D() const;
};
#pragma pack(pop)

#endif
