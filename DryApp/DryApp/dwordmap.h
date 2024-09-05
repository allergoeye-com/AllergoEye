///////////////////////////////////////////////////
// dwordmap.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////

#ifndef __DWORD_MAP__
#define __DWORD_MAP__
#include "rbmap.h"

template <class MAP_VALUE>
struct RBDWordNode {
	RBDWordNode *left;
	RBDWordNode *right;
	RBDWordNode *parent;
	union {
		struct {
			DWORD key : 31;
			DWORD color : 1;
		};
		DWORD dw_key;
	};
	MAP_VALUE value;
	RBDWordNode () { color = 0; parent = left = right = 0; };
	RBDWordNode (RBDWordNode &r) { operator = (r); };
	RBDWordNode *Reset() { key = 0; color = 0; parent = 0; left = 0; right = 0; return this; }
	RBDWordNode &operator = (RBDWordNode &r) { color = r.color; parent = r.parent; left = r.left; right = r.right; return *this; };
	void Init(RBDWordNode *_left, RBDWordNode *_right, RBDWordNode *_parent, bool _color)
	{
		left = _left;
		right = _right;
		parent = _parent;
		color = _color;
	}

	void Init(RBDWordNode *_left, RBDWordNode *_right, RBDWordNode *_parent, bool _color, DWORD &_key, MAP_VALUE &_value)
	{
		left = _left;
		right = _right;
		parent = _parent;
		color = _color;
		key	= _key;
		value = _value;
	}
};
template <class T>
struct MListDWordEntry {
		MListDWordEntry(MListDWordEntry *a = 0) : m_pPrev(0),m_pNext(0) 
		{
			if (a)
			{
				m_pPrev = a;
				a->m_pNext = this;	
			}
		}
		void Init(MListDWordEntry *a)
		{
			m_pPrev = m_pNext = 0;
			if (a)
			{
				m_pPrev = a;
				a->m_pNext = this;	
			}
		}

		~MListDWordEntry() 
		{
			if (m_pPrev)
				m_pPrev->m_pNext = m_pNext;
			if (m_pNext)
				m_pNext->m_pPrev = m_pPrev;
		}
		void Reset() 
		{
			if (m_pPrev)
				m_pPrev->m_pNext = m_pNext;
			if (m_pNext)
				m_pNext->m_pPrev = m_pPrev;
			m_pNext = m_pPrev = 0;
		}

		MListDWordEntry *m_pPrev;
		MListDWordEntry *m_pNext;
		T m_pData;
};


template <class T>
class LisDWord : public Allocator <MListDWordEntry<T> > {
public:
	LisDWord():m_pFirst(0),m_pLast(0),m_iNum(0) {  };
	~LisDWord() { Clear(); }
	void Delete(T *b);
	void Clear();
	T *Add();
	T *Add(MListDWordEntry<T> *entry);

	MListDWordEntry<T> *m_pFirst;
	MListDWordEntry<T> *m_pLast;


	DINT m_iNum;
};

//------------------------------------------------------------------
//
//------------------------------------------------------------------
template <class T>
void LisDWord<T>::Clear()
{
	m_pFirst = m_pLast = 0;
	m_iNum = 0;
	Allocator <MListDWordEntry<T> >::Clear();
	
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
template <class T>
void LisDWord<T>::Delete(T *b)
{
	MListDWordEntry<T> *entry = m_pFirst;
	while (entry)
	{
		 if (&entry->m_pData == b)
		 {
			if (entry == m_pLast)
				m_pLast = entry->m_pPrev;
			if (entry == m_pFirst)
			m_pFirst = entry->m_pNext;
			entry->Reset();
			Free(entry);
			m_iNum--;
			break;
		}
		entry = entry->m_pNext;
	}
	if (!m_pFirst) m_pLast = 0;
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
template <class T>
T *LisDWord<T>::Add()
{
	MListDWordEntry<T> *entry = Allocator <MListDWordEntry<T> >::Alloc(0);
	entry->Init(m_pLast);
	return Add(entry);
}
//---------------------------------------
//
//---------------------------------------
template <class T>
T *LisDWord<T>::Add(MListDWordEntry<T> *entry)
{
	if (!m_pFirst) m_pFirst = entry;
	if ((m_pLast = entry))
	{
		++m_iNum;
		return &entry->m_pData;
	}
	return 0;
}

template <class MAP_VALUE, class _ALLOCATOR_= LisDWord< RBDWordNode<MAP_VALUE> > >
class RBMapDword : public _ALLOCATOR_ {
public:
	enum  { _BLACK, _RED };
	RBMapDword() {  pRoot = &m_Nil; m_Nil.Init(&m_Nil, &m_Nil, &m_Nil, _BLACK); };
	~RBMapDword() { Clear(); };
	RBMapDword(RBMapDword &d) { pRoot = &m_Nil; m_Nil.Init(&m_Nil, &m_Nil, &m_Nil, _BLACK); *this = d; };
	MAP_VALUE &operator [] (DWORD key) { return Add(key, 0); }
	MAP_VALUE &Add(DWORD &key, MAP_VALUE &b) { return Add(key, &b); }
	bool Lookup(DWORD key, MAP_VALUE &b);
	bool Lookup(DWORD key, MAP_VALUE **b = 0);
	void Delete(DWORD key) { RBDWordNode<MAP_VALUE> *p = Search(pRoot, key); if (p != &m_Nil) DeleteNode(p); }
	MAP_VALUE &Value(DWORD key) { return Search(pRoot, key)->value; }

	void Clear();
	RBMapDword &operator = (RBMapDword &a);	
	void GetSortedRBData(MArray<RBData<MAP_VALUE, DWORD> > &arr);
	void GetSortedRBData2(MArray< RBData<MAP_VALUE *, DWORD> > &arr);

	RBDWordNode<MAP_VALUE> *Root() { return pRoot; }
	RBDWordNode<MAP_VALUE> *Nil() { return &m_Nil; }
	const RBDWordNode<MAP_VALUE> *Max() { RBDWordNode<MAP_VALUE> *node = pRoot; while (node->right != &m_Nil) node = node->right; return node; }
	const RBDWordNode<MAP_VALUE> *Min() { RBDWordNode<MAP_VALUE> *node = pRoot; while (node->left != &m_Nil)	node = node->left; return node; }
	DWORD MaxKey() { RBDWordNode<MAP_VALUE> *node = pRoot; while (node->right != &m_Nil) node = node->right; return node->key; }
	DWORD MinKey() { RBDWordNode<MAP_VALUE> *node = pRoot; while (node->left != &m_Nil)	node = node->left; return node->key; }
	DINT GetLen() { return _ALLOCATOR_::m_iNum; }

protected:
	bool AllocNode(RBDWordNode<MAP_VALUE> **node);
	void FreeNode(RBDWordNode<MAP_VALUE> *node);
	MAP_VALUE &Add(DWORD &key, MAP_VALUE *b);
	void DeleteNode (RBDWordNode<MAP_VALUE> *node);
	void DeleteFix(RBDWordNode<MAP_VALUE> *node);
	RBDWordNode<MAP_VALUE> *Search(RBDWordNode<MAP_VALUE> *node, DWORD &key);
	bool Insert(DWORD &key, MAP_VALUE *value, RBDWordNode<MAP_VALUE> **node);
	void LRotate (RBDWordNode<MAP_VALUE> *node);
	void RRotate (RBDWordNode<MAP_VALUE> *node);
	void Copy(RBMapDword &map, RBDWordNode<MAP_VALUE> *node, RBDWordNode<MAP_VALUE> *parent, RBDWordNode<MAP_VALUE> **self)
	{
		if (node == &map.m_Nil)
			*self = &m_Nil;
		else
		{
			AllocNode(self);
			(*self)->value = node->value;
			(*self)->dw_key = node->dw_key;
			(*self)->parent = parent;
			
			Copy(map, node->left, *self, &(*self)->left);
			Copy(map, node->right, *self, &(*self)->right);
		}
	}

protected:
	RBDWordNode<MAP_VALUE> m_Nil;
public:
	RBDWordNode<MAP_VALUE> *pRoot; 
};

//-------------------------------------
//
//-------------------------------------
template <class MAP_VALUE, class _ALLOCATOR_ >
void RBMapDword<MAP_VALUE, _ALLOCATOR_>::DeleteNode (RBDWordNode<MAP_VALUE> *node)
{
	
	RBDWordNode<MAP_VALUE> *tmpX, *tmpY;
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
	{
		node->key = tmpY->key;	
		node->value = tmpY->value;	
	}
	if (tmpY->color == _BLACK)
        DeleteFix(tmpX);
	FreeNode(tmpY);
	m_Nil.Init(&m_Nil, &m_Nil, &m_Nil, _BLACK);
}

template <class MAP_VALUE, class _ALLOCATOR_>
void RBMapDword<MAP_VALUE, _ALLOCATOR_>::Clear() 
{  
	_ALLOCATOR_::Clear(); 
	pRoot = &m_Nil; 
}
template <class MAP_VALUE, class _ALLOCATOR_>
inline bool RBMapDword<MAP_VALUE, _ALLOCATOR_>::Lookup(DWORD key, MAP_VALUE &b) 
{ 
	RBDWordNode<MAP_VALUE> *p = Search(pRoot, key); 
	if (p != &m_Nil) 
	{
		b = p->value; 
		return 1; 
	} 
	return 0; 
}
template <class MAP_VALUE, class _ALLOCATOR_>
inline bool RBMapDword<MAP_VALUE, _ALLOCATOR_>::Lookup(DWORD key, MAP_VALUE **b) 
{ 
	RBDWordNode<MAP_VALUE> *p = Search(pRoot, key); 
	if (p != &m_Nil) 
	{ 
		if (b) 
			*b = &p->value; 
		return 1; 
	} 
	return 0; 
}

//-------------------------------------
//
//-------------------------------------

template <class MAP_VALUE, class _ALLOCATOR_>
void RBMapDword<MAP_VALUE, _ALLOCATOR_>::DeleteFix(RBDWordNode<MAP_VALUE> *node)
{
	while (node != pRoot && node->color == _BLACK) 
	{
		if (node == node->parent->left) 
		{
			RBDWordNode<MAP_VALUE> *tmp = node->parent->right;
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
			RBDWordNode<MAP_VALUE> *tmp = node->parent->left;
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

template <class MAP_VALUE, class _ALLOCATOR_>
MAP_VALUE &RBMapDword<MAP_VALUE, _ALLOCATOR_>::Add(DWORD &key, MAP_VALUE *value)
{
	RBDWordNode<MAP_VALUE> *node = 0, *last;
	if (!Insert(key, value, &node)) 
	{
		if (node)
		{
			if (value) node->value = *value;
			return node->value;
		}
		return m_Nil.value;
	}
	node->color = _RED;
	last = node;
	while (node != pRoot && node->parent->color == _RED)
	{
		if (node->parent == node->parent->parent->left)
		{
			RBDWordNode<MAP_VALUE> *temp = node->parent->parent->right;
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
			RBDWordNode<MAP_VALUE> *temp = node->parent->parent->left;
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
	return last->value;
}

//----------------------------------------------
//
//----------------------------------------------
template <class MAP_VALUE, class _ALLOCATOR_>
inline RBDWordNode<MAP_VALUE> *RBMapDword<MAP_VALUE, _ALLOCATOR_>::Search(RBDWordNode<MAP_VALUE> *node, DWORD &key)
{
		while (node != &m_Nil && node->key != key)
				node = node->key > key ? node->left : node->right;
	return node;
}
//----------------------------------------------
//
//----------------------------------------------
template <class MAP_VALUE, class _ALLOCATOR_>
inline void RBMapDword<MAP_VALUE, _ALLOCATOR_>::FreeNode(RBDWordNode<MAP_VALUE> *node)
{
	_ALLOCATOR_::Delete(node);

}

//----------------------------------------------
//
//----------------------------------------------
template <class MAP_VALUE, class _ALLOCATOR_>
inline bool RBMapDword<MAP_VALUE, _ALLOCATOR_>::AllocNode(RBDWordNode<MAP_VALUE> **node)
{
	if ((*node = _ALLOCATOR_::Add()))
		(*node)->Init(&m_Nil, &m_Nil, &m_Nil, _BLACK);
	return *node != 0;
}

//----------------------------------------------
//
//----------------------------------------------
template <class MAP_VALUE, class _ALLOCATOR_>
bool RBMapDword<MAP_VALUE, _ALLOCATOR_>::Insert(DWORD &key, MAP_VALUE *value, RBDWordNode<MAP_VALUE> **node)
{

	*node = 0;
	if (pRoot == &m_Nil)
	{
		if (!AllocNode(&pRoot)) return 0;
		pRoot->key = key;
		if (value)
			pRoot->value = *value;
		*node = pRoot;
		return true;
	}

	RBDWordNode<MAP_VALUE> *nodeParent = &m_Nil;
	RBDWordNode<MAP_VALUE> *temp = pRoot;
	int n = 0;
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
	if (!AllocNode(node)) return 0;
	(*node)->key = key;
	if (value)
		(*node)->value = *value;
	(*node)->parent = nodeParent;
	if (n) //(*node)->key < nodeParent->key)
		nodeParent->left = *node;
	else
		nodeParent->right = *node;
	return true;
}

//----------------------------------------------
//
//----------------------------------------------
template <class MAP_VALUE, class _ALLOCATOR_>
inline void RBMapDword<MAP_VALUE, _ALLOCATOR_>::LRotate (RBDWordNode<MAP_VALUE> *node)
{

	RBDWordNode<MAP_VALUE> *temp = node->right; 
	if (temp != &m_Nil)
	{
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
}
//----------------------------------------------
//
//----------------------------------------------
template <class MAP_VALUE, class _ALLOCATOR_>
inline void RBMapDword<MAP_VALUE, _ALLOCATOR_>::RRotate (RBDWordNode<MAP_VALUE> *node)
{
	RBDWordNode<MAP_VALUE> *temp = node->left;
	if (temp != &m_Nil)
	{
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
}
//---------------------------------------
//
//---------------------------------------
template <class MAP_VALUE, class _ALLOCATOR_>
RBMapDword<MAP_VALUE, _ALLOCATOR_> &RBMapDword<MAP_VALUE, _ALLOCATOR_>::operator = (RBMapDword<MAP_VALUE, _ALLOCATOR_> &a) 
{ 
	Clear();
	if (a.pRoot != &a.m_Nil)
		Copy(a, a.pRoot, &m_Nil, &pRoot);
	return *this;  
}
template <class MAP_VALUE, class _ALLOCATOR_>
void RBMapDword<MAP_VALUE, _ALLOCATOR_>::GetSortedRBData(MArray<RBData<MAP_VALUE, DWORD> > &arr)
{
	arr.SetLen(_ALLOCATOR_::Len());
	if (pRoot == &m_Nil) return;	
	RBDWordNode<MAP_VALUE> *node = pRoot;	   
	DINT len = 16;
	RBData<MAP_VALUE, DWORD> *pArr = arr.GetPtr();
	MArray<RBDWordNode<MAP_VALUE> *> stack(16);
	RBDWordNode<MAP_VALUE> **test, **pStack = stack.GetPtr();
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
			while(node != &m_Nil);
		}
		node = *(--pStack);
		pArr->Set(node->key, node->value);
		pArr++;
		if((fCheck = node->right != &m_Nil))
		node = node->right;
	}
	while (fCheck || pStack > test);

}
template <class MAP_VALUE, class _ALLOCATOR_>
void RBMapDword<MAP_VALUE, _ALLOCATOR_>::GetSortedRBData2(MArray< RBData<MAP_VALUE *, DWORD> > &arr)
{
	arr.SetLen(_ALLOCATOR_::Len());
	if (pRoot == &m_Nil) return;	
	RBDWordNode<MAP_VALUE> *node = pRoot;	   
	DINT len = 16;
	MArray<RBDWordNode<MAP_VALUE> *> stack(16);
	RBDWordNode<MAP_VALUE> **test, **pStack = stack.GetPtr();

	RBData<MAP_VALUE *, DWORD> *pArr = arr.GetPtr();
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
			while(node != &m_Nil);
		}
		node = *(--pStack);
		pArr->Set(node->key, &node->value);

		*pArr++;
		if((fCheck = node->right != &m_Nil))
		node = node->right;
	}
	while (fCheck || pStack > test);

}

#endif
