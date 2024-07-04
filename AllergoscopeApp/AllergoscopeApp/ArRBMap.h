#ifndef __ArRBMap__
#define __ArRBMap__
#include "ArHeap.h"
#pragma pack (push)
#pragma pack (8)
struct ArRBTreeNode {
	DINT left;
	DINT right;
	DINT parent;
	bool color;
	ArRBTreeNode () { color = 0; parent = left = right = -1; };
	~ArRBTreeNode () {};
	ArRBTreeNode *Reset() { color = 0; parent = left = right = -1; return this; }
	void Init(DINT _left, DINT _right, DINT _parent, bool _color)
	{
		left = _left;
		right = _right;
		parent = _parent;
		color = _color;
	}
};

template <class MAP_VALUE, class MAP_KEY, class CMP = RBCmp<MAP_KEY>, class ARR_KEY = MArray<RBData<MAP_VALUE, MAP_KEY> >,  class ARR_TREE = MArray<ArRBTreeNode > >
class ArRBMap {

public:
	ArRBMap() { 
			_Max_Alloc_ = 0x1000000;
			Data.SetAddedRealSize(_Max_Alloc_);
			Data.Add();
			Tree.Add();
			bData = Data.GetPtr();
			bData = new (bData) RBData<MAP_VALUE, MAP_KEY>();
			++bData;
			bTree = Tree.GetPtr() + 1;
			bTree[-1].Init(-1, -1, -1, _BLACK);
			pRoot = -1;
			m_fKeyFromCompareFunc = 0;
			m_fNewDelele = true;
			m_iNum = 0;
		};
	~ArRBMap() { Clear(); };
	ArRBMap(ArRBMap &a) { 
			Data.Add();
			Tree.Add();
			m_iNum = 0;
			bData = Data.GetPtr() + 1;
			bData = new (bData) RBData<MAP_VALUE, MAP_KEY>();
			bTree = Tree.GetPtr() + 1;
			bTree[-1].Init(-1, -1, -1, _BLACK);
			operator = (a);

	}
	ArRBMap & operator = (ArRBMap &a) { 
		Clear();
		pRoot = a.pRoot;
		m_fNewDelele = a.m_fNewDelele;
		m_fKeyFromCompareFunc = a.m_fKeyFromCompareFunc;
		m_fNewDelele = a.m_fNewDelele;
		Tree = a.Tree;
		Data = a.Data;
		bDeleted = a.bDeleted;
		bData = Data.GetPtr() + 1;
		bTree = Tree.GetPtr() + 1;
		if (m_fNewDelele)
		{
			ArHeap Deleted = bDeleted;
			DINT i = 0, l = Data.GetLen();
			while (!Deleted.IsEmpty())
			{
				DINT j = Deleted.Pop();
				for (;i < j; ++i)
				{
					RBData<MAP_VALUE, MAP_KEY> *d = bData + i;
					d = new (d) RBData<MAP_VALUE, MAP_KEY>(a.bData[i].key, a.bData[i].value);
				}
				++i;
			}
			for (;i < l; ++i)
			{
				RBData<MAP_VALUE, MAP_KEY> *d = bData + i;
				d = new (d) RBData<MAP_VALUE, MAP_KEY>(a.bData[i].key, a.bData[i].value);
			}
		}
			
		m_iNum = a.m_iNum;
		return *this;
	}
	void CreateNew(bool fNewDelele) { m_fNewDelele = fNewDelele; }

	void KeyFromCompareFunc(bool b) { m_fKeyFromCompareFunc = b;  }
	void SetAddedRealSize(int a) { Data.SetAddedRealSize(a); Tree.SetAddedRealSize(a);}
	MAP_VALUE &Value (MAP_KEY key) { return Add(key, 0); }
	MAP_VALUE &operator [] (MAP_KEY key) { return Add(key, 0); }
	bool Lookup(MAP_KEY key, MAP_VALUE **b = 0);
	bool Lookup(MAP_KEY key, MAP_VALUE &b);
	void Delete(MAP_KEY key) { DINT p = Search(pRoot, key); if (p != -1) DeleteNode(p); }
	DINT GetLen() { return m_iNum; }
	void Clear() {
		if (m_fNewDelele)
		{
			ArHeap Deleted = bDeleted;
			DINT i = 0, l = Data.GetLen() - 1;
			while (!Deleted.IsEmpty())
			{
				DINT j = Deleted.Pop();
				for (;i < j; ++i)
				{
					RBData<MAP_VALUE, MAP_KEY> *d = bData + i;
					d->~RBData<MAP_VALUE, MAP_KEY>();
				}
				++i;
			}
			for (;i < l; ++i)
			{
				RBData<MAP_VALUE, MAP_KEY> *d = bData + i;
				d->~RBData<MAP_VALUE, MAP_KEY>();
			}
			RBData<MAP_VALUE, MAP_KEY> *d = bData -1;
			d->~RBData<MAP_VALUE, MAP_KEY>();
		}
		Data.Clear();
		Tree.Clear();
		Data.Add();
		Tree.Add();
		bDeleted.Clear();
		bData = Data.GetPtr() + 1;
		bTree = Tree.GetPtr() + 1;
		bTree[-1].Init(-1, -1, -1, _BLACK);
		pRoot = -1;
		m_iNum = 0;

	}
	void GetSortedRBData(MArray<RBData<MAP_VALUE,MAP_KEY> > &arr);

	DINT Rank() 	{ DINT y = 0; Rank(y, pRoot);  return y; }
	void PreAlloc(DINT len) { Data.SetLen(len + 1); Data.SetLen(1); Tree.SetLen(len + 1); Tree.SetLen(1);
								bData = Data.GetPtr() + 1;
								bTree = Tree.GetPtr() + 1;
							}

protected:
	void Rank(DINT &y, DINT node);
	void AllocNode(DINT *node);
	void FreeNode(DINT  node);

	MAP_VALUE &Add(MAP_KEY &key, MAP_VALUE *value, DINT *_node =0);

	void DeleteNode (DINT  node);
	void DeleteFix(DINT node);
	DINT Search(DINT  node, MAP_KEY &key);
	bool Insert(MAP_KEY &key, MAP_VALUE *value, DINT *node);
	void LRotate (DINT node);
	void RRotate (DINT node);

protected:
	bool m_fKeyFromCompareFunc;

public:
	DINT m_iNum;
	int _Max_Alloc_;
	CMP cmp;
	DINT pRoot; 
	ARR_KEY Data;
	ARR_TREE Tree;
	RBData<MAP_VALUE, MAP_KEY> *bData;
	ArRBTreeNode *bTree;
	ArHeap bDeleted; 
	BOOL m_fNewDelele;
class iterator 
{ 
	ArRBMap &mp;
	DINT t;
	DINT node;
	MArray<DINT> stack;
	DINT *test, *pStack;
	bool fCheck;
	DINT len;
public:
	iterator(ArRBMap &g):mp(g) { t = -1; }
	RBData<MAP_VALUE, MAP_KEY> *begin()
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
	RBData<MAP_VALUE, MAP_KEY> *next()
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

		return t != -1 ? mp.bData + t : 0; 
	}
	bool end()
	{ return t == -1; }
};
	

};
template <class MAP_VALUE, class MAP_KEY, class CMP, class ARR_KEY, class ARR_TREE>
void ArRBMap<MAP_VALUE,  MAP_KEY, CMP, ARR_KEY, ARR_TREE>::GetSortedRBData(MArray<RBData<MAP_VALUE,MAP_KEY> > &arr)
{
	if (pRoot == -1) return;	
	DINT node = pRoot;
	DINT index = 0, len = 16;
	RBData<MAP_VALUE, MAP_KEY> *pArr = arr.GetPtr();
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
template <class MAP_VALUE, class MAP_KEY, class CMP, class ARR_KEY, class ARR_TREE>
void ArRBMap<MAP_VALUE,  MAP_KEY, CMP, ARR_KEY, ARR_TREE>::Rank(DINT &y, DINT node)
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

template <class MAP_VALUE, class MAP_KEY, class CMP, class ARR_KEY, class ARR_TREE>
inline bool ArRBMap<MAP_VALUE,  MAP_KEY, CMP, ARR_KEY, ARR_TREE>::Lookup(MAP_KEY key, MAP_VALUE **b) 
{ 
	DINT p = Search(pRoot, key); 
	if (p != -1) 
	{ 
		if (b) 
			*b = &bData[p].value; 
		return 1; 
	} 
	return 0; 
}
template <class MAP_VALUE, class MAP_KEY, class CMP, class ARR_KEY, class ARR_TREE>
inline bool ArRBMap<MAP_VALUE,  MAP_KEY, CMP, ARR_KEY, ARR_TREE>::Lookup(MAP_KEY key, MAP_VALUE &b) 
{ 
	DINT p = Search(pRoot, key); 
	if (p != -1) 
	{ 
		b = bData[p].value; 
		return 1; 
	} 
	return 0; 
}

template <class MAP_VALUE, class MAP_KEY, class CMP, class ARR_KEY, class ARR_TREE>
inline void ArRBMap<MAP_VALUE,  MAP_KEY, CMP, ARR_KEY, ARR_TREE>::AllocNode(DINT *node)
{
	if (bDeleted.IsEmpty())
	{
		Data.Add();
		Tree.Add();
		*node = (DINT)Data.GetLen() - 2;
		bData = Data.GetPtr() + 1;
		bTree = Tree.GetPtr() + 1;

		
	}
	else
		*node = bDeleted.Pop();
	if (m_fNewDelele) 
	{
		RBData<MAP_VALUE, MAP_KEY> *d = bData + *node;
		d = new (d) RBData<MAP_VALUE, MAP_KEY>();
	}
	bTree[*node].Init(-1, -1, -1, _BLACK);
	++m_iNum;
}
template <class MAP_VALUE, class MAP_KEY, class CMP, class ARR_KEY, class ARR_TREE>
inline void ArRBMap<MAP_VALUE,  MAP_KEY, CMP, ARR_KEY, ARR_TREE>::FreeNode(DINT node)
{
	RBData<MAP_VALUE, MAP_KEY> *d = bData + node;
	if (m_fNewDelele) 
	{
		d->~RBData<MAP_VALUE, MAP_KEY>();
	}		
//	memset(d, 0xff, sizeof(RBData<MAP_VALUE, MAP_KEY>));
	bDeleted.Push(node);
	--m_iNum;
	
}
template <class MAP_VALUE, class MAP_KEY, class CMP, class ARR_KEY, class ARR_TREE>
bool ArRBMap<MAP_VALUE,  MAP_KEY, CMP, ARR_KEY, ARR_TREE>::Insert(MAP_KEY &key, MAP_VALUE *value, DINT *node)
{

	if (pRoot == -1)
	{
		AllocNode(&pRoot);
		bData[pRoot].key = key;
		if (value)
			bData[pRoot].value = *value;
		*node = pRoot;
		return true;
	}

	DINT nodeParent = -1;
	DINT temp = pRoot;
	DINT n = 0;
	
	if (m_fKeyFromCompareFunc)
	{
		while (temp != -1)
		{
			nodeParent = temp;
			if (!(n = cmp.compare(key, bData[temp].key)))
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
			RBData<MAP_VALUE,  MAP_KEY> &rbdat = bData[temp];
			if ((n = (key < rbdat.key)))
				temp = bTree[temp].left;
			else
			if (key > rbdat.key)
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
		 bData[*node].key = key;
		if (value)
			bData[*node].value = *value;
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
template <class MAP_VALUE, class MAP_KEY, class CMP, class ARR_KEY, class ARR_TREE>
inline void ArRBMap<MAP_VALUE,  MAP_KEY, CMP, ARR_KEY, ARR_TREE>::LRotate (DINT node)
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
template <class MAP_VALUE, class MAP_KEY, class CMP, class ARR_KEY, class ARR_TREE>
void ArRBMap<MAP_VALUE,  MAP_KEY, CMP, ARR_KEY, ARR_TREE>::RRotate (DINT node)
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
template <class MAP_VALUE, class MAP_KEY, class CMP, class ARR_KEY, class ARR_TREE>
MAP_VALUE &ArRBMap<MAP_VALUE,  MAP_KEY, CMP, ARR_KEY, ARR_TREE>::Add(MAP_KEY &key, MAP_VALUE *value, DINT *_node)
{
	DINT node = _node ? *_node : -1, last;
	if (!Insert(key, value, &node)) 
	{
		if (node != -1)
		{
			if (_node) *_node = node;
			if (value) bData[node].value = *value;
			return bData[node].value;
		}
		return bData[-1].value;
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

	return bData[last].value;
}
template <class MAP_VALUE, class MAP_KEY, class CMP, class ARR_KEY, class ARR_TREE>
void ArRBMap<MAP_VALUE, MAP_KEY, CMP, ARR_KEY, ARR_TREE>::DeleteFix(DINT node)
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
template <class MAP_VALUE, class MAP_KEY, class CMP, class ARR_KEY, class ARR_TREE>
void ArRBMap<MAP_VALUE,  MAP_KEY, CMP, ARR_KEY, ARR_TREE>::DeleteNode (DINT node)
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
	{
		bData[node].key = bData[tmpY].key;	
		bData[node].value = bData[tmpY].value;	
	}
	if (bTree[tmpY].color == _BLACK)
        DeleteFix(tmpX);
	FreeNode(tmpY);
	bTree[-1].Init(-1, -1, -1, _BLACK);
}
template <class MAP_VALUE, class MAP_KEY, class CMP, class ARR_KEY, class ARR_TREE>
inline DINT ArRBMap<MAP_VALUE,  MAP_KEY, CMP, ARR_KEY, ARR_TREE>::Search(DINT node, MAP_KEY &key)
{
	if (m_fKeyFromCompareFunc)
	{
		DINT n;
		while (node != -1 && (n = cmp.compare(key, bData[node].key)))
			node = n < 0 ? bTree[node].left : bTree[node].right;
	}
	else
		while (node != -1 && bData[node].key != key)
				node = bData[node].key > key ? bTree[node].left : bTree[node].right;
	return node;
}

#pragma pack(pop)
#endif
