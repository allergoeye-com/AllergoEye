#ifndef __FMATH_IMAGE_
#define __FMATH_IMAGE_

#include "FImage.h"


struct LPFilterParam;
#pragma pack (push)
#pragma pack(4)
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
EXPORTNIX class FMathImage {
    friend class LaplasePyramide;

public:
    int X;
    int Y;
    int Len;
    float *Buffer;
    float Dump;
    BOOL Valid;

    void
    InitFindThresholdByMaxEntropy(CMathImage *Mask, float *&var, float *&log_g, float *&log_var,
                                  float &Min, float &Max, float &Mean, float &MeanVar, BOOL VarEnt);

    BOOL CheckThreshold(CMathImage *Mask, float Threshold, float *var, float *log_g, float *log_var,
                        BOOL SumOfEntropy, BOOL VarEnt);

public:
    FMathImage(DINT aX = 4, DINT aY = 4);

    FMathImage(short *buff, DINT aX, DINT aY);

    FMathImage(INT32 *buff, DINT aX, DINT aY);

    FMathImage(float *buff, DINT aX, DINT aY);

    FMathImage(CMathImage &src);

    FMathImage(CMathImage &src, int squeeze);

    FMathImage(FMathImage &src);

    FMathImage(FMathImage &src, int squeeze);

    FMathImage(CFourierImage &src);

    ~FMathImage()
    { if (Buffer) HFREE(Buffer); }

    void SetSize_0(int aX, int aY);

    float FindMaximum(DINT &x, DINT &y);

    float FindMinimum(DINT &x, DINT &y);

    FMathImage &operator=(FMathImage &op);

    float *Data() const
    { return Buffer; }

    float GetValue(DINT x, DINT y);

    float GetValue(DINT x, DINT y, DINT &index);

    float PutValue(DINT x, DINT y, float f);

    float &Pixel(DINT x, DINT y);

    float GetBiLineValue(float x, float y);

    DINT Width() const
    { return X; }

    DINT Height() const
    { return Y; }

    FMathImage &operator*=(const FMathImage &op);

    FMathImage &operator/=(const FMathImage &op);

    FMathImage &operator+=(const FMathImage &op);

    FMathImage &operator-=(const FMathImage &op);

    FMathImage &operator*=(float);

    FMathImage &operator/=(float);

    FMathImage &operator+=(float);

    FMathImage &operator-=(float);

    float &operator()(DINT x, DINT y);

    void Power(float);

    void Ln();

    void Exp();

    void fAbs();

    void Squeeze4();

    void Stretch4();

    void Reset()
    {
        if (Buffer) HFREE(Buffer);
        Buffer = NULL;
        Len = X = Y = 0;
    }

    void Clear()
    { if (Buffer) memset(Buffer, 0, sizeof(float) * Len); }

    float Integral(DINT off_x, DINT off_y, DINT width, DINT height);

    float GetMeanValue();

    FMathImage *GetMeanSD(float &Mean, float &MeanSD, DINT w);

    BOOL IsValid()
    { return Valid; }

    void GetData(CMathImage &dst);

    void SetImage(FMathImage &img, DINT off_x = 0, DINT off_y = 0);

    void LaplacePyramidFilter(LPFilterParam *Params, int nParams, BOOL LogScale);

    float FindThresholdByMaxEntropy(BOOL &IsThreshold, float &Mean, float &MeanVar,
                                    BOOL SumOfEntropy = FALSE, BOOL VarEnt = TRUE);

    float
    FindThresholdByMaxEntropy(CMathImage &Mask, BOOL &IsThreshold, float &Mean, float &MeanVar,
                              BOOL SumOfEntropy = FALSE, BOOL VarEnt = TRUE);

    float GetEntropy(float *Threshold, int NumbThres, BOOL Sum);

    float GetEntropy(FMathImage &SecondImage, float *Threshold, int NumbThres, BOOL Sum);

    void ConvolveWithGauss(double width);

    void ConvolveWithLaplacianOfGauss(double width);

    void ConvolveWithGradXOfGauss(double width);

    void ConvolveWithGradYOfGauss(double width);

    void ConvolveWithMask(CFourierImage &mask);

    void _ConvolveWith(int MaskType, double width);

    void _Squeeze(float *buffer, int X1, int start, int end, int squeeze, float scale);

    void MedianSmoothNonZero(DINT w);

    void MedianSmooth(DINT w);

    void MedianSmooth(DINT w, DINT left, DINT top, DINT width, DINT height);

};
#pragma pack(pop)

//======================================================
//
//======================================================
inline float &FMathImage::operator()(DINT x, DINT y)
{
    if (x < 0)
        x = 0;
    else if (x >= X)
        x = X - 1;
    if (y < 0)
        y = 0;
    else if (y >= Y)
        y = Y - 1;
    return Buffer[x + y * X];
}

//======================================================
//
//======================================================
inline float FMathImage::GetValue(DINT x, DINT y)
{
    if (x < 0)
        x = 0;
    else if (x >= X)
        x = X - 1;
    if (y < 0)
        y = 0;
    else if (y >= Y)
        y = Y - 1;
    return Buffer[x + y * X];
}

//======================================================
//
//======================================================
inline float FMathImage::PutValue(DINT x, DINT y, float f)
{
    if (x >= 0 && x < X && y >= 0 && y < Y)
        return Buffer[x + y * X] = f;
    else
        return f;
}

//======================================================
//
//======================================================
inline float &FMathImage::Pixel(DINT x, DINT y)
{
    if (x >= 0 && x < X && y >= 0 && y < Y)
        return Buffer[x + y * X];
    else
        return Dump;
}

//====================================================================================
//
//====================================================================================
inline float FMathImage::GetValue(DINT x, DINT y, DINT &index)
{
    if (x < 0)
        x = 0;
    else if (x >= X)
        x = X - 1;
    if (y < 0)
        y = 0;
    else if (y >= Y)
        y = Y - 1;
    index = x + y * X;
    return Buffer[index];
}

//======================================================
//
//======================================================
inline float FMathImage::GetBiLineValue(float x, float y)
{
    DINT ix = DINT(x);
    DINT iy = DINT(y);

    if (ix < 0)
        ix = 0;
    else if (ix >= X)
        ix = X - 1;
    if (iy < 0)
        iy = 0;
    else if (iy >= Y)
        iy = Y - 1;

    float dx = x - (float) ix;
    float dy = y - (float) iy;

    DINT pos = ix + iy * X;

    if (dx == 0.0f)
    {
        if (dy == 0.0f)
            return Buffer[pos];
        float v1 = Buffer[pos];
        DINT pos1 = pos + X;
        if (pos1 >= Len)
            pos1 = pos;
        float v2 = Buffer[pos1];
        return (v2 - v1) * dy + v1;
    } else if (dy == 0.0f)
    {
        float v1 = Buffer[pos];
        DINT pos1;
        if (ix == X - 1)
            pos1 = pos;
        else
            pos1 = pos + 1;
        float v2 = Buffer[pos1];
        return (v2 - v1) * dx + v1;
    } else
    {
        float v1 = Buffer[pos];
        DINT pos1;
        if (ix == X - 1)
            pos1 = pos;
        else
            pos1 = pos + 1;
        float v2 = Buffer[pos1];

        DINT pos2 = pos1 + X;
        if (pos2 >= Len)
            pos2 = pos1;

        float v3 = Buffer[pos2];

        DINT pos3 = pos + X;
        if (pos3 >= Len)
            pos3 = pos;

        float v4 = Buffer[pos3];
        float a = (v2 - v1) * dx + v1;
        float b = (v3 - v4) * dx + v4;
        return (b - a) * dy + a;
    }
}

#endif
