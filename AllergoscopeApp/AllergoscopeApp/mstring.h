///////////////////////////////////////////////////
// mstring.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////
#ifndef _MSTRING__1
#define _MSTRING__1
#include "buffer.h"
#include <stdlib.h>
#include <wctype.h>
#include <wchar.h>

#ifndef __PLUK_LINUX__
#ifdef _AFXDLL
#define GetDllResourceInstance AfxGetResourceHandle
#endif 
#endif
#pragma pack (push)
#pragma pack (8)
#ifdef __PLUK_LINUX__
#define MB_PRECOMPOSED 1
#define WC_DEFAULTCHAR 1
#define WC_COMPOSITECHECK 1
#define WC_SEPCHARS 1
#define CP_UTF8 1
#define CP_UTF7 1
#define CP_THREAD_ACP 1
#ifndef ANDROID_NDK
inline DINT MultiByteToWideChar(int ignore, int ignore1, const char *src, DINT srclength, WCHAR *dst, DINT dstlength)
{
    if (srclength <= 0) srclength = strlen(src) + 1;
    DINT ln = 0, cp = min(srclength, dstlength);
    DINT l = 0;
    while (cp)
    {
        
        ln = mbstowcs (dst, src, cp);
        if (ln < 0) return 0;
        if (!dst[ln - 1] && src[ln - 1])
        	--ln;

        dst += ln;
        src += ln;
        cp -= ln;
        l += ln;
    }
    return l;
        
        
}
inline DINT WideCharToMultiByte(int ignore, int ignore1, const WCHAR *src, DINT srclength, char *dst, DINT dstlength, const char *ignore2, int *ignore3)
{
	if (srclength <= 0) srclength = (int)wcslen(src)  + 1;
    
    DINT ln = 0, cp = min(srclength, dstlength);
    DINT l = 0;
    while (cp)
    {
        
        ln = wcstombs (dst, src, cp);
        if (ln < 0) return 0;
        if (!dst[ln - 1] && src[ln - 1])
        	--ln;
        if (!ln)
            break;
        dst += ln;
        src += ln;
        cp -= ln;
        l += ln;
    }
    return l;
}
#else
DINT MultiByteToWideChar(int ignore, int ignore1, const char *src, DINT srclength, WCHAR *dst, DINT dstlength);
DINT WideCharToMultiByte(int ignore, int ignore1, const WCHAR *src, DINT srclength, char *dst, DINT dstlength, const char *ignore2, int *ignore3);
DINT WideCharToMultiByte(const WCHAR *src, MArray<char> &res);

#endif

#endif
//#define CP_LCode CP_THREAD_ACP
//#define CP_LCode CP_UTF8
//---------------------------------------------
//
//---------------------------------------------
inline const wchar_t *__T2U(int CP_LCode, const char *src, wchar_t *dst, int n)
{
	if (dst != 0L && src != 0L)
	{
		DINT len = (DINT)strlen(src);


#ifdef _WIN32_WCE
		len = MultiByteToWideChar(CP_ACP,0, src, len, dst, len);
#else
#ifdef WIN32
		len = MultiByteToWideChar(CP_LCode, 0, src, (int)-1, dst, (int)n);
#else
		 len = MultiByteToWideChar(CP_THREAD_ACP,0, src, len, dst, len);
#endif
#endif

		 dst[len] = 0;
		return dst;
	}
	return 0L;
}
#ifdef WIN32
//------------------------------------------------------------
inline int __T2ULen(int CP_LCode,  const char *src)
{
	int len = 0;
	if (src != 0L)
	{
		
		len = MultiByteToWideChar(CP_LCode,0, src, -1, 0, 0);
	}
	return len;
}
#endif
//---------------------------------------------
//
//---------------------------------------------
inline const char *__U2T(int CP_LCode, const wchar_t *src, char *dst, int n)
{
	if (dst != 0L && src != 0L)
	{

#ifdef _WIN32_WCE
		len = WideCharToMultiByte(CP_ACP,0, src, len, dst, len, 0, 0);
#else
#ifdef WIN32
		int len = WideCharToMultiByte(CP_LCode,0, src, (int)-1, dst, (int)n, 0, 0);
#else
		len = WideCharToMultiByte(CP_THREAD_ACP,0, src, len, dst, len, 0, 0);
#endif
#endif


		dst[len] = 0;
		return dst;
	}
	return 0L;
}
#ifdef WIN32

//---------------------------------------------
//
//---------------------------------------------
inline int __U2TLen(int CP_LCode, const wchar_t *src)
{
	int len = 0;
	if (src != 0L)
	{

		len = WideCharToMultiByte(CP_LCode,0, src, -1, 0, 0, 0, 0);
	}
	return len;
}
#endif
template <class T, int _CP>
class TmString : public T {
	
public:
	int Type() const {
		return _CP;
	}
	TmString():T()  { };
#ifdef WXDLLIMPEXP_BASE 
        TmString(const wxString &s)  { *this = (const wxChar *)s; };
        TmString &operator = (const wxString &s) { *this = (const wxChar *)s; return *this; };
#endif
#ifdef ANDROID_NDK
        TmString(const string &s) { *this = s.c_str(); };
        TmString &operator = (const string &s) { *this = s.c_str(); return *this; };
   //     operator string () const { string str;  str = T::ptr ? T::ptr : ""; return str; }
#endif
	TmString(int i):T(i >= 0 ? i : 0) { };
	TmString(const T &s):T (s) { T::SetLen(T::Len + 1); T::ptr[T::Len - 1] = 0; };
	TmString(const TmString& s) :T() { *this = s;  };
	TmString(const char *s):T () { *this = s; };
	TmString(const wchar_t *s):T () { *this = s; };
	~TmString() {};
	TmString(char ch, int nRepeat = 1) :T() { T::SetLen(nRepeat + 1); memset(T::ptr, ch,nRepeat); }
	TmString(const char *lpch, DINT nLength) :T() { T::SetLen(nLength + 1); memcpy(T::ptr, lpch, nLength); }
	int GetLength() const  { return T::Len && T::ptr ? (int)strlen(T::ptr) : 0; }
	char &operator [] (int i) const {return T::ptr[i]; }

	BOOL IsEmpty() const { return !T::ptr || !T::ptr[0]; }
	void Empty() { T::Clear(); }
	bool IsNumber () { double db; return ToDouble(&db); }
        int ReadInt(DINT offset) { return offset + sizeof(int) < T::Len ? Mem2Int((BYTE *)T::ptr + offset) : 0; }
        WORD ReadWord(DINT offset) { return offset + sizeof(WORD) < T::Len ? Mem2Short((BYTE *)T::ptr + offset) : 0; }
        BYTE ReadByte(DINT offset) { return offset + sizeof(BYTE) < T::Len ? *((BYTE *)T::ptr + offset) : 0; }
        double ReadDouble(DINT offset) 
        { 
            double res = 0.0;
            if (offset + sizeof(double) < T::Len)
                memcpy(&res, T::ptr + offset, sizeof(double));
            return res;
        }
        float ReadFloat(DINT offset) 
        { 
            float res = 0.0;
            if (offset + sizeof(float) < T::Len)
                memcpy(&res, T::ptr + offset, sizeof(float));
            return res;
        }
        void WriteInt(DINT offset, int i) 
        { 
            if (offset + sizeof(int) > T::Len) 
                T::SetLen(offset + sizeof(int));
           Int2Mem(i, (BYTE *)T::ptr + offset); 
        }
        void WriteWord(DINT offset, WORD i) { 
            if (offset + sizeof(WORD) > T::Len) 
                T::SetLen(offset + sizeof(WORD));
            Short2Mem(i, (BYTE *)T::ptr + offset); 
        }
        void WriteByte(DINT offset, BYTE b) { 
            if (offset + sizeof(BYTE) > T::Len) 
                T::SetLen(offset + sizeof(BYTE));
            *((BYTE *)T::ptr + offset) = b;
        }
         
        void WriteDouble(DINT offset, double db) 
        { 
            if (offset + sizeof(double) > T::Len) 
                T::SetLen(offset + sizeof(double));
            memcpy(T::ptr + offset, &db, sizeof(double));
        }
        void WriteFloat(DINT offset, float db) 
        { 
            if (offset + sizeof(float) > T::Len) 
                T::SetLen(offset + sizeof(float));
            memcpy(T::ptr + offset, &db, sizeof(float));
        }

	int ToInt() { 	return StrLen() ? atoi(T::ptr) : 0; }
	double ToDouble() { return StrLen() ? atof(T::ptr) : 0; }
	bool ToDouble(double *db) { int fRet = 0; if (StrLen()) fRet = sscanf(T::ptr, "%lf", db); return fRet == StrLen(); }
	bool ToInt(int *db) { int fRet = 0; if (StrLen()) fRet = sscanf(T::ptr, "%d", db); return fRet == StrLen(); }
	char GetAt(DINT nIndex) { return T::ptr[nIndex]; }
	void SetAt(DINT nIndex, char ch) { T::ptr[nIndex] = ch ; }
	const TmString &operator = (char ch) { T::SetLen(2); T::ptr[0] = ch; T::ptr[1] = 0; return *this; }
	int Compare(const char *lpsz) const { 
		if (T::Len && (!lpsz || !lpsz[0])) return 1;
		if (lpsz && lpsz[0] && !T::Len) return -1; 
		if ((!T::Len || !T::ptr[0]) && (!lpsz || !lpsz[0])) return 0;
		return strcmp(T::ptr, lpsz); 
	}
	int CompareNoCase(const char *lpsz) const { 
		if (T::Len && (!lpsz || !lpsz[0])) return 1;
		if (lpsz && lpsz[0] && !T::Len) return -1; 
		if ((!T::Len || !T::ptr[0]) && (!lpsz || !lpsz[0])) return 0;
	#ifdef __PLUK_LINUX__	
		return strcasecmp(T::ptr, lpsz); 
	#else
		return _strcmpi(T::ptr, lpsz); 
	#endif
	}
	
	void MakeUpper()   { Upper(); }
	void MakeLower()   { Lower(); } 
	DINT Delete(DINT nIndex, DINT nCount = 1) { T::Remove(nIndex, nCount); if (T::ptr)T::ptr[T::Len - 1] = 0; return T::Len; }
	DINT Remove(DINT nIndex, DINT nCount) { T::Remove(nIndex, nCount); if (T::ptr)T::ptr[T::Len - 1] = 0; return T::Len; }
	DINT Replace(char chOld, char chNew);
	DINT Remove(char chRemove);
	//const char *GetBuffer(DINT nMinBufLength) { if (T::Len < nMinBufLength) T::SetLen(nMinBufLength + 1); if (!T::ptr)  {char *p = (char *)alloca(1); *p = 0; return p; } return  T::ptr + nMinBufLength; }
	char *GetBuffer(DINT nMinBufLength);
	void ReleaseBuffer(DINT nNewLength = 0) { T::SetLen(nNewLength); }
	char *GetBufferSetLength(DINT nNewLength);
	TmString &operator = (const T &s) {  T::operator =(s); T::Add() = 0; return *this; }
	TmString& operator = (const TmString &s) {
		return Copy(s.GetPtr(), s.Type());
	}
	TmString& operator = (const char* s) {
		return Copy(s, _CP);
	}

	TmString & Copy (const char* s, int cp);

	TmString operator + (TmString &s) {  TmString a(*this); a += s; return a; }
	TmString operator + (const char *s) {  TmString a(*this); a += s; return a; }
	TmString &operator += (const TmString &s); 
	TmString &operator += (const char *s);
	TmString &operator += (char s);
	TmString &operator += (UINT s) { char str[16]; sprintf(str, "%u", s); operator += (str); return *this; }
	TmString &operator += (USHORT s) { return operator+=((UINT)s); }
	TmString &operator += (int s) { char str[16]; sprintf(str, "%d", s); operator += (str); return *this; }
	TmString &operator += (short s) { return operator+=((int)s); }
	TmString &operator += (float s)  { char str[64]; sprintf(str, "%.4f", s); operator += (str); return *this; }
	TmString &operator += (double s)  { char str[128]; sprintf(str, "%f", s); operator += (str); return *this; }
#ifdef WIN32
	TmString &operator += (INT64 s)  { char str[64]; sprintf(str, "%I64d", s); operator += (str); return *this; }
#else
	TmString &operator += (INT64 s)  { char str[64]; sprintf(str, "%lld", s); operator += (str); return *this; }
#endif

	TmString &operator + (UINT s)  {  TmString a(*this); a += s; return a; }
	TmString &operator + (USHORT s)  {  TmString a(*this); a += s; return a; }
	TmString &operator + (float s)   {  TmString a(*this); a += s; return a; }
	TmString &operator + (double s)   {  TmString a(*this); a += s; return a; }
	TmString &operator + (INT64 s)   {  TmString a(*this); a += s; return a; }



	BOOL operator > (const TmString &s) const { if (T::Len && !s.Len) return true;
	if (s.Len && !T::Len) return false; return strcmp(T::ptr, s.ptr) > 0; }
	BOOL operator < (const TmString &s) const { if (T::Len && !s.Len) return false;
	if (s.Len && !T::Len) return true; return strcmp(T::ptr, s.ptr) < 0; }
	BOOL operator > (const char *s) const { if (T::Len && !s) return true;
	if (s && strlen(s) && !T::Len) return false; return strcmp(T::ptr, s) > 0; }
	BOOL operator < (const char *s)const {if (T::Len && !s) return false;
	if (s && strlen(s) && !T::Len) return true; return strcmp(T::ptr, s) < 0; }
	 
	BOOL operator == (const char *s)const { return Cmp(s); }; 
	BOOL operator != (const char *s)const { return !Cmp(s); };
	BOOL operator == (const TmString &s) const{ return Cmp(s.ptr); };
	BOOL operator != (const TmString &s)const { return !Cmp(s.ptr); };
	int StrLen() const { return T::Len && T::ptr ? (int)strlen(T::ptr) : 0; }
	DINT Insert(DINT nIndex, char ch); 
	DINT Insert(DINT nIndex, const char *pstr); 
	DINT Insert(DINT nIndex, TmString &str) { return Insert(nIndex, str.ptr); } 

	int Find(const char *s, DINT offset = 0); 
	int Find(char s, DINT offset = 0); 
	int ReverseFind(char ch); 
	TmString &operator = (const wchar_t *s);
	void Format(const char *s, ...);
#ifndef _EOS_LINUX_

	void Format(const wchar_t *s, ...);	
#endif
	operator const char *() const { return T::ptr ? T::ptr : ""; }
#ifdef __PLUK_LINUX__
#ifdef ANDROID_NDK
	void Upper() { for (int i = 0, len = StrLen(); i < len; ++i) T::ptr[i] =toupper(T::ptr[i]); }
	void Lower() { for (int i = 0, len = StrLen(); i < len; ++i) T::ptr[i] =tolower(T::ptr[i]); }
#else
	void Upper() { for (int i = 0, len = StrLen(); i < len; ++i) T::ptr[i] =_toupper(T::ptr[i]); }
	void Lower() { for (int i = 0, len = StrLen(); i < len; ++i) T::ptr[i] =_tolower(T::ptr[i]); }
#endif
#else
	void Upper() { if (T::ptr) _strupr_s(T::ptr, T::Len); }
	void Lower() { if (T::ptr) _strlwr_s(T::ptr, T::Len); }
#endif
	BOOL Replace(TmString dst, TmString src, DINT offset = 0);
	void ReplaceAll(TmString dst, TmString src)
	{
	int i = 0;
		while ((i = Find(dst.GetPtr(), i)) != -1)
			Replace(dst, src, i);
	}
	MVector<TmString> Tokenize(const char *ps, bool fAll = true, bool fClear0 = true) { MVector<TmString> v; Tokenize(ps, v, fAll, fClear0); return v; };
	void Tokenize(const char *seps, MVector<TmString> &arr, bool fAll = true, bool fClear0 = true);
	void Tokenize(const char *seps, MArray<int> &arr, bool fAll = true, bool fClear0 = true);
	TmString Tail(DINT i) {return TmString(T::Tail(i)); } 
	TmString Left(DINT i) {return TmString(T::Left(i)); } 
	TmString Right(DINT i) {return TmString(T::Right(i + T::Len - StrLen())); }
	TmString Mid(DINT first, DINT count = -1) {if (!T::ptr) count = 0; else if (count == -1) count = strlen(T::ptr) - first; if (!count) return TmString(); return TmString(T::Mid(first, count)); }
	void Mid(TmString &res, DINT first, DINT count = -1);
	BOOL IsEmpty() { return !StrLen(); }
	BOOL Cmp(const char *s) const;
	void Swap() { DINT n = StrLen(); if (n > 1) T::ReOrder (0, n - 1); }
	void ClearSpace() { Trim(); } 
	DINT GetAllocLength() const { return T::Len; }

	TmString &Trim(char ch =  ' ');
	TmString &Trim(const char *ch);
	int FindOneOf(const char *ch);

	TmString &TrimLeft(char ch =  ' ');
	TmString &TrimRight(char ch =  ' ');
	void MakeReverse(){ Swap();}

#ifndef __NO_OLE__
#ifdef WIN32
	wchar_t *AllocSysString();
#endif
#endif
};
template <class T>
class  WTmString : public T {
public:
	WTmString() { };
	WTmString(int i):T(i >= 0 ? i : 0) { };
#ifdef WXDLLIMPEXP_BASE 
        WTmString(const wxString &s) { *this = (const wxChar *)s; };
        WTmString &operator = (const wxString &s) { *this = (const wxChar *)s; return *this; };
#endif
        
	WTmString(const T &s):T (s) { T::SetLen(T::Len + 1); T::ptr[T::Len - 1] = 0; };
	WTmString(const WTmString &s):T (s) { };
	WTmString(const wchar_t *s):T () {  *this = s; };
	WTmString(const char *s):T () {  *this = s; };
	WTmString(const TmString<MArray<char>, CP_UTF8>& s) :T() { *this = s; };
	WTmString(const TmString<MArray<char>, CP_THREAD_ACP>& s) :T() { *this = s; };

	~WTmString() {};
	WTmString(wchar_t ch, int nRepeat = 1) { T::SetLen(nRepeat + 1); for (DINT i = 0; i < nRepeat; ++i) T::ptr[i] = ch; }
	WTmString(const wchar_t *lpch, DINT nLength) { T::Copy(lpch, nLength); }
	int GetLength()  { return StrLen(); }
	BOOL IsEmpty() const { return !T::ptr || !T::ptr[0]; }
	void Empty() { T::Clear(); }
	DINT ToInt() { DINT n = 0; if (StrLen()) swscanf(T::ptr, L"%d", &n); return n; }
	double ToDouble() { double n = 0.0; if (StrLen()) swscanf(T::ptr, L"%lf", &n); return n; }
	bool IsNumber () { double db; return ToDouble(&db); }
	bool ToDouble(double *db) { DINT fRet = 0; if (StrLen()) fRet = swscanf(T::ptr, L"%lf", db); return fRet == StrLen(); }
	bool ToInt(DINT *db) { DINT fRet = 0; if (StrLen()) fRet = swscanf(T::ptr, L"%d", db); return fRet == StrLen(); }
	void ReplaceAll(WTmString dst, WTmString src)
	{
		int i = 0;
		while ((i = Find(dst.GetPtr(), i)) != -1)
			Replace(dst, src, i);
	}

	wchar_t GetAt(DINT nIndex) { return T::ptr[nIndex]; }
	void SetAt(DINT nIndex, wchar_t ch) { T::ptr[nIndex] = ch ; }
	const WTmString &operator = (wchar_t ch) { T::SetLen(2); T::ptr[0] = ch; T::ptr[1] = 0; return *this; }
	int Compare(const wchar_t *lpsz) const { 
		if (T::Len && (!lpsz || !lpsz[0])) return 1;
		if (lpsz && lpsz[0] && !T::Len) return -1; 
		if ((!T::Len || !T::ptr[0]) && (!lpsz || !lpsz[0])) return 0;
		return wcscmp(lpsz, T::ptr); 
	}
	int CompareNoCase(const wchar_t *lpsz) const { 
		if (T::Len && (!lpsz || !lpsz[0])) return 1;
		if (lpsz && lpsz[0] && !T::Len) return -1; 
		if ((!T::Len || !T::ptr[0]) && (!lpsz || !lpsz[0])) return 0;
	#ifdef __PLUK_LINUX__	
		return wcscasecmp(T::ptr, lpsz); 
	#else
		return _wcsicmp(T::ptr, lpsz); 
	#endif
	}
	
	void MakeUpper()  { Upper(); }
	void MakeLower()  { Lower(); } 
	DINT Delete(DINT nIndex, DINT nCount = 1) { T::Remove(nIndex, nCount); if (T::ptr)T::ptr[T::Len - 1] = 0; return T::Len; }
	DINT Remove(DINT nIndex, DINT nCount) { T::Remove(nIndex, nCount); if (T::ptr)T::ptr[T::Len - 1] = 0; return T::Len; }

	char *GetBuffer(DINT nMinBufLength) { if (T::Len < nMinBufLength/sizeof(wchar_t)) T::SetLen(nMinBufLength/sizeof(wchar_t) + 1); return ((char *)T::ptr) + nMinBufLength; }
	void ReleaseBuffer(DINT nNewLength = -1) { if (nNewLength <= 0) T::Clear(); else T::SetLen(nNewLength); }
	char *GetBufferSetLength(DINT nNewLength) { T::SetLen( nNewLength/sizeof(wchar_t)); return T::ptr ? T::ptr : L""; }

	void Swap() { DINT n = StrLen(); if (n > 1) T::ReOrder (0, n - 1); }
	WTmString &operator = (const TmString<MArray<char>, CP_UTF8> &s) {
		return Copy(s.GetPtr(), CP_UTF8);
	}
	WTmString& operator = (const TmString<MArray<char>, CP_THREAD_ACP>& s) {
		return Copy(s.GetPtr(), CP_THREAD_ACP);
	}
	WTmString& operator = (const char* s) {
		return Copy(s, CP_THREAD_ACP);
	}
	WTmString& Copy(const char* s, int cp);

	//WTmString &operator = (const char *s);
	wchar_t &operator [] (size_t i) const {return T::ptr[i]; }
	wchar_t &operator [] (int i) const {return T::ptr[i]; }
        
	WTmString &operator = (T &s) {  T::operator =(s); return *this; }
	WTmString &operator = (const WTmString &s) {  T::operator =(s); if (!T::Len && T::ptr)T::ptr[0] = 0; return *this; }
	WTmString operator + (WTmString &s) {  WTmString a(*this); a += s; return a; }
	WTmString operator + (const wchar_t *s) {  WTmString a(*this); a += s; return a; }
	WTmString operator + (const char *s) { WTmString a(s), b(*this); b += a; return b; }
	WTmString& operator + (const TmString<MArray<char>, CP_UTF8>& s) { WTmString a(s), b(*this); b += a; return b; }
	WTmString& operator + (const TmString<MArray<char>, CP_THREAD_ACP>& s) { WTmString a(s), b(*this); b += a; return b; }

	WTmString &operator += (const WTmString &s); 
	WTmString &operator += (const char *s) { WTmString a(s); *this += a; return *this; }
	WTmString& operator += (const TmString<MArray<char>, CP_UTF8>& s) { WTmString a(s); *this += a; return *this; }
	WTmString& operator += (const TmString<MArray<char>, CP_THREAD_ACP>& s) { WTmString a(s); *this += a; return *this; }
	WTmString &operator += (const wchar_t *s); 
	WTmString &operator += (wchar_t s);

	WTmString &operator = (const wchar_t *s);
	WTmString &operator += (UINT s) { wchar_t str[16]; swprintf(str, 16, L"%u", s); operator += (str); return *this; }
	WTmString &operator += (USHORT s) { return operator+=((UINT)s); }
	WTmString &operator += (int s) { wchar_t str[16]; swprintf(str, 16, L"%d", s); operator += (str); return *this; }
	WTmString &operator += (short s) { return operator+=((int)s); }
	WTmString &operator += (float s)  { wchar_t str[64]; swprintf(str, 64, L"%.4f", s); operator += (str); return *this; }
	WTmString &operator += (double s)  { wchar_t str[128]; swprintf(str, 128, L"%f", s); operator += (str); return *this; }
#ifdef WIN32
	WTmString &operator += (INT64 s)  { wchar_t str[64]; swprintf(str, 64, L"%I64d", s); operator += (str); return *this; }
#else
	WTmString &operator += (INT64 s)  { wchar_t str[64]; swprintf(str, 64, L"%lld", s); operator += (str); return *this; }
#endif
	WTmString &operator + (UINT s)  {  WTmString a(*this); a += s; return a; }
	WTmString &operator + (USHORT s)  {  WTmString a(*this); a += s; return a; }
	WTmString &operator + (float s)   {  WTmString a(*this); a += s; return a; }
	WTmString &operator + (double s)   {  WTmString a(*this); a += s; return a; }
	WTmString &operator + (INT64 s)   {  WTmString a(*this); a += s; return a; }

	BOOL operator == (const wchar_t *s) const { return Cmp(s); }; 
	BOOL operator != (const wchar_t *s) const { return !Cmp(s); };
	BOOL operator == (const WTmString &s) const { return Cmp(s.ptr); };
	BOOL operator != (const WTmString &s) const { return !Cmp(s.ptr); };
	
	BOOL operator > (const WTmString &s) const { if (T::Len && !s.Len) return true;
	if ((s.Len && !T::Len) || (!s.Len && !T::Len)) return false; return wcscmp(T::ptr, s.ptr) > 0; }
	BOOL operator < (const WTmString &s) { if ((T::Len && !s.Len) || (!T::Len && !s.Len)) return false;
	if (s.Len && !T::Len) return true; return wcscmp(T::ptr, s.ptr) < 0; }
	BOOL operator > (const wchar_t *s) const { if ((T::Len && !s) || (!T::Len && !s)) return true;
	if ((s && wcslen(s)) && !T::Len) return false; return wcscmp(T::ptr, s) > 0; }
	BOOL operator < (const wchar_t *s) const {if ((T::Len && !s) || (!T::Len && !s)) return false;
	if ((s && wcslen(s)) && !T::Len) return true; return wcscmp(T::ptr, s) < 0; }


	int StrLen() { return T::Len && T::ptr ? (int)wcslen(T::ptr) : 0; }
	DINT Insert(DINT nIndex, wchar_t ch); 
	DINT Insert(DINT nIndex, const wchar_t *pstr); 
	DINT Insert(DINT nIndex, const WTmString &str) { return Insert(nIndex, str.ptr); } 

	int Find(const WTmString &s); 
	int Find(const wchar_t *s, DINT offset = 0); 
	int Find(wchar_t s, DINT offset = 0);
	BOOL Replace(WTmString dst, WTmString src, DINT offset = 0);
	int ReverseFind(wchar_t ch); 
	void Format(const char *s, ...);	
	void Format(const wchar_t *s, ...);	
	operator const wchar_t *() const { return T::ptr ? T::ptr : L""; }
#ifdef WIN32
	void Upper() { if (T::ptr) _wcsupr(T::ptr); }
	void Lower() { if (T::ptr) _wcslwr(T::ptr); }
#else        
        void Upper() { for (DINT i = 0, len = StrLen(); i < len; ++i) T::ptr[i] =towupper(T::ptr[i]); }
	void Lower() { for (DINT i = 0, len = StrLen(); i < len; ++i) T::ptr[i] =towlower(T::ptr[i]); }
#endif
        
	BOOL IsEmpty() { return !StrLen(); }

	MVector<WTmString> Tokenize(const wchar_t *ps, bool fAll = true, bool fClear0 = true) { MVector<WTmString> v; Tokenize(ps, v, fAll, fClear0); return v; };
	void Tokenize(const wchar_t *seps, MVector<WTmString> &arr, bool fAll = true, bool fClear0 = true);
	void Tokenize(const wchar_t *seps, MArray<int> &arr, bool fAll = true, bool fClear0 = true);
	WTmString Tail(DINT i) {return WTmString(T::Tail(i)); } 
	WTmString Left(DINT i) {return WTmString(T::Left(i)); } 
	WTmString Right(DINT i) {return WTmString(T::Right(i + T::Len - StrLen())); }
	WTmString Mid(DINT first, DINT count) {return WTmString(T::Mid(first, count)); } 
	BOOL Cmp(const wchar_t *s) const; 
	void ClearSpace() { Trim(); } 
	DINT Replace(wchar_t chOld, wchar_t chNew);
	DINT Remove(wchar_t chRemove);
	WTmString &Trim(wchar_t ch =  L' ');
	WTmString &Trim(const wchar_t *ch);
	int FindOneOf(const wchar_t *ch);
	WTmString &TrimLeft(wchar_t ch =  L' ');
	WTmString &TrimRight(wchar_t ch =  L' ');
	void MakeReverse(){  Swap();}

#ifndef __NO_OLE__
#ifdef WIN32
	wchar_t *AllocSysString();
#endif
#endif
};
template <class T, int _CP>
char *TmString<T, _CP>::GetBuffer(DINT nMinBufLength)
{
	if (T::Len < nMinBufLength)
		T::SetLen(nMinBufLength + 1);
	if (!T::ptr)
		T::Add()= 0;
	return  T::ptr + nMinBufLength;
}
template <class T, int _CP>
char *TmString<T, _CP>::GetBufferSetLength(DINT nNewLength)
{
	T::SetLen( nNewLength);
	if (!T::ptr)
		T::Add()= 0;
	return T::ptr;
}

//---------------------------------------------
//
//---------------------------------------------
template <class T, int _CP>
inline DINT TmString<T, _CP>::Replace(char chOld, char chNew)
{
        DINT k = 0;
        for (DINT i = 0; i < T::Len; ++i)
        {
                if (T::ptr[i] ==chOld)
                {
                        T::ptr[i] = chNew;
                        ++k;
                }
        }
        return k;
}
//---------------------------------------------
//
//---------------------------------------------
template <class T, int _CP>
inline DINT TmString<T, _CP>::Remove(char chRemove)
{
        DINT k = 0;
        for (DINT i = 0; i < T::Len; ++i)
        {
                if (T::ptr[i] ==chRemove)
                {
                        Delete(i);
                        --i;
                        ++k;
                }
        }
        return k;
}

//---------------------------------------------
//
//---------------------------------------------
template <class T, int _CP>
TmString<T, _CP> &TmString<T, _CP>::Trim(char ch)
{
        DINT k = StrLen();
        while (k && T::ptr[0] == ch)
        {
                T::Remove(0, 1);
                --k;
        }
        while (k && T::ptr[k - 1] == ch)
        {
                T::ptr[k - 1] = 0;
                --k;
        }
        return *this;

} 
//---------------------------------------------
//
//---------------------------------------------
template <class T, int _CP>
TmString<T, _CP> &TmString<T, _CP>::Trim(const char *ch)
{
        if (!ch || !ch[0]) return *this;
        DINT k = StrLen();
        while (k)
        {
                char test = T::ptr[0];
                const char *ctrl = ch;
                while (*ctrl)
                {
                        if (*ctrl == test)
                                break; 	
                        ctrl++;
                }
                if (!*ctrl)
                        break;
                T::Remove(0, 1);
                --k;
        }
        while (k)
        {
                char test = T::ptr[k - 1];
                const char *ctrl = ch;
                while (*ctrl)
                {
                        if (*ctrl == test)
                                break; 	
                        ctrl++;
                }
                if (!*ctrl)
                        break;
                T::ptr[k - 1] = 0;
                --k;
        }
        return *this;

} 
//---------------------------------------------
//
//---------------------------------------------
template <class T, int _CP>
inline int TmString<T, _CP>::FindOneOf(const char *ch)
{
        if (!ch || !ch[0]) return -1;
        DINT l = StrLen();
        for (DINT i = 0; i < l; ++i)
        {
                char test = T::ptr[i];
                const char *ctrl = ch;
                while (*ctrl)
                {
                        if (*ctrl == test)
                                return (int)i; 	
                        ctrl++;
                }
        }
        return -1;
}

//---------------------------------------------
//
//---------------------------------------------
template <class T, int _CP>
inline TmString<T, _CP> &TmString<T, _CP>::TrimLeft(char ch)
{
        DINT k = StrLen();
        while (k && T::ptr[0] == ch)
        {
                T::Remove(0, 1);
                --k;
        }
        return *this;
}
//---------------------------------------------
//
//---------------------------------------------
template <class T, int _CP>
inline TmString<T, _CP> &TmString<T, _CP>::TrimRight(char ch)  
{
        DINT k = StrLen();
        while (k && T::ptr[k - 1] == ch)
        {
                T::ptr[k - 1] = 0;
                --k;
        }
        return *this;
}
//---------------------------------------------
//
//---------------------------------------------
template <class T, int _CP>
inline int TmString<T, _CP>::Find(const char *s, DINT offset) 
{ 
	char *c; 
	if (s && T::Len && offset < T::Len && (c = strstr(T::ptr + offset, s)))
		return (int)(c - T::ptr);
	return -1; 
}
//---------------------------------------------
//
//---------------------------------------------
template <class T, int _CP>
inline int TmString<T,_CP>::Find(char s, DINT offset)
{ 
	char *c; 
	if (T::Len && offset < T::Len && (c = strchr(T::ptr + offset, s)))
		return (int)(c - T::ptr);
	return -1; 
}

//---------------------------------------------
//
//---------------------------------------------
template <class T, int _CP>
inline int TmString<T, _CP>::ReverseFind(char ch)
{ 
	char *c; 
	if (T::Len && (c = strrchr(T::ptr, ch)))
		return (int)(c - T::ptr);
	return -1; 
}

//---------------------------------------------
//
//---------------------------------------------
template <class T, int _CP>
inline TmString<T, _CP> &TmString<T, _CP>::operator += (const TmString<T, _CP> &s) 
{  
	if (s.Len > 1)
	{
		if (T::Len)
		{
			T::ReAlloc((DINT)strlen((const char *)s.ptr) + T::Len); 
#ifndef WIN32
			strcat(T::ptr, s.ptr);
#else
			strcat_s(T::ptr, T::Len, s.ptr);
#endif

		}
		else 
			*this = s; 
	}
	return *this; 
}
template <class T, int _CP>
inline DINT TmString<T, _CP>::Insert(DINT nIndex, char ch)
{
	if (StrLen() > nIndex)
	{
		T::Expand(nIndex, 1);
		memcpy(T::ptr + nIndex, &ch, 1);
	}
	else
		*this += ch;
	return StrLen();
}
template <class T, int _CP>
inline DINT TmString<T, _CP>::Insert(DINT nIndex, const char *pstr)
{

	if ((DINT)StrLen() > nIndex)
	{
		DINT n = (DINT)strlen(pstr);
		T::Expand(nIndex, n);
		memcpy(T::ptr + nIndex, pstr, n);
	}
	else
		*this += pstr;
	return StrLen();
} 

template <class T, int _CP>
BOOL TmString<T, _CP>::Replace(TmString<T, _CP> dst, TmString<T, _CP> src, DINT offset)
{
	if (!dst.Len) return 0;
	int i = Find(dst.GetPtr(), offset);
	if (i != -1)
	{
		if (src.StrLen() < dst.StrLen())
			T::Remove(i, dst.StrLen() - src.StrLen());
		else
		if (src.StrLen() > dst.StrLen())
			T::Expand(i, src.StrLen() - dst.StrLen());
		if (src.StrLen())
			memcpy(T::ptr + i, src.ptr, src.StrLen());
		return 1;

	}
	return 0; 
}

template <class T, int _CP>
void TmString<T, _CP>::Mid(TmString &res, DINT first, DINT count)
{
	if (!T::ptr)
		count = 0; 
	else 
		if (count < 0) 
			count = (DINT)strlen(T::ptr) - first;
	if (!count)
		res.Add() = 0;
	else
	{	
		
		if (T::Len)
			_ARRAY<char, MBuffer<char> >::Mid(res, first, count); 
		res.Add() = 0; 

	} 
}
//---------------------------------------------
//
//---------------------------------------------
template <class T, int _CP>
inline TmString<T, _CP> &TmString<T, _CP>::operator += (const char *s) 
{ 
	if (s)
	{
		if (T::Len)
		{
			T::ReAlloc((DINT)strlen(s) + T::Len);
#ifndef WIN32
			strcat(T::ptr, s);
#else
			strcat_s(T::ptr, T::Len - 1 , s);
#endif

		} 
		else 
			*this = s; 
	}
	return *this; 
}
template <class T, int _CP>
inline TmString<T, _CP>& TmString<T, _CP>::Copy(const char* s, int cp)
{
		size_t n;
#ifndef __PLUK_LINUX__
	if (s && HIWORD(s) == 0)
	{
#ifdef GetDllResourceInstance
		UINT nID = LOWORD((DWORD)(DWORD_PTR)s);
		HINSTANCE hInst = GetDllResourceInstance();
		TCHAR b[1024];

		memset(b, 0, 1024 * sizeof(TCHAR));
		if (LoadString(hInst, nID, b, 1024))
			return operator = (b);	
#endif

		T::Clear();
		return *this;
	}
#endif
	if (_CP == cp)
	{
		WTmString< MArray<wchar_t> > tmp;
		tmp.Copy(s, cp);
		return operator = (tmp); 
	}


	if (s && (n = strlen(s)))
	{
		if (s >= T::ptr && s < T::ptr + T::Len)
		{
			*this = TmString(s);
			return *this;
		}   
		T::ReAlloc((DINT)n + 1);
#ifndef WIN32
		strcpy(T::ptr, s);
#else
		strcpy_s(T::ptr, T::Len, s);
#endif
	}
	else
		T::Clear();

	return *this; 
}

//---------------------------------------------
//
//---------------------------------------------
/*
template <class T, int _CP>
inline TmString<T, _CP> &TmString<T, _CP>::operator = (const char *s) 
{ 
	size_t n;
#ifndef __PLUK_LINUX__
	if (s && HIWORD(s) == 0)
	{
#ifdef GetDllResourceInstance
		UINT nID = LOWORD((DWORD)(DWORD_PTR)s);
		HINSTANCE hInst = GetDllResourceInstance();
		TCHAR b[1024];

		memset(b, 0, 1024 * sizeof(TCHAR));
		if (LoadString(hInst, nID, b, 1024))
			return operator = (b);	
#endif

		T::Clear();
		return *this;
	}
#endif

	if (s && (n = strlen(s)))
	{
		if (s >= T::ptr && s < T::ptr + T::Len)
		{
			*this = TmString(s);
			return *this;
		}   
		T::ReAlloc((DINT)n + 1);
#ifndef WIN32
		strcpy(T::ptr, s);
#else
		strcpy_s(T::ptr, T::Len, s);
#endif
	}
	else
		T::Clear();

	return *this; 
}
*/
//---------------------------------------------
//
//---------------------------------------------
template <class T, int _CP>
inline BOOL TmString<T, _CP>::Cmp(const char *s) const
{  
	if ((!s || !*s) && (!T::ptr || !*T::ptr)) return true;
	if (s  && T::ptr)
		return !strcmp(s, T::ptr);
	return s == T::ptr;
}


//---------------------------------------------
//
//---------------------------------------------
template <class T, int _CP>
void TmString<T, _CP>::Format(const char *pFormat, ...)
{ 
    
    if (!pFormat || !pFormat[0]) return;
	if (strlen(pFormat) + 1024 > T::Len)
		T::ReAlloc((DINT)strlen(pFormat) + 1024);
	va_list args;
	va_start(args, pFormat);
#ifdef __PLUK_LINUX__
	vsprintf(T::ptr, pFormat, args);
#else
	vsprintf_s(T::ptr, T::Len,  pFormat, args);
#endif
	va_end(args);
	DINT n;
	if ((n = (DINT)strlen(T::ptr) ))
	{
		T::Len = n + 1;
		T::ptr[n] = 0;
	}
	else
		T::Clear();
}
#ifndef _EOS_LINUX_
template <class T, int _CP>
void TmString<T, _CP>::Format(const wchar_t *_pFormat, ...)
{ 
    TmString<T, _CP> c;
    c = _pFormat;
    const char *pFormat = c.ptr;
    if (!pFormat || !pFormat[0]) return;
	T::ReAlloc((DINT)strlen(pFormat) + 1024);
	va_list args;
	va_start(args, pFormat);
#ifdef __PLUK_LINUX__
	vsprintf(T::ptr, pFormat, args);
#else
	vsprintf_s(T::ptr, T::Len, pFormat, args);
#endif
	va_end(args);
	DINT n;
	if ((n = strlen(T::ptr) ))
	{
		T::Len = n + 1;
		T::ptr[n] = 0;
	}
	else
		T::Clear();
}
#endif
//---------------------------------------------
//
//---------------------------------------------
template <class T, int _CP>
inline TmString<T, _CP> &TmString<T, _CP>::operator = (const wchar_t *s)
{
	DINT n;
#ifndef __PLUK_LINUX__
	if (s && HIWORD(s) == 0)
	{
#ifdef GetDllResourceInstance
		UINT nID = LOWORD((DWORD)(DWORD_PTR)s);
		HINSTANCE hInst = GetDllResourceInstance();
		TCHAR b[512];
		memset(b, 0, 512 * sizeof(TCHAR));
		if (LoadString(hInst, nID, b, 512))
			return operator = (b);	
#endif
		T::Clear();
		return *this;
	}
#endif
#ifndef ANDROID_NDK

	if (s && (n = __U2TLen(_CP, s)))
	{

		n = (DINT)(n + 1);
		T::ReAlloc(n);
		__U2T(_CP, s, T::ptr, n);
	}
#else
	if (s && (n = wcslen(s)))

		WideCharToMultiByte(s, *this);
#endif

	else
		T::Clear();
	return *this;
}
//---------------------------------------------
//
//---------------------------------------------
template <class T>
inline DINT WTmString<T>::Replace(wchar_t chOld, wchar_t chNew)
{
        DINT k = 0;
        for (DINT i = 0; i < T::Len; ++i)
        {
                if (T::ptr[i] ==chOld)
                {
                        T::ptr[i] = chNew;
                        ++k;
                }
        }
        return k;
}
//---------------------------------------------
//
//---------------------------------------------
template <class T>
inline DINT WTmString<T>::Remove(wchar_t chRemove)
{
        DINT k = 0;
        for (DINT i = 0; i < T::Len; ++i)
        {
                if (T::ptr[i] ==chRemove)
                {
                        Delete(i);
                        --i;
                        ++k;
                }
        }
        return k;
}
//---------------------------------------------
//
//---------------------------------------------
template <class T>
WTmString<T> &WTmString<T>::Trim(wchar_t ch)
{
        DINT k = StrLen();
        while (k && T::ptr[0] == ch)
        {
                T::Remove(0, 1);
                --k;
        }
        while (k && T::ptr[k - 1] == ch)
        {
                T::ptr[k - 1] = 0;
                --k;
        }
        return *this;

} 
//---------------------------------------------
//
//---------------------------------------------
template <class T>
WTmString<T> &WTmString<T>::Trim(const wchar_t *ch)
{
        if (!ch || !ch[0]) return *this;
        DINT k = StrLen();
        while (k)
        {
                wchar_t test = T::ptr[0];
                const wchar_t *ctrl = ch;
                while (*ctrl)
                {
                        if (*ctrl == test)
                                break; 	
                        ctrl++;
                }
                if (!*ctrl)
                        break;
                T::Remove(0, 1);
                --k;
        }
        while (k)
        {
                wchar_t test = T::ptr[k - 1];
                const wchar_t *ctrl = ch;
                while (*ctrl)
                {
                        if (*ctrl == test)
                                break; 	
                        ctrl++;
                }
                if (!*ctrl)
                        break;
                T::ptr[k - 1] = 0;
                --k;
        }
        return *this;

} 
//---------------------------------------------
//
//---------------------------------------------
template <class T>
int WTmString<T>::FindOneOf(const wchar_t *ch)
{
        if (!ch || !ch[0]) return -1;
        DINT l = StrLen();
        for (DINT i = 0; i < l; ++i)
        {
                wchar_t test = T::ptr[i];
                const wchar_t *ctrl = ch;
                while (*ctrl)
                {
                        if (*ctrl == test)
                                return (int)i; 	
                        ctrl++;
                }
        }
        return -1;
}
//---------------------------------------------
//
//---------------------------------------------
template <class T>
inline WTmString<T> &WTmString<T>::TrimLeft(wchar_t ch) 
{
        DINT k = StrLen();
        while (k && T::ptr[0] == ch)
        {
                T::Remove(0, 1);
                --k;
        }
        return *this;
}
//---------------------------------------------
//
//---------------------------------------------
template <class T>
inline WTmString<T> &WTmString<T>::TrimRight(wchar_t ch)  
{
        DINT k = StrLen();
        while (k && T::ptr[k - 1] == ch)
        {
                T::ptr[k - 1] = 0;
                --k;
        }
        return *this;
}
template <class T>
inline WTmString<T>& WTmString<T>::Copy(const char* s, int cp)
{
#ifndef __PLUK_LINUX__
	if (s && HIWORD(s) == 0)
	{
#ifdef GetDllResourceInstance
		UINT nID = LOWORD((DWORD)(DWORD_PTR)s);
		HINSTANCE hInst = GetDllResourceInstance();
		TCHAR b[512];
		memset(b, 0, 512 * sizeof(TCHAR));
		if (LoadString(hInst, nID, b, 512))
			return operator = (b);
#endif
		T::Clear();
		return *this;
	}
#endif

	int n;
	if (s && (n = (int)strlen(s)))
	{
#ifdef WIN32
		n = __T2ULen(cp, s) + 1;
#else
		++n;
#endif
		T::ReAlloc((DINT)n);

		__T2U(cp, s, T::ptr, n);
	}
	else
		T::Clear();
	return *this;

}

/*
//---------------------------------------------
//
//---------------------------------------------
template <class T>
inline WTmString<T> &WTmString<T>::operator = (const char *s)
{
#ifndef __PLUK_LINUX__
	if (s && HIWORD(s) == 0)
	{
#ifdef GetDllResourceInstance
		UINT nID = LOWORD((DWORD)(DWORD_PTR)s);
		HINSTANCE hInst = GetDllResourceInstance();
		TCHAR b[512];
		memset(b, 0, 512 * sizeof(TCHAR));
		if (LoadString(hInst, nID, b, 512))
			return operator = (b);	
#endif
		T::Clear();
		return *this;
	}
#endif

	int n;
	if (s && (n = (int)strlen(s)))
	{
#ifdef WIN32
		n = __T2ULen(s) + 1;
#else
		++n;
#endif
		T::ReAlloc((DINT)n);
		
		__T2U(s, T::ptr, n);
	}
	else
		T::Clear();
	return *this;

}
*/
template <class T, int _CP>
inline TmString<T, _CP> &TmString<T, _CP>::operator += (char s)
{
	char ss[2];
	ss[0] =	s; ss[1] = 0;
	return operator += (ss);
	
}
template <class T>
inline WTmString<T> &WTmString<T>::operator += (wchar_t s)
{
	wchar_t ss[2];
	ss[0] =	s; ss[1] = 0;
	return operator += (ss);
	
}

//---------------------------------------------
//
//---------------------------------------------
template <class T>
inline WTmString<T> &WTmString<T>::operator += (const WTmString<T> &s) 
{  
	if (s.Len) 
	{ 
		if (T::Len)
		{ 
			T::ReAlloc((DINT)wcslen(s.ptr) + T::Len);
#ifndef WIN32
			wcscat(T::ptr, s.ptr);
#else
			wcscat_s(T::ptr, T::Len, s.ptr);
#endif
		} 
		else 
			*this = s; 
	} 
	return *this; 
}
//---------------------------------------------
//
//---------------------------------------------
template <class T>
void WTmString<T>::Format(const char *pFormat, ...)
{ 
    if (!pFormat || !pFormat[0]) return;

    T::ReAlloc((DINT)strlen(pFormat) + 1024);
	va_list args;
	va_start(args, pFormat);
#ifdef __PLUK_LINUX__
	vswprintf(T::ptr, T::Len, WTmString(pFormat).GetPtr(), args);

	#else
	vswprintf_s(T::ptr, T::Len, WTmString(pFormat).GetPtr(), args);
#endif
	va_end(args);
	DINT n = (DINT)wcslen(T::ptr);
	if (n)
	{
		T::Len = n + 1;
		T::ptr[n] = 0;
	}
	else
		T::Clear();

}

//---------------------------------------------
//
//---------------------------------------------
template <class T>
void WTmString<T>::Format(const wchar_t *pFormat, ...)
{ 
        if (!pFormat || !pFormat[0]) return;

	T::ReAlloc((DINT)wcslen(pFormat) + 1024* 4);
	va_list args;
	va_start(args, pFormat);
#ifdef __PLUK_LINUX__
	vswprintf(T::ptr, T::Len, pFormat, args);
#else
	vswprintf_s(T::ptr, T::Len, pFormat, args);
#endif
	va_end(args);
	DINT n;
	if ((n = (DINT)wcslen(T::ptr)))
	{
		T::Len = n + 1;
		T::ptr[n] = 0;
	}
	else
		T::Clear();
}

//---------------------------------------------
//
//---------------------------------------------

template <class T>
inline WTmString<T> &WTmString<T>::operator += (const wchar_t *s) 
{ 
	if (s)
	{
		if (T::Len)
		{
			T::ReAlloc((DINT)wcslen(s) + T::Len);
#ifndef WIN32
			wcscat(T::ptr, s);
#else
			wcscat_s(T::ptr, T::Len, s);
#endif
		} 
		else 
			*this = s; 
	}
	return *this; 
}
template <class T>
inline WTmString<T> &WTmString<T>::operator = (const wchar_t *s) 
{ 
	size_t i;
#ifndef __PLUK_LINUX__
	if (s && HIWORD(s) == 0)
	{
#ifdef GetDllResourceInstance	
		UINT nID = LOWORD((DWORD)(DWORD_PTR)s);
		HINSTANCE hInst = GetDllResourceInstance();
		TCHAR b[512];
		memset(b, 0, 512 * sizeof(TCHAR));
		if (LoadString(hInst, nID, b, 512))
			return operator = (b);	
#endif
		T::Clear();
		return *this;
	}
#endif
	if (s && (i = wcslen(s)))
	{
		if (s >= T::ptr && s < T::ptr + T::Len)
		{
			*this = WTmString(s);
			return *this;
		}   
		T::ReAlloc((DINT)i + 1);
#ifndef WIN32
		wcscpy(T::ptr, s);
#else
		wcscpy_s(T::ptr, T::Len, s);
#endif
	}
	else
		T::Clear();
	return *this;
} 

template <class T>
inline BOOL WTmString<T>::Cmp(const wchar_t *s) const
{  
	if ((!s || !*s) && (!T::ptr || !*T::ptr)) return true;
	if (s  && wcslen(s) && T::ptr)
		return !wcscmp(s, T::ptr);
	return s == T::ptr;
}

//---------------------------------------------
//
//---------------------------------------------
template <class T>
inline int WTmString<T>::Find(const WTmString<T> &s) 
{ 
	wchar_t *c; 
	if (wcslen(s.ptr) && T::Len && (c = wcsstr(T::ptr, s.ptr)))
		return (int)(c - T::ptr);
	return -1; 
}
//---------------------------------------------
//
//---------------------------------------------
template <class T>
inline int WTmString<T>::Find(wchar_t s, DINT offset) 
{ 
	wchar_t *c; 
	if (T::Len && offset < StrLen() && (c = wcschr(T::ptr + offset, s)))
		return (int)(c - T::ptr);
	return -1; 
}

//---------------------------------------------
//
//---------------------------------------------
template <class T>
inline int WTmString<T>::Find(const wchar_t *s, DINT offset) 
{ 
	wchar_t *c; 
	if (s && T::Len && offset < StrLen() && (c = wcsstr(T::ptr + offset, s)))
		return (int)(c - T::ptr);
	return -1; 

}
//---------------------------------------------
//
//---------------------------------------------
template <class T>
inline int WTmString<T>::ReverseFind(wchar_t ch) 
{ 
	wchar_t *c; 
	if (T::Len && (c = wcsrchr(T::ptr, ch)))
		return (int)(c - T::ptr);
	return -1; 
}



//--------------------------------------
//
//--------------------------------------
template <class T, int _CP>
void TmString<T, _CP>::Tokenize(const char *_control, MVector<TmString<T, _CP> > &arr, bool fAll, bool fClear0)
{
	if (!StrLen()) return;


	BYTE *str;
	BYTE *ctrl;
	TmString scontrol(_control),  tmp(T::ptr);
	char *control = scontrol.GetPtr();
	char map[32], *string = tmp.ptr;
	DINT count;
	for(;;)
	{
		ctrl = (BYTE *)control;	

		for (count = 0; count < 32; ++count)
			map[count] = 0;
		do
			map[*ctrl >> 3] |= (1 << (*ctrl & 7));
		while (*ctrl++);

		str = (BYTE  *)string;
		if (fClear0)
			while ( (map[*str >> 3] & (1 << (*str & 7))) && *str )
				str++;
		string = (char *)str;

		for (; *str ; str++)
			if ( map[*str >> 3] & (1 << (*str & 7)) ) 
			{
				*str++ = '\0';
				break;
			}
		if (str == (BYTE *)string)
			break;
		if (!(!fAll && fClear0 && !*string))
			 arr.Add() = string;
		string = (char *)str;
	}
}

template <class T, int _CP>
void TmString<T, _CP>::Tokenize(const char *control, MArray<int> &arr, bool fAll, bool fClear0)
{
	if (!StrLen()) return;

	BYTE *str;
	const BYTE *ctrl;
	TmString tmp(T::ptr);
	char map[32], *string = tmp.ptr;
	DINT count;
	for(;;)
	{
		ctrl = (BYTE *)control;	

		for (count = 0; count < 32; ++count)
			map[count] = 0;
		do
			map[*ctrl >> 3] |= (1 << (*ctrl & 7));
		while (*ctrl++);

		str = (BYTE  *)string;
		if (fClear0)
			while ( (map[*str >> 3] & (1 << (*str & 7))) && *str )
				str++;
		string = (char *)str;

		for (; *str ; str++)
			if ( map[*str >> 3] & (1 << (*str & 7)) ) 
			{
				*str++ = '\0';
				break;
			}
		if (str == (BYTE *)string)
			break;
		if (!(!fAll && fClear0 && !*string))
			 arr.Add() = string - T::ptr;
		string = (char *)str;
	}
}

template <class T>
DINT WTmString<T>::Insert(DINT nIndex, wchar_t ch)
{
	if (StrLen() > nIndex)
	{
		T::Expand(nIndex, 1);
		T::ptr[nIndex] = ch;
	}
	else
		*this += ch;
	return StrLen();
}
template <class T>
DINT WTmString<T>::Insert(DINT nIndex, const wchar_t *pstr)
{

	if (StrLen() > nIndex)
	{
		DINT n = (DINT)wcslen(pstr);
		T::Expand(nIndex, n);
		memcpy(T::ptr + nIndex, pstr, n * sizeof(wchar_t));
	}
	else
		*this += pstr;
	return StrLen();
} 

template <class T>
BOOL WTmString<T>::Replace(WTmString<T> dst, WTmString<T> src, DINT offset)
{
	if (!dst.Len) return 0;
	int i = Find(dst.GetPtr(), offset);
	if (i != -1)
	{
		if (src.StrLen() < dst.StrLen())
			T::Remove(i, dst.StrLen() - src.StrLen());
		else
		if (src.StrLen() > dst.StrLen())
			T::Expand(i, src.StrLen() - dst.StrLen());
		if (src.StrLen())
			memcpy(T::ptr + i, src.ptr, src.StrLen() * sizeof(wchar_t));
		return 1;

	}
	return 0; 
}

//--------------------------------------
//
//--------------------------------------
template <class T>
void WTmString<T>::Tokenize(const wchar_t *control, MVector<WTmString<T> > &arr, bool fAll, bool fClear0)
{
	if (!StrLen()) return;
	 
	WTmString<T>  tmp(T::ptr);
	wchar_t *token, *string = tmp.ptr;
	const wchar_t *ctl;
	for(;;)
	{
		if (fClear0)
			while (*string) 
			{
				for (ctl=control; *ctl && *ctl != *string; ++ctl);
				if (!*ctl) 
					break;
				string++;
			}
		token = string;
		for (; *string; ++string) 
		{
			for (ctl=control; *ctl && *ctl != *string; ++ctl);
			if (*ctl) 
			{
				*string++ = L'\0';
				break;
			}
		}
		if (token == string)
			break;
		if (!fAll && fClear0 && !*token)
			continue;
		arr.Add() = token;
	}
}
template <class T>
void WTmString<T>::Tokenize(const wchar_t *control, MArray<int> &arr, bool fAll, bool fClear0)
{
	if (!StrLen()) return;
	 
	WTmString<T>  tmp(T::ptr);
	wchar_t *token, *string = tmp.ptr;
	const wchar_t *ctl;
	for(;;)
	{
		if (fClear0)
			while (*string) 
			{
				for (ctl=control; *ctl && *ctl != *string; ++ctl);
				if (!*ctl) 
					break;
				string++;
			}
		token = string;
		for (; *string; ++string) 
		{
			for (ctl=control; *ctl && *ctl != *string; ++ctl);
			if (*ctl) 
			{
				*string++ = L'\0';
				break;
			}
		}
		if (token == string)
			break;
		if (!fAll && fClear0 && !*token)
			continue;
		arr.Add() = token - T::ptr;
	}
}

#ifndef __NO_OLE__
#ifdef WIN32
//--------------------------------------
//
//--------------------------------------
template <class T, int _CP>
inline wchar_t *TmString<T, _CP>::AllocSysString()
{
	WTmString<MArray<wchar_t> > str = *this;
	return str.AllocSysString();
}

//--------------------------------------
//
//--------------------------------------
template <class T>
inline wchar_t *WTmString<T>::AllocSysString()
{
	return T::ptr ? SysAllocStringLen(T::ptr, T::Len) : 0;
}
#endif
#endif
typedef TmString< MArray<char>, CP_UTF8> UString;
typedef WTmString< MArray<wchar_t> > WString;
typedef TmString< MArray<char>, CP_THREAD_ACP> TString;


inline TString operator + (char ch, TString& str)
{
    TString s; s += ch; s += str; return s;
}


inline TString operator + (const char *psz, TString& str)
{
    TString s; s = psz; s += str; return s;
}


inline WString operator + (wchar_t ch, WString& str)
{
    WString s; s += ch; s += str; return s;
}


inline WString operator + (const wchar_t *psz, WString& str)
{
    WString s; s = psz; s += str; return s;
}




#ifndef UNICODE
typedef TString MString;
#else
	typedef WString MString;
#endif

#define TlsString TString
#define TlsArray MArray
#pragma pack (pop)


#endif
