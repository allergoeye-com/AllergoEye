#ifndef __FIMAGE__
#define  __FIMAGE__

class CFourierImage;

struct LPFilterParam;

class FMathImage;

#pragma pack (push)
#pragma pack (4)

struct SimpleLUT {
    bool Square;
    int Min; // = .Contrast;
    int Max; // = 255 * .Brigthnes;
    SimpleLUT()
    {
        Min = 0;
        Max = 255;
    }

    void Init(int _Min, int _Max)
    {
        Min = _Min;
        Max = _Max;
    }

};

EXPORTNIX class EXPMDC CMathImage {
public:
    int *Buffer;
    int X;
    int Y;
    BOOL fKill;

    CMathImage();

    CMathImage(CMathImage &im);

    CMathImage(FMathImage *im);

    CMathImage(int x, int y);

    CMathImage(int *b, int x, int y);

    ~CMathImage()
    { if (fKill && Buffer) HFREE(Buffer); }

    void SetSize_0(int aX, int aY);

    void LaplacePyramidFilter(LPFilterParam *Params, int nParams, BOOL LogScale);

    void __ConvolveWith(int MaskType, double width);

    void DeconvolveWithGauss(double width, int step);

    void ToBilevel(INT32 Threshold, BOOL Unsigned = TRUE);

    CMathImage &operator=(CMathImage &im);

    void Init(int x, int y, int *b, bool _fKill);

    void Clear()
    { memset(Buffer, 0, sizeof(int) * X * Y); }

    CMathImage *
    GetPotentialBorderMask(float width, FMathImage *&Grad2Mask, float Scale, BOOL UseLaplace);

    CMathImage *GetPotentialBorderMaskByPyrmid(FMathImage *&Grad2Mask, float KernelWidth, int MaxN,
                                               float Scale);

    CMathImage *
    FilterPotentialBorders(FMathImage &Grad2Mask, float &UpperThreshold, float &LowThreshold,
                           BOOL SumEntropy);

    static CMathImage *
    GetBorderMaskRGB(CMathImage *img_r, CMathImage *img_g, CMathImage *img_b, float width,
                     CMathImage *&Grad2Mask_Amp, CMathImage *&Grad2Mask_Sat,
                     CMathImage *&Grad2Mask_Hue, float Scale, BOOL UseLaplace, BOOL SumEntropy);

    double FindThresholdByMaxEntropy(CMathImage &Mask, BOOL SumOfEntropy, BOOL VarEnt);

    double FindThresholdByMaxEntropy(CMathImage &Mask, double UpperThreshold, BOOL SumOfEntropy,
                                     BOOL VarEnt);

//---------------------------------------------------
    void FindCentralLines(DINT R);

    BOOL _ErosionStep(BYTE *IndexBuffer, INT32 UpperBoundary, BOOL KeepBaseLine);

    void _ExpandCentalLineEnds(BYTE *IndexBuffer, DINT R);

    int _IsBaseLinePoint(BYTE *IndexBuffer, DINT x, DINT y, int index, BOOL KeepLast);
//---------------------------------------------------

    void
    LaplacePyramidFilter(float sigma, float alpha, float beta, float ScaleCenter, float ScaleAlpha,
                         BOOL LogScale);

    void
    LaplacePyramidFilter(float sigma, float alpha, float s_beta, float l_beta, float ScaleCenter,
                         float ScaleAlpha, float GaussKernel, BOOL LogScale);

    static void _GetPotentialBorderMask(CMathImage *Mask, FMathImage &Grad2Mask, FMathImage &GradX,
                                        FMathImage &GradY, FMathImage &Laplace, float Scale,
                                        BOOL UseLaplace);

    static void
    _GetPotentialBorderMaskByPyramid(CMathImage *Mask, FMathImage &Grad2Mask, FMathImage **GradX,
                                     FMathImage **GradY, int N, float Scale);

//	static CMathImage *GetBorderMaskRGB(CMathImage *img_r, CMathImage *img_g, CMathImage *img_b, float width, CMathImage *&Grad2Mask_Amp, CMathImage *&Grad2Mask_Sat, CMathImage *&Grad2Mask_Hue, float Scale, BOOL UseLaplase, BOOL SumEntropy);
//	static void ReduceNumberColors(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b, CMathImage &Image_r, CMathImage &Image_g, CMathImage &Image_b, int nColors, BOOL Split2, INT32 (*ColorArray)[3], BYTE *_IndexMap);
    static void ConvertCIELab_to_RGB(FMathImage &image_L, FMathImage &image_a, FMathImage &image_b,
                                     CMathImage &img_r, CMathImage &img_g, CMathImage &img_b);

    static void ConvertRGB_to_CIELab(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b,
                                     FMathImage &image_L, FMathImage &image_a, FMathImage &image_b);

    static void ConvertCIELab_to_XYZ(FMathImage &image_L, FMathImage &image_a, FMathImage &image_b,
                                     FMathImage &image_X, FMathImage &image_Y, FMathImage &image_Z);

    static void ConvertXYZ_to_CIELab(FMathImage &image_X, FMathImage &image_Y, FMathImage &image_Z,
                                     FMathImage &image_L, FMathImage &image_a, FMathImage &image_b);

    static void ConvertXYZ_to_RGB(FMathImage &image_X, FMathImage &image_Y, FMathImage &image_Z,
                                  CMathImage &img_r, CMathImage &img_g, CMathImage &img_b);

    static void
    ConvertRGB_to_XYZ(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b, FMathImage &image_X,
                      FMathImage &image_Y, FMathImage &image_Z);

    static void
    ConvertHSI_to_RGB(FMathImage &image_Hue, FMathImage &image_Sat, FMathImage &image_Amp,
                      CMathImage &img_r, CMathImage &img_g, CMathImage &img_b);

    static void ConvertRGB_to_HSI(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b,
                                  FMathImage &image_Hue, FMathImage &image_Sat,
                                  FMathImage &image_Amp);

    static void
    ConvertNRGB_to_RGB(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b, CMathImage &img_amp,
                       CMathImage &image_r, CMathImage &image_g, CMathImage &image_b);

    static void
    ConvertRGB_to_NRGB(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b, CMathImage &image_r,
                       CMathImage &image_g, CMathImage &image_b, CMathImage &image_amp);

    static void _ConvertBCY_to_RGB(FMathImage &image_Amp, FMathImage &image_C, FMathImage &image_Y,
                                   CMathImage &img_r, CMathImage &img_g, CMathImage &img_b,
                                   DINT start, DINT finish);

    static void ConvertBCY_to_RGB(FMathImage &image_Amp, FMathImage &image_C, FMathImage &image_Y,
                                  CMathImage &img_r, CMathImage &img_g, CMathImage &img_b);

    static void _ConvertRGB_to_BCY(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b,
                                   FMathImage &image_Amp, FMathImage &image_C, FMathImage &image_Y,
                                   DINT start, DINT finish);

    static void ConvertRGB_to_BCY(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b,
                                  FMathImage &image_Amp, FMathImage &image_C, FMathImage &image_Y);

    static void ConvertRGB_to_BSH(CMathImage &img_r, CMathImage &img_g, CMathImage &img_b,
                                  FMathImage &image_Amp, FMathImage &image_Sat,
                                  FMathImage &image_Hue);

    static void Get3BytesImage(CMathImage img[3], SimpleLUT lut[3], MemImage &bmp, int X, int Y);

    static void GetGrayScaleImage(CMathImage &img, SimpleLUT lut, MemImage &bmp, int X, int Y);

    BOOL IsValid()
    { return Buffer != 0; }

    int *Data()
    { return Buffer; }

    int Width()
    { return X; }

    int Height()
    { return Y; }

    void MedianSmoothNonZero(DINT w);

    void MedianSmooth(DINT w);

    void MedianSmooth(DINT w, DINT left, DINT top, DINT width, DINT height);

    INT32 GetMaxValue()
    {
        INT32 Max = -32000;
        DINT Len = X * Y;
        for (DINT i = 0; i < Len; ++i)
        {
            INT32 v = Buffer[i];
            if (Max < v)
                Max = v;
        }
        return Max;
    }

    //==================================================
    //
    //==================================================
    INT32 GetMinValue()
    {
        INT32 Min = 32000;
        DINT Len = X * Y;
        for (DINT i = 0; i < Len; ++i)
        {
            INT32 v = Buffer[i];
            if (Min > v)
                Min = v;
        }
        return Min;
    }

    void MinMax(int &Min, int &Max)
    {
        int L = X * Y;
        Min = Max = 0;
        bool fFirst = true;
        for (int i = 0; i < L; ++i)
            if (Buffer[i])
            {
                if (fFirst)
                {
                    Min = Max = Buffer[i];
                    fFirst = false;
                } else
                {
                    Min = min(Min, Buffer[i]);
                    Max = max(Max, Buffer[i]);
                }
            }
    }

    void OR(CMathImage &op)
    {
        int L = X * Y;
        for (int j = 0; j < L; ++j)
            Buffer[j] |= op.Buffer[j];

    }

    void AND(CMathImage &op)
    {

        int L = X * Y;
        for (int j = 0; j < L; ++j)
            Buffer[j] &= op.Buffer[j];
    }

    void AND_EX(CMathImage &op)
    {
        int L = X * Y;
        for (int j = 0; j < L; ++j)
            if (!op.Buffer[j])
                Buffer[j] = 0;
    }

    CMathImage &operator*=(int i)
    {
        int L = X * Y;
        for (int j = 0; j < L; ++j)
            Buffer[j] *= i;
        return *this;
    }

    void ADD(CMathImage &op)
    {
        int L = X * Y;
        for (int j = 0; j < L; ++j)
            Buffer[j] += op.Buffer[j];
    }

    void _Minus(CMathImage &op)
    {
        for (int j = 0; j < Y; ++j)
        {
            for (int i = 0; i < X; ++i)
                Buffer[i + j * X] -= op.Buffer[i + j * X];
        }
    }

    int Inflation(int NumbItter, int NumbContact, int Value);

    int _Inflation(BYTE *IndexBuffer, int NumbItter, int NumbContact);

    void
    GetRGB(BYTE *rgb, int X, int Y, int sizeX, int MaskR = 255, int MaskG = 255, int MaskB = 255);

    void GetRGB2(BYTE *_buf, int X, int Y, int sizeX, double iMax, double proc);

    void GetRGB3(INT *_buf, int X, int Y, int sizeX, double iMax, double proc);

    void
    _SmoothTensor(INT32 *_Buffer, CFourierImage *A, CFourierImage *B, CFourierImage *C, int wSmooth,
                  DINT Top, DINT Bottom, float Sigma2, BOOL Invert);

    void SmoothTensor(int wTensor, int wSmooth, float Sigma, float tSigma, BOOL GaussKernel,
                      BOOL Invert);

    void SmoothTensorGrad(int wTensor, int wSmooth, float Sigma, float tSigma, BOOL GaussKernel,
                          BOOL Invert);

    void TensorRatio(int wTensor, int Scale, float tSigma, BOOL GaussKernel);

    void TensorGradRatio(int wTensor, int Scale, float tSigma, BOOL GaussKernel);

    friend UINT AFX_CDECL SmoothTensorFunc(void *Param);

    INT32 &operator()(DINT x, DINT y)
    { return Buffer[x + y * X]; }


};
EXPORTNIX class EXPMDC CFourierImage {
public:
    CFourierImage(CMathImage &re);

    CFourierImage(FMathImage &re);

    CFourierImage(CFourierImage &f);

    CFourierImage(int aX, int aY, int *buff = 0);

    ~CFourierImage();

    UINT *Buffer;
    int X;
    int Y;
    int Len;
    float *ReImage;
    float *ImImage;
    float *_Sin_x;
    float *_Cos_x;
    float *_Sin_y;
    float *_Cos_y;
    int *order_x;
    int *order_y;
    BOOL Valid;

    int IsValid() const
    { return Valid; }

    int Width()
    { return X; }

    int Height()
    { return Y; }

    void Squeeze4();

    BOOL InitAuxilaryData(BOOL Direction);

    void CopySrc(float *dst, int *src, int aX, int aY);

    void _Swap(float *re, float *im, int *order);

    void __Swap(float *re, float *im, int *order, int Start, int Finish);

    void Column2Row(float *dst_re, float *dst_im, float *src_re, float *src_im, int *order);

    void Row2Column(float *dst_re, float *dst_im, float *src_re, float *src_im);

    void
    _Row2Column(float *dst_re, float *dst_im, float *src_re, float *src_im, int Start, int Finish);

    void TransformLines(float *&_re, float *&_im, int L, int Start, int N, int Mask, float *Sin,
                        float *Cos);

    void
    _TransformLines(float *&_re, float *&_im, float *&_s_re, float *&_s_im, int L, int Start, int N,
                    int Mask, float *Sin, float *Cos);

    void TransformColumn(float *&_re, float *&_im);

    bool Transform(BOOL Direct);

    void Clear();

    void ClearRe();

    void ClearIm();

    void MakeGaussMask(float width, float cX, float cY, float Amp = 1.0)
    { MakeGaussMask2D(width, cX, cY, Amp); }

    void MakeGaussMask(float width, float Amp = 1.0)
    { MakeGaussMask2D(width, Amp); }

    float ImBiLine(float x, float y);

    float ReBiLine(float x, float y);

    void
    SetReImage(CMathImage &src, DINT off_src_x, DINT off_src_y, DINT width, DINT height, DINT off_x,
               DINT off_y);

    void GetRe(CMathImage &dst, DINT off_x, DINT off_y, DINT width, DINT height, DINT dst_off_x,
               DINT dst_off_y);

    void GetRe(FMathImage &dst, DINT off_x, DINT off_y, DINT width, DINT height, DINT dst_off_x,
               DINT dst_off_y);

    void SetReImageEx(FMathImage &src, DINT off_src_x, DINT off_src_y, DINT width, DINT height,
                      DINT off_x, DINT off_y);

    void Shift(float dX, float dY);

    CFourierImage &operator/=(const CFourierImage &op);

    CFourierImage &operator*=(const CFourierImage &op);

    CFourierImage &operator+=(const CFourierImage &op);

    CFourierImage &operator-=(const CFourierImage &op);

    void GetReShort(int *dst, int aX, int aY);

    void GetImShort(int *dst, int aX, int aY);

    void MakeGaussMask2D(float width, float cX, float cY, float Amp = 1.0);

    void MakeGaussMask2D(float width, float Amp = 1.0);

    void MakeGradYGaussMask2D(float width, float Amp = 1.0);

    void MakeGradXGaussMask2D(float width, float Amp = 1.0);

    void MakeLoGaussMask2D(float width, float Amp = 1.0);

    void MakeLoGaussMask2D(float width, float cX, float cY, float Amp = 1.0);

    static void
    MakeTensor2DMask(CFourierImage &image, CFourierImage &a, CFourierImage &b, CFourierImage &c,
                     int w, float Sigma, BOOL GaussKernel);


    float &_Re(int x, int y) const
    { return ReImage[x + y * X]; }

    float &_Im(int x, int y) const
    { return ImImage[x + y * X]; }

    float &Re(int x, int y)
    {
        int pos = x + y * X;
        if (pos >= 0 && pos < Len)
            return ReImage[pos];
        return ReImage[0];
    }

    //======================================================
    //
    //======================================================
    float &Im(int x, int y)
    {
        int pos = x + y * X;
        if (pos >= 0 && pos < Len)
            return ImImage[pos];
        return ImImage[0];
    }

    CMathImage *GetRe()
    {
        if (Valid)
        {
            CMathImage *img = new CMathImage(X, Y);
            if (img)
            {
                int *Data = img->Buffer;
                float *re = ReImage;
                if (Data)
                {
                    int l = X * Y;
                    while (l--)
                        *Data++ = (int) *re++;
                }
            }
            return img;
        }
        return 0L;
    }

    void GetRe(CMathImage *img)
    {
        if (Valid)
        {
            if (img)
            {
                int *Data = img->Buffer;
                float *re = ReImage;
                int W = img->X;
                int H = img->Y;
                if (Data)
                {
                    int k = 0;
                    for (int j = 0; j < H; ++j)
                    {
                        for (int i = 0; i < W; ++i, ++k)
                            Data[k] = (int) re[i];
                        re += X;
                    }
                }
            }
        }
    }

    CMathImage *GetRe(int W, int H)
    {
        if (Valid)
        {
            CMathImage *img = new CMathImage(W, H);
            if (img)
            {
                int *Data = img->Buffer;
                float *re = ReImage;
                if (Data)
                {
                    int k = 0;
                    for (int j = 0; j < H; ++j)
                    {
                        for (int i = 0; i < W; ++i, ++k)
                            Data[k] = (int) re[i];
                        re += X;
                    }
                }
            }
            return img;
        }
        return 0L;
    }

    CFourierImage &operator*=(float op)
    {
        if (Valid)
        {
            for (int i = 0; i < Len; ++i)
            {
                ReImage[i] *= op;
                ImImage[i] *= op;
            }
        }
        return *this;
    }


};
#pragma pack(pop)

float inline sign(float x)
{ return x >= 0.0f ? 1.0f : -1.0f; }

#endif
