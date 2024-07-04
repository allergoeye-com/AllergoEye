#ifndef __MultiArRBSet__
#define __MultiArRBSet__
#ifndef _PEXPORT 
#define _PEXPORT 
#endif
#include "ArRBMap.h"
#pragma pack (push)
#pragma pack (8)
template <class MAP_KEY, class CMP = RBCmp<MAP_KEY> >
class ParentMultiSet {
public:
	ParentMultiSet() { m_fNewDelele = 0; m_fKeyFromCompareFunc = 0; }
	virtual ~ParentMultiSet() {}
	virtual void AllocNode(DINT *node) = 0;
	virtual void FreeNode(DINT node) = 0;

	MArray<MAP_KEY> Data;
	MArray<ArRBTreeNode> Tree;
	ArHeap bDeleted; 

	MAP_KEY *bData;
	ArRBTreeNode *bTree;

	bool m_fKeyFromCompareFunc;
	CMP cmp;
	bool m_fNewDelele;
	
};
template <class MAP_KEY, class CMP = RBCmp<MAP_KEY> >
class InternalSet {

public:
	InternalSet(ParentMultiSet<MAP_KEY, CMP> *parent, MAP_KEY *&aData,  ArRBTreeNode *&aTree):bData(aData), bTree(aTree), Parent(parent)
	{ 
		m_iNum = 0;
		pRoot = -1;
	};
	~InternalSet() {  };
	bool operator [] (MAP_KEY key) { DINT p = Search(pRoot, key); return p != -1; }
	bool Find (MAP_KEY key) { DINT p = Search(pRoot, key); return p != -1; }
	bool Add(MAP_KEY key) { return Add(key, 0); }
	void Delete(MAP_KEY key) { DINT p = Search(pRoot, key); if (p != -1) DeleteNode(p); }
	DINT GetLen() { return m_iNum; }
	void GetSortedRBData(MArray<MAP_KEY > &arr);
	DINT Rank() 	{ DINT y = 0; Rank(y, pRoot);  return y; }
	DINT Len() { return m_iNum; }
protected:
	void Rank(DINT &y, DINT node);
	void AllocNode(DINT *node);
	void FreeNode(DINT  node);
	void DeleteNode (DINT  node);

	bool Add(MAP_KEY &key, DINT *_node);

	void DeleteFix(DINT node);
	DINT Search(DINT  node, MAP_KEY &key);
	bool Insert(MAP_KEY &key, DINT *node);
	void LRotate (DINT node);
	void RRotate (DINT node);
public:
	DINT m_iNum;
	DINT pRoot; 
	MAP_KEY *&bData;
	ArRBTreeNode *&bTree;
	ParentMultiSet<MAP_KEY, CMP> *Parent;
	
class iterator 
{ 
	InternalSet &mp;
	DINT t;
	DINT node;
	MArray<DINT> stack;
	DINT *test, *pStack;
	bool fCheck;
	DINT len;
public:
	iterator(InternalSet &g):mp(g) { t = -1; }
	MAP_KEY *begin()
	{ 
		if (mp.pRoot == -1) return 0;	
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
			node = mp.bTree[node].left;
		}
		while(node != -1);
		node = *(--pStack);
		t = node;
		if((fCheck = mp.bTree[node].right != -1))
			node = mp.bTree[node].right;
		return t == -1 ? 0 : mp.bData + t; 
	}
	MAP_KEY *next()
	{ 
		if (t == -1)
			return begin();
		if (t != -1 && (fCheck || pStack > test))
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
					node = mp.bTree[node].left;
				}
				while(node != -1);
			}
			node = *(--pStack);
			t = node;
			if((fCheck = mp.bTree[node].right != -1))
			node = mp.bTree[node].right;
		}
		else
			t = -1;

		return t == -1 ? 0 : mp.bData + t; 
	}
	bool end()
	{ return t == -1; }
};
	

};
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
template <class MAP_KEY, class CMP>
void InternalSet<MAP_KEY, CMP>::GetSortedRBData(MArray<MAP_KEY > &arr)
{
	if (pRoot == -1) return;	
	DINT node = pRoot;
	DINT index = 0, len = 16;
	MAP_KEY *pArr = arr.GetPtr();
	MArray<DINT> stack(16);
	DINT *test, *pStack = stack.GetPtr();
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
				node = bTree[node].left;
			}
			while(node != -1);
		}
		node = *(--pStack);
		*pArr++ = bData[node];
		if((fCheck = bTree[node].right != -1))
		node = bTree[node].right;
	}
	while (fCheck || pStack > test);

}

//-------------------------------------
//
//-------------------------------------
template <class MAP_KEY, class CMP>
void InternalSet<MAP_KEY, CMP>::Rank(DINT &y, DINT node)
{
	if (node == -1)
		return;
	++y;
	DINT l = y;
	DINT r = y;
	Rank(l, bTree[node].left);
	Rank(r, bTree[node].right);
	y = max(l, r);
}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
template <class MAP_KEY, class CMP>
inline void InternalSet<MAP_KEY, CMP>::AllocNode(DINT *node)
{
	Parent->AllocNode(node);
	++m_iNum;
}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
template <class MAP_KEY, class CMP>
inline void InternalSet<MAP_KEY, CMP>::FreeNode(DINT node)
{
	Parent->FreeNode(node);
	--m_iNum;
}
template <class MAP_KEY, class CMP>
bool InternalSet<MAP_KEY, CMP>::Insert(MAP_KEY &key, DINT *node)
{

	if (pRoot == -1)
	{
		AllocNode(&pRoot);
		bData[pRoot] = key;
		*node = pRoot;
		return true;
	}

	DINT nodeParent = -1;
	DINT temp = pRoot;
	DINT n = 0;
	
	if (Parent->m_fKeyFromCompareFunc)
	{
		while (temp != -1)
		{
			nodeParent = temp;
			if (!(n = Parent->cmp.compare(key, bData[temp])))
			{
				*node = temp;
				return false;
			}
			if (n < 0)
				temp = bTree[temp].left;
			else
				temp = bTree[temp].right;
		}
		n = n < 0;
	}
	else
		while (temp != -1)
		{
			nodeParent = temp;
			MAP_KEY &rbdat = bData[temp];
			if ((n = (key < rbdat)))
				temp = bTree[temp].left;
			else
			if (key > rbdat)
				temp = bTree[temp].right;
			else 
			{
				*node = temp;
				return false;
			}
		}
	if (*node == -1)
	{
		AllocNode(node);
		 bData[*node] = key;
	}
	bTree[*node].color = _RED;
	bTree[*node].parent = nodeParent;
	if (n)
		bTree[nodeParent].left = *node;
	else
		bTree[nodeParent].right = *node;
	return true;
}
//----------------------------------------------
//
//----------------------------------------------
template <class MAP_KEY, class CMP>
inline void InternalSet<MAP_KEY, CMP>::LRotate (DINT node)
{
	DINT temp = bTree[node].right; 
	if (temp != -1)
	{
		bTree[node].right = bTree[temp].left;  

		if (bTree[temp].left != -1)
			bTree[bTree[temp].left].parent = node;
		bTree[temp].parent = bTree[node].parent;

		if (bTree[node].parent == -1)
			pRoot = temp;
		else
		{
			if (node == bTree[bTree[node].parent].left)
				bTree[bTree[node].parent].left = temp;
			else
				bTree[bTree[node].parent].right = temp;
		}

		bTree[temp].left = node;
		bTree[node].parent = temp;
	}
}
//----------------------------------------------
//
//----------------------------------------------
template <class MAP_KEY, class CMP>
void InternalSet<MAP_KEY, CMP>::RRotate (DINT node)
{
	DINT temp = bTree[node].left;
	if (temp != -1)
	{
		bTree[node].left = bTree[temp].right;

		if (bTree[temp].right != -1)
			bTree[bTree[temp].right].parent = node;
		bTree[temp].parent = bTree[node].parent;
		if (bTree[node].parent == -1)
			pRoot = temp;
		else
		{
			if (node == bTree[bTree[node].parent].right)
				bTree[bTree[node].parent].right = temp;
			else
				bTree[bTree[node].parent].left = temp;
		}
		bTree[temp].right = node;
		bTree[node].parent = temp;
	}

}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
template <class MAP_KEY, class CMP>
bool InternalSet<MAP_KEY, CMP>::Add(MAP_KEY &key, DINT *_node)
{
	DINT node = _node ? *_node : -1, last;
	if (!Insert(key, &node)) 
	{
		if (node != -1)
		{
			if (_node) *_node = node;
			return true;
		}
		return false;
	}
	bTree[node].color = _RED;
	last = node;
	for(;bTree[bTree[node].parent].color  == _RED;)
	{
		if (bTree[node].parent ==bTree[bTree[bTree[node].parent].parent].left)
		{
			DINT temp =bTree[bTree[bTree[node].parent].parent].right;
			if (bTree[temp].color == _RED)
			{
				bTree[bTree[node].parent].color = _BLACK;
				bTree[temp].color = _BLACK;
				bTree[bTree[bTree[node].parent].parent].color = _RED;
				node = bTree[bTree[node].parent].parent;
			}
			else
			{
				if (node == bTree[bTree[node].parent].right)
				{
					node = bTree[node].parent;
					LRotate(node);
				}
				bTree[bTree[node].parent].color = _BLACK;
				bTree[bTree[bTree[node].parent].parent].color = _RED;
				RRotate(bTree[bTree[node].parent].parent);
			}
		}
		else
		{
			DINT temp =bTree[bTree[bTree[node].parent].parent].left;
			if (bTree[temp].color == _RED)
			{
				bTree[bTree[node].parent].color = _BLACK;
				bTree[temp].color = _BLACK;
				bTree[bTree[bTree[node].parent].parent].color = _RED;
				node = bTree[bTree[node].parent].parent;
			}
			else
			{
				if (node == bTree[bTree[node].parent].left)
				{
					node = bTree[node].parent;
					RRotate(node);
				}
				bTree[bTree[node].parent].color = _BLACK;
				bTree[bTree[bTree[node].parent].parent].color = _RED;
				LRotate(bTree[bTree[node].parent].parent);
			}
		}
	}
	bTree[pRoot].color = _BLACK;
	bTree[-1].Init(-1, -1, -1, _BLACK);

	if (_node) *_node = last;

	return true;
}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
template <class MAP_KEY, class CMP>
void InternalSet<MAP_KEY, CMP>::DeleteFix(DINT node)
{
	while (node != pRoot && bTree[node].color == _BLACK) 
	{
		if (node == bTree[bTree[node].parent].left) 
		{
			DINT tmp = bTree[bTree[node].parent].right;
			if (bTree[tmp].color == _RED) 
			{
				bTree[tmp].color = _BLACK;
				bTree[bTree[node].parent].color = _RED;
				LRotate (bTree[node].parent);
				tmp = bTree[bTree[node].parent].right;
			}
			if (bTree[bTree[tmp].left].color == _BLACK && bTree[bTree[tmp].right].color == _BLACK) 
			{
				bTree[tmp].color = _RED;
				node = bTree[node].parent;
			} 
			else 
			{
				if (bTree[bTree[tmp].right].color == _BLACK) 
				{
					bTree[bTree[tmp].left].color = _BLACK;
					bTree[tmp].color = _RED;
					RRotate (tmp);
					tmp = bTree[bTree[node].parent].right;
				}
				bTree[tmp].color = bTree[bTree[node].parent].color;
				bTree[bTree[node].parent].color = _BLACK;
				bTree[bTree[tmp].right].color = _BLACK;
				LRotate(bTree[node].parent);
				node = pRoot;
			}
		} 
		else 
		{
			DINT tmp = bTree[bTree[node].parent].left;
			if (bTree[tmp].color == _RED) 
			{
				bTree[tmp].color = _BLACK;
				bTree[bTree[node].parent].color = _RED;
				RRotate (bTree[node].parent);
				tmp = bTree[bTree[node].parent].left;
			}
			if (bTree[bTree[tmp].right].color == _BLACK && bTree[bTree[tmp].left].color == _BLACK) 
			{
				bTree[tmp].color = _RED;
				node = bTree[node].parent;
			} 
			else 
			{
				if (bTree[bTree[tmp].left].color == _BLACK) 
				{
					bTree[bTree[tmp].right].color = _BLACK;
					bTree[tmp].color = _RED;
					LRotate (tmp);
					tmp = bTree[bTree[node].parent].left;
				}
				bTree[tmp].color = bTree[bTree[node].parent].color;
				bTree[bTree[node].parent].color = _BLACK;
				bTree[bTree[tmp].left].color = _BLACK;
				RRotate (bTree[node].parent);
				node = pRoot;
			}
		}
	}
	bTree[node].color = _BLACK;

}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
template <class MAP_KEY, class CMP>
void InternalSet<MAP_KEY, CMP>::DeleteNode (DINT node)
{
	
	DINT tmpX, tmpY;
	if (node == -1) return;
	if (bTree[node].left == -1 || bTree[node].right == -1)
		tmpY = 	node;
	else
	{
		tmpY = bTree[node].right;
		while (bTree[tmpY].left != -1) tmpY = bTree[tmpY].left;
	}
	if (bTree[tmpY].left != -1)
		tmpX = bTree[tmpY].left;
	else
		tmpX = bTree[tmpY].right;

	bTree[tmpX].parent = bTree[tmpY].parent;
	if (bTree[tmpY].parent == -1)
		pRoot = tmpX;
	else
	{
		if (bTree[bTree[tmpY].parent].left == tmpY)
			bTree[bTree[tmpY].parent].left = tmpX;
		else
			bTree[bTree[tmpY].parent].right = tmpX;
	}
	if (tmpY != node)
		bData[node] = bData[tmpY];	
	if (bTree[tmpY].color == _BLACK)
        DeleteFix(tmpX);
	FreeNode(tmpY);
	bTree[-1].Init(-1, -1, -1, _BLACK);
}
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
template <class MAP_KEY, class CMP>
inline DINT InternalSet<MAP_KEY, CMP>::Search(DINT node, MAP_KEY &key)
{
	if (Parent->m_fKeyFromCompareFunc)
	{
		DINT n;
		while (node != -1 && (n = Parent->cmp.compare(key, bData[node])))
			node = n < 0 ? bTree[node].left : bTree[node].right;
	}
	else
		while (node != -1 && bData[node] != key)
				node = bData[node] > key ? bTree[node].left : bTree[node].right;
	return node;
}
/************************************************************************
*																		*
************************************************************************/
template <class MAP_KEY, class CMP = RBCmp<MAP_KEY> >
class MultiSet : public ParentMultiSet<MAP_KEY, CMP> {
public:
	MultiSet() {}
	MultiSet(DINT iLen, bool fKeyFromCompareFunc, bool fNewDelele)
	{
		sets.SetLen(iLen);
		ParentMultiSet<MAP_KEY, CMP>::m_fKeyFromCompareFunc = fKeyFromCompareFunc;
		ParentMultiSet<MAP_KEY, CMP>::m_fNewDelele = fNewDelele;
		ParentMultiSet<MAP_KEY, CMP>::Data.Add();
		ParentMultiSet<MAP_KEY, CMP>::Tree.Add();
		ParentMultiSet<MAP_KEY, CMP>::bData = ParentMultiSet<MAP_KEY, CMP>::Data.GetPtr();
		if (ParentMultiSet<MAP_KEY, CMP>::m_fNewDelele)
			ParentMultiSet<MAP_KEY, CMP>::bData = new (ParentMultiSet<MAP_KEY, CMP>::bData) MAP_KEY();
		++ParentMultiSet<MAP_KEY, CMP>::bData;
		ParentMultiSet<MAP_KEY, CMP>::bTree = ParentMultiSet<MAP_KEY, CMP>::Tree.GetPtr() + 1;
		ParentMultiSet<MAP_KEY, CMP>::bTree[-1].Init(-1, -1, -1, _BLACK);
		InternalSet<MAP_KEY, CMP> *s = sets.GetPtr();
		for (DINT i = 0; i < iLen; ++i, ++s)
			s = new (s) InternalSet<MAP_KEY, CMP> (this, ParentMultiSet<MAP_KEY, CMP>::bData, ParentMultiSet<MAP_KEY, CMP>::bTree);
	}
	void Init(DINT iLen, bool fKeyFromCompareFunc, bool fNewDelele)
	{
		DINT l = sets.GetLen();
		sets.SetLen(iLen);
		ParentMultiSet<MAP_KEY, CMP>::m_fKeyFromCompareFunc = fKeyFromCompareFunc;
		ParentMultiSet<MAP_KEY, CMP>::m_fNewDelele = fNewDelele;
		ParentMultiSet<MAP_KEY, CMP>::Data.Add();
		ParentMultiSet<MAP_KEY, CMP>::Tree.Add();
		ParentMultiSet<MAP_KEY, CMP>::bData = ParentMultiSet<MAP_KEY, CMP>::Data.GetPtr();
		if (ParentMultiSet<MAP_KEY, CMP>::m_fNewDelele)
			ParentMultiSet<MAP_KEY, CMP>::bData = new (ParentMultiSet<MAP_KEY, CMP>::bData) MAP_KEY();
		++ParentMultiSet<MAP_KEY, CMP>::bData;
		ParentMultiSet<MAP_KEY, CMP>::bTree = ParentMultiSet<MAP_KEY, CMP>::Tree.GetPtr() + 1;
		ParentMultiSet<MAP_KEY, CMP>::bTree[-1].Init(-1, -1, -1, _BLACK);
		InternalSet<MAP_KEY, CMP> *s = sets.GetPtr();
		for (DINT i = l; i < iLen; ++i, ++s)
			s = new (s) InternalSet<MAP_KEY, CMP> (this, ParentMultiSet<MAP_KEY, CMP>::bData, ParentMultiSet<MAP_KEY, CMP>::bTree);
	}

	virtual ~MultiSet() { Clear(); }
	virtual void AllocNode(DINT *node)
	{
		if (ParentMultiSet<MAP_KEY, CMP>::bDeleted.IsEmpty())
		{
			ParentMultiSet<MAP_KEY, CMP>::Data.Add();
			ParentMultiSet<MAP_KEY, CMP>::Tree.Add();
			*node = (DINT)ParentMultiSet<MAP_KEY, CMP>::Data.GetLen() - 2;
			ParentMultiSet<MAP_KEY, CMP>::bData = ParentMultiSet<MAP_KEY, CMP>::Data.GetPtr() + 1;
			ParentMultiSet<MAP_KEY, CMP>::bTree = ParentMultiSet<MAP_KEY, CMP>::Tree.GetPtr() + 1;

			
		}
		else
			*node = ParentMultiSet<MAP_KEY, CMP>::bDeleted.Pop();
		if (ParentMultiSet<MAP_KEY, CMP>::m_fNewDelele) 
		{
			MAP_KEY *d = ParentMultiSet<MAP_KEY, CMP>::bData + *node;
			d = new (d) MAP_KEY();
		}
		ParentMultiSet<MAP_KEY, CMP>::bTree[*node].Init(-1, -1, -1, _BLACK);
	}
	virtual void FreeNode(DINT node)
	{
		if (ParentMultiSet<MAP_KEY, CMP>::m_fNewDelele) 
		{
			MAP_KEY *d = ParentMultiSet<MAP_KEY, CMP>::bData + node;
			d->~MAP_KEY();
		}		
		ParentMultiSet<MAP_KEY, CMP>::bDeleted.Push(node);
	}
	void Clear()
	{
		if (ParentMultiSet<MAP_KEY, CMP>::m_fNewDelele)
		{
			ArHeap Deleted = ParentMultiSet<MAP_KEY, CMP>::bDeleted;
			DINT i = 0, l = ParentMultiSet<MAP_KEY, CMP>::Data.GetLen() - 1;
			while (!Deleted.IsEmpty())
			{
				DINT j = Deleted.Pop();
				for (;i < j; ++i)
				{
					MAP_KEY *d = ParentMultiSet<MAP_KEY, CMP>::bData + i;
					d->~MAP_KEY();
				}
				++i;
			}
			for (;i < l; ++i)
			{
				MAP_KEY *d = ParentMultiSet<MAP_KEY, CMP>::bData + i;
				d->~MAP_KEY();
			}
			MAP_KEY *d = ParentMultiSet<MAP_KEY, CMP>::bData -1;
			d->~MAP_KEY();
		}
		ParentMultiSet<MAP_KEY, CMP>::Data.SetLen(1);
		ParentMultiSet<MAP_KEY, CMP>::Tree.SetLen(1);
		ParentMultiSet<MAP_KEY, CMP>::bDeleted.Clear();
		ParentMultiSet<MAP_KEY, CMP>::bData = ParentMultiSet<MAP_KEY, CMP>::Data.GetPtr() + 1;
		ParentMultiSet<MAP_KEY, CMP>::bTree = ParentMultiSet<MAP_KEY, CMP>::Tree.GetPtr() + 1;
		ParentMultiSet<MAP_KEY, CMP>::bTree[-1].Init(-1, -1, -1, _BLACK);
		InternalSet<MAP_KEY, CMP> *s = sets.GetPtr();
		for (DINT i = 0, iLen = sets.GetLen(); i < iLen; ++i, ++s)
			s->pRoot = -1;

	}
	InternalSet<MAP_KEY, CMP> &operator [] (DINT i) { return sets[i]; }
	InternalSet<MAP_KEY, CMP> *GetPtr() { return sets.GetPtr(); }
	DINT GetLen() { return sets.GetLen(); }
	MArray<InternalSet<MAP_KEY, CMP> > sets;
	void SetAddedRealSize(int a) { 
		ParentMultiSet<MAP_KEY, CMP>::Data.SetAddedRealSize(a); 
		ParentMultiSet<MAP_KEY, CMP>::Tree.SetAddedRealSize(a); 		
		
 }

	
};

#pragma pack(pop)
#endif
