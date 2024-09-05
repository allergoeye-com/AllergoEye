#include "stdafx.h"
#include "LaplasePyramide.h"
#include "MultiThread.h"

extern MultiThreadS *pMultiThreads;

UINT AFX_CDECL _LaplacePyramide_logLPyr0(void *Param);

UINT AFX_CDECL _LaplacePyramide_filter(void *Param);

UINT AFX_CDECL _LaplacePyramide_logLPyr0(void *Param);

UINT AFX_CDECL _LaplacePyramide_build(void *Param);

UINT AFX_CDECL _LaplacePyramide_ReconstructPyr(void *Param);

UINT AFX_CDECL _LaplacePyramide_ReconstructLPyr(void *Param);

UINT AFX_CDECL _LaplacePyramide_BuildGPyr(void *Param);

UINT AFX_CDECL _LaplacePyramide_ReconstructGPyr(void *Param);

struct ThreadLaplacePyramideParam {
    LaplacePyramide *Self;
    int Level;
    LPFilterParam *Param;
    DINT start;
    DINT finish;
    BOOL LogScale;
    float LMin;
    float LMax;
    float GMin;
    float GMax;
    BOOL Product;
};

//=====================================================
//
//=====================================================
LaplacePyramide::LaplacePyramide(FMathImage &anImage, int aLevel, BOOL aLogScale)
{
    LPyr = RPyr = RLPyr = NULL;
    GPyr = RGPyr = NULL;
    LScale = 1.0f;
    nLevels = 0;
    DINT L = min(anImage.X, anImage.Y);
    int MaxL = 1;
    while (L > 8)
    {
        L /= 2;
        ++MaxL;
    }
    nLevels = min(MaxL, aLevel);
    LogScale = aLogScale;

    LPyr = (FMathImage *) HMALLOCZ(sizeof(FMathImage) * (nLevels + 1));
    RPyr = (FMathImage *) HMALLOCZ(sizeof(FMathImage) * (nLevels + 1));
    RLPyr = (FMathImage *) HMALLOCZ(sizeof(FMathImage) * (nLevels + 1));

    GPyr = (FMathImage (*)[2]) HMALLOCZ(sizeof(FMathImage) * 2 * (nLevels + 1));
    RGPyr = (FMathImage (*)[2]) HMALLOCZ(sizeof(FMathImage) * 2 * (nLevels + 1));

    if (LPyr && GPyr && RPyr && RLPyr && RGPyr)
    {
        new(LPyr) FMathImage(anImage);
        Init();
    }
}

//=====================================================
//
//=====================================================
LaplacePyramide::LaplacePyramide(CMathImage &anImage, int aLevel, BOOL aLogScale)
{
    LPyr = RPyr = RLPyr = NULL;
    GPyr = RGPyr = NULL;
    LScale = 1.0f;
    nLevels = 0;
    DINT L = min(anImage.Width(), anImage.Height());
    int MaxL = 1;
    while (L > 8)
    {
        L /= 2;
        ++MaxL;
    }
    nLevels = min(MaxL, aLevel);
    LogScale = aLogScale;

    LPyr = (FMathImage *) HMALLOCZ(sizeof(FMathImage) * (nLevels + 1));
    RPyr = (FMathImage *) HMALLOCZ(sizeof(FMathImage) * (nLevels + 1));
    RLPyr = (FMathImage *) HMALLOCZ(sizeof(FMathImage) * (nLevels + 1));

    GPyr = (FMathImage (*)[2]) HMALLOCZ(sizeof(FMathImage) * 2 * (nLevels + 1));
    RGPyr = (FMathImage (*)[2]) HMALLOCZ(sizeof(FMathImage) * 2 * (nLevels + 1));

    if (LPyr && GPyr && RPyr && RLPyr && RGPyr)
    {
        new(LPyr) FMathImage(anImage);
        Init();
    }
}

//=====================================================
//
//=====================================================
void LaplacePyramide::Init()
{
    int NThreads = pMultiThreads->Num();
    DINT step = LPyr[0].Len / NThreads;
    ThreadLaplacePyramideParam *tParam = (ThreadLaplacePyramideParam *) _alloca(
            sizeof(ThreadLaplacePyramideParam) * (NThreads + 1));
    memset(tParam, 0, sizeof(ThreadLaplacePyramideParam) * (NThreads + 1));

    tParam[0].Self = this;
    tParam[0].Level = 0;
    if (LogScale)
    {
        float *buff = (float *) HMALLOC(sizeof(float) * LPyr[0].Len);
        if (buff)
        {
            memcpy(buff, LPyr[0].Buffer, sizeof(float) * LPyr[0].Len);
            if (NThreads > 1)
            {
                tParam[0].LogScale = TRUE;
                tParam[0].start = 0;
                tParam[0].finish = step;
                int m;
                for (m = 1; m < NThreads && tParam[m - 1].finish < LPyr[0].Len; ++m)
                {
                    memcpy(tParam + m, tParam, sizeof(ThreadLaplacePyramideParam));
                    tParam[m].start = tParam[m - 1].finish;
                    tParam[m].finish = tParam[m].start + step;
                }
                NThreads = m;
                tParam[NThreads - 1].finish = LPyr[0].Len;
                for (m = 0; m < NThreads; ++m)
                    pMultiThreads->Run(m, _LaplacePyramide_logLPyr0, tParam + m);
                pMultiThreads->Wait();
                for (m = 0; m < NThreads; ++m)
                {
                    if (tParam[m].LogScale == FALSE)
                    {
                        LogScale = FALSE;
                        break;
                    }
                }
            } else
                LogScale = logLPyr0(0, LPyr[0].Len);
        }
        if (LogScale == FALSE)
            memcpy(LPyr[0].Buffer, buff, sizeof(float) * LPyr[0].Len);
        HFREE(buff);
    }

    int i, j;
    for (i = 1, j = 0; i < nLevels; ++i, ++j)
    {
        new(LPyr + i) FMathImage(LPyr[j], 2);
        NThreads = pMultiThreads->Num();
        if (NThreads > 1)
        {
            DINT step = LPyr[j].Y / NThreads;
            tParam[0].Self = this;
            tParam[0].Level = j;
            tParam[0].start = 0;
            tParam[0].finish = step;
            tParam[0].LMin = 1.0e30f;
            tParam[0].LMax = -1.0e30f;
            tParam[0].GMin = 1.0e30f;
            tParam[0].GMax = -1.0e30f;
            int m;
            for (m = 1; m < NThreads && tParam[m - 1].finish < LPyr[j].Y; ++m)
            {
                memcpy(tParam + m, tParam, sizeof(ThreadLaplacePyramideParam));
                tParam[m].start = tParam[m - 1].finish;
                tParam[m].finish = tParam[m].start + step;
            }
            NThreads = m;
            tParam[NThreads - 1].finish = LPyr[j].Y;
            for (m = 0; m < NThreads; ++m)
                pMultiThreads->Run(m, _LaplacePyramide_build, tParam + m);
            pMultiThreads->Wait();
            for (m = 0; m < NThreads; ++m)
                LScale = max(LScale, (float) max(fabs(tParam[m].LMax), fabs(tParam[m].LMin)));
        } else
        {
            float LMin = 1.0e30f;
            float LMax = -1.0e30f;
            float GMin = 1.0e30f;
            float GMax = -1.0e30f;
            build(j, 0, LPyr[j].Y, LMin, LMax, GMin, GMax);
            LScale = max(LScale, (float) max(fabs(LMax), fabs(LMin)));
        }
    }
}

//=====================================================
//
//=====================================================
LaplacePyramide::~LaplacePyramide()
{
    if (LPyr)
    {
        for (int i = 0; i < nLevels; ++i)
            LPyr[i].Reset();
        HFREE(LPyr);
    }
    if (RPyr)
    {
        for (int i = 0; i < nLevels; ++i)
            RPyr[i].Reset();
        HFREE(RPyr);
    }
    if (RLPyr)
    {
        for (int i = 0; i < nLevels; ++i)
            RLPyr[i].Reset();
        HFREE(RLPyr);
    }
    if (GPyr)
    {
        for (int i = 0; i < nLevels; ++i)
        {
            GPyr[i][0].Reset();
            GPyr[i][1].Reset();
        }
        HFREE(GPyr);
    }
    if (RGPyr)
    {
        for (int i = 0; i < nLevels; ++i)
        {
            RGPyr[i][0].Reset();
            RGPyr[i][1].Reset();
        }
        HFREE(RGPyr);
    }
}

//======================================================
//
//======================================================
void LaplacePyramide::__filter(LPFilterParam &Param, int Level, DINT start, DINT finish)
{
    float sigma = Param.Scale * Param.sigma;
    float delta = Param.Scale * Param.delta;
    for (DINT k = start; k < finish; ++k)
    {
        float x = LPyr[Level].Buffer[k];
        float mx = fabs(x);
        if (mx <= sigma)
        {
            if (Param.alpha != 1.0f)
            {
                mx -= delta;
                if (mx > 0.0)
                {
                    int i = (int) (mx * 4093.0f / sigma);
                    LPyr[Level].Buffer[k] = sign(x) * (sigma * _filer_LUT[i] + delta);
                }
            }
        } else if (x > 0)
            LPyr[Level].Buffer[k] = Param.beta * (x - sigma) + sigma;
        else
            LPyr[Level].Buffer[k] = Param.beta * (x + sigma) - sigma;
    }
}

//======================================================
//
//======================================================
void LaplacePyramide::Init_filer_LUT(float alpha)
{
    for (int i = 0; i < 4096; ++i)
        _filer_LUT[i] = pow((float) i / 4095.0f, alpha);
}

//=====================================================
//
//=====================================================
void LaplacePyramide::PHKFilter(LPFilterParam *Params, int nParams)
{
    int NThreads = pMultiThreads->Num();
    ThreadLaplacePyramideParam *tParam = (ThreadLaplacePyramideParam *) _alloca(
            sizeof(ThreadLaplacePyramideParam) * (NThreads + 1));
    int i, j;
    for (i = nLevels - 2; i >= 0; --i)
    {
        LPFilterParam *aParam = NULL;
        for (j = 0; j < nParams; ++j)
        {
            if (Params[j].Level == i)
            {
                aParam = Params + j;
                break;
            }
        }
        if (aParam)
        {
            NThreads = pMultiThreads->Num();
            Init_filer_LUT(aParam->alpha);
            aParam->Scale = LScale;
            if (NThreads > 1)
            {
                DINT step = LPyr[i].Len / NThreads;
                tParam[0].Self = this;
                tParam[0].start = 0;
                tParam[0].finish = step;
                tParam[0].Param = aParam;
                tParam[0].Level = i;
                int m;
                for (m = 1; m < NThreads && tParam[m - 1].finish < LPyr[i].Len; ++m)
                {
                    memcpy(tParam + m, tParam, sizeof(ThreadLaplacePyramideParam));
                    tParam[m].start = tParam[m - 1].finish;
                    tParam[m].finish = tParam[m].start + step;
                }
                NThreads = m;
                tParam[NThreads - 1].finish = LPyr[i].Len;
                for (m = 0; m < NThreads; ++m)
                    pMultiThreads->Run(m, _LaplacePyramide_filter, tParam + m);
                pMultiThreads->Wait();
            } else
                __filter(*aParam, i, 0, LPyr[i].Len);
        }
    }
}

//=====================================================
//
//=====================================================
FMathImage *LaplacePyramide::ReconstructPyr(int level)
{
    if (level < 0 || level > nLevels - 1)
        return NULL;
    RPyr[nLevels - 1].Reset();
    new(RPyr + nLevels - 1) FMathImage(LPyr[nLevels - 1]);

    int NThreads = pMultiThreads->Num();
    ThreadLaplacePyramideParam *tParam = (ThreadLaplacePyramideParam *) _alloca(
            sizeof(ThreadLaplacePyramideParam) * (NThreads + 1));
    for (int i = nLevels - 2; i >= 0; --i)
    {
        RPyr[i].Reset();
        new(RPyr + i) FMathImage(LPyr[i]);

        NThreads = pMultiThreads->Num();
        if (NThreads > 1)
        {
            DINT step = RPyr[i].Y / NThreads;
            tParam[0].Self = this;
            tParam[0].start = 0;
            tParam[0].finish = step;
            tParam[0].Level = i;
            tParam[0].LogScale = LogScale;
            int m;
            for (m = 1; m < NThreads && tParam[m - 1].finish < RPyr[i].Len; ++m)
            {
                memcpy(tParam + m, tParam, sizeof(ThreadLaplacePyramideParam));
                tParam[m].start = tParam[m - 1].finish;
                tParam[m].finish = tParam[m].start + step;
            }
            NThreads = m;
            tParam[NThreads - 1].finish = RPyr[i].Y;

            for (m = 0; m < NThreads; ++m)
                pMultiThreads->Run(m, _LaplacePyramide_ReconstructPyr, tParam + m);

            pMultiThreads->Wait();
        } else
            ReconstructPyrLevel(i, 0, RPyr[i].Y);
    }
    if (LogScale)
    {
        for (int i = nLevels - 1; i >= 0; --i)
            for (int j = 0; j < RPyr[i].Len; ++j)
                RPyr[i].Buffer[j] = exp(RPyr[i].Buffer[j]);
    }
    return &RPyr[level];
}

//=====================================================
//
//=====================================================
FMathImage *LaplacePyramide::ReconstructLPyr(int Level_from, int Level_to, BOOL Product)
{
    if (nLevels == 0)
        return NULL;
    if (Level_to >= nLevels)
        Level_to = nLevels - 1;
    if (Level_from > Level_to)
        Level_from = Level_to;
    if (Level_from < 0)
        Level_from = 0;

    RLPyr[Level_to].Reset();
    new(RLPyr + Level_to) FMathImage(LPyr[Level_to]);

    int NThreads = pMultiThreads->Num();
    ThreadLaplacePyramideParam *tParam = (ThreadLaplacePyramideParam *) _alloca(
            sizeof(ThreadLaplacePyramideParam) * (NThreads + 1));
    for (int i = Level_to - 1; i >= Level_from; --i)
    {
        RLPyr[i].Reset();
        new(RLPyr + i) FMathImage(LPyr[i]);

        float _Min = 1.0e30f;
        float _Max = -1.0e30f;
        NThreads = pMultiThreads->Num();
        if (NThreads > 1)
        {
            DINT step = RLPyr[i].Y / NThreads;
            tParam[0].Self = this;
            tParam[0].start = 0;
            tParam[0].finish = step;
            tParam[0].LMin = _Min;
            tParam[0].LMax = _Max;
            tParam[0].Level = i;
            tParam[0].Product = Product;
            int m;
            for (m = 1; m < NThreads && tParam[m - 1].finish < RLPyr[i].Len; ++m)
            {
                memcpy(tParam + m, tParam, sizeof(ThreadLaplacePyramideParam));
                tParam[m].start = tParam[m - 1].finish;
                tParam[m].finish = tParam[m].start + step;
            }
            NThreads = m;
            tParam[NThreads - 1].finish = RLPyr[i].Y;

            for (m = 0; m < NThreads; ++m)
                pMultiThreads->Run(m, _LaplacePyramide_ReconstructLPyr, tParam + m);
            pMultiThreads->Wait();
        } else
            ReconstructLPyrLevel(i, 0, RLPyr[i].Y, Product, _Min, _Max);
    }
    return &RLPyr[Level_from];
}

//=====================================================
//
//=====================================================
FMathImage *LaplacePyramide::ReconstructGPyr(int Level_from, int Level_to, BOOL Product)
{
    if (nLevels == 0)
        return NULL;
    if (Level_to >= nLevels)
        Level_to = nLevels - 1;
    if (Level_from > Level_to)
        Level_from = Level_to;
    if (Level_from < 0)
        Level_from = 0;
    ReconstructPyr(0);
    int i;
    int NThreads = pMultiThreads->Num();
    ThreadLaplacePyramideParam *tParam = (ThreadLaplacePyramideParam *) _alloca(
            sizeof(ThreadLaplacePyramideParam) * (NThreads + 1));
    for (i = Level_to + 1; i >= Level_from; --i)
    {
        float _Min = 1.0e30f;
        float _Max = -1.0e30f;
        DINT X1 = RPyr[i].X;
        DINT Y1 = RPyr[i].Y;
        if (GPyr[i][0].X != X1 || GPyr[i][0].Y != Y1)
        {
            GPyr[i][0].Reset();
            new(&GPyr[i][0]) FMathImage(X1, Y1);
            GPyr[i][1].Reset();
            new(&GPyr[i][1]) FMathImage(X1, Y1);
        }
        NThreads = pMultiThreads->Num();
        if (NThreads > 1)
        {
            DINT step = RPyr[i].Y / NThreads;
            tParam[0].Self = this;
            tParam[0].start = 0;
            tParam[0].finish = step;
            tParam[0].Level = i;
            tParam[0].GMin = _Min;
            tParam[0].GMax = _Max;
            int m;
            for (m = 1; m < NThreads && tParam[m - 1].finish < RPyr[i].Len; ++m)
            {
                memcpy(tParam + m, tParam, sizeof(ThreadLaplacePyramideParam));
                tParam[m].start = tParam[m - 1].finish;
                tParam[m].finish = tParam[m].start + step;
            }
            NThreads = m;
            tParam[NThreads - 1].finish = RLPyr[i].Y;

            for (m = 0; m < NThreads; ++m)
                pMultiThreads->Run(m, _LaplacePyramide_BuildGPyr, tParam + m);

            pMultiThreads->Wait();
        } else
            BuildGPyrLevel(i, 0, RPyr[i].Y, _Min, _Max);
    }
    RGPyr[Level_to + 1][0].Reset();
    new(&RGPyr[Level_to + 1][0]) FMathImage(GPyr[Level_to + 1][0]);
    RGPyr[Level_to + 1][1].Reset();
    new(&RGPyr[Level_to + 1][1]) FMathImage(GPyr[Level_to + 1][1]);

    for (i = Level_to; i >= Level_from; --i)
    {
        float _Min = 1.0e30f;
        float _Max = -1.0e30f;
        DINT X1 = RPyr[i].X;
        DINT Y1 = RPyr[i].Y;
        RGPyr[i][0].Reset();
        new(&RGPyr[i][0]) FMathImage(GPyr[i][0]);
        RGPyr[i][1].Reset();
        new(&RGPyr[i][1]) FMathImage(GPyr[i][1]);

        NThreads = pMultiThreads->Num();
        if (NThreads > 1)
        {
            DINT step = RPyr[i].Y / NThreads;
            tParam[0].Self = this;
            tParam[0].start = 0;
            tParam[0].finish = step;
            tParam[0].Level = i;
            tParam[0].Product = Product;
            tParam[0].GMin = _Min;
            tParam[0].GMax = _Max;
            int m;
            for (m = 1; m < NThreads && tParam[m - 1].finish < RPyr[i].Len; ++m)
            {
                memcpy(tParam + m, tParam, sizeof(ThreadLaplacePyramideParam));
                tParam[m].start = tParam[m - 1].finish;
                tParam[m].finish = tParam[m].start + step;
            }
            NThreads = m;
            tParam[NThreads - 1].finish = RLPyr[i].Y;

            for (m = 0; m < NThreads; ++m)
                pMultiThreads->Run(m, _LaplacePyramide_ReconstructGPyr, tParam + m);

            pMultiThreads->Wait();
            for (m = 0; m < NThreads; ++m)
            {
                if (_Max < tParam[m].GMax)
                    _Max = tParam[m].GMax;
                if (_Min > tParam[m].GMin)
                    _Min = tParam[m].GMin;
            }
        } else
            ReconstructGPyrLevel(i, 0, RPyr[i].Y, Product, _Min, _Max);
    }

    return RGPyr[Level_from];
}

//===================================================================
//
//===================================================================
UINT AFX_CDECL _LaplacePyramide_filter(void *Param)
{
    ThreadLaplacePyramideParam *d = (ThreadLaplacePyramideParam *) Param;
    d->Self->__filter(*d->Param, d->Level, d->start, d->finish);
    return 0;
}

//===================================================================
//
//===================================================================
UINT AFX_CDECL _LaplacePyramide_build(void *Param)
{
    ThreadLaplacePyramideParam *d = (ThreadLaplacePyramideParam *) Param;
    d->Self->build(d->Level, d->start, d->finish, d->LMin, d->LMax, d->GMin, d->GMax);
    return 0;
}

//===================================================================
//
//===================================================================
UINT AFX_CDECL _LaplacePyramide_logLPyr0(void *Param)
{
    ThreadLaplacePyramideParam *d = (ThreadLaplacePyramideParam *) Param;
    d->LogScale = d->Self->logLPyr0(d->start, d->finish);
    return 0;
}

//===================================================================
//
//===================================================================
UINT AFX_CDECL _LaplacePyramide_ReconstructPyr(void *Param)
{
    ThreadLaplacePyramideParam *d = (ThreadLaplacePyramideParam *) Param;
    d->Self->ReconstructPyrLevel(d->Level, d->start, d->finish);
    return 0;
}

//===================================================================
//
//===================================================================
UINT AFX_CDECL _LaplacePyramide_ReconstructLPyr(void *Param)
{
    ThreadLaplacePyramideParam *d = (ThreadLaplacePyramideParam *) Param;
    d->Self->ReconstructLPyrLevel(d->Level, d->start, d->finish, d->Product, d->LMin, d->LMax);
    return 0;
}

//===================================================================
//
//===================================================================
UINT AFX_CDECL _LaplacePyramide_BuildGPyr(void *Param)
{
    ThreadLaplacePyramideParam *d = (ThreadLaplacePyramideParam *) Param;
    d->Self->BuildGPyrLevel(d->Level, d->start, d->finish, d->GMin, d->GMax);
    return 0;
}

//===================================================================
//
//===================================================================
UINT AFX_CDECL _LaplacePyramide_ReconstructGPyr(void *Param)
{
    ThreadLaplacePyramideParam *d = (ThreadLaplacePyramideParam *) Param;
    d->Self->ReconstructGPyrLevel(d->Level, d->start, d->finish, d->Product, d->GMin, d->GMax);
    return 0;
}

//=====================================================
//
//=====================================================
void LaplacePyramide::BuildGPyrLevel(int Level, DINT start, DINT finish, float &Min, float &Max)
{
    DINT X1 = RPyr[Level].X;
    DINT Y1 = RPyr[Level].Y;
    for (DINT y = start; y < finish; ++y)
    {
        DINT l = y * X1;
        for (DINT x = 0; x < X1; ++x)
        {
            DINT pos = l + x;
            if (x < X1 - 1)
                GPyr[Level][0].Buffer[pos] = RPyr[Level].Buffer[pos + 1] - RPyr[Level].Buffer[pos];
            else
                GPyr[Level][0].Buffer[pos] = RPyr[Level].Buffer[pos] - RPyr[Level].Buffer[pos - 1];

            if (y < Y1 - 1)
                GPyr[Level][1].Buffer[pos] = RPyr[Level].Buffer[pos + X1] - RPyr[Level].Buffer[pos];
            else
                GPyr[Level][1].Buffer[pos] = RPyr[Level].Buffer[pos] - RPyr[Level].Buffer[pos - X1];

            if (Max < GPyr[Level][0].Buffer[pos])
                Max = GPyr[Level][0].Buffer[pos];
            if (Min > GPyr[Level][0].Buffer[pos])
                Min = GPyr[Level][0].Buffer[pos];
            if (Max < GPyr[Level][1].Buffer[pos])
                Max = GPyr[Level][1].Buffer[pos];
            if (Min > GPyr[Level][1].Buffer[pos])
                Min = GPyr[Level][1].Buffer[pos];
        }
    }
}

//=====================================================
//
//=====================================================
void LaplacePyramide::ReconstructPyrLevel(int Level, DINT start, DINT finish)
{
    DINT X1 = RPyr[Level].X;
    for (DINT y = start; y < finish; ++y)
    {
        DINT l = y * X1;
        for (DINT x = 0; x < X1; ++x)
        {
            DINT pos = l + x;
            float _x = ((float) x - 0.5f) * 0.5f;
            float _y = ((float) y - 0.5f) * 0.5f;
            RPyr[Level].Buffer[pos] += RPyr[Level + 1].GetBiLineValue(_x, _y);
        }
    }
}

//=====================================================
//
//=====================================================
void
LaplacePyramide::ReconstructGPyrLevel(int Level, DINT start, DINT finish, BOOL Product, float &Min,
                                      float &Max)
{
    DINT X1 = RPyr[Level].X;
    DINT Y1 = RPyr[Level].Y;
    for (DINT y = start; y < finish; ++y)
    {
        DINT l = y * X1;
        for (DINT x = 0; x < X1; ++x)
        {
            DINT pos = l + x;
            float _x = ((float) x - 0.5f) * 0.5f;
            float _y = ((float) y - 0.5f) * 0.5f;
            if (Product)
            {
                RGPyr[Level][0].Buffer[pos] *= RGPyr[Level + 1][0].GetBiLineValue(_x, _y);
                RGPyr[Level][1].Buffer[pos] *= RGPyr[Level + 1][1].GetBiLineValue(_x, _y);
            } else
            {
                RGPyr[Level][0].Buffer[pos] += 0.5f * RGPyr[Level + 1][0].GetBiLineValue(_x, _y);
                RGPyr[Level][1].Buffer[pos] += 0.5f * RGPyr[Level + 1][1].GetBiLineValue(_x, _y);
            }
            if (Max < RGPyr[Level][0].Buffer[pos])
                Max = RGPyr[Level][0].Buffer[pos];
            if (Min > RGPyr[Level][0].Buffer[pos])
                Min = RGPyr[Level][0].Buffer[pos];
            if (Max < RGPyr[Level][1].Buffer[pos])
                Max = RGPyr[Level][1].Buffer[pos];
            if (Min > RGPyr[Level][1].Buffer[pos])
                Min = RGPyr[Level][1].Buffer[pos];
        }
    }
}

//=====================================================
//
//=====================================================
void
LaplacePyramide::ReconstructLPyrLevel(int Level, DINT start, DINT finish, BOOL Product, float &Min,
                                      float &Max)
{
    DINT X1 = RLPyr[Level].X;
    for (DINT y = start; y < finish; ++y)
    {
        DINT l = y * X1;
        for (DINT x = 0; x < X1; ++x)
        {
            DINT pos = l + x;
            float _x = ((float) x - 0.5f) * 0.5f;
            float _y = ((float) y - 0.5f) * 0.5f;
            if (Product)
                RLPyr[Level].Buffer[pos] *= RLPyr[Level + 1].GetBiLineValue(_x, _y);
            else
                RLPyr[Level].Buffer[pos] += 0.25f * RLPyr[Level + 1].GetBiLineValue(_x, _y);
            if (Max < RLPyr[Level].Buffer[pos])
                Max = RLPyr[Level].Buffer[pos];
            if (Min > RLPyr[Level].Buffer[pos])
                Min = RLPyr[Level].Buffer[pos];
        }
    }
}

//=====================================================
//
//=====================================================
BOOL LaplacePyramide::logLPyr0(DINT start, DINT finish)
{
    DINT m;
    for (m = start; m < finish; ++m)
    {
        if (LPyr[0].Buffer[m] > 0.0f)
            LPyr[0].Buffer[m] = log(LPyr[0].Buffer[m]);
        else
            break;
    }
    return m == finish;
}

//=====================================================
//
//=====================================================
void
LaplacePyramide::build(int Level, DINT start, DINT finish, float &LMin, float &LMax, float &GMin,
                       float &GMax)
{
    DINT X1 = LPyr[Level].Width();
    for (DINT y = start; y < finish; ++y)
    {
        DINT line = y * X1;
        for (DINT x = 0; x < X1; ++x)
        {
            DINT pos = line + x;
            float _x = ((float) x - 0.5f) * 0.5f;
            float _y = ((float) y - 0.5f) * 0.5f;
            LPyr[Level].Buffer[pos] -= LPyr[Level + 1].GetBiLineValue(_x, _y);
            float v = LPyr[Level].Buffer[pos];
            if (LMax < v)
                LMax = v;
            if (LMin > v)
                LMin = v;
        }
    }
}
