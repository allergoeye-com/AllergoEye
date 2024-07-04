///////////////////////////////////////////////////
// mallocator.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////
#ifndef __M__ALLOCATOR__
#define __M__ALLOCATOR__

#include "buffer.h"

#define MAX_SHIFT 17
#ifndef DEF_MAX_ALLOCATOR__
#define DEF_MAX_ALLOCATOR__ 4096/sizeof(T)
#endif

template<class T>
struct Allocator : public MArray<T *> {
#define m_Free2 (HeapFree)
#define m_ReAlloc2 (HeapReAlloc)
#ifdef HeapAlloc
#undef HeapAlloc
#endif
#define m_Alloc2  (HeapAlloc)

    Allocator()
    {
        _MinAlloc = 0;
        _Max_Alloc_ = DEF_MAX_ALLOCATOR__;
        SizeOfT = sizeof(T);
        MArray<T *>::SetAddedRealSize(0x2000000);
        count = 0;
    };

    ~Allocator();

    T *Alloc(bool fErase = true);

    T *Alloc(T &);

    void FreeAll();

    void Free(T *);

    bool CheckedFree(T *);

    BOOL private_alloc();

    void Clear()
    {
        Reset();
        MArray<T *>::Clear();
    }

    bool IsFree();

    int NumBytes();

    void Reset();

    MArray<BYTE *> locked;
    int SizeOfT;
    DINT _Max_Alloc_, _MinAlloc;

    int count;


};

template<class T>
struct MNew : Allocator<T> {
    MNew()
    {};

    ~MNew()
    {};

    T *Alloc(bool f = 0)
    {
        T *p = Allocator<T>::Alloc(false);
        return new(p) T();
    }

    void Free(T *p)
    {
        p->~T();
        Allocator<T>::Free(p);
    }

    T *Alloc(T &t)
    {
        T *p = Allocator<T>::Alloc(false);
        return new(p) T(t);
    }

};

//---------------------------------------
//
//---------------------------------------
template<class T>
inline T *Allocator<T>::Alloc(bool fErase)
{

    if (!MArray<T *>::Len)
        private_alloc();

    if (MArray<T *>::Len)
    {

        --MArray<T *>::Len;
        if (fErase)
            memset(*(MArray<T *>::ptr + MArray<T *>::Len), 0, SizeOfT);
        ++count;
        return *(MArray<T *>::ptr + MArray<T *>::Len);
    }
    return 0;

}

//---------------------------------------
//
//---------------------------------------
template<class T>
inline T *Allocator<T>::Alloc(T &t)
{
    if (!MArray<T *>::Len)
        private_alloc();

    if (MArray<T *>::Len)
    {
                BoolWarning(MArray<T *>::Len > 0);
        --MArray<T *>::Len;
        **(MArray<T *>::ptr + MArray<T *>::Len) = t;
        ++count;

        return *(MArray<T *>::ptr + MArray<T *>::Len);
    }
    return 0;

}

//---------------------------------------
//
//---------------------------------------

template<class T>
inline BOOL Allocator<T>::private_alloc()
{
    DINT lckn = (int) locked.GetLen();
    DINT n = (size_t) 1 << ((lckn > MAX_SHIFT ? MAX_SHIFT : lckn) + 1);
    if (n < _MinAlloc)
        n = _MinAlloc;
#ifdef _VER_PLK_64
    if (_Max_Alloc_ != (size_t) -1 && n > _Max_Alloc_)
        n = _Max_Alloc_;
#else
        if (_Max_Alloc_ != (size_t)-1 && n > _Max_Alloc_)
            n = _Max_Alloc_;
#endif
    DINT alc = n * SizeOfT;

#ifdef _ALLOC_GLOBAL__
    BYTE *p = (BYTE *)gmalloc(alc);
#else
    BYTE *p = (BYTE *) m_Alloc2(GetProcessHeap(), HEAP_ZERO_MEMORY, alc);
#endif

    if (!p)
    {
        n = GetLastError();
        SetLastError(0);
        return 0;
    }
#ifdef USE_HeapSize
    n = HeapSize(hHeap, 0, p)/SizeOfT;
#endif
    locked.Add() = p;
            BoolWarning(count >= 0);
    DINT ln = MArray<T *>::Len;
    MArray<T *>::SetLen(n + MArray<T *>::Len);
    T **ptr1 = MArray<T *>::ptr + ln;
    while (n--)
    {
        *ptr1++ = (T *) p;
        p += SizeOfT; // SizeOfT mozhet byt' ne raven sizeof(T)
    }
    return 1;

}

template<class T>
inline bool Allocator<T>::IsFree()
{
    return !count;

}

template<class T>
inline int Allocator<T>::NumBytes()
{
    DINT ln = locked.GetLen();
    if (!ln) return 0;
    DINT len, i = 0;
    MArray<T *>::Len = 0;
    DWORD n = 0;
    while (ln--)
    {
        n = (DWORD) 1 << ((i > MAX_SHIFT ? MAX_SHIFT : i) + 1);
        if (n < _MinAlloc)
            n = _MinAlloc;
        if ((DINT) _Max_Alloc_ != -1 && n > _Max_Alloc_)
            n = _Max_Alloc_;

        len += n;
        ++i;
    }
    return len;
}

template<class T>
inline void Allocator<T>::FreeAll()
{

    DINT ln = locked.GetLen();

    if (!ln) return;
    BYTE **p = locked.GetPtr();
    DINT len, i = 0;

    MArray<T *>::Len = 0;
    DINT n = 0;
    T **ptr1;
    while (ln--)
    {
        BYTE *t = *p++;
#ifdef USE_HeapSize
        n = HeapSize(hHeap, 0, t)/SizeOfT;
#else

        n = (DINT) 1 << ((i > MAX_SHIFT ? MAX_SHIFT : i) + 1);
        if (n < _MinAlloc)
            n = _MinAlloc;
        if ((DINT) _Max_Alloc_ != -1 && n > _Max_Alloc_)
            n = _Max_Alloc_;
#endif
        len = MArray<T *>::Len;
        MArray<T *>::SetLen(MArray<T *>::Len + n);
        ptr1 = MArray<T *>::ptr + len;
        while (n--)
        {
            *ptr1++ = (T *) t;
            t += SizeOfT;
        }
        ++i;
    }
    count = 0;
}

//---------------------------------------
//
//---------------------------------------
template<class T>
inline void Allocator<T>::Reset()
{
    DINT ln = locked.GetLen();
    BYTE **p = locked.GetPtr();
    HANDLE hp = GetProcessHeap();
    while (ln--)
    {
        m_Free2(hp, 0, *p);
        ++p;
    }
    MArray<T *>::Clear();
    locked.Clear();
    count = 0;


}

//---------------------------------------
//
//---------------------------------------
template<class T>
Allocator<T>::~Allocator()
{
    Reset();
}

template<class T>
void Allocator<T>::Free(T *p)
{
/*
	if (count)
	{
	#ifdef _DEBUG
		BoolWarning(count && Len + 1 <= RealSize); //Add(p);
	#endif
		*(ptr + Len++) = p;

		--count;
	}
	*/
    if (count)
    {
        if (MArray<T *>::Len<MArray<T *>::RealSize)
            *(MArray<T *>::ptr + MArray<T *>::Len++) = p;
        else
            MArray<T *>::Add(p);
        --count;
    }
}

template<class T>
inline bool Allocator<T>::CheckedFree(T *obj)
{
    int ln = locked.GetLen();
    if (ln)
    {
        BYTE *t, **p = locked.GetPtr(), *_obj = (BYTE *) obj;
        DINT n = 0;
        int i = 0;

        while (ln--)
        {
            t = *p++;

#ifdef USE_HeapSize
            n = HeapSize(hHeap, 0, t);
#else
            n = (DWORD) 1 << ((i > MAX_SHIFT ? MAX_SHIFT : i) + 1);
            if (n < _MinAlloc)
                n = _MinAlloc;
            else
#ifdef _VER_PLK_64
            if (_Max_Alloc_ != (DINT) -1 && n > _Max_Alloc_)
                n = _Max_Alloc_;
#else
            if (_Max_Alloc_ != (DINT)-1 && n > _Max_Alloc_)
                n = _Max_Alloc_;
#endif
            n *= SizeOfT;
#endif
            if (_obj >= t && t + n > _obj)
            {
                Free(obj);
                return true;
            }
            ++i;
        }
    }
    return false;


}

#endif
