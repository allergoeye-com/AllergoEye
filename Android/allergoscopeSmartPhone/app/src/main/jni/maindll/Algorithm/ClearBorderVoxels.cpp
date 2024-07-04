#include "stdafx.h"
#include "mstack.h"
#include "MultiArRBSet.h"

struct Char3 {
    char x;
    char y;
    char z;
};
const Char3 ind6[6] = {{-1, 0,  0},
                       {1,  0,  0},
                       {0,  -1, 0},
                       {0,  1,  0},
                       {0,  0,  -1},
                       {0,  0,  1}};

const Char3 ind18[18] = {{-1, -1, 0},
                         {-1, 0,  0},
                         {-1, 1,  0},
                         {-1, 0,  -1},
                         {-1, 0,  1},
                         {1,  -1, 0},
                         {1,  0,  0},
                         {1,  1,  0},
                         {1,  0,  -1},
                         {1,  0,  1},
                         {0,  -1, -1},
                         {0,  1,  -1},
                         {0,  0,  -1},
                         {0,  -1, 1},
                         {0,  1,  1},
                         {0,  0,  1},
                         {0,  -1, 0},
                         {0,  1,  0}};


inline char Value3(char *mask, int x, int y, int z)
{
    return (x >= 0 && y >= 0 && z >= 0 && x < 3 && y < 3 && z < 3) ? mask[x + y * 3 + z * 9] : 127;

}


inline int Value3(int *mask, int x, int y, int z, int X, int Y, int Z)
{
    return (x >= 0 && y >= 0 && z >= 0 && x < X && y < Y && z < Z) ? mask[x + y * X + z * X * Y]
                                                                   : 0;

}

void MakeMask(int *mask, int *buffer, int X, int Y, int Z)
{
    for (int i = 0, l = X * Y * Z; i < l; ++i)
        mask[i] = buffer[i] != 0;

}

void MakeMask(int *mask, char *buffer, int X, int Y, int Z)
{
    for (int i = 0, l = X * Y * Z; i < l; ++i)
        mask[i] = buffer[i] != 0;

}

void InitLinks26_black(char *cube, char *links, int &Nc)
{
    memset(links, 127, 702);
    Nc = 0;
    for (int z = 0, plane = 0; z < 3; ++z, plane += 9)
    {
        for (int y = 0, line = plane; y < 3; ++y, line += 3)
        {
            for (int x = 0; x < 3; ++x)
            {
                int j = line + x;
                if (j == 13)
                    continue;
                char value = cube[j];
                if (value)
                {
                    ++Nc;
                    int k = 0;
                    for (int ix = -1; ix <= 1; ++ix)
                    {
                        int _ix = x + ix;
                        if (_ix >= 0 && _ix < 3)
                        {
                            for (int iy = -1; iy <= 1; ++iy)
                            {
                                int _iy = y + iy;
                                if (_iy >= 0 && _iy < 3)
                                {
                                    for (int iz = -1; iz <= 1; ++iz)
                                    {
                                        int _iz = z + iz;
                                        if (_iz >= 0 && _iz < 3)
                                        {
                                            int p = _ix + _iy * 3 + _iz * 9;
                                            if (p != 13 && cube[p])
                                                links[j * 26 + k++] = p;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


void InitLinks6_in_18_white(char *cube, char *links)
{
    memset(links, 127, 702);
    for (int l = 0; l < 18; ++l)
    {
        int x = ind18[l].x + 1;
        int y = ind18[l].y + 1;
        int z = ind18[l].z + 1;
        int j = x + 3 * y + 9 * z;
        char value = cube[j];
        if (value == 0)
        {
            int k = 0;
            for (int m = 0; m < 6; ++m)
            {
                int ix = x + ind6[m].x;
                int iy = y + ind6[m].y;
                int iz = z + ind6[m].z;
                int p = ix + iy * 3 + iz * 9;
                if ((ix == 1 || iy == 1 || iz == 1) && p != 13 && !Value3(cube, ix, iy, iz))
                    links[j * 26 + k++] = p;
            }
        }
    }
}

void TraceLinks6_in_18_white(char *cube, char *tree, char *links, int &_Nn)
{
    memset(tree, 127, 27);
    int Nn = -64;
    char stack[27];
    int i_stack = 0;
    char value;
    for (int m = 0; m < 6; ++m)
    {
        int j = (ind6[m].x + 1) + (ind6[m].y + 1) * 3 + (ind6[m].z + 1) * 9;
        if (tree[j] == 127 && cube[j] == 0)
        {
            value = Nn--;
            tree[j] = value;
            stack[0] = j;
            i_stack = 1;
            while (i_stack)
            {
                --i_stack;
                int n = stack[i_stack];
                for (int k = 0; k < 26; ++k)
                {
                    int index = links[n * 26 + k];
                    if (index == 127)
                        break;
                    if (tree[index] == 127)
                    {
                        tree[index] = value;
                        stack[i_stack] = index;
                        ++i_stack;
                    }
                }
            }
        }
    }
    _Nn = -Nn - 64;
}

void TraceLinks26_black(char *cube, char *tree, char *links, int &_Np)
{
    memset(tree, 127, 27);
    int Np = 64;
    char stack[27];
    int i_stack = 0;
    char value;

    for (int j = 0; j < 27; ++j)
    {
        if (j == 13)
            continue;

        if (tree[j] == 127 && cube[j] > 0)
        {
            value = Np++;
            tree[j] = value;
            stack[0] = j;
            i_stack = 1;
            while (i_stack)
            {
                --i_stack;
                int n = stack[i_stack];
                for (int k = 0; k < 26; ++k)
                {
                    int index = links[n * 26 + k];
                    if (index == 127)
                        break;
                    if (tree[index] == 127)
                    {
                        tree[index] = value;
                        stack[i_stack] = index;
                        ++i_stack;
                    }
                }
            }
        }
    }
    _Np = Np - 64;
}

int Mark1(int *mask, int X, int Y, int Z, int side, int off_x, int off_y, int off_z)
{
    int XY = X * Y;
    int XYZ = XY * Z;
    int N = 0;
    for (int plane_z = off_z * XY, z = off_z; plane_z < XYZ; plane_z += 2 * XY, z += 2)
    {
        for (int line_y = off_y * X, y = off_y; line_y < XY; line_y += 2 * X, y += 2)
        {
            for (int x = off_x; x < X; x += 2)
            {
                int i = x + line_y + plane_z;
                if (mask[i] &&
                    !Value3(mask, x + ind6[side].x, y + ind6[side].y, z + ind6[side].z, X, Y, Z))
                {
                    mask[i] |= 0x4000;
                    ++N;
                }
            }
        }
    }
    return N;
}

int
Mark2(int *Src, int X, int Y, int Z, int off_x, int off_y, int off_z, int Level, BOOL KeepLastPoint)
{
    int N = 0;
    int XY = X * Y;
    int XYZ = XY * Z;
    int j, ix, iy, iz;
    int ch;
    char cube[27];
    char links[702];
    char tree[27];
    for (int plane_z = off_z * XY, z = off_z; plane_z < XYZ; plane_z += 2 * XY, z += 2)
    {
        for (int line_y = off_y * X, y = off_y; line_y < XY; line_y += 2 * X, y += 2)
        {
            for (int x = off_x; x < X; x += 2)
            {
                int i = x + line_y + plane_z;
                ch = Src[i];
                if (Src[i] & 0x4000)
                {
                    j = 0;
                    for (ix = -1; ix <= 1; ++ix)
                        for (iy = -1; iy <= 1; ++iy)
                            for (iz = -1; iz <= 1; ++iz, ++j)
                                cube[j] = Value3(Src, x + ix, y + iy, z + iz, X, Y, Z) != 0;

                    ch = 0;
                    int Np = 0;
                    int Nn = 0;
                    int Nc = 0;
                    InitLinks26_black(cube, links, Nc);
                    TraceLinks26_black(cube, tree, links, Np);
                    if (Np > 1 || (KeepLastPoint && Nc == 1))
                        ch = Src[i] & 0x3fff;
                    else if (Np == 1)
                    {
                        InitLinks6_in_18_white(cube, links);
                        TraceLinks6_in_18_white(cube, tree, links, Nn);
                        if (Nn > 1)
                            ch = Src[i] & 0x3fff;
                    }
                    if (ch == 0)
                        ++N;
                    else if (ch == 1)
                        ch = Level;
                    Src[i] = ch;
                }
            }
        }
    }
    return N;
}

void ClearBorderVoxels(int *Buffer, int X, int Y, int Z, double CropLevel, BOOL fMoreBranches)
{
    int XY = X * Y;
    int XYZ = XY * Z;
    MArray<int> mask1(XYZ);
    MArray<int> mask2(XYZ);
    int *c1 = mask1.GetPtr();
    int *c2 = mask2.GetPtr();
    MakeMask(c1, Buffer, X, Y, Z);
    int Level = 1;
    int N = 1;
    int N0 = 1;
    double r;
    int f = 0;
    while (N)
    {
        ++Level;
        N = 0;
        N0 = 0;
        memcpy(c2, c1, sizeof(int) * XYZ);
        for (int side = 0; side < 6; ++side)
        {
            for (int off_x = 0; off_x < 2; ++off_x)
            {
                for (int off_y = 0; off_y < 2; ++off_y)
                {
                    for (int off_z = 0; off_z < 2; ++off_z)
                    {
                        int n0 = Mark1(c1, X, Y, Z, side, off_x, off_y, off_z);
                        int n = Mark2(c1, X, Y, Z, off_x, off_y, off_z, Level,
                                      fMoreBranches ? f == 0 : 0);
                        N0 += n0;
                        N += n;
                        if (n0 > 0 && (r = double(n) / n0) < CropLevel)
                        {
//							--Level;
//							memcpy(c1, c2, sizeof(int) * XYZ);
                            goto exit;
                        }
                    }
                }
            }
        }
        if (N0 > 0 && (r = double(N) / N0) < CropLevel)
        {
            --Level;
            memcpy(c1, c2, sizeof(int) * XYZ);
            break;
        }
        if (fMoreBranches)
            f = !f;
    }
    exit :
    N = 1;
    while (N)
    {
        ++Level;
        N = 0;
        N0 = 0;
        memcpy(c2, c1, sizeof(int) * XYZ);
        for (int side = 0; side < 6; ++side)
        {
            for (int off_x = 0; off_x < 2; ++off_x)
            {
                for (int off_y = 0; off_y < 2; ++off_y)
                {
                    int off_z = 0;
                    {
                        N0 += Mark1(c1, X, Y, Z, side, off_x, off_y, off_z);
                        N += Mark2(c1, X, Y, Z, off_x, off_y, off_z, Level, TRUE);
                    }
                }
            }
        }
    }
    int i;
    for (i = 0; i < XYZ; ++i)
    {
        if (Buffer[i] && !c1[i])
            Buffer[i] = 0;
    }
}

void ClearBorderVoxels(char *Buffer, int X, int Y, int Z, double CropLevel, BOOL fMoreBranches)
{
    int XY = X * Y;
    int XYZ = XY * Z;
    MArray<int> mask1(XYZ);
    MArray<int> mask2(XYZ);
    int *c1 = mask1.GetPtr();
    int *c2 = mask2.GetPtr();
    MakeMask(c1, Buffer, X, Y, Z);
    int Level = 1;
    int N = 1;
    int N0 = 1;
    double r;
    int f = 0;
    while (N)
    {
        ++Level;
        N = 0;
        N0 = 0;
        memcpy(c2, c1, sizeof(int) * XYZ);
        for (int side = 0; side < 6; ++side)
        {
            for (int off_x = 0; off_x < 2; ++off_x)
            {
                for (int off_y = 0; off_y < 2; ++off_y)
                {
                    for (int off_z = 0; off_z < 2; ++off_z)
                    {
                        int n0 = Mark1(c1, X, Y, Z, side, off_x, off_y, off_z);
                        int n = Mark2(c1, X, Y, Z, off_x, off_y, off_z, Level,
                                      fMoreBranches ? f == 0 : 0);
                        N0 += n0;
                        N += n;
                        if (n0 > 0 && (r = double(n) / n0) < CropLevel)
                        {
//							--Level;
//							memcpy(c1, c2, sizeof(int) * XYZ);
                            goto exit;
                        }
                    }
                }
            }
        }
        if (N0 > 0 && (r = double(N) / N0) < CropLevel)
        {
            --Level;
            memcpy(c1, c2, sizeof(int) * XYZ);
            break;
        }
        if (fMoreBranches)
            f = !f;
    }
    exit :
    N = 1;
    while (N)
    {
        ++Level;
        N = 0;
        N0 = 0;
        memcpy(c2, c1, sizeof(int) * XYZ);
        for (int side = 0; side < 6; ++side)
        {
            for (int off_x = 0; off_x < 2; ++off_x)
            {
                for (int off_y = 0; off_y < 2; ++off_y)
                {
                    int off_z = 0;
                    {
                        N0 += Mark1(c1, X, Y, Z, side, off_x, off_y, off_z);
                        N += Mark2(c1, X, Y, Z, off_x, off_y, off_z, Level, TRUE);
                    }
                }
            }
        }
    }
    int i;
    for (i = 0; i < XYZ; ++i)
    {
        if (Buffer[i] && !c1[i])
            Buffer[i] = 0;
    }
}
