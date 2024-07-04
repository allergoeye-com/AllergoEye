///////////////////////////////////////////////////
// Line3D.inl
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////

inline Line3D::operator Vector3D() const { return Vector3D(dest.x - org.x, dest.y - org.y, dest.z - org.z); }
//------------------------------------
//
//------------------------------------
inline Vector3D Line3D::Point(double t) { Vector3D norm = dest - org; return org + norm * t; }
//------------------------------------
//
//------------------------------------
inline Vector3D Line3D::Dir() { return (dest - org); }
//------------------------------------
//
//------------------------------------
inline Line3D &Line3D::Flip() { Vector3D tmp = org; org = dest; dest = tmp; return *this; }
