/////////////////////////////////////////////////////////////////////////////////////////////////
//
// File buildervoron2d.cpp
// is a part of the Pluk CMath Library
// Copyright (C) 1995-2016 Alexander L. Kalaidzidis.
//
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "BuilderVoron2D.h"
#include "MultiThread.h"

extern MultiThreadS *pMultiThreads;


//--------------------------------------
//
//--------------------------------------
void BuilderVoron2D::Build(RuningPoygon *pe, TListArray<DINT> &sn, TListArray<DINT> &run)
{
TListArray<DINT>::node *curent = sn.first, *entry = run.first;
	bool fEq = curent == entry;
	Vector2D p1, p2;
	Line2D p;
	double d;
	int N = 0;
	int k = 0;
	curent = sn.first;
	while (entry)
	{
	k = 0;
			curent = fEq ? entry->next : sn.first;
	
		while (curent)
		{
			if (curent != entry)
			{
	++k;
				p1 = (pe[entry->val].box.center + pe[curent->val].box.center);
				p1 /= 2.0;
				p.n = (pe[curent->val].box.center - p1);
				d = p.n.LenQ();
				if (d > pe[entry->val].box.radiusQ || d > pe[curent->val].box.radiusQ)
				{
					curent = curent->next;
					continue;
				}
				p.n /= sqrt(d);
				p.dist = -p1.x * p.n.x - p1.y * p.n.y;
			/*	if ((p.Distance(pe[entry->val].box.center)) < 1.0e-06)
				{
				double k1 = p.Distance(pe[entry->val].box.center);
				double k2 = p.Distance(pe[curent->val].box.center);
					curent = curent->next;
					continue;
				}
			*/		
				DecPos pos1[2], pos2[2];
				Vector2D n1[2], n2[2];
/*				if (pe[curent->val].IsClipLines(p, pe[entry->val].iIndex, pos1, n1) && pe[entry->val].IsClipLines(p, pe[curent->val].iIndex, pos2, n2))
				{
					pe[curent->val].ClipLines(pe[entry->val].iIndex, pos1, n1);
					pe[entry->val].ClipLines(pe[curent->val].iIndex, pos2, n2);
				}
*/				
				if (pe[curent->val].IsClipLines(p, pe[entry->val].iIndex, pos1, n1))
					pe[curent->val].ClipLines(pe[entry->val].iIndex, pos1, n1);
				if (pe[entry->val].IsClipLines(p, pe[curent->val].iIndex, pos2, n2))
					pe[entry->val].ClipLines(pe[curent->val].iIndex, pos2, n2);
				
			
			}
			curent = curent->next;
		}
		entry = entry->next;
		
	}
}
/*
void BuilderVoron2D::Build(MArray<Vector2F> &in, Box2D &rc, GraphTriangle &tri)
{
	Vector2F *p = in.GetPtr();
	DINT len = in.GetLen();
	MArray<Vector2D> m_Data(len);
	Vector2D *p2D = m_Data.GetPtr();
	for (int i = 0; i < len; ++i)
		p2D[i].Set(p[i].x, p[i].y);
	Box2D orig = rc;
	Box2D mk = orig;
	mk.minPoint.x -= 1.0;
	mk.minPoint.y -= 1.0;
	mk.maxPoint.x += 1.0;
	mk.maxPoint.y += 1.0;
	double S1 = mk.Width() * mk.Height();
	MVector<RuningPoygon> Poly;
	MVector<AllocatorLine > pAlloc;
	Build2(m_Data, mk, Poly, pAlloc);
	for (int i = 0; i < len; ++i)
		p[i].Set((float)p2D[i].x, (float)p2D[i].y);

	tri.Init(Poly, in);
		Poly.Clear();
	pAlloc.Clear();

}
*/
//------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
void BuilderVoron2D::Build(MArray<Vector2F> &in, Box2D &rc, MVector<MArray<Vector2D> > &res)
{
	double S = 0, S1;
	Vector2F *p = in.GetPtr();
	DINT k, j, l, i, len = in.GetLen();
	MArray<Vector2D> m_Data(len);
	Vector2D *p2D = m_Data.GetPtr();
	for (i = 0; i < len; ++i)
		p2D[i].Set(p[i].x, p[i].y);
	Box2D orig = rc;
	Box2D mk = orig;
	mk.minPoint.x -= 1.0;
	mk.minPoint.y -= 1.0;
	mk.maxPoint.x += 1.0;
	mk.maxPoint.y += 1.0;
	S1 = mk.Width() * mk.Height();
	MVector<RuningPoygon> Poly;
	MVector<AllocatorLine > pAlloc;
	//AllocatorLine  pAlloc;
	Build2(m_Data, mk, Poly, pAlloc);
	RuningPoygon *pe = Poly.GetPtr();
	Line2D line;
	res.SetLen(len);
	RBMap<int, Vector2D> ps;
	MArray<Vector2D> pp; 
	for (i = 0; i < len; ++i)
	{
		S += pe[i].Square(); 
		MArray<Vector2D> vertex;
		orig.Line(Line2D::PL_BOTTOM, line);
		pe[i].ClipLines(line, -11);
		orig.Line(Line2D::PL_TOP, line);
		pe[i].ClipLines(line, -12);
		orig.Line(Line2D::PL_LEFT, line);
		pe[i].ClipLines(line, -13);
		orig.Line(Line2D::PL_RIGHT, line);
		pe[i].ClipLines(line, -14);
		pe[i].GetVertex(vertex);

		res[i].SetLen(vertex.GetLen());
		int ip;
		for (k = 0, j = 0, l = (int)vertex.GetLen(); j < l; ++j, ++k)
			if (ps.Lookup(vertex[j], ip))
				res[i][k] = pp[ip];
			else
			{
				ps[vertex[j]] = (int)pp.GetLen();
				res[i][k] = vertex[j];
				pp.Add() = vertex[j];
			}
	//	Vector2D v = res[i][0];
	//	res[i].Add() = v;

	}
	
	Poly.Clear();
	pAlloc.Clear();
}
//------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
void BuilderVoron2D::Build(MArray<Vector2F> &pos, MArray<Vector2F> &minus, int X, int Y, MArray<GNode> &adj)
{
	Box2D orig(0, 0, X - 1, Y - 1);
	Box2D mk = orig;
	mk.minPoint.x -= 1.0;
	mk.minPoint.y -= 1.0;
	mk.maxPoint.x += 1.0;
	mk.maxPoint.y += 1.0;
	MVector<RuningPoygon> Poly;
//	AllocatorLine pAlloc;
//	pAlloc._Max_Alloc_ = 0x100000;
	MVector<AllocatorLine > pAlloc;
	Build(pos, minus, mk, Poly, pAlloc);
	int len = (int)pos.GetLen();
	RuningPoygon *pe = Poly.GetPtr();
	adj.SetLen(len);
	pos.SetLen(len);
	Line2D line;
	Vector2F *pPos = pos.GetPtr();
	for (int i = 0; i < len; ++i)
	{
		Vector2D c = pe[i].box.center;
		int n = (int)adj.GetLen();
		int num = (int)pe[i].list.Len();
		int j = 0;
		adj.SetLen(n + num);
		GNode *ver = adj.GetPtr() + n;
		pPos[i].Set((float)c.x, (float)c.y); 
		adj[i].pw = n; 
		double mx, dw = LONG_MAX; 
		EntryLine *node = (EntryLine *)pe[i].list.GetFirstPos();
		while(node)
		{
			if (node->value.iIndex >= 0)
			{
				mx = (pe[node->value.iIndex].box.center - c).LenQ();
				
				ver[j].pwt = (float)mx;
				ver[j].pw = node->value.iIndex;
				++j;
			}
			node = node->next;	
		}
		adj[i].num = j;		 
		adj.SetLen(n + j);



	}
	Poly.Clear();
	pAlloc.Clear();
	

}	
//------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
void BuilderVoron2D::BuildGabriel(MArray<Vector2F> &pos, MArray<Vector2F> &minus, int X, int Y, MArray<GNode> &adj)
{
	Box2D orig(0, 0, X - 1, Y - 1);
	Box2D mk = orig;
	mk.minPoint.x -= 1.0;
	mk.minPoint.y -= 1.0;
	mk.maxPoint.x += 1.0;
	mk.maxPoint.y += 1.0;
	MVector<RuningPoygon> Poly;
//	AllocatorLine pAlloc;
//	pAlloc._Max_Alloc_ = 0x100000;
	MVector<AllocatorLine > pAlloc;
	Build(pos, minus, mk, Poly, pAlloc);
	int len = (int)pos.GetLen();
	RuningPoygon *pe = Poly.GetPtr();
	adj.SetLen(len);
	pos.SetLen(len);
	Line2D line;
	Vector2F *pPos = pos.GetPtr();
	for (int i = 0; i < len; ++i)
	{
		Vector2D c = pe[i].box.center;
		int n = (int)adj.GetLen();
		int num = (int)pe[i].list.Len();
		int j = 0;
		adj.SetLen(n + num);
		GNode *ver = adj.GetPtr() + n;
		ListLine &list = pe[i].list;
		EntryLine *node = (EntryLine *)list.GetFirstPos();
		pPos[i].Set((float)c.x, (float)c.y); 
		adj[i].pw = n; 
		double mx; 
		int N = 0;
		while(node)
		{
			if (node->value.iIndex >= 0 && node->value.iIndex < len)
			{
				mx = (pe[node->value.iIndex].box.center - c).LenQ();
								bool fOk = true;
				Vector2D cc(pe[node->value.iIndex].box.center);
                                cc += c;
                                cc *= 0.5; 
				Circle2D test(cc, (pe[node->value.iIndex].box.center - c).Len()/2.0);
				EntryLine *n1 = (EntryLine *)list.GetFirstPos();
				while(n1)
				{
					if (n1->value.iIndex != node->value.iIndex && n1->value.iIndex != i && n1->value.iIndex >= 0)
					{
						if (test.PtInCircle2D(pe[n1->value.iIndex].box.center))
						{
							fOk = false;
							break;
						}
					}
					n1 = n1->next;	
				}
				if (fOk)
				{
					n1 = (EntryLine *)pe[node->value.iIndex].list.GetFirstPos();
					while(n1)
					{
						if (n1->value.iIndex != node->value.iIndex && n1->value.iIndex != i && n1->value.iIndex >= 0)
						{
							if (test.PtInCircle2D(pe[n1->value.iIndex].box.center))
							{
								fOk = false;
								break;
							}
						}
						n1 = n1->next;	
					}
				}
				if (fOk)
				{
					ver[j].pwt = (float)mx;
					ver[j].pw = node->value.iIndex;
					++j;
				}
				else
				node->value.iIndex = -node->value.iIndex;
			}
			node = node->next;	
		}
		adj[i].num = j;		 
		adj.SetLen(n + j);
	}
	Poly.Clear();
	pAlloc.Clear();
	

}


//------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------
void BuilderVoron2D::Build(MArray<Vector2F> &vpt, MArray<Vector2F> &minus, Box2D &rc, MVector<RuningPoygon> &e, MVector<AllocatorLine > &pAlloc)
{

	int i, len = (int)vpt.GetLen();
	int len1 = (int)minus.GetLen();
	MArray<Vector2D> m_Data(len + len1);
	Vector2D *p2D = m_Data.GetPtr();
	Vector2F *p = vpt.GetPtr();
	for (i = 0; i < len; ++i, ++p)
		p2D[i].Set(p->x, p->y);
	p = minus.GetPtr();
	len += len1;
	for (; i < len; ++i, ++p)
		p2D[i].Set(p->x, p->y);
	Build2(m_Data, rc, e, pAlloc);

}

//--------------------------------------
//
//--------------------------------------
void BuilderVoron2D::Build(MArray<Vector2D> &m_Data, Box2D &rc, MVector<RuningPoygon> &e, AllocatorLine &pAlloc)
{
	Vector2D *rec = m_Data.GetPtr();
	DINT len = m_Data.GetLen();
	if (!len)
		return;
	
	MArray<TListArray<DINT>::node> list(len);
	MArray<DINT> wh;
	TListArray<DINT>::node *entry, *plist = list.GetPtr();
	
	int i;
	e.SetLen(len);
	RuningPoygon *pe1, *pe = e.GetPtr();
	RuningPoygon tmp_r;
	Vector2D c = rc.CenterPoint();
	pAlloc._Max_Alloc_ = 0x100000;
	tmp_r.Init(&pAlloc, c, 0, rc);
	for (i = 0; i < len; ++i, ++plist)
	{
		plist->Init(i);
		pe[i] = tmp_r;
		pe[i].box.center = rec[i];
		pe[i].iIndex = i;
	}
	tmp_r.Reset();
	e.SortIndex(wh, XSortFunc);
	plist = list.GetPtr();
	double sh;
	DINT *pa = wh.GetPtr();
	TListArray<DINT> run;
	TListArray<DINT> list_run;
	int j = 0;
	
	
	sh = pe[pa[len > 100 ? 100 : len - 1]].box.center.x + 1;
	
	for (i = 0; i < len; )
	{
		
		while (i < len && (sh - pe[pa[i]].box.center.x) > 1.0)
		{
			run.Add(plist + pa[i]);
			++i;
		
		}
		if (run.num)
		{

			if (list_run.num)
				Build(pe, list_run, run);
			Build(pe, run, run);
			entry = list_run.first;

			while (entry)
			{
				pe1 = pe + entry->val;
				if (sh - (pe1->box.center.x + pe1->box.radius) > pe1->box.radius*1.5)
				{
					list_run.Del(entry);
					entry = list_run.first;
				}
				else
					entry = entry->next;
			}

			run.MoveTo(&list_run);
	
		}
		sh = pe[pa[len > i + 100 ? i + 100 : len - 1]].box.center.x + 2.0;

	}
	if (run.num)
	{
		if (list_run.num)
			Build(pe, list_run, run);
		Build(pe, run, run);
		run.MoveTo(&list_run);
	}
	Build(pe, list_run, list_run);
//	CheckResult(pe, list_run, run);
	//TRACE("%d\r\n", run.num);
		

}

//**********************************************************************************/
struct ParamBuldVoron2D {
	BuilderVoron2D *self;
	TListArray<DINT>::node *plist;
	DINT *pa;
	RuningPoygon *pe;
	DINT Start;
	DINT N;
};
struct ParamBuldVoron2D_2 {
	BuilderVoron2D *self;
	RuningPoygon *pe;
	TListArray<DINT> run[2];
};

UINT AFX_CDECL BuilderVoron2D::Build2ThreadFunc(void *Param)
{
	ParamBuldVoron2D *self = (ParamBuldVoron2D *)Param;
	self->self->Build((int)self->Start, (int)self->N, self->plist, self->pa, self->pe);
	return 0;
}
UINT AFX_CDECL BuilderVoron2D::Build2ThreadFunc_2(void *Param)
{
	ParamBuldVoron2D_2 *self = (ParamBuldVoron2D_2 *)Param;
	self->self->Build(self->run[0], self->run[1], self->pe);
	return 0;
}

//--------------------------------------
//
//--------------------------------------
void BuilderVoron2D::Build2(MArray<Vector2D> &m_Data, Box2D &rc, MVector<RuningPoygon> &e, MVector<AllocatorLine > &pAlloc)
{
	int NThreads = pMultiThreads->Num();
	NThreads = 1;

	if (NThreads & 1)
		--NThreads;

	Vector2D *rec = m_Data.GetPtr();
	DINT len = m_Data.GetLen();
/*	srand((unsigned)time( NULL ) );
	for (DINT i = 0; i < len; ++i)
	{
		double u;
		int ix = (int)rec[i].x;
		double ox = rec[i].x;
		do {
			u = ((double)rand()/RAND_MAX - 0.5) * 0.02;
			rec[i].x = ox + u;
		}
		while (fabs(u) < 1.0e-3 || (int)rec[i].x != ix);
		int iy = (int)rec[i].y;
		double oy = rec[i].y;
		do {
			u = ((double)rand()/RAND_MAX - 0.5) * 0.02;
			rec[i].y = oy + u; 
		}
		while (fabs(u) < 1.0e-3 || (int)rec[i].y != iy);
	}
	*/
/*	for (DINT i = 0; i < len; ++i)
	{
		double u = (double)rand()/(RAND_MAX + 1) * 0.4;
		rec[i].x += u; 
		u = (double)rand()/(RAND_MAX + 1) * 0.4;
		rec[i].y += u; 
	} 
*/

	if (!NThreads)
	{
		pAlloc.Add();
		Build(m_Data, rc, e, pAlloc[0]);
		return;
	}
	
	MArray<DINT> wh;
	m_Data.SortIndex(wh, XSortFunc2);
	
	MArray<TListArray<DINT>::node> list(len);
	TListArray<DINT>::node *plist = list.GetPtr();
	DINT *pa = wh.GetPtr();
	int i;
	e.SetLen(len);
	RuningPoygon *pe1, *pe = e.GetPtr();
	Vector2D c = rc.CenterPoint();
	DINT step = max(1, (int)len / NThreads);
	ParamBuldVoron2D *Param = (ParamBuldVoron2D *)_alloca(sizeof(ParamBuldVoron2D) * (NThreads + 1));
	Param[0].self = this;
	Param[0].pe = pe;
	Param[0].N = step;
	Param[0].plist = plist;
	Param[0].Start = 0;
	Param[0].pa = pa;
	pAlloc.SetLen(NThreads);
	
	for (i = 0; i < NThreads && Param[i - 1].N < len; ++i)
	{
		memcpy(Param + i, Param, sizeof(ParamBuldVoron2D));
		Param[i].Start = i ? Param[i - 1].N : 0;
		int step2 = (int)step - 1;
		if (Param[i].Start + step2 < len) 
		{
			double x1 = rec[pa[Param[i].Start + step2]].x + PRECISION;
			while (Param[i].Start + step2 < len && rec[pa[Param[i].Start + step2]].x <= x1)
				++step2; 
			Param[i].N = Param[i].Start + step2;
		}
		else
		Param[i].N = len;
	}
	NThreads = i;
	Param[NThreads - 1].N = len;
	int j;
	for (j = 0; j < NThreads; ++j)
	{
		AllocatorLine &iAlloc = pAlloc[j];
		iAlloc._Max_Alloc_ = 0x100000;
		RuningPoygon tmp_r;
		tmp_r.Init(&iAlloc, c, 0, rc);
		for (i = (int)Param[j].Start; i < (int)Param[j].N; ++i)
		{
			int k = (int)pa[i];
			plist[k].Init(k);
			pe[k] = tmp_r;
			pe[k].box.center = rec[k];
			pe[k].iIndex = k;
		}
		tmp_r.Reset();
	}
	for (j = 0; j < NThreads; ++j)
	{
		pMultiThreads->Run(j, Build2ThreadFunc, Param + j);
	}
	pMultiThreads->Wait();

	ParamBuldVoron2D_2 *Param2 = (ParamBuldVoron2D_2 *)_alloca(sizeof(ParamBuldVoron2D_2) * (NThreads + 1));
	
	while (true)
	{
		memset(Param2, 0, sizeof(ParamBuldVoron2D_2) * (NThreads + 1));
		int k = 0;
		double right, left;
		for (int j = 0; j < NThreads; ++k)
		{
			for (int n = 0; n < 2; ++n, ++j)
			{
				right = pe[pa[Param[j].N - 1]].box.center.x + 1.0;
				left = pe[pa[Param[j].Start]].box.center.x - 1.0;
				for (i = (int)Param[j].Start; i < (int)Param[j].N; ++i)
				{
					pe1 = pe + pa[i];
					
//					if (n)
//					{
//						if (pe1->IsRight(pe))
//							Param2[k].run[n].Add(plist + pa[i]);
//					}
//					else
//						if (pe1->IsLeft(pe)) 
							Param2[k].run[n].Add(plist + pa[i]);
				}
			}
			Param[k].Start = Param[j - 2].Start;
			Param[k].N = Param[j - 1].N;
			Param2[k].pe = pe;
			Param2[k].self = this;
			if (NThreads > 1)
				pMultiThreads->Run(k, Build2ThreadFunc_2, Param2 + k);
		}
		if (NThreads > 1)
			pMultiThreads->Wait();
		else
		{
			Build(Param2[0].run[0], Param2[0].run[1], pe);
			break;
		}
		NThreads /= 2;
	}
	
	len = e.GetLen();
	if (0)
	{
		for (int i = 0; i < len; ++i)
		{
				EntryLine *node = (EntryLine *)pe[i].list.GetFirstPos();
				EntryLine *node1 = node;
				while(node)
				{
					int first = node->value.iIndex;
					int next = node->next ? node->next->value.iIndex : node1->value.iIndex;
				if (next >= 0 && first >= 0)
				{
					if (!pe[first].CheckLink(next) && !pe[next].CheckLink(first))
					{
						Vector2D p1 = (pe[next].box.center + pe[first].box.center);
						p1 /= 2.0;
						Line2D p;
						p.n = (pe[first].box.center - p1);
						p.n.Normalize();
						p.dist = -p1.x * p.n.x - p1.y * p.n.y;
						pe[next].AddLineToPoint(p, pe[first].iIndex);
						pe[first].AddLineToPoint(p, pe[next].iIndex);
			
					} 
				}
				node = node->next;	
			}
		}
	}
}

void BuilderVoron2D::Build(int i, int len, TListArray<DINT>::node *plist, DINT *pa, RuningPoygon *pe)
{
	TListArray<DINT> run;
	TListArray<DINT> list_run;
	int j = 0;
	double sh = pe[pa[len > 100 ? 100 : len - 1]].box.center.x + 1;
	
	for (; i < len; )
	{
		
		while (i < len && (sh - pe[pa[i]].box.center.x) > 1.0)
		{
			run.Add(plist + pa[i]);
			++i;
		
		}
		if (run.num)
		{

			if (list_run.num)
				Build(pe, list_run, run);
			
			Build(pe, run, run);
			run.MoveTo(&list_run);
		

		}
		sh = pe[pa[len > i + 100 ? i + 100 : len - 1]].box.center.x + 2.0;

	}
	if (run.num)
	{
		if (list_run.num)
			Build(pe, list_run, run);
		Build(pe, run, run);
		
	}

}
void BuilderVoron2D::Build(TListArray<DINT> &run, TListArray<DINT> &list_run, RuningPoygon *pe)
{
	Build(pe, list_run, run);

}
