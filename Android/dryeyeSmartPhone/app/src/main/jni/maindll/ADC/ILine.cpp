#include "stdafx.h"
#include "ILine.h"
#include "Vector2F.h"

void IPixel::SetRop(int rop)
{
    switch (rop)
    {
        case R2_BLACK:
            _Rop = &ILine::opBLACK;
            break;
        case R2_NOTMERGEPEN:
            _Rop = &ILine::opNOTMERGEPEN;
            break;
        case R2_MASKNOTPEN:
            _Rop = &ILine::opMASKNOTPEN;
            break;
        case R2_NOTCOPYPEN:
            _Rop = &ILine::opNOTCOPYPEN;
            break;
        case R2_MASKPENNOT:
            _Rop = &ILine::opMASKPENNOT;
            break;
        case R2_NOT:
            _Rop = &ILine::opNOT;
            break;
        case R2_XORPEN:
            _Rop = &ILine::opXORPEN;
            break;
        case R2_NOTMASKPEN:
            _Rop = &ILine::opNOTMASKPEN;
            break;
        case R2_MASKPEN:
            _Rop = &ILine::opMASKPEN;
            break;
        case R2_NOTXORPEN:
            _Rop = &ILine::opNOTXORPEN;
            break;
        case R2_NOP:
            _Rop = &ILine::opNOP;
            break;
        case R2_MERGENOTPEN:
            _Rop = &ILine::opMERGENOTPEN;
            break;
        case R2_MERGEPENNOT:
            _Rop = &ILine::opMERGEPENNOT;
            break;
        case R2_MERGEPEN:
            _Rop = &ILine::opMERGEPEN;
            break;
        case R2_WHITE:
            _Rop = &ILine::opWHITE;
            break;
        case R2_COPYALPHA:
            _Rop = &ILine::opCOPYALPHA;
            break;
        default:
            _Rop = &ILine::opCOPYPEN;


    }
}

void ILine::Init(int x1, int y1, int x2, int y2)
{
    if (y1 == y2)
        if (x1 > x2)
        {
            int n = x1;
            x1 = x2;
            x2 = n;
        }

    if (x1 == x2)
        if (y1 > y2)
        {
            int n = y1;
            y1 = y2;
            y2 = n;
        }

    sX = cX = x1;
    sY = cY = y1;

    XEnd = x2;
    YEnd = y2;
    Dx = x2 - x1;
    Dy = y2 - y1;
    if (Dx != 0)
    {
        dX = abs(Dx) / Dx;
        Dx = abs(Dx);
    } else
        dX = 0;
    Dx2 = Dx + Dx;
    if (Dy != 0)
    {
        dY = abs(Dy) / Dy;
        Dy = abs(Dy);
    } else
        dY = 0;
    Dy2 = Dy + Dy;
    if (x1 == x2)
        type = VERT;
    else if (y1 == y2)
        type = HORZ;
    else if (Dx >= Dy)
        type = NHORZ;
    else
        type = NVERT;

    if (Dx >= Dy)
    {
        Ey0 = Ey = 2 + Dy - Dx;
        _Next = &ILine::NextForX;
    } else
    {
        Ex0 = Ex = 2 + Dx - Dy;
        _Next = &ILine::NextForY;
    }
    fIsPixel = (x1 == x2 && y1 == y2);
    Reset();
}

void ILine::Draw(DWORD *Buffer, int X, int Y, DWORD color)
{
    if (!(color & 0xFF000000)) return;

    //Reset();
    if (fIsPixel)
    {
        if (cX >= 0 && cX < X && cY >= 0 && cY < Y)
            Buffer[cX + cY * X] = color;
        return;
    }
    do
    {
        if (cX >= 0 && cX < X && cY >= 0 && cY < Y)
            Buffer[cX + cY * X] = color;
    } while (Next());
    if (cX >= 0 && cX < X && cY >= 0 && cY < Y)
        Buffer[cX + cY * X] = color;
}

void ILine::DrawRestrict(ColorRef *Buffer, const ImageInfo &info, BYTE color_alpha, BYTE bg_alpha)
{
    if (fIsPixel)
    {
        if (cX >= 0 && cX < info.width && cY >= 0 && cY < info.height)
            if (Buffer[cX + cY * info.wline].a == bg_alpha)
                Buffer[cX + cY * info.wline].a = color_alpha;
        return;
    }
    //Reset();
    do
    {
        int _x = cX;
        int _y = cY;
        if (_x < 0)
            _x = 0;
        if (_x >= info.width)
            _x = info.width - 1;
        if (_y < 0)
            _y = 0;
        if (_y >= info.height)
            _y = info.height - 1;
        if (Buffer[_x + _y * info.wline].a == bg_alpha)
            Buffer[_x + _y * info.wline].a = color_alpha;
    } while ((this->*_Next)());
    int _x = cX;
    int _y = cY;
    if (_x < 0)
        _x = 0;
    if (_x >= info.width)
        _x = info.width - 1;
    if (_y < 0)
        _y = 0;
    if (_y >= info.height)
        _y = info.height - 1;
    if (Buffer[_x + _y * info.wline].a == bg_alpha)
        Buffer[_x + _y * info.wline].a = color_alpha;
}

void ILine::Draw(DWORD *Buffer, const ImageInfo &info, DWORD color)
{
    if (!(color & 0xFF000000)) return;
    if (fIsPixel)
    {
        if (cX >= 0 && cX < info.width && cY >= 0 && cY < info.height)
            Buffer[cX + cY * info.wline] = color;
        return;
    }

//	Reset();
    do
    {
        if (cX >= 0 && cX < info.width && cY >= 0 && cY < info.height)
            Buffer[cX + cY * info.wline] = color;
    } while ((this->*_Next)());
    if (cX >= 0 && cX < info.width && cY >= 0 && cY < info.height)
        Buffer[cX + cY * info.wline] = color;
}

void ILine::DrawDashX(DWORD *Buffer, const ImageInfo &info, int width, DWORD color, int step1,
                      DWORD bkcolor, int bkmode, int step2)
{
//	Reset();

    DWORD clr[2] = {color, bkcolor};
    if (bkmode == TRANSPARENT)
    {
        ColorRef *c = (ColorRef *) clr + 1;
        c->a = 0;
    }
    int step[2] = {step1, step2};
    int cur = 0;

    int s = cX;
    for (; cX <= XEnd; ++cX)
        if (cX >= 0 && cX < info.width && cY >= 0 && cY < info.height)
        {
            if ((clr[cur] & 0xFF000000))
                (this->*_Rop)(Buffer + cX + cY * info.wline, clr[cur]);
            if (abs(cX - s) == step[cur])
            {
                cur = !cur;
                s = cX;
            }

        }

}

void ILine::DrawDashY(DWORD *Buffer, const ImageInfo &info, int width, DWORD color, int step1,
                      DWORD bkcolor, int bkmode, int step2)
{
//	Reset();
    DWORD clr[2] = {color, bkcolor};
    int step[2] = {step1, step2};
    int cur = 0;
    if (bkmode == TRANSPARENT)
    {
        ColorRef *c = (ColorRef *) clr + 1;
        c->a = 0;
    }

    int s = 0;
    s = cY;
    for (; cY <= YEnd; ++cY)
        if (cX >= 0 && cX < info.width && cY >= 0 && cY < info.height)
        {
            if ((clr[cur] & 0xFF000000))
                (this->*_Rop)(Buffer + cX + cY * info.wline, clr[cur]);
            if (abs(cY - s) == step[cur])
            {
                cur = !cur;
                s = cY;
            }

        }

}

void ILine::DrawDash(DWORD *Buffer, const ImageInfo &info, int width, DWORD color, int step1,
                     DWORD bkcolor, int bkmode, int step2)
{
    //Reset();
    DWORD clr[2] = {color, bkcolor};
    int step[2] = {step1, step2};
    int cur = 0;
    if (bkmode == TRANSPARENT)
    {
        ColorRef *c = (ColorRef *) clr + 1;
        c->a = 0;
    }

    int s = 0;
    s = type == NHORZ ? cX : cY;
    do
    {

        if (cX >= 0 && cX < info.width && cY >= 0 && cY < info.height)
        {
            if (type == NHORZ)
            {
                if ((clr[cur] & 0xFF000000))
                    (this->*_Rop)(Buffer + cX + cY * info.wline, clr[cur]);
                if (abs(cX - s) == step[cur])
                {
                    cur = !cur;
                    s = cX;
                }

            } else
            {
                if ((clr[cur] & 0xFF000000))
                    (this->*_Rop)(Buffer + cX + cY * info.wline, clr[cur]);
                if (abs(cY - s) == step[cur])
                {
                    cur = !cur;
                    s = cY;
                }
            }

        }
    } while ((this->*_Next)());
    if (cX >= 0 && cX < info.width && cY >= 0 && cY < info.height)
        Buffer[cX + cY * info.wline] = clr[0];

}

void
ILine::Draw(DWORD *Buffer, const ImageInfo &info, int width, DWORD color, DWORD bkcolor, int bkmode,
            int opp, int attrib)
{
    if (!(color & 0xFF000000)) return;

    SetRop(opp);
    if (fIsPixel)
    {
        if (cX >= 0 && cX < info.width && cY >= 0 && cY < info.height)
            (this->*_Rop)(Buffer + cX + cY * info.wline, color);
        return;
    }
    //Reset();
    if (width > 1)
        attrib = PS_SOLID;
    switch (attrib)
    {
        case PS_SOLID:
            if (type == HORZ)
                DrawX(Buffer, info, width, color);
            else if (type == VERT)
                DrawY(Buffer, info, width, color);
            else
                Draw(Buffer, info, width, color);

            break;
        case PS_DASH:
            if (type == HORZ)
                DrawDashX(Buffer, info, width, color, 18, bkcolor, bkmode, 4);
            else if (type == VERT)
                DrawDashY(Buffer, info, width, color, 18, bkcolor, bkmode, 4);
            else
                DrawDash(Buffer, info, width, color, 18, bkcolor, bkmode, 4);
            break;
        case PS_DOT:
            if (type == HORZ)
                DrawDashX(Buffer, info, width, color, 3, bkcolor, bkmode, 4);
            else if (type == VERT)
                DrawDashY(Buffer, info, width, color, 3, bkcolor, bkmode, 4);
            else
                DrawDash(Buffer, info, width, color, 3, bkcolor, bkmode, 4);
            break;
    }

}

void ILine::DrawX(DWORD *Buffer, const ImageInfo &info, int width, DWORD color)
{

    if (width <= 1)
    {
        for (; cX <= XEnd; ++cX)
            if (cX >= 0 && cX < info.width && cY >= 0 && cY < info.height)
                (this->*_Rop)(Buffer + cX + cY * info.wline, color);
    } else
    {
        width /= 2;
        int w2p1 = width * width + 1;
        for (; cX <= XEnd; ++cX)
        {
            int _x = cX;
            int _y = cY;
            for (int k = -width; k <= width; ++k)
            {
                int __x = _x + k;
                if (__x >= 0 && __x < info.width)
                {
                    int k2 = k * k;
                    for (int l = -width; l <= width; ++l)
                    {
                        int __y = _y + l;
                        if (__y >= 0 && __y < info.height)
                        {
                            int d2 = k2 + l * l;
                            if ((d2 + 1) <= w2p1)
                                (this->*_Rop)(Buffer + __x + __y * info.wline, color);
                        }
                    }
                }
            }
        }
    }
}

void ILine::DrawY(DWORD *Buffer, const ImageInfo &info, int width, DWORD color)
{


    if (width <= 1)
    {
        for (; cY <= YEnd; ++cY)
            if (cX >= 0 && cX < info.width && cY >= 0 && cY < info.height)
                (this->*_Rop)(Buffer + cX + cY * info.wline, color);
    } else
    {
        width /= 2;
        int w2p1 = width * width + 1;
        for (; cY <= YEnd; ++cY)
        {
            int _x = cX;
            int _y = cY;
            for (int k = -width; k <= width; ++k)
            {
                int __x = _x + k;
                if (__x >= 0 && __x < info.width)
                {
                    int k2 = k * k;
                    for (int l = -width; l <= width; ++l)
                    {
                        int __y = _y + l;
                        if (__y >= 0 && __y < info.height)
                        {
                            int d2 = k2 + l * l;
                            if ((d2 + 1) <= w2p1)
                                (this->*_Rop)(Buffer + __x + __y * info.wline, color);
                        }
                    }
                }
            }
        }
    }
}

void ILine::Draw(DWORD *Buffer, const ImageInfo &info, int width, DWORD color)
{
    if (!(color & 0xFF000000)) return;
    if (fIsPixel)
    {
        if (cX >= 0 && cX < info.width && cY >= 0 && cY < info.height)
            (this->*_Rop)(Buffer + cX + cY * info.wline, color);
        return;
    }
//	Reset();

    if (width <= 1)
    {
        do
        {
            if (cX >= 0 && cX < info.width && cY >= 0 && cY < info.height)
                (this->*_Rop)(Buffer + cX + cY * info.wline, color);
        } while ((this->*_Next)());
        if (XEnd >= 0 && XEnd < info.width && YEnd >= 0 && YEnd < info.height)
            (this->*_Rop)(Buffer + XEnd + YEnd * info.wline, color);
    } else
    {
        width /= 2;
        int w2p1 = width * width + 1;
        do
        {
            int _x = cX;
            int _y = cY;
            for (int k = -width; k <= width; ++k)
            {
                int __x = _x + k;
                if (__x >= 0 && __x < info.width)
                {
                    int k2 = k * k;
                    for (int l = -width; l <= width; ++l)
                    {
                        int __y = _y + l;
                        if (__y >= 0 && __y < info.height)
                        {
                            int d2 = k2 + l * l;
                            if ((d2 + 1) <= w2p1)
                                (this->*_Rop)(Buffer + __x + __y * info.wline, color);
                        }
                    }
                }
            }
        } while ((this->*_Next)());
        int _x = cX;
        int _y = cY;
        for (int k = -width; k <= width; ++k)
        {
            int __x = _x + k;
            if (__x >= 0 && __x < info.width)
            {
                int k2 = k * k;
                for (int l = -width; l <= width; ++l)
                {
                    int __y = _y + l;
                    if (__y >= 0 && __y < info.height)
                    {
                        int d2 = k2 + l * l;
                        if ((d2 + 1) <= w2p1)
                            (this->*_Rop)(Buffer + __x + __y * info.wline, color);
                    }
                }
            }
        }
    }
}

