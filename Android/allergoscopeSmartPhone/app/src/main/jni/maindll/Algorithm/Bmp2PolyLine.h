/*
 * Bmp2PolyLine.h
 *
 *  Created on: Oct 15, 2016
 *      Author: alex
 */

#ifndef MAINDLL_MDC_BMP2POLYLINE_H_
#define MAINDLL_MDC_BMP2POLYLINE_H_

#include "FImage.h"
#include "BuilderVoron2D.h"
#include "AproxLine.h"

class Bmp2PolyLine
{
public:
	Bmp2PolyLine();
	~Bmp2PolyLine();
	void Build(CMathImage *_img, MVector<MArray<POINT> > &res);
protected:
	void InitEdgeGraph();
	void InitEdgeGraph(MArray<Vector2F> &arr_tmp,  MArray<int> &pos, bool fInitMinus, bool fFullGraph);
	void InitVertexGraph();
	void MakePolyline();
	void Union();
	void UnionFromDist();
	void UnionFromDist(float dist);
	void UnionFromAngle();
	void UpdateGraph(bool fInitMinus, bool fFullGraph);
	double FindPathGamilton(int ii, int jj, float maxDistCalck, MArray<int> &path1, MArray<int> *weight = 0);
	void UnionInsertFromDist(float MaxCon);
	void CreateGrehemPolygon(MArray<int> &wx, MArray<Vector2F> &vertex,  MArray<int> &out);


	void SmoothPoints(MArray<Vector2F> &arr_tmp, MArray<int> &pos);
	void SetLastPointFromCurves();
	void helpSort(int *item, int left, int right);
	int SortIndexFunc(MArray<int> *a, MArray<int> *b);
	void Make8Graph(MArray<GNode> &_adj, MArray<Vector2F> &_pos, int *Buffer, int X, int Y);

	void PutRes(MVector<MArray<POINT> > &dat);


protected:
	CMathImage *img;
	MArray<IPoint> pindex;
	MVector<MArray<int> > index;
public:
	MArray<GNode> adj;
	MArray<Vector2F> arr;
private :
	//from path gamilton
	MArray<bool> ord;
	MDeq<GNode> stack;
	HeapVertexInv heap;



};

#endif /* MAINDLL_MDC_BMP2POLYLINE_H_ */
