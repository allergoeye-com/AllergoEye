#ifndef __LP_PYR_
#define __LP_PYR_

#include "fmathimage.h"

#pragma pack(push)
#pragma pack(4)

struct LPFilterParam {
    int Level;
    float alpha;
    float sigma;
    float delta;
    float beta;
    float Scale;

    LPFilterParam()
    {
        Level = 0;
        alpha = 1.0f;
        sigma = 0.5f;
        delta = 0.03f;
        beta = 1.0f;
        Scale = 1.0f;
    }
};

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
EXPORTNIX class _FEXPORT32 LaplacePyramide {
public:
    friend UINT AFX_CDECL _LaplacePyramide_filter(void *Param);

    friend UINT AFX_CDECL _LaplacePyramide_logLPyr0(void *Param);

    friend UINT AFX_CDECL _LaplacePyramide_build(void *Param);

    friend UINT AFX_CDECL _LaplacePyramide_ReconstructPyr(void *Param);

    friend UINT AFX_CDECL _LaplacePyramide_ReconstructLPyr(void *Param);

    friend UINT AFX_CDECL _LaplacePyramide_BuildGPyr(void *Param);

    friend UINT AFX_CDECL _LaplacePyramide_ReconstructGPyr(void *Param);

protected:
    BOOL LogScale;
    float LScale;
    FMathImage *LPyr;            // original Laplacian pyramid
    FMathImage (*GPyr)[2];        // original grad pyramid
    FMathImage *RPyr;            // reconstructed pyramid
    FMathImage *RLPyr;            // reconstructed Laplacian pyramid
    FMathImage (*RGPyr)[2];        // reconstructed Laplacian pyramid
    int nLevels;
    float _filer_LUT[4096];

    void Init();

    void Init_filer_LUT(float alpha);

    void __filter(LPFilterParam &Param, int Level, DINT start, DINT finish);

    BOOL logLPyr0(DINT start, DINT finish);

    void
    build(int Level, DINT start, DINT finish, float &LMin, float &LMax, float &GMin, float &GMax);

    void ReconstructPyrLevel(int Level, DINT start, DINT finish);

    void
    ReconstructLPyrLevel(int Level, DINT start, DINT finish, BOOL Product, float &Min, float &Max);

    void BuildGPyrLevel(int Level, DINT start, DINT finish, float &Min, float &Max);

    void
    ReconstructGPyrLevel(int Level, DINT start, DINT finish, BOOL Product, float &Min, float &Max);

public:
    LaplacePyramide(FMathImage &anImage, int _nLevels, BOOL LogScale = FALSE);

    LaplacePyramide(CMathImage &anImage, int _nLevels, BOOL LogScale = FALSE);

    ~LaplacePyramide();

    int GetNumbLevels()
    { return nLevels; }

    void PHKFilter(LPFilterParam *Params, int nPar);

    FMathImage *ReconstructPyr(int level);

    FMathImage *ReconstructGPyr(int Level_from, int Level_to, BOOL Product = TRUE);

    FMathImage *ReconstructLPyr(int Level_from, int Level_to, BOOL Product = TRUE);
};
#pragma pack(pop)

#endif
	
