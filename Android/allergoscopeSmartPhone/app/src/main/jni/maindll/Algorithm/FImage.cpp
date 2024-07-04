#include "stdafx.h"
#include "FImage.h"
#include "MultiThread.h"
#include "LaplasePyramide.h"

//======================================================
// Returns the complementary error function erfc(x) with fractional error everywhere less than
// 1.2 x 10-7.
//======================================================
double Erf(double x)
{
    double z = fabs(x);
    double t = 1.0 / (1.0 + 0.5 * z);
    double ans = t * exp(-z * z - 1.26551223 +
                         t * (1.00002368 +
                              t * (0.37409196 +
                                   t * (0.09678418 +
                                        t * (-0.18628806 +
                                             t * (0.27886807 +
                                                  t * (-1.13520398 +
                                                       t * (1.48851587 + t * (-0.82215223 + t *
                                                                                            0.17087277)))))))));
//	return x >= 0.0 ? (1.0 - ans) * 0.5 :  (ans  - 1.0) * 0.5;
    if (x >= 0.0)
        return 1.0 - ans;
    else
        return ans - 1.0;
}

struct GParam {
    CMathImage *Img; //0
    double *Var; //1
    CMathImage *Mask; //2
    BOOL SumOfEntropy; //3
    BOOL VarEnt; //4
    double UpperThreshold;
};

typedef double (*PGS_Func)(double, GParam &);

struct GoldSection_Double {
    double Accuracy;
    int MaxItterationNumber;
    GParam *ExtraParam;
    BOOL Silent;
    PGS_Func _Func;
    double Boundaries[2];
    double Result;

    double Func(double x)
    { return (_Func)(x, *ExtraParam); }

    void TraceResult(double x, double FV);

    GoldSection_Double(PGS_Func Func, GParam *aExtraParam, double *Boundaries, double Accuracy);

    double GetResult()
    { return Result; }

    double *GetBoundaries()
    { return Boundaries; }

    void SetMaxItterationNumber(int n)
    { MaxItterationNumber = n; }

    void SetSilent(BOOL s)
    { Silent = s; }

    double Solve();
};

float __scaleFactor(int scale, int NumbScales, float ScaleCenter, float ScaleAlpha)
{
    if (ScaleAlpha == 1.0f)
        return 1.0f;
    float Max = 1.0f - ScaleCenter;
    float x = float(scale) / float(NumbScales) - ScaleCenter;
    float _s = 1.0f;
    if (x >= 0.0f)
    {
        if (Max > 0.0f)
        {
            float y = fabs(x) / Max;
            _s = pow(y, fabs(ScaleAlpha));
        }
    } else
    {
        if (ScaleCenter > 0.0f)
        {
            float y = fabs(x) / ScaleCenter;
            _s = pow(y, fabs(ScaleAlpha));
        }
    }
    if (ScaleAlpha < 0.0)
        _s = 1.0f - _s;
    return _s;
}

//======================================================
//
//======================================================
float __filter(float x, float sigma, float alpha, float beta)
{
    float mx = fabs(x);
    if (mx < sigma)
    {
        float y = mx / sigma;

        if (alpha == 1.0f)
            return x;
        else if (alpha == 0.25f)
            return SGN(x) * sigma * (float) sqrt(y);
        else if (alpha == 2.0f)
            return SGN(x) * sigma * y * y;
        else if (alpha == 3.0f)
            return SGN(x) * sigma * y * y * y;
        else if (alpha == 4.0f)
            return SGN(x) * sigma * y * y * y * y;
        else
            return SGN(x) * sigma * (float) pow(y, alpha);
    } else
        return SGN(x) * (beta * (mx - sigma) + sigma);
}

extern MultiThreadS *pMultiThreads;


struct TransformLinesParam {
    CFourierImage *Image;
    float *_re;
    float *_im;
    float *_s_re;
    float *_s_im;
    int *order;
    int L;
    int Start;
    int N;
    int Mask;
    float *Sin;
    float *Cos;
    int *done;
    float dX;
    float dY;
    float dZ;
};

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
double ThresholdedImageEntropyWithMask(double Threshold, GParam &Data)
{
    CMathImage *Image = Data.Img;
    double *var = (double *) Data.Var;
    CMathImage *Mask = Data.Mask;
    BOOL SumEntropy = Data.SumOfEntropy;
    BOOL VarEnt = Data.VarEnt;

    int X = Image->X;
    int Y = Image->Y;
    int *Buffer = Image->Data();
    int x, y, off_y;
    double G0 = 0.0;
    double G1 = 0.0;
    double VarG0 = 0.0;
    double VarG1 = 0.0;
    int count0 = 0;
    int count1 = 0;
    int *Mask_Buffer = Mask->Data();
    for (y = 0, off_y = 0; y < Y; ++y, off_y += X)
    {
        for (x = 0; x < X; ++x)
        {
            int pos = x + off_y;
            if (Mask_Buffer[pos])
            {
                double g = Buffer[pos];
                if (g <= Threshold)
                {
                    G0 += g;
                    VarG0 += var[pos];
                    ++count0;
                } else
                {
                    G1 += g;
                    VarG1 += var[pos];
                    ++count1;
                }
            }
        }
    }
    if (count0 > 0)
    {
        G0 /= count0;
        VarG0 /= count0;
    }
    if (count1 > 0)
    {
        G1 /= count1;
        VarG1 /= count1;
    }
    double H0 = 0.0;
    double H1 = 0.0;
    for (y = 0, off_y = 0; y < Y; ++y, off_y += X)
    {
        for (x = 0; x < X; ++x)
        {
            int pos = x + off_y;
            if (Mask_Buffer[pos])
            {
                double g = Buffer[pos];
                double v = var[pos];
                if (g <= Threshold)
                {
                    if (g > 0.0)
                        H0 += g - G0 - g * log(g / G0);
                    if (VarEnt && v > 0.0)
                        H0 += v - VarG0 - v * log(v / VarG0);
                } else
                {
                    if (g > 0.0)
                        H1 += g - G1 - g * log(g / G1);
                    if (VarEnt && v > 0.0)
                        H1 += v - VarG1 - v * log(v / VarG1);
                }
            }
        }
    }
    return SumEntropy ? -(H0 + H1) : -min(H0, H1);
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
double ThresholdedImageEntropyWithMask1(double Threshold, GParam &Data)
{
    CMathImage *Image = Data.Img;
    double *var = (double *) Data.Var;
    CMathImage *Mask = Data.Mask;
    BOOL SumEntropy = Data.SumOfEntropy;
    BOOL VarEnt = Data.VarEnt;
    double UpperThreshold = Data.UpperThreshold;

    int X = Image->X;
    int Y = Image->Y;
    int *Buffer = Image->Data();
    int x, y, off_y;
    double G0 = 0.0;
    double G1 = 0.0;
    double VarG0 = 0.0;
    double VarG1 = 0.0;
    int count0 = 0;
    int count1 = 0;
    int *Mask_Buffer = Mask->Data();
    for (y = 0, off_y = 0; y < Y; ++y, off_y += X)
    {
        for (x = 0; x < X; ++x)
        {
            int pos = x + off_y;
            if (Mask_Buffer[pos])
            {
                double g = Buffer[pos];
                if (g < UpperThreshold)
                {
                    if (g <= Threshold)
                    {
                        G0 += g;
                        VarG0 += var[pos];
                        ++count0;
                    } else
                    {
                        G1 += g;
                        VarG1 += var[pos];
                        ++count1;
                    }
                }
            }
        }
    }
    if (count0 > 0)
    {
        G0 /= count0;
        VarG0 /= count0;
    }
    if (count1 > 0)
    {
        G1 /= count1;
        VarG1 /= count1;
    }
    double H0 = 0.0;
    double H1 = 0.0;
    for (y = 0, off_y = 0; y < Y; ++y, off_y += X)
    {
        for (x = 0; x < X; ++x)
        {
            int pos = x + off_y;
            if (Mask_Buffer[pos])
            {
                double g = Buffer[pos];
                if (g < UpperThreshold)
                {
                    double v = var[pos];
                    if (g <= Threshold)
                    {
                        if (g > 0.0)
                            H0 += g - G0 - g * log(g / G0);
                        if (VarEnt && v > 0.0)
                            H0 += v - VarG0 - v * log(v / VarG0);
                    } else
                    {
                        if (g > 0.0)
                            H1 += g - G1 - g * log(g / G1);
                        if (VarEnt && v > 0.0)
                            H1 += v - VarG1 - v * log(v / VarG1);
                    }
                }
            }
        }
    }
    return SumEntropy ? -(H0 + H1) : -min(H0, H1);
}

CFourierImage::CFourierImage(CFourierImage &Src)
{
    order_x = 0L;
    order_y = 0L;
    _Sin_x = 0L;
    _Cos_x = 0L;
    _Sin_y = 0L;
    _Cos_y = 0L;
    X = Src.Width();
    Y = Src.Height();
    Len = X * Y;
    ReImage = (float *) HMALLOCZ(Len * sizeof(float));
    ImImage = (float *) HMALLOCZ(Len * sizeof(float));
    if (ReImage && ImImage)
    {
        Valid = TRUE;
        memcpy(ReImage, Src.ReImage, sizeof(float) * Len);
        memcpy(ImImage, Src.ImImage, sizeof(float) * Len);
    } else
    {
        Valid = FALSE;
        X = 0;
        Y = 0;
    }
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
CFourierImage::CFourierImage(CMathImage &aRe)
{
    order_x = 0L;
    order_y = 0L;
    _Sin_x = 0L;
    _Cos_x = 0L;
    _Sin_y = 0L;
    _Cos_y = 0L;
    int aX = aRe.X;
    int aY = aRe.Y;
    float n = (float) (log((double) aX) / log(2.0));
    if (n != int(n))
        X = (int) pow(2.0, int(n) + 1.0);
    else
        X = aX;
    n = (float) (log((double) aY) / log(2.0));
    if (n != int(n))
        Y = (int) pow(2.0, int(n) + 1.0);
    else
        Y = aY;
    Len = X * Y;
    ReImage = (float *) HMALLOC(Len * sizeof(float));
    memset(ReImage, 0, Len * sizeof(float));
    ImImage = (float *) HMALLOC(Len * sizeof(float));
    memset(ImImage, 0, Len * sizeof(float));
    if (ReImage && ImImage)
    {
        Valid = TRUE;
        if (X == aX && Y == aY)
        {
            for (int i = 0; i < Len; ++i)
                ReImage[i] = (float) aRe.Buffer[i];
        } else
        {
            for (int y = 0, off1 = 0, off2 = 0; y < aY; ++y, off1 += aX, off2 += X)
                for (int x = 0; x < aX; ++x)
                    ReImage[x + off2] = (float) aRe.Buffer[x + off1];

            int dx = X - aX;
            if (dx > 0)
            {
                for (int y = 0, off1 = 0, off2 = 0; y < aY; ++y, off1 += aX, off2 += X)
                {
                    float Re0 = ReImage[off2 + aX - 1];
                    float step = ((float) aRe.Buffer[off1] - Re0) / (float) dx;
                    for (int x = aX, _x = 0; x < X; ++x, ++_x)
                        ReImage[x + off2] = Re0 + step * _x;
                }
            }
            int dy = Y - aY;
            if (dy > 0)
            {
                for (int x = 0; x < X; ++x)
                {
                    int off = X * (aY - 1) + x;
                    float Re0 = ReImage[off];
                    float step = (ReImage[x] - Re0) / (float) dy;
                    off = X * aY + x;
                    for (int y = 0, _y = 0; _y < dy; y += X, ++_y)
                        ReImage[off + y] = Re0 + step * _y;
                }
            }
        }
    } else
    {
        X = 0;
        Y = 0;
        Valid = FALSE;
    }

}

//======================================================
//
//======================================================
CFourierImage::CFourierImage(FMathImage &aRe)
{
    order_x = 0L;
    order_y = 0L;
    _Sin_x = 0L;
    _Cos_x = 0L;
    _Sin_y = 0L;
    _Cos_y = 0L;
    DINT aX = aRe.Width();
    DINT aY = aRe.Height();
    float n = (float) (log((double) aX) / log(2.0));
    if (n != DINT(n))
        X = (DINT) pow(2.0, DINT(n) + 1.0);
    else
        X = aX;
    n = (float) (log((double) aY) / log(2.0));
    if (n != DINT(n))
        Y = (DINT) pow(2.0, DINT(n) + 1.0);
    else
        Y = aY;
    Len = X * Y;
    ReImage = (float *) HMALLOCZ(Len * sizeof(float));
    ImImage = (float *) HMALLOCZ(Len * sizeof(float));
    if (ReImage && ImImage)
    {
        Valid = TRUE;
        if (X == aX && Y == aY)
        {
            for (DINT i = 0; i < Len; ++i)
                ReImage[i] = aRe.Data()[i];
        } else
        {
            for (DINT y = 0, off1 = 0, off2 = 0, y_lim = min((DINT) Y, aY);
                 y < y_lim; ++y, off1 += aX, off2 += X)
                for (DINT x = 0, x_lim = min((DINT) X, aX); x < x_lim; ++x)
                    ReImage[x + off2] = aRe.Data()[x + off1];

            DINT dx = X - aX;
            if (dx > 0)
            {
                for (DINT y = 0, off1 = 0, off2 = 0; y < aY; ++y, off1 += aX, off2 += X)
                {
                    float Re0 = ReImage[off2 + aX - 1];
                    float step = (aRe.Data()[off1] - Re0) / (float) dx;
                    for (DINT x = aX, _x = 0; x < X; ++x, ++_x)
                        ReImage[x + off2] = Re0 + step * _x;
                }
            }
            DINT dy = Y - aY;
            if (dy > 0)
            {
                for (DINT x = 0; x < X; ++x)
                {
                    DINT off = X * (aY - 1) + x;
                    float Re0 = ReImage[off];
                    float step = (ReImage[x] - Re0) / (float) dy;
                    off = X * aY + x;
                    for (DINT y = 0, _y = 0; _y < dy; y += X, ++_y)
                        ReImage[off + y] = Re0 + step * _y;
                }
            }
        }
    } else
    {
        X = 0;
        Y = 0;
        Valid = FALSE;
    }
}

float CFourierImage::ReBiLine(float x, float y)
{
    DINT ix = DINT(x);
    DINT iy = DINT(y);
    float dx = x - (float) ix;
    float dy = y - (float) iy;
    if (dx == 0.0f)
    {
        if (dy == 0.0f)
            return Re(ix, iy);
        float v1 = Re(ix, iy);
        float v2 = Re(ix, iy + 1);
        return (v2 - v1) * dy + v1;
    } else if (dy == 0.0f)
    {
        float v1 = Re(ix, iy);
        float v2 = Re(ix + 1, iy);
        return (v2 - v1) * dx + v1;
    } else
    {
        float v1 = Re(ix, iy);
        float v2 = Re(ix + 1, iy);
        float v3 = Re(ix + 1, iy + 1);
        float v4 = Re(ix, iy + 1);
        float a = (v2 - v1) * dx + v1;
        float b = (v3 - v4) * dx + v4;
        return (b - a) * dy + a;
    }
}

//======================================================
//
//======================================================
float CFourierImage::ImBiLine(float x, float y)
{
    DINT ix = DINT(x);
    DINT iy = DINT(y);
    float dx = x - (float) ix;
    float dy = y - (float) iy;
    if (dx == 0.0f)
    {
        if (dy == 0.0f)
            return Im(ix, iy);
        float v1 = Im(ix, iy);
        float v2 = Im(ix, iy + 1);
        return (v2 - v1) * dy + v1;
    } else if (dy == 0.0f)
    {
        float v1 = Im(ix, iy);
        float v2 = Im(ix + 1, iy);
        return (v2 - v1) * dx + v1;
    } else
    {
        float v1 = Im(ix, iy);
        float v2 = Im(ix + 1, iy);
        float v3 = Im(ix + 1, iy + 1);
        float v4 = Im(ix, iy + 1);
        float a = (v2 - v1) * dx + v1;
        float b = (v3 - v4) * dx + v4;
        return (b - a) * dy + a;
    }
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------

CFourierImage::CFourierImage(int aX, int aY, int *buff)
{
    order_x = 0L;
    order_y = 0L;
    _Sin_x = 0L;
    _Cos_x = 0L;
    _Sin_y = 0L;
    _Cos_y = 0L;
    X = aX;
    Y = aY;
    double n = log((double) X) / log(2.0);
    if (n != int(n))
        X = (int) pow(2.0, int(n) + 1.0);
    n = log((double) Y) / log(2.0);
    if (n != int(n))
        Y = (int) pow(2.0, int(n) + 1.0);
    Len = X * Y;
    ReImage = (float *) HMALLOC(Len * sizeof(float));
    if (!buff)
        memset(ReImage, 0, Len * sizeof(float));
    ImImage = (float *) HMALLOC(Len * sizeof(float));
    memset(ImImage, 0, Len * sizeof(float));
    if (ReImage && ImImage)
    {
        Valid = TRUE;
        if (buff)
            CopySrc(ReImage, buff, aX, aY);
    } else
    {
        X = 0;
        Y = 0;
        Valid = FALSE;
    }
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void CFourierImage::Squeeze4()
{
    DINT x_X = X / 2;
    DINT _Y = Y / 2;
    DINT _Len = x_X * _Y;
    float *Re = (float *) HMALLOCZ(_Len * sizeof(float));
    float *Im = (float *) HMALLOCZ(_Len * sizeof(float));
    if (Re && Im)
    {
        for (DINT y = 0; y < Y; y += 2)
        {
            DINT l = y * X;
            DINT _l = (y / 2) * x_X;
            for (DINT x = 0; x < X; x += 2)
            {
                DINT _x = x / 2;
                for (DINT j = 0; j < 2; ++j)
                {
                    DINT k = l + X * j;
                    for (DINT i = 0; i < 2; ++i)
                    {
                        Re[_x + _l] += ReImage[x + i + k];
                        Im[_x + _l] += ImImage[x + i + k];
                    }
                }
                Re[_x + _l] *= 0.25f;
                Im[_x + _l] *= 0.25f;
            }
        }
        X = x_X;
        Y = _Y;
        Len = _Len;
        HFREE(ReImage);
        HFREE(ImImage);
        ReImage = Re;
        ImImage = Im;
    }
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void CFourierImage::CopySrc(float *dst, int *src, int aX, int aY)
{
    if (aX == X && aY == Y)
    {
        for (int i = 0; i < Len; ++i)
            dst[i] = (float) src[i];
    } else
    {

        int x, y;
        for (y = 0; y < aY; ++y)
        {
            int line = y * X;
            int a_line = y * aX;
            for (x = 0; x < aX; ++x)
                dst[x + line] = (float) src[x + a_line];
        }

        int dx = X - aX;
        if (dx > 0)
        {
            for (int y = 0, off = 0; y < aY; ++y, off += X)
            {
                float Re0 = dst[off + aX - 1];
                float step = (dst[off] - Re0) / (float) dx;
                for (int x = aX, _x = 0; x < X; ++x, ++_x)
                    dst[x + off] = Re0 + step * _x;
            }
        }
        int dy = Y - aY;
        if (dy > 0)
        {
            for (int x = 0; x < X; ++x)
            {
                int off = X * (aY - 1) + x;
                float Re0 = dst[off];
                float step = (dst[x] - Re0) / (float) dy;
                off = X * aY + x;
                for (int y = 0, _y = 0; _y < dy; y += X, ++_y)
                    dst[off + y] = Re0 + step * _y;
            }
        }
    }
}

CFourierImage::~CFourierImage()
{
    if (ImImage)
        HFREE(ImImage);
    if (ReImage)
        HFREE(ReImage);
    if (order_x)
        HFREE(order_x);
    if (order_y)
        HFREE(order_y);
    if (_Sin_x)
        HFREE(_Sin_x);
    if (_Cos_x)
        HFREE(_Cos_x);
    if (_Sin_y)
        HFREE(_Sin_y);
    if (_Cos_y)
        HFREE(_Cos_y);
}
//-----------------------------------------------------------------
//
//-----------------------------------------------------------------

BOOL CFourierImage::InitAuxilaryData(BOOL Direction)
{
    if (order_x == 0L)
        order_x = (int *) HMALLOC(X * sizeof(int));
    if (!order_x)
        return FALSE;

    int Value;
    for (Value = 0; Value < X; ++Value)
    {
        DWORD mask1 = (DWORD) X >> 1;
        DWORD mask2 = 1;
        DWORD inverse = 0;
        while (mask1 > 0)
        {
            if (Value & mask2)
                inverse |= mask1;
            mask1 >>= 1;
            mask2 <<= 1;
        }
        order_x[Value] = inverse;
    }
    if (order_y == 0L)
        order_y = (int *) HMALLOC(Y * sizeof(int));
    if (!order_y)
        return FALSE;

    for (Value = 0; Value < Y; ++Value)
    {
        DWORD mask1 = (DWORD) Y >> 1;
        DWORD mask2 = 1;
        DWORD inverse = 0;
        while (mask1 > 0)
        {
            if (Value & mask2)
                inverse |= mask1;
            mask1 >>= 1;
            mask2 <<= 1;
        }
        order_y[Value] = inverse;
    }


    float sign = Direction ? 1.0f : -1.0f;
    double PI_2 = asin(1.0) * 4.0;
    if (_Sin_x == 0L)
        _Sin_x = (float *) HMALLOC((X + 1) * sizeof(float));
    if (_Cos_x == 0L)
        _Cos_x = (float *) HMALLOC((X + 1) * sizeof(float));
    if (!_Cos_x || !_Sin_x)
        return FALSE;
    int i;
    for (i = 0; i < X; ++i)
    {
        _Sin_x[i] = (float) (sin(PI_2 * i / X * sign));
        _Cos_x[i] = (float) (cos(PI_2 * i / X));
    }
    _Sin_x[i] = 0.0;
    _Cos_x[i] = 1.0;
    if (_Sin_y == 0L)
        _Sin_y = (float *) HMALLOC((Y + 1) * sizeof(float));
    if (_Cos_y == 0L)
        _Cos_y = (float *) HMALLOC((Y + 1) * sizeof(float));
    if (!_Cos_y || !_Sin_y)
        return FALSE;
    for (i = 0; i < Y; ++i)
    {
        _Sin_y[i] = (float) (sin(PI_2 * i / Y * sign));
        _Cos_y[i] = (float) (cos(PI_2 * i / Y));
    }
    _Sin_y[i] = 0.0;
    _Cos_y[i] = 1.0;

    return TRUE;
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
UINT AFX_CDECL _SwapThreadFunc(void *Param)
{
    TransformLinesParam *d = (TransformLinesParam *) Param;
    d->Image->__Swap(d->_re, d->_im, d->order, d->Start, d->N);
    InterlockedIncrement(d->done);
    return 0;
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void CFourierImage::_Swap(float *re, float *im, int *order)
{
    int NTreads = pMultiThreads->Num();
    if (NTreads > 1)
    {
        int step = max(1, (int) Y / NTreads);
        int done = 0;
        TransformLinesParam *Param = (TransformLinesParam *) _alloca(
                sizeof(TransformLinesParam) * (NTreads + 1));
        Param[0].Image = this;
        Param[0]._re = re;
        Param[0]._im = im;
        Param[0].order = order;
        Param[0].Start = 0;
        Param[0].N = step;
        Param[0].done = &done;
        int i;
        for (i = 1; i < NTreads && Param[i - 1].N < Y; ++i)
        {
            memcpy(Param + i, Param, sizeof(TransformLinesParam));
            Param[i].Start = Param[i - 1].N;
            Param[i].N = Param[i].Start + step;
            Param[i].done = &done;
        }
        NTreads = i;
        Param[NTreads - 1].N = Y;
        for (i = 0; i < NTreads; ++i)
            pMultiThreads->Run(i, _SwapThreadFunc, Param + i);

        pMultiThreads->Wait();
    } else
        __Swap(re, im, order, 0, Y);
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void CFourierImage::__Swap(float *re, float *im, int *order, int Start, int Finish)
{

    float tmp;
    int YFinish = Finish * X;
    int Plane = X * Y;
    int pos, oPos;
    for (int y = Start * X; y < YFinish; y += X)
    {
        for (int x = 0; x < X; ++x)
        {
            if (order[x] < x)
            {
                pos = x + y;
                oPos = order[x] + y;
                tmp = re[pos];
                re[pos] = re[oPos];
                re[oPos] = tmp;
                tmp = im[pos];
                im[pos] = im[oPos];
                im[oPos] = tmp;
            }
        }
    }
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void
CFourierImage::Column2Row(float *dst_re, float *dst_im, float *src_re, float *src_im, int *order)
{
    int x, y, pos, iPos, order_iPos, order_pos;
    for (y = 0; y < Y; ++y)
    {
        int oy = order[y];
        if (oy < y)
        {
            for (x = 0; x < X; ++x)
            {
                pos = y + x * Y;
                iPos = x + X * y;
                order_iPos = x + X * oy;
                order_pos = oy + x * Y;
                dst_re[pos] = src_re[order_iPos];
                dst_re[order_pos] = src_re[iPos];
                dst_im[pos] = src_im[order_iPos];
                dst_im[order_pos] = src_im[iPos];
            }
        } else if (order[oy] == y)
        {
            for (x = 0; x < X; ++x)
            {
                pos = y + x * Y;
                iPos = x + X * y;
                dst_re[pos] = src_re[iPos];
                dst_im[pos] = src_im[iPos];
            }
        }
    }
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
UINT AFX_CDECL Row2ColumnThreadFunc(void *Param)
{
    TransformLinesParam *d = (TransformLinesParam *) Param;
    d->Image->_Row2Column(d->_re, d->_im, d->_s_re, d->_s_im, d->Start, d->N);
    InterlockedIncrement(d->done);
    return 0;
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void CFourierImage::Row2Column(float *dst_re, float *dst_im, float *src_re, float *src_im)
{
    int NTreads = pMultiThreads->Num();
    if (NTreads > 1)
    {
        int step = max(1, (int) X / NTreads);
        int done = 0;
        TransformLinesParam *Param = (TransformLinesParam *) _alloca(
                sizeof(TransformLinesParam) * (NTreads + 1));
        Param[0].Image = this;
        Param[0]._re = dst_re;
        Param[0]._im = dst_im;
        Param[0]._s_re = src_re;
        Param[0]._s_im = src_im;
        Param[0].Start = 0;
        Param[0].N = step;
        Param[0].done = &done;
        int i;
        for (i = 1; i < NTreads && Param[i - 1].N < X; ++i)
        {
            memcpy(Param + i, Param, sizeof(TransformLinesParam));
            Param[i].Start = Param[i - 1].N;
            Param[i].N = Param[i].Start + step;
            Param[i].done = &done;
        }
        NTreads = i;
        Param[NTreads - 1].N = X;
        for (i = 0; i < NTreads; ++i)
            pMultiThreads->Run(i, Row2ColumnThreadFunc, Param + i);
        pMultiThreads->Wait();
    } else
        _Row2Column(dst_re, dst_im, src_re, src_im, 0, X);
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void
CFourierImage::_Row2Column(float *dst_re, float *dst_im, float *src_re, float *src_im, int Start,
                           int Finish)
{
    int pos, oPos;
    int Plane = X * Y;
    for (int x = Start; x < Finish; ++x)
    {
        for (int y = 0; y < Y; ++y)
        {
            pos = y + x * Y;
            oPos = x + X * y;
            dst_re[oPos] = src_re[pos];
            dst_im[oPos] = src_im[pos];
        }
    }
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
UINT AFX_CDECL TransformLinesThreadFunc(void *Param)
{
    TransformLinesParam *d = (TransformLinesParam *) Param;
    d->Image->_TransformLines(d->_re, d->_im, d->_s_re, d->_s_im, d->L, d->Start, d->N, d->Mask,
                              d->Sin, d->Cos);
    InterlockedIncrement(d->done);
    return 0;
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void CFourierImage::TransformLines(float *&_re, float *&_im, int L, int Start, int N, int Mask,
                                   float *Sin, float *Cos)
{
    int NTreads = pMultiThreads->Num();
    if (NTreads > 1)
    {
        int step = max(1, (int) (N / L) / NTreads);
        int done = 0;
        TransformLinesParam *Param = (TransformLinesParam *) _alloca(
                sizeof(TransformLinesParam) * (NTreads + 1));
        Param[0].Image = this;
        Param[0]._re = _re;
        Param[0]._im = _im;
        Param[0]._s_re = ReImage;
        Param[0]._s_im = ImImage;
        Param[0].L = L;
        Param[0].Start = 0;
        Param[0].N = step * L;
        Param[0].Mask = Mask;
        Param[0].Sin = Sin;
        Param[0].Cos = Cos;
        Param[0].done = &done;
        int i;
        for (i = 1; i < NTreads && Param[i - 1].N < N; ++i)
        {
            memcpy(Param + i, Param, sizeof(TransformLinesParam));
            Param[i].Start = Param[i - 1].N;
            Param[i].N = Param[i].Start + step * L;
            Param[i].done = &done;
        }
        NTreads = i;
        Param[NTreads - 1].N = N;
        for (i = 0; i < NTreads; ++i)
            pMultiThreads->Run(i, TransformLinesThreadFunc, Param + i);
        pMultiThreads->Wait();


        int Factor = L / 2;
        for (int SubLen = 1; SubLen < L; Factor >>= 1)
        {
            step = SubLen;
            SubLen = step << 1;
            float *_ReImage = ReImage;
            ReImage = _re;
            _re = _ReImage;
            float *_ImImage = ImImage;
            ImImage = _im;
            _im = _ImImage;
        }
    } else
        _TransformLines(_re, _im, ReImage, ImImage, L, Start, N, Mask, Sin, Cos);
}

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
void CFourierImage::_TransformLines(float *&_re, float *&_im, float *&_s_re, float *&_s_im, int L,
                                    int Start, int N, int Mask, float *Sin, float *Cos)
{
    int Factor = L / 2;
    int step = 0;
    int Mask1 = 0;
    int j_odd, j_even, j_index, j, i, offset;
    float s, c;
    float *_ReImage, *Re_in, *_ImImage, *Im_in;
    memcpy(_re + Start, _s_re + Start, sizeof(float) * (N - Start));
    memcpy(_im + Start, _s_im + Start, sizeof(float) * (N - Start));
    float im, re, im_o, re_o;
    for (int SubLen = 1; SubLen < L; Factor >>= 1, Mask1 |= step)
    {
        step = SubLen;
        SubLen = step << 1;
        _ReImage = _s_re;
        _s_re = _re;
        _re = _ReImage;
        _ImImage = _s_im;
        _s_im = _im;
        _im = _ImImage;
        for (offset = Start; offset < N; offset += L)
        {
            _ReImage = _s_re + offset;
            Re_in = _re + offset;
            _ImImage = _s_im + offset;
            Im_in = _im + offset;
            for (j = 0; j < SubLen; ++j)
            {
                j_index = (j * Factor) & Mask;
                s = Sin[j_index];
                c = Cos[j_index];
                for (i = 0; i < L; i += SubLen)
                {
                    j_odd = i + (j & Mask1);
                    j_even = j_odd + step;
                    re = Re_in[j_even];
                    im = Im_in[j_even];
                    im_o = Im_in[j_odd];
                    re_o = Re_in[j_odd];
                    _ReImage[i + j] = re_o + c * re - s * im;
                    _ImImage[i + j] = im_o + s * re + c * im;
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
void CFourierImage::TransformColumn(float *&_re, float *&_im)
{
    int i, MaskY = 0;
    for (i = (int) Y >> 1; i > 0; i >>= 1)
        MaskY |= i;
    Column2Row(_re, _im, ReImage, ImImage, order_y);
    float *_ReImage = ReImage;
    ReImage = _re;
    _re = _ReImage;
    float *_ImImage = ImImage;
    ImImage = _im;
    _im = _ImImage;
    TransformLines(_re, _im, Y, 0, X * Y, MaskY, _Sin_y, _Cos_y);
    Row2Column(_re, _im, ReImage, ImImage);
    _ReImage = ReImage;
    ReImage = _re;
    _re = _ReImage;
    _ImImage = ImImage;
    ImImage = _im;
    _im = _ImImage;
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
bool CFourierImage::Transform(BOOL Direct)
{
    float *_ReImage = (float *) HMALLOC(Len * sizeof(float));
    if (!_ReImage) return 0;
    float *_ImImage = (float *) HMALLOC(Len * sizeof(float));
    if (!_ImImage)
    {
        HFREE(_ReImage);
        return 0;
    }


    int i, MaskX = 0;
    for (i = (int) X >> 1; i > 0; i >>= 1)
        MaskX |= i;
    if (InitAuxilaryData(Direct))
    {
        if (Direct)
        {
            _Swap(ReImage, ImImage, order_x);
            TransformLines(_ReImage, _ImImage, X, 0, X * Y, MaskX, _Sin_x, _Cos_x);
            TransformColumn(_ReImage, _ImImage);
        } else
        {
            TransformColumn(_ReImage, _ImImage);
            _Swap(ReImage, ImImage, order_x);
            TransformLines(_ReImage, _ImImage, X, 0, X * Y, MaskX, _Sin_x, _Cos_x);
            float denom = 1.0f / (X * Y);
            for (i = 0; i < Len; ++i)
            {
                ReImage[i] *= denom;
                ImImage[i] *= denom;
            }
        }
    }
    HFREE(_ImImage);
    HFREE(_ReImage);
    return TRUE;
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
void CFourierImage::Clear()
{
    memset(ReImage, 0, sizeof(float) * Len);
    memset(ImImage, 0, sizeof(float) * Len);
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
void CFourierImage::ClearRe()
{
    memset(ReImage, 0, sizeof(float) * Len);
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
void CFourierImage::ClearIm()
{
    memset(ImImage, 0, sizeof(float) * Len);
}


//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
void CFourierImage::Shift(float dX, float dY)
{
    int _Y = Y * X;
    double wx = 2.0 * PI * dX / X;
    double wy = 2.0 * PI * dY / Y;
    for (int x = 0; x < X; ++x)
    {
        double cx = cos(double(x) * wx);
        double sx = sin(double(x) * wx);
        for (int y = 0, _y = 0; y < Y; _y += X, ++y)
        {
            double cy = cos(double(y) * wy);
            double sy = sin(double(y) * wy);
            double c_xy = cy * cx - sy * sx;
            double s_xy = cy * sx + sy * cx;
            int pos = x + _y;
            double _re = ReImage[pos] * c_xy - ImImage[pos] * s_xy;
            double _im = ImImage[pos] * c_xy + ReImage[pos] * s_xy;
            ReImage[pos] = (float) _re;
            ImImage[pos] = (float) _im;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
CFourierImage &CFourierImage::operator*=(const CFourierImage &op)
{
    if (Valid && op.Valid && X == op.X && Y == op.Y)
    {
        for (int i = 0; i < Len; ++i)
        {
            float aRe = ReImage[i];
            float anIm = ImImage[i];
            ReImage[i] = aRe * op.ReImage[i] - anIm * op.ImImage[i];
            ImImage[i] = aRe * op.ImImage[i] + anIm * op.ReImage[i];
        }
    }
    return *this;
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
CFourierImage &CFourierImage::operator/=(const CFourierImage &op)
{
    if (Valid && op.Valid && X == op.X && Y == op.Y)
    {
        for (int i = 0; i < Len; ++i)
        {
            float denom = op.ReImage[i] * op.ReImage[i] + op.ImImage[i] * op.ImImage[i];
            if (denom != 0.0f)
            {
                float aRe = ReImage[i];
                float anIm = ImImage[i];
                ReImage[i] = (aRe * op.ReImage[i] + anIm * op.ImImage[i]) / denom;
                ImImage[i] = (anIm * op.ReImage[i] - aRe * op.ImImage[i]) / denom;
            } else
            {
                ReImage[i] = 0.0f;
                ImImage[i] = 0.0f;
            }
        }
    }
    return *this;
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
CFourierImage &CFourierImage::operator+=(const CFourierImage &op)
{
    if (Valid && op.Valid && X == op.X && Y == op.Y)
    {
        for (int i = 0; i < Len; ++i)
        {
            ReImage[i] += op.ReImage[i];
            ImImage[i] += op.ImImage[i];
        }
    }
    return *this;
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
CFourierImage &CFourierImage::operator-=(const CFourierImage &op)
{
    if (Valid && op.Valid && X == op.X && Y == op.Y)
    {
        for (int i = 0; i < Len; ++i)
        {
            ReImage[i] -= op.ReImage[i];
            ImImage[i] -= op.ImImage[i];
        }
    }
    return *this;
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
void CFourierImage::GetReShort(int *dst, int aX, int aY)
{
    if (aX == X && aY == Y)
    {
        for (int i = 0; i < Len; ++i)
        {
            if (ReImage[i] > 32767.0f)
                dst[i] = (short int) 32767;
            else if (ReImage[i] < -32768.0f)
                dst[i] = (short int) -32768;
            else
                dst[i] = (short int) (ReImage[i] + 0.5);
        }
    } else
    {
        int XY = X * Y;
        int aXY = aX * aY;
        int x_X = min(aX, X);
        int _Y = min(aY, Y);
        for (int y = 0; y < _Y; ++y)
        {
            int line = y * X;
            int aline = y * aX;
            for (int x = 0; x < x_X; ++x)
            {
                int pos = line + x;
                int pos_dst = aline + x;
                if (ReImage[pos] > 32767.0f)
                    dst[pos_dst] = (short int) 32767;
                else if (ReImage[pos] < -32768.0f)
                    dst[pos_dst] = (short int) -32768;
                else
                    dst[pos_dst] = (short int) (ReImage[pos] + 0.5);
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
void CFourierImage::GetImShort(int *dst, int aX, int aY)
{
    if (aX == X && aY == Y)
    {
        for (int i = 0; i < Len; ++i)
        {
            if (ImImage[i] > 32767.0f)
                dst[i] = (short int) 32767;
            else if (ImImage[i] < -32768.0f)
                dst[i] = (short int) -32768;
            else
                dst[i] = (short int) (ImImage[i] + 0.5);
        }
    } else
    {
        int XY = X * Y;
        int aXY = aX * aY;
        int x_X = min(aX, X);
        int _Y = min(aY, Y);
        for (int y = 0; y < _Y; ++y)
        {
            int line = y * X;
            int aline = y * aX;
            for (int x = 0; x < x_X; ++x)
            {
                int pos = line + x;
                int pos_dst = aline + x;
                if (ImImage[pos] > 32767.0f)
                    dst[pos_dst] = (short int) 32767;
                else if (ImImage[pos] < -32768.0f)
                    dst[pos_dst] = (short int) -32768;
                else
                    dst[pos_dst] = (short int) (ImImage[pos] + 0.5);
            }
        }
    }
}

//======================================================
//
//======================================================
void CFourierImage::MakeGaussMask2D(float width, float Amp)
{
    ClearRe();
    ClearIm();
    float w2_2 = 1.0f / (width * width * 2.0f);
    float norm = Amp / (2.0f * (float) PI * width * width);
    DINT lim = DINT(10.0 * width + 0.5) + 1;
    DINT aX = Width();
    DINT aY = Height();
    if (lim > aX / 2)
        lim = aX / 2;
    if (lim > aY / 2)
        lim = aY / 2;
    DINT x, y;
    _Re(0, 0) = norm;
    for (x = 1; x < lim; ++x)
    {
        float dx = (float) x;
        float v = exp(-(dx * dx) * w2_2) * norm;
        _Re(aX - x, 0) += v;
        _Re(x, 0) += v;
    }
    for (y = 1; y < lim; ++y)
    {
        float dy = (float) y;
        float v = exp(-(dy * dy) * w2_2) * norm;
        _Re(0, aY - y) += v;
        _Re(0, y) += v;
    }
    for (x = 1; x < lim; ++x)
    {
        float dx = (float) x;
        float dx2 = dx * dx;
        for (y = 1; y < lim; ++y)
        {
            float dy = (float) y;
            float v = exp(-(dx2 + dy * dy) * w2_2) * norm;
            _Re(aX - x, aY - y) += v;
            _Re(x, aY - y) += v;
            _Re(aX - x, y) += v;
            _Re(x, y) += v;
        }
    }
}

//======================================================
//
//======================================================
void CFourierImage::MakeGaussMask2D(float width, float cX, float cY, float Amp)
{
    ClearRe();
    ClearIm();
    float w2_2 = 1.0f / (width * width * 2.0f);
    float norm = Amp / (2.0f * (float) PI * width * width);
    DINT lim = DINT(15.0f * width + 0.5f) + 1;
    DINT aX = Width();
    DINT aY = Height();
    if (lim > X / 2)
        lim = X / 2;
    if (lim > Y / 2)
        lim = Y / 2;
    DINT x, y;
    DINT min_x = max((DINT) 0, (DINT) cX - lim);
    DINT max_x = min(aX, (DINT) cX + lim);
    DINT min_y = max((DINT) 0, (DINT) cY - lim);
    DINT max_y = min(aY, (DINT) cY + lim);

    for (x = min_x; x < max_x; ++x)
    {
        float dx = (float) (x - cX);
        for (y = min_y; y < max_y; ++y)
        {
            float dy = (float) (y - cY);
            _Re(x, y) += (float) exp(-(dx * dx + dy * dy) * w2_2) * norm;
        }
    }
}

//======================================================
//
//======================================================
void CFourierImage::MakeGradXGaussMask2D(float width, float Amp)
{
    ClearRe();
    ClearIm();
    float w2 = 1.0f / (width * width);
    float w2_2 = w2 / 2.0f;
    float norm = Amp / (2.0f * (float) PI * width * width);
    DINT lim = DINT(10.0 * width + 0.5) + 1;
    if (lim > X / 2)
        lim = X / 2;
    if (lim > Y / 2)
        lim = Y / 2;
    DINT x, y;
    for (x = 1; x < lim; ++x)
    {
        float dx = (float) x;
        float dx2 = dx * dx;
        float _norm = -norm * dx * w2;
        float v = exp(-dx2 * w2_2) * _norm;
        _Re(x, 0) = v;
        _Re(X - x, 0) = -v;
        for (y = 1; y < lim; ++y)
        {
            float dy = (float) y;
            float v = exp(-(dx2 + dy * dy) * w2_2) * _norm;
            _Re(x, y) = v;
            _Re(x, Y - y) = v;

            _Re(X - x, y) = -v;
            _Re(X - x, Y - y) = -v;
        }
    }
}

//======================================================
//
//======================================================
void CFourierImage::MakeGradYGaussMask2D(float width, float Amp)
{
    ClearRe();
    ClearIm();
    float w2 = 1.0f / (width * width);
    float w2_2 = w2 / 2.0f;
    float norm = Amp / (2.0f * (float) PI * width * width);
    DINT lim = DINT(10.0 * width + 0.5) + 1;
    if (lim > X / 2)
        lim = X / 2;
    if (lim > Y / 2)
        lim = Y / 2;
    DINT x, y;
    for (y = 1; y < lim; ++y)
    {
        float dy = (float) y;
        float dy2 = dy * dy;
        float _norm = -norm * dy * w2;
        float v = exp(-dy2 * w2_2) * _norm;
        _Re(0, y) = v;
        _Re(0, Y - y) = -v;
        for (x = 1; x < lim; ++x)
        {
            float dx = (float) x;
            float v = exp(-(dx * dx + dy2) * w2_2) * _norm;
            _Re(x, y) = v;
            _Re(X - x, y) = v;

            _Re(x, Y - y) = -v;
            _Re(X - x, Y - y) = -v;
        }
    }

}

//======================================================
//
//======================================================
void CFourierImage::MakeLoGaussMask2D(float width, float Amp)
{
    ClearRe();
    ClearIm();
    float width2 = width * width;
    float w2_2 = 1.0f / (width2 * 2.0f);
    float norm = Amp / ((float) PI * width2 * width2);

    DINT lim = DINT(10.0f * width + 0.5f) + 1;
    DINT aX = Width();
    DINT aY = Height();
    if (lim > aX / 2)
        lim = aX / 2;
    if (lim > aY / 2)
        lim = aY / 2;
    DINT x, y;
    _Re(0, 0) = -norm;
    for (x = 1; x < lim; ++x)
    {
        double dx = (double) x;
        double v = (dx * dx) * w2_2;
        v = (v - 1.0f) * exp(-v) * norm;
        _Re(aX - x, 0) = _Re(x, 0) = (float) v;
    }
    for (y = 1; y < lim; ++y)
    {
        double dy = (double) y;
        double v = (dy * dy) * w2_2;
        v = (v - 1.0) * exp(-v) * norm;
        _Re(0, aY - y) = _Re(0, y) = (float) v;
    }
    for (x = 1; x < lim; ++x)
    {
        for (y = 1; y < lim; ++y)
        {
            double dx = (double) x;
            double dy = (double) y;
            double v = (dx * dx + dy * dy) * w2_2;
            v = (v - 1.0f) * exp(-v) * norm;
            _Re(aX - x, aY - y) = _Re(aX - x, y) = _Re(x, aY - y) = _Re(x, y) = (float) v;
        }
    }
/*
	aX -= 1;
	aY -= 1;
	for (x = 0; x < lim; ++x)
	{
		for (y = 0; y < lim; ++y)
		{
			float dx = (float)x + 0.5f;
			float dy = (float)y + 0.5f;
			float v = (dx * dx + dy * dy) * w2_2;
			v = (v - 1.0f) * exp(-v) * norm;
			_Re(aX - x, aY - y) += v;
			_Re(x, aY - y) += v;
			_Re(aX - x, y) += v;
			_Re(x, y) += v;
		}
	}
*/
}

//======================================================
//
//======================================================
void CFourierImage::MakeLoGaussMask2D(float width, float cX, float cY, float Amp)
{
    ClearRe();
    ClearIm();
    float width2 = width * width;
    float w2_2 = 1.0f / (width2 * 2.0f);
    float norm = 1.0f / (width2 * (float) PI * width2);
    DINT lim = DINT(10.0 * width + 0.5) + 1;
    DINT aX = Width();
    DINT aY = Height();
    if (lim > X / 2)
        lim = X / 2;
    if (lim > Y / 2)
        lim = Y / 2;
    DINT x, y;
    DINT min_x = max((DINT) 0, (DINT) cX - lim);
    DINT max_x = min(aX, (DINT) cX + lim);
    DINT min_y = max((DINT) 0, (DINT) cY - lim);
    DINT max_y = min(aY, (DINT) cY + lim);
    norm *= Amp;
    for (x = min_x; x < max_x; ++x)
    {
        float dx = (float) (x - cX);
        for (y = min_y; y < max_y; ++y)
        {
            float dy = (float) (y - cY);
            double v = (dx * dx + dy * dy) * w2_2;
            v = (v - 1.0f) * exp(-v) * norm;
            _Re(x, y) = (float) v;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
CMathImage::CMathImage(int x, int y)
{
    Buffer = 0;
    X = 0;
    Y = 0;
    fKill = true;
    Init(x, y, 0, true);
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
CMathImage::CMathImage(int *b, int x, int y)
{
    Buffer = 0;
    X = 0;
    Y = 0;
    fKill = true;
    Init(x, y, b, fKill);
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
CMathImage::CMathImage()
{
    Buffer = 0;
    X = 0;
    Y = 0;
    fKill = true;
}

CMathImage::CMathImage(CMathImage &im)
{
    Buffer = 0;
    X = 0;
    Y = 0;
    fKill = true;
    Init(im.X, im.Y, im.Buffer, true);

}

//====================================================================================
//
//====================================================================================
CMathImage::CMathImage(FMathImage *src)
{
    Buffer = 0;
    X = 0;
    Y = 0;
    fKill = true;


    Init(src->Width(), src->Height(), 0, fKill);
    DINT Len = X * Y;
    if (Buffer)
    {
        for (DINT i = 0; i < Len; ++i)
            Buffer[i] = (INT32) src->Data()[i];
    } else
    {
        X = 0;
        Y = 0;
    }
}

CMathImage &CMathImage::operator=(CMathImage &im)
{
    Init(im.X, im.Y, im.Buffer, true);
    return *this;

}

void CMathImage::SetSize_0(int aX, int aY)
{
    if (!fKill && Buffer)
    {
        Buffer = 0;
        X = 0;
        Y = 0;
        fKill = true;
        Init(aX, aY, 0, fKill);
    } else
    {
        INT32 *aBuffer = (INT32 *) (Buffer ? HREALLOC(Buffer, aX * aY * sizeof(INT32)) : HMALLOCZ(
                aX * aY * sizeof(int)));
        if (aBuffer)
        {
            Buffer = aBuffer;
            X = aX;
            Y = aY;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
void CMathImage::Init(int x, int y, int *b, bool _fKill)
{
    if (fKill && Buffer) HFREE(Buffer);
    if (!_fKill && b)
    {
        Buffer = b;
        X = x;
        Y = y;
        fKill = FALSE;
    } else
    {
        Buffer = (int *) HMALLOC(x * y * sizeof(int));
        if (!Buffer)
        {
            X = 0;
            Y = 0;
            fKill = true;
        } else
        {
            if (b)
                memcpy(Buffer, b, x * y * sizeof(int));
            else
                memset(Buffer, 0, x * y * sizeof(int));
            X = x;
            Y = y;
            fKill = _fKill;
        }


    }
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
double CMathImage::FindThresholdByMaxEntropy(CMathImage &Mask, BOOL SumOfEntropy, BOOL VarEnt)
{
    double Threshold = 0.0;
    double *var = new double[X * Y];
    if (var)
    {
        memset(var, 0, X * Y * sizeof(double));
        int *Mask_Buffer = Mask.Buffer;
        int Lim_Y = X * (Y - 1);
        double Min = 1e30;
        double Max = -1e30;
        for (int y = 0, off_y = 0; y < Y; ++y, off_y += X)
        {
            for (int x = 0; x < X; ++x)
            {
                if (Mask_Buffer[x + off_y] == 0)
                    continue;
                int count = 0;
                double v = 0.0;
                double v2 = 0.0;
                double a = Buffer[x + off_y];
                if (Min > a)
                    Min = a;
                if (Max < a)
                    Max = a;
                for (int k = max(0, off_y - X), lim_k = min(Lim_Y, off_y + X); k <= lim_k; k += X)
                {
                    for (int m = max(0, x - 1), lim_m = min(X - 1, x + 1); m <= lim_m; ++m)
                    {
                        ++count;
                        a = Buffer[m + k];
                        v += a;
                        v2 += a * a;
                    }
                }
                v /= count;
                v2 /= count;
                int pos = x + off_y;
                var[pos] = v2 - v * v;
                if (var[pos] < 0.0)
                    var[pos] = 0.0;
                else
                    var[pos] = sqrt(var[pos]);
            }
        }
        if (Max > Min)
        {
            GParam param;
            param.Img = this;
            param.Var = var;
            param.Mask = &Mask;
            param.SumOfEntropy = SumOfEntropy;
            param.VarEnt = VarEnt;
            double Boundaries[2];
            Boundaries[0] = Min;
            Boundaries[1] = Max;
            GoldSection_Double gs(ThresholdedImageEntropyWithMask, &param, Boundaries, 1.0e-5);
            Threshold = gs.Solve();
        } else if (Max == Min)
            Threshold = Max;
        else
            Threshold = 0.0;
    }
    delete[] var;
    return Threshold;
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
double
CMathImage::FindThresholdByMaxEntropy(CMathImage &Mask, double UpperThreshold, BOOL SumOfEntropy,
                                      BOOL VarEnt)
{
    double Threshold = 0.0;
    double *var = new double[X * Y];
    if (var)
    {
        memset(var, 0, X * Y * sizeof(double));
        int *Mask_Buffer = Mask.Buffer;
        int Lim_Y = X * (Y - 1);
        double Min = 1e30;
        double Max = -1e30;
        for (int y = 0, off_y = 0; y < Y; ++y, off_y += X)
        {
            for (int x = 0; x < X; ++x)
            {
                if (Mask_Buffer[x + off_y] == 0)
                    continue;
                int count = 0;
                double v = 0.0;
                double v2 = 0.0;
                double a = Buffer[x + off_y];
                if (a < UpperThreshold)
                {
                    if (Min > a)
                        Min = a;
                    if (Max < a)
                        Max = a;
                    for (int k = max(0, off_y - X), lim_k = min(Lim_Y, off_y + X);
                         k <= lim_k; k += X)
                    {
                        for (int m = max(0, x - 1), lim_m = min(X - 1, x + 1); m <= lim_m; ++m)
                        {
                            ++count;
                            a = Buffer[m + k];
                            v += a;
                            v2 += a * a;
                        }
                    }
                    v /= count;
                    v2 /= count;
                    int pos = x + off_y;
                    var[pos] = v2 - v * v;
                    if (var[pos] < 0.0)
                        var[pos] = 0.0;
                    else
                        var[pos] = sqrt(var[pos]);
                }
            }
        }
        if (Max > Min)
        {
            GParam param;
            param.Img = this;
            param.Var = var;
            param.Mask = &Mask;
            param.SumOfEntropy = SumOfEntropy;
            param.VarEnt = VarEnt;
            param.UpperThreshold = UpperThreshold;
            double Boundaries[2];
            Boundaries[0] = Min;
            Boundaries[1] = Max;
            GoldSection_Double gs(ThresholdedImageEntropyWithMask1, &param, Boundaries, 1.0e-5);
            Threshold = gs.Solve();
        } else if (Max == Min)
            Threshold = Max;
        else
            Threshold = 0.0;
    }
    delete[] var;
    return Threshold;
}

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------

//======================================================
//
//======================================================
CMathImage *CMathImage::GetPotentialBorderMask(float width, FMathImage *&Grad2Mask, float Scale,
                                               BOOL UseLaplace)
{
    if (Grad2Mask && (Grad2Mask->Width() != X || Grad2Mask->Height() != Y))
    {
        delete Grad2Mask;
        Grad2Mask = 0;
    }
    if (Grad2Mask == NULL)
        Grad2Mask = new FMathImage(X, Y);
    else
        Grad2Mask->Clear();

    CMathImage *Mask = new CMathImage(X, Y);
    FMathImage GradX(*this);
    FMathImage GradY(*this);
    FMathImage Laplace(*this);
    if (Mask && GradX.IsValid() && GradY.IsValid() && (!UseLaplace || Laplace.IsValid()))
    {
        GradX.ConvolveWithGradXOfGauss((float) width);
        GradY.ConvolveWithGradYOfGauss((float) width);
        if (UseLaplace)
            Laplace.ConvolveWithLaplacianOfGauss((float) width);
        _GetPotentialBorderMask(Mask, *Grad2Mask, GradX, GradY, Laplace, Scale, UseLaplace);

    }
    return Mask;
}

//======================================================
//
//======================================================
void CMathImage::_GetPotentialBorderMask(CMathImage *Mask, FMathImage &Grad2Mask, FMathImage &GradX,
                                         FMathImage &GradY, FMathImage &Laplace, float Scale,
                                         BOOL UseLaplace)
{
    DINT x, y, ly;
    DINT XY = Mask->X * Mask->Y;
    DINT limX = Mask->X - 1;
    DINT limY = Mask->Y - 1;
    if (UseLaplace)
    {
        for (ly = Mask->X, y = 1; y < limY; ly += Mask->X, ++y)
        {
            for (x = 1; x < limX; ++x)
            {
                DINT pos;
                float _gx = GradX.GetValue(x, y, pos);
                float _gy = GradY.Data()[pos];
                float dg = Laplace.Data()[pos];
                float s0 = _gx * _gx + _gy * _gy;

                if ((Grad2Mask.Data()[pos] = sqrt(s0) * Scale + 0.5f) > 1.0f)
                {
                    if (dg == 0.0f)
                        Mask->Buffer[pos] = -1;
                    else if (dg < 0.0f)
                    {
                        for (int i = -1; i <= 1; ++i)
                        {
                            for (int j = -1; j <= 1; ++j)
                            {
                                float _dg = Laplace.GetValue(x + i, y + j);
                                if (dg * _dg < 0.0f)
                                {
                                    Mask->Buffer[pos] = -1;
                                    j = i = 2;
                                }
                            }
                        }
                    }
                }
            }
        }
    } else
    {
        for (ly = Mask->X, y = 1; y < limY; ly += Mask->X, ++y)
        {
            for (x = 1; x < limX; ++x)
            {
                float s[9];
                float gx[9];
                float gy[9];
                for (int j = -1; j <= 1; ++j)
                {
                    int line = 3 * (j + 1);
                    for (int i = -1; i <= 1; ++i)
                    {
                        DINT index;
                        float _gx = GradX.GetValue(x + i, y + j, index);
                        float _gy = GradY.Data()[index];
                        float _s = _gx * _gx + _gy * _gy;
                        int p = (i + 1) + line;
                        s[p] = _s;
                        gx[p] = _gx;
                        gy[p] = _gy;
                    }
                }
                DINT pos = x + ly;
                if ((Grad2Mask.Data()[pos] = sqrt(s[4]) * Scale + 0.5f) > 1.0f)
                {
                    float nx2 = gx[4] * gx[4] / s[4];
                    float ny2 = gy[4] * gy[4] / s[4];
                    if (ny2 <= 0.25f)
                    {
                        float s0 = s[4];
                        if (s0 >= s[3] && s0 >= s[5])
                            Mask->Buffer[pos] = -1;
                    } else if (nx2 <= 0.25f)
                    {
                        float s0 = s[4];
                        if (s0 >= s[1] && s0 >= s[7])
                            Mask->Buffer[pos] = -1;
                    } else if (gx[4] * gy[4] > 0.0)
                    {
                        float s0 = s[4];
                        if (s0 >= s[0] && s0 >= s[8])
                            Mask->Buffer[pos] = -1;
                    } else
                    {
                        float s0 = s[4];
                        if (s0 >= s[2] && s0 >= s[6])
                            Mask->Buffer[pos] = -1;
                    }
                }
            }
        }
    }
}

//====================================================================================
//
//====================================================================================
void CMathImage::ToBilevel(INT32 Threshold, BOOL Unsigned)
{
    INT32 mask = Unsigned ? 0xffffffff : 0x7fffffff;
    for (DINT j = 0; j < Y; ++j)
    {
        for (DINT i = 0; i < X; ++i)
        {
            int &v = (*this)(i, j);
            if (v <= Threshold)
                v = 0;
            else
                v = mask;
        }
    }
}

//======================================================
//
//======================================================
CMathImage *CMathImage::FilterPotentialBorders(FMathImage &Grad2Mask, float &UpperThreshold,
                                               float &LowThreshold, BOOL SumEntropy)
{
    CMathImage temp1(&Grad2Mask);
    UpperThreshold = temp1.FindThresholdByMaxEntropy(*this, SumEntropy, FALSE);
    CMathImage temp(*this);
    temp.ToBilevel((INT32) (UpperThreshold + 0.5f));
    LowThreshold = temp1.FindThresholdByMaxEntropy(temp, SumEntropy, FALSE) / 2.0f;
    CMathImage *Mask = new CMathImage(X, Y);
    if (Mask && Mask->IsValid())
    {
        DINT x, y, ly;
        DINT XY = X * Y;
        DINT limX = X - 1;
        DINT limY = Y - 1;
        for (x = 0; x < X; ++x)
            Mask->Buffer[XY - x - 1] = Mask->Buffer[x] = -1;

        for (y = 0; y < XY; y += X)
            Mask->Buffer[y] = Mask->Buffer[y + limX] = -1;

        for (ly = X, y = 1; y < limY; ly += X, ++y)
        {
            for (x = 1; x < limX; ++x)
            {
                DINT pos = x + ly;
                if (Buffer[pos] && temp1.Buffer[pos] > UpperThreshold)
                    Mask->Buffer[pos] = -1;
            }
        }
        for (BOOL rep = TRUE; rep;)
        {
            rep = FALSE;
            for (ly = X, y = 1; y < limY; ly += X, ++y)
            {
                for (x = 1; x < limX; ++x)
                {
                    DINT pos = x + ly;
                    if (Buffer[pos] && !Mask->Buffer[pos] && temp1.Buffer[pos] > LowThreshold)
                    {
                        BOOL Stop = FALSE;
                        for (DINT i = -1; !Stop && i <= 1; ++i)
                        {
                            for (DINT j = -1; j <= 1; ++j)
                            {
                                if (Mask->Buffer[pos + i + X * j])
                                {
                                    Stop = rep = TRUE;
                                    Mask->Buffer[pos] = -1;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return Mask;
}

//======================================================
//
//======================================================
CMathImage *
CMathImage::GetPotentialBorderMaskByPyrmid(FMathImage *&Grad2Mask, float KernelWidth, int MaxN,
                                           float Scale)
{
    if (Grad2Mask && (Grad2Mask->Width() != X || Grad2Mask->Height() != Y))
    {
        delete Grad2Mask;
        Grad2Mask = 0;
    }
    if (Grad2Mask == NULL)
        Grad2Mask = new FMathImage(X, Y);
    else
        Grad2Mask->Clear();

    CMathImage *Mask = new CMathImage(X, Y);

    if (!Grad2Mask || !Mask)
        return NULL;

    DINT L = min(X, Y);
    int n = 1;
    while (L > 32)
    {
        L /= 2;
        ++n;
    }
    n = min(n, MaxN);
    int i;
    int denom = 1;
    FMathImage **GradX = (FMathImage **) _alloca(sizeof(FMathImage *) * n);
    FMathImage **GradY = (FMathImage **) _alloca(sizeof(FMathImage *) * n);
    memset(GradX, 0, sizeof(FMathImage *) * n);
    memset(GradY, 0, sizeof(FMathImage *) * n);
    FMathImage image(*this);
    for (i = 0; i < n; ++i)
    {
        if (i > 0)
            image.Squeeze4();
        GradX[i] = new FMathImage(image);
        GradY[i] = new FMathImage(image);
        if (GradX[i] && GradY[i] && GradX[i]->IsValid() && GradY[i]->IsValid())
        {
            GradX[i]->ConvolveWithGradXOfGauss(KernelWidth);
            GradY[i]->ConvolveWithGradYOfGauss(KernelWidth);
        }
    }
    _GetPotentialBorderMaskByPyramid(Mask, *Grad2Mask, GradX, GradY, n, Scale);
    for (i = 0; i < n; ++i)
    {
        delete GradX[i];
        delete GradY[i];
    }
    return Mask;
}

//======================================================
//
//======================================================
void CMathImage::_GetPotentialBorderMaskByPyramid(CMathImage *Mask, FMathImage &Grad2Mask,
                                                  FMathImage **GradX, FMathImage **GradY, int N,
                                                  float Scale)
{
    DINT x, y, ly;
    DINT XY = Mask->X * Mask->Y;
    DINT limX = Mask->X - 1;
    DINT limY = Mask->Y - 1;
    for (ly = Mask->X, y = 1; y < limY; ly += Mask->X, ++y)
    {
        for (x = 1; x < limX; ++x)
        {
            float s[9];
            float gx[9];
            float gy[9];
            memset(s, 0, sizeof(float) * 9);
            memset(gx, 0, sizeof(float) * 9);
            memset(gy, 0, sizeof(float) * 9);
            DINT pos = x + ly;
            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    float _gx = 0.0f;
                    float _gy = 0.0f;
                    float gx0 = 0.0f;
                    float gy0 = 0.0f;
                    for (int k = 0, denom = 1; k < N; ++k, denom *= 2)
                    {
                        if (GradX[k] && GradY[k])
                        {
                            float __gx, __gy;
                            if (denom == 1)
                            {
                                DINT index;
                                gx0 = __gx = GradX[k]->GetValue(x + i, y + j, index);
                                gy0 = __gy = GradY[k]->Data()[index];
                            } else
                            {
                                float _x = float(x) / denom + i;
                                float _y = float(y) / denom + j;
                                __gx = GradX[k]->GetBiLineValue(_x, _y);
                                __gy = GradY[k]->GetBiLineValue(_x, _y);
                                __gx = gx0 + (__gx - gx0) / denom;
                                __gy = gy0 + (__gy - gy0) / denom;
                            }
                            float norm = sqrt((float) denom);
                            _gx += __gx / norm;
                            _gy += __gy / norm;
                        }
                    }
                    float _s = _gx * _gx + _gy * _gy;
                    s[(i + 1) + 3 * (j + 1)] = _s;
                    gx[(i + 1) + 3 * (j + 1)] = _gx;
                    gy[(i + 1) + 3 * (j + 1)] = _gy;
                }
            }
            if ((Grad2Mask.Data()[pos] = sqrt(s[4]) * Scale + 0.5f) > 1.0f)
            {
                float nx = gx[4] * gx[4] / s[4];
                float ny = gy[4] * gy[4] / s[4];
                if (ny <= 0.25f)
                {
                    float s0 = s[4];
                    if (s0 >= s[3] && s0 >= s[5])
                        Mask->Buffer[pos] = -1;
                } else if (nx <= 0.25f)
                {
                    float s0 = s[4];
                    if (s0 >= s[1] && s0 >= s[7])
                        Mask->Buffer[pos] = -1;
                } else if (gx[4] * gy[4] > 0.0)
                {
                    float s0 = s[4];
                    if (s0 >= s[0] && s0 >= s[8])
                        Mask->Buffer[pos] = -1;
                } else
                {
                    float s0 = s[4];
                    if (s0 >= s[2] && s0 >= s[6])
                        Mask->Buffer[pos] = -1;
                }
            }
        }
    }
}


void CMathImage::ConvertRGB_to_BSH(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b,
                                   FMathImage &image_Amp, FMathImage &image_Sat,
                                   FMathImage &image_Hue)
{
    DINT x, y, line, pos;
    float PI2 = (float) (2.0 * PI);
    float center = 1.0f / 3.0f;
    float v1[3] = {2.0f / sqrt(6.0f), -1.0f / sqrt(6.0f), -1.0f / sqrt(6.0f)};
    for (y = 0, line = 0; y < img_r.Y; ++y, line += img_r.X)
    {
        for (x = 0; x < img_r.X; ++x)
        {
            pos = line + x;
            float r = (float) img_r.Buffer[pos];
            float g = (float) img_g.Buffer[pos];
            float b = (float) img_b.Buffer[pos];
            float amp = sqrt(r * r + g * g + b * b);
            image_Amp.Data()[pos] = amp;

            float rgb = r + g + b;
            if (rgb == 0.0f)
            {
                image_Sat.Data()[pos] = 0.0f;
                image_Hue.Data()[pos] = 0.0f;
            } else
            {
                float amp = image_Amp.Data()[pos];
                float s_r = (r / rgb - center);
                float s_g = (g / rgb - center);
                float s_b = (b / rgb - center);
                float sat = sqrt(s_r * s_r + s_g * s_g + s_b * s_b);
                image_Sat.Data()[pos] = sat;
                if (sat > 0.0)
                {
                    float cosa = (v1[0] * s_r + v1[1] * s_g + v1[2] * s_b) / sat;
                    if (cosa >= 1.0f)
                        cosa = 0.9999f;
                    else if (cosa <= -1.0f)
                        cosa = -0.9999f;
                    if (s_b > s_g)
                        image_Hue.Data()[pos] = acos(cosa);
                    else
                        image_Hue.Data()[pos] = (PI2 - acos(cosa));
                } else
                    image_Hue.Data()[pos] = 0.0f;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
struct ThreadConvertCMath3_to_FMath3 {
    CMathImage *cImage[3];
    FMathImage *fImage[3];
    DINT start;
    DINT finish;
};

//===================================================================
//
//===================================================================
UINT AFX_CDECL ThreadsConvertRGB_to_BCY(void *Param)
{
    ThreadConvertCMath3_to_FMath3 *d = (ThreadConvertCMath3_to_FMath3 *) Param;
    CMathImage::_ConvertRGB_to_BCY(*d->cImage[0], *d->cImage[1], *d->cImage[2], *d->fImage[0],
                                   *d->fImage[1], *d->fImage[2], d->start, d->finish);
    return 0;
}

//======================================================
//
//======================================================
void CMathImage::ConvertRGB_to_BCY(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b,
                                   FMathImage &image_Amp, FMathImage &image_C, FMathImage &image_Y)
{
    DINT Len = img_r.X * img_r.Y;
    int NThreads = pMultiThreads->Num();
    if (NThreads > 1)
    {
        ThreadConvertCMath3_to_FMath3 *tParam = (ThreadConvertCMath3_to_FMath3 *) _alloca(
                sizeof(ThreadConvertCMath3_to_FMath3) * (NThreads + 1));
        DINT step = Len / NThreads;
        tParam[0].start = 0;
        tParam[0].finish = step;
        tParam[0].cImage[0] = &img_r;
        tParam[0].cImage[1] = &img_g;
        tParam[0].cImage[2] = &img_b;
        tParam[0].fImage[0] = &image_Amp;
        tParam[0].fImage[1] = &image_C;
        tParam[0].fImage[2] = &image_Y;
        int m;
        for (m = 1; m < NThreads && tParam[m - 1].finish < Len; ++m)
        {
            memcpy(tParam + m, tParam, sizeof(ThreadConvertCMath3_to_FMath3));
            tParam[m].start = tParam[m - 1].finish;
            tParam[m].finish = tParam[m].start + step;
        }
        NThreads = m;
        tParam[NThreads - 1].finish = Len;

        for (m = 0; m < NThreads; ++m)
            pMultiThreads->Run(m, ThreadsConvertRGB_to_BCY, tParam + m);
        pMultiThreads->Wait();
    } else
        _ConvertRGB_to_BCY(img_r, img_g, img_b, image_Amp, image_C, image_Y, 0, Len);
}

//======================================================
//
//======================================================
void CMathImage::_ConvertRGB_to_BCY(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b,
                                    FMathImage &image_Amp, FMathImage &image_C, FMathImage &image_Y,
                                    DINT start, DINT finish)
{
    DINT pos;
    DINT Len = img_r.X * img_r.Y;
    for (pos = start; pos < finish; ++pos)
    {
        float r = (float) img_r.Buffer[pos];
        float g = (float) img_g.Buffer[pos];
        float b = (float) img_b.Buffer[pos];
        float amp = sqrt(r * r + g * g + b * b);
        image_Amp.Data()[pos] = amp;
        float rgb = r + g + b;
        if (rgb == 0.0f)
        {
            image_C.Data()[pos] = 0.816497f;
            image_Y.Data()[pos] = 0.816497f;
        } else
        {
            float _r = r / rgb;
            float _g = g / rgb;
            float _b = b / rgb;
            image_C.Data()[pos] = sqrt((1.0f - _r) * (1.0f - _r) + _g * _g + _b * _b);
            image_Y.Data()[pos] = sqrt(_r * _r + _g * _g + (1.0f - _b) * (1.0f - _b));
        }
    }
}

//===================================================================
//
//===================================================================
UINT AFX_CDECL ThreadsConvertBCY_to_RGB(void *Param)
{
    ThreadConvertCMath3_to_FMath3 *d = (ThreadConvertCMath3_to_FMath3 *) Param;
    CMathImage::_ConvertBCY_to_RGB(*d->fImage[0], *d->fImage[1], *d->fImage[2], *d->cImage[0],
                                   *d->cImage[1], *d->cImage[2], d->start, d->finish);
    return 0;
}

//======================================================
//
//======================================================
void CMathImage::ConvertBCY_to_RGB(FMathImage &image_Amp, FMathImage &image_C, FMathImage &image_Y,
                                   CMathImage &img_r, CMathImage &img_g, CMathImage &img_b)
{
    DINT Len = img_r.X * img_r.Y;
    int NThreads = pMultiThreads->Num();
    if (NThreads > 1)
    {
        ThreadConvertCMath3_to_FMath3 *tParam = (ThreadConvertCMath3_to_FMath3 *) _alloca(
                sizeof(ThreadConvertCMath3_to_FMath3) * (NThreads + 1));
        DINT step = Len / NThreads;
        tParam[0].start = 0;
        tParam[0].finish = step;
        tParam[0].cImage[0] = &img_r;
        tParam[0].cImage[1] = &img_g;
        tParam[0].cImage[2] = &img_b;
        tParam[0].fImage[0] = &image_Amp;
        tParam[0].fImage[1] = &image_C;
        tParam[0].fImage[2] = &image_Y;
        int m;
        for (m = 1; m < NThreads && tParam[m - 1].finish < Len; ++m)
        {
            memcpy(tParam + m, tParam, sizeof(ThreadConvertCMath3_to_FMath3));
            tParam[m].start = tParam[m - 1].finish;
            tParam[m].finish = tParam[m].start + step;
        }
        NThreads = m;
        tParam[NThreads - 1].finish = Len;

        for (m = 0; m < NThreads; ++m)
            pMultiThreads->Run(m, ThreadsConvertBCY_to_RGB, tParam + m);
        pMultiThreads->Wait();
    } else
        _ConvertBCY_to_RGB(image_Amp, image_C, image_Y, img_r, img_g, img_b, 0, Len);
}

//======================================================
//
//======================================================
void CMathImage::_ConvertBCY_to_RGB(FMathImage &image_Amp, FMathImage &image_C, FMathImage &image_Y,
                                    CMathImage &img_r, CMathImage &img_g, CMathImage &img_b,
                                    DINT start, DINT finish)
{
    DINT pos;
    for (pos = start; pos < finish; ++pos)
    {
        float Amp = (float) image_Amp.Data()[pos];
        float C = (float) image_C.Data()[pos];
        float Y = (float) image_Y.Data()[pos];
        if (Amp == 0.0f)
        {
            img_r.Buffer[pos] = 0;
            img_g.Buffer[pos] = 0;
            img_b.Buffer[pos] = 0;
        } else
        {
            float A = (Y * Y - C * C) / 2.0f;
            float alpha = 3.0f;
            float beta = 3.0f * (A - 1.0f);
            float gamma = 1.0f + A * A - A - Y * Y / 2.0f;
            float _b = (-beta - sqrt(beta * beta - 4.0f * alpha * gamma)) / (2.0f * alpha);
            float _r = A + _b;
            float _g = 1.0f - _r - _b;
            float a2 = _r * _r + _g * _g + _b * _b;
            float q = Amp / sqrt(a2);

            float r = _r * q;
            float g = _g * q;
            float b = _b * q;
            float max_comp = max(max(r, g), b);
            if (max_comp > 254.0f)
            {
                float s = 254.0f / max_comp;
                r *= s;
                g *= s;
                b *= s;
            }
            img_r.Buffer[pos] = (INT32) r;
            img_g.Buffer[pos] = (INT32) g;
            img_b.Buffer[pos] = (INT32) b;
        }
    }
}

//======================================================
//
//======================================================
void CMathImage::ConvertRGB_to_NRGB(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b,
                                    CMathImage &image_r, CMathImage &image_g, CMathImage &image_b,
                                    CMathImage &image_amp)
{
    DINT x, y, line, pos;
    float center = 1.0f / 3.0f;
    for (y = 0, line = 0; y < img_r.Y; ++y, line += img_r.X)
    {
        for (x = 0; x < img_r.X; ++x)
        {
            pos = line + x;
            float r = (float) img_r.Buffer[pos];
            float g = (float) img_g.Buffer[pos];
            float b = (float) img_b.Buffer[pos];
            float amp = sqrt(r * r + g * g + b * b);
            image_amp.Buffer[pos] = (int) (amp);
            if (amp == 0.0f)
            {
                image_r.Buffer[pos] = 0;
                image_g.Buffer[pos] = 0;
                image_b.Buffer[pos] = 0;
            } else
            {
                image_r.Buffer[pos] = (INT32) (255.0f * r / amp);
                image_g.Buffer[pos] = (INT32) (255.0f * g / amp);
                image_b.Buffer[pos] = (INT32) (255.0f * b / amp);
            }
        }
    }
}

//======================================================
//
//======================================================
void CMathImage::ConvertNRGB_to_RGB(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b,
                                    CMathImage &img_amp, CMathImage &image_r, CMathImage &image_g,
                                    CMathImage &image_b)
{
    DINT x, y, line, pos;
    float center = 1.0f / 3.0f;
    for (y = 0, line = 0; y < img_r.Y; ++y, line += img_r.X)
    {
        for (x = 0; x < img_r.X; ++x)
        {
            pos = line + x;
            float r = (float) img_r.Buffer[pos];
            float g = (float) img_g.Buffer[pos];
            float b = (float) img_b.Buffer[pos];
            float amp = (float) img_amp.Buffer[pos];
            if (amp == 0.0f)
            {
                image_r.Buffer[pos] = 0;
                image_g.Buffer[pos] = 0;
                image_b.Buffer[pos] = 0;
            } else
            {
                image_r.Buffer[pos] = (INT32) (r * amp / 255.0f);
                image_g.Buffer[pos] = (INT32) (g * amp / 255.0f);
                image_b.Buffer[pos] = (INT32) (b * amp / 255.0f);
            }
        }
    }
}

//======================================================
//
//======================================================
void CMathImage::ConvertRGB_to_HSI(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b,
                                   FMathImage &image_Hue, FMathImage &image_Sat,
                                   FMathImage &image_Amp)
{
    DINT x, y, line, pos;
    float PI2 = (float) (2.0 * PI);
    for (y = 0, line = 0; y < img_r.Y; ++y, line += img_r.X)
    {
        for (x = 0; x < img_r.X; ++x)
        {
            pos = line + x;
            float r = (float) img_r.Buffer[pos];
            float g = (float) img_g.Buffer[pos];
            float b = (float) img_b.Buffer[pos];
            float amp = (r + g + b) / 3.0f;
            image_Amp.Data()[pos] = amp;

            if (amp == 0.0f)
            {
                image_Sat.Data()[pos] = 0.0f;
                image_Hue.Data()[pos] = 0.0f;
            } else
            {
                float sat = 1.0f - min(r, min(g, b)) / amp;
                image_Sat.Data()[pos] = sat;
                if (sat > 0.0)
                {
                    float denom = sqrt((r - g) * (r - g) + (r - b) * (g - b));
                    float cosa = (r - (g + b) * 0.5f) / denom;
                    if (cosa >= 1.0)
                        cosa = 0.9999f;
                    else if (cosa <= -1.0f)
                        cosa = -0.9999f;
                    if (b <= g)
                        image_Hue.Data()[pos] = acos(cosa);
                    else
                        image_Hue.Data()[pos] = (PI2 - acos(cosa));
                } else
                    image_Hue.Data()[pos] = 0.0f;
            }
        }
    }
}

//======================================================
//
//======================================================
void
CMathImage::ConvertHSI_to_RGB(FMathImage &image_Hue, FMathImage &image_Sat, FMathImage &image_Amp,
                              CMathImage &img_r, CMathImage &img_g, CMathImage &img_b)
{
    DINT x, y, line, pos;
    float PI2 = (float) (2.0 * PI);
    float PI_3 = (float) PI / 3.0f;
    float PI2_3 = PI2 / 3.0f;
    float PI4_3 = PI2_3 * 2.0f;
    for (y = 0, line = 0; y < img_r.Y; ++y, line += img_r.X)
    {
        for (x = 0; x < img_r.X; ++x)
        {
            pos = line + x;
            float amp = image_Amp.Data()[pos];
            float sat = image_Sat.Data()[pos];
            float hue = image_Hue.Data()[pos];
            float r, g, b;
            if (hue >= 0 && hue < PI2_3)
            {
                r = amp * (1.0f + sat * cos(hue) / cos(PI_3 - hue));
                b = amp * (1.0f - sat);
                g = 3.0f * amp - r - b;
            } else if (hue >= PI2_3 && hue < PI4_3)
            {
                float h = hue - PI2_3;
                r = amp * (1.0f - sat);
                g = amp * (1.0f + sat * cos(h) / cos(PI_3 - h));
                b = 3.0f * amp - r - g;
            } else
            {
                float h = hue - PI4_3;
                g = amp * (1.0f - sat);
                b = amp * (1.0f + sat * cos(h) / cos(PI_3 - h));
                r = 3.0f * amp - r - b;
            }

            img_r.Buffer[pos] = (INT32) r;
            img_g.Buffer[pos] = (INT32) g;
            img_b.Buffer[pos] = (INT32) b;
        }
    }
}

//======================================================
//
//======================================================
void CMathImage::ConvertRGB_to_XYZ(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b,
                                   FMathImage &image_X, FMathImage &image_Y, FMathImage &image_Z)
{
    DINT x, y, line, pos;
    for (y = 0, line = 0; y < img_r.Y; ++y, line += img_r.X)
    {
        for (x = 0; x < img_r.X; ++x)
        {
            pos = line + x;

            float var_R = (float) img_r.Buffer[pos] / 255.0f;        //R from 0 to 255
            float var_G = (float) img_g.Buffer[pos] / 255.0f;        //G from 0 to 255
            float var_B = (float) img_b.Buffer[pos] / 255.0f;        //B from 0 to 255

            if (var_R > 0.04045f)
                var_R = pow((var_R + 0.055f) / 1.055f, 2.4f);
            else
                var_R /= 12.92f;
            if (var_G > 0.04045f)
                var_G = pow((var_G + 0.055f) / 1.055f, 2.4f);
            else
                var_G /= 12.92f;
            if (var_B > 0.04045f)
                var_B = pow((var_B + 0.055f) / 1.055f, 2.4f);
            else
                var_B /= 12.92f;

            var_R *= 100.0f;
            var_G *= 100.0f;
            var_B *= 100.0f;

            //Observer. = 2\B0, Illuminant = D65
            image_X.Data()[pos] = var_R * 0.4124f + var_G * 0.3576f + var_B * 0.1805f;
            image_Y.Data()[pos] = var_R * 0.2126f + var_G * 0.7152f + var_B * 0.0722f;
            image_Z.Data()[pos] = var_R * 0.0193f + var_G * 0.1192f + var_B * 0.9505f;
        }
    }
}

//======================================================
//
//======================================================
void CMathImage::ConvertXYZ_to_RGB(FMathImage &image_X, FMathImage &image_Y, FMathImage &image_Z,
                                   CMathImage &img_r, CMathImage &img_g, CMathImage &img_b)
{
    DINT x, y, line, pos;
    for (y = 0, line = 0; y < img_r.Y; ++y, line += img_r.X)
    {
        for (x = 0; x < img_r.X; ++x)
        {
            pos = line + x;

            float var_X = image_X.Data()[pos] /
                          100.0f;        //X from 0 to  95.047      (Observer = 2\B0, Illuminant = D65)
            float var_Y = image_Y.Data()[pos] / 100.0f;        //Y from 0 to 100.000
            float var_Z = image_Z.Data()[pos] / 100.0f;        //Z from 0 to 108.883

            float var_R = var_X * 3.2406f + var_Y * -1.5372f + var_Z * -0.4986f;
            float var_G = var_X * -0.9689f + var_Y * 1.8758f + var_Z * 0.0415f;
            float var_B = var_X * 0.0557f + var_Y * -0.2040f + var_Z * 1.0570f;

            if (var_R > 0.0031308f)
                var_R = 1.055f * pow(var_R, 1.0f / 2.4f) - 0.055f;
            else
                var_R *= 12.92f;
            if (var_G > 0.0031308f)
                var_G = 1.055f * pow(var_G, 1.0f / 2.4f) - 0.055f;
            else
                var_G *= 12.92f;
            if (var_B > 0.0031308f)
                var_B = 1.055f * pow(var_B, 1.0f / 2.4f) - 0.055f;
            else
                var_B *= 12.92f;

            img_r.Buffer[pos] = INT32(var_R * 255.0f);
            img_g.Buffer[pos] = INT32(var_G * 255.0f);
            img_b.Buffer[pos] = INT32(var_B * 255.0f);
        }
    }
}

//======================================================
//
//======================================================
void CMathImage::ConvertXYZ_to_CIELab(FMathImage &image_X, FMathImage &image_Y, FMathImage &image_Z,
                                      FMathImage &image_L, FMathImage &image_a, FMathImage &image_b)
{
    DINT x, y, line, pos;
    const float ref_X = 95.047f;
    const float ref_Y = 100.0f;
    const float ref_Z = 108.883f;
    for (y = 0, line = 0; y < image_X.Height(); ++y, line += image_X.Width())
    {
        for (x = 0; x < image_X.Width(); ++x)
        {
            pos = line + x;

            float var_X = image_X.Data()[pos] / ref_X;
            float var_Y = image_Y.Data()[pos] / ref_Y;
            float var_Z = image_Z.Data()[pos] / ref_Z;

            if (var_X > 0.008856f)
                var_X = pow(var_X, 1.0f / 3.0f);
            else
                var_X = (7.787f * var_X) + (16.0f / 116.0f);
            if (var_Y > 0.008856f)
                var_Y = pow(var_Y, 1.0f / 3.0f);
            else
                var_Y = (7.787f * var_Y) + (16.0f / 116.0f);
            if (var_Z > 0.008856f)
                var_Z = pow(var_Z, 1.0f / 3.0f);
            else
                var_Z = (7.787f * var_Z) + (16.0f / 116.0f);

            image_L.Data()[pos] = (116.0f * var_Y) - 16.0f;
            image_a.Data()[pos] = 500.0f * (var_X - var_Y);
            image_b.Data()[pos] = 200.0f * (var_Y - var_Z);
        }
    }
}

//======================================================
//
//======================================================
void CMathImage::ConvertCIELab_to_XYZ(FMathImage &image_L, FMathImage &image_a, FMathImage &image_b,
                                      FMathImage &image_X, FMathImage &image_Y, FMathImage &image_Z)
{
    DINT x, y, line, pos;
    const float ref_X = 95.047f;
    const float ref_Y = 100.0f;
    const float ref_Z = 108.883f;
    for (y = 0, line = 0; y < image_X.Height(); ++y, line += image_X.Width())
    {
        for (x = 0; x < image_X.Width(); ++x)
        {
            pos = line + x;

            float var_Y = (image_L.Data()[pos] + 16.0f) / 116.0f;
            float var_X = image_a.Data()[pos] / 500.0f + var_Y;
            float var_Z = var_Y - image_b.Data()[pos] / 200.0f;
            float var_Y3 = var_Y * var_Y * var_Y;
            if (var_Y3 > 0.008856f)
                var_Y = var_Y3;
            else
                var_Y = (var_Y - 16.0f / 116.0f) / 7.787f;

            float var_X3 = var_X * var_X * var_X;
            if (var_X3 > 0.008856f)
                var_X = var_X3;
            else
                var_X = (var_X - 16.0f / 116.0f) / 7.787f;

            float var_Z3 = var_Z * var_Z * var_Z;
            if (var_Z3 > 0.008856f)
                var_Z = var_Z3;
            else
                var_Z = (var_Z - 16.0f / 116.0f) / 7.787f;

            image_X.Data()[pos] = var_X * ref_X;
            image_Y.Data()[pos] = var_Y * ref_Y;
            image_Z.Data()[pos] = var_Z * ref_Z;
        }
    }
}

//======================================================
//
//======================================================
void CMathImage::ConvertRGB_to_CIELab(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b,
                                      FMathImage &image_L, FMathImage &image_a, FMathImage &image_b)
{
    const float ref_X = 95.047f;
    const float ref_Y = 100.0f;
    const float ref_Z = 108.883f;
    DINT x, y, line, pos;
    for (y = 0, line = 0; y < img_r.Y; ++y, line += img_r.X)
    {
        for (x = 0; x < img_r.X; ++x)
        {
            pos = line + x;

            float var_R = (float) img_r.Buffer[pos] / 255.0f;        //R from 0 to 255
            float var_G = (float) img_g.Buffer[pos] / 255.0f;        //G from 0 to 255
            float var_B = (float) img_b.Buffer[pos] / 255.0f;        //B from 0 to 255

            if (var_R > 0.04045f)
                var_R = pow((var_R + 0.055f) / 1.055f, 2.4f);
            else
                var_R /= 12.92f;
            if (var_G > 0.04045f)
                var_G = pow((var_G + 0.055f) / 1.055f, 2.4f);
            else
                var_G /= 12.92f;
            if (var_B > 0.04045f)
                var_B = pow((var_B + 0.055f) / 1.055f, 2.4f);
            else
                var_B /= 12.92f;

            var_R *= 100.0f;
            var_G *= 100.0f;
            var_B *= 100.0f;

            //Observer. = 2\B0, Illuminant = D65
            float var_X = (var_R * 0.4124f + var_G * 0.3576f + var_B * 0.1805f) / ref_X;
            float var_Y = (var_R * 0.2126f + var_G * 0.7152f + var_B * 0.0722f) / ref_Y;
            float var_Z = (var_R * 0.0193f + var_G * 0.1192f + var_B * 0.9505f) / ref_Y;

            if (var_X > 0.008856f)
                var_X = pow(var_X, 1.0f / 3.0f);
            else
                var_X = (7.787f * var_X) + (16.0f / 116.0f);
            if (var_Y > 0.008856f)
                var_Y = pow(var_Y, 1.0f / 3.0f);
            else
                var_Y = (7.787f * var_Y) + (16.0f / 116.0f);
            if (var_Z > 0.008856f)
                var_Z = pow(var_Z, 1.0f / 3.0f);
            else
                var_Z = (7.787f * var_Z) + (16.0f / 116.0f);

            image_L.Data()[pos] = (116.0f * var_Y) - 16.0f;
            image_a.Data()[pos] = 500.0f * (var_X - var_Y);
            image_b.Data()[pos] = 200.0f * (var_Y - var_Z);
        }
    }
}

//======================================================
//
//======================================================
void CMathImage::ConvertCIELab_to_RGB(FMathImage &image_L, FMathImage &image_a, FMathImage &image_b,
                                      CMathImage &img_r, CMathImage &img_g, CMathImage &img_b)
{
    DINT x, y, line, pos;
    const float ref_X = 0.95047f;
    const float ref_Y = 1.000f;
    const float ref_Z = 1.08883f;
    for (y = 0, line = 0; y < image_L.Height(); ++y, line += image_L.Width())
    {
        for (x = 0; x < image_L.Width(); ++x)
        {
            pos = line + x;

            float var_Y = (image_L.Data()[pos] + 16.0f) / 116.0f;
            float var_X = image_a.Data()[pos] / 500.0f + var_Y;
            float var_Z = var_Y - image_b.Data()[pos] / 200.0f;
            float var_Y3 = var_Y * var_Y * var_Y;
            if (var_Y3 > 0.008856f)
                var_Y = var_Y3;
            else
                var_Y = (var_Y - 16.0f / 116.0f) / 7.787f;

            float var_X3 = var_X * var_X * var_X;
            if (var_X3 > 0.008856f)
                var_X = var_X3;
            else
                var_X = (var_X - 16.0f / 116.0f) / 7.787f;

            float var_Z3 = var_Z * var_Z * var_Z;
            if (var_Z3 > 0.008856f)
                var_Z = var_Z3;
            else
                var_Z = (var_Z - 16.0f / 116.0f) / 7.787f;

            var_X *= ref_X;
            var_Y *= ref_Y;
            var_Z *= ref_Z;

            float var_R = var_X * 3.2406f + var_Y * -1.5372f + var_Z * -0.4986f;
            float var_G = var_X * -0.9689f + var_Y * 1.8758f + var_Z * 0.0415f;
            float var_B = var_X * 0.0557f + var_Y * -0.2040f + var_Z * 1.0570f;

            if (var_R > 0.0031308f)
                var_R = 1.055f * pow(var_R, 1.0f / 2.4f) - 0.055f;
            else
                var_R *= 12.92f;
            if (var_G > 0.0031308f)
                var_G = 1.055f * pow(var_G, 1.0f / 2.4f) - 0.055f;
            else
                var_G *= 12.92f;
            if (var_B > 0.0031308f)
                var_B = 1.055f * pow(var_B, 1.0f / 2.4f) - 0.055f;
            else
                var_B *= 12.92f;

            img_r.Buffer[pos] = min(255, INT32(var_R * 255.0f));
            img_g.Buffer[pos] = min(255, INT32(var_G * 255.0f));
            img_b.Buffer[pos] = min(255, INT32(var_B * 255.0f));
        }
    }
}

void CMathImage::LaplacePyramidFilter(LPFilterParam *Params, int nParams, BOOL LogScale)
{
    FMathImage fImage(*this);
    if (fImage.IsValid())
    {
        fImage.LaplacePyramidFilter(Params, nParams, LogScale);
        DINT Len = X * Y;
        float *data = fImage.Data();
        for (int i = 0; i < Len; ++i)
            Buffer[i] = (INT32) data[i];
    }
}

//======================================================
//
//======================================================
void CFourierImage::MakeTensor2DMask(CFourierImage &image, CFourierImage &a, CFourierImage &b,
                                     CFourierImage &c, int w, float Sigma, BOOL GaussKernel)
{
    a.ClearRe();
    a.ClearIm();
    b.ClearRe();
    b.ClearIm();
    c.ClearRe();
    c.ClearIm();
    int x_X = image.X;
    int _Y = image.Y;

    if (w > x_X / 2 - 1)
        w = x_X / 2 - 1;
    if (w > _Y / 2 - 1)
        w = _Y / 2 - 1;

    int x, y;
    float G = 1.0f;
    float Sigma_2 = 1.0f / (Sigma * Sigma);
    for (x = 0; x <= w; ++x)
    {
        float x2 = (float) x * x;
        if (GaussKernel)
            G = exp(-0.5f * x2 * Sigma_2);
        c._Re(x, 0) = x2 * G;
    }

    for (y = 0; y <= w; ++y)
    {
        float y2 = (float) y * y;
        if (GaussKernel)
            G = exp(-0.5f * y2 * Sigma_2);
        a._Re(0, y) = y2 * G;
    }

    for (x = 1; x <= w; ++x)
    {
        float x2 = (float) x * x;
        for (y = 1; y <= w; ++y)
        {
            float y2 = (float) y * y;
            float xy = (float) x * y;
            if (GaussKernel)
                G = exp(-0.5f * (x2 + y2) * Sigma_2);
            a._Re(x, y) = a._Re(x_X - x, y) = a._Re(x, _Y - y) = a._Re(x_X - x, _Y - y) = y2 * G;
            c._Re(x, y) = c._Re(x_X - x, y) = c._Re(x, _Y - y) = c._Re(x_X - x, _Y - y) = x2 * G;

            b._Re(x, y) = b._Re(x_X - x, _Y - y) = -xy * G;
            b._Re(x_X - x, y) = b._Re(x, _Y - y) = xy * G;
        }
    }
    image.Transform(TRUE);
    a.Transform(TRUE);
    b.Transform(TRUE);
    c.Transform(TRUE);
    a *= image;
    b *= image;
    c *= image;
    a.Transform(FALSE);
    b.Transform(FALSE);
    c.Transform(FALSE);
}


//---------------------------------------------------------------------------------------------------------------======
//
//---------------------------------------------------------------------------------------------------------------======
GoldSection_Double::GoldSection_Double(PGS_Func aFunc, GParam *aExtraParam, double *aBoundaries,
                                       double aAccuracy)
{
    ExtraParam = aExtraParam;
    Accuracy = aAccuracy;
    MaxItterationNumber = 1000;
    Silent = TRUE;
    _Func = aFunc;
    Boundaries[0] = aBoundaries[0];
    Boundaries[1] = aBoundaries[1];
    Result = (Boundaries[1] + Boundaries[0]) / 2.0;
}

//---------------------------------------------------------------------------------------------------------------======
//
//---------------------------------------------------------------------------------------------------------------======
//---------------------------------------------------------------------------------------------------------------======
//
//---------------------------------------------------------------------------------------------------------------======
void GoldSection_Double::TraceResult(double x, double FV)
{
    TRACE("Argument = %g, Function = %g\r\n", x, FV);
}

//---------------------------------------------------------------------------------------------------------------======
//
//---------------------------------------------------------------------------------------------------------------======
double GoldSection_Double::Solve()
{
    double down = Boundaries[0];
    double up = Boundaries[1];
    double Nom = fabs(Boundaries[1] - Boundaries[0]);
    double Up = Func(up);
    if (!Silent)
        TraceResult(up, Up);
    double Down = Func(down);
    if (!Silent)
        TraceResult(down, Down);
    double w = (3.0 - sqrt(5.0)) / 2.0;
    double mid = (up - down) * w + down;
    double Mid = Func(mid);
    if (!Silent)
        TraceResult(mid, Mid);
    double mid1, Mid1;
    BOOL flg;
    while (fabs(up - down) / Nom > Accuracy)
    {
        if (fabs(up - mid) > fabs(down - mid))
        {
            flg = TRUE;
            mid1 = (up - mid) * w + mid;
        } else
        {
            flg = FALSE;
            mid1 = (mid - down) * w + down;
        }
        Mid1 = Func(mid1);
        if (!Silent)
            TraceResult(mid1, Mid1);
        if (Mid1 > Mid)
        {
            if (flg)
            {
                up = mid1;
                Up = Mid1;
            } else
            {
                down = mid1;
                Down = Mid1;
            }
        } else
        {
            if (flg)
            {
                down = mid;
                Down = Mid;
            } else
            {
                up = mid;
                Up = Mid;
            }
            mid = mid1;
            Mid = Mid1;
        }
    }
    if (Down < Mid)
        Result = down;
    else if (Up < Mid)
        Result = up;
    else
        Result = mid;
    return Result;
}

int CMathImage::Inflation(int NumbItter, int NumbContact, int Value)
{
    int i_count = 0;
    if (NumbItter > 0)
    {
        MArray<BYTE> bb(X * Y, 0);
        BYTE *IndexBuffer = bb.GetPtr();
        if (!IndexBuffer)
            return 0;
        _Inflation(IndexBuffer, NumbItter, NumbContact);
        int _XY = X * Y;
        for (int i = 0; i < _XY; ++i)
            if (IndexBuffer[i] == 2)
            {
                Buffer[i] = Value;
                ++i_count;
            }

    }
    return i_count;
}

int CMathImage::_Inflation(BYTE *IndexBuffer, int NumbItter, int NumbContact)
{
    int XY = X * Y;
    if (NumbItter == 0)
        return 0;
    memset(IndexBuffer, 0, XY);
    BYTE *IndexBuffer1 = (BYTE *) HMALLOC(XY);
    if (!IndexBuffer1)
        return 0;
    memset(IndexBuffer, 0, XY);
    int x_X = X - 1;
    int _Y = Y - 1;
    int x, y, off_y;
    for (y = 1, off_y = X; y < _Y; ++y, off_y += X)
    {
        for (x = 1; x < x_X; ++x)
        {
            if (Buffer[x + off_y])
                IndexBuffer[x + off_y] = 1;
        }
    }
    memcpy(IndexBuffer1, IndexBuffer, XY);
    int count;
    BYTE *Src = IndexBuffer;
    BYTE *Dst = IndexBuffer1;
    int i_count = 0;
    for (count = 0; count < NumbItter; ++count)
    {
        for (y = 1, off_y = X; y < _Y; ++y, off_y += X)
        {
            for (x = 1; x < x_X; ++x)
            {
                if (Src[x + off_y] == 0)
                {
                    int nContact = 0;
                    if (Src[x + off_y - 1])
                        ++nContact;
                    if (Src[x + off_y + 1])
                        ++nContact;
                    if (Src[x + off_y - X])
                        ++nContact;
                    if (Src[x + off_y + X])
                        ++nContact;
                    if (nContact >= NumbContact)
                    {
                        Dst[x + off_y] = 2;
                        ++i_count;
                    }
                }
            }
        }
        memcpy(Src, Dst, XY);
    }
    HFREE(IndexBuffer1);
    return i_count;
}

void CMathImage::GetRGB(BYTE *_buf, int X, int Y, int sizeX, int MaskR, int MaskG, int MaskB)
{
    int Min, Max;
    MinMax(Min, Max);
    double n = Min;
    double x = Max;
    //Min +=  Max/10;
    double d = (x - n) / 255.0;
    BYTE b;
    for (int k = 0, i = 0; k < Y; ++k)
    {
        for (int j = 0; j < X; ++j)
        {
            if (Buffer[i + j] > Min)
            {
                if (MaskR && MaskB && MaskG)
                    b = 255 - (BYTE) (((double) Buffer[i + j] - n) / d + 0.5);
                else
                    b = (BYTE) (((double) Buffer[i + j] - n) / d + 0.5);

                _buf[j * 3] = b & MaskB;
                _buf[j * 3 + 1] = b & MaskG;
                _buf[j * 3 + 2] = b & MaskR;
            } else
            {
                if ((MaskR && MaskB && MaskG))
                {
                    _buf[j * 3] = 255;
                    _buf[j * 3 + 1] = 255;
                    _buf[j * 3 + 2] = 255;
                } else
                {
                    _buf[j * 3] = 0;
                    _buf[j * 3 + 1] = 0;
                    _buf[j * 3 + 2] = 0;
                }
            }

        }
        i += X;
        _buf += sizeX;

    }

}

void CMathImage::GetRGB2(BYTE *_buf, int X, int Y, int sizeX, double iMax, double proc)
{
    int Min, Max;
    MinMax(Min, Max);
    double n = iMax / 100.0 * proc;
    int x = Max / n;
    double d = Max / 255.0;

    for (int k = 0, i = 0; k < Y; ++k)
    {
        for (int j = 0; j < X; ++j)
        {
            if (Buffer[i + j] >= x)
            {
                BYTE b = 0;
                _buf[j * 3] = 0;
                _buf[j * 3 + 1] = 0;
                _buf[j * 3 + 2] = 0;
            } else
            { ;
                _buf[j * 3] = 255;
                _buf[j * 3 + 1] = 255;
                _buf[j * 3 + 2] = 255;
            }

        }
        i += X;
        _buf += sizeX;

    }

}

void CMathImage::GetRGB3(int *_buf, int X, int Y, int sizeX, double iMax, double proc)
{
    int Min, Max;
    MinMax(Min, Max);
    double n = iMax / 100.0 * proc;
    int x = Max / n;
    double d = Max / 255.0;

    for (int k = 0, i = 0; k < Y; ++k)
    {
        for (int j = 0; j < X; ++j)
        {
            if (Buffer[i + j] > x)
            {
                _buf[i + j] = 100;
//				BYTE b = 0;
//				_buf[j * 3] = 0;
//				_buf[j * 3 + 1] = 0;
//				_buf[j * 3 + 2] = 0;
            } else
            {
                _buf[i + j] = 0;;
//				_buf[j * 3] = 255;
//				_buf[j * 3 + 1] = 255;
//				_buf[j * 3 + 2] = 255;
            }

        }
        i += X;
        //	_buf += sizeX;

    }

}

//===================================================================
//
//===================================================================
struct SmoothTensorParam {
    CMathImage *Image;
    int *Buffer;
    CFourierImage *A;
    CFourierImage *B;
    CFourierImage *C;
    DINT Top;
    DINT Bottom;
    int wSmooth;
    float Sigma2;
    BOOL Invert;
};

UINT AFX_CDECL SmoothTensorFunc(void *Param)
{
    SmoothTensorParam *d = (SmoothTensorParam *) Param;
    d->Image->_SmoothTensor(d->Buffer, d->A, d->B, d->C, d->wSmooth, d->Top, d->Bottom, d->Sigma2,
                            d->Invert);
    return 0;
}

//======================================================
//
//======================================================
void CMathImage::_SmoothTensor(INT32 *_Buffer, CFourierImage *A, CFourierImage *B, CFourierImage *C,
                               int wSmooth, DINT Top, DINT Bottom, float Sigma2, BOOL Invert)
{
    DINT x, y;
    int W = (2 * wSmooth + 1);
    for (y = Top; y < Bottom; ++y)
    {
        for (x = 0; x < X; ++x)
        {
            float a = A->_Re(x, y);
            float b = B->_Re(x, y);
            float c = C->_Re(x, y);
            float Alpha;
            float a11;
            float a22;
            float cosa, sina;
            if (a == c)
            {
                Alpha = (float) (PI / 4.0);
                if (b < 0.0f)
                {
                    a11 = (a + c) * 0.5f - b;
                    a22 = (a + c) * 0.5f + b;
                } else
                {
                    Alpha *= -1.0f;
                    a11 = (a + c) * 0.5f + b;
                    a22 = (a + c) * 0.5f - b;
                }
                cosa = cos(Alpha);
                sina = sin(Alpha);
            } else
            {
                float tg2a = b / (c - a);
                Alpha = acos(sqrt(1.0f / (1.0f + tg2a * tg2a))) / 2.0f;
                if (tg2a < 0.0f)
                    Alpha *= -1.0f;
                cosa = cos(Alpha);
                sina = sin(Alpha);
                float cosa2 = cosa * cosa;
                float sina2 = sina * sina;
                a11 = a * cosa2 + c * sina2 - 2.0f * b * sina * cosa;
                a22 = a * sina2 + c * cosa2 + 2.0f * b * sina * cosa;
            }
            float R1, R2;
            R1 = max(0.2, (double) fabs(a22 * a22) / max((double) fabs(a11 * a11), 0.2));
            R2 = 1.0f;
            float Nom = max(R1, R2);
            R1 /= Nom;
            R2 /= Nom;
            if (R1 > R2)
                R2 = max(R2, 0.2f);
            else
                R1 = max(R1, 0.2f);
            if (Invert)
            {
                Alpha += (float) (PI / 2.0);
                cosa = cos(Alpha);
                sina = sin(Alpha);
            }
            float Sum = 0.0f;
            float I = 0.0f;
            for (DINT _y = 0; _y < W; ++_y)
            {
                DINT dy = _y - wSmooth;
                DINT cy = y + dy;
                if (cy < 0 || cy >= Y)
                    continue;
                for (DINT _x = 0; _x < W; ++_x)
                {
                    DINT dx = _x - wSmooth;
                    DINT cx = x + dx;
                    if (cx < 0 || cx >= X)
                        continue;
                    float x1 = (dx * cosa - dy * sina) / R1;
                    float y1 = (dx * sina + dy * cosa) / R2;
                    float Sm = exp(-0.5f * (x1 * x1 + y1 * y1) / Sigma2);
                    I += Buffer[cy * X + cx] * Sm;
                    Sum += Sm;
                }
            }
            _Buffer[y * X + x] = (INT32) (I / Sum);
        }
    }
}

//======================================================
//
//======================================================
void CMathImage::SmoothTensor(int wTensor, int wSmooth, float Sigma, float tSigma, BOOL GaussKernel,
                              BOOL Invert)
{
    float Sigma2 = Sigma * Sigma;
    INT32 *_Buffer = (INT32 *) HMALLOCZ(X * Y * sizeof(INT32));
    if (!_Buffer)
        return;
    CFourierImage *image = new CFourierImage(*this);
    if (!image || !image->IsValid())
    {
        HFREE(_Buffer);
        delete image;
        return;
    }

    CFourierImage *A = new CFourierImage(image->Width(), image->Height());
    CFourierImage *B = new CFourierImage(image->Width(), image->Height());
    CFourierImage *C = new CFourierImage(image->Width(), image->Height());
    if (A && B && C && A->IsValid() && B->IsValid() && C->IsValid())
    {
        CFourierImage::MakeTensor2DMask(*image, *A, *B, *C, wTensor, tSigma, GaussKernel);

        int NTreads = pMultiThreads->Num();
        if (NTreads > 1)
        {
            DINT step = max(Y / NTreads, 1);
            int done = 0;
            SmoothTensorParam *Param = (SmoothTensorParam *) _alloca(
                    sizeof(SmoothTensorParam) * (NTreads + 1));
            Param[0].Image = this;
            Param[0].Top = 0;
            Param[0].Bottom = step;
            Param[0].Buffer = _Buffer;
            Param[0].A = A;
            Param[0].B = B;
            Param[0].C = C;
            Param[0].wSmooth = wSmooth;
            Param[0].Sigma2 = Sigma2;
            Param[0].Invert = Invert;
            int i;
            for (i = 1; i < NTreads; ++i)
            {
                memcpy(Param + i, Param, sizeof(SmoothTensorParam));
                Param[i].Top = Param[i - 1].Bottom;
                Param[i].Bottom = Param[i].Top + step;
            }
            NTreads = i;
            Param[i - 1].Bottom = Y;
            for (i = 0; i < NTreads; ++i)
                pMultiThreads->Run(i, SmoothTensorFunc, Param + i);
            pMultiThreads->Wait();
        } else
            _SmoothTensor(_Buffer, A, B, C, wSmooth, 0, Y, Sigma2, Invert);
        HFREE(Buffer);
        Buffer = _Buffer;
    } else
        HFREE(_Buffer);
    delete image;
    delete A;
    delete B;
    delete C;
}

//======================================================
//
//======================================================
void
CMathImage::SmoothTensorGrad(int wTensor, int wSmooth, float Sigma, float tSigma, BOOL GaussKernel,
                             BOOL Invert)
{
    float Sigma2 = Sigma * Sigma;
    INT32 *_Buffer = (INT32 *) HMALLOCZ(X * Y * sizeof(INT32));
    if (!_Buffer)
        return;
    CFourierImage *image = new CFourierImage(*this);
    if (!image || !image->IsValid())
    {
        HFREE(_Buffer);
        delete image;
        return;
    }

    CFourierImage *A = new CFourierImage(image->Width(), image->Height());
    CFourierImage *B = new CFourierImage(image->Width(), image->Height());
    CFourierImage *C = new CFourierImage(image->Width(), image->Height());
    CFourierImage *G_X = new CFourierImage(image->Width(), image->Height());
    CFourierImage *G_Y = new CFourierImage(image->Width(), image->Height());
    if (A && B && C && G_X && G_Y && A->IsValid() && B->IsValid() && C->IsValid() &&
        G_X->IsValid() && G_Y->IsValid())
    {
        G_X->MakeGradXGaussMask2D(wTensor / 2.0f);
        G_Y->MakeGradYGaussMask2D(wTensor / 2.0f);
        image->Transform(TRUE);
        G_X->Transform(TRUE);
        G_Y->Transform(TRUE);
        *G_X *= *image;
        *G_Y *= *image;
        G_X->Transform(FALSE);
        G_Y->Transform(FALSE);
        image->ClearIm();
        for (DINT x = 0; x < X; ++x)
        {
            for (DINT y = 0; y < Y; ++y)
            {
                float gx = G_X->_Re(x, y);
                float gy = G_Y->_Re(x, y);
                image->_Re(x, y) = gx * gx + gy * gy;
            }
        }
        CFourierImage::MakeTensor2DMask(*image, *A, *B, *C, wTensor, tSigma, GaussKernel);

        int NTreads = pMultiThreads->Num();
        if (NTreads > 1)
        {
            DINT step = max(Y / NTreads, 1);
            int done = 0;
            SmoothTensorParam *Param = (SmoothTensorParam *) _alloca(
                    sizeof(SmoothTensorParam) * (NTreads + 1));
            Param[0].Image = this;
            Param[0].Top = 0;
            Param[0].Bottom = step;
            Param[0].Buffer = _Buffer;
            Param[0].A = A;
            Param[0].B = B;
            Param[0].C = C;
            Param[0].wSmooth = wSmooth;
            Param[0].Sigma2 = Sigma2;
            Param[0].Invert = Invert;
            int i;
            for (i = 1; i < NTreads; ++i)
            {
                memcpy(Param + i, Param, sizeof(SmoothTensorParam));
                Param[i].Top = Param[i - 1].Bottom;
                Param[i].Bottom = Param[i].Top + step;
            }
            NTreads = i;
            Param[i - 1].Bottom = Y;
            for (i = 0; i < NTreads; ++i)
                pMultiThreads->Run(i, SmoothTensorFunc, Param + i);
            pMultiThreads->Wait();
        } else
            _SmoothTensor(_Buffer, A, B, C, wSmooth, 0, Y, Sigma2, Invert);
        HFREE(Buffer);
        Buffer = _Buffer;
    } else
        HFREE(_Buffer);
    delete image;
    delete A;
    delete B;
    delete C;
    delete G_X;
    delete G_Y;
}

//======================================================
//
//======================================================
void CMathImage::TensorRatio(int wTensor, int Scale, float tSigma, BOOL GaussKernel)
{
    INT32 *_Buffer = (INT32 *) HMALLOCZ(X * Y * sizeof(INT32));
    if (!_Buffer)
        return;
    CFourierImage *image = new CFourierImage(*this);
    if (!image || !image->IsValid())
    {
        HFREE(_Buffer);
        delete image;
        return;
    }

    CFourierImage *A = new CFourierImage(image->Width(), image->Height());
    CFourierImage *B = new CFourierImage(image->Width(), image->Height());
    CFourierImage *C = new CFourierImage(image->Width(), image->Height());
    if (A && B && C && A->IsValid() && B->IsValid() && C->IsValid())
    {
        CFourierImage::MakeTensor2DMask(*image, *A, *B, *C, wTensor, tSigma, GaussKernel);

        DINT x, y;
        DINT XY = X * Y;
        for (y = 0; y < Y; ++y)
        {
            for (x = 0; x < X; ++x)
            {
                float a = A->_Re(x, y);
                float b = B->_Re(x, y);
                float c = C->_Re(x, y);
                float Alpha;
                float a11;
                float a22;
                float cosa, sina;
                if (a == c)
                {
                    Alpha = (float) (PI / 4.0);
                    if (b < 0.0f)
                    {
                        a11 = (a + c) * 0.5f - b;
                        a22 = (a + c) * 0.5f + b;
                    } else
                    {
                        Alpha *= -1.0f;
                        a11 = (a + c) * 0.5f + b;
                        a22 = (a + c) * 0.5f - b;
                    }
                    cosa = cos(Alpha);
                    sina = sin(Alpha);
                } else
                {
                    float tg2a = b / (c - a);
                    Alpha = acos(sqrt(1.0f / (1.0f + tg2a * tg2a))) / 2.0f;
                    if (tg2a < 0.0f)
                        Alpha *= -1.0f;
                    cosa = cos(Alpha);
                    sina = sin(Alpha);
                    float cosa2 = cosa * cosa;
                    float sina2 = sina * sina;
                    a11 = a * cosa2 + c * sina2 - 2.0f * b * sina * cosa;
                    a22 = a * sina2 + c * cosa2 + 2.0f * b * sina * cosa;
                }
                double R1 = max(0.2, (double) fabs(a22 * a22) / max((double) fabs(a11 * a11), 0.2));
                double R2 = 1.0f;
                double Nom = max(R1, R2);
                R1 /= Nom;
                R2 /= Nom;
                if (Alpha > 0.0 && Alpha < PI / 2.0)
                    _Buffer[y * X + x] = (INT32) (Scale * R1 / max(R2, 0.2));
                else
                    _Buffer[y * X + x] = (INT32) (Scale * R2 / max(R1, 0.2));
            }
        }
        HFREE(Buffer);
        Buffer = _Buffer;
    } else
        HFREE(_Buffer);
    delete image;
    delete A;
    delete B;
    delete C;
}

//======================================================
//
//======================================================
void CMathImage::TensorGradRatio(int wTensor, int Scale, float tSigma, BOOL GaussKernel)
{
    INT32 *_Buffer = (INT32 *) HMALLOCZ(X * Y * sizeof(INT32));
    if (!_Buffer)
        return;
    CFourierImage *image = new CFourierImage(*this);
    if (!image || !image->IsValid())
    {
        HFREE(_Buffer);
        delete image;
        return;
    }

    CFourierImage *A = new CFourierImage(image->Width(), image->Height());
    CFourierImage *B = new CFourierImage(image->Width(), image->Height());
    CFourierImage *C = new CFourierImage(image->Width(), image->Height());
    CFourierImage *G_X = new CFourierImage(image->Width(), image->Height());
    CFourierImage *G_Y = new CFourierImage(image->Width(), image->Height());
    if (A && B && C && G_X && G_Y && A->IsValid() && B->IsValid() && C->IsValid() &&
        G_X->IsValid() && G_Y->IsValid())
    {
        G_X->MakeGradXGaussMask2D(wTensor / 2.0f);
        G_Y->MakeGradYGaussMask2D(wTensor / 2.0f);
        image->Transform(TRUE);
        G_X->Transform(TRUE);
        G_Y->Transform(TRUE);
        *G_X *= *image;
        *G_Y *= *image;
        G_X->Transform(FALSE);
        G_Y->Transform(FALSE);
        image->ClearIm();
        DINT x, y;
        for (x = 0; x < X; ++x)
        {
            for (DINT y = 0; y < Y; ++y)
            {
                float gx = G_X->_Re(x, y);
                float gy = G_Y->_Re(x, y);
                image->_Re(x, y) = gx * gx + gy * gy;
            }
        }
        CFourierImage::MakeTensor2DMask(*image, *A, *B, *C, wTensor, tSigma, GaussKernel);
        CFourierImage::MakeTensor2DMask(*image, *A, *B, *C, wTensor, tSigma, GaussKernel);

        DINT XY = X * Y;
        for (y = 0; y < Y; ++y)
        {
            for (x = 0; x < X; ++x)
            {
                float a = A->_Re(x, y);
                float b = B->_Re(x, y);
                float c = C->_Re(x, y);
                float Alpha;
                float a11;
                float a22;
                float cosa, sina;
                if (a == c)
                {
                    Alpha = (float) (PI / 4.0);
                    if (b < 0.0f)
                    {
                        a11 = (a + c) * 0.5f - b;
                        a22 = (a + c) * 0.5f + b;
                    } else
                    {
                        Alpha *= -1.0f;
                        a11 = (a + c) * 0.5f + b;
                        a22 = (a + c) * 0.5f - b;
                    }
                    cosa = cos(Alpha);
                    sina = sin(Alpha);
                } else
                {
                    float tg2a = b / (c - a);
                    Alpha = acos(sqrt(1.0f / (1.0f + tg2a * tg2a))) / 2.0f;
                    if (tg2a < 0.0f)
                        Alpha *= -1.0f;
                    cosa = cos(Alpha);
                    sina = sin(Alpha);
                    float cosa2 = cosa * cosa;
                    float sina2 = sina * sina;
                    a11 = a * cosa2 + c * sina2 - 2.0f * b * sina * cosa;
                    a22 = a * sina2 + c * cosa2 + 2.0f * b * sina * cosa;
                }
                float R1 = (float) max(0.2, (double) fabs(a22 * a22) /
                                            max((double) fabs(a11 * a11), 0.2));
                float R2 = 1.0f;
                float Nom = max(R1, R2);
                R1 /= Nom;
                R2 /= Nom;
                if (Alpha > 0.0 && Alpha < PI / 2.0)
                    _Buffer[y * X + x] = (INT32) (Scale * R1 / max(R2, 0.2f));
                else
                    _Buffer[y * X + x] = (INT32) (Scale * R2 / max(R1, 0.2f));
            }
        }
        HFREE(Buffer);
        Buffer = _Buffer;
    } else
        HFREE(_Buffer);
    delete image;
    delete A;
    delete B;
    delete C;
    delete G_X;
    delete G_Y;
}

//======================================================
//
//======================================================
void CMathImage::FindCentralLines(DINT R)
{
    MArray<BYTE> bb(X * Y, 0);
    BYTE *IndexBuffer = bb.GetPtr();
    if (!IndexBuffer)
        return;
    DINT lim = X * Y;
    INT32 Max = INT_MIN;
    INT32 Min = INT_MAX;
    DINT i;
    for (i = 0; i < lim; ++i)
    {
        if (Buffer[i] > 0)
        {
            IndexBuffer[i] = 1;
            if (Max < Buffer[i])
                Max = Buffer[i];
            if (Min > Buffer[i])
                Min = Buffer[i];
        }
    }
    for (INT32 level = Min; level <= Max; ++level)
    {
        while (_ErosionStep(IndexBuffer, level, TRUE));
    }
    if (R > 0)
    {
        _ExpandCentalLineEnds(IndexBuffer, R);
        Max = 0;
        Min = INT_MAX;
        for (i = 0; i < lim; ++i)
        {
            if (IndexBuffer[i])
            {
                if (Max < Buffer[i])
                    Max = Buffer[i];
                if (Min > Buffer[i])
                    Min = Buffer[i];
            }
        }
        for (INT32 level = Min; level <= Max; ++level)
        {
            while (_ErosionStep(IndexBuffer, level, TRUE));
        }
    }
    for (i = 0; i < lim; ++i)
    {
        if (!IndexBuffer[i])
            Buffer[i] = 0;
    }
}

//======================================================
//
//======================================================
BOOL CMathImage::_ErosionStep(BYTE *IndexBuffer, INT32 UpperBoundary, BOOL KeepBaseLine)
{
    BOOL Done = TRUE;
    DINT lim = X * Y;
    if (!KeepBaseLine)
    {
        for (DINT i = 0; i < lim; ++i)
            if (IndexBuffer[i])
                IndexBuffer[i] = 1;
    }
    for (int index = 0; index < 4; ++index)
    {
        DINT y, x, off_y;
        for (y = 0, off_y = 0; y < Y; ++y, off_y += X)
        {
            for (x = 0; x < X; ++x)
            {
                if (IndexBuffer[x + off_y] == 1 && Buffer[x + off_y] <= UpperBoundary)
                {
                    switch (_IsBaseLinePoint(IndexBuffer, x, y, index, !KeepBaseLine))
                    {
                        case 0 :
                            IndexBuffer[x + off_y] = 3;
                            break;
                        case 1 :
                            IndexBuffer[x + off_y] = 2;
                            break;
                        case -1 :
                            break;
                    }
                }
            }
        }
        for (DINT i = 0; i < lim; ++i)
        {
            if (IndexBuffer[i] == 3)
            {
                Done = FALSE;
                IndexBuffer[i] = 0;
            }
        }
    }
    return !Done;
}

//======================================================
//
//======================================================
void CMathImage::_ExpandCentalLineEnds(BYTE *IndexBuffer, DINT R)
{
    DINT lim = X * Y;
    for (DINT x = 1, lim_X = X - 1; x < lim_X; ++x)
    {
        for (DINT y = 1, lim_Y = Y - 1; y < lim_Y; ++y)
        {
            if (IndexBuffer[x + y * X] == 2)
            {
                DINT count = 0;
                for (DINT k = -1; k <= 1; ++k)
                    for (DINT m = -1; m <= 1; ++m)
                        if (IndexBuffer[x + k + X * (y + m)] == 2)
                            ++count;
                if (count <= 2)
                {
                    DINT R2 = R * R;
                    for (DINT k = -R; k <= R; ++k)
                    {
                        for (DINT m = -R; m <= R; ++m)
                        {
                            DINT pos = x + k + X * (y + m);
                            if (m * m + k * k <= R2 && pos >= 0 && pos < lim &&
                                IndexBuffer[pos] == 0)
                                IndexBuffer[pos] = 1;
                        }
                    }
                }
            }
        }
    }
}

//======================================================
//
//======================================================
int CMathImage::_IsBaseLinePoint(BYTE *IndexBuffer, DINT x, DINT y, int index, BOOL KeepLast)
{
/*
	1     2    4
	8     0   32
	64  128  256
*/
    static INT32 BitMatrix[3][3] = {{1,  2,   4},
                                    {8,  0,   32},
                                    {64, 128, 256}};
    DINT count = 0;
    DINT i;
    DINT count3 = 0;
    for (i = -1; i <= 1; ++i)
    {
        DINT _x = x + i;
        if (_x >= 0 && _x < X)
        {
            for (DINT j = -1; j <= 1; ++j)
            {
                DINT _y = y + j;
                if (_y >= 0 && _y < Y && IndexBuffer[_x + _y * X])
                {
                    count |= BitMatrix[j + 1][i + 1];
                    if (IndexBuffer[_x + _y * X] == 3)
                        count3 |= BitMatrix[j + 1][i + 1];
                }
            }
        }
    }
    if (count == 0 || count == count3)
        return 1;

    if (KeepLast)
    {
        switch (count)
        {
            case 1 :
            case 2 :
            case 4 :
            case 8 :
            case 16 :
            case 32 :
            case 64 :
            case 128 :
            case 256 :
                return 1;
        }
    }

    switch (index)
    {
        case 0 :
            if (count & 2)
                return -1;
            else
                break;
        case 1 :
            if (count & 32)
                return -1;
            else
                break;
        case 2 :
            if (count & 128)
                return -1;
            else
                break;
        case 3 :
            if (count & 8)
                return -1;
            else
                break;
        default :
            break;
    }

    if ((count & (1 | 2 | 4)) && (count & (64 | 128 | 256)) && !(count & (8 | 32)))
        return 1;
    if ((count & (1 | 8 | 64)) && (count & (4 | 32 | 256)) && !(count & (2 | 128)))
        return 1;

    if ((count & 256) && (count & (1 | 2 | 4 | 8 | 64)) && !(count & (32 | 128)))
    {
        if (IndexBuffer[x + 1 + (y + 1) * X] == 2)
            return 1;
//		else
//			return -1;
    } else if ((count & 4) && (count & (1 | 8 | 64 | 128 | 256)) && !(count & (32 | 2)))
    {
        if (IndexBuffer[x + 1 + (y - 1) * X] == 2)
            return 1;
//		else
//			return -1;
    } else if ((count & 1) && (count & (64 | 128 | 256 | 32 | 4)) && !(count & (8 | 2)))
    {
        if (IndexBuffer[x - 1 + (y - 1) * X] == 2)
            return 1;
//		else
//			return -1;
    } else if ((count & 64) && (count & (1 | 2 | 4 | 32 | 256)) && !(count & (8 | 128)))
    {
        if (IndexBuffer[x - 1 + (y + 1) * X] == 2)
            return 1;
//		else
//			return -1;
    }
    return 0;
}

float __filter(float x, float sigma, float alpha, float s_beta, float l_beta)
{
    float mx = fabs(x);
    if (mx < sigma)
    {
        float y = mx / sigma;

        if (alpha == 1.0f)
            return x;
        else if (alpha == 0.5f)
            return sign(x) * sigma * (float) sqrt(y);
        else if (alpha == 2.0f)
            return sign(x) * sigma * y * y;
        else if (alpha == 3.0f)
            return sign(x) * sigma * y * y * y;
        else if (alpha == 4.0f)
            return sign(x) * sigma * y * y * y * y;
        else
            return sign(x) * sigma * (float) pow(y, alpha);
    } else
    {
        if (x > 0)
            return l_beta * (mx - sigma) + sigma;
        else
            return -(s_beta * (mx - sigma) + sigma);
    }
}

void CMathImage::LaplacePyramidFilter(float sigma, float alpha, float beta, float ScaleCenter,
                                      float ScaleAlpha, BOOL LogScale)
{
    if (alpha <= 0.0f)
        alpha = 0.01f;
    if (beta < 0.0f)
        beta = 0.0f;
    DINT L = min(X, Y);
    int n = 1;
    while (L > 16)
    {
        L /= 2;
        ++n;
    }
    CFourierImage **Pyr = (CFourierImage **) _alloca(sizeof(CFourierImage *) * (n + 1));
    memset(Pyr, 0, sizeof(CFourierImage *) * (n + 1));

    float *MinV = (float *) _alloca(sizeof(float) * (n + 1));
    float *MaxV = (float *) _alloca(sizeof(float) * (n + 1));

    Pyr[0] = new CFourierImage(*this);
    int i;
    for (i = 1; i < n; ++i)
    {
        Pyr[i] = new CFourierImage(*(Pyr[i - 1]));
        DINT x_X = Pyr[i - 1]->Width();
        DINT _Y = Pyr[i - 1]->Height();

        Pyr[i]->Squeeze4();
        CFourierImage *mask = new CFourierImage(Pyr[i]->Width(), Pyr[i]->Height());
        mask->MakeGaussMask(1.5f);
        mask->Transform(TRUE);
        Pyr[i]->Transform(TRUE);
        *(Pyr[i]) *= *mask;
        Pyr[i]->Transform(FALSE);
        delete mask;
        MinV[i - 1] = 1.0e30f;
        MaxV[i - 1] = -1.0e30f;
        for (DINT y = 0; y < _Y; ++y)
        {
            for (DINT x = 0; x < x_X; ++x)
            {
                float v;
                if (LogScale)
                    v = (float) log(max(1.0f, Pyr[i - 1]->_Re(x, y)) /
                                    max(1.0f, Pyr[i]->ReBiLine(x / 2.0f, y / 2.0f)));
                else
                    v = (float) Pyr[i - 1]->_Re(x, y) - Pyr[i]->ReBiLine(x / 2.0f, y / 2.0f);

                Pyr[i - 1]->_Re(x, y) = v;

                if (MaxV[i - 1] < v)
                    MaxV[i - 1] = v;
                if (MinV[i - 1] > v)
                    MinV[i - 1] = v;
            }
        }
    }
    float gMinV = 1.0e30f;
    float gMaxV = -1.0e30f;
    for (i = n - 2; i >= 0; --i)
    {
        if (gMaxV < MaxV[i])
            gMaxV = MaxV[i];
        if (gMinV > MinV[i])
            gMinV = MinV[i];
    }
    for (i = n - 2; i >= 0; --i)
    {
        float _sigma = LogScale ? sigma : sigma * (MaxV[i] - MinV[i]) / 4.0f;
        DINT x_X = Pyr[i]->Width();
        DINT _Y = Pyr[i]->Height();
        DINT _XY = x_X * _Y;
        float scale = __scaleFactor(i, n, ScaleCenter, ScaleAlpha);
        for (DINT k = 0; k < _XY; ++k)
        {
            float v = Pyr[i]->ReImage[k];
            float _v = (__filter(v, _sigma, alpha, beta) - v) * scale + v;
            Pyr[i]->ReImage[k] = _v;
        }

        for (DINT y = 0; y < _Y; ++y)
        {
            DINT l = y * x_X;
            for (DINT x = 0; x < x_X; ++x)
                if (LogScale)
                    Pyr[i]->_Re(x, y) =
                            Pyr[i + 1]->ReBiLine(x / 2.0f, y / 2.0f) * exp(Pyr[i]->_Re(x, y));
                else
                    Pyr[i]->_Re(x, y) = max(1.0f, Pyr[i]->_Re(x, y) +
                                                  Pyr[i + 1]->ReBiLine(x / 2.0f, y / 2.0f));
        }
    }
    Pyr[0]->GetRe(this);
    for (i = 0; i < n; ++i)
        delete Pyr[i];
}

void CMathImage::LaplacePyramidFilter(float sigma, float alpha, float s_beta, float l_beta,
                                      float ScaleCenter, float ScaleAlpha, float GaussKernel,
                                      BOOL LogScale)
{
    if (alpha <= 0.0f)
        alpha = 0.01f;
    if (s_beta < 0.0f)
        s_beta = 0.01f;
    if (l_beta < 0.0f)
        l_beta = 0.01f;
    DINT L = min(X, Y);
    int n = 1;
    while (L > 32)
    {
        L /= 2;
        ++n;
    }
    CFourierImage **Pyr = (CFourierImage **) _alloca(sizeof(CFourierImage *) * (n + 1));
    memset(Pyr, 0, sizeof(CFourierImage *) * (n + 1));

    float *MinV = (float *) _alloca(sizeof(float) * (n + 1));
    float *MaxV = (float *) _alloca(sizeof(float) * (n + 1));

    Pyr[0] = new CFourierImage(*this);
    int i;
    for (i = 1; i < n; ++i)
    {
        Pyr[i] = new CFourierImage(*(Pyr[i - 1]));
        DINT x_X = Pyr[i - 1]->Width();
        DINT _Y = Pyr[i - 1]->Height();

        Pyr[i]->Squeeze4();
        CFourierImage *mask = new CFourierImage(Pyr[i]->Width(), Pyr[i]->Height());
        mask->MakeGaussMask(GaussKernel);
        mask->Transform(TRUE);
        Pyr[i]->Transform(TRUE);
        *(Pyr[i]) *= *mask;
        Pyr[i]->Transform(FALSE);
        delete mask;
        MinV[i - 1] = 1.0e30f;
        MaxV[i - 1] = -1.0e30f;
        for (DINT y = 0; y < _Y; ++y)
        {
            for (DINT x = 0; x < x_X; ++x)
            {
                float v;
                if (LogScale)
                    v = log(max(1.0f, Pyr[i - 1]->_Re(x, y)) /
                            max(1.0f, Pyr[i]->ReBiLine(x / 2.0f, y / 2.0f)));
                else
                    v = Pyr[i - 1]->_Re(x, y) - Pyr[i]->ReBiLine(x / 2.0f, y / 2.0f);

                Pyr[i - 1]->_Re(x, y) = v;

                if (MaxV[i - 1] < v)
                    MaxV[i - 1] = v;
                if (MinV[i - 1] > v)
                    MinV[i - 1] = v;
            }
        }
    }
    float gMinV = 1.0e30f;
    float gMaxV = -1.0e30f;
    for (i = n - 2; i >= 0; --i)
    {
        if (gMaxV < MaxV[i])
            gMaxV = MaxV[i];
        if (gMinV > MinV[i])
            gMinV = MinV[i];
    }
    for (i = n - 2; i >= 0; --i)
    {
        float _sigma = LogScale ? sigma : sigma * (MaxV[i] - MinV[i]) / 4.0f;
        DINT x_X = Pyr[i]->Width();
        DINT _Y = Pyr[i]->Height();
        DINT _XY = x_X * _Y;
        float scale = __scaleFactor(i, n, ScaleCenter, ScaleAlpha);
        for (DINT k = 0; k < _XY; ++k)
        {
            float v = Pyr[i]->ReImage[k];
            float _v = (__filter(v, _sigma, alpha, s_beta, l_beta) - v) * scale + v;
            Pyr[i]->ReImage[k] = _v;
        }

        for (DINT y = 0; y < _Y; ++y)
        {
            DINT l = y * x_X;
            for (DINT x = 0; x < x_X; ++x)
                if (LogScale)
                    Pyr[i]->_Re(x, y) =
                            Pyr[i + 1]->ReBiLine(x / 2.0f, y / 2.0f) * exp(Pyr[i]->_Re(x, y));
                else
                    Pyr[i]->_Re(x, y) = max(1.0f, Pyr[i]->_Re(x, y) +
                                                  Pyr[i + 1]->ReBiLine(x / 2.0f, y / 2.0f));
        }
    }
    Pyr[0]->GetRe(this);
    for (i = 0; i < n; ++i)
        delete Pyr[i];
}

//====================================================================================
//
//====================================================================================
int __cdecl compare_INT32(const void *elem1, const void *elem2)
{
    int *a = (int *) elem1;
    int *b = (int *) elem2;
    if (*a > *b)
        return 1;
    else if (*a < *b)
        return -1;
    return 0;
}

//====================================================================================
//
//====================================================================================
void CMathImage::MedianSmooth(DINT w)
{
    INT32 *data = (INT32 *) HMALLOCZ(X * Y * sizeof(INT32));
    DINT L = 2 * w + 1;
    INT32 *buff = (INT32 *) _alloca(L * L * sizeof(INT32));
    if (data && buff)
    {
        for (DINT j = 0; j < Y; ++j)
        {
            DINT line = j * X;
            for (DINT i = 0; i < X; ++i)
            {
                DINT n = 0;
                for (DINT m = max(j - w, (DINT) 0) * X, lim_m = min((DINT) Y - 1, j + w) * X;
                     m <= lim_m; m += X)
                {
                    for (DINT k = max(i - w, (DINT) 0), lim_k = min((DINT) X - 1, i + w);
                         k <= lim_k; ++k)
                    {
                        buff[n] = Buffer[k + m];
                        ++n;
                    }
                }
                if (n > 0)
                {
                    qsort(buff, n, sizeof(INT32), compare_INT32);
                    if (n % 2 == 1)
                        data[i + line] = buff[n / 2];
                    else
                    {
                        DINT k = n / 2;
                        data[i + line] = (buff[k] + buff[k - 1]) / 2;
                    }
                }
            }
        }
        if (Buffer)
            HFREE(Buffer);
        Buffer = data;
        data = NULL;
    }
    if (data)
        HFREE(data);
}

//====================================================================================
//
//====================================================================================
void CMathImage::MedianSmoothNonZero(DINT w)
{
    INT32 *data = (INT32 *) HMALLOCZ(X * Y * sizeof(INT32));
    DINT L = 2 * w + 1;
    INT32 *buff = (INT32 *) _alloca(L * L * sizeof(INT32));
    if (data && buff)
    {
        for (DINT i = 0; i < X; ++i)
        {
            for (DINT j = 0, _j = 0; j < Y; ++j, _j += X)
            {
                DINT n = 0;
                if (Buffer[i + _j])
                {
                    for (DINT m = max(j - w, (DINT) 0) * X, lim_m = min((DINT) Y - 1, j + w) * X;
                         m <= lim_m; m += X)
                    {
                        for (DINT k = max(i - w, (DINT) 0), lim_k = min((DINT) X - 1, i + w);
                             k <= lim_k; ++k)
                        {
                            if (Buffer[k + m])
                            {
                                buff[n] = Buffer[k + m];
                                ++n;
                            }
                        }
                    }
                    if (n > 0)
                    {
                        qsort(buff, n, sizeof(INT32), compare_INT32);
                        if (n % 2 == 1)
                            data[i + _j] = buff[(n - 1) / 2];
                        else
                            data[i + _j] = buff[(n - 1) / 2 + 1];
                    }
                }
            }
        }
        if (Buffer)
            HFREE(Buffer);
        Buffer = data;
        data = NULL;
    }
    if (data)
        HFREE(data);
}

//====================================================================================
//
//====================================================================================
void CMathImage::MedianSmooth(DINT w, DINT left, DINT top, DINT width, DINT height)
{
    if (left < 0)
        left = 0;
    if (left > X)
        left = X - 1;
    if (top < 0)
        top = 0;
    if (top > Y)
        top = Y - 1;
    if (width <= 0 || width > X - left)
        width = X - left;
    if (height <= 0 || height > Y - top)
        height = Y - top;
    MArray<DINT> _data(X * Y);
    DINT *data = _data.GetPtr();
    DINT L = 2 * w + 1;
    DINT n = L * L;
    INT32 *buff = (INT32 *) _alloca(n * sizeof(INT32));
    if (data && buff)
    {
        memcpy(data, Buffer, sizeof(INT32) * X * Y);
        for (DINT i = left, lim_X = left + width; i < lim_X; ++i)
        {
            for (DINT j = top, lim_Y = top + height; j < lim_Y; ++j)
            {
                DINT n = 0;
                for (DINT m = max(j - w, (DINT) 0) * X, lim_m = min((DINT) Y - 1, j + w) * X;
                     m <= lim_m; m += X)
                {
                    for (DINT k = max(i - w, (DINT) 0), lim_k = min((DINT) X - 1, i + w);
                         k <= lim_k; ++k)
                    {
                        buff[n] = Buffer[k + m];
                        ++n;
                    }
                }
                if (n > 0)
                {
                    qsort(buff, n, sizeof(INT32), compare_INT32);
                    data[i + j * X] = buff[(n - 1) / 2];
                }
            }
        }
        memcpy(Buffer, data, sizeof(INT32) * X * Y);
    }

}

void CMathImage::Get3BytesImage(CMathImage img[3], SimpleLUT lut[3], MemImage &bmp, int X, int Y)
{
    DINT i, j;
    DINT Lim = X * Y;
    double scale_k;
    double min_i_k;
    double max_i_k;
    MArray<USHORT> dst(Lim * 3);
    USHORT *Dst = dst.GetPtr();
    //memset(Dst, 0, Lim * 3 * sizeof(USHORT));

    for (int k = 0; k < 3; ++k)
    {
        min_i_k = lut[k].Min;
        max_i_k = lut[k].Max;
        int *img_k = img[k].Buffer;
        scale_k = 256.0 / (fabs(max_i_k - min_i_k) + 1.0);
        for (i = 0, j = k; i < Lim; ++i, j += 3)
        {
            double v = (img_k[i] - min_i_k) * scale_k;
            if (v > 0.0)
                Dst[j] += int(v);
        }

    }
    int width = X * 4;
    BYTE *_buf = (BYTE *) bmp.GetPtr();
    int Len = X * Y;
    for (i = 0; i < Y; ++i)
    {
        for (int j = 0; j < X; ++j)
        {

            INT32 _r = Dst[j * 3];
            INT32 _g = Dst[j * 3 + 1];
            INT32 _b = Dst[j * 3 + 2];
            if (_r < 0)
                _r = 0;
            if (_r > 255)
                _r = 255;

            if (_g < 0)
                _g = 0;
            if (_g > 255)
                _g = 255;

            if (_b < 0)
                _b = 0;
            if (_b > 255)
                _b = 255;

            _buf[j * 4 + 2] = (BYTE) _b;
            _buf[j * 4 + 1] = (BYTE) _g;
            _buf[j * 4] = (BYTE) _r;
        }
        _buf += width;
        Dst += X * 3;
    }
}

void CMathImage::GetGrayScaleImage(CMathImage &img, SimpleLUT lut, MemImage &bmp, int X, int Y)
{

    DINT i, j;
    DINT Lim = X * Y;
    double scale_k;
    double min_i_k;
    double max_i_k;
    MArray<USHORT> dst(Lim);
    USHORT *Dst = dst.GetPtr();
    int Min, Max;
    img.MinMax(Min, Max);
    lut.Min = Min;
    lut.Max = Max;
    min_i_k = lut.Min;
    max_i_k = lut.Max;
    int *img_k = img.Buffer;
    scale_k = 256.0 / (fabs(max_i_k - min_i_k) + 1.0);
    for (i = 0, j = 0; i < Lim; ++i, ++j)
    {
        double v = (img_k[i] - min_i_k) * scale_k;
        if (v > 0.0)
            Dst[j] += int(v);
    }
    int width = X * 4;
    BYTE *_buf = (BYTE *) bmp.GetPtr();
    int Len = X * Y;
    for (i = 0; i < Y; ++i)
    {
        for (int j = 0; j < X; ++j)
        {

            INT32 _r = Dst[j];
            if (_r < 0)
                _r = 0;
            if (_r > 255)
                _r = 255;
            _r = 255 - _r;
            _buf[j * 4 + 2] = (BYTE) _r;
            _buf[j * 4 + 1] = (BYTE) _r;
            _buf[j * 4] = (BYTE) _r;
        }
        _buf += width;
        Dst += X;
    }
    /*
	int Min, Max;
	img.MinMax(Min, Max);
	double n = Min ;
	double x = Max;
	BYTE *_buf = (BYTE *)bmp.GetPtr();
	int sizeX = X * 4;

		double d = (x - n)/255.0;
		BYTE b;
		for (int k = 0, i = 0; k < Y; ++k)
		{
			for (int j = 0; j < X; ++j)
			{
				if (img.Buffer[i + j] > Min)
				{
					b = 255 - (BYTE)(((double)img.Buffer[i + j] - n)/d + 0.5);

					_buf[j * 4] = b & 255;
					_buf[j * 4 + 1] = b & 255;
					_buf[j * 4 + 2] = b & 255;
				}
				else
				{
					_buf[j * 4] = 255;
					_buf[j * 4 + 1] = 255;
					_buf[j * 4 + 2] = 255;
				}

			}
			i += X;
			_buf += sizeX;

		}
*/

}

void CMathImage::__ConvolveWith(int MaskType, double width)
{
    int N = 0;
    double corr = 0.0;
    double k = 1.0 / (sqrt(2.0) * width);
    double width2 = width * width;
    double k2 = k * k;
    do
    {
        ++N;
        double dx = (double) N;
        double f1 = 0.5 * (Erf((dx + 0.5) * k) - Erf((dx - 0.5) * k));
        corr = 0.0;
        for (int y = -N; y <= N; ++y)
        {
            double dy = (double) y;
            double f2 = 0.5 * (Erf((dy + 0.5) * k) - Erf((dy - 0.5) * k));
            corr += f2;
        }
        corr *= 4.0 * f1;
        corr -= 4.0 * f1 * f1;
    } while (corr >= 0.05);
    int wnd = 2 * N + 1;
    double *Mask = (double *) HMALLOC(wnd * wnd * sizeof(double));
    int count = 0;
    if (MaskType == 0)
    {
        for (int y = -N; y <= N; ++y)
        {
            double dy = (double) y;
            double f1 = 0.5 * (Erf((dy + 0.5) * k) - Erf((dy - 0.5) * k));
            for (int x = -N; x <= N; ++x)
            {
                double dx = (double) x;
                double f2 = 0.5 * (Erf((dx + 0.5) * k) - Erf((dx - 0.5) * k));
                Mask[count++] = f1 * f2;
            }
        }
    } else if (MaskType == 1) // Laplacian of Gaussian
    {
        double scale = 1.0 / (2.0 * sqrt(2.0 * PI) * width * width * width);
        for (int y = -N; y <= N; ++y)
        {
            double dy = (double) y;
            double f1y = Erf((dy + 0.5) * k) - Erf((dy - 0.5) * k);
            double f2y = exp(-k2 * (dy - 0.5) * (dy - 0.5));
            double ey = exp(-dy / width2);
            double f3y = dy * (1.0 - ey) - 0.5 * (1.0 + ey);
            for (int x = -N; x <= N; ++x)
            {
                double dx = (double) x;
                double f1x = Erf((dx + 0.5) * k) - Erf((dx - 0.5) * k);
                double f2x = exp(-k2 * (dx - 0.5) * (dx - 0.5));
                double ex = exp(-dx / width2);
                double f3x = dx * (1.0 - ex) - 0.5 * (1.0 + ex);
                Mask[count++] = scale * (f2x * f1y * f3x + f2y * f1x * f3y);
            }
        }
    } else if (MaskType == 2) // GradXGauss
    {
        double scale = 1.0 / (2.0 * sqrt(2.0 * PI) * width);
        for (int y = -N; y <= N; ++y)
        {
            double dy = (double) y;
            double f1y = Erf((dy + 0.5) * k) - Erf((dy - 0.5) * k);
            for (int x = -N; x <= N; ++x)
            {
                double dx = (double) x;
                double f2x = exp(-k2 * (dx - 0.5) * (dx - 0.5));
                double ex = exp(-dx / width2) - 1.0;
                Mask[count++] = scale * f2x * ex * f1y;
            }
        }
    } else if (MaskType == 3) // GradYGauss
    {
        double scale = 1.0 / (2.0 * sqrt(2.0 * PI) * width);
        for (int y = -N; y <= N; ++y)
        {
            double dy = (double) y;
            double f2y = exp(-k2 * (dy - 0.5) * (dy - 0.5));
            double ey = exp(-dy / width2) - 1.0;
            for (int x = -N; x <= N; ++x)
            {
                double dx = (double) x;
                double f1x = Erf((dx + 0.5) * k) - Erf((dx - 0.5) * k);
                Mask[count++] = scale * f2y * ey * f1x;
            }
        }
    }
    INT32 *tBuffer = (INT32 *) HMALLOCZ(sizeof(INT32) * X * Y);
    if (tBuffer)
    {
        for (DINT y = 0; y < Y; ++y)
        {
            DINT line = y * X;
            for (DINT x = 0; x < X; ++x)
            {
                DINT pos = x + line;
                double sum = 0.0;
                double w = 0.0;
                count = 0;
                for (DINT _y = y - N, lim_y = y + N; _y <= lim_y; ++_y)
                {
                    DINT _line = _y * X;
                    for (DINT _x = x - N, lim_x = x + N; _x <= lim_x; ++_x)
                    {
                        double _w = Mask[count++];
                        if (_x >= 0 && _x < X && _y >= 0 && _y < Y)
                        {
                            DINT _pos = _line + _x;
                            sum += Buffer[_pos] * _w;
                            w += _w;
                        }
                    }
                }
                tBuffer[pos] = (int) (sum / w);
            }
        }
        HFREE(Buffer);
        Buffer = tBuffer;
        tBuffer = NULL;
    }
    if (tBuffer)
        HFREE(tBuffer);
}

void CMathImage::DeconvolveWithGauss(double width, int step)
{
    CFourierImage *mask = new CFourierImage(X, Y);
    if (mask && mask->IsValid())
    {
        mask->MakeGaussMask((float) width);
        mask->Transform(TRUE);
        CMathImage Result(X, Y);
        CFourierImage data(step, step);
        CFourierImage result(step, step);
        DINT _step = step - DINT(4.0 * width);
        for (DINT x = 0; x < X; x += _step)
        {
            DINT _width = min((DINT) X - x, (DINT) step);
            if (_width <= 4.0 * width)
                continue;
            for (DINT y = 0; y < Y; y += _step)
            {
                DINT _height = min((DINT) Y - y, (DINT) step);
                if (_height <= 4.0 * width)
                    continue;
                data.Clear();
                data.SetReImage(*this, x, y, _width, _height, 0, 0);
                result.Clear();
                result.SetReImage(Result, x, y, _width, _height, 0, 0);
                //			Deconvolve(data, result, mask, DINT(4.0 * width));
                result.GetRe(Result, 0, 0, step, step, x, y);
            }
        }
    }
    delete mask;
}

//======================================================
//
//======================================================
void CFourierImage::GetRe(CMathImage &dst, DINT off_x, DINT off_y, DINT width, DINT height,
                          DINT dst_off_x, DINT dst_off_y)
{
    if (Valid)
    {
        DINT X_1 = width + off_x;
        DINT _Y = height + off_y;

        if (X_1 > X)
            X_1 = X;
        if (_Y > Y)
            _Y = Y;
        if (X_1 - off_x + dst_off_x > dst.Width())
            X_1 = off_x - dst_off_x + dst.Width();
        if (_Y - off_y + dst_off_y > dst.Height())
            _Y = off_y - dst_off_y + dst.Height();
        DINT start_x = max((DINT) 0, off_x);
        DINT start_y = max((DINT) 0, off_y);
        if (dst_off_x < 0)
        {
            start_x -= dst_off_x;
            dst_off_x = 0;
        }
        if (dst_off_y < 0)
        {
            start_x -= dst_off_y;
            dst_off_y = 0;
        }
        for (DINT x = start_x, _x = dst_off_x; x < X_1; ++x, ++_x)
            for (DINT y = start_y, _y = dst_off_y; y < _Y; ++y, ++_y)
                dst(_x, _y) = INT32(_Re(x, y));
    }
}

//======================================================
//
//======================================================
void
CFourierImage::SetReImage(CMathImage &src, DINT off_src_x, DINT off_src_y, DINT width, DINT height,
                          DINT off_x, DINT off_y)
{
    float *_ReImage = ReImage;
    if (off_y < 0)
        off_y = 0;
    if (off_x < 0)
        off_x = 0;
    if (off_src_x < 0)
        off_src_x = 0;
    if (off_src_y < 0)
        off_src_y = 0;
    DINT lim_x = min(min(X - off_x, src.Width() - off_src_x), width) + off_src_x;
    DINT lim_y = min(min(Y - off_y, src.Height() - off_src_y), height) + off_src_y;
    for (DINT x = off_x, _x = off_src_x; x < lim_x; ++x, ++_x)
        for (DINT y = off_y, _y = off_src_y; y < lim_y; ++y, ++_y)
            _ReImage[x + X * y] = (float) src(_x, _y);
}

//======================================================
//
//======================================================
void CFourierImage::GetRe(FMathImage &dst, DINT off_x, DINT off_y, DINT width, DINT height,
                          DINT dst_off_x, DINT dst_off_y)
{
    if (Valid)
    {
        DINT X_1 = width + off_x;
        DINT _Y = height + off_y;

        if (X_1 > X)
            X_1 = X;
        if (_Y > Y)
            _Y = Y;
        if (X_1 - off_x + dst_off_x > dst.Width())
            X_1 = off_x - dst_off_x + dst.Width();
        if (_Y - off_y + dst_off_y > dst.Height())
            _Y = off_y - dst_off_y + dst.Height();
        DINT start_x = max((DINT) 0, off_x);
        DINT start_y = max((DINT) 0, off_y);
        if (dst_off_x < 0)
        {
            start_x -= dst_off_x;
            dst_off_x = 0;
        }
        if (dst_off_y < 0)
        {
            start_x -= dst_off_y;
            dst_off_y = 0;
        }
        for (DINT x = start_x, _x = dst_off_x; x < X_1; ++x, ++_x)
            for (DINT y = start_y, _y = dst_off_y; y < _Y; ++y, ++_y)
                dst(_x, _y) = _Re(x, y);
    }
}

//======================================================
//
//======================================================
void CFourierImage::SetReImageEx(FMathImage &src, DINT off_src_x, DINT off_src_y, DINT width,
                                 DINT height, DINT off_x, DINT off_y)
{
    float *_ReImage = ReImage;
    if (off_y < 0)
        off_y = 0;
    if (off_x < 0)
        off_x = 0;
    if (off_src_x < 0)
        off_src_x = 0;
    if (off_src_y < 0)
        off_src_y = 0;
    DINT lim_x = min(min(X - off_x, src.Width() - off_src_x), width) + off_src_x;
    DINT lim_y = min(min(Y - off_y, src.Height() - off_src_y), height) + off_src_y;
    for (DINT x = off_x, _x = off_src_x; x < X; ++x, ++_x)
    {
        DINT __x = _x;
        if (_x >= lim_x)
            __x = lim_x - 1;
        for (DINT y = off_y, _y = off_src_y; y < Y; ++y, ++_y)
        {
            DINT __y = _y;
            if (_y >= lim_y)
                __y = lim_y - 1;

            _ReImage[x + X * y] = src(__x, __y);
        }
    }
}