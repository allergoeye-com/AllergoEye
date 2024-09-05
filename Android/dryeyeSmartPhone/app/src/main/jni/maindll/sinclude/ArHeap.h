#ifndef __ArHeap__
#define __ArHeap__
#ifndef _PEXPORT
#define _PEXPORT 
#endif
#ifndef ANDROID_NDK
#include "buffer.h"
#else

#include "std.h"

#endif
#pragma pack (push)
#pragma pack (8)

class ArHeap : public MArray<DINT> {
public:
    ArHeap()
    { N = 0; }

    ArHeap(ArHeap &h) : MArray<DINT>(h)
    {
        N = 0;
        if (Len == h.Len)
        { N = h.N; }
        else
        { N = 0; }
    }

    ArHeap &operator=(ArHeap &h)
    {
        MArray<DINT>::Clear();
        N = 0;
        MArray<DINT>::operator=(h);
        if (Len == h.Len)
        { N = h.N; }
        else
        { N = 0; }
        return *this;
    }

    virtual ~ArHeap()
    {}

    void Swap(ArHeap &h)
    {
        MArray<DINT>::Swap(h);
        DINT n = N;
        N = h.N;
        h.N = n;
    }

    void Clear()
    {
        MArray<DINT>::Clear();
        N = 0;
    }

    DINT IsEmpty()
    { return N == 0; }

    void Reset()
    { N = 0; }

    DINT Num() const
    { return N; }

    void Push(DINT e)
    {
        if (N >= Len - 1)
            SetLen(Len ? Len * 2 : 128);
        ptr[++N] = e;
        FixUp(N);
    }

    DINT Pop()
    {
        if (!N) return -1;
        if (N == 1)
        {
            --N;
            return ptr[1];
        }
        Swap(1, N);
        --N;
        FixDown(1);
        return ptr[N + 1];
    }

protected:
    void Swap(DINT a, DINT b)
    {
        *ptr = *(ptr + a);
        *(ptr + a) = *(ptr + b);
        *(ptr + b) = *ptr;
    }

    void FixUp(DINT k)
    {
        DINT iParent = k >> 1;
        while (iParent && *(ptr + iParent) > *(ptr + k))
        {
            Swap(k, iParent);
            k = iParent;
            iParent >>= 1;
        }
    }

    void FixDown(DINT _i)
    {
        DINT mini, l;
        while ((l = _i << 1) <= N)
        {
            mini = *(ptr + l) < *(ptr + _i) ? l : _i;
            ++l;
            if (l <= N && *(ptr + l) < *(ptr + mini))
                mini = l;
            if (mini != _i)
            {
                Swap(_i, mini);
                _i = mini;
            } else
                break;
        }

    }

protected:
    DINT N;

};

#pragma pack(pop)
#endif
