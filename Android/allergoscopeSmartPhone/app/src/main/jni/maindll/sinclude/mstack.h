///////////////////////////////////////////////////
// Stack.h: interface for the SquareSort class.
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////
#ifndef __M_STACK__
#define __M_STACK__

#include "ArHashMap.h"

template<class T, class ARR = MArray<T> >
class MLifo : public ARR {
public :
    MLifo()
    { ARR::SetAddedRealSize(-1); };

    ~MLifo()
    {};

    MLifo(MLifo &l) : ARR(l)
    {};

    void Push(const T &t)
    { ARR::Add(t); }

    T Pop()
    { return ARR::Len ? ARR::ptr[--ARR::Len] : m_Nil; }

    T Pop(int i)
    {
        if (i < ARR::Len)
        {
            T tmp = ARR::ptr[i];
            ARR::Remove(i, 1);
            return tmp;
        }
        return m_Nil;
    }

    bool IsEmpty() const
    { return ARR::Len == 0; }

    T Last()
    { return ARR::Len ? ARR::ptr[ARR::Len - 1] : m_Nil; }

    void Clear()
    { ARR::Clear(); }

    T m_Nil;
};

template<class T>
struct ItemMFifo {
    T value;
    ItemMFifo *next;

    ItemMFifo()
    { next = 0; }

    ItemMFifo *Init(const T &x, ItemMFifo *t)
    {
        value = x;
        next = t;
        return this;
    }
};

template<class T, class ALLOC = Allocator<ItemMFifo<T> > >
class MFifo {
public:
    MFifo(bool fNewDelele = true)
    {
        m_fNewDelele = fNewDelele;
        m_iNum = 0;
        first = last = 0;
    }

    MFifo(const MFifo &f)
    { *this = f; }

    ~MFifo()
    {}

    MFifo &operator=(const MFifo &f)
    {
        Clear();
        ItemMFifo<T> *tmp = f.first;
        m_iNum = f.m_iNum;
        m_fNewDelele = f.m_fNewDelele;
        while (tmp)
        {
            first = AllocNode()->Init(tmp->value, first);
            tmp = tmp->next;
        }
        return *this;
    }

    void CreateNew(bool fNewDelele)
    { m_fNewDelele = fNewDelele; }

    T &operator[](DINT i)
    {
        if (i < 0 || i >= m_iNum) return m_Nil;
        ItemMFifo<T> *entry = first;
        for (int j = 0; entry && j < i; ++j, entry = entry->next);
        return entry->value;
    }

    void Delete(DINT i)
    {
        if (i < 0 || i >= m_iNum) return;
        ItemMFifo<T> *entry = first;
        if (!i)
        {
            Pop();
            return;
        }
        DINT j;
        for (j = 0; entry && j < i - 1; ++j, entry = entry->next);
        if (j != i - 1)
            ASSERT(0);
        ItemMFifo<T> *del = entry->next;
        entry->next = del->next;
        if (last == del)
            last = entry;
        FreeNode(del);
        --m_iNum;
    }

    DINT Len()
    { return m_iNum; }

    void Clear()
    {
        ItemMFifo<T> *tnext, *tmp = first;
        while (tmp)
        {
            tnext = tmp->next;
            FreeNode(tmp);
            tmp = tnext;
        }
        m_Al.Clear();
        last = first = 0;
        m_iNum = 0;
    }

    ItemMFifo<T> *GetPtr()
    { return first; }

    void Push(const T &t)
    {

        ++m_iNum;
        ItemMFifo<T> *_last = last;
        last = AllocNode()->Init(t, 0);
        if (!first)
            first = last;
        else
            _last->next = last;

    }

    T &Push()
    {
        ++m_iNum;
        ItemMFifo<T> *_last = last;
        last = AllocNode();
        if (!first)
            first = last;
        else
            _last->next = last;
        return last->value;
    }

    T Pop()
    {
        if (first)
        {
            --m_iNum;
            ItemMFifo<T> *tmp = first;
            first = first->next;
            if (!first)
                last = 0;
            FreeNode(tmp);
            return tmp->value;
        }
        return m_Nil;
    }

    int IsEmpty()
    { return first == 0; }

    T m_Nil;
protected:
    ItemMFifo<T> *AllocNode()
    {
        ItemMFifo<T> *p = m_Al.Alloc(false);
        if (m_fNewDelele)
            p = new(p) ItemMFifo<T>();
        return p;
    }

    void FreeNode(ItemMFifo<T> *p)
    {
        if (p)
        {
            if (m_fNewDelele)
                p->~ItemMFifo<T>();
            m_Al.Free(p);
        }
    }

    bool m_fNewDelele;
    DINT m_iNum;
    ItemMFifo<T> *first, *last;
    ALLOC m_Al;
public:
    class iterator {
        MFifo &stk;

        ItemMFifo<T> *t;
    public:
        iterator(MFifo &g) : stk(g)
        { t = (ItemMFifo<T> *) 0xFFFFFFFF; }

        T *begin()
        {
            t = stk.first;
            return t ? &t->value : 0;
        }

        T *next()
        {
            if (t == (ItemMFifo<T> *) 0xFFFFFFFF) return begin();
            if (t) t = t->next;
            return t ? &t->value : 0;
        }

        bool end()
        { return t == 0; }
    };

    friend class iterator;

};

struct __DecPos {
};
typedef __DecPos *DecPos;

template<class T>
struct ItemMDeq : public __DecPos {
    T value;
    ItemMDeq *next;
    ItemMDeq *prev;

    ItemMDeq()
    { prev = next = 0; }

    ItemMDeq *InitF(const T &x, ItemMDeq *t)
    {
        prev = 0;
        value = x;
        next = t;
        if (t) t->prev = this;
        return this;
    }

    ItemMDeq *InitL(const T &x, ItemMDeq *t)
    {
        next = 0;
        value = x;
        prev = t;
        if (t) t->next = this;
        return this;
    }

    ItemMDeq *InitF(ItemMDeq *t)
    {
        prev = 0;
        next = t;
        if (t) t->prev = this;
        return this;
    }

    ItemMDeq *InitL(ItemMDeq *t)
    {
        next = 0;
        prev = t;
        if (t) t->next = this;
        return this;
    }

    ItemMDeq(ItemMDeq *a)
    {
        prev = next = 0;
        if (a)
        {
            prev = a;
            a->next = this;
        }
    }

    void Clear()
    { prev = next = 0; }

    void InsertBefore(ItemMDeq *a)
    {
        prev = next = 0;
        if (a)
        {
            prev = a->prev;
            a->prev = this;
            next = a;
            if (prev)
                prev->next = this;
        }
    }

    void InsertAfter(ItemMDeq *a)
    {
        prev = next = 0;
        if (a)
        {
            prev = a;
            next = a->next;
            if (next)
                next->prev = this;
            a->next = this;

        }
    }

    ~ItemMDeq()
    {
        if (prev)
            prev->next = next;
        if (next)
            next->prev = prev;
    }

    void Reset()
    {
        if (prev)
            prev->next = next;
        if (next)
            next->prev = prev;
        next = prev = 0;
    }
};


template<class T, class ALLOC = Allocator<ItemMDeq<T> > >
class MDeq {
    struct Position {
        DecPos pos;
        DINT index;

        Position()
        { Reset(); }

        void Reset()
        { pos = 0; }

        bool IsEmpty()
        { return pos == 0; }

        void Set(DecPos _pos, DINT _index)
        {
            pos = _pos;
            index = _index;
        }
    };

public:

    MDeq(bool fNewDelele = true)
    {
        m_fNewDelele = fNewDelele;
        fKill = true;
        m_iNum = 0;
        m_Al = 0;
        memset(&m_Nil, 0, sizeof(T));
        last = first = 0;
    }

    MDeq(const MDeq &f)
    {
        memset(&m_Nil, 0, sizeof(T));
        fKill = true;
        m_iNum = 0;
        m_Al = 0;
        last = first = 0;
        *this = f;
    }

    virtual ~MDeq()
    {
        Clear();
        DeleteAlocator();
    }

    void CreateNew(bool fNewDelele)
    { m_fNewDelele = fNewDelele; }

    void DeleteAlocator()
    {
        if (m_Al && fKill) delete m_Al;
        m_Al = 0;
        fKill = true;
    }

    void SetAlocator(ALLOC *a, bool _fKill)
    {
        ALLOC *tmp = m_Al;
        if (!a)
        {
            Clear();
            DeleteAlocator();
        }
        m_Al = a;
        if (tmp)
        {
            ItemMDeq<T> *tmp = first;
            bool fLast = 0;
            while (tmp)
            {
                first = AllocNode()->InitF(tmp->value, first);
                if (!fLast)
                {
                    last = first;
                    fLast = true;
                }
                tmp = tmp->next;
            }

        }
        if (tmp && fKill) delete tmp;
        fKill = _fKill;

    }

    //----------------------------------------------------------------
    MDeq &operator=(const MDeq &f)
    {
        Clear();
        DeleteAlocator();
        m_fNewDelele = f.m_fNewDelele;

        ItemMDeq<T> *tmp = f.first;
        bool fLast = false;
        m_iNum = f.m_iNum;
        if (!m_Al) m_Al = new Allocator<ItemMDeq<T> >();
        while (tmp)
        {
            PushBack(tmp->value);
            tmp = tmp->next;
        }
        return *this;
    }
    //----------------------------------------------------------------
    DINT Len()
    { return m_iNum; }

    //----------------------------------------------------------------
    void Clear()
    {
        if (!m_Al) return;
        ItemMDeq<T> *tnext, *tmp = first;
        while (tmp)
        {
            tnext = tmp->next;
            FreeNode(tmp);
            tmp = tnext;
        }
        if (fKill && m_Al)
            m_Al->Clear();
        last = first = 0;
        m_iNum = 0;
    }

    //----------------------------------------------------------------
    DecPos PushFront(const T &t)
    {
        if (!m_Al) m_Al = new Allocator<ItemMDeq<T> >();
        first = AllocNode()->InitF(t, first);
        ++m_iNum;
        if (!first->next)
            last = first;
        return first;
    }

    //----------------------------------------------------------------
    void PushFront(MVector<T> &t)
    {
        T *p = t.GetPtr();
        int l = t.GetLen();
        if (p & l)
        {
            if (!m_Al) m_Al = new Allocator<ItemMDeq<T> >();
            p += l - 1;
            first = AllocNode()->InitF(*p, first);
            ++m_iNum;
            if (!first->next)
                last = first;
            while (--l)
            {
                --p;
                first = AllocNode()->InitF(*p, first);
                ++m_iNum;

            }
        }
    }

    //----------------------------------------------------------------
    void PushFront(MArray<T> &t)
    {
        T *p = t.GetPtr();
        int l = t.GetLen();
        if (p & l)
        {
            p += l - 1;
            if (!m_Al) m_Al = new Allocator<ItemMDeq<T> >();
            first = AllocNode()->InitF(*p, first);
            ++m_iNum;
            if (!first->next)
                last = first;
            while (--l)
            {
                --p;
                first = AllocNode()->InitF(*p, first);
                ++m_iNum;

            }
        }
    }

    //----------------------------------------------------------------
    void PushFront(MDeq<T> &p)
    {
        ItemMDeq<T> *n = p.last;
        if (!n) return;
        if (!m_Al) m_Al = new Allocator<ItemMDeq<T> >();

        first = AllocNode()->InitF(n->value, first);
        ++m_iNum;
        if (!first->next)
            last = first;
        n = n->prev;
        while (n)
        {
            first = AllocNode()->InitF(n->value, first);
            ++m_iNum;
            n = n->prev;
        }
    }

    //----------------------------------------------------------------
    void PushBack(MDeq<T> &p)
    {
        ItemMDeq<T> *n = p.first;
        if (!n) return;
        if (!m_Al) m_Al = new Allocator<ItemMDeq<T> >();

        last = AllocNode()->InitF(n->value, last);
        ++m_iNum;
        if (!last->prev)
            first = last;
        n = n->next;
        while (n)
        {
            last = AllocNode()->InitF(n->value, last);
            ++m_iNum;
            n = n->next;
        }
    }

    //----------------------------------------------------------------
    DecPos PushBack(const T &t)
    {
        if (!m_Al) m_Al = new Allocator<ItemMDeq<T> >();

        last = AllocNode()->InitL(t, last);
        ++m_iNum;
        if (!last->prev)
            first = last;
        return last;
    }

    //----------------------------------------------------------------
    void PushBack(MArray<T> &t)
    {
        T *p = t.GetPtr();
        int l = t.GetLen();
        if (p && l)
        {
            if (!m_Al) m_Al = new Allocator<ItemMDeq<T> >();

            first = AllocNode()->InitF(p[0], last);
            ++m_iNum;
            if (!last->prev)
                first = last;
            for (int i = 1; i < l; ++i)
            {
                first = AllocNode()->InitF(p[i], last);
                ++m_iNum;

            }
        }
    }

    //----------------------------------------------------------------
    void PushBack(MVector<T> &t)
    {
        T *p = t.GetPtr();
        int l = t.GetLen();
        if (p && l)
        {
            if (!m_Al) m_Al = new Allocator<ItemMDeq<T> >();

            first = AllocNode()->InitF(p[0], last);
            ++m_iNum;
            if (!last->prev)
                first = last;
            for (int i = 1; i < l; ++i)
            {
                first = AllocNode()->InitF(p[i], last);
                ++m_iNum;

            }
        }
    }

    //----------------------------------------------------------------
    T &PushFront()
    {
        if (!m_Al) m_Al = new Allocator<ItemMDeq<T> >();

        first = AllocNode()->InitF(first);
        ++m_iNum;
        if (!first->next)
            last = first;
        return first->value;
    }

    //----------------------------------------------------------------
    T &PushBack()
    {
        if (!m_Al) m_Al = new Allocator<ItemMDeq<T> >();

        last = AllocNode()->InitL(last);
        ++m_iNum;
        if (!last->prev)
            first = last;
        return last->value;
    }

    //----------------------------------------------------------------
    ItemMDeq<T> *GetPtr()
    { return first; }

    //----------------------------------------------------------------
    DecPos GetFirstPos() const
    { return first; }

    //----------------------------------------------------------------
    DecPos GetLastPos() const
    { return last; }

    //----------------------------------------------------------------
    DecPos GetPos(int i)
    {
        if (i < 0 || i >= m_iNum) return 0;
        ItemMDeq<T> *entry;
        if (i <= m_iNum / 2)
        {
            entry = first;
            for (DINT j = 0; entry && j < i; ++j, entry = entry->next);
        } else
        {
            entry = last;
            for (DINT j = m_iNum - 1; entry && j > i; --j, entry = entry->prev);
        }
        return entry;
    }

    T &Get(DecPos rPosition)
    {
        ItemMDeq<T> *p = (ItemMDeq<T> *) rPosition;
        if (p)
            return rPosition ? p->value : m_Nil;
        return m_Nil;
    }

    //----------------------------------------------------------------
    T &First()
    {
        if (m_iNum)
            if (first)
                return first->value;
        return m_Nil;
    }

    //----------------------------------------------------------------
    T &Last()
    {
        if (m_iNum)
            if (first)
                return last->value;
        return m_Nil;
    }

    //----------------------------------------------------------------
    T &GetNext(DecPos &rPosition)
    {
        ItemMDeq<T> *p = (ItemMDeq<T> *) rPosition;
        if (p)
        {
            rPosition = p->next;
            return rPosition ? p->next->value : m_Nil;
        }
        return m_Nil;
    }

    //----------------------------------------------------------------
    T &GetPrev(DecPos &rPosition)
    {
        ItemMDeq<T> *p = (ItemMDeq<T> *) rPosition;
        if (p)
        {
            rPosition = p->prev;
            return rPosition ? p->prev->value : m_Nil;
        }
        return m_Nil;
    }

    //----------------------------------------------------------------
    void Remove(DecPos rPosition)
    {
        if (rPosition && m_iNum)
        {
            ItemMDeq<T> *p = (ItemMDeq<T> *) rPosition;
            if (p == last)
                last = p->prev;
            if (p == first)
                first = p->next;
            p->Reset();
            FreeNode(p);
            --m_iNum;

        }
    }

    //----------------------------------------------------------------
    DecPos InsertBefore(DecPos rPosition, const T &newElement)
    {
        ItemMDeq<T> *p = (ItemMDeq<T> *) rPosition;
        if (!p || p == first)
            return PushFront(newElement);
        if (!m_Al) m_Al = new Allocator<ItemMDeq<T> >();
        ItemMDeq<T> *n = AllocNode();
        n->InsertBefore(p);
        n->value = newElement;
        ++m_iNum;
        return n;
    }

    //----------------------------------------------------------------
    DecPos InsertAfter(DecPos rPosition, const T &newElement)
    {
        ItemMDeq<T> *p = (ItemMDeq<T> *) rPosition;
        if (!p || p == last)
            return PushBack(newElement);
        if (!m_Al) m_Al = new Allocator<ItemMDeq<T> >();
        ItemMDeq<T> *n = AllocNode();
        n->InsertAfter(p);
        n->value = newElement;
        ++m_iNum;
        return n;
    }

    //----------------------------------------------------------------
    DecPos FindPos(const T &searchValue, DecPos start = 0, bool fAfter = true)
    {
        ItemMDeq<T> *n = start ? (ItemMDeq<T> *) start : (fAfter ? first : last);

        if (fAfter)
        {
            while (n)
            {
                if (n->value == searchValue)
                    break;
                n = n->next;
            }
        } else
        {
            while (n)
            {
                if (n->value == searchValue)
                    break;
                n = n->prev;
            }

        }
        return n;
    }

    //----------------------------------------------------------------
    T &operator[](DecPos rPosition)
    {
        ItemMDeq<T> *p = (ItemMDeq<T> *) rPosition;
        return p ? p->value : m_Nil;
    }

    //----------------------------------------------------------------
    T &NextPos(DecPos &rPosition)
    {
        ItemMDeq<T> *p = (ItemMDeq<T> *) rPosition;
        rPosition = p->next;
        return p ? p->value : m_Nil;
    }

    void Remove(const T &searchValue)
    {
        ItemMDeq<T> *p = first;
        while (p)
        {
            if (p->value == searchValue)
                break;
            p = p->next;
        }
        if (p)
        {
            p->Reset();
            FreeNode(p);
            --m_iNum;
        }

    }

    void Remove(DINT i)
    {
        if (!mPos.IsEmpty())
        {
            ItemMDeq<T> *p = (ItemMDeq<T> *) mPos.pos;
            if (i - 1 == mPos.index)
                mPos.pos = p->next;
            else if (i + 1 == mPos.index)
                mPos.pos = p->prev;
            else
                mPos.Reset();

        }

        if (mPos.IsEmpty())
        {
            mPos.index = i;
            mPos.pos = GetPos(i);
        }
        if (mPos.pos)
        {
            ItemMDeq<T> *p = (ItemMDeq<T> *) mPos.pos;
            if (p == last)
                last = p->prev;
            if (p == first)
                first = p->next;
            p->Reset();
            FreeNode(p);
            --m_iNum;

        }

    }

    //----------------------------------------------------------------
    T &operator[](DINT i)
    {
        if (!mPos.IsEmpty())
        {
            ItemMDeq<T> *p = (ItemMDeq<T> *) mPos.pos;
            if (i - 1 == mPos.index)
                mPos.pos = p->next;
            else if (i + 1 == mPos.index)
                mPos.pos = p->prev;
            else
                mPos.Reset();

        }

        if (mPos.IsEmpty())
        {
            mPos.index = i;
            mPos.pos = GetPos(i);
        }
        return mPos.pos ? ((ItemMDeq<T> *) mPos.pos)->value : m_Nil;


    }

    //----------------------------------------------------------------
    T PopFront()
    {
        if (m_iNum)
            if (first)
            {
                ItemMDeq<T> *tmp = first;
                first = first->next;
                if (first)
                    first->prev = 0;
                else
                    last = 0;
                T t = tmp->value;
                FreeNode(tmp);
                --m_iNum;
                return t;
            }
        return m_Nil;
    }

    T PopBack()
    {
        if (m_iNum)
            if (last)
            {
                ItemMDeq<T> *tmp = last;
                last = last->prev;
                if (last)
                    last->next = 0;
                else
                    first = 0;
                T t = tmp->value;
                FreeNode(tmp);
                --m_iNum;
                return t;
            }
        return m_Nil;
    }

    int IsEmpty()
    { return !m_iNum || first == 0; }

    T m_Nil;
protected:
    ItemMDeq<T> *AllocNode()
    {
        mPos.Reset();
        ItemMDeq<T> *p = m_Al->Alloc(false);
        if (m_fNewDelele)
            p = new(p) ItemMDeq<T>();
        return p;
    }

    void FreeNode(ItemMDeq<T> *p)
    {
        mPos.Reset();
        if (p)
        {
            if (m_fNewDelele)
                p->~ItemMDeq<T>();
            m_Al->Free(p);
        }
    }

protected:
    ItemMDeq<T> *first, *last;
    ALLOC *m_Al;
    DINT m_iNum;
    bool fKill;
    bool m_fNewDelele;
private:
    Position mPos;
public:
    class iterator {
        MDeq<T> &stk;
        bool direct;
        ItemMDeq<T> *t;
    public:
        iterator(MDeq &g, bool _direct = true) : stk(g), direct(_direct)
        { t = (ItemMDeq<T> *) -1; }

        T *begin()
        {
            t = direct ? stk.first : stk.last;
            return t ? &t->value : 0;
        }

        T *next()
        {
            if (t == (ItemMDeq<T> *) -1) return begin();
            if (t) t = direct ? t->next : t->prev;
            return t ? &t->value : 0;
        }

        bool end()
        { return t == 0; }
    };

    friend class iterator;
};


template<class T, class ARR = MArray<T> >
class RandomStack : public ARR {
public:
    RandomStack(int Size = 0) : ARR(Size)
    {
        srand(time(0));
        ARR::SetAddedRealSize(-1);
        N = 0;
    }

    ~RandomStack()
    {}

    RandomStack(RandomStack &s)
    { *this = s; }

    RandomStack &operator=(RandomStack &s)
    {
        Copy(s);
        N = s.N;
        return *this;
    }

    int IsEmpty()
    { return N == 0; }

    void Push(T t)
    {
        if (ARR::Len > N) ARR::ptr[N++] = t;
        else
        {
            ARR::Add(t);
            ARR::Len = ARR::RealSize - 1;
        }
    }

    T Last()
    { return ARR::ptr[N]; }

    T First()
    { return *ARR::ptr; }

    T Pop()
    {
        DINT i = (N * rand() / (1.0 + RAND_MAX));
        T t = *(ARR::ptr + i);
        *(ARR::ptr + i) = *(ARR::ptr + N - 1);
        *(ARR::ptr + N - 1) = t;
        return ARR::ptr[--N];
    }

    void Clear()
    {
        ARR::Clear();
        N = 0;
    }

    DINT Len()
    { return N; }

private :
    DINT N;
};

#endif
