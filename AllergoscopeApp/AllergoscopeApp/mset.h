///////////////////////////////////////////////////
// mset.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////
#ifndef __M__SET__RB__
#define __M__SET__RB__
#include "mlist.h"
//----------------------------------------------
//
//----------------------------------------------

																	 
template <class MAP_KEY, class CMP = RBCmp<MAP_KEY>, class LIST_ALLOC = ListNotNew<RBKeyNode<MAP_KEY> > >
class RBSet : public LIST_ALLOC {
public:
	enum  { _BLACK, _RED };

	RBSet() { m_fKeyFromCompareFunc = 0; m_fNewDelele = 1; pRoot = &m_Nil; m_Nil.Init(&m_Nil, &m_Nil, &m_Nil, _BLACK); };
	virtual ~RBSet() { Clear(); };
	RBSet(RBSet &r) { m_fKeyFromCompareFunc = 0; pRoot = &m_Nil; m_Nil.Init(&m_Nil, &m_Nil, &m_Nil, _BLACK); *this = r; } 
	void CreateNew(bool fNewDelele) { m_fNewDelele = fNewDelele; } 
	void KeyFromCompareFunc(bool b) { m_fKeyFromCompareFunc = b;  }
	RBKeyNode<MAP_KEY> *Root() { return pRoot; }
	RBKeyNode<MAP_KEY> *Nil() { return &m_Nil; }
	bool Add(const MAP_KEY &key) { return Add(key, 0); }
	bool operator [] (const MAP_KEY &key) { return Find(key); }
	bool Find(const MAP_KEY &key);
	RBKeyNode<MAP_KEY> *Get(const MAP_KEY &key) { return Search(pRoot, key); }

	void Delete(const MAP_KEY &key) { RBKeyNode<MAP_KEY> *p = Search(pRoot, key); if (p != &m_Nil) DeleteNode(p); }
	RBSet &operator = (RBSet &a);
	void Clear();
	void Reset();
	MAP_KEY Min() { return Min(pRoot)->key; }
	MAP_KEY Max() { return Max(pRoot)->key; }
	void OR (RBSet &in, RBSet &out);
	RBSet &operator |= (RBSet &in) 
	{
		Copy(in);
		return *this;
		
	}
	bool AND (RBSet &in, RBSet &out);
	void GetSortedRBData(MArray<MAP_KEY> &arr) { ::GetSortedRBData(arr, *this); }
	void GetSortedRBData(MVector<MAP_KEY> &arr) { ::GetSortedRBData(arr, *this); }
	DINT Rank() 	{ DINT y = 0; Rank(y, pRoot);  return y; }
protected:
	bool Add(const MAP_KEY &key, RBKeyNode< MAP_KEY> **_node);
	void Rank(DINT &y, RBKeyNode<MAP_KEY> *node);

	void Copy(RBSet &mp)
	{
		iterator it(mp);
		for (MAP_KEY *t = it.begin(); t; t = it.next())
			Add(*t);
	}
	RBKeyNode<MAP_KEY> *Max(RBKeyNode<MAP_KEY> *node);
	RBKeyNode<MAP_KEY> *Min(RBKeyNode<MAP_KEY> *node);
	virtual bool AllocNode(RBKeyNode<MAP_KEY> **node);
	virtual void FreeNode(RBKeyNode<MAP_KEY> *node);
	void DeleteNode (RBKeyNode<MAP_KEY> *node);
	void DeleteFix(RBKeyNode<MAP_KEY> *node);
	RBKeyNode<MAP_KEY> *Search(RBKeyNode<MAP_KEY> *node, const MAP_KEY &key);
	bool Insert(const MAP_KEY &key, RBKeyNode<MAP_KEY> **node);
	void LRotate (RBKeyNode<MAP_KEY> *node);
	void RRotate (RBKeyNode<MAP_KEY> *node);
protected:
	RBKeyNode<MAP_KEY> *pRoot; 
	RBKeyNode<MAP_KEY> m_Nil; 
	CMP cmp;
	bool m_fNewDelele;
	bool m_fKeyFromCompareFunc;
public:
class iterator
{ 
	RBSet &mp;
	RBKeyNode<MAP_KEY> *t, *node;
	MArray<RBKeyNode<MAP_KEY> *> stack;
	RBKeyNode<MAP_KEY> **test, **pStack;
	bool fCheck;
	DINT len;
public:
	iterator(RBSet &g):mp(g), t(&mp.m_Nil) { }
	MAP_KEY *begin()
	{ 
		if (mp.pRoot == &mp.m_Nil) return 0;	
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
			node = node->left;
		}
		while(node != &mp.m_Nil);
		node = *(--pStack);
		t = node;
		if((fCheck = node->right != &mp.m_Nil))
			node = node->right;
		return &t->key; 
	}
	MAP_KEY *begin(MAP_KEY key)
	{ 
		if (mp.pRoot == &mp.m_Nil) return 0;	
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
			node = node->left;
			if (node != &mp.m_Nil)
			{
				if (mp.m_fKeyFromCompareFunc)
				{
					if (mp.cmp.compare(key, node->key) <= 0)
						break;
				}
				else
					if (node->key >= key)
						break;
			}
		}
		while(node != &mp.m_Nil);
		if (node == &mp.m_Nil)
			node = *(--pStack);
		t = node;
		if((fCheck = node->right != &mp.m_Nil))
			node = node->right;
		if (key != t->key)
		{
			MAP_KEY *tmp = 0;
			for(tmp = next(); tmp; tmp = next())
			{
				if (mp.m_fKeyFromCompareFunc)
				{
					if (mp.cmp.compare(key, *tmp) <= 0)
						break;
				}
				else
					if (*tmp >= key)
						break;

			}
			return tmp;
		}
		return &t->key; 
	}

	MAP_KEY *next()
	{ 
		if (t == &mp.m_Nil)
			return begin();
		if (t != &mp.m_Nil && (fCheck || pStack > test))
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
					node = node->left;
				}
				while(node != &mp.m_Nil);
			}
			node = *(--pStack);
			t = node;
			if((fCheck = node->right != &mp.m_Nil))
			node = node->right;
		}
		else
			t = &mp.m_Nil;

		return t != &mp.m_Nil ? &t->key : 0; 
	}
	bool end()
	{ return !t || t == &mp.m_Nil; }
};
class iteratorNode
{ 
	RBSet &mp;
	RBKeyNode<MAP_KEY> *t, *node;
	MArray<RBKeyNode<MAP_KEY> *> stack;
	RBKeyNode<MAP_KEY> **test, **pStack;
	bool fCheck;
	DINT len;
public:
	iteratorNode(RBSet &g):mp(g), t(&mp.m_Nil) { }
	RBKeyNode<MAP_KEY> *begin()
	{ 
		if (mp.pRoot == &mp.m_Nil) return 0;	
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
			node = node->left;
		}
		while(node != &mp.m_Nil);
		node = *(--pStack);
		t = node;
		if((fCheck = node->right != &mp.m_Nil))
			node = node->right;
		return t; 
	}
	RBKeyNode<MAP_KEY> *next()
	{ 
		if (t == &mp.m_Nil)
			return begin();
		if (t != &mp.m_Nil && (fCheck || pStack > test))
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
					node = node->left;
				}
				while(node != &mp.m_Nil);
			}
			node = *(--pStack);
			t = node;
			if((fCheck = node->right != &mp.m_Nil))
			node = node->right;
		}
		else
			t = &mp.m_Nil;

		return t != &mp.m_Nil ? t : 0; 
	}
	bool end()
	{ return !t || t == &mp.m_Nil; }
};
friend class iterator;
friend class iteratorNode;
};

//-------------------------------------
//
//-------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
void RBSet< MAP_KEY, CMP, LIST_ALLOC>::DeleteNode (RBKeyNode< MAP_KEY> *node)
{
	
	RBKeyNode< MAP_KEY> *tmpX, *tmpY;
	if (!node || node == &m_Nil) return;
	if (node->left == &m_Nil || node->right == &m_Nil)
		tmpY = 	node;
	else
	{
		tmpY = node->right;
		while (tmpY->left != &m_Nil) tmpY = tmpY->left;
	}
	if (tmpY->left != &m_Nil)
		tmpX = tmpY->left;
	else
		tmpX = tmpY->right;

	tmpX->parent = tmpY->parent;
	if (tmpY->parent == &m_Nil)
		pRoot = tmpX;
	else
	{
		if (tmpY->parent->left == tmpY)
			tmpY->parent->left = tmpX;
		else
			tmpY->parent->right = tmpX;
	}
	if (tmpY != node)
		node->key = tmpY->key;	
	if (tmpY->color == _BLACK)
        DeleteFix(tmpX);
	FreeNode(tmpY);
	m_Nil.Init(&m_Nil, &m_Nil, &m_Nil, _BLACK);
}

template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline void RBSet< MAP_KEY, CMP, LIST_ALLOC>::Clear() 
{  
	if (m_fNewDelele)
		LIST_ALLOC::RunAllDataDestructor();
	LIST_ALLOC::Clear(); 
	pRoot = &m_Nil; 
}
template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline void RBSet< MAP_KEY, CMP, LIST_ALLOC>::Reset() 
{  
	if (m_fNewDelele)
		LIST_ALLOC::RunAllDataDestructor();
	LIST_ALLOC::Reset(); 
	pRoot = &m_Nil; 
}

template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline bool RBSet< MAP_KEY, CMP, LIST_ALLOC>::Find(const MAP_KEY &key) 
{ 
	RBKeyNode< MAP_KEY> *p = Search(pRoot, key); 
	return p != &m_Nil; 
}
//-------------------------------------
//
//-------------------------------------

template <class MAP_KEY, class CMP, class LIST_ALLOC>
void RBSet< MAP_KEY, CMP, LIST_ALLOC>::DeleteFix(RBKeyNode< MAP_KEY> *node)
{
	while (node != pRoot && node->color == _BLACK) 
	{
		if (node == node->parent->left) 
		{
			RBKeyNode< MAP_KEY> *tmp = node->parent->right;
			if (tmp->color == _RED) 
			{
				tmp->color = _BLACK;
				node->parent->color = _RED;
				LRotate (node->parent);
				tmp = node->parent->right;
			}
			if (tmp->left->color == _BLACK && tmp->right->color == _BLACK) 
			{
				tmp->color = _RED;
				node = node->parent;
			} 
			else 
			{
				if (tmp->right->color == _BLACK) 
				{
					tmp->left->color = _BLACK;
					tmp->color = _RED;
					RRotate (tmp);
					tmp = node->parent->right;
				}
				tmp->color = node->parent->color;
				node->parent->color = _BLACK;
				tmp->right->color = _BLACK;
				LRotate(node->parent);
				node = pRoot;
			}
		} 
		else 
		{
			RBKeyNode< MAP_KEY> *tmp = node->parent->left;
			if (tmp->color == _RED) 
			{
				tmp->color = _BLACK;
				node->parent->color = _RED;
				RRotate (node->parent);
				tmp = node->parent->left;
			}
			if (tmp->right->color == _BLACK && tmp->left->color == _BLACK) 
			{
				tmp->color = _RED;
				node = node->parent;
			} 
			else 
			{
				if (tmp->left->color == _BLACK) 
				{
					tmp->right->color = _BLACK;
					tmp->color = _RED;
					LRotate (tmp);
					tmp = node->parent->left;
				}
				tmp->color = node->parent->color;
				node->parent->color = _BLACK;
				tmp->left->color = _BLACK;
				RRotate (node->parent);
				node = pRoot;
			}
		}
	}
	node->color = _BLACK;

}
//-------------------------------------
//
//-------------------------------------

template <class MAP_KEY, class CMP, class LIST_ALLOC>
bool RBSet< MAP_KEY, CMP, LIST_ALLOC>::Add(const MAP_KEY &key, RBKeyNode< MAP_KEY> **_node)
{
	RBKeyNode< MAP_KEY> *node = _node ? *_node : 0;
	if (!Insert(key, &node)) 
		return node ? 1 : 0;
	node->color = _RED;

	while (node != pRoot && node->parent->color == _RED)
	{
		if (node->parent == node->parent->parent->left)
		{
			RBKeyNode< MAP_KEY> *temp = node->parent->parent->right;
			if (temp->color == _RED)
			{
				node->parent->color = _BLACK;
				temp->color = _BLACK;
				node->parent->parent->color = _RED;
				node = node->parent->parent;
			}
			else
			{
				if (node == node->parent->right)
				{
					node = node->parent;
					LRotate(node);
				}
				node->parent->color = _BLACK;
				node->parent->parent->color = _RED;
				RRotate(node->parent->parent);
			}
		}
		else
		{
			RBKeyNode< MAP_KEY> *temp = node->parent->parent->left;
			if (temp->color == _RED)
			{
				node->parent->color = _BLACK;
				temp->color = _BLACK;
				node->parent->parent->color = _RED;
				node = node->parent->parent;
			}
			else
			{
				if (node == node->parent->left)
				{
					node = node->parent;
					RRotate(node);
				}
				node->parent->color = _BLACK;
				node->parent->parent->color = _RED;
				LRotate(node->parent->parent);
			}
		}
	}
	pRoot->color = _BLACK;
	m_Nil.Init(&m_Nil, &m_Nil, &m_Nil, _BLACK);
	return 1;
}

//----------------------------------------------
//
//----------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline RBKeyNode< MAP_KEY> *RBSet< MAP_KEY, CMP, LIST_ALLOC>::Search(RBKeyNode< MAP_KEY> *node, const MAP_KEY &key)
{
	if (m_fKeyFromCompareFunc)
	{
		DINT n;
		while (node != &m_Nil && (n = cmp.compare(key, node->key)))
			node = n < 0 ? node->left : node->right;
	}
	else
		while (node != &m_Nil && node->key != key)
				node = node->key > key ? node->left : node->right;
	return node;
}
//----------------------------------------------
//
//----------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline RBKeyNode< MAP_KEY> *RBSet< MAP_KEY, CMP, LIST_ALLOC>::Min(RBKeyNode< MAP_KEY> *node)
{
		while (node->left != &m_Nil)
			node = node->left;
	return node;
}
//----------------------------------------------
//
//----------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline RBKeyNode< MAP_KEY> *RBSet< MAP_KEY, CMP, LIST_ALLOC>::Max(RBKeyNode< MAP_KEY> *node)
{
		while (node->right != &m_Nil)
			node = node->right;
	return node;
}

//----------------------------------------------
//
//----------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline void RBSet< MAP_KEY, CMP, LIST_ALLOC>::FreeNode(RBKeyNode< MAP_KEY> *node)
{
	if (m_fNewDelele) node->~RBKeyNode< MAP_KEY>();
	LIST_ALLOC::Delete(node);

}

//----------------------------------------------
//
//----------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline bool RBSet< MAP_KEY, CMP, LIST_ALLOC>::AllocNode(RBKeyNode< MAP_KEY> **node)
{
	*node = LIST_ALLOC::Add();
	if (*node)
	{
		if (m_fNewDelele) *node = new (*node) RBKeyNode< MAP_KEY>();
		(*node)->Init(&m_Nil, &m_Nil, &m_Nil, _BLACK);
	}
	return *node != 0;
}

//----------------------------------------------
//
//----------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
bool RBSet< MAP_KEY, CMP, LIST_ALLOC>::Insert(const MAP_KEY &key, RBKeyNode< MAP_KEY> **node)
{

	*node = 0;
	if (pRoot == &m_Nil)
	{
		if (*node)
			pRoot = *node;
		else
		{
			if (!AllocNode(&pRoot)) return 0;
			pRoot->key = key;
		}
		*node = pRoot;
		return true;
	}


	RBKeyNode< MAP_KEY> *nodeParent = &m_Nil;
	RBKeyNode< MAP_KEY> *temp = pRoot;
	DINT n = 0;
	if (m_fKeyFromCompareFunc)
	{
		while (temp != &m_Nil)
		{
			nodeParent = temp;
			if (!(n = cmp.compare(key, temp->key)))
			{
				*node = temp;
				return false;
			}
			if (n < 0)
				temp = temp->left;
			else
				temp = temp->right;
		}
		n = n < 0;
	}
	else
		while (temp != &m_Nil)
		{
			nodeParent = temp;
			if ((n = (key < temp->key)))
				temp = temp->left;
			else
			if (key > temp->key)
				temp = temp->right;
			else 
			{
				*node = temp;
				return false;
			}
		}
	if (!*node)
	{
		if (!AllocNode(node)) return 0;
		(*node)->key = key;
	}
	(*node)->parent = nodeParent;
	(*node)->color = _RED;
	if (n)
		nodeParent->left = *node;
	else
		nodeParent->right = *node;
	return true;
}

//----------------------------------------------
//
//----------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline void RBSet< MAP_KEY, CMP, LIST_ALLOC>::LRotate (RBKeyNode< MAP_KEY> *node)
{

	RBKeyNode< MAP_KEY> *temp = node->right; 
	node->right = temp->left;  

	if (temp->left != &m_Nil)
		temp->left->parent = node;
	temp->parent = node->parent;

	if (node->parent == &m_Nil)
		pRoot = temp;
	else
	{
		if (node == node->parent->left)
			node->parent->left = temp;
		else
			node->parent->right = temp;
	}

	temp->left = node;
	node->parent = temp;
}
//----------------------------------------------
//
//----------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline void RBSet< MAP_KEY, CMP, LIST_ALLOC>::RRotate (RBKeyNode< MAP_KEY> *node)
{
	RBKeyNode< MAP_KEY> *temp = node->left;
	node->left = temp->right;

	if (temp->right != &m_Nil)
		temp->right->parent = node;
	temp->parent = node->parent;
	if (node->parent == &m_Nil)
		pRoot = temp;
	else
	{
		if (node == node->parent->right)
			node->parent->right = temp;
		else
			node->parent->left = temp;
	}
	temp->right = node;
	node->parent = temp;
}
//---------------------------------------
//
//---------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline RBSet< MAP_KEY, CMP, LIST_ALLOC> &RBSet< MAP_KEY, CMP, LIST_ALLOC>::operator = (RBSet< MAP_KEY, CMP, LIST_ALLOC> &a) 
{ 
	Clear();
	m_fNewDelele = a.m_fNewDelele;
	m_fKeyFromCompareFunc = a.m_fKeyFromCompareFunc;
	Copy(a);
	return *this;  
} 
//----------------------------------------------
//
//----------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
void RBSet< MAP_KEY, CMP, LIST_ALLOC>::Rank(DINT &y, RBKeyNode< MAP_KEY> *node)
{
	if (node == &m_Nil)
		return;
	++y;
	DINT l = y;
	DINT r = y;
	Rank(l, node->left);
	Rank(r, node->right);
	y = max(l, r);
}
//----------------------------------------------
//
//----------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline void RBSet< MAP_KEY, CMP, LIST_ALLOC>::OR(RBSet< MAP_KEY, CMP, LIST_ALLOC> &in, RBSet< MAP_KEY, CMP, LIST_ALLOC> &out)
{
	RBSet *a, *b;
	if (in.pRoot != &in.m_Nil)
	{
		if (pRoot == &m_Nil)
		{
			out = in;
			return;
		}
	}
	else
	{
		out = *this;
		return;

	}
	if (this->Len() > in.Len())
	{
	   b = this;
	   a = &in;
	}
	else
	{
	   a = this;
	   b = &in;
	}
	out = *b;
	RBKeyNode<MAP_KEY> *node = a->pRoot;
	DINT index = 0, len = 16;
	MArray<RBKeyNode<MAP_KEY> *> stack(16);
	RBKeyNode<MAP_KEY> **test, **pStack = stack.GetPtr();
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
				node = node->left;
			}
			while(node != &a->m_Nil);
		}
		node = *(--pStack);
		out.Add(node->key);
		if((fCheck = node->right != &a->m_Nil))
		node = node->right;
	}
	while (fCheck || pStack > test);
}
//----------------------------------------------
//
//----------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
bool RBSet< MAP_KEY, CMP, LIST_ALLOC>::AND(RBSet< MAP_KEY, CMP, LIST_ALLOC> &in, RBSet< MAP_KEY, CMP, LIST_ALLOC> &out)
{
	RBSet *a, *b;
	out.Clear();
	out.m_fNewDelele = m_fNewDelele;
	out.m_fKeyFromCompareFunc = m_fKeyFromCompareFunc;

	if (this->Len() > in.Len())
	{
	   b = this;
	   a = &in;
	}
	else
	{
	   a = this;
	   b = &in;
	}
	MAP_KEY aMin = a->Min();
	MAP_KEY bMin = b->Min();
	MAP_KEY aMax = a->Max();
	MAP_KEY abMin, abMax, bMax = b->Max();
	if (m_fKeyFromCompareFunc)
	{
		abMin = cmp.compare(aMin, bMin) > 0 ? aMin : bMin;
		abMax = cmp.compare(aMax, bMax) <= 0 ? aMax : bMax;
	}
	else
	{
		abMin = max(aMin, bMin);
		abMax = min(aMax, bMax); 
	}
	if (abMax >= abMin)
	{
		RBKeyNode<MAP_KEY> *node = a->pRoot;
		DINT n, index = 0, len = 16;
		MArray<RBKeyNode<MAP_KEY> *> stack(16);
		RBKeyNode<MAP_KEY> **test, **pStack = stack.GetPtr();
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
					node = node->left;
				}
				while(node != &a->m_Nil);
			}
			node = *(--pStack);
			if (m_fKeyFromCompareFunc)
				n = cmp.compare(node->key, abMax) <= 0 || cmp.compare(node->key, abMin) >= 0;
			else
				n = node->key <= abMax && node->key >= abMin;
			if (n)
			{
				if (b->Find(node->key))
					out.Add(node->key); 
			}
 			if((fCheck = node->right != &a->m_Nil))
			node = node->right;
		}
		while (fCheck || pStack > test);
	}
	return out.Len() != 0;
}
//----------------------------------------------
//
//----------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC, class DATA__ARRAY__>
void GetSortedRBData(DATA__ARRAY__ &arr, RBSet< MAP_KEY, CMP, LIST_ALLOC> &mMap)
{
	arr.SetLen(mMap.Len());
	if (mMap.Root() == mMap.Nil()) return;	
	RBKeyNode<MAP_KEY> *node = mMap.Root();
	DINT index = 0, len = 16;
	MAP_KEY *pArr = arr.GetPtr();
	if (node == mMap.Nil())	return;		   
	MArray<RBKeyNode<MAP_KEY> *> stack(16);
	RBKeyNode<MAP_KEY> **test, **pStack = stack.GetPtr();
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
				node = node->left;
			}
			while(node != mMap.Nil());
		}
		node = *(--pStack);
		*pArr++ = node->key;
		if((fCheck = node->right != mMap.Nil()))
		node = node->right;
	}
	while (fCheck || pStack > test);

}

#endif
