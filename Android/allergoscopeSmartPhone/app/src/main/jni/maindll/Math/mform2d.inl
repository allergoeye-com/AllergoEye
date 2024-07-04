///////////////////////////////////////////////////
// MFORM2D.inl
// GRAPHICS Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////
//------------------------------------
//
//------------------------------------
inline double MFORM2D::Det()
{
    return arM[0] * (arM[4] * arM[8] - arM[7] * arM[5]) +
           arM[1] * (arM[6] * arM[5] - arM[3] * arM[8]) +
           arM[2] * (arM[3] * arM[7] - arM[6] * arM[4]);
}

inline void MFORM2D::SubMat1(Vector3D &v)
{
    v.x = (arM[4] * arM[8] - arM[7] * arM[5]);
    v.y = (arM[6] * arM[5] - arM[3] * arM[8]);
    v.z = (arM[3] * arM[7] - arM[6] * arM[4]);
}

//------------------------------------
//
//------------------------------------
inline void MFORM2D::SubMat2(Vector3D &v)
{
    v.x = (arM[2] * arM[7] - arM[8] * arM[1]);
    v.y = (arM[0] * arM[8] - arM[6] * arM[2]);
    v.z = (arM[1] * arM[6] - arM[7] * arM[0]);
}

//------------------------------------
//
//------------------------------------
inline void MFORM2D::SubMat3(Vector3D &v)
{
    v.x = (arM[1] * arM[5] - arM[4] * arM[2]);
    v.y = (arM[2] * arM[3] - arM[5] * arM[0]);
    v.z = (arM[0] * arM[4] - arM[3] * arM[1]);
}


//------------------------------------
//
//------------------------------------
inline double MFORM2D::Det1(double a1, double b1, double c1)
{
    return a1 * (arM[4] * arM[8] - arM[7] * arM[5]) +
           b1 * (arM[6] * arM[5] - arM[3] * arM[8]) +
           c1 * (arM[3] * arM[7] - arM[6] * arM[4]);
}

//------------------------------------
//
//------------------------------------
inline double MFORM2D::Det2(double a2, double b2, double c2)
{
    return a2 * (arM[2] * arM[7] - arM[8] * arM[1]) +
           b2 * (arM[0] * arM[8] - arM[6] * arM[2]) +
           c2 * (arM[1] * arM[6] - arM[7] * arM[0]);
}

//------------------------------------
//
//------------------------------------
inline double MFORM2D::Det3(double a3, double b3, double c3)
{
    return a3 * (arM[1] * arM[5] - arM[4] * arM[2]) +
           b3 * (arM[2] * arM[3] - arM[5] * arM[0]) +
           c3 * (arM[0] * arM[4] - arM[3] * arM[1]);
}

//------------------------------------
//
//------------------------------------
inline void MFORM2D::SetCol(int i, const Vector3D &p)
{
    i *= 3;
    if (i < 6)
    {
        arM[i] = p.x;
        arM[i + 1] = p.y;
        arM[i + 2] = p.z;
    }
}

//------------------------------------
//
//------------------------------------
inline void MFORM2D::SetRow(int i, const Vector3D &p)
{
    if (i < 3)
    {
        arM[i] = p.x;
        arM[i + 3] = p.y;
        arM[i + 6] = p.z;
    }
}

//------------------------------------
//
//------------------------------------
inline void MFORM2D::SetRows(Vector3D &a, Vector3D &b, Vector3D &c)
{
    arM[0] = a.x;
    arM[3] = a.y;
    arM[6] = a.z;
    arM[1] = b.x;
    arM[4] = b.y;
    arM[7] = b.z;
    arM[2] = c.x;
    arM[5] = c.y;
    arM[8] = c.z;

}

//------------------------------------
//
//------------------------------------
inline MFORM2D::MFORM2D(XFORM &m)
{
    M.m11ScaleX = m.eM11;
    M.m12ShiftX = m.eM12;
    M.m13Dx = m.eDx;
    M.m21ShiftY = m.eM21;
    M.m22ScaleY = m.eM22;
    M.m23Dy = m.eDy;
    M.m31 = M.m32 = 0;
    M.m33 = 1;

}

//------------------------------------
//
//------------------------------------
inline void MFORM2D::Init()
{
    memset(dwM, 0, sizeof(dwM));
    M.m33 = M.m11ScaleX = M.m22ScaleY = M.m33 = 1;
}

//------------------------------------
//
//------------------------------------
inline MFORM2D::operator XFORM() const
{
    XFORM m;
    m.eM11 = (FLOAT) M.m11ScaleX;
    m.eM12 = (FLOAT) M.m12ShiftX;
    m.eDx = (FLOAT) M.m13Dx;
    m.eM21 = (FLOAT) M.m21ShiftY;
    m.eM22 = (FLOAT) M.m22ScaleY;
    m.eDy = (FLOAT) M.m23Dy;
    return m;
}

//------------------------------------
//
//------------------------------------
inline MFORM2D &MFORM2D::operator*=(MFORM2D &m)
{
    double tmp[3][3];
    memset(tmp, 0, sizeof(tmp));
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
                tmp[i][j] += dwM[k][j] * m.dwM[i][k];

        }
    memcpy(dwM, tmp, sizeof(tmp));
    return *this;
}

//------------------------------------
//
//------------------------------------
inline void MFORM2D::Shift(double x, double y, double xref, double yref)
{
    MFORM2D m;
    m.M.m12ShiftX = x;
    m.M.m21ShiftY = y;
    m.M.m13Dx = -x * yref;
    m.M.m23Dy = -y * xref;

    operator*=(m);
}

//------------------------------------
//
//------------------------------------
inline void MFORM2D::Scale(double x, double y)
{
    MFORM2D m;
    m.M.m11ScaleX = x;
    m.M.m22ScaleY = y;

    operator*=(m);
}

//------------------------------------
//
//------------------------------------
inline void MFORM2D::Rotate(double angle_degree)
{
    double angle = angle_degree * PI / 180.0;
    MFORM2D m;
    m.M.m22ScaleY = m.M.m11ScaleX = cos(angle);
    m.M.m12ShiftX = -sin(angle);
    m.M.m21ShiftY = -m.M.m12ShiftX;
    operator*=(m);
}

//------------------------------------
//
//------------------------------------
inline void MFORM2D::Rotate(double angle_degree, int x_org, int y_org)
{
    Move(-x_org, -y_org);
    Rotate(angle_degree);
    Move(x_org, y_org);
}

//------------------------------------
//
//------------------------------------
inline void MFORM2D::Scale(double x, double y, int x_org, int y_org)
{
    Move(-x_org, -y_org);
    Scale(x, y);
    Move(x_org, y_org);
}

//------------------------------------
//
//------------------------------------
inline void MFORM2D::Move(double x, double y)
{
    MFORM2D m;
    m.M.m13Dx = x;
    m.M.m23Dy = y;
    operator*=(m);
}

//------------------------------------
//
//------------------------------------
inline void MFORM2D::Decompose(MFORM2D &shift, MFORM2D &scale, MFORM2D &rotate, Vector2D &offset)
{
    offset.x = M.m13Dx;
    offset.y = M.m23Dy;
    double R = sqrt(M.m11ScaleX * M.m11ScaleX + M.m12ShiftX * M.m12ShiftX);
    shift.Init();
    shift.M.m21ShiftY = R ? (M.m11ScaleX * M.m21ShiftY + M.m12ShiftX * M.m22ScaleY) / (R * R) : 0;
    scale.Init();
    scale.M.m11ScaleX = R;
    scale.M.m22ScaleY = R; // ? (M.m11ScaleX * M.m22ScaleY - M.m12ShiftX * M.m21ShiftY)/R : 0;
    rotate.Init();
    rotate.M.m11ScaleX = R ? M.m11ScaleX / R : 0;
    rotate.M.m21ShiftY = R ? -M.m12ShiftX / R : 0;
    rotate.M.m12ShiftX = -rotate.M.m21ShiftY;
    rotate.M.m22ScaleY = rotate.M.m11ScaleX;
}

