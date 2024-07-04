#ifndef __RuningPoygon__
#define __RuningPoygon__

#include "mvect.h"
#include "hashset.h"
#include "mstack.h"
#include "ArHashMap.h"
#include "ArRBMap.h"

#pragma pack(push)
#pragma pack(8)

struct LineItem : public Segment2D {
    int iIndex;

    LineItem()
    { iIndex = -1; }

    ~LineItem()
    {}

    LineItem(const Vector2D &sorg, const Vector2D &sdest, int i)
    {
        org = sorg;
        dest = sdest;
        iIndex = i;
    }

    void Set(Segment2D &s, int i)
    {
        org = s.org;
        dest = s.dest;
        iIndex = i;
    }
};

typedef ItemMDeq<LineItem> EntryLine;

struct ListLine : public MDeq<LineItem> {
    ListLine()
    {}

    virtual ~ListLine()
    {}

    ListLine(ListLine &f)
    {
        fKill = true;
        m_iNum = 0;
        m_Al = 0;
        last = first = 0;
        *this = f;
    }

    ListLine &operator=(const ListLine &f)
    {
        Clear();
        DeleteAlocator();
        EntryLine *tmp = f.first;
        bool fLast = false;
        m_iNum = 0;
        if (!f.fKill)
        {
            m_Al = f.m_Al;
            fKill = 0;
        }
        while (tmp)
        {
            PushBack(tmp->value);
            tmp = tmp->next;
        }
        return *this;
    }
};

typedef Allocator<ItemMDeq<LineItem> > AllocatorLine;

#ifndef _FEXPORT32_
#define _FEXPORT32_
#endif
EXPORTNIX class _FEXPORT32_ RuningPoygon {
public:
    RuningPoygon()
    { iIndex = -1; };

    ~RuningPoygon()
    {};

    RuningPoygon(const RuningPoygon &p)
    {
        list = p.list;
        box = p.box;
        iIndex = p.iIndex;
    }

    void Clip(Box2D &_box);

    void Reset()
    {
        box.Reset();
        list.Clear();
    }

    RuningPoygon &operator=(const RuningPoygon &p)
    {
        Reset();
        list = p.list;
        box = p.box;
        iIndex = p.iIndex;
        return *this;
    }

    bool Init(IPoint *p, int len);

    bool Init(AllocatorLine *pMyAlloc, Vector2D &center, int _iIndex, Box2D &b);

    bool PtIn(Vector2D &v);

    double Square();

    void FlipLine(Line2D &p);

    void FlipLine(Segment2D &p);

    bool ClipLines(int _iIndex, DecPos pos[2], Vector2D pt[2], bool fInStack = false);

    bool ClipLines(Line2D &p, int _iIndex);

    bool AddLineToPoint(Line2D &p, int _iIndex);

    bool CheckCenter();

    bool IsLeft(RuningPoygon *pe);

    bool IsRight(RuningPoygon *pe);

    int IsClipXRay(Segment2D &p, Vector2D pt[2]);

    bool IsClipLines(Line2D &p, int _iIndex, DecPos pos[2], Vector2D pt[2]);

    void GetVertex(MArray<Vector2D> &res);

    void SetBox();

    bool CheckLink(int _iIndex);

    bool IsClosed();

protected:
    bool IsLeft();

    bool IsRight();

public:
    Circle2D box;
    ListLine list;
    int iIndex;
};
#pragma pack(pop)
#endif

