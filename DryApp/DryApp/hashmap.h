///////////////////////////////////////////////////
// hashmap.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////

#ifndef __MHashMap__
#define	 __MHashMap__

#include "rbmap.h"
#ifndef __PLUK_LINUX__
#pragma warning (disable : 4786)
#endif
#pragma pack(push) //NADA
#pragma pack(4)

struct GraphEdge {
	DINT pv;
	union {
	DINT pw;
	float fw;
	};
	GraphEdge() : pv(0xFFFFFFFF),pw(0xFFFFFFFF) { } 
	GraphEdge(DINT _pv, DINT _pw):pv(_pv), pw(_pw) {}
	void Set(DINT _pv, DINT _pw) { pv = _pv; pw = _pw; }
	GraphEdge &Init(DINT _pv, DINT _pw) { pv = _pv; pw = _pw; return *this; }
	bool operator > (const GraphEdge &e) { return pv > e.pv || (pv == e.pv && pw > e.pw); }
	bool operator < (const GraphEdge &e) { return pv < e.pv || (pv == e.pv && pw < e.pw); }
	bool operator == (const GraphEdge &e) { return pv == e.pv && pw == e.pw; }
	bool operator != (const GraphEdge &e) { return pv != e.pv || pw != e.pw; }
};
#pragma pack(pop)



template <class MAP_VALUE, class MAP_KEY>
struct HMapTreeNode : public RBData<MAP_VALUE, MAP_KEY> {
	HMapTreeNode *left;
	HMapTreeNode *right;
	HMapTreeNode () { left = right = 0; };
	~HMapTreeNode () {};
	HMapTreeNode (HMapTreeNode &r) { operator = (r); };
	HMapTreeNode &operator = (HMapTreeNode &r) { RBData<MAP_VALUE, MAP_KEY>::key = r.key; RBData<MAP_VALUE, MAP_KEY>::value = r.value; left = r.left; right = r.right; return *this; };
	void Init(HMapTreeNode *_left, HMapTreeNode *_right)
	{
		left = _left;
		right = _right;
	}
	void Init(HMapTreeNode *_left, HMapTreeNode *_right, MAP_KEY &_key, MAP_VALUE &_value)
	{
		left = _left;
		right = _right;
		RBData<MAP_VALUE, MAP_KEY>::key	= _key;
		RBData<MAP_VALUE, MAP_KEY>::value = _value;
	}
};

template <class MAP_VALUE, class MAP_KEY,  class ARRAY = MMalloc<HMapTreeNode<MAP_VALUE, MAP_KEY> *>, class LIST_ALLOC = AllocatorSimple<HMapTreeNode<MAP_VALUE, MAP_KEY> > >
class MHashMap : protected ARRAY, public LIST_ALLOC {
public:
	MHashMap(int _iRank = 2, int max_alloc_block = 1024, int min_alloc_block = 4, int _iStartSize = 128) {  
			LIST_ALLOC::m_mAlloc._Max_Alloc_ = max_alloc_block;
			LIST_ALLOC::m_mAlloc._MinAlloc =   min_alloc_block ;
			iStartSize = _iStartSize;
			iRank = _iRank;
#ifdef _WIN32_WCE
				iMaxSize =0x100000;
#else
				iMaxSize =0x2000000;
#endif
				ARRAY::ReAlloc(iStartSize);
#ifdef __MASK_HASH__
				iMask = 0x7f; 
#endif
				m_fNewDelele = true;
				stack.SetLen(16);
				tab.SetLen(16);	
				memset(ARRAY::ptr, 0, ARRAY::Len * sizeof(HMapTreeNode<MAP_VALUE, MAP_KEY> *)); 
			}
	virtual ~MHashMap();
	MHashMap(MHashMap &m) { 
		LIST_ALLOC::m_mAlloc._Max_Alloc_ = m.LIST_ALLOC::m_mAlloc._Max_Alloc_;
		LIST_ALLOC::m_mAlloc._MinAlloc =   m.LIST_ALLOC::m_mAlloc._MinAlloc;
		iStartSize = m.iStartSize;
		iRank = m.iRank;
#ifdef _WIN32_WCE
			iMaxSize =0x100000;
#else
			iMaxSize =0x2000000;
#endif
			m_fNewDelele = true;
	stack.SetLen(16);
	tab.SetLen(16);	
	ARRAY::ReAlloc(iStartSize);
#ifdef __MASK_HASH__
	iMask = 0x7f;
#endif
	m_fNewDelele = true;
	stack.SetLen(16);
	tab.SetLen(16);
	memset(ARRAY::ptr, 0, ARRAY::Len * sizeof(HMapTreeNode<MAP_VALUE, MAP_KEY> *));
	*this = m;
	}
	MHashMap &operator = (MHashMap &m);  
	void CreateNew(bool fNewDelele) { m_fNewDelele = fNewDelele; } 
	MAP_VALUE &Add(MAP_KEY &key, MAP_VALUE &value);
	MAP_VALUE &Value(MAP_KEY key);
	MAP_VALUE &operator[] (MAP_KEY key) { return Value(key); }
	void Build(MArray<RBData<MAP_VALUE, MAP_KEY> > &ar);
	void GetData(MArray<RBData<MAP_VALUE, MAP_KEY> > &res);
	bool operator == (MHashMap &m);
	HMapTreeNode<MAP_VALUE, MAP_KEY> **GetRoot() { return  ARRAY::GetPtr(); }
	DINT GetLenRoot() { return  ARRAY::GetLen(); } 
	HMapTreeNode<MAP_VALUE, MAP_KEY> *Nil() { return 0; } 
	bool Lookup(MAP_KEY key, MAP_VALUE &b);
	bool Lookup(MAP_KEY key, MAP_VALUE **b = 0);
	void Delete(MAP_KEY key);
	void Clear();
	DINT Len() const { return LIST_ALLOC::m_iNum; }
	virtual MListEntry<HMapTreeNode<MAP_VALUE, MAP_KEY> > *Last() { return LIST_ALLOC::Last(); }
	virtual MListEntry<HMapTreeNode<MAP_VALUE, MAP_KEY> > *First()  { return LIST_ALLOC::First(); }
	int &MaxSizeTable() { return iMaxSize; }
protected:
	virtual void FreeNode(HMapTreeNode<MAP_VALUE, MAP_KEY> *node);
	virtual bool AllocNode(HMapTreeNode<MAP_VALUE, MAP_KEY> **node);
	bool SetRoot(HMapTreeNode<MAP_VALUE, MAP_KEY> **root, MAP_KEY &key, MAP_VALUE *value, HMapTreeNode<MAP_VALUE, MAP_KEY> **node);
	bool AllocNode(MAP_KEY &key, MAP_VALUE *value, HMapTreeNode<MAP_VALUE, MAP_KEY> **node);
	bool Insert(HMapTreeNode<MAP_VALUE, MAP_KEY> **root, MAP_KEY &key, MAP_VALUE *value, HMapTreeNode<MAP_VALUE, MAP_KEY> **node);
	void GetData(HMapTreeNode<MAP_VALUE, MAP_KEY> *root, MArray<HMapTreeNode<MAP_VALUE, MAP_KEY> *> &stack, MArray<HMapTreeNode<MAP_VALUE, MAP_KEY> *> &res);
	void Delete(HMapTreeNode<MAP_VALUE, MAP_KEY> **root, MAP_KEY &key);
	void Rebuild();
	int GetHash(LPCSTR p) const;
	int GetHash(LPCWSTR p) const;
	int GetHash(void *p) const;
#ifdef __PLUK_LINUX__
	int GetHash(POINT &p) { return GetHash(p.x + p.y); }
	int GetHash(GraphEdge &p) { return GetHash(p.pv + p.pw); }
#else
int GetHash(const POINT &p) { return GetHash(p.x + p.y); }
	int GetHash(const GraphEdge &p) { return GetHash(p.pv + p.pw); }
	
#endif
	int _GetHash(DWORD p) const;
	int GetHash(size_t p) const { return _GetHash((DWORD)p); }
	int GetHash(BYTE *p, int len) const;
protected:
	DINT iStartSize, iRank, iMaxSize;
#ifdef __MASK_HASH__
	int iMask;
#endif
	MArray<HMapTreeNode<MAP_VALUE, MAP_KEY> *> tab, stack;

	bool m_fNewDelele;
	HMapTreeNode<MAP_VALUE, MAP_KEY> nil; 

protected:
class iteratorIT 
{ 
	HMapTreeNode<MAP_VALUE, MAP_KEY> *t, *node;
	MArray<HMapTreeNode<MAP_VALUE, MAP_KEY> *> stack;
	HMapTreeNode<MAP_VALUE, MAP_KEY> **test, **pStack;
	bool fCheck;
	HMapTreeNode<MAP_VALUE, MAP_KEY> *root;
	int len;
public:
	iteratorIT() { t = 0; root = 0; node = 0; pStack = 0; fCheck = false; len = 0; test = 0; }
	RBData<MAP_VALUE, MAP_KEY> *begin(HMapTreeNode<MAP_VALUE, MAP_KEY> *_root)
	{ 
		root = _root;
		if (!root) return 0;	
		node = root; 
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
		while(node);
		node = *(--pStack);
		t = node;
		if((fCheck = node->right != 0))
			node = node->right;

		return (RBData<MAP_VALUE, MAP_KEY> *)t; 
	}
	RBData<MAP_VALUE, MAP_KEY> *next()
	{ 
		if (!t)
			return 0;
		if (t && (fCheck || pStack > test))
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
				while(node);
			}
			node = *(--pStack);
			t = node;
			if((fCheck = node->right != 0))
			node = node->right;
		}
		else
			t = 0;

		return (RBData<MAP_VALUE, MAP_KEY> *)t; 
	}
	bool end()
	{ return !t; }
};


public:


	class iterator;
	friend class iterator;

class iterator {
public:
	iterator(MHashMap &m): M(m){ hi = M.ARRAY::GetLen(); 
		len = M.ARRAY::GetLen(); }
	~iterator() {  }
	RBData<MAP_VALUE, MAP_KEY> *begin() 
	{
		hi = 0;
		while(hi < len && !M.ptr[hi]) ++hi;
		if (hi == len)
			return 0;
		return it.begin(M.ptr[hi]);
	};
	RBData<MAP_VALUE, MAP_KEY> *next() 
	{
		
		if (hi == len)
			return 0;
		RBData<MAP_VALUE, MAP_KEY> *t = it.next();
		if (!t)
		{
			++hi;
			while(hi < len && !M.ptr[hi]) ++hi;
			if (hi == len)
				return 0;
			return it.begin(M.ptr[hi]);
		}
		return t;
	};
	bool end() { return hi == len; }
	DINT hi;
	DINT len;
	iteratorIT it;
	MHashMap &M;

};


};
template <class MAP_VALUE, class MAP_KEY, class CMP = RBCmp<MAP_KEY>, class LIST_ALLOC = AllocatorSimple<RBTreeNode<MAP_VALUE, MAP_KEY> > >
class RBHashMap : protected MMalloc<RBTreeNode<MAP_VALUE, MAP_KEY> *>, protected RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC> {
public:
	typedef typename RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::iterator RBiterator;
	typedef MMalloc<RBTreeNode<MAP_VALUE, MAP_KEY> *> RBMALLOCH;
	typedef RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC> RBMMAPH;

	RBHashMap(int start_hash_size = 128, int _iRank = 2, int max_alloc_block = 1024, int min_alloc_block = 4) {  
		RBMMAPH::m_mAlloc._Max_Alloc_ = max_alloc_block;
		RBMMAPH::m_mAlloc._MinAlloc =   min_alloc_block ;
				iStartSize = start_hash_size;
				iRank = _iRank;
#ifdef _WIN32_WCE
				iMaxSize =0x100000;
#else
				iMaxSize =0x2000000;
#endif
				RBMALLOCH::ReAlloc(iStartSize);
#ifdef __MASK_HASH__
				iMask = start_hash_size - 1; 
#endif
				tab.SetLen(20);
				stack.SetLen(20);
				for (int i = 0; i < RBMALLOCH::Len; ++i) RBMALLOCH::ptr[i] = RBMMAPH::Nil();
			}
	virtual ~RBHashMap() { Clear(); }
	RBHashMap(RBHashMap &m) {
		RBMMAPH::m_mAlloc._Max_Alloc_ = m.RBMMAPH::m_mAlloc._Max_Alloc;
			RBMMAPH::m_mAlloc._MinAlloc =   m.RBMMAPH::m_mAlloc._MinAlloc;
					iStartSize = m.iStartSize;
					iRank = m.iRank;
	#ifdef _WIN32_WCE
					iMaxSize =0x100000;
	#else
					iMaxSize =0x2000000;
	#endif
	tab.SetLen(20);
	stack.SetLen(20);
	for (int i = 0; i < RBMALLOCH::Len; ++i) RBMALLOCH::ptr[i] = RBMMAPH::Nil();
	  *this = m; }
	RBHashMap &operator = (RBHashMap &m);  
	void CreateNew(bool fNewDelele) { RBMMAPH::m_fNewDelele = fNewDelele; }
	void KeyFromCompareFunc(bool b) { RBMMAPH::m_fKeyFromCompareFunc = b;  }
	MAP_VALUE &Add(MAP_KEY &key, MAP_VALUE &value);
	MAP_VALUE &Value(MAP_KEY key);
	MAP_VALUE &operator[] (MAP_KEY key) { return Value(key); }
	void Build(MArray<RBData<MAP_VALUE, MAP_KEY> > &ar);
	void GetData(MArray<RBData<MAP_VALUE, MAP_KEY> > &res);

	RBTreeNode<MAP_VALUE, MAP_KEY> **GetRoot() { return  MMalloc<RBTreeNode<MAP_VALUE, MAP_KEY> *>::GetPtr(); }
	int GetLenRoot() { return  MMalloc<RBTreeNode<MAP_VALUE, MAP_KEY> *>::GetLen(); } 
	RBTreeNode<MAP_VALUE, MAP_KEY> *Nil() { return RBMMAPH::Nil(); }
	bool Lookup(MAP_KEY key, MAP_VALUE &b);
	bool Lookup(MAP_KEY key, MAP_VALUE **b = 0);
	void Delete(MAP_KEY key);
	void Clear();
	int Len() const { return LIST_ALLOC::m_iNum; }
	int &MaxSizeTable() { return iMaxSize; }
	virtual ListNotNewEntry<RBTreeNode<MAP_VALUE, MAP_KEY> > *First() { return RBMMAPH::First(); }
	virtual ListNotNewEntry<RBTreeNode<MAP_VALUE, MAP_KEY> > *Last() { return RBMMAPH::Last(); }

protected:
	void Rebuild();
	int GetHash(LPCSTR p);
	int GetHash(LPCWSTR p);
	int GetHash(void *p)const;
	int GetHash(GraphEdge &p) { return GetHash(p.pv + p.pw); }
	int _GetHash(DWORD p)const;
	int GetHash(size_t p) const { return _GetHash((DWORD)p); }
	int GetHash(BYTE *p, int len);
	void GetData(RBTreeNode<MAP_VALUE, MAP_KEY> *root, MArray<RBTreeNode<MAP_VALUE, MAP_KEY> *> stack, MArray<RBTreeNode<MAP_VALUE, MAP_KEY> *> &res);

	int iStartSize, iRank, iMaxSize;
#ifdef __MASK_HASH__
	int iMask;
#endif
	MArray<RBTreeNode<MAP_VALUE, MAP_KEY> *> tab, stack;

public:
	class iterator;
	friend class iterator;
class iterator {
public:
		iterator(RBHashMap &m): M(m){ hi = M.RBMALLOCH::GetLen();
			it = new RBiterator(m);
			len = M.RBMALLOCH::GetLen(); }
	~iterator() { delete it; }
	RBData<MAP_VALUE, MAP_KEY> *begin() 
	{
		hi = 0;
		while(hi < len && M.ptr[hi] == &M.m_Nil) ++hi;
		if (hi == len)
			return 0;
		M.pRoot = M.ptr[hi];
		return it->begin();
	};
	RBData<MAP_VALUE, MAP_KEY> *next() 
	{
		
		if (hi == len)
			return 0;
		RBData<MAP_VALUE, MAP_KEY> *t = it->next();
		if (!t)
		{
			++hi;
			while(hi < len && M.ptr[hi] == &M.m_Nil) ++hi;
			if (hi == len)
				return 0;
			M.pRoot = M.ptr[hi];
			return it->begin();
		}
		return t;
	};
	bool end() const { return hi == len; }
	DINT hi;
	DINT len;
	RBiterator *it;
	RBHashMap &M;

};


};
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::~MHashMap() 
{ 	
	if (m_fNewDelele)
		LIST_ALLOC::RunAllDataDestructor();
	LIST_ALLOC::Clear(); 
	ARRAY::Free();
}

template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline void MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::FreeNode(HMapTreeNode<MAP_VALUE, MAP_KEY> *node)
{
	if (m_fNewDelele) node->~HMapTreeNode<MAP_VALUE, MAP_KEY>();
	LIST_ALLOC::Delete(node);

}

template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline bool MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::AllocNode(HMapTreeNode<MAP_VALUE, MAP_KEY> **node)
{
	*node = LIST_ALLOC::Add();
	if (*node)
	{
		if (m_fNewDelele) *node = new (*node) HMapTreeNode<MAP_VALUE, MAP_KEY>();
		(*node)->Init(0, 0);
	}
	return *node != 0;
}
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline bool MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::SetRoot(HMapTreeNode<MAP_VALUE, MAP_KEY> **root, MAP_KEY &key, MAP_VALUE *value, HMapTreeNode<MAP_VALUE, MAP_KEY> **node)
{
	if (*node)
		*root = *node;
	else
	{
		if (!AllocNode(root)) return 0;
		(*root)->key = key;
		if (value)
			(*root)->value = *value;
		*node = *root;

	}
	return true;
}
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline bool MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::AllocNode(MAP_KEY &key, MAP_VALUE *value, HMapTreeNode<MAP_VALUE, MAP_KEY> **node)
{
	if (!*node)
	{
		if (!AllocNode(node)) return 0;
		(*node)->key = key;
		if (value)
			(*node)->value = *value;
	}
	return true;
}
//-------------------------------------
//
//-------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
void MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::Delete(HMapTreeNode<MAP_VALUE, MAP_KEY> **root, MAP_KEY &key)
{
	HMapTreeNode<MAP_VALUE, MAP_KEY> *parentY = 0, *node = *root;
	HMapTreeNode<MAP_VALUE, MAP_KEY> *tmpX, *tmpY;
	while (node && node->key != key)
	{
		parentY = node;
		node = node->key > key ? node->left : node->right;
	}
	if (!node) return;
	if (!node->left || !node->right)
		tmpY = 	node;
	else
	{
		parentY = node;
		tmpY = node->right;
		while (tmpY->left) 
		{
			parentY = tmpY;
			tmpY = tmpY->left;
		}
	}
	tmpX = tmpY->left ? tmpY->left : tmpY->right;
	if (!parentY)
		*root = tmpX;
	else
	{
		if (parentY->left == tmpY)
			parentY->left = tmpX;
		else
			parentY->right = tmpX;
	}
	if (tmpY != node)
	{
		node->key = tmpY->key;	
		node->value = tmpY->value;	
	}
	FreeNode(tmpY);

}

//----------------------------------------------
//
//----------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
bool MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::Insert(HMapTreeNode<MAP_VALUE, MAP_KEY> **root, MAP_KEY &key, MAP_VALUE *value, HMapTreeNode<MAP_VALUE, MAP_KEY> **node)
{
	if (!*root) return SetRoot(root, key, value, node);
	HMapTreeNode<MAP_VALUE, MAP_KEY> *nodeParent = 0, *temp = *root;
	int n = 0;
	while (temp)
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
	if (AllocNode(key, value, node))
	{
		if (n)
			nodeParent->left = *node;
		else
			nodeParent->right = *node;
		return true;
	}
	return false;
}

//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline void MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::Build(MArray<RBData<MAP_VALUE, MAP_KEY> > &ar)
{
	Clear();
	RBData<MAP_VALUE, MAP_KEY> *p = ar.GetPtr();
	int len = ar.GetLen();
	while(len--)
	{
		Value(p->key) = p->value;
		++p;
	}
}
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline void MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::GetData(MArray<RBData<MAP_VALUE, MAP_KEY> > &res) 
{
	res.SetLen(LIST_ALLOC::m_iNum);
	RBData<MAP_VALUE, MAP_KEY> *ptab = res.GetPtr();
	iterator it(*this);
	for (RBData<MAP_VALUE, MAP_KEY> *t = it.begin(); t; t = it.next())
			*ptab++ = *t;
}

template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
void MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::GetData(HMapTreeNode<MAP_VALUE, MAP_KEY> *root, MArray<HMapTreeNode<MAP_VALUE, MAP_KEY> *> &stack, MArray<HMapTreeNode<MAP_VALUE, MAP_KEY> *> &res) 
{
		bool fCheck = true;
		HMapTreeNode<MAP_VALUE, MAP_KEY> **test, **pStack = stack.GetPtr();
		test = pStack;
		DINT len = stack.GetLen();
		do
		{
			if (fCheck)
				do
				{
					if (pStack - test == len)
					{
						stack.SetLen(len << 1);
						pStack = test = stack.GetPtr();
						pStack += len;
						len <<= 1;
					}
					*pStack++ = root;
					root = root->left;
				}
				while(root);
			root = *(--pStack);
			res.Add(root);
			if((fCheck = root->right != 0))
				root = root->right;
		}
		while (root && (fCheck || pStack > test));
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
void MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::Rebuild() 
{
	HMapTreeNode<MAP_VALUE, MAP_KEY> **ptab, **ptmp = ARRAY::ptr, **tmp = ARRAY::ptr;
	DINT j, len = ARRAY::Len;
#ifdef __MASK_HASH__
	int m = iMask;
#endif
	ARRAY::ptr = 0;
	ARRAY::RealSize = ARRAY::Len = 0;
#ifdef __MASK_HASH__
	iMask = len << iRank;
	ARRAY::ReAlloc(iMask);
#else
	ARRAY::ReAlloc(len << iRank);
#endif

	if (!ARRAY::ptr)
	{
		iMaxSize = ARRAY::RealSize = ARRAY::Len = len;
		ARRAY::ptr = ptmp;
#ifdef __MASK_HASH__
		iMask = m;
#endif
		return;
		
		
	}
	
#ifdef __MASK_HASH__
	--iMask;
#endif
	tab.SetLen(LIST_ALLOC::m_iNum);
	memset(ARRAY::ptr, 0, ARRAY::Len * sizeof(HMapTreeNode<MAP_VALUE, MAP_KEY> *)); 
	DINT n = LIST_ALLOC::m_iNum;
	while (len--)
	{
		if (*ptmp)
		{
			tab.SetLen(0);
			GetData(*ptmp, stack, tab);
			ptab = tab.GetPtr();
			j = tab.GetLen();
			while(j--)
			{
				(*ptab)->Init(0, 0);
				Insert(ARRAY::ptr + GetHash((*ptab)->key), (*ptab)->key, &(*ptab)->value, ptab);
				++ptab;
			}
		}
		++ptmp;
	}
	LIST_ALLOC::m_iNum = n;
	ARRAY::Free(tmp);

	
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC> &MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::operator = (MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC> &m)
{
	Clear();
	iMaxSize = m.iMaxSize;
	ARRAY::ReAlloc(m.ARRAY::Len);
#ifdef __MASK_HASH__
	iMask = m.iMask;
#endif
	iRank = m.iRank;
	memset(ARRAY::ptr, 0, ARRAY::Len * sizeof(HMapTreeNode<MAP_VALUE, MAP_KEY> *)); 
	iterator it(m);
	for (RBData<MAP_VALUE, MAP_KEY> *t = it.begin(); t; t = it.next())
		Value(t->key) = t->value;
	return *this;
}
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline bool MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::operator == (MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC> &m) 
{ 
	if (LIST_ALLOC::m_iNum == m.m_iNum)
	{ 
		iterator it(m);
		MAP_VALUE *val;
		for (RBData<MAP_VALUE, MAP_KEY> *t = it.begin(); t; t = it.next())
			if (!Lookup(t->key, &val) || t->value != *val) return false;
		return true;
	}
	return false;
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline void MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::Clear()
{
	if (m_fNewDelele)
		LIST_ALLOC::RunAllDataDestructor();
	LIST_ALLOC::Clear(); 
	LIST_ALLOC::m_iNum = 0;
	ARRAY::Free();
	ARRAY::ReAlloc(iStartSize);
#ifdef __MASK_HASH__
	iMask = iStartSize - 1; 
#endif
	memset(ARRAY::ptr, 0, ARRAY::Len * sizeof(HMapTreeNode<MAP_VALUE, MAP_KEY> *));  
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline MAP_VALUE &MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::Add(MAP_KEY &key, MAP_VALUE &value)
{
	MAP_VALUE &val = Value(key);
	val = value;
	return val;
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline void MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::Delete(MAP_KEY key)
{
	Delete(ARRAY::ptr + GetHash(key), key);
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline bool MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::Lookup(MAP_KEY key, MAP_VALUE &b)
{
	HMapTreeNode<MAP_VALUE, MAP_KEY> *node = ARRAY::ptr[GetHash(key)];
		while (node && node->key != key)
			node = node->key > key ? node->left : node->right;
	if (node) 
	{ 
		b = node->value; 
		return 1; 
	} 
	return false;
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline bool MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::Lookup(MAP_KEY key, MAP_VALUE **b)
{
	HMapTreeNode<MAP_VALUE, MAP_KEY> *node = ARRAY::ptr[GetHash(key)];
		while (node && node->key != key)
			node = node->key > key ? node->left : node->right;
	if (node) 
	{ 
		if (b) 
			*b = &node->value; 
		return 1; 
	} 
	return false;
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline MAP_VALUE &MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::Value(MAP_KEY key)
{
#ifdef __MASK_HASH__
	if (ARRAY::Len < iMaxSize && (m_iNum >> iRank) > iMask) Rebuild();
#else
	if (ARRAY::Len < iMaxSize && (LIST_ALLOC::m_iNum >> iRank) > ARRAY::Len) Rebuild();
#endif
	HMapTreeNode<MAP_VALUE, MAP_KEY> *node = 0;
	Insert(ARRAY::ptr + GetHash(key), key, 0, &node);
	return node ? node->value : nil.value;
}

//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline int MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::GetHash(LPCSTR p) const
{
	if (!p) return 0xFFFFFFFF;
	DWORD hash = 2166136261U;
	while (*p)
		hash = 16777619U * hash ^ *p++;
	return GetHash(hash);
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline int MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::GetHash(LPCWSTR p) const
{
	if (!p) return 0xFFFFFFFF;
	DWORD hash = 2166136261U;
	while (*p)
		hash = 16777619U * hash ^ *p++;
	return GetHash(hash);
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline int MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::GetHash(BYTE *p, int len) const
{
	if (!p) return 0xFFFFFFFF;
//	DWORD hash = 0x9e3779b9;
	DWORD hash = 2166136261U;
	while (len--)
		hash = 16777619U * hash ^ *p++;
	return GetHash(hash);
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline int MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::_GetHash(DWORD ID) const
{

#ifdef __MASK_HASH__
	return (int)(ID & iMask);
#else
	return (DWORD)(( ID % (DWORD)ARRAY::Len + ID / (DWORD)ARRAY::Len ) % (DWORD)ARRAY::Len );
#endif
}
template <class MAP_VALUE, class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline int MHashMap<MAP_VALUE, MAP_KEY, ARRAY, LIST_ALLOC>::GetHash(void *ID) const
{
	return GetHash((size_t)ID);
}

//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline void RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Build(MArray<RBData<MAP_VALUE, MAP_KEY> > &ar)
{
	Clear();
	RBData<DWORD, DWORD> *p = ar.GetPtr();
	DINT len = ar.GetLen();
	while(len--)
	{
		Value(p->key) = p->value;
		++p;
	}
}
template <class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline void RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::GetData(MArray<RBData<MAP_VALUE, MAP_KEY> > &res) 
{
	res.SetLen(LIST_ALLOC::m_iNum);
	RBData<MAP_VALUE, MAP_KEY> *ptab = res.GetPtr();
	iterator it(*this);
	for (RBData<MAP_VALUE, MAP_KEY> *t = it.begin(); t; t = it.next())
			*ptab++ = *t;
}
template <class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
void RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::GetData(RBTreeNode<MAP_VALUE, MAP_KEY> *root, MArray<RBTreeNode<MAP_VALUE, MAP_KEY> *> stack, MArray<RBTreeNode<MAP_VALUE, MAP_KEY> *> &res) 
{
		bool fCheck = true;
		RBTreeNode<MAP_VALUE, MAP_KEY> **test, **pStack = stack.GetPtr();
		test = pStack;
		DINT len = stack.GetLen();
		do
		{
			if (fCheck)
				do
				{
					if (pStack - test == len)
					{
						stack.SetLen(len << 1);
						pStack = test = stack.GetPtr();
						pStack += len;
						len <<= 1;
					}
					*pStack++ = root;
					root = root->left;
				}
				while(root != RBMMAPH::Nil());
			root = *(--pStack);
			res.Add(root);
			if((fCheck = root->right != RBMMAPH::Nil()))
				root = root->right;
		}
		while (root != RBMMAPH::Nil() && (fCheck || pStack > test));
}


//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
void RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Rebuild() 
{
	RBTreeNode<MAP_VALUE, MAP_KEY> **ptab, **ptmp = RBMALLOCH::ptr, **tmp = RBMALLOCH::ptr;
	DINT index, n, j, len = MMalloc<RBTreeNode<MAP_VALUE, MAP_KEY> *>::Len;
	RBMALLOCH::ptr = 0;
	RBMALLOCH::Len = 0;
	RBMALLOCH::RealSize = 0;
#ifdef __MASK_HASH__
	iMask = len << iRank;
#endif
	tab.SetLen(RBMMAPH::m_iNum);
#ifdef __MASK_HASH__
	RBMALLOCH::ReAlloc(iMask);
	--iMask;
#else
	RBMALLOCH::ReAlloc(len << iRank);
#endif
	
	for (DINT i = 0; i < RBMALLOCH::Len; ++i) RBMALLOCH::ptr[i] = RBMMAPH::Nil();
	n = RBMMAPH::m_iNum;
	while (len--)
	{
		if (*ptmp != RBMMAPH::Nil())
		{
			tab.SetLen(0);
			GetData(*ptmp, stack, tab);
			ptab = tab.GetPtr();
			j = tab.GetLen();
			while(j--)
			{
				(*ptab)->Init(RBMMAPH::Nil(), RBMMAPH::Nil(), RBMMAPH::Nil(), _BLACK);
				index = GetHash((*ptab)->key);
				RBMMAPH::pRoot = RBMALLOCH::ptr[index];
				RBMMAPH::Add((*ptab)->key, &(*ptab)->value, ptab);
				RBMALLOCH::ptr[index] = RBMMAPH::pRoot;
				++ptab;
			}
		}
		++ptmp;
	}
	RBMMAPH::m_iNum = n;
	RBMALLOCH::Free(tmp);
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC> &RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::operator = (RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC> &m)
{
	Clear();
	iMaxSize = m.iMaxSize;
	ReAlloc(m.Len);
#ifdef __MASK_HASH__
	iMask = m.iMask;
#endif
	iRank = m.iRank;

	for (DINT i = 0; i < RBMALLOCH::Len; ++i) RBMALLOCH::ptr[i] = RBMMAPH::Nil();
	iterator it(m);
	for (RBData<MAP_VALUE, MAP_KEY> *t = it.begin(); t; t = it.next())
		Value(t->key) = t->value;
	return *this;
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline void RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Clear()
{
	if (RBMMAPH::m_fNewDelele)
		RBMMAPH::RunAllDataDestructor();
	LIST_ALLOC::Clear(); 
	RBMMAPH::pRoot = RBMMAPH::Nil();
	RBMMAPH::m_iNum = 0;
	RBMALLOCH::Free();
	RBMALLOCH::ReAlloc(iStartSize);
#ifdef __MASK_HASH__
	iMask = iStartSize - 1; 
#endif
	for (DINT i = 0; i < RBMALLOCH::Len; ++i) RBMALLOCH::ptr[i] = RBMMAPH::Nil();
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline MAP_VALUE &RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Add(MAP_KEY &key, MAP_VALUE &value)
{
	MAP_VALUE &val = Value(key);
	val = value;
	return val;
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline void RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Delete(MAP_KEY key)
{
	DINT index = GetHash(key);
	
	RBMMAPH::pRoot = RBMALLOCH::ptr[index];
	RBMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Delete(key);
	RBMALLOCH::ptr[index] = RBMMAPH::pRoot;
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline bool RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Lookup(MAP_KEY key, MAP_VALUE &b)
{
	if (!RBMMAPH::m_iNum) return false;
	DINT index = GetHash(key);
	
	RBMMAPH::pRoot = RBMALLOCH::ptr[index];
	return RBMMAPH::Lookup(key, b);
}
template <class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline bool RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Lookup(MAP_KEY key, MAP_VALUE **b)
{
	if (!RBMMAPH::m_iNum) return false;
	DINT index = GetHash(key);
	
	RBMMAPH::pRoot = RBMALLOCH::ptr[index];
	return RBMMAPH::Lookup(key, b);
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline MAP_VALUE &RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::Value(MAP_KEY key)
{
#ifdef __MASK_HASH__
	if (MMalloc<RBTreeNode<MAP_VALUE, MAP_KEY> *>::Len < iMaxSize && (m_iNum >> iRank) > iMask) Rebuild();
#else
	if (RBMALLOCH::Len < iMaxSize && (RBMMAPH::m_iNum >> iRank) > RBMALLOCH::Len) Rebuild();
#endif
	DINT index = GetHash(key);
	RBMMAPH::pRoot = RBMALLOCH::ptr[index];
	MAP_VALUE &value = RBMMAPH::Add(key, 0);
	RBMALLOCH::ptr[index] = RBMMAPH::pRoot;
	return value;
}

//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline int RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::GetHash(LPCSTR p)
{
	if (!p) return 0xFFFFFFFF;
/*	DWORD hash = 0x9e3779b9;
	while (*p)
	{
		hash += *p++;
		hash = (hash << 4) | (hash >> 28);
	}
*/
	DWORD hash = 2166136261U;
	while (*p)
		hash = 16777619U * hash ^ *p++;
	return GetHash(hash);
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline int RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::GetHash(LPCWSTR p)
{
	if (!p) return 0xFFFFFFFF;
	DWORD hash = 2166136261U;
	while (*p)
		hash = 16777619U * hash ^ *p++;

	return GetHash(hash);
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline int RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::GetHash(BYTE *p, int len)
{
	if (!p) return 0xFFFFFFFF;
	DWORD hash = 0x9e3779b9;
	while (len--)
		hash = 16777619U * hash ^ *p++;
	return GetHash(hash);
}
//-------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline int RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::_GetHash(DWORD ID) const
{
#ifdef __MASK_HASH__
	return (int)(ID & iMask);
#else
	return (DWORD)(( ID % (DWORD)MMalloc<RBTreeNode<MAP_VALUE, MAP_KEY> *>::Len + ID / (DWORD)MMalloc<RBTreeNode<MAP_VALUE, MAP_KEY> *>::Len ) % (DWORD)MMalloc<RBTreeNode<MAP_VALUE, MAP_KEY> *>::Len );
#endif
}
template <class MAP_VALUE, class MAP_KEY, class CMP, class LIST_ALLOC>
inline int RBHashMap<MAP_VALUE, MAP_KEY, CMP, LIST_ALLOC>::GetHash(void *p) const
{
	return GetHash((size_t)p);
}
#define	MHashListMap(MAP_VALUE, MAP_KEY) MHashMap<MAP_VALUE, MAP_KEY,  MMalloc<HMapTreeNode<MAP_VALUE, MAP_KEY> *>, ListNotNew<HMapTreeNode<MAP_VALUE, MAP_KEY> > >

#endif
