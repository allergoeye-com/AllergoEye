#ifndef __MakePolyLine__
#define __MakePolyLine__
#include "variable.h"
class MakePolyLine
{
	union tstFilter {
		struct {
			USHORT matr0 : 1;
			USHORT matr1 : 1;
			USHORT matr2 : 1;
			USHORT matr3 : 1;
			USHORT matr4 : 1;
			USHORT matr5 : 1;
			USHORT matr6 : 1;
			USHORT matr7 : 1;
			USHORT matr8 : 1;
			USHORT res : 7;
		};
		USHORT n;
	};

public:
	MVector<MArray<Vector2F> > res;
	
public:
	MakePolyLine(LPCTSTR path, Variable &result, BOOL fRecalc = FALSE);
	MakePolyLine(MVector<WString> &_path, MVector < Variable> &result, BOOL fRecalc = FALSE);
	~MakePolyLine() {}
protected:
	bool Init(char color, MArray<char>& _data, int X, int Y);
	void Make8Graph();
	void MakeLines(int n = 0, MArray<bool> * b = 0, int fFirstAdd = -1);
	void SmoothPoints(MArray<Vector2F>& arr_tmp, MArray<int>& pos);
	void helpSort(int* item, int left, int right);
	int SortIndexFunc(MArray<int>* a, MArray<int>* b);
	void InitEdgeGraph(MArray<Vector2F>& arr_tmp, MArray<int>& pos, bool fInitMinus, bool fFullGraph);
	void FilterBmpLoop(int build, BYTE* ptr, BYTE* out, int _width, int _height, BYTE color);

	MArray<char> data;
	MArray<IPoint> pindex;
	MVector<MArray<int> > index;


	int X, Y;
	MArray<GNode> adj;
	MArray<Vector2F> arr;
	MArray<bool> ord;
	MDeq<GNode> stack;
	HeapVertexInv heap;

};
#endif
