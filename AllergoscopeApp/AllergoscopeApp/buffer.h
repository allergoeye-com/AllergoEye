///////////////////////////////////////////////////
// buffer.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////

#ifndef __M_MALLOC__1
#define __M_MALLOC__1

#include <malloc.h>
#define DINTABS(N) ((N < 0) ? -N : N)
#ifndef DINT
	#ifdef _VER_PLK_64
		#define DINT	INT64
		#define DUINT	QWORD
	#else
		#define DINT	int
		#define DUINT	DWORD
	#endif
#endif

#ifdef __PLUK_LINUX__
#include <errno.h>
#ifndef GHND
#define GHND 0
#endif
inline DWORD GetLastError() { return errno; }
inline DWORD SetLastError(DWORD dw) { if (!dw) errno = 0; return 0; }
        #define HEAP_ZERO_MEMORY 1
#define GMEM_ZEROINIT HEAP_ZERO_MEMORY
#define GMEM_FIXED 0
#define M_MEMLABEL 0x12345678U
EXPORTNIX size_t GlobalSize (void *m_hMem);
EXPORTNIX bool GlobalIsValid (HGLOBAL m_hMems);
EXPORTNIX bool GlobalFree (HGLOBAL m_hMems);
EXPORTNIX HGLOBAL GlobalReAlloc (HGLOBAL m_hMem, size_t dwBytes, int dwFlags);
EXPORTNIX HGLOBAL GlobalAlloc (UINT dwFlags, size_t dwBytes);
#define LocalFree GlobalFree
#define LocalAlloc GlobalAlloc
#define LocalUnlock GlobalUnlock
#define LocalLock GlobalLock
EXPORTNIX size_t HeapGetSize();
EXPORTNIX size_t HeapAllocatedSize();
EXPORTNIX size_t HeapFreeSize();

#define HLOCAL HGLOBAL
#define LHND 0

inline size_t HeapSize(HANDLE hHeap, DWORD dwFlags, void *lpMem)
        {
                return GlobalSize(lpMem);
        }

inline BOOL HeapFree (HANDLE hHeap, DWORD dwFlags, void *lpMem)
{
        return GlobalFree(lpMem);
}
inline LPVOID HeapReAlloc(HANDLE hHeap, DWORD dwFlags, void *lpMem, SIZE_T dwBytes)
{
     return GlobalReAlloc (lpMem, dwBytes, dwFlags);
}
inline LPVOID HeapAlloc (HANDLE hHeap,  DWORD dwFlags, SIZE_T dwBytes)
{
        return GlobalAlloc (dwFlags, dwBytes);
}
#define GetProcessHeap() (HANDLE)0x12345678U
#ifdef NOPLUK
extern "C" LPVOID HMALLOC(size_t dwBytes);
extern "C" LPVOID HCALLOC(size_t num, size_t size);
extern "C" LPVOID HREALLOC(void *lpMem, size_t dwBytes);
extern "C" void HFREE(void *lpMem);
inline LPVOID HMALLOCZ(size_t dwBytes) { void *p = HMALLOC(dwBytes); if (p) memset(p, 0, dwBytes); return p; }

#endif 
#ifdef DEBUG_ALLOC
inline void  operator delete( void * p )
{
	HFREE(p);
}
inline void  operator delete[]( void * p )
{
	HFREE(p);
}
inline void *operator new[] (size_t cb)
{
	return HMALLOC(cb);
}
inline void *operator new (size_t cb)
{
	return HMALLOC( cb);
}
#endif

//#define GHND 0
        #define GlobalLock(a) ((void *)a)
        #define GlobalUnlock(a)
        #define CopyMemory memcpy
        #define FillMemory(a, c, b) memset(a, b, c) 

#else
#include <windows.h>
#include <new.h>
//#define USE_HeapSize
#endif
#include <stdarg.h>
#ifndef BoolWarning
#define BoolWarning(a)
#endif

inline void gfree(void *p)
{
	if ((HGLOBAL)p)
#ifndef __PLUK_LINUX__
		GlobalFree((HGLOBAL)p);
#else
	HFREE(p);
#endif
}
inline void *gmalloc(size_t size)
{
#ifndef __PLUK_LINUX__
	void *mem = (void *)GlobalAlloc(GMEM_FIXED, size);
	if(mem)
		ZeroMemory(mem, size);
	return mem;
#else
	void *mem = HMALLOC(size);
	if(mem)
		bzero(mem, size);
	return mem;
#endif
}
inline void *grealloc(void *pOldMem, size_t size)
{
#ifndef __PLUK_LINUX__
	size_t old_size = pOldMem ? (size_t)GlobalSize((HANDLE)pOldMem) : 0;
	void *pNewMem = gmalloc(size);

	if (pNewMem)
	{
		if (old_size)
			CopyMemory(pNewMem, pOldMem, old_size < size ? old_size : size);
		gfree(pOldMem);
	}
	return pNewMem;
#else
	return  HREALLOC(pOldMem, size);
#endif
}
#ifdef _ALLOC_GLOBAL__

	#define hmalloc(size) gmalloc(size)
	#define hrealloc(p, len) grealloc(p, len)
	#define hfree(p) gfree(p)
#else
	#define hmalloc(size) HMALLOC(size)
	#define hrealloc(p, len) HREALLOC(p, len)
	#define hfree(p) HFREE(p)
#endif

#ifdef _WIN32_WCE
void * __cdecl bsearch (
        const void *key,
        const void *base,
        size_t num,
        size_t width,
        int (__cdecl *compare)(const void *, const void *));
#endif
typedef  BOOL (WINAPI *f_Free) (HANDLE hHeap, DWORD dwFlags, void *lpMem);
typedef LPVOID (WINAPI *f_ReAlloc) (HANDLE hHeap, DWORD dwFlags, void *lpMem, SIZE_T dwBytes);
typedef LPVOID (WINAPI *f_Alloc) (HANDLE hHeap,  DWORD dwFlags, SIZE_T dwBytes);
#ifndef __PLUK_LINUX__
#pragma warning (disable : 4786)
#endif

#ifndef DEF_ADD_ARRAY__
	#define DEF_ADD_ARRAY__ sizeof(T) * 4
#endif
template <class T>
class  MMalloc  {
public:
	#define m_Free (HeapFree)
	#define m_ReAlloc (HeapReAlloc)
//	#define USE_HeapSize
	#ifdef HeapAlloc
		#undef HeapAlloc
	#endif
	#define m_Alloc  (HeapAlloc)
	MMalloc() {
	hHeap = GetProcessHeap();
	//fFreeRealloc = true;
	defAdded = DEF_ADD_ARRAY__; RealSize = 0; Len = 0; ptr = 0L; };
	~MMalloc() { Free(); }
	MMalloc(const MMalloc &b) {
#ifndef _ALLOC_GLOBAL__
	hHeap = b.hHeap ? b.hHeap : GetProcessHeap();
	#ifdef __HEAP_USER_ALLOC_
		m_Free = b.m_Free;
		m_ReAlloc = b.m_ReAlloc;
		m_Alloc = b.m_Alloc;
	#endif
#endif
	//fFreeRealloc = b.fFreeRealloc;
	hHeap = b.hHeap; defAdded = b.defAdded; RealSize = 0; Len = 0; ptr = 0L; ;  Copy(b.ptr, b.Len); }
	void SetAddedRealSize(int dw) { defAdded = dw > 0 ? (int)(dw/sizeof(T)) : dw; }
	DINT GetLen() const { return Len; }
	BOOL Compact();

	void Set(T c)
	{
		DINT count = Len;
		T *_ptr = ptr;
		while (count--)	*_ptr++=c;
	}
protected:
	BOOL Malloc(DINT len);
	BOOL ReAlloc(DINT len);
    void Free();
    void Move(MMalloc &m);
    void Swap(MMalloc &m);
    BOOL ExpandBuff(DINT pos, DINT len);
	BOOL Copy(const T *buff, DINT len);
	void CopyTo(T *buff) { if (Len) MemCpy(buff, ptr, Len); }
	void Detach(HANDLE _hHeap = GetProcessHeap()) { ptr = 0; RealSize = Len = 0; hHeap = _hHeap; }
	void MemCpy(T *dst, const T *src, DINT len);
	void Free(T *);
//	void FreeInRealloc(bool fFree) { fFreeRealloc = fFree; }
protected:
	T *ptr;
	DINT Len;
	DINT RealSize;
//	char fFreeRealloc;
	HANDLE hHeap;
#ifndef _ALLOC_GLOBAL__
	#ifdef __HEAP_USER_ALLOC_
		f_Free m_Free;
		f_ReAlloc m_ReAlloc;
		f_Alloc m_Alloc;
	#endif
#endif


private:
	int defAdded;
};

//------------------------------------------
//
//------------------------------------------
template <class T>
inline BOOL MMalloc<T>::Malloc(DINT len)
{
	//BoolWarning (defAdded >= 0);
//	BoolWarning (len >= 0);

	if (!len)
	{
		Free();
		return FALSE;
	}

	if (ptr)
		return ReAlloc(len);
	DINT add = len;
	if (defAdded > -1)
		add = RealSize > (DINT)defAdded ? defAdded : (len > (DINT)defAdded ? (DINT)defAdded : len);


#ifdef _ALLOC_GLOBAL__
	ptr = (T *)gmalloc((len + add) * sizeof(T));
#else
	ptr = hHeap ? (T *)m_Alloc(hHeap, HEAP_ZERO_MEMORY, (len + add) * sizeof(T)) : 0;
#endif

	if (ptr)
	{
		RealSize =len + add;
	#ifdef USE_HeapSize
		// BoolWarning(RealSize == HeapSize(hHeap, 0, ptr)/sizeof(T));
		 RealSize = (int)HeapSize(hHeap, 0, ptr)/sizeof(T);
	#endif
		Len = len;
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------
//
//------------------------------------------
template <class T>
inline BOOL MMalloc<T>::Compact()
{
	if (!ptr) return TRUE;
	T *new_ptr = 0; 
#ifdef _ALLOC_GLOBAL__
	new_ptr = (T *)grealloc(ptr, Len * sizeof(T));
#else
	if (hHeap)
		new_ptr = (T *)m_ReAlloc(hHeap, HEAP_ZERO_MEMORY, ptr, Len * sizeof(T));
	else
		return FALSE;
#endif

	if (new_ptr)
	{
		ptr = new_ptr;
		RealSize = Len;
	#ifdef USE_HeapSize
		// BoolWarning(RealSize == HeapSize(hHeap, 0, new_ptr)/sizeof(T));
		 RealSize = (int)HeapSize(hHeap, 0, new_ptr)/sizeof(T);
	#endif
		return TRUE;
	}
	return FALSE;
	

}

template <class T>
inline BOOL MMalloc<T>::ReAlloc(DINT len)
{
	if (!ptr)
	{
		if (!len) return true;
		return Malloc(len);
	}
	else
	if (RealSize > len)
	{
		if(len < 0) len = 0;
		if (Len < len)
		{
			#ifdef __PLUK_LINUX__
			//bzero(ptr + Len, (len - Len) * sizeof(T));
            memset((char *)ptr + Len * sizeof(T), 0, (len - Len) * sizeof(T));
			#else
			memset(ptr + Len, 0, (len - Len) * sizeof(T));
			#endif
		}
   		Len = len;
		return TRUE;
	}

    if (len <= 0)
    {
    //	if (fFreeRealloc)
		Free();
		Len = 0;
		return 0;
	}
	T *new_ptr;
	DINT add = len;
	if (defAdded > -1)
		add = RealSize > (DINT)defAdded ? (DINT)defAdded : (len > (DINT)defAdded ? (DINT)defAdded : len);

#ifdef _ALLOC_GLOBAL__
	new_ptr = (T *)grealloc(ptr, (len + add) * sizeof(T));
#else
	if (hHeap)
		new_ptr = (T *)m_ReAlloc(hHeap, HEAP_ZERO_MEMORY, ptr, (len  + add) * sizeof(T));
	else
		return FALSE;
#endif

	if (new_ptr)
	{
		ptr = new_ptr;
		RealSize = len + add;
	#ifdef USE_HeapSize
		// BoolWarning(RealSize == HeapSize(hHeap, 0, new_ptr)/sizeof(T));
		 RealSize = (int)HeapSize(hHeap, 0, new_ptr)/sizeof(T);
	#endif
		Len = len;
		return TRUE;
	}

	return FALSE;

}

//------------------------------------------
//
//------------------------------------------
template <class T>
inline void MMalloc<T>::MemCpy(T *dst, const T *src, DINT len)
{
	if (!src || !dst)
		return;
	if (len > Len)
		len = Len;
	memmove(dst, src, len * sizeof(T));
}
//------------------------------------------
//
//------------------------------------------
template <class T>
inline BOOL MMalloc<T>::ExpandBuff(DINT pos, DINT len)
{
	DINT old_len = Len;
	DINT new_len = Len <= pos ? pos + len : Len + len;
	if (!ReAlloc(new_len))
	{
		return FALSE;
	}
	if (old_len > pos)
   		MemCpy(ptr + pos + len, ptr + pos, old_len - pos);
	Len = new_len;
	return TRUE;
}
//----------------------------------------------
//
//----------------------------------------------
template <class T>
inline BOOL MMalloc<T>::Copy(const T *buff, DINT len)
{
	if (len)
	{
		if (!ReAlloc(len))
		{
			Free();
			return FALSE;
		}
		MemCpy(ptr, buff, len);
	}
	else
		Free();

	return TRUE;
}
//----------------------------------------------
//
//----------------------------------------------
template <class T>
inline void MMalloc<T>::Free()
{
	if (!ptr)
		return;
	if (hHeap)
	{
#ifdef _ALLOC_GLOBAL__
	gfree(ptr);
#else
		m_Free(hHeap, 0, ptr);
#endif
	}
	else
		Detach(GetProcessHeap());
	RealSize = Len = 0;
	ptr = 0L;

}
template <class T>
inline void MMalloc<T>::Free(T *p)
{
	if (hHeap)
		m_Free(hHeap, 0, p);

}

//----------------------------------------------
//
//----------------------------------------------
template <class T>
inline void MMalloc<T>::Move(MMalloc<T> &m)
{
	Free();
	ptr = m.ptr;
	Len = m.Len;
	RealSize = m.RealSize;
	m.ptr = 0;
	m.Len = 0;
	m.RealSize = 0;
}
//----------------------------------------------
//
//----------------------------------------------
template <class T>
inline void MMalloc<T>::Swap(MMalloc<T> &m)
{
	T *p = ptr;
	DINT len = Len;
	DINT rz = RealSize;
	ptr = m.ptr;
	Len = m.Len;
	RealSize = m.RealSize;
	m.ptr = p;
	m.Len = len;
	m.RealSize = rz;
}
//*******************************************************
//
//*******************************************************
template <class T>
class  MBuffer: protected MMalloc<T> {
public :
	MBuffer() {};

	MBuffer(DINT i) { MMalloc<T>::Malloc(i); }
	MBuffer(const MBuffer &m):MMalloc<T>(m) { }
	~MBuffer() {};
        char IsValid() { return MMalloc<T>::ptr != 0 || !MMalloc<T>::Len; }
	DINT GetLen() const { return MMalloc<T>::Len; }
	BOOL Compact(){ return MMalloc<T>::Compact(); }

	DINT GetRealSize() { return MMalloc<T>::RealSize; }
	void SetLen(DINT l) {	if (MMalloc<T>::Len != l)  { if (MMalloc<T>::RealSize >= l) MMalloc<T>::Len = l; else MMalloc<T>::ReAlloc(l); } }
        BOOL IsEmpty() { return !MMalloc<T>::Len; }
	T *GetPtr() const { return MMalloc<T>::ptr; }
	void Remove(DINT pos, DINT num);
	void MoveTo(T **p, DINT &len) { *p = MMalloc<T>::ptr; len = MMalloc<T>::Len; MMalloc<T>::RealSize = 0;  MMalloc<T>::Len = 0; MMalloc<T>::ptr = 0L; }
	void Init(T *p, DINT len) { MMalloc<T>::ptr = p; MMalloc<T>::Len = len; MMalloc<T>::RealSize = MMalloc<T>::Len;  }
	void Move(MBuffer &b) { MMalloc<T>::Move(b); }
	void Swap(MBuffer &b) { MMalloc<T>::Swap(b); }
	BOOL Insert(DINT pos, const T &src);
	BOOL Append(T *src, DINT num);
	BOOL Append(MBuffer &src) { DINT i = MMalloc<T>::Len; BOOL ret = MMalloc<T>::ReAlloc(MMalloc<T>::Len + src.Len); if (ret) MMalloc<T>::MemCpy(MMalloc<T>::ptr + i, src.ptr, src.Len); return ret; }

	BOOL operator == (const MBuffer<T> &a);
	BOOL operator != (const MBuffer<T> &a) { return !(*this == a); }

	void QSort(int (__cdecl *)(const void *, const void *));
	BOOL Expand(DINT pos, DINT num) { return MMalloc<T>::ExpandBuff(pos, num); }
	T &operator [] (DINT i)  const { return MMalloc<T>::ptr[i]; }
	void Set(T c) { MMalloc<T>::Set(c); }


};
//----------------------------------------------
//
//----------------------------------------------
template <class T>
inline BOOL MBuffer<T>::Append(T *buff, DINT len)
{
	if (!len)
		return TRUE;
	else
	{
		DINT pos = MMalloc<T>::Len;
		if (!MMalloc<T>::ReAlloc(MMalloc<T>::Len + len))
			return FALSE;
		MMalloc<T>::MemCpy(MMalloc<T>::ptr + pos, buff, len);
	}
	return TRUE;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T>
inline BOOL MBuffer<T>::Insert(DINT pos, const T &src)
{
	if (MMalloc<T>::Len <= pos)
	{
		pos = MMalloc<T>::Len;
		if (!MMalloc<T>::ReAlloc(MMalloc<T>::Len + 1))
			return FALSE;
	}
	else
		MMalloc<T>::ExpandBuff(pos, 1);
	MMalloc<T>::MemCpy(MMalloc<T>::ptr + pos, &src, 1);
	return TRUE;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T>
inline void MBuffer<T>::QSort(int (__cdecl *func)(const void *, const void *))
{
	if (MMalloc<T>::Len > 1) qsort(MMalloc<T>::ptr, MMalloc<T>::Len, sizeof(T), func);
}
//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T>
inline void MBuffer<T>::Remove(DINT pos, DINT num)
{
	if (MMalloc<T>::Len <= pos || !MMalloc<T>::Len)
		return;
	if (MMalloc<T>::Len  > pos + num)
	{
		MMalloc<T>::MemCpy(MMalloc<T>::ptr + pos, MMalloc<T>::ptr + pos + num, MMalloc<T>::Len - pos - num);
		MMalloc<T>::ReAlloc(MMalloc<T>::Len - num);
	}
	else
	{
		if (!pos)
			MMalloc<T>::Free();
		else
			MMalloc<T>::ReAlloc(pos);
	}
}
//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T>
inline BOOL MBuffer<T>::operator == (const MBuffer<T> &a)
{
	if (MMalloc<T>::Len == a.Len)
	{
		if (MMalloc<T>::Len)
			return !memcmp(MMalloc<T>::ptr, a.ptr, MMalloc<T>::Len);
		else
			return TRUE;
	}
	return FALSE;
}

//****************************************************
//
//****************************************************
template <class T>
class  __Vector : protected MMalloc<T> {
public:
	__Vector() { };
	__Vector(DINT i) { ReAlloc(i); };
	__Vector(const __Vector &c);
	~__Vector() { Free(); }
	T *GetPtr() { return MMalloc<T>::ptr; }
    char IsValid() { return MMalloc<T>::ptr != 0 || !MMalloc<T>::Len; }
	DINT GetLen() const { return MMalloc<T>::Len; }
	BOOL Compact(){ return MMalloc<T>::Compact(); }

	void SetLen(DINT l) { ReAlloc(l); }
    BOOL IsEmpty() const { return MMalloc<T>::ptr ? 0 : 1; }
    T &Add() { DINT i = MMalloc<T>::Len; ReAlloc(i + 1); return MMalloc<T>::ptr[i]; }
	BOOL Add(const T &t) { DINT i = MMalloc<T>::Len; BOOL ret = ReAlloc(i + 1); if (ret) MMalloc<T>::ptr[i] = t; return ret; }
    void Swap(__Vector &m) {MMalloc<T>::Swap(m); }
    BOOL Expand(DINT pos, DINT len);
	BOOL Append(T *src, DINT num) { DINT i = MMalloc<T>::Len; BOOL ret = ReAlloc(MMalloc<T>::Len + num); if (ret) MemCpy(MMalloc<T>::ptr + i, src, num); return ret; }
	BOOL Append(__Vector &src) { DINT i = MMalloc<T>::Len; BOOL ret = ReAlloc(MMalloc<T>::Len + src.Len); if (ret) MemCpy(MMalloc<T>::ptr + i, src.ptr, src.Len); return ret; }
	BOOL Copy(T *buff, DINT len);
	void MemCpy(T *dst, T *src, DINT len);
	void MemMove(T *dst, T *src, DINT len);
	BOOL Remove(DINT from, DINT num);
	void Move(__Vector<T> &m);
	BOOL Insert(DINT pos, T *src, DINT num);
	BOOL operator == (const __Vector &a);
	BOOL operator != (const __Vector &a) { return !(*this == a); }
        T &operator [] (DINT i) const  { return MMalloc<T>::ptr[i]; }
	__Vector &operator = (const __Vector &c) { SetLen(c.Len); Copy(c.ptr, c.Len); return *this; }
	void Set(T c);
	void Clear() { Free(); } //tmp
	void QSort(int (__cdecl *)(const void *, const void *));


	BOOL Insert(DINT pos, const T &src);

  private:
	void HelpSort(DINT left, DINT right, int (__cdecl *)(const void *, const void *));
protected:
	void Free() { Delete(0, MMalloc<T>::Len); MMalloc<T>::Free(); };
	BOOL ReAlloc(DINT len);
	void New(DINT from, DINT num);
	void Delete(DINT from, DINT num);
};
//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T>
inline __Vector<T>::__Vector(const __Vector<T> &c)
{
	DINT len = c.Len;
	if (len)
	if (MMalloc<T>::Malloc(len))
	{
		T *p = MMalloc<T>::ptr;
		T *p1 = c.ptr;
		while(len--)
			new (p++)T(*(p1++));

	}
}

template <class T>
inline void __Vector<T>::Set(T c)
{
	DINT i = MMalloc<T>::Len;
	T *p = MMalloc<T>::ptr;
	while (i--)
		*p++ = c;
}
template <class T>
inline BOOL __Vector<T>::operator == (const __Vector<T> &a)
{
	if (MMalloc<T>::Len == a.Len)
	{
		DINT i = MMalloc<T>::Len;
		T *p = a.ptr, *p1 = MMalloc<T>::ptr;
		while (i--)
			if (*(p1++) != *(p++))
				return FALSE;
		return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
template<class T>
inline void __Vector<T>::Move(__Vector<T> &m)
{
	Free();
	MMalloc<T>::ptr = m.ptr;
	MMalloc<T>::Len = m.Len;

	MMalloc<T>::RealSize = m.RealSize;
	m.ptr = 0;
	m.Len = 0;
	m.RealSize = 0;
}
template <class T>
inline BOOL __Vector<T>::Insert(DINT pos, const T &src)
{
	if (MMalloc<T>::Len <= pos)
		Add(src);
	else
	{
		Expand(pos, 1);
		MMalloc<T>::ptr[pos] = src;
	}
	return TRUE;

}

//-----------------------------------------------------
//
//-----------------------------------------------------
template<class T>
BOOL __Vector<T>::Remove(DINT pos, DINT num)
{
	if (MMalloc<T>::Len <= pos || !MMalloc<T>::Len)
		return FALSE;
	if (MMalloc<T>::Len  > pos + num)
	{
		Delete(pos, num);
		MemMove(MMalloc<T>::ptr + pos, MMalloc<T>::ptr + pos + num, MMalloc<T>::Len - pos - num);
		MMalloc<T>::ReAlloc(MMalloc<T>::Len - num);
	}
	else
	{
		if (!pos)
			Free();
		else
			ReAlloc(pos);
	}
	return TRUE;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
template<class T>
inline void __Vector<T>::New(DINT from, DINT num)
{
	T *p = MMalloc<T>::ptr + from;
	while(num--)
		new (p++) T ();
}
//-----------------------------------------------------
//
//-----------------------------------------------------
template<class T>
inline void __Vector<T>::Delete(DINT from, DINT num)
{
	T *p = MMalloc<T>::ptr + from;
	while(num--)
		(p++)->~T();

}

//-----------------------------------------------------
//
//-----------------------------------------------------
template<class T>
BOOL __Vector<T>::ReAlloc(DINT len)
{
	if (MMalloc<T>::Len == len) return TRUE;
	if (MMalloc<T>::Len > len)
	{
		Delete(len, MMalloc<T>::Len - len);
		return MMalloc<T>::ReAlloc(len);
	}
	if (len > MMalloc<T>::Len)
	{
		DINT cur = MMalloc<T>::Len;
		if (MMalloc<T>::ReAlloc(len))
		{
			New(cur, MMalloc<T>::Len - cur);
			return TRUE;
		}
	}
	return FALSE;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
template<class T>
inline BOOL __Vector<T>::Expand(DINT pos, DINT len)
{
	if (MMalloc<T>::ExpandBuff(pos, len))
	{
		New(pos, len);
		return TRUE;
	}
	return FALSE;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
template<class T>
BOOL __Vector<T>::Copy(T *buff, DINT len)
{
	if (len > MMalloc<T>::Len)
	{
		DINT num = MMalloc<T>::Len - len;
		DINT oldlen = MMalloc<T>::Len;
		if (ReAlloc(len))
		{
			T *p = MMalloc<T>::ptr;
			while (oldlen--)
				*p++ = *buff++;

			while(num--)
				new (p++)T(*buff++);
			return TRUE;
		}
	}
	else
		if (ReAlloc(len))
		{
			T *p = MMalloc<T>::ptr;
			while (len--)
				*p++ = *buff++;
			return TRUE;
		}
	return FALSE;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
template<class T>
inline void __Vector<T>::MemCpy(T *dst, T *src, DINT len)
{
	if (!src || !dst)
		return;
	if (len > MMalloc<T>::Len)
		len = MMalloc<T>::Len;

	BoolWarning(dst != 0);

	while (len--)
		*dst++ = *src++;
}
template<class T>
inline void __Vector<T>::MemMove(T *dst, T *src, DINT len)
{

	MMalloc<T>::MemCpy(dst, src, len);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
template<class T>
void __Vector<T>::HelpSort(DINT left, DINT right, int (__cdecl *func)(const void *, const void *))
{
	DINT i, j;
	i = left;
	j = right;
	T Data = MMalloc<T>::ptr[(i + j)/2];

	do
	{
		while(func(MMalloc<T>::ptr + i, &Data) < 0 && i <= right)
			++i;
		while(j >= 0 && func(&Data, MMalloc<T>::ptr + j) < 0 && j >= left)
			  --j;
		if (i <= j)
		{
			if (i < j)
			{
				if (func(MMalloc<T>::ptr + i, MMalloc<T>::ptr + j) != 0)
				{
					T a = MMalloc<T>::ptr[i];
					MMalloc<T>::ptr[i] = MMalloc<T>::ptr[j];
					MMalloc<T>::ptr[j] = a;
				}
			}
			++i;
			--j;
		}
	} while (i <= j);

	if ( left < j)
		HelpSort(left, j, func);
	if ( i  < right)
		HelpSort(i, right, func);
}
//-----------------------------------------------------
//
//-----------------------------------------------------
template<class T>
inline void __Vector<T>::QSort(int (__cdecl *func)(const void *, const void *))
{
	if (!MMalloc<T>::Len)
		return;
	HelpSort(0, MMalloc<T>::Len - 1, func);

}
template <class T, class K>
class  _ARRAY: public K {
public :
	_ARRAY() {};
	_ARRAY(DINT i):K(i) { }
	_ARRAY(const _ARRAY &m):K(m) { }
	~_ARRAY() {};
	void SetAddedRealSize(int dw) { K::SetAddedRealSize(dw); }
	void Clear() { K::Free(); }
	BOOL Copy(const T *src, DINT num);
#ifndef _ALLOC_GLOBAL__
	void Attach(T *src, DINT len, bool fKill = false) { K::Free(); K::ptr = src; K::RealSize = K::Len = len; if (!fKill) K::hHeap = 0; }
	void Detach(HANDLE _hHeap = GetProcessHeap()) { K::ptr = 0; K::RealSize = K::Len = 0; K::hHeap = _hHeap; } //{ K::Detach(_hHeap); }
#else
	void Attach(T *src, DINT len, bool fKill = false) { K::Free(); K::ptr = src; MMalloc<T>::RealSize = K::Len = len; }
	void Detach() { K::ptr = 0; K::RealSize = K::Len = 0; }
#endif
	BOOL empty() { return K::Len == 0; }
	T &back() {return K::ptr[K::Len - 1]; }
	void clear() { Clear(); }
	size_t size() { return K::GetLen(); }
	T *data() { return K::ptr; }
	BOOL push_back(const T &t) {return Add(t); }
	BOOL CopyTo(T *src, DINT num);
	BOOL CopyTo(DINT from, T *dst, DINT num);
	BOOL Replace(DINT pos, _ARRAY &a) {return Replace(pos, a.ptr, a.Len); }
	BOOL Replace(DINT pos, const T *src, DINT num);
	BOOL Insert(DINT pos, const T *src, DINT num);
	BOOL Insert(DINT pos, _ARRAY &src) { return Insert(pos, src.ptr, src.Len); }
	BOOL Insert(DINT pos, const T &src)  { return K::Insert(pos, src); }
	T &Add() { DINT i = K::Len; K::SetLen(i + 1); return K::ptr[i]; }
	BOOL Add(const T &src);
	_ARRAY &operator = (const _ARRAY &b) { K::SetLen(b.Len);  Copy(b.ptr, b.Len); return *this; }
	_ARRAY &operator += (const _ARRAY &b);
	_ARRAY &operator += (const T &b) { Add(b); return *this; }
	T &operator [] (DINT i) const { return K::ptr[i]; }
	void Tail(_ARRAY &tail, DINT &i);
	void Left(_ARRAY &left, DINT &i);
	void Right(_ARRAY &left, DINT &i);
	void Mid(_ARRAY &mid, DINT first, DINT count);
	void Swap(_ARRAY &a) {K::Swap(a); }
	void Swap();
	BOOL Swap(DINT i, DINT j);
	BOOL Move2End(DINT i);
	void RemoveObj(DINT pos, const T &t);
	DINT Find(const T &t, DINT offset = 0, BOOL (__cdecl *)(const void *, const void *) = 0);
	DINT BFind(const T &t, int (__cdecl *)(const void *, const void *));
	DINT BInsert(const T &t, int (__cdecl *)(const void *, const void *));
	void Remove (const T &t);
	void Remove(DINT pos, DINT num) { K::Remove(pos, num); }


	BOOL LeftSwapTail(DINT i);
	BOOL ReOrder (DINT from, DINT to);
#ifdef _VER_PLK_64
	void SortIndex(MBuffer<int> &index, int (__cdecl *func)(const void *, const void *));
#endif
	void SortIndex(MBuffer<DINT> &index, int (__cdecl *func)(const void *, const void *));
	void SortBHeap(int (__cdecl *func)(const void *, const void *));
	void InsertBHeap(const T &t, int (__cdecl *func)(const void *, const void *));
	T ExtractBHeap(int (__cdecl *func)(const void *, const void *));
#ifdef __HEAP_USER_ALLOC_
	void InitMemMananger(f_Alloc a, f_ReAlloc r, f_Free f)
	{
		m_Free = f;
		m_ReAlloc = r;
		m_Alloc = a;
	}
#endif
private:
	void helpSort(DINT *item, DINT left, DINT right, int (__cdecl *func)(const void *, const void *));
#ifdef _VER_PLK_64
	void helpSort(int *item, int left, int right, int (__cdecl *func)(const void *, const void *));
#endif
	void ReMakeHeap(DINT i, int (__cdecl *func)(const void *, const void *));

};
//---------------------------------------
//
//---------------------------------------
template <class T, class K>
inline T _ARRAY<T,K>::ExtractBHeap(int (__cdecl *func)(const void *, const void *))
{
	T tmp;
	if (K::Len)
	{
		if (--K::Len)
		{
			tmp = *K::ptr;
			*K::ptr = K::ptr[K::Len - 1];
			K::SetLen(K::Len - 1);
			ReMakeHeap(0, func);
		}
		else
			return *K::ptr;
	}
	return tmp;
}
//---------------------------------------
//
//---------------------------------------
template <class T, class K>
inline void _ARRAY<T,K>::InsertBHeap(const T &t, int (__cdecl *func)(const void *, const void *))
{
	DINT l = K::Len;
	if (K::RealSize > K::Len + 1)
		++K::Len;
	else
		if (!K::ReAlloc(K::Len + 1)) return;

	DINT iParent = l >> 1;
	while (l && func(&t, (K::ptr + iParent)) < 0)
	{
		*(K::ptr + l) = *(K::ptr + iParent);
		l = iParent;
		*(K::ptr + l) = t;
		iParent >>= 1;

	}
	*(K::ptr + l) = t;
}

//---------------------------------------
//
//---------------------------------------
template <class T, class K>
inline void _ARRAY<T,K>::SortBHeap(int (__cdecl *func)(const void *, const void *))
{

	if (K::Len > 1)
	{
		DINT j = K::Len >> 1;
		do
			ReMakeHeap(j, func);
		while(j--);
	}
}

template <class T, class K>
void _ARRAY<T,K>::ReMakeHeap(DINT i, int (__cdecl *func)(const void *, const void *))
{
	if (K::Len)
	{
		T tmp;
		DINT mini, l, r;
		while(true)
		{
			l =  i << 1;
			r =  l + 1;
			if (l < K::Len  && func(K::ptr + l, K::ptr + i) < 0)
				mini = l;
			else
				mini = i;
			if (r < K::Len && func(K::ptr + r, K::ptr + mini) < 0)
				mini = r;
			if (mini != i)
			{
				tmp = *(K::ptr + i);
				*(K::ptr + i) = *(K::ptr + mini);
				i = mini;
				*(K::ptr + i) = tmp;
			}
			else
				break;
		}
	}
}


//------------------------------------------
//
//------------------------------------------
template <class T, class K>
inline void _ARRAY<T,K>::Remove (const T &t)
{
	RemoveObj(0, t);
}
//------------------------------------------
//
//------------------------------------------
template <class T, class K>
void _ARRAY<T,K>::SortIndex(MBuffer<DINT> &index, int (__cdecl *func)(const void *, const void *))
{
	index.SetLen(K::Len);
	DINT *p = index.GetPtr();
	for (DINT i = 0; i < K::Len; ++i)
		*p++ = i;

	if (K::Len > 2)
		helpSort(index.GetPtr(), 0, K::Len - 1, func);
}
#ifdef _VER_PLK_64
template <class T, class K>
void _ARRAY<T,K>::SortIndex(MBuffer<int> &index, int (__cdecl *func)(const void *, const void *))
{
	index.SetLen(K::Len);
	int *p = index.GetPtr();
	for (int i = 0; i < (int)K::Len; ++i)
		*p++ = i;

	if (K::Len > 2)
		helpSort(index.GetPtr(), 0, (int)K::Len - 1, func);
}
#endif
//------------------------------------------
//
//------------------------------------------
template <class T, class K>
void _ARRAY<T,K>::helpSort(DINT *item, DINT left, DINT right, int (__cdecl *func)(const void *, const void *))
{
DINT i, j, y;
	i = left;
	j = right;
	T *Data = K::ptr + item[(i + j)/ 2];


	do
	{
		while(func(K::ptr + item[i], Data) < 0 && i <= right)
			++i;
		while(j >= 0 && func(Data, K::ptr + item[j]) < 0 && j >= left)
			  --j;
		if (i <= j)
		{
			if (i < j)
			{
				y = item[i];
				item[i] = item[j];
				item[j] = y;
			}
			++i;
			--j;
		}
	} while (i <= j);

	if ( left < j)
		helpSort(item, left, j, func);
	if ( i  < right)
		helpSort(item, i, right, func);
}
#ifdef _VER_PLK_64
template <class T, class K>
void _ARRAY<T,K>::helpSort(int *item, int left, int right, int (__cdecl *func)(const void *, const void *))
{
int i, j, y;
	i = left;
	j = right;
	T *Data = K::ptr + item[(i + j)/ 2];


	do
	{
		while(func(K::ptr + item[i], Data) < 0 && i <= right)
			++i;
		while(j >= 0 && func(Data, K::ptr + item[j]) < 0 && j >= left)
			  --j;
		if (i <= j)
		{
			if (i < j)
			{
				y = item[i];
				item[i] = item[j];
				item[j] = y;
			}
			++i;
			--j;
		}
	} while (i <= j);

	if ( left < j)
		helpSort(item, left, j, func);
	if ( i  < right)
		helpSort(item, i, right, func);
}
#endif
//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T, class K>
inline void _ARRAY<T,K>::RemoveObj(DINT pos, const T &t)
{
	for(DINT i = pos; i < K::Len; ++i)
		if (*(K::ptr + i) == t)
		{
			Remove(i, 1);
			--i;
		}
}

template <class T, class K>
inline BOOL _ARRAY<T,K>::Add(const T &src)
{
	DINT len = K::Len;
	if (!K::ReAlloc(K::Len + 1))
		return FALSE;
	K::ptr[len] = src;
	return TRUE;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T, class K>
DINT _ARRAY<T,K>::BInsert(const T &key, int (__cdecl *func)(const void *, const void *))
{
	DINT up = K::Len - 1;
	DINT down = 0;
	BOOL dir;
	if (!K::Len)
	{
		Add(key);
		return 0;
	}
	if (K::Len == 1)
	{
		dir = func(&key , K::ptr + up) != -1;
		if (!dir)
			Insert(0, key);
		else
			Add(key);
		return dir;
	}

	dir = func(K::ptr, K::ptr + up) == 1;
 	if (!dir)
	{
		down = up;
		up = 0L;
	}
	if (func(&key , K::ptr + up) == -1)
	{
		if (dir)
			Add(key);
		else
			Insert(up, key);
		return up;
	}

	if (func(&key , K::ptr + down) == 1)
	{
		if (dir)
			Insert(down, key);
		else
			Add(key);
		return K::Len - 1;
	}
	while (DINTABS((DINT)(up - down)) > 1)
	{
		DINT l = (up + down) / 2;
		if (func(&key , K::ptr + l) == -1)
			down = l;
		else
			up = l;
	}
   	if (dir)
	{
            Insert(down + 1, key);
            return down + 1L;
	}
        Insert(down, key);
        return down;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T, class K>
inline DINT _ARRAY<T,K>::Find(const T &t, DINT offset, BOOL (__cdecl *func)(const void *, const void *))
{
	if (offset >= K::Len)
		return -1;
	for (DINT i = offset; i < K::Len; ++i)
		 if ((func && func(&t, K::ptr + i)) || *(K::ptr + i) == t)
				return i;

	return -1;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T, class K>
inline DINT _ARRAY<T,K>::BFind(const T &key, int (__cdecl *func)(const void *, const void *))
{

	T *t = 0;
	if (K::ptr)
	{
		if (K::Len == 1)
		{
			if (!func(&key, K::ptr))
				return 0;
			else
				return -1;
		}
		t = (T *)bsearch(&key, K::ptr, K::Len, sizeof(T), func);
	}
	return !t ? -1 : (DINT)(t - K::ptr);

}

template <class T, class K>
inline void _ARRAY<T,K>::Swap()
{
	ReOrder (0, K::Len - 1);
}
template <class T, class K>
inline BOOL _ARRAY<T,K>::ReOrder (DINT from, DINT to)
{
	T t;
	if (!K::Len) return 0;
        DINT mid = (to + from)/2;
	for (DINT i = from; i <= mid; ++i)
	{
		t = K::ptr[i];
		K::MemCpy(K::ptr + i,  K::ptr + to - i + from, 1);
		K::ptr[to - i + from] = t;
	}
	return 1;
}
template <class T, class K>
inline BOOL _ARRAY<T,K>::Swap(DINT i, DINT j)
{
	if (i < 0 || !K::Len || K::Len <= i ||
		j < 0 ||  K::Len <= j)
		return FALSE;
	T a = K::ptr[i];
	K::ptr[i] = K::ptr[j];
	K::ptr[j] = a;
	return TRUE;
}
template <class T, class K>
inline BOOL _ARRAY<T,K>::Move2End(DINT i)
{
	if (i < 0 || !K::Len || K::Len <= i)
		return FALSE;
	if (i != K::Len - 1)
	{
		T a = K::ptr[i];
		K::MemCpy(K::ptr + i, K::ptr + i + 1, K::Len - i - 1);
		K::ptr[K::Len - 1] = a;
	}
	return TRUE;
}

template <class T, class K>
inline void _ARRAY<T,K>::Mid(_ARRAY<T,K> &mid, DINT first, DINT count)
{
	BoolWarning (first >= 0 && count > 0);
	if (first < 0 || !K::Len || K::Len <= first  || count <= 0)
		return;
	if (count + first > K::Len)
		count = K::Len - first;
	mid.SetLen(count);
	CopyTo(first, mid.ptr, count);

}

//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T, class K>
inline BOOL _ARRAY<T,K>::LeftSwapTail(DINT i)
{
	if (!K::Len || K::Len <= i)
		return FALSE;
	_ARRAY<T,K> left;
	Left(left, i);
	K::MemCpy(K::ptr, K::ptr + i, K::Len - i);
	K::MemCpy(K::ptr + K::Len - i, left.ptr, left.Len);
	return TRUE;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T, class K>
inline void _ARRAY<T,K>::Left(_ARRAY<T,K> &left, DINT &i)
{
	BoolWarning (i >= 0);
	if (i < 0 || !K::Len || K::Len < i)
	{
		left.SetLen(K::Len);
		if (K::Len)
			CopyTo(left.ptr, K::Len);
		i = K::Len;
	}
	else
	{
		left.SetLen(i);
		CopyTo(left.ptr, i);
	}
}
//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T, class K>
inline void _ARRAY<T,K>::Right(_ARRAY<T,K> &right, DINT &i)
{
	

	if (!K::Len || K::Len < i)
	{
		right.SetLen(K::Len);
		if (K::Len)
			CopyTo(right.ptr, K::Len);
		i = K::Len;
	}
	else
	{
		right.SetLen(i);
		K::MemCpy(right.ptr, K::ptr + K::Len - i, i);
	}
}

//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T, class K>
inline void _ARRAY<T,K>::Tail(_ARRAY<T,K> &tail, DINT &i)
{
	if (!K::Len || K::Len < i)
	{
		tail.SetLen(K::Len);
		CopyTo(tail.ptr, K::Len);
		i = K::Len;
	}
	else
	{
		tail.SetLen(K::Len - i);
		CopyTo(i, tail.ptr, K::Len - i);
	}
}
//-------------------------------------------------------------
//
//-------------------------------------------------------------

template <class T, class K>
inline BOOL _ARRAY<T,K>::Copy(const T *src, DINT num)
{
	
	if (!src)
		return FALSE;
	if (!num)
		K::Free();
	else
	{
		K::ReAlloc(num);
		K::MemCpy(K::ptr, (T *)src, num);
	}
	return TRUE;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T, class K>
inline BOOL _ARRAY<T,K>::CopyTo(T *dst, DINT num)
{

	if (!num || !K::ptr)
		return FALSE;
	if (num > K::Len)
		num = K::Len;
	K::MemCpy(dst, K::ptr, num);
	return TRUE;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T, class K>
inline BOOL _ARRAY<T,K>::CopyTo(DINT from, T *src, DINT num)
{
	if (!num || !K::ptr || from > K::Len)
		return FALSE;
	if (num + from > K::Len)
		num = num - from;
	K::MemCpy(src, K::ptr + from, num);
	return TRUE;

}
//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T, class K>
inline BOOL _ARRAY<T,K>::Replace(DINT pos, const T *src, DINT num)
{
	BoolWarning (pos >= 0 && num >= 0);
	if (pos < 0 || num <= 0 || !src)
		return FALSE;
	if (K::Len < pos + num)
		K::ReAlloc(pos + num);
	K::MemCpy(K::ptr + pos, (T*)src, num);
	return TRUE;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T, class K>
inline BOOL _ARRAY<T,K>::Insert(DINT pos, const T *src, DINT num)
{
	BoolWarning (pos >= 0 && num >= 0);

	if (!num || !src)
		return FALSE;
	K::Expand(pos, num);
	K::MemCpy(K::ptr + pos, (T *)src, num);
	return TRUE;
}
//-----------------------------------------------------
//
//-----------------------------------------------------
template <class T, class K>
inline _ARRAY<T,K> &_ARRAY<T,K>::operator += (const _ARRAY<T,K> &b)
{
	DINT len = K::Len;
	if (K::ReAlloc(b.Len + K::Len))
		K::MemCpy(K::ptr + len, b.ptr, b.Len);
	return *this;
}
//****************************************************
//
//****************************************************
template <class T>
class  MArray: public _ARRAY<T, MBuffer<T> > {
public:
	MArray() {};
//	void FreeInRealloc(bool fFree) { 	_ARRAY<T, MBuffer<T> >::FreeInRealloc(fFree); }
	MArray(DINT i):_ARRAY<T, MBuffer<T> > (i){} 
	MArray(DINT i, const T &t):_ARRAY<T, MBuffer<T> > (i){ _ARRAY<T, MBuffer<T> >::Set(t); } 
	MArray(const MArray &m): _ARRAY<T, MBuffer<T> > (m) { }
	~MArray() {};

	MArray &operator = (const  MArray &b) { _ARRAY<T, MBuffer<T> >::operator=(b); return *this; }
	BOOL operator == (const  MArray &b) { return _ARRAY<T, MBuffer<T> >::operator == (b);  }
	BOOL operator != (const  MArray &b) { return _ARRAY<T, MBuffer<T> >::operator != (b);  }

	MArray &operator += (const MArray &b) { _ARRAY<T, MBuffer<T> >::operator+=(b); return *this; }
	MArray &operator += (const T &b) { Add(b); return *this; }
	MArray Tail(DINT i) {MArray tail; _ARRAY<T, MBuffer<T> >::Tail(tail, i); return tail; }
	MArray Left(DINT i) {MArray left; _ARRAY<T, MBuffer<T> >::Left(left, i); return left; }
	MArray Right(DINT i) {MArray right; _ARRAY<T, MBuffer<T> >::Right(right, i); return right; }
	MArray Mid(DINT first, DINT count) {MArray mid; _ARRAY<T, MBuffer<T> >::Mid(mid, first, count); return mid; }

};
class  MArrayByte: public MArray<BYTE> {
public:
	MArrayByte() {};
	MArrayByte(DINT i):MArray<BYTE> (i){}
	MArrayByte(const MArrayByte &m): MArray<BYTE> (m) { }
	~MArrayByte() {};
	MArrayByte &operator = (const  MArrayByte &b) { _ARRAY<BYTE, MBuffer<BYTE> >::operator=(b); return *this; }
	BOOL operator == (const  MArrayByte &b) { return _ARRAY<BYTE, MBuffer<BYTE> >::operator == (b);  }
	BOOL operator != (const  MArrayByte &b) { return _ARRAY<BYTE, MBuffer<BYTE> >::operator != (b);  }

	MArrayByte &operator += (const MArrayByte &b) { _ARRAY<BYTE, MBuffer<BYTE> >::operator+=(b); return *this; }
	MArrayByte &operator += (BYTE &b) { Add(b); return *this; }
	MArrayByte Tail(DINT i) {MArrayByte tail; _ARRAY<BYTE, MBuffer<BYTE> >::Tail(tail, i); return tail; }
	MArrayByte Left(DINT i) {MArrayByte left; _ARRAY<BYTE, MBuffer<BYTE> >::Left(left, i); return left; }
	MArrayByte Right(DINT i) {MArrayByte right; _ARRAY<BYTE, MBuffer<BYTE> >::Right(right, i); return right; }
	MArrayByte Mid(DINT first, DINT count) {MArrayByte mid; _ARRAY<BYTE, MBuffer<BYTE> >::Mid(mid, first, count); return mid; }
  //Binary operations
	MArrayByte &operator &= (MArrayByte &a);
	MArrayByte &operator |= (MArrayByte &a);
	MArrayByte &operator ^= (MArrayByte &a);
	MArrayByte &operator -= (MArrayByte &a);
	void Not();
};
inline MArrayByte &MArrayByte::operator &= (MArrayByte &a)
{
	DINT len = min(Len, a.Len);
	SetLen(len);
	BYTE *ptra = a.ptr;
	BYTE *ptrb = ptr;
	while (len--)
		*(ptrb++) &= *(ptra++);
	return *this;
}
inline void MArrayByte::Not()
{
	DINT len = Len;
	BYTE *ptrb = ptr;
	while (len--)
	{
		*(ptrb) = ~*(ptrb);
		++ptrb;
	}

}

inline MArrayByte &MArrayByte::operator |= (MArrayByte &a)
{
	DINT len = min(Len, a.Len);
	BYTE *ptra = a.ptr;
	BYTE *ptrb = ptr;
	while (len--)
		*(ptrb++) |= *(ptra++);
	return *this;
}
inline MArrayByte &MArrayByte::operator ^= (MArrayByte &a)
{
	DINT len = min(Len, a.Len);
	SetLen(len);
	BYTE *ptra = a.ptr;
	BYTE *ptrb = ptr;
	while (len--)
		*(ptrb++) ^= *(ptra++);
	return *this;
}
inline MArrayByte &MArrayByte::operator -= (MArrayByte &a)
{
	DINT len = min(Len, a.Len);
	SetLen(len);
	BYTE *ptra = a.ptr;
	BYTE *ptrb = ptr;
	while (len--)
	{
		*ptrb= *ptra == *ptrb ? 0 : *ptrb;
		ptra++;
		ptrb++;
	}
	return *this;
}

inline MArrayByte operator & (MArrayByte &a, MArrayByte &b)
{
	MArrayByte res;
	DINT len = min(b.GetLen(), a.GetLen());
	res.SetLen(len);
	BYTE *ptr = res.GetPtr();
	BYTE *ptra = a.GetPtr();
	BYTE *ptrb = b.GetPtr();
	while (len--)
		*(ptr++) = *(ptrb++) & *(ptra++);
	return res;

}

inline MArrayByte operator | (MArrayByte &a, MArrayByte &b)
{
	MArrayByte res;
	DINT len = min(b.GetLen(), a.GetLen());
	res.SetLen(len);
	if (b.GetLen() > a.GetLen())
		res.Append(b.GetPtr() + len, b.GetLen() - len);
	else
	if (a.GetLen() > b.GetLen())
		res.Append(a.GetPtr() + len, a.GetLen() - len);

	BYTE *ptr = res.GetPtr();
	BYTE *ptra = a.GetPtr();
	BYTE *ptrb = b.GetPtr();
	while (len--)
		*(ptr++) = *(ptrb++) & *(ptra++);
 	return res;
}
inline MArrayByte operator ^ (MArrayByte &a, MArrayByte &b)
{
	MArrayByte res;
	DINT len = min(b.GetLen(), a.GetLen());
	res.SetLen(len);
	BYTE *ptr = res.GetPtr();
	BYTE *ptra = a.GetPtr();
	BYTE *ptrb = b.GetPtr();;
	while (len--)
		*(ptr++) = *(ptrb++) ^ *(ptra++);
	return res;
}


//****************************************************
//
//****************************************************
template <class T>
class  MVector : public _ARRAY<T, __Vector<T> > {
public:
	MVector() {};
	MVector(DINT i):_ARRAY<T, __Vector<T> > (i){}
	MVector(DINT i, const T &t):_ARRAY<T, __Vector<T> > (i) { Set(t); }

	MVector(const MVector &m): _ARRAY<T, __Vector<T> > (m) { }
	~MVector() {};
	BOOL operator == (const MVector &b) { return _ARRAY<T, __Vector<T> >::operator == (b); }
	BOOL operator != (const MVector &b) { return _ARRAY<T, __Vector<T> >::operator != (b); }

	MVector &operator = (const MVector &b) { _ARRAY<T, __Vector<T> >::operator =(b); return *this; }
	MVector &operator += (const MVector &b) { _ARRAY<T, __Vector<T> >::operator+=(b); return *this; }
	MVector &operator += (const T &b) { Add(b); return *this; }
	MVector Tail(DINT i) {MVector tail; _ARRAY<T, __Vector<T> >::Tail(tail, i); return tail; }
	MVector Left(DINT i) {MVector left; _ARRAY<T, __Vector<T> >::Left(left, i); return left; }
	MVector Right(DINT i) {MVector right; _ARRAY<T, __Vector<T> >::Right(right, i); return right; }
	MVector Mid(DINT first, DINT count) {MVector mid; _ARRAY<T, __Vector<T> >::Mid(mid, first, count); return mid; }
};






#define ReorderBits(b)												\
	switch (sizeof(b))											\
	{															\
	case 1:				 										\
		b = ((b & 0x0F) << 4) | ((b & 0xF0) >> 4);				\
		b = ((b & 0x33) << 2) | ((b & 0xCC) >> 2);				\
		b = ((b & 0x55) << 1) | ((b & 0xAA) >> 1);				\
		break;													\
	case 2:														\
		b = ((b & 0x00FF) << 8) | ((b & 0xFF00) >> 8);			\
		b = ((b & 0x0F0F) << 4) | ((b & 0xF0F0) >> 4);			\
		b = ((b & 0x3333) << 2) | ((b & 0xCCCC) >> 2);			\
		b = ((b & 0x5555) << 1) | ((b & 0xAAAA) >> 1);			\
		break;													\
	case 4:														\
		b = ((b & 0x0000FFFF) << 16) | ((b & 0xFFFF0000) >> 16);\
		b = ((b & 0x00FF00FF) << 8) | ((b & 0xFF00FF00) >> 8);	\
		b = ((b & 0x0F0F0F0F) << 4) | ((b & 0xF0F0F0F0) >> 4);	\
		b = ((b & 0x33333333) << 2) | ((b & 0xCCCCCCCC) >> 2);	\
		b = ((b & 0x55555555) << 1) | ((b & 0xAAAAAAAA) >> 1);	\
		break;													\
	}

#define InverseDWORD(dword)									\
{															\
	DWORD tmp_dword = ((dword << 16) & 0xFFFF0000L) | ((dword >> 16)  & 0x0000FFFFL); \
	dword = ((tmp_dword & 0xFF00FF00L) >> 8) | ((tmp_dword & 0x00FF00FFL) << 8);	\
}															   						\

#define ReorderBytes(array, count)										\
    {																	\
        for (int i = 0; i < count; ++i)									\
		{																\
			if (sizeof(*array) == sizeof (DWORD))						\
				InverseDWORD(array[i]);									\
			else														\
			if (sizeof(*array) == sizeof (USHORT))						\
				((USHORT *)array)[i] = (((USHORT *)array)[i] << 8) |	\
					(((USHORT *)array)[i] >> 8);						\
        }																\
    }


//------------------------------------------------
//
//------------------------------------------------
inline short Mem2Short(BYTE *in)
{
	return (((USHORT)(in[1]) << 8) & 0xff00) | in[0];
}
inline USHORT Mem2UShort(BYTE *in)
{
	return Mem2Short(in);
}

//------------------------------------------------
//
//------------------------------------------------
inline void Short2Mem(short s, BYTE *b)
{
	*b = (BYTE)s;
	b[1] = (BYTE)(s >> 8);
}
//------------------------------------------------
//
//------------------------------------------------
inline int Mem2Long(BYTE *in)
{
	return *in | ((USHORT)(in[1]) << 8) | ((DWORD)(in[2]) << 16) | ((DWORD)(in[3]) << 24);
}

//------------------------------------------------
//
//------------------------------------------------
inline void Long2Mem(int i, BYTE *in)
{
	*in = (BYTE)(i & 0xff);
	in[1] = (BYTE)((i >> 8) & 0xff);
	in[2] = (BYTE)((i >> 16) & 0xff);
	in[3] = (BYTE)((i >> 24) & 0xff);
} 
//====================================================================================
//
//====================================================================================
inline int __cdecl _compare_int(const void *elem1, const void *elem2 )
{
	if (*(int *)elem1 > *(int *)elem2)
		return 1;
	else
	if (*(int *)elem1 == *(int *)elem2)
		return 0;
	else
		return -1;
} 

#define Int2Mem(a, b) Long2Mem(a, b)
#define DWord2Mem(a, b) Long2Mem((int)a, b)
#define UShort2Mem(s, b) Short2Mem((short)s, b)
#define Mem2DWord(in) Mem2Long(in)
#define Mem2Int(in) ((int)Mem2Long(in))
#define _SIGN(a) (a > 0 ? 1 : 0)
#define VINT int 

#endif
