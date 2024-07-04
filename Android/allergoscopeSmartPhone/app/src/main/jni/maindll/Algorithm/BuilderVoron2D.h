/////////////////////////////////////////////////////////////////////////////////////////////////
//
// File buildervoron2d.h
// is a part of the Pluk CMath Library
// Copyright (C) 1995-2016 Alexander L. Kalaidzidis.
//
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
#ifndef __BuilderVoron2D__
#define __BuilderVoron2D__

#include "RuningPolygon.h"
#include "NodeTreeIt.h"
#include "fpoint.h"
#include "tlistarray.h"

EXPORTNIX class _FEXPORT32_ BuilderVoron2D {
public:
    BuilderVoron2D()
    {}

    ~BuilderVoron2D()
    {}

    void Build(MArray<Vector2F> &in, Box2D &rc, MVector<MArray<Vector2D> > &res);

//	void Build(MArray<Vector2F> &in, Box2D &rc, GraphTriangle &tri);
    void Build(MArray<Vector2F> &pos, int X, int Y, MArray<GNode> &adj)
    {
        MArray<Vector2F> minus;
        Build(pos, minus, X, Y, adj);
    }

    void BuildGabriel(MArray<Vector2F> &pos, int X, int Y, MArray<GNode> &adj)
    {
        MArray<Vector2F> minus;
        BuildGabriel(pos, minus, X, Y, adj);
    }

    void BuildGabriel(MArray<Vector2D> &pos, int X, int Y, MArray<GNode> &adj);

    void
    BuildGabriel(MArray<Vector2F> &pos, MArray<Vector2F> &minus, int X, int Y, MArray<GNode> &adj);

    void Build(MArray<Vector2F> &pos, MArray<Vector2F> &minus, int X, int Y, MArray<GNode> &adj);

protected:
    void Build(MArray<Vector2F> &vpt, MArray<Vector2F> &minus, Box2D &rc, MVector<RuningPoygon> &e,
               MVector<AllocatorLine> &pAlloc);

    void Build(RuningPoygon *pe, TListArray<DINT> &sn, TListArray<DINT> &run);

    void
    Build(MArray<Vector2D> &m_Data, Box2D &rc, MVector<RuningPoygon> &e, AllocatorLine &pAlloc);

    void Build2(MArray<Vector2D> &m_Data, Box2D &rc, MVector<RuningPoygon> &e,
                MVector<AllocatorLine> &pAlloc);

    void Build(int i, int len, TListArray<DINT>::node *plist, DINT *pa, RuningPoygon *pe);

    void Build(TListArray<DINT> &run, TListArray<DINT> &list_run, RuningPoygon *pe);

    static int __cdecl XSortFunc(const void *a, const void *b);

    static int __cdecl XSortFunc2(const void *a, const void *b);

    static UINT AFX_CDECL Build2ThreadFunc(void *Param);

    static UINT AFX_CDECL Build2ThreadFunc_2(void *Param);
};

/*
inline int __cdecl BuilderVoron2D::VesSortFunc(const void *a, const void *b)
{
	RuningPoygon *pa = (RuningPoygon *)a;
	RuningPoygon *pb = (RuningPoygon *)b;
	if (pa->dist > pb->dist) return 1;
	if (pa->dist < pb->dist) return -1;
	return 0;
}*/
inline int __cdecl BuilderVoron2D::XSortFunc(const void *a, const void *b)
{
    RuningPoygon *pa = (RuningPoygon *) a;
    RuningPoygon *pb = (RuningPoygon *) b;
    if (pa->box.center.x > pb->box.center.x) return 1;
    if (pa->box.center.x < pb->box.center.x) return -1;
    return 0;
}

inline int __cdecl BuilderVoron2D::XSortFunc2(const void *a, const void *b)
{
    Vector2D *pa = (Vector2D *) a;
    Vector2D *pb = (Vector2D *) b;
    if (pa->x > pb->x) return 1;
    if (pa->x < pb->x) return -1;
    if (pa->y > pb->y) return 1;
    if (pa->y < pb->y) return -1;

    return 0;
}

#endif
