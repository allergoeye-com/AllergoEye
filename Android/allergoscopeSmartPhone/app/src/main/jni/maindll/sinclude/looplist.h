#pragma once

#include "buffer.h"

#pragma pack(push) //NADA
#pragma pack(4)

struct LoopEntry;

struct DataLoopEntry {
    LoopEntry *self;

    DataLoopEntry()
    { self = 0; }
};

struct LoopEntry {
    LoopEntry *left;
    LoopEntry *right;
    DataLoopEntry *data;

    LoopEntry()
    {
        data = 0;
        left = right = this;
    };

    void Init(LoopEntry *_left, LoopEntry *_right, DataLoopEntry *_data = 0)
    {
        left = _left;
        right = _right;
        data = _data;
        if (data) data->self = this;
    };


};

class LoopList : public MArray<LoopEntry *> {
public:
    LoopList()
    {
        pMin = 0;
        Num = 0;
        m_DefaultAlloc = 4096;
    }

    ~LoopList()
    {
        HANDLE hp = GetProcessHeap();
        DINT ln = locked.GetLen();
        BYTE **p = locked.GetPtr();
        while (ln--)
        {
            HeapFree(hp, 0, *p);
            ++p;
        }
        Clear();
        locked.Clear();
    }

    DataLoopEntry *Min()
    { return pMin ? pMin->left->data : 0; }

    DataLoopEntry *Max()
    { return pMin ? pMin->right->data : 0; }

    bool IsEmpty()
    { return Num == 0; }

    DataLoopEntry *RemoveMin()
    {
        if (!pMin) return 0;
        DataLoopEntry *x = pMin->left->data;
        Remove(pMin->left->data);
        return x;
    }

    DINT Len() const
    { return Num; }

    void Insert(DataLoopEntry *data)
    {
        if (!data->self)
        {
            if (AllocLoopEntry(&data->self))
            {
                InsertLoopEntry(data, &data->self);
                ++Num;
            }
        } else
        {
            BoolWarning(0);
        }
    }

    void Remove(DataLoopEntry *data)
    {
        if (Num && data->self)
        {
            RemoveLoopEntry(&data->self);
            AddRemoved(&data->self);

        }
    }

private:
    DWORD private_allocNodes(BYTE **p)
    {
        DINT lckn = locked.GetLen();
        DWORD
        n = (DWORD)
        1 << ((lckn > 23 ? 23 : lckn) + 1);
        if ((m_DefaultAlloc != 0xFFFFFFFF && n > m_DefaultAlloc))
            n = m_DefaultAlloc;
        DWORD alc = n * sizeof(LoopEntry);
#ifdef _ALLOC_GLOBAL__
        *p = (BYTE *)gmalloc(alc);
#else
        *p = (BYTE *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, alc);
#endif

        return *p ? n : 0;
    }

    void private_allocInitFree(LoopEntry **ptr1, BYTE *p, DINT n)
    {
        while (n--)
        {
            *ptr1++ = (LoopEntry *) p;
            p += sizeof(LoopEntry);
        }
    }

    BOOL private_alloc()
    {
        DWORD n;
        BYTE *p;
        if ((n = private_allocNodes(&p)))
        {
            locked.Add() = p;
            DINT ln = MArray<LoopEntry *>::Len;
            SetLen(n + ln);
            private_allocInitFree(ptr + ln, p, n);
        }
        return 1;

    }

    bool AllocLoopEntry(LoopEntry **self)
    {
        if (MArray<LoopEntry *>::Len)
        {
            --MArray<LoopEntry *>::Len;
            *self = *(ptr + MArray<LoopEntry *>::Len);
        } else
        {
            if (!private_alloc()) return false;
            --MArray<LoopEntry *>::Len;
            *self = *(ptr + MArray<LoopEntry *>::Len);
        }
        return true;
    }

    void InsertLoopEntry(DataLoopEntry *data, LoopEntry **self)
    {
        (*self)->Init(*self, *self, data);
        if (pMin)
        {
            pMin->left->right = (*self)->right;
            (*self)->right->left = pMin->left;
            pMin->left = *self;
            (*self)->right = pMin;
            pMin = *self;
        } else
            pMin = data->self;
    }

    void RemoveLoopEntry(LoopEntry **self)
    {
        (*self)->left->right = (*self)->right;
        (*self)->right->left = (*self)->left;
        if (--Num)
        {
            if (pMin == *self)
                pMin = (*self)->right;
        } else
            pMin = 0;
    }

    void AddRemoved(LoopEntry **self)
    {
        if (MArray<LoopEntry *>::Len + 1 < RealSize)
            *(ptr + MArray<LoopEntry *>::Len++) = *self;
        else
            Add(*self);
        *self = 0;
    }

    MArray<BYTE *> locked;
    DWORD m_DefaultAlloc;
public:
    LoopEntry *pMin;
    DINT Num;
};

#pragma pack(pop) //NADA
