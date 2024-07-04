/*
 * Bmp2PolyLine.cpp
 *
 *  Created on: Oct 15, 2016
 *      Author: alex
 */
#include "stdafx.h"
#include "Bmp2PolyLine.h"
#include "BuilderVoron2D.h"

#define FIRST_ANGLE 30
#define FIRST_VES 25

void ClearBorderVoxels(int *Buffer, int X, int Y, int Z, double CropLevel, BOOL fMoreBranches);

Bmp2PolyLine::Bmp2PolyLine()
{
    img = 0;
}

Bmp2PolyLine::~Bmp2PolyLine()
{

}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void Bmp2PolyLine::Build(CMathImage *_img, MVector<MArray<POINT> > &res)
{
    adj.Clear();
    arr.Clear();
    index.Clear();
    pindex.Clear();
    img = _img;
    InitEdgeGraph();
    Union();
//	InitEdgeGraph(true);
    PutRes(res);


}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void Bmp2PolyLine::PutRes(MVector<MArray<POINT> > &dat)
{
    dat.Clear();
    for (int l, i = 0, len = index.GetLen(); i < len; ++i)
        if ((l = index[i].GetLen()))
        {
            MArray<POINT> &a = dat.Add();
            MArray<int> &id = index[i];
            a.SetLen(l);
            for (int j = 0; j < l; ++j)
            {
                Vector2F &v = arr[id[j]];
                a[j].x = 2 * (int) (v.x + 0.5f);
                a[j].y = 2 * (int) (img->Y - (int) (v.y + 0.5f));
            }
        }
}

void Bmp2PolyLine::UnionInsertFromDist(float MaxCon)
{

    MArray<GNode> adj;
    BuilderVoron2D vor;
    MArray<Vector2F> minus;
    vor.Build(arr, minus, img->X, img->Y, adj);
    int kk, count = 0;
    do
    {
        kk = 0;
        for (int i = 0, ln = index.GetLen(); i < ln; ++i)
        {
            if (!index[i].GetLen() || index[i].GetLen() > 20) continue;

            MArray<int> &ind = index[i];
            int first = ind[0];
            int last = ind[ind.GetLen() - 1];
            GNode *node = adj.GetPtr() + adj[last].pw;
            float sumLast = SHRT_MAX;

            float Nlast = MaxCon;
            int lk, k, ilast = -1;
            for (k = 0, lk = adj[last].num; k < lk; ++k)
            {
                if (node[k].pwt < Nlast && pindex[node[k].pw].y != i)
                {
                    float test = (arr[node[k].pw] - arr[first]).LenQ();
                    if (test < MaxCon)
                        if (test / 2.0 < node[k].pwt && sumLast > Nlast + test)
                        {
                            ilast = node[k].pw;
                            Nlast = node[k].pwt;
                            sumLast = Nlast;
                        }
                }

            }
            node = adj.GetPtr() + adj[first].pw;
            float Nfirst = MaxCon;
            float sumFirst = SHRT_MAX;
            int ifirst = -1;
            for (k = 0, lk = adj[first].num; k < lk; ++k)
            {
                if (node[k].pwt < Nfirst && pindex[node[k].pw].y != i)
                {

                    float test = (arr[node[k].pw] - arr[last]).LenQ();
                    if (test < MaxCon)
                        if (test / 2.0 < node[k].pwt && sumFirst > Nfirst + test)
                        {
                            ifirst = node[k].pw;
                            Nfirst = node[k].pwt;
                            sumFirst = Nfirst;
                        }
                    //s2.Add(node[k].pw);

                }
            }

            if (ifirst != -1 || ilast != -1)
            {
                if (sumLast < sumFirst)
                {
                    int sw = ifirst;
                    ifirst = ilast;
                    ilast = sw;
                    ind.Swap();
                }
                IPoint start = pindex[ifirst];
                MArray<int> &ind1 = index[start.y];
                int j, N = ind1.GetLen();
                int N1 = ind.GetLen();
                for (j = start.x + (start.x != 0); j < N; ++j)
                    pindex[ind1[j]].x += N1;
                N = ind.GetLen();
                for (j = 0; j < N; ++j)
                    pindex[ind[j]].Set(start.x + j + (start.x != 0), start.y);
                ind1.Insert(start.x + (start.x != 0), ind);
                ind.Clear();
                ++kk;
            } else
            { ;

            }

        }
    } while (kk);
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void Bmp2PolyLine::UnionFromDist(float dist)
{
    IPoint prev(-1, -1);
    MVector<MArray<int> > wrem;
    int I, k = 0;
    MArray<int> path;

    double f = FindPathGamilton(0, 0, dist, path);
    MArray<int> path1;
    int i, l, ll;
    for (i = 1, l = index.GetLen(); i < l; ++i)
    {
        path1.SetLen(0);
        for (int j = 0; j < 2; ++j)
        {

            double f1 = FindPathGamilton(i, j ? index[i].GetLen() - 1 : 0, dist, path1);
            if (f1 < f)
            {
                f = f1;
                path = path1;
            }
        }
    }

    wrem.Add().SetLen(l);
    for (i = 0, ll = path.GetLen(); i < ll; ++i)
    {
        I = wrem.GetLen() - 1;
        MArray<int> &ind = wrem[I];
        IPoint start = pindex[path[i]];
        MArray<int> &ind1 = index[start.y];
        if (prev.y == start.y)
        {
            if (!prev.x && start.x)
            {
                for (int j = 0, len = ind1.GetLen(); j < len; ++j, ++k)
                    ind[k] = ind1[j];
            } else
            {
                for (int j = ind1.GetLen() - 1; j >= 0; --j, ++k)
                    ind[k] = ind1[j];
            }

        } else
        {
            Vector2F f1 = arr[ind[ind.GetLen() - 1]];
            Vector2F f2 = arr[ind1[start.x]];
            if ((f1 - f2).LenQ() >= dist)
            {
                ind.SetLen(k);
                //ind.Compact();
                wrem.Add().SetLen(l);
                k = 0;
            }
        }
        prev = start;

    }
    wrem[wrem.GetLen() - 1].SetLen(k);
    wrem[wrem.GetLen() - 1].Compact();
    index = wrem;


}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void Bmp2PolyLine::UnionFromAngle()
{
    float MaxCon = 100000000.0f;
    //return;
    float a, b;
    Vector2F f1, f2, f3, f4;
    int kk, count = 0;
    do
    {
        kk = 0;
        for (int i = 0, ln = index.GetLen(); i < ln; ++i)
        {
            if (index[i].GetLen() < 4) continue;

            MArray<int> &ind = index[i];


            int last = ind[ind.GetLen() - 1];
            int l1 = ind.GetLen();
            f1 = arr[ind[l1 - 3]];
            f2 = arr[last];
            GNode *node = adj.GetPtr() + adj[last].pw;
            float Nlast = MaxCon;
            int lk, k, ilast = -1;
            for (k = 0, lk = adj[last].num; k < lk; ++k)
            {
                if (node[k].pwt < Nlast && pindex[node[k].pw].y != i &&
                    index[pindex[node[k].pw].y].GetLen())
                {
                    IPoint &start = pindex[node[k].pw];
                    MArray<int> &ind1 = index[start.y];

                    int N = ind1.GetLen();
                    if (N < 4) continue;
                    if (!start.x)
                    {
                        f3 = arr[ind1[0]];
                        f4 = arr[ind1[3]];
                    } else
                    {
                        f4 = arr[ind1[N - 3]];
                        f3 = arr[ind1[N - 1]];
                    }
                    a = fabs(PolarAngle(f1, f2, f3, f4));
                    b = fabs(PolarAngle(f1, f2, f3));
                    if (node[k].pwt < FIRST_VES || (b < FIRST_ANGLE && a < FIRST_ANGLE))
                    {
                        ilast = node[k].pw;
                        Nlast = node[k].pwt + (a + b) / 2.0f;
                    }
                }

            }
            int first = ind[0];
            f1 = arr[ind[3]];
            f2 = arr[first];
            node = adj.GetPtr() + adj[first].pw;
            float Nfirst = MaxCon;
            int ifirst = -1;
            for (k = 0, lk = adj[first].num; k < lk; ++k)
            {
                if (node[k].pwt < Nfirst && pindex[node[k].pw].y != i &&
                    index[pindex[node[k].pw].y].GetLen())
                {

                    IPoint start = pindex[node[k].pw];
                    MArray<int> &ind1 = index[start.y];
                    int N = ind1.GetLen();
                    if (N < 4) continue;

                    if (!start.x)
                    {
                        f3 = arr[ind1[0]];
                        f4 = arr[ind1[3]];
                    } else
                    {
                        f4 = arr[ind1[N - 3]];
                        f3 = arr[ind1[N - 1]];
                    }
                    a = fabs(PolarAngle(f1, f2, f3, f4));
                    b = fabs(PolarAngle(f1, f2, f3));
                    if (node[k].pwt < FIRST_VES || (b < FIRST_ANGLE && a < FIRST_ANGLE))
                    {
                        ifirst = node[k].pw;
                        Nfirst = node[k].pwt + (a + b) / 2.0f;
                    }
                }
            }

            if (ifirst != -1 || ilast != -1)
            {
                if (Nlast < Nfirst)
                {
                    int sw = ifirst;
                    ifirst = ilast;
                    ilast = sw;
                }
                IPoint start = pindex[ifirst];
                MArray<int> &ind1 = index[start.y];
                int j, N = ind1.GetLen();
                BOOL fOk = start.x == 0 || start.x == N - 1;

                if (fOk)
                {
                    if (Nlast < Nfirst)
                        ind.Swap();
                    if (start.x == 0)
                        ind.Swap();
                    int N1 = ind.GetLen();
                    for (j = start.x + (start.x != 0); j < N; ++j)
                        pindex[ind1[j]].x += N1;
                    for (j = 0; j < N1; ++j)
                        pindex[ind[j]].Set(start.x + j + (start.x != 0), start.y);
                    ind1.Insert(start.x + (start.x != 0), ind);
                    ind.Clear();
                    ++kk;
                }
            } else
            { ;

            }

        }

    } while (kk);
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void Bmp2PolyLine::UpdateGraph(bool fInitMinus, bool fFullGraph)
{
    MArray<Vector2F> tmp_arr, tmp;
    MArray<int> pos;
    RBSet<Vector2F> nn;

    for (int k, l, i = 0, len = index.GetLen(); i < len; ++i)
        if ((l = index[i].GetLen()))
        {
            MArray<int> &id = index[i];
            tmp.SetLen(l);
            k = 0;
            for (int j = 0; j < l; ++j)
            {
                if (!nn.Find(arr[id[j]]))
                {
                    tmp[k++] = arr[id[j]];
                    nn.Add(arr[id[j]]);
                }
            }
            tmp.SetLen(k);
            if (k)
            {
                pos.Add() = tmp_arr.GetLen();
                tmp_arr += tmp;
            }
        }
    pos.Add() = tmp_arr.GetLen();
    InitEdgeGraph(tmp_arr, pos, fInitMinus, fFullGraph);
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
double Bmp2PolyLine::FindPathGamilton(int ii, int jj, float maxDistCalck, MArray<int> &_path1,
                                      MArray<int> *weight)
{
    ord.SetLen(arr.GetLen());
    bool *_ord = ord.GetPtr();
    memset(_ord, 0, arr.GetLen() * sizeof(bool));
    heap.Reset();
    int p = index[ii][jj];
    stack.PushFront(GNode((DINT) 0, p));
    _path1.SetLen(index.GetLen() * 2 + 1);
    int *path1 = _path1.GetPtr();
    int cnt = 0;
    int i, l;
    double weight1 = 0.0;
    GNode *padj = adj.GetPtr();
    int w1 = 0;
    while (!stack.IsEmpty())
    {
        GNode p = stack.PopFront();
        if (!_ord[p.pw])
        {
            weight1 += maxDistCalck < 0.0f ? p.pwt : p.pwt > maxDistCalck;
            _ord[p.pw] = true;
            GNode *node = padj + padj[p.pw].pw;
            if (weight)
            {
                if (p.pwt != 0)
                    (*weight)[w1++] = (int) sqrt(p.pwt);
            }
            path1[cnt] = p.pw;
            ++cnt;
            l = padj[p.pw].num;
            for (i = 0; i < l; ++i)
                if (!_ord[node[i].pw])
                    heap.Insert(p.pw, node[i].pw, node[i].pwt);
            while (!heap.IsEmpty())
            {
                GNodeW *nod = heap.Extract();
                stack.PushFront(GNode((float) nod->pwt, nod->pw));
            }
        }
    }
    _path1.SetLen(cnt);
    if (weight)
        weight->SetLen(w1);

    return weight1;
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void Bmp2PolyLine::UnionFromDist()
{
    float MaxCon = 10;
    int kk, count = 0;
    do
    {
        kk = 0;
        for (int i = 0, ln = index.GetLen(); i < ln; ++i)
        {
            if (!index[i].GetLen()) continue;

            MArray<int> &ind = index[i];
            int last = ind[ind.GetLen() - 1];
            GNode *node = adj.GetPtr() + adj[last].pw;
            float Nlast = MaxCon;
            int lk, k, ilast = -1;
            for (k = 0, lk = adj[last].num; k < lk; ++k)
            {
                if (node[k].pwt < Nlast && pindex[node[k].pw].y != i &&
                    index[pindex[node[k].pw].y].GetLen())
                {

                    ilast = node[k].pw;
                    Nlast = node[k].pwt;
                }

            }
            int first = ind[0];
            node = adj.GetPtr() + adj[first].pw;
            float Nfirst = MaxCon;
            int ifirst = -1;
            for (k = 0, lk = adj[first].num; k < lk; ++k)
            {
                if (node[k].pwt < Nfirst && pindex[node[k].pw].y != i &&
                    index[pindex[node[k].pw].y].GetLen())
                {

                    ifirst = node[k].pw;
                    Nfirst = node[k].pwt;
                }
            }

            if (ifirst != -1 || ilast != -1)
            {
                if (Nlast < Nfirst)
                {
                    int sw = ifirst;
                    ifirst = ilast;
                    ilast = sw;
                }
                IPoint start = pindex[ifirst];
                MArray<int> &ind1 = index[start.y];
                int j, N = ind1.GetLen();
                BOOL fOk = start.x == 0 || start.x == N - 1;

                if (fOk)
                {
                    if (Nlast < Nfirst)
                        ind.Swap();
                    if (start.x == 0)
                        ind.Swap();
                    int N1 = ind.GetLen();
                    for (j = start.x + (start.x != 0); j < N; ++j)
                        pindex[ind1[j]].x += N1;
                    for (j = 0; j < N1; ++j)
                        pindex[ind[j]].Set(start.x + j + (start.x != 0), start.y);
                    ind1.Insert(start.x + (start.x != 0), ind);
                    ind.Clear();
                    ++kk;
                }
            } else
            { ;

            }

        }
        if (!kk)
        {
            if (count < 4)
            {
                if (MaxCon < 200)
                {
                    MaxCon *= 2;
                    kk = 1;
                    ++count;
                } else
                    break;
            }
        } else
            count = 0;


    } while (kk);
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void Bmp2PolyLine::CreateGrehemPolygon(MArray<int> &wx, MArray<Vector2F> &vertex, MArray<int> &out)
{
    int t, i, k = 0;
    Vector2F *v = vertex.GetPtr();
    int *p = wx.GetPtr();
    int n = wx.GetLen();
    MArray<int> up, down;
    int p1 = p[0], p2 = p[n - 1];
    up.Add(p1);
    down.Add(p1);

    for (k = t = 1, i = 1; i < n; ++i)
    {
        if (i == n - 1 || direct(v[p1], v[p[i]], v[p2]) == 1)
        {
            while (k >= 2 && direct(v[up[k - 2]], v[up[k - 1]], v[p[i]]) != 1)
                up.SetLen(--k);
            up.Add(p[i]);
            ++k;
        }
        if (i == n - 1 || direct(v[p1], v[p[i]], v[p2]) == -1)
        {
            while (t >= 2 && direct(v[down[t - 2]], v[down[t - 1]], v[p[i]]) != -1)
                down.SetLen(--t);
            down.Add(p[i]);
            ++t;
        }
    }

    for (i = 0, n = (int) up.GetLen(); i < n; ++i)
        out.Add(up[i]);
    for (i = (int) down.GetLen() - 2; i > 0; --i)
        out.Add(down[i]);
    if (out.GetLen())
        out.Add() = out[0];
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void Bmp2PolyLine::Union()
{

    UnionFromAngle();
    UpdateGraph(false, false);
    int i, ll;
    for (i = 5; i <= 25; i += 5)
    {
        UnionFromDist(i);
        UpdateGraph(false, false);
    }
    UpdateGraph(false, true);
    UnionInsertFromDist(50);
    UpdateGraph(false, false);

//	UpdateGraph(false, false);
    //return;
    MDeq<GNode> stack;
    HeapVertexInv heap;
    MArray<int> path;
    double weight = 0;
    MArray<int> w(index.GetLen() * 2 + 1);
    FindPathGamilton(0, 0, -1, path, &w);

    int maxLen = sqrt(double(img->X * img->X + img->Y + img->Y));
    MArray<int> ml(maxLen, 0);

    for (i = 0, ll = w.GetLen(); i < ll; ++i)
        ++ml[w[i]];
    int check = 0;
    int count = 0;
    for (i = 0; check < 20 && i < ml.GetLen(); ++i)
        if (!ml[i])
            ++check;
        else
            check = 0;
    float step_dist = (float) (i + 5) / 4.0f;
    float dist = step_dist;
/*	for (int j  = 0; j < 4; ++j, dist +=  step_dist)
	{
		UnionFromDist(dist * dist);
		UpdateGraph(false, false);
	}

*/

    maxLen = i * i;
    UnionFromDist(maxLen);
    UpdateGraph(false, false);


    IPoint prev(-1, -1);
    int l = arr.GetLen();
/*	MVector<MArray<int> > wrem;
	int I, k = 0;
	wrem.Add().SetLen(l);
	for (i = 0, ll = path.GetLen(); i < ll; ++i)
	{
		I = wrem.GetLen() - 1;
		MArray<int> &ind = wrem[I];
		IPoint start = pindex[path[i]];
		MArray<int> &ind1 = index[start.y];
		if (prev.y == start.y)
		{
			if (!prev.x && start.x)
			{
				for (int j = 0, len = ind1.GetLen(); j < len; ++j, ++k)
					ind[k] = ind1[j];
			}
			else
			{
				for (int j = ind1.GetLen() - 1; j >= 0; --j, ++k)
					ind[k] = ind1[j];
			}

		}
		else
		{
			Vector2F f1 = arr[ind[ind.GetLen() - 1]];
			Vector2F f2 = arr[ind1[start.x]];
			if ((f1 - f2).LenQ() >= maxLen)
			{
				ind.SetLen(k);
				wrem.Add().SetLen(l);
				k = 0;
			}
		}
		prev = start;

	}
	wrem[wrem.GetLen() - 1].SetLen(k);
	index = wrem;
	UpdateGraph(false, false);
	//return;
*/
    //UnionInsertFromDist(maxLen);
//	UpdateGraph(false, false);
//	for (i = 0, ll = index.GetLen(); i < ll; ++i)
    //	if (index[i].GetLen() < 8)
    //		index[i].Clear();
    //UpdateGraph(false, false);
    FindPathGamilton(0, 0, -1, path, &w);

    maxLen = sqrt(double(img->X * img->X + img->Y + img->Y));
    ml.Set(0);
    for (i = 0, ll = w.GetLen(); i < ll; ++i)
        ++ml[w[i]];
    check = 0;
    count = 0;
    for (i = 0; check < 10 && i < ml.GetLen(); ++i)
        if (!ml[i])
            ++check;
        else
            check = 0;
    maxLen = (i - 10) * (i - 10);
    MArray<int> ord(arr.GetLen());
    for (int ii = 0, iil = index.GetLen(); ii < iil; ++ii)
    {
        for (int j = 0; j < 2; ++j)
        {
            ord.Set(0);
            int p = index[ii][j ? index[ii].GetLen() - 1 : 0];
            stack.PushFront(GNode((DINT) 0, p));
            MArray<int> path1(index.GetLen() * 2 + 1);
            int cnt = 0;
            int i, l;
            double weight1 = 0;
            GNode *padj = adj.GetPtr();
            while (!stack.IsEmpty())
            {
                GNode p = stack.PopFront();
                if (!ord[p.pw])
                {
                    weight1 += p.pwt; // >= maxLen; // 1024;
                    ord[p.pw] = 1;
                    GNode *node = padj + padj[p.pw].pw;
                    path1[cnt] = p.pw;
                    ++cnt;
                    l = padj[p.pw].num;
                    for (i = 0; i < l; ++i)
                        if (!ord[node[i].pw])
                            heap.Insert(p.pw, node[i].pw, node[i].pwt);
                    while (!heap.IsEmpty())
                    {
                        GNodeW *nod = heap.Extract();
                        stack.PushFront(GNode((float) nod->pwt, nod->pw));
                    }
                }
            }
            path1.SetLen(cnt);
            if (!ii || weight > weight1)
            {
                weight = weight1;
                path = path1;
            }
        }
    }

    prev.Set(-1, -1);
    l = arr.GetLen();
    MArray<int> ind(l);
    int k = 0;
    for (i = 0, ll = path.GetLen(); k != l && i < ll; ++i)
    {
        IPoint start = pindex[path[i]];
        MArray<int> &ind1 = index[start.y];
        if (prev.y == start.y)
        {
            if (!prev.x && start.x)
            {
                for (int i = 0, len = ind1.GetLen(); i < len; ++i, ++k)
                    ind[k] = ind1[i];
            } else
            {
                for (int i = ind1.GetLen() - 1; i >= 0; --i, ++k)
                    ind[k] = ind1[i];
            }

        }
        prev = start;
    }
    index.Clear();
    index.Add(ind);


}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void Bmp2PolyLine::InitEdgeGraph()
{
    MArray<Vector2F> arr_tmp;
    MArray<int> pos;
    InitVertexGraph();
    SmoothPoints(arr_tmp, pos);
    InitEdgeGraph(arr_tmp, pos, true, true);
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void Bmp2PolyLine::InitEdgeGraph(MArray<Vector2F> &arr_tmp, MArray<int> &pos, bool fInitMinus,
                                 bool fFullGraph)
{
    MHashMap<int, int> edge;
    MHashMap<int, int> edge_inv;
    MArray<GNode> edge_adj;
    MArray<Vector2F> tmp;

    arr.Clear();
    pindex.Clear();
    index.Clear();
    adj.Clear();
    tmp.SetLen(pos.GetLen() * 2);
    int i, len, k = 0;
    RBSet<Vector2F> test;
    Vector2F *at = arr_tmp.GetPtr();
    for (i = 0, len = pos.GetLen() - 1; i < len; ++i)
    {
        edge[pos[i]] = k;
        edge_inv[k] = pos[i];
        //	ASSERT(!test.Find(at[pos[i]]));
        tmp[k++] = at[pos[i]];
        test.Add(at[pos[i]]);
        if (at[pos[i + 1] - 1] != at[pos[i]])
        {
            edge_inv[k] = pos[i + 1] - 1;
            edge[pos[i + 1] - 1] = k;
            tmp[k++] = at[pos[i + 1] - 1];
            test.Add(at[pos[i + 1] - 1]);
        }
    }
    tmp.SetLen(k);
    if (fInitMinus)
    {
        MArray<Vector2F> minus;

        minus.SetLen(arr_tmp.GetLen());
        Vector2F *mt = minus.GetPtr();

        for (k = i = 0, len = arr_tmp.GetLen(); i < len; ++i)
        {
            if (!test.Find(at[i]))
            {
                mt[k] = at[i];
                test.Add(at[i]);
                ++k;
            }

        }
        minus.SetLen(k);
        BuilderVoron2D vor;
        vor.BuildGabriel(tmp, minus, img->X, img->Y, edge_adj);
    } else
    {
        MArray<Vector2F> minus;
        BuilderVoron2D vor;
        vor.Build(tmp, minus, img->X, img->Y, edge_adj);
    }
    arr.Swap(arr_tmp);
    pindex.SetLen(arr.GetLen());
    len = pos.GetLen() - 1;
    index.SetLen(len);
    adj.SetLen(arr.GetLen());
    adj.SetAddedRealSize(1024);
    IPoint *ppi = pindex.GetPtr();
    k = 0;
    GNode *new_n = edge_adj.GetPtr();
    int l;
    for (i = 0; i < len; ++i)
    {
        index[i].SetLen(l = (pos[i + 1] - pos[i]));
        int *ip = index[i].GetPtr();
        for (int j = 0; j < l; ++j, ++k)
        {
            ip[j] = k;
            ppi[k].Set(j, i);
            if (!j || j == l - 1)
            {
                int p = edge[k];
                GNode *node_n = new_n + new_n[p].pw;
                int ps = adj.GetLen();
                adj[k].Set(1 + new_n[p].num, ps);
                adj.SetLen(ps + adj[k].num);
                GNode *node = adj.GetPtr() + ps;
                if (fFullGraph)
                {
                    node->Set(0, !j ? k + 1 : k - 1);
                    ++node;
                } else
                {
                    node->Set(0, !j ? pos[i + 1] - 1 : pos[i]);
                    ++node;
                    //--adj[k].num;
                }
                for (int n = 0; n < new_n[p].num; ++n)
                {
                    int ps = edge_inv[node_n[n].pw];
                    if ((ps == pos[i + 1] - 1) || ps == pos[i])
                    {
                        --adj[k].num;
                        continue;
                    }
                    node->Set(node_n[n].pwt, ps);
                    ++node;
                }
            } else
            {
                if (fFullGraph)
                {
                    adj[k].Set(2, adj.GetLen());
                    adj.Add().Set(0, k - 1);
                    adj.Add().Set(0, k + 1);
                } else
                    adj[k].Set(0, 0);

            }
        }
    }
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void Bmp2PolyLine::InitVertexGraph()
{
    ::ClearBorderVoxels(img->Buffer, img->X, img->Y, 1, 0.3, FALSE);
    Make8Graph(adj, arr, img->Buffer, img->X, img->Y);
    MakePolyline();


}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void Bmp2PolyLine::SetLastPointFromCurves()
{
    for (int i = 0, l = index.GetLen(); i < l; ++i)
    {
        if (index[i].GetLen())
        {
            int first = index[i][0];
            int last = index[i][index[i].GetLen() - 1];
            GNode *node = adj.GetPtr() + adj[last].pw;
            for (int k = 0, lk = adj[last].num; k < lk; ++k)
                if (node[k].pw == first)
                {
                    index[i].Add() = first;
                    break;
                }
        }
    }
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
int Bmp2PolyLine::SortIndexFunc(MArray<int> *a, MArray<int> *b)
{
    if (!a->GetLen() && !b->GetLen()) return 0;
    if (!a->GetLen()) return -1;
    if (!b->GetLen()) return 1;

    if (arr[(*a)[0]] > arr[(*b)[0]] || arr[(*a)[0]] > arr[(*b)[b->GetLen() - 1]] ||
        arr[(*a)[a->GetLen() - 1]] > arr[(*b)[0]] ||
        arr[(*a)[a->GetLen() - 1]] > arr[(*b)[b->GetLen() - 1]])
        return 1;
    if (arr[(*b)[0]] > arr[(*a)[0]] || arr[(*b)[0]] > arr[(*a)[a->GetLen() - 1]] ||
        arr[(*b)[b->GetLen() - 1]] > arr[(*a)[0]] ||
        arr[(*b)[b->GetLen() - 1]] > arr[(*a)[a->GetLen() - 1]])
        return -1;

    return 0;
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void Bmp2PolyLine::helpSort(int *item, int left, int right)
{
    int i, j, y;
    i = left;
    j = right;
    MArray<int> *Data = index.GetPtr() + item[(i + j) / 2];


    do
    {
        while (SortIndexFunc(index.GetPtr() + item[i], Data) < 0 && i <= right)
            ++i;
        while (j >= 0 && SortIndexFunc(Data, index.GetPtr() + item[j]) < 0 && j >= left)
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

    if (left < j)
        helpSort(item, left, j);
    if (i < right)
        helpSort(item, i, right);
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void Bmp2PolyLine::SmoothPoints(MArray<Vector2F> &arr_tmp, MArray<int> &pos)
{

    MArray<Vector2F> tmp, tmp1;
    MArray<int> ipos;
    ipos.SetLen(index.GetLen());
    int *p = ipos.GetPtr();
    int l, i = 0, len = index.GetLen();
    for (; i < len; ++i)
        p[i] = i;

    if (len > 2)
        helpSort(ipos.GetPtr(), 0, len - 1);
    AproxLine<Vector2F, float> test;

    for (i = 0; i < len; ++i)
        if ((l = index[p[i]].GetLen()))
        {
            if (l > 3)
            {
                MArray<int> &id = index[p[i]];
                tmp.SetLen(l);
                for (int j = 0; j < l; ++j)
                    tmp[j] = arr[id[j]];

                pos.Add() = arr_tmp.GetLen();
                tmp1.SetLen(0);
                test.Exec(tmp, tmp1, 4);

                if (tmp[0] != tmp1[0])
                    tmp1[0] = tmp[0];
                if (tmp[l - 1] != tmp1[tmp1.GetLen() - 1])
                    tmp1[tmp1.GetLen() - 1] = tmp[l - 1];

                arr_tmp += tmp1;
            } else
            {
                MArray<int> &id = index[p[i]];
                tmp.SetLen(l);
                for (int j = 0; j < l; ++j)
                    tmp[j] = arr[id[j]];

                pos.Add() = arr_tmp.GetLen();
                arr_tmp += tmp;

            }

        }
    pos.Add() = arr_tmp.GetLen();

}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void Bmp2PolyLine::MakePolyline()
{
    MFifo<int> stack;
    MArray<bool> a_ord(arr.GetLen(), false);
    pindex.SetLen(arr.GetLen());
    bool *ord = a_ord.GetPtr();
    GNode *aFirst = adj.GetPtr();
    for (int i = 0, l = arr.GetLen(); i < l; ++i)
    {
        if (!ord[i])
        {
            ord[i] = 1;
            stack.Push(i);
            MArray<int> &ind = index.Add();
            while (!stack.IsEmpty())
            {
                int j = stack.Pop();
                GNode *node = aFirst + adj[j].pw;
                ind.Add(j);
                pindex[j].Set(ind.GetLen() - 1, index.GetLen() - 1);
                switch (adj[j].num)
                {
                    case 1:
                        if (!ord[node[0].pw])
                        {
                            stack.Push(node[0].pw);
                            ord[node[0].pw] = 1;
                        }
                        break;
                    case 2:
                    {
                        for (int k = 0, lk = adj[j].num; k < lk; ++k)
                            if (!ord[node[k].pw])
                            {
                                stack.Push(node[k].pw);
                                ord[node[k].pw] = 1;
                                break;
                            }
                        break;
                    }
                    case 3:
                    {
                        GNode *pnode[3] = {0, 0, 0};
                        int n1[3] = {-1, -1, -1};
                        int n = 0;
                        for (int k = 0, lk = adj[j].num; k < lk; ++k)
                            if (!ord[node[k].pw])
                            {
                                pnode[n] = node + k;
                                ++n;
                            }

                        if (n == 2)
                        {
                            GNode *node1 = aFirst + pnode[0]->pw;
                            GNode *node2 = aFirst + pnode[1]->pw;

                            n = 0;
                            if (node2->num == 2)
                            {
                                n = 1;
                                GNode *node3 = node1;
                                node1 = node2;
                                node2 = node3;
                            }
                            if (node1->num == 2)
                            {
                                GNode *node3 = aFirst + node1->pw;
                                if ((node3[0].pw == node2->pw || node3[1].pw == j) ||
                                    (node3[1].pw == node2->pw || node3[0].pw == j))
                                {
                                    ord[pnode[n]->pw] = 1;
                                    ord[pnode[!n]->pw] = 1;
                                    stack.Push(node[!n].pw);
                                }

                            }
                        }
                        break;
                    }
                    default:
                        for (int k = 0, lk = adj[j].num; k < lk; ++k)
                            if (!ord[node[k].pw])
                            {
                                stack.Push(node[k].pw);
                                ord[node[k].pw] = 1;
                                break;
                            }

                }
            }
        }
    }

}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void
Bmp2PolyLine::Make8Graph(MArray<GNode> &_adj, MArray<Vector2F> &_pos, int *Buffer, int X, int Y)
{
    struct Char2 {
        char x;
        char y;
    };
    const Char2 ind9[8] = {{-1, -1},
                           {-1, 0},
                           {-1, 1},
                           {0,  -1},
                           {0,  1},
                           {1,  -1},
                           {1,  0},
                           {1,  1}};
    RBMap<int, Vector2F> map;
    int L = X * Y;
    int x, y, num = 0, XY = X * Y;
    Vector2F c;

    MArray<Vector2F> tmp;
    tmp.SetAddedRealSize(X);
    for (y = 0; y < Y; ++y)
    {
        for (x = 0; x < X; ++x)
        {
            int bv = Buffer[x + y * X];
            if (bv != 0)
            {
                c.Set(x, y);
                tmp.Add(c);

                map[c] = num++;
            }
        }
    }
    /*	MArray<int> ii, out;
        tmp.SortIndex(ii, CmpVector2F);
        CreateGrehemPolygon(ii, tmp,  out);

        for (int i = 0, l = out.GetLen() - 1; i < l; ++i)
        {

            Vector2F f = tmp[out[i + 1]] - tmp[out[i]];
            Vector2F org = tmp[out[i]];
            float fl = f.Len();
            if (fl == 0.0) continue;
            float t =  1.0/fl;
            for (float ti = t; ti < 1.0; ti += t)
            {
                POINT c1 = org  + ti * f;
                c.Set(c1.x, c1.y);
                if (!map.Lookup(c))
                    map[c] = num++;
            }
        }*/
    tmp.Clear();
    _adj.Clear();
    _pos.Clear();
    _pos.SetAddedRealSize(1024);
    _adj.SetAddedRealSize(1024);
    _adj.SetLen(num + num * 8);

    _pos.SetLen(num);
    Vector2F cc, *pos = _pos.GetPtr();
    int l = num;
    GNode *adj = _adj.GetPtr();
    memset(adj, 0, sizeof(GNode) * _adj.GetLen());
    memset(pos, 0, sizeof(Vector2F) * num);
    int key;
    RBMap<int, Vector2F>::iterator it(map);
    for (RBData<int, Vector2F> *p = it.begin(); p; p = it.next())
    {
        c = p->key;
        adj[p->value].num = 0;
        adj[p->value].pw = l;
        pos[p->value] = c;
        for (int i = 0; i < 8; ++i)
        {

            cc.Set(c.x + ind9[i].x, c.y + ind9[i].y);
            if (cc.x >= 0.0f && c.y >= 0.0f && cc.x < X && c.y < Y && map.Lookup(cc, key))
            {
                adj[l].Set(p->value, key);
                ++l;
                ++adj[p->value].num;

            }
        }
    }
    _adj.SetLen(l);
}
