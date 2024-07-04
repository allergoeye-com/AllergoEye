///////////////////////////////////////////////////
// mbool.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////
#ifndef __MBOOL__
#define __MBOOL__
#include "buffer.h"

//****************************************************
//
//****************************************************
class MBoolArray : protected MArray<USHORT> {
public:
	void Attach(USHORT *_ptr, int len, int _iMin, int _iMax) { 
		iMin = min(_iMax, _iMin);
		iMax = max(_iMax, _iMin);
		MArray<USHORT>::Attach(_ptr, len);
	}
	void Detach() { MArray<USHORT>::Detach(); iMin = iMax = 0; }
	MBoolArray(int iMin = 0, int iMax = 1024);
	MBoolArray(MBoolArray &s) { *this = s; }
	~MBoolArray() {}
	MBoolArray & operator = (MBoolArray &dst);
	bool operator [] (int n);
	MBoolArray &operator |= (int n);
	MBoolArray &operator |= (MBoolArray &dst);
	MBoolArray &operator &= (MBoolArray &dst);
	MBoolArray &operator -= (int n);
	MBoolArray &operator -= (MBoolArray &dst);
	MBoolArray &Inverse();
	MBoolArray operator | (MBoolArray &s2) { return MBoolArray(*this) |= s2; }
	MBoolArray operator & (MBoolArray &s2) { return MBoolArray(*this) &= s2; }
	MBoolArray operator - (MBoolArray &s2) { return MBoolArray(*this) -= s2; }
	int MaxRange() { return iMax; }
	int MinRange() { return iMin; }
protected:
	int iMin;
	int iMax;
};
//****************************************************
//
//****************************************************
class MBoolDim : protected MArray<USHORT> {
public:
	MBoolDim(int SizeXY);
	MBoolDim(MBoolDim &s) { *this = s; }
	~MBoolDim() {}
	MBoolDim &operator = (MBoolDim &dst);
	bool operator () (int y, int x);
	MBoolDim &Plus (int y, int x);
	MBoolDim &operator |= (MBoolDim &dst);
	MBoolDim &operator &= (MBoolDim &dst);
	MBoolDim &Minus (int y, int x);
	MBoolDim &operator -= (MBoolDim &dst);
	MBoolDim &Inverse();
	MBoolDim operator | (MBoolDim &s2) { return MBoolDim(*this) |= s2; }
	MBoolDim operator & (MBoolDim &s2) { return MBoolDim(*this) &= s2; }
	MBoolDim operator - (MBoolDim &s2) { return MBoolDim(*this) -= s2; }
	int Range() { return iMax; }
protected:
	int iMax;
};

//-----------------------------------------------------
//
//-----------------------------------------------------
inline MBoolArray::MBoolArray(int _iMin, int _iMax) 
{
	iMin = min(_iMax, _iMin);
	iMax = max(_iMax, _iMin);
	SetLen((iMax - iMin + 16) >> 4);
	Set(0);
}
//-----------------------------------------------------
//
//-----------------------------------------------------
inline MBoolArray &MBoolArray::operator = (MBoolArray &s) 
{
	iMin = s.iMin;
	iMax = s.iMax;
	Copy(s.GetPtr(), s.GetLen());
	return *this;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
inline bool MBoolArray::operator [] (int n)
{
	BoolWarning(!(n > iMax || n < iMin));
	return n <= iMax && n >= iMin ? (*(ptr + ((n - iMin) >> 4)) & (1 << ((n - iMin) & 0x0f))) != 0 : false;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
inline MBoolArray &MBoolArray::operator |= (int n) 
{
	if (n > iMax || n < iMin) 
		BoolWarning(0);
	else
		*(ptr + ((n - iMin) >> 4)) |= (1 << ((n - iMin) & 0x0f));
	return *this;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
inline MBoolArray &MBoolArray::operator |= (MBoolArray &dst) 
{
		
	if (dst.iMin == iMin && dst.iMax == iMax) 
		for (int i = 0;  i < Len;  ++i) 
			*(ptr + i) |= *(dst.ptr + i);
	else
		BoolWarning(0);
	return *this;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
inline MBoolArray &MBoolArray::operator &= (MBoolArray &dst) 
{
	if (dst.iMin == iMin && dst.iMax == iMax) 
		for (int i = 0;  i < Len;  ++i) 
			*(ptr + i) &= *(dst.ptr + i);
	else
		BoolWarning(0);
	return *this;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
inline MBoolArray &MBoolArray::operator -= (int n) 
{
	if (n <= iMax && n >= iMin) 
		*(ptr + ((n - iMin) >> 4)) &= ~(1 << ((n - iMin) & 0x0f));
  return *this;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
inline MBoolArray &MBoolArray::operator -= (MBoolArray &dst) 
{
	if (dst.iMin == iMin && dst.iMax == iMax) 
	{
		for (int i = 0;  i < Len;  ++i) 
			*(ptr + i) &= ~*(dst.ptr + i);
	}
	return *this;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
inline MBoolArray &MBoolArray::Inverse() 
{
	for (int i = 0;  i < Len;  ++i) 
		*(ptr + i) = ~*(ptr + i);
  return *this;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
inline MBoolDim::MBoolDim(int Size) 
{
	iMax = Size;
	SetLen((Size * Size + 16) >> 4);
	Set(0);
}
//-----------------------------------------------------
//
//-----------------------------------------------------
inline MBoolDim &MBoolDim::operator = (MBoolDim &s) 
{
	iMax = s.iMax;
	Copy(s.GetPtr(), s.GetLen());
	return *this;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
inline bool MBoolDim::operator () (int y, int x)
{
	if (x >= 0 && y >= 0 && x < iMax && y < iMax)
	{
		int n = y * iMax + x; 
		return (*(ptr + (n >> 4)) & (1 << (n & 0x0f))) != 0;
	}
	return false;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
inline MBoolDim &MBoolDim::Plus (int y, int x)
{
	if (x >= 0 && y >= 0 && x < iMax && y < iMax)
	{
		int n = y * iMax + x; 
		*(ptr + (n >> 4)) |= (1 << (n & 0x0f));
	}
	return *this;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
inline MBoolDim &MBoolDim::operator |= (MBoolDim &dst) 
{
		
	if (dst.iMax == iMax) 
		for (int i = 0;  i < Len;  ++i) 
			*(ptr + i) |= *(dst.ptr + i);
	else
		BoolWarning(0);
	return *this;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
inline MBoolDim &MBoolDim::operator &= (MBoolDim &dst) 
{
	if (dst.iMax == iMax) 
		for (int i = 0;  i < Len;  ++i) 
			*(ptr + i) &= *(dst.ptr + i);
	else
		BoolWarning(0);
	return *this;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
inline MBoolDim &MBoolDim::Minus (int y, int x) 
{
	if (x >= 0 && y >= 0 && x < iMax && y < iMax)
	{
		int n = y * iMax + x; 
		*(ptr + (n >> 4)) &= ~(1 << (n & 0x0f));
	}
  return *this;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
inline MBoolDim &MBoolDim::operator -= (MBoolDim &dst) 
{
	if (dst.iMax == iMax) 
	{
		for (int i = 0;  i < Len;  ++i) 
			*(ptr + i) &= ~*(dst.ptr + i);
	}
	return *this;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
inline MBoolDim &MBoolDim::Inverse() 
{
	for (int i = 0;  i < Len;  ++i) 
		*(ptr + i) = ~*(ptr + i);
  return *this;
}
#endif
