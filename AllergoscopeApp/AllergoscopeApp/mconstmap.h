///////////////////////////////////////////////////
// mconstmap.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////
#ifndef __M_CONST_MAP__
#define	__M_CONST_MAP__
#include "buffer.h"
#include "rbmap.h"
#pragma pack(push) //NADA
#pragma pack(4)

struct ConstMapNode {
	DINT key;
	DINT key_max;
	DINT pos;
	DINT len;

};
#pragma pack(pop)

template <class T >
struct ConstMapStack {
	DINT pos;
	DINT level;
	DINT offset;
	T *ptr;
};
inline DINT CalcRank(DINT n, MArray<DINT> &rank, DWORD iMaxRank = 0xFFFFFFFF)
{
	DWORD y = 0;
	while (n >>= 1)
		++y;
	if (y > iMaxRank) y = iMaxRank;
	for (DWORD i = 0; i <= y + 1; ++i)
		rank.Add() = (DINT)((1 << i) - 1);
	return (DINT)y;
}
template <class T>
void SortStaticTree(T *node_ptr, DINT *col, DINT k, T *arr, DINT num, DINT maxRank)
{
	
	if (k <= maxRank)
	{
		if (num > 0)
		{
			DINT n1, c, n = num >> 1;
			if (num >= 2 && (num & 1))
			{
				
				*(node_ptr + *(col + k)) = *(arr + n);
					
				c = n + 1;
				n1 = n;
				
			}
			else  
			{	bool f = !n; 
				if (!n) n = 1;
				*(node_ptr + *(col + k)) = *(arr + n - 1);
				if (f)
					n1 = c = n = 0;
				else
				{
					n1 = n - 1;
					c = n;
				}

			
			}
			++col[k];
			SortStaticTree(node_ptr, col, k + 1, arr + c, n, maxRank);
			SortStaticTree(node_ptr, col, k + 1, arr, n1, maxRank);
			
		}
		else
		{
			(node_ptr + *(col + k))->len = 0;
			++col[k];
		}
	}
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class T>
void SortStaticMap(T *ptr, DINT *col, DINT k, T *arr, DINT num, DINT maxRank, MArray<ConstMapStack<T> > &stack)
{
	ConstMapStack<T> *startS = stack.GetPtr();
	ConstMapStack<T> *pS = startS;
	DINT n1, c, n;
	bool f;
	do
	{
		while(num)
		{
			n = num >> 1;
			if (num >= 2 && (num & 1))
			{
				
				*(ptr + *(col + k)) = *(arr + n);
				c = n + 1;
				n1 = n;
				
			}
			else  
			{	f = !n; 
				if (!n) n = 1;
				*(ptr + *(col + k)) = *(arr + n - 1);
				if (f)
					n1 = c = n = 0;
				else
				{
					n1 = n - 1;
					c = n;
				}

			
			}
			++ (*(col + k));
			++k;
			if (k > maxRank)
				break;
			pS->level = k;
			pS->pos = n1;
			pS->ptr = arr;
			++pS;
			arr += c;
			num = n;
		}
		if (pS == startS)
			break;
		--pS;
		k = pS->level;
		num = pS->pos;
		arr = pS->ptr;
	}while(num);

}


template <class MAP_VALUE, class MAP_KEY, class MAP_ARRAY = MArray<RBData<MAP_VALUE, MAP_KEY> > >
class ConstMap : protected MAP_ARRAY {
public:
	enum OP {OP_NONE, OP_AND, OP_OR };	
public:	
	
	ConstMap (DINT i = 15) { node_ptr = 0; nodeLen = 0; iMaxRank = i;}
	ConstMap (ConstMap &a) { *this = a; }
	ConstMap &operator = (ConstMap &a){ tree = a.tree; 
							iMaxRank = a.iMaxRank; nodeLen = tree.GetLen(); node_ptr = tree.GetPtr(); MAP_ARRAY::operator = (a); return *this; }
	DINT MaxRank(DINT i = 0) { if (i > 0) iMaxRank = i; return iMaxRank; }
	~ConstMap () {  }
	RBData<MAP_VALUE, MAP_KEY> *DataPtr() { return MAP_ARRAY::ptr; }
	ConstMapNode *TreePtr() { return tree.GetPtr(); }
	DINT TreeLen() const  { return tree.GetLen(); }
	DINT DataLen() const { return MAP_ARRAY::GetLen(); }
	void Build(MAP_ARRAY &sortedArray);
	void Clear() { tree.Clear(); MAP_ARRAY::Clear(); node_ptr = 0; nodeLen = 0; }
	void GetSortedRBData(MAP_ARRAY &arr);
	bool Lookup (MAP_KEY i, MAP_VALUE &p) 
	{ 
		MAP_VALUE *v = operator [] (i);
		 if (v) 
			p = *v; 
		return v != 0;
	}

	bool Lookup (MAP_KEY i, MAP_VALUE **p) 
	{ 
		MAP_VALUE *v = operator [] (i);
		 if (p && v) 
			*p = v; 
		return v != 0;
	}
	MAP_VALUE *operator [] (MAP_KEY key)
	{
		if (MAP_ARRAY::Len)
		{
			DINT i = 0;
			while (i < nodeLen && ((MAP_ARRAY::ptr + (node_ptr + i)->key)->key > key || (MAP_ARRAY::ptr + (node_ptr + i)->key_max)->key < key))
				i = (MAP_ARRAY::ptr + (node_ptr + i)->pos)->key < key ? ((i + 1) << 1) - 1 : (i + 1) << 1;
			if (i < nodeLen && (node_ptr + i)->len)
			{
				RBData<MAP_VALUE, MAP_KEY> *_ptr = MAP_ARRAY::ptr + (node_ptr + i)->pos;
				DINT len = (node_ptr + i)->len;
				i = 0;
				while (i < len && (_ptr + i)->key != key)
					i = (_ptr + i)->key < key ? ((i + 1) << 1) - 1 : (i + 1) << 1;
				return i < len ? &(_ptr + i)->value : 0;
			}
		}
		return 0;
	}

	RBData<MAP_VALUE, MAP_KEY> *Min();
	RBData<MAP_VALUE, MAP_KEY> *Max();
protected:	
	MArray<ConstMapNode > tree;
	ConstMapNode *node_ptr;
	DINT nodeLen, iMaxRank;
protected:	
	void GetSortedRBData(MAP_ARRAY &arr, ConstMapNode &node, MArray<DINT> &stack);
	void Build(MAP_ARRAY &sortedArray, DINT y, MArray<DINT> &rank, MArray<ConstMapStack<RBData<MAP_VALUE, MAP_KEY>  > > &stack);
	void Build(MArray<ConstMapNode > &sortedArray);
	
	
};
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class MAP_ARRAY>
void ConstMap<MAP_VALUE, MAP_KEY, MAP_ARRAY>::Build(MArray<ConstMapNode > &sortedArray)
{
	DINT n = sortedArray.GetLen();
	MArray<DINT> rank;
	DINT y = CalcRank(n, rank);
	n = rank[rank.GetLen() - 1];
	tree.SetLen(n);
	node_ptr = tree.GetPtr();
	nodeLen = n;
	n = 0;
	SortStaticTree(node_ptr, rank.GetPtr(), n, sortedArray.GetPtr(), sortedArray.GetLen(), y);
//	CString n1;
//	n1.Format("tree interval rank = %d",  y);
//	MessageBox(0, n1, n1, MB_OK);
}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class MAP_ARRAY>
void ConstMap<MAP_VALUE, MAP_KEY, MAP_ARRAY>::Build(MAP_ARRAY &sortedArray)
{
	
	MAP_ARRAY::SetLen(0);
	tree.SetLen(0);
	node_ptr = 0; 
	nodeLen = 0;

	DINT j, n, y, n1, i, size = sortedArray.GetLen();
	RBData<MAP_VALUE, MAP_KEY> *pArr = sortedArray.GetPtr();
	MArray<ConstMapNode > tmp_tree;
	//DINT m = 0;
	//DINT max_n = 0;
	MArray<ConstMapStack<RBData<MAP_VALUE, MAP_KEY>  > > stack;
	if (size)
	{
		MAP_ARRAY::SetLen(size);
		MAP_ARRAY::Len = 0;
		MArray<DINT> rank;
		MAP_ARRAY tmp;
		while (size)
		{	
			n = size;
			rank.SetLen(0);			
			y = CalcRank(n, rank, iMaxRank);
			DINT prevLen = MAP_ARRAY::Len;
			n1 = rank[rank.GetLen() - 1];
		
			
			if (n1 > size)
			{
				--y;
				n1 >>= 1;
				rank.SetLen(rank.GetLen() - 1);
			}
			tmp.Attach(pArr, n1);
			ConstMapNode &node = tmp_tree.Add();
			node.pos = MAP_ARRAY::Len;
	//		m = max(y, m);
			stack.SetLen(y + 1);
			Build(tmp, y, rank, stack);
	//		max_n = max(max_n, n1);

	//		BoolWarning(n1 && Len - node.pos == n1);
			node.len = MAP_ARRAY::Len - node.pos;
			
			i = 0;
			while (true)
			{
				j = ((i + 1) << 1) - 1;
				if (j >= node.len) 
				{
					node.key_max = i + node.pos;
					break;
				}
				i = j;
			 }
			i = 0;
			while (true)
			{
				j = ((i + 1) << 1);
				if (j >= node.len) 
				{
					node.key = i + node.pos;
					break;
				}
				i = j;
			 }
			tmp.Detach();
			pArr += n1;
			size -= n1;
		}
		Build(tmp_tree);
	//	CString n;
	//	n.Format("tmp_tree num = %d max rank -> %d, max n = %d",  tmp_tree.GetLen(), m, max_n);
	//MessageBox(0, n, n, MB_OK);

	}
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class MAP_ARRAY>
inline void ConstMap<MAP_VALUE, MAP_KEY, MAP_ARRAY>::Build(MAP_ARRAY &sortedArray, DINT y, MArray<DINT> &rank, MArray<ConstMapStack<RBData<MAP_VALUE, MAP_KEY>  > > &stack)
{
	DINT prevLen = MAP_ARRAY::Len;
	DINT n1 = rank[rank.GetLen() - 1];
	MAP_ARRAY::SetLen(prevLen + n1);
	RBData<MAP_VALUE, MAP_KEY> *p = MAP_ARRAY::ptr;
	MAP_ARRAY::ptr = p + prevLen;
	MAP_ARRAY::Len = sortedArray.GetLen();
	n1 = 0;
	SortStaticMap(MAP_ARRAY::ptr, rank.GetPtr(), n1, sortedArray.GetPtr(), sortedArray.GetLen(), y, stack);
/*		nume = pe - empty.GetPtr();
	pe = empty.GetPtr();
	while(nume--)
	{
		n1 = *pe; 
		if (!(n1 & 1))
			--n1;
		n1 >>= 1;
		if (n1 < Len)
			*(ptr + *pe) = *(ptr + (n1 >> 1)); 
		++pe;
	} */
	MAP_ARRAY::ptr = p;
	MAP_ARRAY::Len += prevLen;

}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class MAP_ARRAY>
inline RBData<MAP_VALUE, MAP_KEY> *ConstMap<MAP_VALUE, MAP_KEY, MAP_ARRAY>::Min()
{
	DINT j, i = 0;
	ConstMapNode *node = node_ptr;
	if (node)
		while (true)
		{
			j = ((i + 1) << 1);
			if (j >= nodeLen || !(node_ptr + j)->len) 
				return MAP_ARRAY::ptr + (node_ptr + i)->key;
			i = j;
		 }
	return 0;

}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class MAP_ARRAY>
inline RBData<MAP_VALUE, MAP_KEY> *ConstMap<MAP_VALUE, MAP_KEY, MAP_ARRAY>::Max()
{
	DINT j, i = 0;
	ConstMapNode *node = node_ptr;
	if (node)
		while (true)
		{
			j = ((i + 1) << 1) - 1;
			if (j >= nodeLen || !(node_ptr + j)->len) 
				return MAP_ARRAY::ptr + (node_ptr + i)->key_max;
			i = j;
		 }
	return 0;

}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class MAP_ARRAY>
void ConstMap<MAP_VALUE, MAP_KEY, MAP_ARRAY>::GetSortedRBData(MAP_ARRAY &arr)
{
	if (!MAP_ARRAY::Len) return;
	arr.SetLen(0);
	ConstMapNode *root = node_ptr;
	MArray<DINT> stack(iMaxRank), stack2(iMaxRank);
	DINT *test, *pStack = stack.GetPtr();
	test = pStack;
	bool fCheck = 1;
	DINT j, i = 0;
	do
	{
		if (fCheck)
		{
			do
			{
				*pStack++ = i;
				j = ((i + 1) << 1);
				if (j >= nodeLen) 
					break;
				i = j;
			}
			while(true);
		}
		i = *(--pStack);
		//BoolWarning(i < nodeLen); 
		if ((root + i)->len)
			GetSortedRBData(arr, *(root + i), stack2);
		j = ((i + 1) << 1) - 1;
		if((fCheck = (j < nodeLen)))
			i = ((i + 1) << 1) - 1;
	}
	while (fCheck || pStack > test);
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class MAP_ARRAY>
void ConstMap<MAP_VALUE, MAP_KEY, MAP_ARRAY>::GetSortedRBData(MAP_ARRAY &arr, ConstMapNode &node, MArray<DINT> &stack)
{
	DINT prevLen = arr.GetLen();
	arr.SetLen(prevLen + node.len);
	RBData<MAP_VALUE, MAP_KEY> *root = MAP_ARRAY::ptr + node.pos;
	RBData<MAP_VALUE, MAP_KEY> *pArr = arr.GetPtr() + prevLen;
	RBData<MAP_VALUE, MAP_KEY> *pArrT = pArr;
	DINT *test, *pStack = stack.GetPtr();
	test = pStack;
	bool fCheck = 1;
	DINT j, i = 0;
	do
	{
		if (fCheck)
		{
			do
			{
				*pStack++ = i;
				j = ((i + 1) << 1);
				if (j >= node.len) 
					break;
				i = j;
			}
			while(true);
		}
		i = *(--pStack);
		//BoolWarning(i < node.Len); 
		*pArr++ = *(root + i);
		
		j = ((i + 1) << 1) - 1;
		if((fCheck = (j < node.len)))
			i = ((i + 1) << 1) - 1;
	}
	while (fCheck || pStack > test);
	arr.SetLen(prevLen + (pArr - pArrT));


}

#endif


