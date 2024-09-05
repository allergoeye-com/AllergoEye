///////////////////////////////////////////////////
// rbmap.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////
#ifndef __R_B_MAP__
#define __R_B_MAP__

#include "mallocator.h"
#include "mlist.h"

enum {
    _BLACK, _RED
};

/***************************************************
****************************************************/
template<class MAP_KEY>
struct RBKeyNode {
    RBKeyNode *left;
    RBKeyNode *right;
    RBKeyNode *parent;
    bool color;
    MAP_KEY key;

    RBKeyNode()
    {
        color = 0;
        parent = left = right = 0;
    };

    RBKeyNode(RBKeyNode &r)
    { operator=(r); };

    RBKeyNode *Reset()
    {
        key = 0;
        color = 0;
        parent = 0;
        left = 0;
        right = 0;
        return this;
    }

    RBKeyNode &operator=(RBKeyNode &r)
    {
        color = r.color;
        parent = r.parent;
        left = r.left;
        right = r.right;
        return *this;
    };

    void Init(RBKeyNode *_left, RBKeyNode *_right, RBKeyNode *_parent, bool _color)
    {
        left = _left;
        right = _right;
        parent = _parent;
        color = _color;
    }

    void Init(RBKeyNode *_left, RBKeyNode *_right, RBKeyNode *_parent, bool _color, MAP_KEY &_key)
    {
        left = _left;
        right = _right;
        parent = _parent;
        color = _color;
        key = _key;
    }
};

/***************************************************
****************************************************/

template<class MAP_VALUE, class MAP_KEY>
struct RBData {
    MAP_KEY key;
    MAP_VALUE value;

    RBData()
    {};

    ~RBData()
    {};

    RBData(MAP_KEY _key, MAP_VALUE _value)
    {
        key = _key;
        value = _value;
    }

    RBData(const RBData &r)
    { operator=(r); };

    RBData &operator=(const RBData &r)
    {
        key = r.key;
        value = r.value;
        return *this;
    };

    bool operator==(RBData &r) const
    { return key == r.key && value == r.value; };

    void Set(const MAP_KEY &_key, const MAP_VALUE &_value)
    {
        key = _key;
        value = _value;
    }

};

template<class MAP_VALUE, class MAP_KEY>
struct RBTreeNode : public RBData<MAP_VALUE, MAP_KEY> {
    RBTreeNode *left;
    RBTreeNode *right;
    RBTreeNode *parent;
    bool color;

    RBTreeNode()
    {
        color = 0;
        parent = left = right = 0;
    };

    ~RBTreeNode()
    {};

    RBTreeNode(RBTreeNode &r)
    { operator=(r); };

    RBTreeNode *Reset()
    {
        RBData<MAP_VALUE, MAP_KEY>::key = 0;
        color = 0;
        parent = 0;
        left = 0;
        right = 0;
        return this;
    }

    RBTreeNode &operator=(RBTreeNode &r)
    {
        RBData<MAP_VALUE, MAP_KEY>::key = r.key;
        RBData<MAP_VALUE, MAP_KEY>::value = r.value;
        color = r.color;
        parent = r.parent;
        left = r.left;
        right = r.right;
        return *this;
    };

    void Init(RBTreeNode *_left, RBTreeNode *_right, RBTreeNode *_parent, bool _color)
    {
        left = _left;
        right = _right;
        parent = _parent;
        color = _color;
    }

    void
    Init(RBTreeNode *_left, RBTreeNode *_right, RBTreeNode *_parent, bool _color, MAP_KEY &_key,
         MAP_VALUE &_value)
    {
        left = _left;
        right = _right;
        parent = _parent;
        color = _color;
        RBData<MAP_VALUE, MAP_KEY>::key = _key;
        RBData<MAP_VALUE, MAP_KEY>::value = _value;
    }
};

template<class T>
class RBCmp {
public:
    int compare(const T &, const T &)
    { return 0; };
};

struct MMapCmp {
    int compare(LPCSTR a, LPCSTR b)
    { return strcmp(a, b); }

    int compare(LPCWSTR a, LPCWSTR b)
    { return wcscmp(a, b); }
};

template<class MAP_VALUE, class MAP_KEY, class CMP = RBCmp<MAP_KEY>, class LIST_ALLOC =  AllocatorSimple<RBTreeNode<MAP_VALUE, MAP_KEY> > >
class RBMap : public LIST_ALLOC {

public:

    RBMap(LIST_ALLOC *allocator = 0)
    {
        if (!allocator) m_Alloc = this; else m_Alloc = allocator;
        m_fKeyFromCompareFunc = 0;
        m_fNewDelele = 1;
        pRoot = &m_Nil;
        m_Nil.Init(&m_Nil, &m_Nil, &m_Nil, _BLACK);
    };

    virtual ~RBMap()
    { Clear(); };

    RBMap(RBMap &r)
    {
        m_Alloc = this;
        m_fKeyFromCompareFunc = 0;
        pRoot = &m_Nil;
        m_Nil.Init(&m_Nil, &m_Nil, &m_Nil, _BLACK);
        *this = r;
    }

    void SetAllocator(LIST_ALLOC *allocator)
    {
        Clear();
        if (!allocator) m_Alloc = this; else m_Alloc = allocator;
    };

    LIST_ALLOC *GetAllocator()
    { return m_Alloc; }

    void CreateNew(bool fNewDelele)
    { m_fNewDelele = fNewDelele; }

    void KeyFromCompareFunc(bool b)
    { m_fKeyFromCompareFunc = b; }

    MAP_VALUE &operator[](MAP_KEY key)
    { return Add(key, 0); }

    MAP_VALUE &Value(MAP_KEY key)
    { return Add(key, 0); }

    virtual ListNotNewEntry<RBTreeNode<MAP_VALUE, MAP_KEY> > *First()
    { return m_Alloc == this ? LIST_ALLOC::First() : m_Alloc->First(); }

    virtual ListNotNewEntry<RBTreeNode<MAP_VALUE, MAP_KEY> > *Last()
    { return m_Alloc == this ? LIST_ALLOC::Last() : m_Alloc->Last(); }

    RBTreeNode<MAP_VALUE, MAP_KEY> *Root()
    { return pRoot; }

    RBTreeNode<MAP_VALUE, MAP_KEY> *Nil()
    { return &m_Nil; }

    MAP_VALUE &Add(MAP_KEY key, MAP_VALUE &b)
    { return Add(key, &b); }

    bool Lookup(MAP_KEY key, MAP_VALUE &b);

    bool Lookup(MAP_KEY key, MAP_VALUE **b = 0);

    void Delete(MAP_KEY key)
    {
        RBTreeNode<MAP_VALUE, MAP_KEY> *p = Search(pRoot, key);
        if (p != &m_Nil) DeleteNode(p);
    }

    RBData<MAP_VALUE, MAP_KEY> *Get(MAP_KEY key)
    { return (RBData<MAP_VALUE, MAP_KEY> *) Search(pRoot, key); }

    DINT GetLen()
    { return m_Alloc->Len(); }

    RBMap &operator=(RBMap &a);

    void Clear();

    void Reset();

    MAP_KEY MinKey()
    {
        RBTreeNode<MAP_VALUE, MAP_KEY> *node = pRoot;
        while (node->left != &m_Nil) node = node->left;
        return node->key;
    }

    MAP_KEY MaxKey()
    {
        RBTreeNode<MAP_VALUE, MAP_KEY> *node = pRoot;
        while (node->right != &m_Nil) node = node->right;
        return node->key;
    }

    const RBData<MAP_VALUE, MAP_KEY> *Min()
    {
        RBTreeNode<MAP_VALUE, MAP_KEY> *node = pRoot;
        while (node->left != &m_Nil) node = node->left;
        return node;
    }

    const RBData<MAP_VALUE, MAP_KEY> *Max()
    {
        RBTreeNode<MAP_VALUE, MAP_KEY> *node = pRoot;
        while (node->right != &m_Nil) node = node->right;
        return node;
    }

    void GetSortedRBData(MVector<RBData<MAP_VALUE, MAP_KEY> > &arr);

    void GetSortedRBData(MArray<RBData<MAP_VALUE, MAP_KEY> > &arr);

    void GetSortedKeys(MArray<MAP_KEY> &arr);

    void GetSortedPtrRBData(MArray<RBData<MAP_VALUE, MAP_KEY> *> &arr);

    int Rank()
    {
        int y = 0;
        Rank(y, pRoot);
        return y;
    }

protected:
    void Rank(int &y, RBTreeNode<MAP_VALUE, MAP_KEY> *node);

    void Copy(RBMap &mp)
    {
        iterator it(mp);
        for (RBData<MAP_VALUE, MAP_KEY> *t = it.begin(); t; t = it.next())
        {
            Add(t->key, t->value);
        }
    }

    virtual bool AllocNode(RBTreeNode<MAP_VALUE, MAP_KEY> **node);

    virtual void FreeNode(RBTreeNode<MAP_VALUE, MAP_KEY> *node);

    virtual MAP_VALUE &Add(MAP_KEY &key, MAP_VALUE *b, RBTreeNode<MAP_VALUE, MAP_KEY> **_node = 0);

    void DeleteNode(RBTreeNode<MAP_VALUE, MAP_KEY> *node);

    void DeleteFix(RBTreeNode<MAP_VALUE, MAP_KEY> *node);

    virtual RBTreeNode<MAP_VALUE, MAP_KEY> *
    Search(RBTreeNode<MAP_VALUE, MAP_KEY> *node, MAP_KEY &key);

    bool Insert(MAP_KEY &key, MAP_VALUE *value, RBTreeNode<MAP_VALUE, MAP_KEY> **node);

    void LRotate(RBTreeNode<MAP_VALUE, MAP_KEY> *node);

    void RRotate(RBTreeNode<MAP_VALUE, MAP_KEY> *node);

protected:
    RBTreeNode<MAP_VALUE, MAP_KEY> m_Nil;
    LIST_ALLOC *m_Alloc;
    bool m_fNewDelele;
    bool m_fKeyFromCompareFunc;

public:
    class iterator {
        RBMap &mp;
        RBTreeNode<MAP_VALUE, MAP_KEY> *t, *node;
        MArray<RBTreeNode<MAP_VALUE, MAP_KEY> *> stack;
        RBTreeNode<MAP_VALUE, MAP_KEY> **test, **pStack;
        bool fCheck;
        int len;
    public:
        iterator(RBMap &g) : mp(g)
        { t = 0; }

        RBData<MAP_VALUE, MAP_KEY> *begin()
        {
            if (mp.pRoot == &mp.m_Nil) return 0;
            node = mp.pRoot;
            len = 16;
            stack.SetLen(16);
            pStack = stack.GetPtr();
            test = pStack;
            fCheck = 1;
            do
            {
                if (pStack - test == len)
                {
                    stack.SetLen(len << 1);
                    pStack = test = stack.GetPtr();
                    pStack += len;
                    len <<= 1;
                }

                *pStack++ = node;
                node = node->left;
            } while (node != &mp.m_Nil);
            node = *(--pStack);
            t = node;
            if ((fCheck = node->right != &mp.m_Nil))
                node = node->right;
            return (RBData<MAP_VALUE, MAP_KEY> *) t;
        }

        RBData<MAP_VALUE, MAP_KEY> *next()
        {
            if (!t)
                return begin();
            if (t != &mp.m_Nil && (fCheck || pStack > test))
            {
                if (fCheck)
                {
                    do
                    {
                        if (pStack - test == len)
                        {
                            stack.SetLen(len << 1);
                            pStack = test = stack.GetPtr();
                            pStack += len;
                            len <<= 1;
                        }

                        *pStack++ = node;
                        node = node->left;
                    } while (node != &mp.m_Nil);
                }
                node = *(--pStack);
                t = node;
                if ((fCheck = node->right != &mp.m_Nil))
                    node = node->right;
            } else
                t = &mp.m_Nil;

            return t != &mp.m_Nil ? (RBData<MAP_VALUE, MAP_KEY> *) t : 0;
        }

        bool end()
        { return !t || t == &mp.m_Nil; }
    };

    class iteratorRev {
        RBMap &mp;
        RBTreeNode<MAP_VALUE, MAP_KEY> *t, *node;
        MArray<RBTreeNode<MAP_VALUE, MAP_KEY> *> stack;
        RBTreeNode<MAP_VALUE, MAP_KEY> **test, **pStack;
        bool fCheck;
        int len;
    public:
        iteratorRev(RBMap &g) : mp(g)
        { t = 0; }

        RBData<MAP_VALUE, MAP_KEY> *begin()
        {
            if (mp.pRoot == &mp.m_Nil) return 0;
            node = mp.pRoot;
            len = 16;
            stack.SetLen(16);
            pStack = stack.GetPtr();
            test = pStack;
            fCheck = 1;
            do
            {
                if (pStack - test == len)
                {
                    stack.SetLen(len << 1);
                    pStack = test = stack.GetPtr();
                    pStack += len;
                    len <<= 1;
                }

                *pStack++ = node;
                node = node->right;
            } while (node != &mp.m_Nil);
            node = *(--pStack);
            t = node;
            if ((fCheck = node->left != &mp.m_Nil))
                node = node->left;
            return (RBData<MAP_VALUE, MAP_KEY> *) t;
        }

        RBData<MAP_VALUE, MAP_KEY> *next()
        {
            if (!t)
                return begin();
            if (t != &mp.m_Nil && (fCheck || pStack > test))
            {
                if (fCheck)
                {
                    do
                    {
                        if (pStack - test == len)
                        {
                            stack.SetLen(len << 1);
                            pStack = test = stack.GetPtr();
                            pStack += len;
                            len <<= 1;
                        }

                        *pStack++ = node;
                        node = node->right;
                    } while (node != &mp.m_Nil);
                }
                node = *(--pStack);
                t = node;
                if ((fCheck = node->left != &mp.m_Nil))
                    node = node->left;
            } else
                t = &mp.m_Nil;

            return t != &mp.m_Nil ? (RBData<MAP_VALUE, MAP_KEY> *) t : 0;
        }

        bool end()
        { return !t || t == &mp.m_Nil; }
    };

    friend class iterator;

    friend class iteratorRev;

    CMP cmp;
    RBTreeNode<MAP_VALUE, MAP_KEY> *pRoot;

};

#define MAP2 RBMap<MAP_VALUE, MAP_KEY, RBCmp<MAP_KEY>, ListNotNew2<RBTreeNode<MAP_VALUE, MAP_KEY> > >

template<class MAP_VALUE, class MAP_KEY>
class RBMap2
        : public RBMap<MAP_VALUE, MAP_KEY, RBCmp<MAP_KEY>, ListNotNew2<RBTreeNode<MAP_VALUE, MAP_KEY> > > {
public:
    RBMap2()
            : RBMap<MAP_VALUE, MAP_KEY, RBCmp<MAP_KEY>, ListNotNew2<RBTreeNode<MAP_VALUE, MAP_KEY> > >(
            0)
    {};

    RBMap2(RBMap2 &a)
            : RBMap<MAP_VALUE, MAP_KEY, RBCmp<MAP_KEY>, ListNotNew2<RBTreeNode<MAP_VALUE, MAP_KEY> > >(
            a)
    {};

    MAP_VALUE &Value(MAP_KEY key)
    { return AddSort(key, 0); }

    MAP_VALUE &Add(MAP_KEY key, MAP_VALUE &b)
    { return AddSort(key, &b); }

    MAP_VALUE &operator[](MAP_KEY key)
    { return AddSort(key, 0); }

    RBTreeNode<MAP_VALUE, MAP_KEY> *FirstNode()
    { return ListNotNew2<RBTreeNode<MAP_VALUE, MAP_KEY> >::m_pFirst; }

    RBTreeNode<MAP_VALUE, MAP_KEY> *LastNode()
    { return ListNotNew2<RBTreeNode<MAP_VALUE, MAP_KEY> >::m_pLast; }

    RBTreeNode<MAP_VALUE, MAP_KEY> *NextNode(RBTreeNode<MAP_VALUE, MAP_KEY> *node)
    {
        MListEntry2<RBTreeNode<MAP_VALUE, MAP_KEY> > *entry = (MListEntry2<RBTreeNode<MAP_VALUE, MAP_KEY> > *) node;
        return entry->m_pNext ? &entry->m_pNext->m_pData : 0;
    }

    RBTreeNode<MAP_VALUE, MAP_KEY> *PrevNode(RBTreeNode<MAP_VALUE, MAP_KEY> *node)
    {
        MListEntry2<RBTreeNode<MAP_VALUE, MAP_KEY> > *entry = (MListEntry2<RBTreeNode<MAP_VALUE, MAP_KEY> > *) node;
        return entry->m_pPrev ? &entry->m_pPrev->m_pData : 0;
    }

    void GetSortedRBData(MVector<RBData<MAP_VALUE, MAP_KEY> > &arr)
    {
        arr.SetLen(ListNotNew2<RBTreeNode<MAP_VALUE, MAP_KEY> >::m_iNum);
        RBData<MAP_VALUE, MAP_KEY> *ptr = arr.GetPtr();
        MListEntry2<RBTreeNode<MAP_VALUE, MAP_KEY> > *eprev = ListNotNew2<RBTreeNode<MAP_VALUE, MAP_KEY> >::m_pFirst;
        while (eprev)
        {
            *ptr++ = eprev->m_pData;
            eprev = eprev->m_pNext;
        }
    }

    void GetSortedRBData(MArray<RBData<MAP_VALUE, MAP_KEY> > &arr)
    {
        arr.SetLen(ListNotNew2<RBTreeNode<MAP_VALUE, MAP_KEY> >::m_iNum);
        RBData<MAP_VALUE, MAP_KEY> *ptr = arr.GetPtr();
        MListEntry2<RBTreeNode<MAP_VALUE, MAP_KEY> > *eprev = ListNotNew2<RBTreeNode<MAP_VALUE, MAP_KEY> >::m_pFirst;
        while (eprev)
        {
            *ptr++ = eprev->m_pData;
            eprev = eprev->m_pNext;
        }
    }

    void GetSortedKeys(MArray<MAP_KEY> &arr)
    {
        arr.SetLen(ListNotNew2<RBTreeNode<MAP_VALUE, MAP_KEY> >::m_iNum);
        MAP_KEY *ptr = arr.GetPtr();
        MListEntry2<RBTreeNode<MAP_VALUE, MAP_KEY> > *eprev = ListNotNew2<RBTreeNode<MAP_VALUE, MAP_KEY> >::m_pFirst;
        while (eprev)
        {
            *ptr++ = eprev->m_pData.key;
            eprev = eprev->m_pNext;
        }
    }

    void GetSortedPtrRBData(MArray<RBData<MAP_VALUE, MAP_KEY> *> &arr)
    {
        arr.SetLen(ListNotNew2<RBTreeNode<MAP_VALUE, MAP_KEY> >::m_iNum);
        RBData<MAP_VALUE, MAP_KEY> **ptr = arr.GetPtr();
        MListEntry2<RBTreeNode<MAP_VALUE, MAP_KEY> > *eprev = ListNotNew2<RBTreeNode<MAP_VALUE, MAP_KEY> >::m_pFirst;
        while (eprev)
        {
            *ptr++ = (RBData<MAP_VALUE, MAP_KEY> *) eprev;
            eprev = eprev->m_pNext;
        }

    }

protected:
    MAP_VALUE &AddSort(MAP_KEY &key, MAP_VALUE *b);


};

//-------------------------------------
//
//-------------------------------------
template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
void RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::DeleteNode(RBTreeNode<MAP_VALUE, MAP_KEY> *node)
{

    RBTreeNode<MAP_VALUE, MAP_KEY> *tmpX, *tmpY;
    if (!node || node == &m_Nil) return;
    if (node->left == &m_Nil || node->right == &m_Nil)
        tmpY = node;
    else
    {
        tmpY = node->right;
        while (tmpY->left != &m_Nil) tmpY = tmpY->left;
    }
    if (tmpY->left != &m_Nil)
        tmpX = tmpY->left;
    else
        tmpX = tmpY->right;

    tmpX->parent = tmpY->parent;
    if (tmpY->parent == &m_Nil)
        pRoot = tmpX;
    else
    {
        if (tmpY->parent->left == tmpY)
            tmpY->parent->left = tmpX;
        else
            tmpY->parent->right = tmpX;
    }
    if (tmpY != node)
    {
        node->key = tmpY->key;
        node->value = tmpY->value;
    }
    if (tmpY->color == _BLACK)
        DeleteFix(tmpX);
    FreeNode(tmpY);
    m_Nil.Init(&m_Nil, &m_Nil, &m_Nil, _BLACK);
}

template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline void RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Clear()
{
    if (m_fNewDelele)
        LIST_ALLOC::RunAllDataDestructor();
    LIST_ALLOC::Clear();
    pRoot = &m_Nil;
}

template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline void RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Reset()
{
    if (m_fNewDelele)
        LIST_ALLOC::RunAllDataDestructor();
    LIST_ALLOC::Reset();
    pRoot = &m_Nil;
}

template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline bool RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Lookup(MAP_KEY key, MAP_VALUE &b)
{
    RBTreeNode<MAP_VALUE, MAP_KEY> *p = Search(pRoot, key);
    if (p != &m_Nil)
    {
        b = p->value;
        return 1;
    }
    return 0;
}

template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline bool RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Lookup(MAP_KEY key, MAP_VALUE **b)
{
    RBTreeNode<MAP_VALUE, MAP_KEY> *p = Search(pRoot, key);
    if (p != &m_Nil)
    {
        if (b)
            *b = &p->value;
        return 1;
    }
    return 0;
}

template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
void RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::GetSortedPtrRBData(
        MArray<RBData<MAP_VALUE, MAP_KEY> *> &arr)
{
    arr.SetLen(m_Alloc->Len());
    if (pRoot == &m_Nil) return;
    RBTreeNode<MAP_VALUE, MAP_KEY> *node = pRoot;
    int len = 16;
    RBData<MAP_VALUE, MAP_KEY> **pArr = arr.GetPtr();
    MArray<RBTreeNode<MAP_VALUE, MAP_KEY> *> stack(16);
    RBTreeNode<MAP_VALUE, MAP_KEY> **test, **pStack = stack.GetPtr();
    test = pStack;
    bool fCheck = 1;
    do
    {
        if (fCheck)
        {
            do
            {
                if (pStack - test == len)
                {
                    stack.SetLen(len << 1);
                    pStack = test = stack.GetPtr();
                    pStack += len;
                    len <<= 1;
                }

                *pStack++ = node;
                node = node->left;
            } while (node != &m_Nil);
        }
        node = *(--pStack);
        *pArr++ = (RBData<MAP_VALUE, MAP_KEY> *) node;
        if ((fCheck = node->right != &m_Nil))
            node = node->right;
    } while (fCheck || pStack > test);

}
//-------------------------------------
//
//-------------------------------------

template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
void RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::DeleteFix(RBTreeNode<MAP_VALUE, MAP_KEY> *node)
{
    while (node != pRoot && node->color == _BLACK)
    {
        if (node == node->parent->left)
        {
            RBTreeNode<MAP_VALUE, MAP_KEY> *tmp = node->parent->right;
            if (tmp->color == _RED)
            {
                tmp->color = _BLACK;
                node->parent->color = _RED;
                LRotate(node->parent);
                tmp = node->parent->right;
            }
            if (tmp->left->color == _BLACK && tmp->right->color == _BLACK)
            {
                tmp->color = _RED;
                node = node->parent;
            } else
            {
                if (tmp->right->color == _BLACK)
                {
                    tmp->left->color = _BLACK;
                    tmp->color = _RED;
                    RRotate(tmp);
                    tmp = node->parent->right;
                }
                tmp->color = node->parent->color;
                node->parent->color = _BLACK;
                tmp->right->color = _BLACK;
                LRotate(node->parent);
                node = pRoot;
            }
        } else
        {
            RBTreeNode<MAP_VALUE, MAP_KEY> *tmp = node->parent->left;
            if (tmp->color == _RED)
            {
                tmp->color = _BLACK;
                node->parent->color = _RED;
                RRotate(node->parent);
                tmp = node->parent->left;
            }
            if (tmp->right->color == _BLACK && tmp->left->color == _BLACK)
            {
                tmp->color = _RED;
                node = node->parent;
            } else
            {
                if (tmp->left->color == _BLACK)
                {
                    tmp->right->color = _BLACK;
                    tmp->color = _RED;
                    LRotate(tmp);
                    tmp = node->parent->left;
                }
                tmp->color = node->parent->color;
                node->parent->color = _BLACK;
                tmp->left->color = _BLACK;
                RRotate(node->parent);
                node = pRoot;
            }
        }
    }
    node->color = _BLACK;

}

//-------------------------------------
//
//-------------------------------------
template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
MAP_VALUE &RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Add(MAP_KEY &key, MAP_VALUE *value,
                                                           RBTreeNode<MAP_VALUE, MAP_KEY> **_node)
{
    RBTreeNode<MAP_VALUE, MAP_KEY> *node = _node ? *_node : 0, *last;
    if (!Insert(key, value, &node))
    {
        if (node)
        {
            if (_node) *_node = node;
            if (value) node->value = *value;
            return node->value;
        }
        return m_Nil.value;
    }
    node->color = _RED;
    last = node;
    while (node->parent->color == _RED)
    {
        if (node->parent == node->parent->parent->left)
        {
            RBTreeNode<MAP_VALUE, MAP_KEY> *temp = node->parent->parent->right;
            if (temp->color == _RED)
            {
                node->parent->color = _BLACK;
                temp->color = _BLACK;
                node->parent->parent->color = _RED;
                node = node->parent->parent;
            } else
            {
                if (node == node->parent->right)
                {
                    node = node->parent;
                    LRotate(node);
                }
                node->parent->color = _BLACK;
                node->parent->parent->color = _RED;
                RRotate(node->parent->parent);
            }
        } else
        {
            RBTreeNode<MAP_VALUE, MAP_KEY> *temp = node->parent->parent->left;
            if (temp->color == _RED)
            {
                node->parent->color = _BLACK;
                temp->color = _BLACK;
                node->parent->parent->color = _RED;
                node = node->parent->parent;
            } else
            {
                if (node == node->parent->left)
                {
                    node = node->parent;
                    RRotate(node);
                }
                node->parent->color = _BLACK;
                node->parent->parent->color = _RED;
                LRotate(node->parent->parent);
            }
        }
    }
    pRoot->color = _BLACK;
    m_Nil.Init(&m_Nil, &m_Nil, &m_Nil, _BLACK);
    if (_node) *_node = last;

    return last->value;
}

//-------------------------------------
//
//-------------------------------------
template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
void RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Rank(int &y, RBTreeNode<MAP_VALUE, MAP_KEY> *node)
{
    if (node == &m_Nil)
        return;
    ++y;
    int l = y;
    int r = y;
    Rank(l, node->left);
    Rank(r, node->right);
    y = max(l, r);
}

//----------------------------------------------
//
//----------------------------------------------
template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline RBTreeNode<MAP_VALUE, MAP_KEY> *
RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Search(RBTreeNode<MAP_VALUE, MAP_KEY> *node,
                                                   MAP_KEY &key)
{
    if (m_fKeyFromCompareFunc)
    {
        int n;
        while (node != &m_Nil && (n = cmp.compare(key, node->key)))
            node = n < 0 ? node->left : node->right;
    } else
        while (node != &m_Nil && node->key != key)
            node = node->key > key ? node->left : node->right;
    return node;
}

//----------------------------------------------
//
//----------------------------------------------
template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline void
RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::FreeNode(RBTreeNode<MAP_VALUE, MAP_KEY> *node)
{
    if (m_fNewDelele) node->~RBTreeNode<MAP_VALUE, MAP_KEY>();
    m_Alloc->Delete(node);

}

//----------------------------------------------
//
//----------------------------------------------
template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline bool
RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::AllocNode(RBTreeNode<MAP_VALUE, MAP_KEY> **node)
{
    *node = m_Alloc->Add();
    if (*node)
    {
        if (m_fNewDelele) *node = new(*node) RBTreeNode<MAP_VALUE, MAP_KEY>();
        (*node)->Init(&m_Nil, &m_Nil, &m_Nil, _BLACK);
    }
    return *node != 0;
}

//----------------------------------------------
//
//----------------------------------------------
template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
bool RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Insert(MAP_KEY &key, MAP_VALUE *value,
                                                        RBTreeNode<MAP_VALUE, MAP_KEY> **node)
{

    if (pRoot == &m_Nil)
    {
        if (*node)
            pRoot = *node;
        else
        {
            if (!AllocNode(&pRoot)) return 0;
            pRoot->key = key;
            if (value)
                pRoot->value = *value;
            *node = pRoot;
        }
        return true;
    }

    RBTreeNode<MAP_VALUE, MAP_KEY> *nodeParent = &m_Nil;
    RBTreeNode<MAP_VALUE, MAP_KEY> *temp = pRoot;
    int n = 0;
    if (m_fKeyFromCompareFunc)
    {
        while (temp != &m_Nil)
        {
            nodeParent = temp;
            if (!(n = cmp.compare(key, temp->key)))
            {
                *node = temp;
                return false;
            }
            if (n < 0)
                temp = temp->left;
            else
                temp = temp->right;
        }
        n = n < 0;
    } else
        while (temp != &m_Nil)
        {
            nodeParent = temp;
            if ((n = (key < temp->key)))
                temp = temp->left;
            else if (key > temp->key)
                temp = temp->right;
            else
            {
                *node = temp;
                return false;
            }
        }
    if (!*node)
    {
        if (!AllocNode(node)) return 0;
        (*node)->key = key;
        if (value)
            (*node)->value = *value;
    }
    (*node)->color = _RED;
    (*node)->parent = nodeParent;
    if (n) //(*node)->key < nodeParent->key)
        nodeParent->left = *node;
    else
        nodeParent->right = *node;
    return true;
}

//----------------------------------------------
//
//----------------------------------------------
template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline void
RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::LRotate(RBTreeNode<MAP_VALUE, MAP_KEY> *node)
{

    RBTreeNode<MAP_VALUE, MAP_KEY> *temp = node->right;
    if (temp != &m_Nil)
    {
        node->right = temp->left;

        if (temp->left != &m_Nil)
            temp->left->parent = node;
        temp->parent = node->parent;

        if (node->parent == &m_Nil)
            pRoot = temp;
        else
        {
            if (node == node->parent->left)
                node->parent->left = temp;
            else
                node->parent->right = temp;
        }

        temp->left = node;
        node->parent = temp;
    }
}

//----------------------------------------------
//
//----------------------------------------------
template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
void RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::RRotate(RBTreeNode<MAP_VALUE, MAP_KEY> *node)
{
    RBTreeNode<MAP_VALUE, MAP_KEY> *temp = node->left;
    if (temp != &m_Nil)
    {
        node->left = temp->right;

        if (temp->right != &m_Nil)
            temp->right->parent = node;
        temp->parent = node->parent;
        if (node->parent == &m_Nil)
            pRoot = temp;
        else
        {
            if (node == node->parent->right)
                node->parent->right = temp;
            else
                node->parent->left = temp;
        }
        temp->right = node;
        node->parent = temp;
    }
}

//---------------------------------------
//
//---------------------------------------
template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC> &
RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::operator=(RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC> &a)
{
    Clear();
    m_fNewDelele = a.m_fNewDelele;
    m_fKeyFromCompareFunc = a.m_fKeyFromCompareFunc;
    if (a.pRoot != &a.m_Nil)
        Copy(a);
    return *this;
}

//----------------------------------------------
//
//----------------------------------------------
template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC, class DATA__ARRAY__>
void GetSortedRBData(DATA__ARRAY__ &arr, RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC> &mp)
{
    arr.SetLen(mp.Len());
    if (mp.Root() == mp.Nil()) return;
    RBTreeNode<MAP_VALUE, MAP_KEY> *node = mp.Root();
    int index = 0, len = 16;
    RBData<MAP_VALUE, MAP_KEY> *pArr = arr.GetPtr();
    if (node == mp.Nil()) return;
    MArray<RBTreeNode<MAP_VALUE, MAP_KEY> *> stack(16);
    RBTreeNode<MAP_VALUE, MAP_KEY> **test, **pStack = stack.GetPtr();
    test = pStack;
    bool fCheck = 1;
    do
    {
        if (fCheck)
        {
            do
            {
                if (pStack - test == len)
                {
                    stack.SetLen(len << 1);
                    pStack = test = stack.GetPtr();
                    pStack += len;
                    len <<= 1;
                }

                *pStack++ = node;
                node = node->left;
            } while (node != mp.Nil());
        }
        node = *(--pStack);
        *pArr++ = *(RBData<MAP_VALUE, MAP_KEY> *) node;
        if ((fCheck = node->right != mp.Nil()))
            node = node->right;
    } while (fCheck || pStack > test);

}

template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC, class DATA__ARRAY__>
void GetSortedKeys(DATA__ARRAY__ &arr, RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC> &mp)
{
    arr.SetLen(mp.Len());
    if (mp.Root() == mp.Nil()) return;
    RBTreeNode<MAP_VALUE, MAP_KEY> *node = mp.Root();
    int index = 0, len = 16;
    RBData<MAP_VALUE, MAP_KEY> *pArr = arr.GetPtr();
    if (node == mp.Nil()) return;
    MArray<RBTreeNode<MAP_VALUE, MAP_KEY> *> stack(16);
    RBTreeNode<MAP_VALUE, MAP_KEY> **test, **pStack = stack.GetPtr();
    test = pStack;
    bool fCheck = 1;
    do
    {
        if (fCheck)
        {
            do
            {
                if (pStack - test == len)
                {
                    stack.SetLen(len << 1);
                    pStack = test = stack.GetPtr();
                    pStack += len;
                    len <<= 1;
                }

                *pStack++ = node;
                node = node->left;
            } while (node != mp.Nil());
        }
        node = *(--pStack);
        *pArr++ = node->key;
        if ((fCheck = node->right != mp.Nil()))
            node = node->right;
    } while (fCheck || pStack > test);

}

template<class MAP_VALUE, class MAP_KEY>
MAP_VALUE &RBMap2<MAP_VALUE, MAP_KEY>::AddSort(MAP_KEY &key, MAP_VALUE *b)
{
    RBTreeNode<MAP_VALUE, MAP_KEY> *node = 0, *prev = 0;
    int i = MAP2::m_iNum;

    RBMap<MAP_VALUE, MAP_KEY, RBCmp<MAP_KEY>, ListNotNew2<RBTreeNode<MAP_VALUE, MAP_KEY> > >::Add(
            key, b, &node);
    if (MAP2::m_iNum > 1 && i != MAP2::m_iNum)
    {
        MListEntry2<RBTreeNode<MAP_VALUE, MAP_KEY> > *eprev, *entry = (MListEntry2<RBTreeNode<MAP_VALUE, MAP_KEY> > *) node;
        if (node == MAP2::pRoot)
            prev = node->left != &MAP2::m_Nil ? node->left : node->right;
        else
            prev = node->parent;
        eprev = (MListEntry2<RBTreeNode<MAP_VALUE, MAP_KEY> > *) prev;
        MAP2::m_pLast = entry->m_pPrev;
        entry->Reset();
        if (prev->key > node->key)
        {
            while (eprev)
            {
                if (eprev->m_pData.key > node->key)
                {
                    if (eprev == MAP2::m_pFirst)
                        break;
                    eprev = eprev->m_pPrev;
                } else
                {
                    eprev = eprev->m_pNext;
                    break;
                }
            }
        } else
        {
            while (eprev)
            {
                if (eprev->m_pData.key < node->key)
                {
                    if (MAP2::m_pLast == eprev)
                    {
                        eprev = 0;
                        break;
                    } else
                        eprev = eprev->m_pNext;
                } else
                {
                    break;
                }
            }
        }
        if (eprev)
        {
            if (eprev->m_pPrev)
            {
                entry->m_pPrev = eprev->m_pPrev;
                eprev->m_pPrev->m_pNext = entry;
            }
            entry->m_pNext = eprev;
            eprev->m_pPrev = entry;
            if (MAP2::m_pFirst == eprev)
                MAP2::m_pFirst = entry;
        } else
        {
            entry->Init(MAP2::m_pLast);
            MAP2::m_pLast = entry;
        }
    }
    return node->value;


}

template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline void RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::GetSortedRBData(
        MVector<RBData<MAP_VALUE, MAP_KEY> > &arr)
{ ::GetSortedRBData(arr, *this); }

template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline void RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::GetSortedRBData(
        MArray<RBData<MAP_VALUE, MAP_KEY> > &arr)
{ ::GetSortedRBData(arr, *this); }

template<class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline void RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::GetSortedKeys(MArray<MAP_KEY> &arr)
{ ::GetSortedKeys(arr, *this); }


#endif
