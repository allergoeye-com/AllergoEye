///////////////////////////////////////////////////
// MConstset.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////
#ifndef __M__CONST__SET__
#define __M__CONST__SET__
#include "mset.h"
#include "mconstmap.h"


template <class MAP_KEY, class MAP_ARRAY = MArray<MAP_KEY > >
class ConstSet : protected MAP_ARRAY {
public:
	enum OP {OP_NONE, OP_AND, OP_OR };	
public:	
	
	ConstSet (DINT i = 15) { node_ptr = 0; nodeLen = 0; iMaxRank = i; }
	ConstSet (const ConstSet &a) { *this = a; }
	ConstSet &operator = (const ConstSet &a){ tree = a.tree; 
							iMaxRank = a.iMaxRank; nodeLen = tree.GetLen(); node_ptr = tree.GetPtr(); MAP_ARRAY::operator = (a); return *this; }
	DINT MaxRank(DINT i = 0) { if (i > 0) iMaxRank = i; return iMaxRank; }
	~ConstSet () { }

	MAP_KEY *DataPtr() { return MAP_ARRAY::ptr; }
	ConstMapNode *TreePtr() { return tree.GetPtr(); }
	DINT TreeLen() { return tree.GetLen(); }
	DINT DataLen() { return MAP_ARRAY::GetLen(); }
	void Build(MAP_ARRAY &sortedArray);
	bool Find(MAP_KEY key);
	void GetSortedRBData(MAP_ARRAY &arr);
	MAP_KEY *Min();
	MAP_KEY *Max();
	void OR(ConstSet &in, MAP_ARRAY &out);
	bool AND(ConstSet &in, MAP_ARRAY &out);
	bool FindFirstIntersect(ConstSet &in, MAP_KEY &out);
protected:	
	bool FindFirst(MAP_KEY &res, ConstMapNode &node, MArray<DINT> &stack, ConstSet<MAP_KEY, MAP_ARRAY> *check, MAP_KEY *pMin, MAP_KEY *pMax);

	MArray<ConstMapNode > tree;
	ConstMapNode *node_ptr;
	DINT nodeLen, iMaxRank;
protected:	
	
	void GetSortedRBData(MAP_ARRAY &arr, ConstMapNode &node, MArray<DINT> &stack, DINT op = OP_NONE, ConstSet *check = 0, MAP_KEY *pMin = 0, MAP_KEY *pMax = 0);
	void Build(MAP_ARRAY &sortedArray, DINT y, MArray<DINT> &rank, MArray<ConstMapStack<MAP_KEY> > &stack);
	void Build(MArray<ConstMapNode > &sortedArray);
	
	
};
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class MAP_KEY, class MAP_ARRAY>
bool ConstSet<MAP_KEY, MAP_ARRAY>::FindFirstIntersect(ConstSet<MAP_KEY, MAP_ARRAY> &in, MAP_KEY &out)
{
	if (!MAP_ARRAY::ptr || !in.ptr) return 0;

	ConstSet<MAP_KEY, MAP_ARRAY> *a, *b;
	MAP_KEY *a1 = Min(); 
	MAP_KEY *b1 = in.Min();
	MAP_KEY aMin = max(*a1, *b1);
	if (in.Len == 1 && MAP_ARRAY::Len == 1)
	{
		if (*MAP_ARRAY::ptr == *in.ptr)
			out = *MAP_ARRAY::ptr;
		return *MAP_ARRAY::ptr == *in.ptr;
	}
	if (*a1 > *b1)
	{
	   b = this;
	   a = &in;
	}
	else
	{
	   a = this;
	   b = &in;
	}
	a1 = a->Max(); 
	b1 = b->Max();
	MAP_KEY aMax = min(*a1, *b1);

	ConstMapNode *root = a->node_ptr;
	MArray<DINT> stack(a->iMaxRank), stack1(a->iMaxRank);
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
				if (j >= a->nodeLen) 
					break;
				i = j;
			}
			while(true);
		}
		i = *(--pStack);
		//BoolWarning(i < a->nodeLen); 

		if ((root + i)->len)
		{
			if (*(a->ptr + (root + i)->key) > aMax)
				break;
			if (*(a->ptr + (root + i)->key_max) >= aMin)
				if (a->FindFirst(out, *(root + i), stack1,  b, &aMin, &aMax))
					return true;
		}
		j = ((i + 1) << 1) - 1;
		if((fCheck = (j < a->nodeLen)))
			i = ((i + 1) << 1) - 1;
	}
	while (fCheck || pStack > test);

   return false;

}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class MAP_KEY, class MAP_ARRAY>
bool ConstSet<MAP_KEY, MAP_ARRAY>::AND(ConstSet<MAP_KEY, MAP_ARRAY> &in, MAP_ARRAY &out)
{
	if (!MAP_ARRAY::ptr || !in.ptr) return 0;

	ConstSet<MAP_KEY, MAP_ARRAY> *a, *b;
	MAP_KEY *a1 = Min(); 
	MAP_KEY *b1 = in.Min();
	MAP_KEY aMin = max(*a1, *b1);
	if (in.Len == 1 && MAP_ARRAY::Len == 1)
	{
		if (*MAP_ARRAY::ptr == *in.ptr)
			out.Add(*MAP_ARRAY::ptr);
		return out.GetLen() != 0;
	}
	if (*a1 > *b1)
	{
	   b = this;
	   a = &in;
	}
	else
	{
	   a = this;
	   b = &in;
	}
	a1 = a->Max(); 
	b1 = b->Max();
	MAP_KEY aMax = min(*a1, *b1);

	ConstMapNode *root = a->node_ptr;
	MArray<DINT> stack(a->iMaxRank), stack1(a->iMaxRank);
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
				if (j >= a->nodeLen) 
					break;
				i = j;
			}
			while(true);
		}
		i = *(--pStack);
		//BoolWarning(i < a->nodeLen); 

		if ((root + i)->len)
		{
			if (*(a->ptr + (root + i)->key) > aMax)
				break;
			if (*(a->ptr + (root + i)->key_max) >= aMin)
				a->GetSortedRBData(out, *(root + i), stack1, OP_AND, b, &aMin, &aMax);
		}
		j = ((i + 1) << 1) - 1;
		if((fCheck = (j < a->nodeLen)))
			i = ((i + 1) << 1) - 1;
	}
	while (fCheck || pStack > test);

   return out.GetLen() != 0;
}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class MAP_KEY, class MAP_ARRAY>
void ConstSet<MAP_KEY, MAP_ARRAY>::Build(MArray<ConstMapNode > &sortedArray)
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
	node_ptr = tree.GetPtr();
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class MAP_KEY, class MAP_ARRAY>
void ConstSet<MAP_KEY, MAP_ARRAY>::Build(MAP_ARRAY &sortedArray)
{
	
	MAP_ARRAY::SetLen(0);
	tree.SetLen(0);
	node_ptr = 0; 
	nodeLen = 0;
	DINT j, n, y, n1, i, size = sortedArray.GetLen();
	MAP_KEY *pArr = sortedArray.GetPtr();
	MArray<ConstMapNode > tmp_tree;
	MArray<ConstMapStack<MAP_KEY> > stack;
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
			stack.SetLen(y + 1);
			Build(tmp, y, rank, stack);
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
	}
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class MAP_KEY, class MAP_ARRAY>
inline void ConstSet<MAP_KEY, MAP_ARRAY>::Build(MAP_ARRAY &sortedArray, DINT y, MArray<DINT> &rank, MArray<ConstMapStack<MAP_KEY> > &stack)
{
	DINT prevLen = MAP_ARRAY::Len;
	DINT n1 = rank[rank.GetLen() - 1];
	MAP_ARRAY::SetLen(prevLen + n1);
	MAP_KEY *p = MAP_ARRAY::ptr;
	MAP_ARRAY::ptr = p + prevLen;
	MAP_ARRAY::Len = sortedArray.GetLen();
	n1 = 0;
	SortStaticMap(MAP_ARRAY::ptr, rank.GetPtr(), n1, sortedArray.GetPtr(), sortedArray.GetLen(), y, stack);
	MAP_ARRAY::ptr = p;
	MAP_ARRAY::Len += prevLen;

}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class MAP_KEY, class MAP_ARRAY>
inline bool ConstSet<MAP_KEY, MAP_ARRAY>::Find(MAP_KEY key)
{
	DINT i = 0;
	while (i < nodeLen && (*(MAP_ARRAY::ptr + (node_ptr + i)->key) > key || *(MAP_ARRAY::ptr + (node_ptr + i)->key_max) < key))
		i = *(MAP_ARRAY::ptr + (node_ptr + i)->pos) < key ? ((i + 1) << 1) - 1 : (i + 1) << 1;
	if (i < nodeLen && (node_ptr + i)->len)
	{
		MAP_KEY *_ptr = MAP_ARRAY::ptr + (node_ptr + i)->pos;
		DINT len = (node_ptr + i)->len;
		i = 0;
		while (i < len && *(_ptr + i) != key)
			i = *(_ptr + i) < key ? ((i + 1) << 1) - 1 : (i + 1) << 1;
		return i < len;
	}
	return false;
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class MAP_KEY, class MAP_ARRAY>
void ConstSet<MAP_KEY, MAP_ARRAY>::GetSortedRBData(MAP_ARRAY &arr)
{
	if (!MAP_ARRAY::Len) return;
//	arr.SetLen(arr.GetLen() + Len);
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
template <class MAP_KEY, class MAP_ARRAY>
void ConstSet<MAP_KEY, MAP_ARRAY>::GetSortedRBData(MAP_ARRAY &arr, ConstMapNode &node, MArray<DINT> &stack, DINT op, ConstSet<MAP_KEY, MAP_ARRAY> *check, MAP_KEY *pMin, MAP_KEY *pMax)
{
	DINT prevLen = arr.GetLen();
	arr.SetLen(prevLen + node.len);
	MAP_KEY *root = MAP_ARRAY::ptr + node.pos;
	MAP_KEY *pArr = arr.GetPtr() + prevLen;
	MAP_KEY *pArrT = pArr;
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
		//BoolWarning(i < node.len); 
		if (op == OP_NONE)
			*pArr++ = *(root + i);
		else
		if (op == OP_AND)
		{
			if (*(root + i) <= *pMax && *(root + i) >= *pMin)
			{
				if (check->Find(*(root + i)))
					*pArr++ = *(root + i);
			}
		}
		
		j = ((i + 1) << 1) - 1;
		if((fCheck = (j < node.len)))
			i = ((i + 1) << 1) - 1;
	}
	while (fCheck || pStack > test);
	arr.SetLen(prevLen + (pArr - pArrT));
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class MAP_KEY, class MAP_ARRAY>
bool ConstSet<MAP_KEY, MAP_ARRAY>::FindFirst(MAP_KEY &res, ConstMapNode &node, MArray<DINT> &stack, ConstSet<MAP_KEY, MAP_ARRAY> *check, MAP_KEY *pMin, MAP_KEY *pMax)
{
	MAP_KEY *root = MAP_ARRAY::ptr + node.pos;

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
		//BoolWarning(i < node.len); 
		if (*(root + i) <= *pMax && *(root + i) >= *pMin)
		{
			if (check->Find(*(root + i)))
			{
				res = *(root + i);
				return true;
			}
		}
		
		j = ((i + 1) << 1) - 1;
		if((fCheck = (j < node.len)))
			i = ((i + 1) << 1) - 1;
	}
	while (fCheck || pStack > test);
	return false;


}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class MAP_KEY, class MAP_ARRAY>
inline MAP_KEY *ConstSet<MAP_KEY, MAP_ARRAY>::Min()
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
template <class MAP_KEY, class MAP_ARRAY>
inline MAP_KEY *ConstSet<MAP_KEY, MAP_ARRAY>::Max()
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
template <class MAP_KEY, class MAP_ARRAY>
void ConstSet<MAP_KEY, MAP_ARRAY>::OR(ConstSet<MAP_KEY, MAP_ARRAY> &in, MAP_ARRAY &out)
{
	if (!MAP_ARRAY::ptr && !in.ptr) return;
	if (!MAP_ARRAY::ptr)
	{
		in.GetSortedRBData(out);
		return;
	}
	else
	if (!in.ptr)
	{
		GetSortedRBData(out);
		return;
	}
	RBSet<MAP_KEY, RBCmp<MAP_KEY>, AllocatorSimple<RBKeyNode<MAP_KEY> > > set;
	DINT i = MAP_ARRAY::Len;
	MAP_KEY *p = MAP_ARRAY::ptr;
	while(i--) 
		set.Add(*p++); 
	i = in.Len;
	p = in.ptr;
	while(i--) 
		set.Add(*p++);
	set.GetSortedRBData(out);
}
#endif
