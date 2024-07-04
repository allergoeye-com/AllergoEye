///////////////////////////////////////////////////
// mlist.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////
#ifndef __M_LIST__
#define __M_LIST__

#include "mallocator.h"

//**************************************************************
//
//**************************************************************
template<class T>
struct MListEntry {
    MListEntry(MListEntry *a = 0) : m_pData(0), m_pPrev(0), m_pNext(0)
    {
        if (a)
        {
            m_pPrev = a;
            a->m_pNext = this;
        }
    }

    void Init(MListEntry *a)
    {
        if (a)
        {
            m_pPrev = a;
            a->m_pNext = this;
        }
    }

    ~MListEntry()
    {
        if (m_pPrev)
            m_pPrev->m_pNext = m_pNext;
        if (m_pNext)
            m_pNext->m_pPrev = m_pPrev;
    }

    void Reset()
    {
        if (m_pPrev)
            m_pPrev->m_pNext = m_pNext;
        if (m_pNext)
            m_pNext->m_pPrev = m_pPrev;
        m_pNext = m_pPrev = 0;
    }

    union {
        MListEntry *m_pPrev;
        MListEntry *prev;
    };
    union {
        MListEntry *m_pNext;
        MListEntry *next;
    };
    union {
        T *m_pData;
        T *val;
    };
};

template<class T>
struct MListEntry2 {
    MListEntry2(MListEntry2 *a = 0) : m_pPrev(0), m_pNext(0)
    {
        if (a)
        {
            m_pPrev = a;
            a->m_pNext = this;
        }
    }

    void Init(MListEntry2 *a)
    {
        m_pPrev = m_pNext = 0;
        if (a)
        {
            m_pPrev = a;
            if (a->m_pNext)
                a->m_pNext->m_pPrev = this;

            a->m_pNext = this;
        }
    }

    void Insert(MListEntry2 *a)
    {
        m_pPrev = m_pNext = 0;
        if (a)
        {
            m_pPrev = a->m_pPrev;
            a->m_pPrev = this;
            m_pNext = a;
            if (m_pPrev)
                m_pPrev->m_pNext = this;
        }
    }

    ~MListEntry2()
    {
        if (m_pPrev)
            m_pPrev->m_pNext = m_pNext;
        if (m_pNext)
            m_pNext->m_pPrev = m_pPrev;
    }

    void Reset()
    {
        if (m_pPrev)
            m_pPrev->m_pNext = m_pNext;
        if (m_pNext)
            m_pNext->m_pPrev = m_pPrev;
        m_pNext = m_pPrev = 0;
    }

    T m_pData;
    union {
        MListEntry2 *m_pPrev;
        MListEntry2 *prev;
    };

    union {
        MListEntry2 *m_pNext;
        MListEntry2 *next;

    };
};

#define ListNotNewEntry MListEntry

//**************************************************************
//
//**************************************************************
template<class T>
class ListAllocator {
public:
    ListAllocator()
    {};

    virtual ~ListAllocator()
    { Clear(); }

    virtual void Delete(T *)
    {}

    virtual void Clear()
    {};

    virtual void Reset()
    { Clear(); }

    virtual T *Add()
    { return 0; }

    virtual T *Add(bool)
    { return 0; }

    virtual void RunAllDataDestructor()
    {};

    virtual MListEntry<T> *Last()
    { return 0; }

    virtual MListEntry<T> *First()
    { return 0; }

    virtual DINT Len()
    { return 0; }

};

//****************************************************
//
//****************************************************
template<class T>
struct MList : public ListAllocator<T> {

    MList(BOOL fKillData = TRUE) : m_pFirst(0), m_pLast(0), m_iNum(0), m_fKill(fKillData)
    {};

    MList(MList &a) : m_pFirst(0), m_pLast(0), m_iNum(0), m_fKill(true)
    { operator=(a); };

    virtual ~MList()
    { Clear(); }

    MList &operator=(const MList &m);

    T *operator[](DINT i);

    BOOL Delete(DINT i);

    void Delete(MListEntry<T> *entry);

    virtual void Delete(T *b);

    virtual void Clear();

    virtual T *Add()
    { return Add((T *) 0); }

    T *Add(T *a);

    T *Insert(DINT i, T *a = 0);

    virtual DINT Len()
    { return m_iNum; }

    T *Add(MListEntry<T> *entry);

    virtual MListEntry<T> *First()
    { return m_pFirst; }

    virtual MListEntry<T> *Last()
    { return m_pLast; }

    void Cut(MListEntry<T> *b);


    MListEntry<T> *Find(T t, DINT &index, BOOL (__cdecl *func)(const void *, const void *))
    {
        MListEntry<T> *entry = m_pFirst;

        DINT i = 0;
        while (i < index)
        {
            entry = entry->m_pNext;
            ++i;
        }
        for (; index < m_iNum && entry; ++index, entry = entry->m_pNext)
            if ((func && func(&t, entry->m_pData)) || t == *entry->m_pData)
                return entry;
        return 0;
    }

    MListEntry<T> *Get(DINT i)
    {
        MListEntry<T> *entry = 0;
        if (i >= 0 && i < m_iNum)
        {
            if (i <= m_iNum / 2)
            {
                entry = m_pFirst;
                for (DINT j = 0; entry && j < i; ++j, entry = entry->m_pNext);
            } else
            {
                entry = m_pLast;
                for (DINT j = m_iNum - 1; entry && j > i; --j, entry = entry->m_pPrev);
            }
        }
        return entry;
    }

    MListEntry<T> *m_pFirst;
    MListEntry<T> *m_pLast;
    DINT m_iNum;
    BOOL m_fKill;
};

//**************************************************************
//
//**************************************************************
template<class T>
class TemplateListNotNew : public ListAllocator<T> {
public:
    TemplateListNotNew() : m_pFirst(0), m_pLast(0), m_iNum(0)
    {};

    virtual ~TemplateListNotNew()
    { Clear(); }

    virtual void Delete(T *b);

    virtual void Clear();

    virtual DINT Len()
    { return m_iNum; }

    virtual T *Add();

    virtual ListNotNewEntry<T> *Last()
    { return m_pLast; }

    virtual ListNotNewEntry<T> *First()
    { return m_pFirst; }

    virtual void RunAllDataDestructor()
    {
        ListNotNewEntry<T> *entry = m_pFirst;
        while (entry)
        {
            entry->m_pData->~T();
            entry = entry->m_pNext;
        }
    }

    ListNotNewEntry<T> *m_pFirst;
    ListNotNewEntry<T> *m_pLast;


    DINT m_iNum;
protected:
    virtual ListNotNewEntry<T> *AllocListEntry() = 0;

    virtual T *AllocData() = 0;

    virtual void Free(T *) = 0;

    virtual void Free(ListNotNewEntry<T> *) = 0;

    T *Add(ListNotNewEntry<T> *entry);
};

//**************************************************************
//
//**************************************************************
template<class T>
class ListNotNew : public TemplateListNotNew<T> {
public:
    ListNotNew()
    {};

    virtual ~ListNotNew()
    {}

    virtual void Clear()
    {
        TemplateListNotNew<T>::Clear();
        m_TAlloc.Clear();
        m_mAlloc.Clear();
    }

    virtual void Reset()
    {
        m_mAlloc.FreeAll();
        m_TAlloc.FreeAll();
        TemplateListNotNew<T>::Clear();
    }

protected:
    virtual ListNotNewEntry<T> *AllocListEntry()
    { return m_mAlloc.Alloc(0); }

    virtual T *AllocData()
    { return m_TAlloc.Alloc(0); }

    virtual void Free(T *t)
    { m_TAlloc.Free(t); }

    virtual void Free(ListNotNewEntry<T> *t)
    { m_mAlloc.Free(t); }

protected:
    Allocator<ListNotNewEntry<T> > m_mAlloc;
    Allocator<T> m_TAlloc;
};

//********************************************************************
//
//********************************************************************
template<class T>
class AllocatorSimple : public ListAllocator<T> {
public:
    AllocatorSimple()
    { m_iNum = 0; };

    virtual ~AllocatorSimple()
    { Clear(); }

    virtual void Delete(T *t)
    {
        if (m_iNum)
        {
            --m_iNum;
            m_mAlloc.Free(t);
        }
    }

    virtual void Clear()
    {
        m_mAlloc.Clear();
        m_iNum = 0;
    };

    virtual void Reset()
    {
        m_mAlloc.FreeAll();
        m_iNum = 0;
    }

    virtual T *Add()
    {
        ++m_iNum;
        return m_mAlloc.Alloc(0);
    }

    virtual void RunAllDataDestructor();

    virtual MListEntry<T> *Last()
    { return 0; }

    virtual MListEntry<T> *First()
    { return 0; }

    virtual DINT Len()
    { return m_iNum; }

    Allocator<T> m_mAlloc;
    DINT m_iNum;


};

template<class T>
class ListNotNew2 {
public:
    ListNotNew2() : m_pFirst(0), m_pLast(0), m_iNum(0)
    {};

    virtual ~ListNotNew2()
    { Clear(); }

    ListNotNew2(const ListNotNew2 &l) : m_pFirst(0), m_pLast(0), m_iNum(0)
    {
        MListEntry2<T> *node = l.m_pFirst;
        while (node)
        {
            *Add() = node->m_pData;
            node = node->next;
        }
    }

    ListNotNew2 &operator=(const ListNotNew2 &l)
    {
        m_pFirst = m_pLast = 0;
        m_iNum = 0;
        MListEntry2<T> *node = l.m_pFirst;
        while (node)
        {
            *Add() = node->m_pData;
            node = node->next;
        }
        return *this;
    }

    void Delete(T *t)
    {
        MListEntry2<T> *entry = (MListEntry2<T> *) t;
        if (entry == m_pLast)
            m_pLast = entry->m_pPrev;
        if (entry == m_pFirst)
            m_pFirst = entry->m_pNext;
        entry->Reset();
        m_mAlloc.Free(entry);
        m_iNum--;
        if (!m_pFirst) m_pLast = 0;
    }

    void Delete(MListEntry2<T> *entry)
    {
        if (entry == m_pLast)
            m_pLast = entry->m_pPrev;
        if (entry == m_pFirst)
            m_pFirst = entry->m_pNext;
        entry->Reset();
        m_mAlloc.Free(entry);
        m_iNum--;
        if (!m_pFirst) m_pLast = 0;
    }

    void Clear()
    {
        m_pFirst = m_pLast = 0;
        m_iNum = 0;
        m_mAlloc.Clear();
    }

    void Reset()
    {
        m_pFirst = m_pLast = 0;
        m_iNum = 0;
        m_mAlloc.Reset();
    }

    T *Add()
    {
        MListEntry2<T> *entry = m_mAlloc.Alloc();
        entry->Init(m_pLast);
        if (!m_pFirst) m_pFirst = entry;
        if ((m_pLast = entry))
        {
            ++m_iNum;
            return &entry->m_pData;
        }
        return 0;

    }

    void RunAllDataDestructor()
    {
        MListEntry2<T> *entry = m_pFirst;
        while (entry)
        {
            T *t = &entry->m_pData;
            t->~T();
            entry = entry->m_pNext;
        }
    }

    ListNotNewEntry<T> *Last()
    { return 0; }

    ListNotNewEntry<T> *First()
    { return 0; }

    virtual DINT Len()
    { return m_iNum; }

    MListEntry2<T> *m_pFirst;
    MListEntry2<T> *m_pLast;
    DINT m_iNum;
protected:
    Allocator<MListEntry2<T> > m_mAlloc;
};

//------------------------------------------------------------------
//
//------------------------------------------------------------------
template<class T>
void AllocatorSimple<T>::RunAllDataDestructor()
{
    DINT ln = m_mAlloc.locked.GetLen();
    if (ln)
    {
        BYTE *t, **p = m_mAlloc.locked.GetPtr();
        DINT n = 0;
        int i = 0;

        T *node, *test = (T *) _alloca(sizeof(T));
        memset(test, 0, sizeof(T));
        while (ln--)
        {
            t = *p++;
#ifdef USE_HeapSize
            n = (int)HeapSize(GetProcessHeap(), 0, t)/m_mAlloc.SizeOfT;
#else

            n = (DINT) 1 << ((i > MAX_SHIFT ? MAX_SHIFT : i) + 1);
            if (n < m_mAlloc._MinAlloc)
                n = m_mAlloc._MinAlloc;
            else if ((DINT) m_mAlloc._Max_Alloc_ != -1 && n > m_mAlloc._Max_Alloc_)
                n = m_mAlloc._Max_Alloc_;
#endif
            while (n--)
            {
                if (memcmp(test, t, sizeof(T)))
                {
                    node = (T *) t;
                    node->~T();
                }
                t += m_mAlloc.SizeOfT;
            }
            ++i;
        }


    }
}

//----------------------------------------------------
//
//----------------------------------------------------
template<class T>
MList<T> &MList<T>::operator=(const MList<T> &p)
{
    if (!m_fKill) Clear();
    MListEntry<T> *m_entry = m_pFirst, *entry = p.m_pFirst;
    m_fKill = true;
    for (DINT i = 0;
         i < p.m_iNum; ++i, entry = entry->m_pNext, m_entry = i < m_iNum ? m_entry->m_pNext : 0)
    {
        if (m_entry)
            *m_entry->m_pData = *entry->m_pData;
        else
        {
#ifdef _ALLOC_GLOBAL__
            BYTE *p = (BYTE *)gmalloc(sizeof(T));
            Add(new (p) T(*entry->m_pData));
#else
            Add(new T(*entry->m_pData));
#endif

        }

    }
    if (p.m_iNum < m_iNum)
    {
        m_pLast = m_entry->m_pPrev;
        while (m_entry)
        {
            entry = m_entry->m_pNext;
#ifdef _ALLOC_GLOBAL__
            if (m_entry->m_pData)
            {
                m_entry->m_pData->~T();
                gfree(m_entry->m_pData);
            }
            if (m_entry)
            {
                m_entry->~MListEntry<T>();
                gfree(m_entry);
            }
#else

            delete m_entry->m_pData;
            delete m_entry;
#endif
            m_entry = entry;
        }

    }
    m_iNum = p.m_iNum;
    return *this;
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
template<class T>
inline T *MList<T>::operator[](DINT i)
{
    MListEntry<T> *entry = Get(i);
    return entry ? entry->m_pData : 0;
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
template<class T>
inline void MList<T>::Clear()
{
    MListEntry<T> *entry, *tmp;
    entry = m_pFirst;
    while (entry)
    {
        tmp = entry->m_pNext;
#ifdef _ALLOC_GLOBAL__
        if (m_fKill && entry->m_pData)
        {
            entry->m_pData->~T();
            gfree(entry->m_pData);
        }
        if (entry)
        {
            entry->~MListEntry<T>();
            gfree(entry);
        }
#else

        if (m_fKill)
            delete entry->m_pData;
        delete entry;
#endif
        entry = tmp;
    }
    m_pFirst = m_pLast = 0;
    m_iNum = 0;
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
template<class T>
inline BOOL MList<T>::Delete(DINT i)
{
    MListEntry<T> *entry = Get(i);
    if (!entry) return 0;
#ifdef _ALLOC_GLOBAL__
    if (m_fKill && entry->m_pData)
    {
        entry->m_pData->~T();
        gfree(entry->m_pData);
    }
#else

    if (m_fKill)
        delete entry->m_pData;
#endif
    if (entry == m_pLast)
        m_pLast = entry->m_pPrev;
    if (entry == m_pFirst)
        m_pFirst = entry->m_pNext;
    entry->Reset();

#ifdef _ALLOC_GLOBAL__
    if (entry)
    {
        entry->~MListEntry<T>()
        gfree(entry);
    }
#else
    delete entry;
#endif
    m_iNum--;
    return 1;
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
template<class T>
void MList<T>::Delete(T *b)
{
    MListEntry<T> *entry = m_pFirst;
    while (entry)
    {
        if (entry->m_pData == b)
        {
#ifdef _ALLOC_GLOBAL__
            if (m_fKill && entry->m_pData)
            {
                entry->m_pData->~T();
                gfree(entry->m_pData);
            }
#else
            if (m_fKill)
                delete entry->m_pData;
#endif
            if (entry == m_pLast)
                m_pLast = entry->m_pPrev;
            if (entry == m_pFirst)
                m_pFirst = entry->m_pNext;
            entry->Reset();
#ifdef _ALLOC_GLOBAL__
            if (entry)
            {
                entry->~MListEntry<T>();
                gfree(entry);
            }
#else
            delete entry;
#endif
            m_iNum--;
            break;
        }
        entry = entry->m_pNext;
    }
    if (!m_pFirst) m_pLast = 0;

}

template<class T>
void MList<T>::Delete(MListEntry<T> *entry)
{
#ifdef _ALLOC_GLOBAL__
    if (m_fKill && entry->m_pData)
    {
        entry->m_pData->~T();
        gfree(entry->m_pData);
    }
#else
    if (m_fKill)
        delete entry->m_pData;
#endif
    if (entry == m_pLast)
        m_pLast = entry->m_pPrev;
    if (entry == m_pFirst)
        m_pFirst = entry->m_pNext;
    entry->Reset();
#ifdef _ALLOC_GLOBAL__
    if (entry)
    {
        entry->~MListEntry<T>();
        gfree(entry);
    }
#else
    delete entry;
#endif
    m_iNum--;
    if (!m_pFirst) m_pLast = 0;

}

template<class T>
void MList<T>::Cut(MListEntry<T> *entry)
{
    if (entry == m_pLast)
        m_pLast = entry->m_pPrev;
    if (entry == m_pFirst)
        m_pFirst = entry->m_pNext;
    entry->Reset();
    m_iNum--;
    if (!m_pFirst) m_pLast = 0;

}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
template<class T>
inline T *MList<T>::Add(T *ptr)
{
#ifdef _ALLOC_GLOBAL__
    BYTE *p = (BYTE *)gmalloc(sizeof(MListEntry<T>));
    if (!p) return 0;
    MListEntry<T> *entry = new (p) MListEntry<T>(m_pLast);
    if (!ptr)
    {
        p = (BYTE *)gmalloc(sizeof(T));
        if (!p) return 0;
        entry->m_pData = new (p) T();
    }
    else
        entry->m_pData = ptr;
#else
    MListEntry<T> *entry = new MListEntry<T>(m_pLast);
    entry->m_pData = ptr ? ptr : new T();
#endif
    return Add(entry);
}

template<class T>
inline T *MList<T>::Add(MListEntry<T> *entry)
{
    if (!m_pFirst) m_pFirst = entry;
    if ((m_pLast = entry))
    {
        ++m_iNum;
        return entry->m_pData;
    }
    return 0;
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
template<class T>
T *MList<T>::Insert(DINT i, T *ptr)
{
    if (i >= m_iNum) return Add();
    MListEntry<T> *entry = Get(i);
#ifdef _ALLOC_GLOBAL__
    BYTE *p = (BYTE *)gmalloc(sizeof(MListEntry<T>));
    MListEntry<T> *add = new (p) MListEntry<T>();
#else
    MListEntry<T> *add = new MListEntry<T>();
#endif

    if (!add)
        return 0;
#ifdef _ALLOC_GLOBAL__
    if (!ptr)
    {
        p = (BYTE *)gmalloc(sizeof(T));
        if (!p) return 0;
        entry->m_pData = new (p) T();
    }
    else
        entry->m_pData = ptr;
#else
    add->m_pData = ptr ? ptr : new T();
#endif

    if (entry)
    {
        if (entry->m_pPrev)
        {
            add->m_pPrev = entry->m_pPrev;
            entry->m_pPrev->m_pNext = add;
        }
        add->m_pNext = entry;
        entry->m_pPrev = add;
        m_iNum++;
        if (!i)
        {
            m_pFirst = add;
            if (!m_pLast)
                m_pLast = add;
        }

    } else
    {
#ifdef _ALLOC_GLOBAL__
        if (m_fKill && add->m_pData)
        {
            add->m_pData->~T();
            gfree(add->m_pData);
        }
        if (add)
        {
            add->~MListEntry<T>();
            gfree(add);
        }
#else
        if (m_fKill) delete add->m_pData;
        delete add;
#endif
    }


    return add->m_pData;
}


//------------------------------------------------------------------
//
//------------------------------------------------------------------
template<class T>
void TemplateListNotNew<T>::Clear()
{
    m_pFirst = m_pLast = 0;
    m_iNum = 0;
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
template<class T>
void TemplateListNotNew<T>::Delete(T *b)
{
    ListNotNewEntry<T> *entry = m_pFirst;
    while (entry)
    {
        if (entry->m_pData == b)
        {
            Free(entry->m_pData);
            if (entry == m_pLast)
                m_pLast = entry->m_pPrev;
            if (entry == m_pFirst)
                m_pFirst = entry->m_pNext;
            entry->Reset();
            Free(entry);
            m_iNum--;
            break;
        }
        entry = entry->m_pNext;
    }
    if (!m_pFirst) m_pLast = 0;
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
template<class T>
T *TemplateListNotNew<T>::Add()
{
    ListNotNewEntry<T> *entry = AllocListEntry();

    entry->Init(m_pLast);
    entry->m_pData = AllocData();
    return Add(entry);
}

//---------------------------------------
//
//---------------------------------------
template<class T>
T *TemplateListNotNew<T>::Add(ListNotNewEntry<T> *entry)
{
    if (!m_pFirst) m_pFirst = entry;
    if ((m_pLast = entry))
    {
        ++m_iNum;
        return entry->m_pData;
    }
    return 0;
}


#endif
