/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
//
// File fmathimage.cpp
// is a part of the Pluk Mathematic Library
// intended to use with Pluk Interpreter.
// Copyright (C) 2016 Yannis L. Kalaidzidis.
//
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"  
#include "fmathimage.h"
#include "LaplasePyramide.h"
#include "MultiThread.h"
extern MultiThreadS *pMultiThreads;
#define memset0_32(a, b) memset(a, 0, b)
//======================================================
//
//======================================================
FMathImage::FMathImage(DINT aX, DINT aY)
{
	X = aX;
	Y = aY;
	Len = X * Y;
	Buffer = (float *)HMALLOCZ(Len * sizeof(float));
	if (Buffer)
		Valid = TRUE;
	else
	{
		X = 0;
		Y = 0;
		Valid =  FALSE;
	}
}
//======================================================
//
//======================================================
FMathImage::FMathImage(short *buff, DINT aX, DINT aY)
{
	X = aX;
	Y = aY;
	Len = X * Y;
	Buffer = (float *)HMALLOC(Len * sizeof(float));
	if (Buffer)
	{
		Valid = TRUE;
		for (DINT i = 0; i < Len; ++i)
			Buffer[i] = (float)buff[i];
	}
	else
	{
		X = 0;
		Y = 0;
		Valid =  FALSE;
	}
}
void FMathImage::SetSize_0(int aX, int aY)
	{

		float *aBuffer = (float *)(Buffer ? HREALLOC(Buffer, aX * aY * sizeof(float)) : HMALLOCZ(aX * aY * sizeof(float)));
		if (aBuffer)
		{
			Valid = TRUE;
			Buffer = aBuffer;
			X = aX;
			Y = aY;
			Len = X * Y;
		}

	}

//======================================================
//
//======================================================
FMathImage::FMathImage(INT32 *buff, DINT aX, DINT aY)
{
	X = aX;
	Y = aY;
	Len = X * Y;
	Buffer = (float *)HMALLOC(Len * sizeof(float));
	if (Buffer)
	{
		Valid = TRUE;
		for (DINT i = 0; i < Len; ++i)
			Buffer[i] = (float)buff[i];
	}
	else
	{
		X = 0;
		Y = 0;
		Valid =  FALSE;
	}
}
//======================================================
//
//======================================================
FMathImage::FMathImage(float *buff, DINT aX, DINT aY)
{
	X = aX;
	Y = aY;
	Len = X * Y;
	Buffer = (float *)HMALLOC(Len * sizeof(float));
	if (Buffer)
	{
		Valid = TRUE;
		memcpy(Buffer, buff, sizeof(float) * Len);
	}
	else
	{
		X = 0;
		Y = 0;
		Valid =  FALSE;
	}
}
//======================================================
//
//======================================================
FMathImage::FMathImage(CFourierImage &src)
{
	X = src.Width();
	Y = src.Height();
	Len = X * Y;
	Buffer = (float *)HMALLOC(Len * sizeof(float));
	if (Buffer)
	{
		Valid = TRUE;
		memcpy(Buffer, src.ReImage, sizeof(float) * Len);
	}
	else
	{
		X = 0;
		Y = 0;
		Valid =  FALSE;
	}
}
//======================================================
//
//======================================================
FMathImage::FMathImage(CMathImage &Img)
{
	X = Img.Width();
	Y = Img.Height();
	Len = X * Y;
	Buffer = (float *)HMALLOC((Len + 1) * sizeof(float));
	if (Buffer)
	{
		Valid = TRUE;
		for (DINT i = 0; i < Len; ++i)
			Buffer[i] = (float)Img.Data()[i];
	}
	else
	{
		X = 0;
		Y = 0;
		Valid =  FALSE;
	}
}
//======================================================
//
//======================================================
FMathImage::FMathImage(CMathImage &Img, int squeeze)
{
	DINT X1 = Img.Width();
	DINT Y1 = Img.Height();
	X = X1 / squeeze;
	Y = Y1 / squeeze;
	Len = X * Y;
	float scale = 1.0f / (squeeze * squeeze);
	Buffer = (float *)HMALLOCZ(Len * sizeof(float));
	if (Buffer)
	{
		Valid =  TRUE;
		for (DINT y = 0; y < Y; ++y)
		{
			DINT _y = squeeze * y;
			DINT l = y * X;
			DINT _l = _y * X1;
			for (DINT x = 0; x < X; ++x)
			{
				DINT pos = l + x;
				DINT _x = squeeze * x;
				for (DINT j = 0; j < squeeze; ++j)
				{
					DINT _pos =  _l + X1 * j + _x;
					for (DINT i = 0; i < squeeze; ++i)
					{
						Buffer[pos] += (float)Img.Data()[_pos + i]; 
					}
				}
				Buffer[pos] *= scale;
			}
		}
	}
	else
	{
		X = Y = Len = 0;
		Valid =  FALSE;
	}
}
struct SqueezeParam {
	FMathImage *Self;
	float *buffer;
	int X1;
	int start;
	int finish;
	int squeeze;
	float scale;

};
UINT AFX_CDECL Squeeze_build(void *Param)
{
	SqueezeParam *d = (SqueezeParam *)Param;
	d->Self->_Squeeze(d->buffer, d->X1, d->start, d->finish, d->squeeze, d->scale);
	return 0;
}

//======================================================
//
//======================================================
FMathImage::FMathImage(FMathImage &Img, int squeeze)
{
	DINT X1 = Img.Width();
	DINT Y1 = Img.Height();
	X = X1 / squeeze;
	Y = Y1 / squeeze;
	Len = X * Y;
	float scale = 1.0f / (squeeze * squeeze);
	Buffer = (float *)HMALLOCZ(Len * sizeof(float));
	if (Buffer)
	{
		Valid =  TRUE;
		float scale = 1.0f / (squeeze * squeeze);
		int N = pMultiThreads->Num();
		if (N > 1)
		{
			int n = Y/N;
			SqueezeParam *params = (SqueezeParam *)_alloca(sizeof(SqueezeParam) * (N + 1));
			params[0].Self = this;
			params[0].buffer = Img.Buffer;
			params[0].X1 = X1;
			params[0].scale = scale;
			params[0].squeeze = squeeze;
			params[0].start = 0;
			params[0].finish = N == 1 ? Y : n;
			pMultiThreads->Run(0, (AFX_THREADPROC)Squeeze_build, params);
			for (int k = 1; k < N; ++k)
			{
				memcpy(params + k, params + k - 1,sizeof(SqueezeParam));
				params[k].start = params[k - 1].finish;
				params[0].finish =  k == N - 1 ? Y : (k + 1) * n;

				pMultiThreads->Run(k, (AFX_THREADPROC)Squeeze_build, params + k);
			}
			pMultiThreads->Wait();
		}
		else
		{
			_Squeeze(Img.Buffer, X1, 0, Y, squeeze, scale);
		}
/*
		for (int y = 0; y < Y; ++y)
		{
			int _y = squeeze * y;
			int l = y * X;
			int _l = _y * X1;
			for (int x = 0; x < X; ++x)
			{
				int _x = squeeze * x;
				int n = x + l;
				int k = _x + _l;
				for (int j = 0; j < squeeze; ++j)
				{
					int _k = k + X1 * j;
					for (int i = 0; i < squeeze; ++i)
					{
						Buffer[n] += Img.Buffer[i + _k];
					}
				}
				Buffer[n] *= scale;
			}
		}
*/
	}
	else
	{
		X = Y = Len = 0;
		Valid =  FALSE;
	}
}
void FMathImage::_Squeeze(float *buffer, int X1, int start, int end, int squeeze, float scale)
{
	for (int y = start; y < end; ++y)
	{
		int _y = squeeze * y;
		int l = y * X;
		int _l = _y * X1;
		for (int x = 0; x < X; ++x)
		{
			int _x = squeeze * x;
			int n = x + l;
			int k = _x + _l;
			float res = 0; //Buffer[n];
			for (int j = 0; j < squeeze; ++j)
			{
				int _k = k + X1 * j;
				for (int i = 0; i < squeeze; ++i)
				{
					res += buffer[i + _k];
				}
			}
			Buffer[n] = res * scale;
		}
	}
}
//======================================================
//
//======================================================
FMathImage::FMathImage(FMathImage &src)
{
	X = src.Width();
	Y = src.Height();
	Len = X * Y;
	Buffer = (float *)HMALLOC((Len + 1) * sizeof(float));
	if (Buffer)
	{
		Valid = TRUE;
		memcpy(Buffer, src.Buffer, sizeof(float) * Len);
	}
	else
	{
		X = 0;
		Y = 0;
		Valid =  FALSE;
	}
}
//======================================================
//
//======================================================
float FMathImage::FindMaximum(DINT &x, DINT &y)
{
	x = 0; 
	y = 0; 
	float Max = -1.0e30f;
	for (DINT _y = 0; _y < Y; ++_y)
	{
		DINT line = _y * X;
		for (DINT _x = 0; _x < X; ++_x)
		{
			if (Buffer[_x + line] > Max)
			{
				Max = Buffer[_x + line];
				x = _x;
				y = _y;
			}
		}
	}
	return Max;
}
//======================================================
//
//======================================================
float FMathImage::FindMinimum(DINT &x, DINT &y)
{
	x = 0; 
	y = 0; 
	float Min = 1.0e30f;
	for (DINT _y = 0; _y < Y; ++_y)
	{
		DINT line = _y * X;
		for (DINT _x = 0; _x < X; ++_x)
		{
			if (Buffer[_x + line] < Min)
			{
				Min = Buffer[_x + line];
				x = _x;
				y = _y;
			}
		}
	}
	return Min;
}
//======================================================
//
//======================================================
FMathImage &FMathImage::operator = (FMathImage &op)
{
	X = op.Width();
	Y = op.Height();
	if (Len != op.Len)
	{
		Len = X * Y;
		if (Buffer)
		{
			float *t = (float *)HREALLOC(Buffer, Len * sizeof(float));
			if (t)
				Buffer = t;
			else
				Valid = FALSE;
		}
		else
			Buffer = (float *)HMALLOCZ(Len * sizeof(float));
	}
	if (Buffer)
	{
		Valid = TRUE;
		memcpy(Buffer, op.Buffer, sizeof(float) * Len);
	}
	else
	{
		X = 0;
		Y = 0;
		Valid =  FALSE;
	}
	return *this;
}
//======================================================
//
//======================================================
FMathImage &FMathImage::operator *=(const FMathImage &op)
{
	if (Len == op.Len)
		for (DINT i = 0; i < Len; ++i)
			Buffer[i] *= op.Buffer[i];
	return *this;
}
//======================================================
//
//======================================================
FMathImage &FMathImage::operator /=(const FMathImage &op)
{
	if (Len == op.Len)
		for (DINT i = 0; i < Len; ++i)
			if (op.Buffer[i] != 0.0f)
				Buffer[i] /= op.Buffer[i];
	return *this;
}
//======================================================
//
//======================================================
FMathImage &FMathImage::operator +=(const FMathImage &op)
{
	if (Len == op.Len)
		for (DINT i = 0; i < Len; ++i)
			Buffer[i] += op.Buffer[i];
	return *this;
}
//======================================================
//
//======================================================
FMathImage &FMathImage::operator -=(const FMathImage &op)
{
	if (Len == op.Len)
		for (DINT i = 0; i < Len; ++i)
			Buffer[i] -= op.Buffer[i];
	return *this;
}
//======================================================
//
//======================================================
FMathImage &FMathImage::operator *=(float c)
{
	for (DINT i = 0; i < Len; ++i)
		Buffer[i] *= c;
	return *this;
}
//======================================================
//
//======================================================
FMathImage &FMathImage::operator /=(float c)
{
	if (c != 0.0f)
	{
		for (DINT i = 0; i < Len; ++i)
			Buffer[i] /= c;
	}
	return *this;
}
//======================================================
//
//======================================================
FMathImage &FMathImage::operator +=(float c)
{
	for (DINT i = 0; i < Len; ++i)
		Buffer[i] += c;
	return *this;
}
//======================================================
//
//======================================================
FMathImage &FMathImage::operator -=(float c)
{
	for (DINT i = 0; i < Len; ++i)
		Buffer[i] -= c;
	return *this;
}
//======================================================
//
//======================================================
float FMathImage::GetMeanValue()
{
	float f = 0.0f;
	for (DINT i = 0; i < Len; ++i)
		f += Buffer[i];
	return f / Len;
}
//======================================================
//
//======================================================
float FMathImage::Integral(DINT off_x, DINT off_y, DINT width, DINT height)
{
	float f = 0.0f;
	DINT count = 0;
	for (DINT y = max((DINT)0, off_y), lim_y = min((DINT)Y, off_y + height); y < lim_y; ++y)
	{
		DINT line = y * X;
		for (DINT x = max((DINT)0, off_x), lim_x = min((DINT)X, off_x + width); x < lim_x; ++x)
		{
			f += Buffer[line + x];
			++count;
		}
	}
	if (count > 0)
		f /= (float)count;
	return f;
}
//======================================================
//
//======================================================
FMathImage *FMathImage::GetMeanSD(float &Mean, float &MeanSD, DINT w )
{
	FMathImage *SD = new FMathImage(X, Y);
	Mean = 0.0f;
	MeanSD = 0.0f;
	for (DINT y = 0, line = 0; y < Y; ++y, line += X)
	{
		for (DINT x = 0; x < X; ++x)
		{
			int count = 0;
			float s2 = 0.0f;
			float s = 0.0f;
			for (DINT k = -w, L_pos = line - w * X; k <= w;  ++k, L_pos += X)
			{	
				DINT L_y = y + k;
				if (L_y >= 0 && L_y < Y)
				{
					for (DINT m = -w; m <= w; ++m)
					{
						DINT L_x = x + m;
						if (L_x >= 0 && L_x < X)
						{
							float v = Buffer[L_pos + L_x];
							s2 += v * v;
							s += v;
							++count;
						}
					}
				}
			}
			s /= count;
			s2 /= count;
			float disp = max(s2 - s * s, 0.0f);
			SD->Buffer[line + x] = sqrt(disp);
			Mean += Buffer[line + x];
			MeanSD += SD->Buffer[line + x];
		}
	}
	Mean /= Len;
	MeanSD /= Len;
	return SD;
}
//======================================================
//
//======================================================
void FMathImage::Squeeze4()
{
	DINT X1 = X / 2;
	DINT Y1 = Y / 2;
	DINT _Len = X1 * Y1;
	float *B = (float *)HMALLOCZ(_Len * sizeof(float));
	if (B)
	{
		for (DINT _y = 0; _y < Y1; ++_y)
		{
			DINT y = 2 * _y;
			DINT l = y * X;
			DINT _l = _y * X1;
			for (DINT _x = 0; _x < X1; ++_x)
			{
				DINT x = _x * 2;
				int count = 0;
				for (DINT j = 0; j < 2; ++j)
				{
					DINT k;
					if (y + j >= Y)
						k = X * (Y - 1);
					else
						k = l + X * j;
					for (int i = 0; i < 2; ++i)
					{
						if (x + i >= X)
							B[_x + _l] += Buffer[X - 1 + k]; 
						else
							B[_x + _l] += Buffer[x + i + k]; 
					}
				}
				B[_x + _l] *= 0.25f;
			}
		}
		X = X1;
		Y = Y1;
		Len = _Len;
		HFREE(Buffer);
		Buffer = B;
	}
}
//======================================================
//
//======================================================
void FMathImage::Stretch4()
{
	DINT X1 = X * 2;
	DINT Y1 = Y * 2;
	DINT _Len = X1 * Y1;
	float *B = (float *)HMALLOCZ(_Len * sizeof(float));
	for (DINT y = 0; y < Y; ++y)
	{
		DINT l = y * X;
		DINT _l = y * 2 * X1;
		for (DINT x = 0; x < X; ++x)
		{
			DINT _x = x * 2;
			for (DINT j = 0; j < 2; ++j)
			{
				if (y + j < Y)
				{
					DINT k = _l + X1 * j;
					for (DINT i = 0; i < 2; ++i)
						B[_x + i + k] = Buffer[x + l]; 
				}
			}
		}
	}
	X = X1;
	Y = Y1;
	Len = _Len;
	HFREE(Buffer);
	Buffer = B;
}
//======================================================
//
//======================================================
void FMathImage::GetData(CMathImage &dst)
{
	dst.SetSize_0(X, Y);
	INT32 *Data = dst.Data();
	if (Data)
	{
		for (DINT i = 0; i < Len; ++i)
			Data[i] = (INT32)Buffer[i];
	}
}
//======================================================
//
//======================================================
void FMathImage::SetImage(FMathImage &img, DINT off_x, DINT off_y)
{
	for (DINT y = off_y, sy = 0; y < Y && sy < img.Y; ++y, ++sy)
	{
		if (y >= 0)
		{
			DINT line = y * X;
			DINT sline = sy * img.X;
			for (DINT x = off_x, sx = 0; x < X && sx < img.X; ++x, ++sx)
			{
				if (x >= 0)
					Buffer[line + x] = img.Buffer[sline + sx];
			}
		}
	}
}
//====================================================================================
//
//====================================================================================
void FMathImage::Power(float op)
{
	for (DINT i = 0; i < Len; ++i)
		Buffer[i] = pow(Buffer[i], op);
}
//====================================================================================
//
//====================================================================================
void FMathImage::Ln()
{
	for (DINT i = 0; i < Len; ++i)
		Buffer[i] = log(Buffer[i]);
}
//====================================================================================
//
//====================================================================================
void FMathImage::Exp()
{
	for (DINT i = 0; i < Len; ++i)
		Buffer[i] = exp(Buffer[i]);
}
//====================================================================================
//
//====================================================================================
void FMathImage::fAbs()
{
	for (DINT i = 0; i < Len; ++i)
		Buffer[i] = fabs(Buffer[i]);
}
//======================================================
//
//======================================================
void FMathImage::LaplacePyramidFilter(LPFilterParam *Params, int nParams, BOOL LogScale)
{
	int MaxLevel = 1;
		for (int i = 0; i < nParams; ++i)
		{
			if (Params[i].Level >= MaxLevel)
				MaxLevel = Params[i].Level + 1;
		}
		LaplacePyramide LP(*this, MaxLevel + 1, LogScale);
		LP.PHKFilter(Params, nParams);
		FMathImage *img = LP.ReconstructPyr(0);
		if (img && img->Buffer)
			memcpy(Buffer, img->Buffer, sizeof(float) * Len);
}
//====================================================================================
//
//====================================================================================
int __cdecl compare_FLOAT(const void *elem1, const void *elem2)
{
	float *a = (float *)elem1;
	float *b = (float *)elem2;
	if (fabs(*a - *b) > 0.001)
	{
		if (*a > *b)
			return 1;
		else
		if (*a < *b)
			return -1;
	}
	return 0;
}

//====================================================================================
//
//====================================================================================
void FMathImage::MedianSmooth(DINT w)
{
	float *data = (float *)HMALLOCZ(X * Y * sizeof(float));
	DINT L = 2 * w + 1;
	float *buff = (float *)_alloca(L * L * sizeof(float));
    if (data && buff)
    {
        for (DINT j = 0; j < Y; ++j)
        {
            DINT line = j * X;
            for (DINT i = 0; i < X; ++i)
            {
                DINT n = 0;
                for (DINT m = max(j - w, (DINT)0) * X, lim_m = min((DINT)Y - 1, j + w) * X; m <= lim_m; m += X)
                {
                    for (DINT k = max(i - w, (DINT)0),lim_k = min((DINT)X - 1, i + w); k <= lim_k; ++k)
                    {
                        buff[n] = Buffer[k + m];
                        ++n;
                    }
                }
                if (n > 0)
                {
                    qsort(buff, n, sizeof(float), compare_FLOAT);
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
void FMathImage::MedianSmoothNonZero(DINT w)
{
	float *data = (float *)HMALLOCZ(X * Y * sizeof(float));
	DINT L = 2 * w + 1;
	float *buff = (float *)_alloca(L * L * sizeof(float));
    if (data && buff)
    {
        for (DINT i = 0; i < X; ++i)
        {
            for (DINT j = 0, _j = 0; j < Y; ++j, _j += X)
            {
                DINT n = 0;
                if (Buffer[i + _j])
                {
                    for (DINT m = max(j - w, (DINT)0) * X, lim_m = min((DINT)Y - 1, j + w) * X; m <= lim_m; m += X)
                    {
                        for (DINT k = max(i - w, (DINT)0),lim_k = min((DINT)X - 1, i + w); k <= lim_k; ++k)
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
                        qsort(buff, n, sizeof(float), compare_FLOAT);
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
void FMathImage::MedianSmooth(DINT w, DINT left, DINT top, DINT width, DINT height)
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
    MArray<float> _data(X * Y);
    float *data = _data.GetPtr();
    DINT L = 2 * w + 1;
    DINT n = L * L;
    float *buff = (float *)_alloca(n * sizeof(float));
    if (data && buff)
    {
        memcpy(data, Buffer, sizeof(float) * X * Y);
        for (DINT i = left, lim_X = left + width; i < lim_X; ++i)
        {
            for (DINT j = top, lim_Y = top + height; j < lim_Y; ++j)
            {
                DINT n = 0;
                for (DINT m = max(j - w, (DINT)0) * X, lim_m = min((DINT)Y - 1, j + w) * X; m <= lim_m; m += X)
                {
                    for (DINT k = max(i - w, (DINT)0),lim_k = min((DINT)X - 1, i + w); k <= lim_k; ++k)
                    {
                        buff[n] = Buffer[k + m];
                        ++n;
                    }
                }
                if (n > 0)
                {
                    qsort(buff, n, sizeof(float), compare_FLOAT);
                    data[i + j * X] = buff[(n - 1) / 2];
                }
            }
        }
        memcpy(Buffer, data, sizeof(float) * X * Y);
    }

}
//======================================================
//
//======================================================
void FMathImage::ConvolveWithGauss(double width)
{
	_ConvolveWith(0, width);
}
//======================================================
//
//======================================================
void FMathImage::ConvolveWithLaplacianOfGauss(double width)
{
	_ConvolveWith(1, width);
}
//======================================================
//
//======================================================
void FMathImage::ConvolveWithGradXOfGauss(double width)
{
	_ConvolveWith(2, width);
}
//======================================================
//
//======================================================
void FMathImage::ConvolveWithGradYOfGauss(double width)
{
	_ConvolveWith(3, width);
}
//======================================================
//
//======================================================
void FMathImage::ConvolveWithMask(CFourierImage &mask)
{
	CFourierImage image(*this);
	image.Transform(TRUE);
	image *= mask;
	image.Transform(FALSE);
	image.GetRe(*this, 0, 0, X, Y, 0, 0);
}
//======================================================
//
//======================================================
void FMathImage::_ConvolveWith(int MaskType, double width)
{
	if (width < 0.75)
		width = 0.75;
	DINT wnd = max((DINT)(width + 0.5), (DINT)1);
	//DINT wnd = max((DINT)((width - (int)width) > 0.1 ? (int)width + 1 : (int)width), 1);
	DINT X_1 = (DINT)(16 * wnd);
	if (X_1 < 64)
		X_1 = 64;
	if (X_1 > min(X, Y))
		X_1 = min(X, Y);
	CFourierImage mask(X_1, X_1);
	CFourierImage image(X_1, X_1);
	float *tBuffer = (float *)HMALLOCZ(sizeof(float) * X * Y); 
	if (tBuffer && mask.IsValid() && image.IsValid())
	{
		X_1 = image.Width();
		switch (MaskType)
		{
		case 0 :
			mask.MakeGaussMask((float)width);
			break;
		case 1 :
			mask.MakeLoGaussMask2D((float)width);
			break;
		case 2 :
			mask.MakeGradXGaussMask2D((float)width);
			break;
		case 3 :
			mask.MakeGradYGaussMask2D((float)width);
			break;
		default :
			mask.MakeGaussMask((float)width);
			break;
		}
		mask.Transform(TRUE);
		for (DINT x = 0; x < X; x += X_1 - 6 * wnd)
		{
			for (DINT y = 0; y < Y; y += X_1 - 6 * wnd)
			{
				DINT off_x, off_y, w, h;
				if (x == 0)
				{
					off_x = 0;
					w = X_1 - 3 * wnd;
				}
				else
				{
					off_x = 3 * wnd;
					w = X_1 - 6 * wnd;
					if (x + X_1 - 6 * wnd > X)
						w = X - x;
				}
				
				if (y == 0)
				{
					off_y = 0;
					h = X_1 - 3 * wnd;
				}
				else
				{
					off_y = 3 * wnd;
					h = X_1 - 6 * wnd;
					if (y + X_1 - 6 * wnd > Y)
						h = Y - y;
				}
				image.ClearIm();
				image.ClearRe();
				image.SetReImageEx(*this, x, y, X_1, X_1, 0, 0);
				image.Transform(TRUE);
				image *= mask;
				image.Transform(FALSE);
				float *temp = Buffer;
				Buffer = tBuffer;
				image.GetRe(*this, off_x, off_y, w, h, x + off_x, y + off_y);
				Buffer = temp;
			}
		}
		HFREE(Buffer);
		Buffer = tBuffer;
		tBuffer = NULL;
	}
	if (tBuffer)
		HFREE(tBuffer);
}