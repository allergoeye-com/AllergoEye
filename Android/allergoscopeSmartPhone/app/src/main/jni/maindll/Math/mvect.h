///////////////////////////////////////////////////
// mvect.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////

#ifndef __ZM_V_T_GEOM__
#define __ZM_V_T_GEOM__

#include <math.h>
#include <limits.h>

#ifndef ANDROID_NDK
#include "buffer.h"
#else

#include "std.h"
#include "ArRBMap.h"

#endif

//#include "pointrect.h"
#pragma pack(push)
#pragma pack(8)
#ifndef PRECISION
#define PRECISION 1.0e-08
#endif
#ifndef PRECISION_F
#define PRECISION_F 1.0e-06
#endif

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif
#ifndef SGN
#define SGN(x)  ((x) >= 0 ? 1 : -1)
#endif
struct Vector3D;
struct Vector2D;
enum CROSS {
    EMPTY, INTERSECT, ENCLOSURED
};

#include "mform3d.h"
#include "mform2d.h"
#include "vector2d.h"
#include "vector3d.h"
#include "line2d.h"
#include "line3d.h"
#include "Segment2d.h"
#include "box2d.h"
#include "circle2d.h"
#include "vector3d.inl"

struct Vector4D {
    double x;
    double y;
    double z;
    double w;

    operator Vector3D() const
    { return Vector3D(x, y, z); }
};

#include "mform2d.inl"
#include "mform3d.inl"
#include "line3d.inl"
#include "plane3d.h"
#include "box3d.h"
#include "sphera3d.h"
#include "frustrum.h"
#include "vector3f.h"

typedef Vector3D Point3D;
#pragma pack(pop)

#endif
