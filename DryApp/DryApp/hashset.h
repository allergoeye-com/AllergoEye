///////////////////////////////////////////////////
// hashset.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////
#ifndef __RBHashSet__
#define	 __RBHashSet__
#include "mtree.h"

template <class MAP_KEY>
struct HKeyNode {
	MAP_KEY key;
	HKeyNode *left;
	HKeyNode *right;
	HKeyNode () { left = right = 0; };
	~HKeyNode () {};
	HKeyNode (HKeyNode &r) { operator = (r); };
	HKeyNode &operator = (HKeyNode &r) { key = r.key; left = r.left; right = r.right; return *this; };
	void Init(HKeyNode *_left, HKeyNode *_right)
	{
		left = _left;
		right = _right;
	}
	void Init(HKeyNode *_left, HKeyNode *_right, MAP_KEY &_key)
	{
		left = _left;
		right = _right;
		key	= _key;
	}

};
template <class MAP_KEY,  class ARRAY = MMalloc<HKeyNode<MAP_KEY> *>, class LIST_ALLOC = AllocatorSimple<HKeyNode<MAP_KEY> > >
class MHashSet : protected ARRAY,  protected LIST_ALLOC {
public:
	MHashSet(int _iRank = 2, int max_alloc_block = 1024, int min_alloc_block = 4) {  
		LIST_ALLOC::m_mAlloc._Max_Alloc_ = max_alloc_block;
		LIST_ALLOC::m_mAlloc._MinAlloc =   min_alloc_block ;
				iStartSize = 128;
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
				memset(ARRAY::ptr, 0, ARRAY::Len * sizeof(HKeyNode<MAP_KEY> *)); 
			}
	virtual ~MHashSet() { Clear(); }
	MHashSet(MHashSet &m) { 
		LIST_ALLOC::m_mAlloc._Max_Alloc_ = m.LIST_ALLOC::m_mAlloc._Max_Alloc_;
		LIST_ALLOC::m_mAlloc._MinAlloc =   m.LIST_ALLOC::m_mAlloc._MinAlloc;
				iStartSize = m.iStartSize;
				iRank = m.iRank;
				iMaxSize =m.iMaxSize;
				ARRAY::ReAlloc(iStartSize);
#ifdef __MASK_HASH__
				iMask = 0x7f;
#endif
				m_fNewDelele =  m.m_fNewDelele;
				stack.SetLen(16);
				tab.SetLen(16);
				memset(ARRAY::ptr, 0, ARRAY::Len * sizeof(HKeyNode<MAP_KEY> *));
				*this = m; }
	MHashSet &operator = (MHashSet &m);  
	void CreateNew(bool fNewDelele) { m_fNewDelele = fNewDelele; } 
	void Add(MAP_KEY key);
	bool operator[] (MAP_KEY key);
	void Build(MArray<MAP_KEY> &c) { Clear(); Build(c.GetPtr(), c.GetLen()); }
	void Build(MAP_KEY *p, int l) { Clear(); while(l--) Add(*p++); }
	bool Find(MAP_KEY key);
	void Delete(MAP_KEY key);
	HKeyNode<MAP_KEY> *Get(MAP_KEY key);

	HKeyNode<MAP_KEY> **GetRoot() { return  ARRAY::GetPtr(); }
	DINT GetLenRoot() { return  ARRAY::GetLen(); } 
	HKeyNode<MAP_KEY> *Nil() { return 0; } 
	void Clear();
	DINT Len() const { return LIST_ALLOC::m_iNum; }
	virtual MListEntry<HKeyNode<MAP_KEY> > *Last() { return LIST_ALLOC::Last(); }
	virtual MListEntry<HKeyNode<MAP_KEY> > *First()  { return LIST_ALLOC::First(); }
	int &MaxSizeTable() { return iMaxSize; }
protected:
	virtual void FreeNode(HKeyNode<MAP_KEY> *node);
	virtual bool AllocNode(HKeyNode<MAP_KEY> **node);
	bool SetRoot(HKeyNode<MAP_KEY> **root, MAP_KEY &key, HKeyNode<MAP_KEY> **node);
	bool AllocNode(MAP_KEY &key, HKeyNode<MAP_KEY> **node);
	bool Insert(HKeyNode<MAP_KEY> **root, MAP_KEY &key, HKeyNode<MAP_KEY> **node);
	void GetData(HKeyNode<MAP_KEY> *root, MArray<HKeyNode<MAP_KEY> *> &stack, MArray<HKeyNode<MAP_KEY> *> &res);
	void Delete(HKeyNode<MAP_KEY> **root, MAP_KEY &key);
	void Rebuild();
	int GetHash(LPCSTR p) const;
	int GetHash(LPCWSTR p) const;
	int GetHash(void *p) const;
	int GetHash(POINT &p) { return GetHash(p.x + p.y); }
	int GetHash(GraphEdge &p) { return GetHash(p.pv + p.pw); }
	int _GetHash(DWORD p) const;
	int GetHash(size_t p) const { return _GetHash((DWORD)p); }
	int GetHash(BYTE *p, int len) const;
protected:
	DINT iStartSize, iRank, iMaxSize;
#ifdef __MASK_HASH__
	int iMask;
#endif
	MArray<HKeyNode<MAP_KEY> *> tab, stack;

	bool m_fNewDelele;
	HKeyNode<MAP_KEY> nil; 

protected:
class iteratorIT 
{ 
	HKeyNode<MAP_KEY> *t, *node;
	MArray<HKeyNode<MAP_KEY> *> stack;
	HKeyNode<MAP_KEY> **test, **pStack;
	bool fCheck;
	HKeyNode<MAP_KEY> *root;
	DINT len;
public:
	iteratorIT() { t = 0; }
	MAP_KEY *begin(HKeyNode<MAP_KEY> *_root)
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

		return (MAP_KEY *)t; 
	}
	MAP_KEY *next()
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

		return (MAP_KEY *)t; 
	}
	bool end()
	{ return !t; }
};


public:


	class iterator;
	friend class iterator;

class iterator {
public:
	iterator(MHashSet &m): M(m){ hi = M.ARRAY::GetLen(); 
		len = M.ARRAY::GetLen(); }
	~iterator() {  }
	MAP_KEY *begin() 
	{
		hi = 0;
		while(hi < len && !M.ptr[hi]) ++hi;
		if (hi == len)
			return 0;
		return it.begin(M.ptr[hi]);
	};
	MAP_KEY *next() 
	{
		
		if (hi == len)
			return 0;
		MAP_KEY *t = it.next();
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
	MHashSet &M;

};


};

template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline void MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::FreeNode(HKeyNode<MAP_KEY> *node)
{
	if (m_fNewDelele) node->~HKeyNode<MAP_KEY>();
	LIST_ALLOC::Delete(node);

}

template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline bool MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::AllocNode(HKeyNode<MAP_KEY> **node)
{
	*node = LIST_ALLOC::Add();
	if (*node)
	{
		if (m_fNewDelele) *node = new (*node) HKeyNode<MAP_KEY>();
		(*node)->Init(0, 0);
	}
	return *node != 0;
}
template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline bool MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::SetRoot(HKeyNode<MAP_KEY> **root, MAP_KEY &key, HKeyNode<MAP_KEY> **node)
{
	if (*node)
		*root = *node;
	else
	{
		if (!AllocNode(root)) return 0;
		(*root)->key = key;
		*node = *root;
	}
	return true;
}
template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline bool MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::AllocNode(MAP_KEY &key, HKeyNode<MAP_KEY> **node)
{
	if (!*node)
	{
		if (!AllocNode(node)) return 0;
		(*node)->key = key;
	}
	return true;
}
//-------------------------------------
//
//-------------------------------------
template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
void MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::Delete(HKeyNode<MAP_KEY> **root, MAP_KEY &key)
{
	HKeyNode<MAP_KEY> *parentY = 0, *node = *root;
	HKeyNode<MAP_KEY> *tmpX, *tmpY;
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
		node->key = tmpY->key;	
	FreeNode(tmpY);

}

//----------------------------------------------
//
//----------------------------------------------
template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
bool MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::Insert(HKeyNode<MAP_KEY> **root, MAP_KEY &key, HKeyNode<MAP_KEY> **node)
{
	if (!*root) return SetRoot(root, key, node);
	HKeyNode<MAP_KEY> *nodeParent = 0, *temp = *root;
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
	if (AllocNode(key, node))
	{
		if (n)
			nodeParent->left = *node;
		else
			nodeParent->right = *node;
		return true;
	}
	return false;
}


template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
void MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::GetData(HKeyNode<MAP_KEY> *root, MArray<HKeyNode<MAP_KEY> *> &stack, MArray<HKeyNode<MAP_KEY> *> &res) 
{
		bool fCheck = true;
		HKeyNode<MAP_KEY> **test, **pStack = stack.GetPtr();
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
template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
void MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::Rebuild() 
{
	HKeyNode<MAP_KEY> **ptab, **ptmp = ARRAY::ptr, **tmp = ARRAY::ptr;
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
	memset(ARRAY::ptr, 0, ARRAY::Len * sizeof(HKeyNode<MAP_KEY> *)); 
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
				Insert(ARRAY::ptr + GetHash((*ptab)->key), (*ptab)->key, ptab);
				++ptab;
			}
		}
		++ptmp;
	}
	LIST_ALLOC::m_iNum = n;
	ARRAY::Free(tmp);

	
}
//-------------------------------------------------------
template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline MHashSet<MAP_KEY, ARRAY, LIST_ALLOC> &MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::operator = (MHashSet<MAP_KEY, ARRAY, LIST_ALLOC> &m)
{
	Clear();
	iMaxSize = m.iMaxSize;
	ARRAY::ReAlloc(m.ARRAY::Len);
#ifdef __MASK_HASH__
	iMask = m.iMask;
#endif
	iRank = m.iRank;
	memset(ARRAY::ptr, 0, ARRAY::Len * sizeof(HKeyNode<MAP_KEY> *)); 
	iterator it(m);
	for (MAP_KEY *t = it.begin(); t; t = it.next())
		Add(*t);
	return *this;
}
//-------------------------------------------------------
template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline void MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::Clear()
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
	memset(ARRAY::ptr, 0, ARRAY::Len * sizeof(HKeyNode<MAP_KEY> *));  
}
//-------------------------------------------------------
template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline void MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::Add(MAP_KEY key)
{
#ifdef __MASK_HASH__
	if (ARRAY::Len < iMaxSize && (LIST_ALLOC::m_iNum >> iRank) > iMask) Rebuild();
#else
	if (ARRAY::Len < iMaxSize && (LIST_ALLOC::m_iNum >> iRank) > ARRAY::Len) Rebuild();
#endif
	HKeyNode<MAP_KEY> *node = 0;
	Insert(ARRAY::ptr + GetHash(key), key, &node);
}
//-------------------------------------------------------
template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline void MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::Delete(MAP_KEY key)
{
	Delete(ARRAY::ptr + GetHash(key), key);
}
//-------------------------------------------------------
template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline bool MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::Find(MAP_KEY key)
{
	HKeyNode<MAP_KEY> *node = ARRAY::ptr[GetHash(key)];
		while (node && node->key != key)
			node = node->key > key ? node->left : node->right;
	return node ? true : false; 
}
//-------------------------------------------------------
template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline HKeyNode<MAP_KEY> *MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::Get(MAP_KEY key)
{
	HKeyNode<MAP_KEY> *node = ARRAY::ptr[GetHash(key)];
		while (node && node->key != key)
			node = node->key > key ? node->left : node->right;
	return node; 
}

//-------------------------------------------------------
template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline int MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::GetHash(LPCSTR p) const
{
	if (!p) return 0xFFFFFFFF;
	DWORD hash = 2166136261U;
	while (*p)
		hash = 16777619U * hash ^ *p++;
	return GetHash(hash);
}
//-------------------------------------------------------
template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline int MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::GetHash(LPCWSTR p) const
{
	if (!p) return 0xFFFFFFFF;
	DWORD hash = 2166136261U;
	while (*p)
		hash = 16777619U * hash ^ *p++;
	return GetHash(hash);
}
//-------------------------------------------------------
template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline int MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::GetHash(BYTE *p, int len) const
{
	if (!p) return 0xFFFFFFFF;
//	DWORD hash = 0x9e3779b9;
	DWORD hash = 2166136261U;
	while (len--)
		hash = 16777619U * hash ^ *p++;
	return GetHash(hash);
}
//-------------------------------------------------------
template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline int MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::_GetHash(DWORD ID) const
{

#ifdef __MASK_HASH__
	return (int)(ID & iMask);
#else
	return (DWORD)(( ID % (DWORD)ARRAY::Len + ID / (DWORD)ARRAY::Len ) % (DWORD)ARRAY::Len );
#endif
}

template <class MAP_KEY, class ARRAY, class LIST_ALLOC>
inline int MHashSet<MAP_KEY, ARRAY, LIST_ALLOC>::GetHash(void *ID) const
{
	return GetHash((size_t)ID);
}

template <class MAP_KEY, class CMP = RBCmp<MAP_KEY>, class LIST_ALLOC = AllocatorSimple<RBKeyNode<MAP_KEY> > >
class RBHashSet : protected MArray<RBKeyNode<MAP_KEY> *>, protected RBSet<MAP_KEY, CMP, LIST_ALLOC> {
public:
	typedef typename RBSet<MAP_KEY, CMP, LIST_ALLOC>::iterator RBiterator;
	typedef typename RBSet<MAP_KEY, CMP, LIST_ALLOC>::iteratorNode RBiteratorNode;
	typedef MArray<RBKeyNode<MAP_KEY> *> RBMALLOCH;
	typedef RBSet<MAP_KEY, CMP, LIST_ALLOC> RBMMAPH;
	RBHashSet(int size = 100, int rank = 2) {iRank = rank; iStartSize = size, mDefSize = size; RBMALLOCH::SetLen(size);
				nRank = RBMALLOCH::Len << iRank; RBMALLOCH::Set(RBMMAPH::Nil()); }
	virtual ~RBHashSet() { Clear(); }
	RBHashSet(const RBHashSet &m) { mDefSize = m.mDefSize; RBMALLOCH::SetLen(mDefSize); Set(RBMMAPH::Nil());
		nRank = RBMALLOCH::Len << iRank;  *this = m; }
	RBHashSet &operator = (RBHashSet &m);  
	void CreateNew(bool fNewDelele) { RBMMAPH::m_fNewDelele = fNewDelele; }
	void KeyFromCompareFunc(bool b) { RBMMAPH::m_fKeyFromCompareFunc = b;  }
	bool Add(MAP_KEY key);
	bool operator[] (MAP_KEY key);
	void Build(MArray<MAP_KEY> &c) { Build(c.GetPtr(), c.GetLen()); }
	void Build(MAP_KEY *p, DINT l) { Clear(); while(l--) Add(*p++); }
	bool Find(MAP_KEY key);
	void Delete(MAP_KEY key);
	RBKeyNode<MAP_KEY> *Get(MAP_KEY &key) { 
		RBMMAPH::pRoot = RBMALLOCH::ptr[GetHash(key)];
		return Search(RBMMAPH::pRoot, key);
	}
	RBKeyNode<MAP_KEY> *Nil() { return RBMMAPH::Nil(); }
	virtual ListNotNewEntry<RBKeyNode<MAP_KEY> > *Last() { return LIST_ALLOC::Last(); }
	virtual ListNotNewEntry<RBKeyNode<MAP_KEY> > *First() { return LIST_ALLOC::First(); }

	void Clear();
	DINT Len() const { return LIST_ALLOC::m_iNum; }
protected:
	void Rebuild();
	DWORD GetHash(LPCSTR p)  const;
	DWORD GetHash(LPCWSTR p) const;
	DWORD GetHash(void *p) const;
	DWORD _GetHash(DWORD p) const;
	int GetHash(DINT p) const { return _GetHash((DWORD)p); }
	DWORD GetHash(BYTE *p, int len);
	DINT iStartSize, iRank, nRank;
	int mDefSize;
public:
	class iterator;
	friend class iterator;
	class iteratorNode;
	friend class iteratorNode;

class iterator {
public:
	iterator(RBHashSet &m): M(m){ hi = M.MArray<RBKeyNode<MAP_KEY> *>::GetLen();
	it = new RBiterator(m);
	len = M.MArray<RBKeyNode<MAP_KEY> *>::GetLen(); }
	~iterator() { delete it; }
	MAP_KEY *begin() 
	{
		hi = 0;
		while(hi < len && M.ptr[hi] == &M.m_Nil) ++hi;
		if (hi == len)
			return 0;
		M.pRoot = M.ptr[hi];
		return it->begin();
	};
	MAP_KEY *next() 
	{
		if (hi == len)
			return 0;
		MAP_KEY *t = it->next();
		if (!t)
		{
			++hi;
			while(hi < len && M.ptr[hi] == &M.m_Nil) ++hi;
			if (hi == len)
				return 0;
			M.pRoot = M.ptr[hi];
			t = it->begin();
		}
		if (it->end())
		{
			while(hi < len && M.ptr[hi] == &M.m_Nil) ++hi;
			if (hi != len)
				M.pRoot = M.ptr[hi];
		}
		return t;
	};
	bool end() { return hi == len; }
	DINT hi, len;
	RBiterator *it;
	RBHashSet &M;

};
class iteratorNode {
public:
	iteratorNode(RBHashSet &m): M(m){ it = new RBiteratorNode(m);
	hi = len = M.MArray<RBKeyNode<MAP_KEY> *>::GetLen(); }
	~iteratorNode() { delete it; }
	RBKeyNode<MAP_KEY> *begin() 
	{
		hi = 0;
		while(hi < len && M.ptr[hi] == &M.m_Nil) ++hi;
		if (hi == len)
			return 0;
		M.pRoot = M.ptr[hi];
		return it->begin();
	};
	RBKeyNode<MAP_KEY> *next() 
	{
		if (hi == len)
			return 0;
		RBKeyNode<MAP_KEY> *t = it->next();
		
		if (!t)
		{
			++hi;
			while(hi < len && M.ptr[hi] == &M.m_Nil) ++hi;
			if (hi == len)
				return 0;
			M.pRoot = M.ptr[hi];
			t = it->begin();
		}
		if (it->end())
		{
			while(hi < len && M.ptr[hi] == &M.m_Nil) 
				++hi;
			if (hi != len)
				M.pRoot = M.ptr[hi];
		}
		return t;
	};
	bool end() { return hi == len; }
	DINT hi, len;
	RBiteratorNode *it;
	RBHashSet &M;

};

};


//-------------------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
void RBHashSet<MAP_KEY, CMP, LIST_ALLOC>::Rebuild() 
{
	MArray<RBKeyNode<MAP_KEY> *> tmp(RBMMAPH::m_iNum);
	RBKeyNode<MAP_KEY> **ptab = tmp.GetPtr();
	if (!ptab) return;
	iteratorNode it(*this);
	for (*ptab = it.begin(); *ptab; *ptab = it.next()) ptab++; 
	DINT index, j = RBMMAPH::m_iNum;
	 RBMALLOCH::SetLen(RBMMAPH::m_iNum << 1);
	nRank = RBMMAPH::m_iNum << (iRank + 1);
	MArray<RBKeyNode<MAP_KEY> *>::Set(RBMMAPH::Nil());
	ptab = tmp.GetPtr();
	DINT n = RBMMAPH::m_iNum;
	while(j--)
	{
		if (*ptab)
		{
			(*ptab)->Init(RBMMAPH::Nil(), RBMMAPH::Nil(), RBMMAPH::Nil(), _BLACK);
			index = GetHash((*ptab)->key);
			RBMMAPH::pRoot = RBMALLOCH::ptr[index];
			RBMMAPH::Add((*ptab)->key, ptab);
			RBMALLOCH::ptr[index] = RBMMAPH::pRoot;
		}
		++ptab;
	}
	RBMMAPH::m_iNum = n;
}
//-------------------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
RBHashSet<MAP_KEY, CMP, LIST_ALLOC> &RBHashSet<MAP_KEY, CMP, LIST_ALLOC>::operator = (RBHashSet<MAP_KEY, CMP, LIST_ALLOC> &m)
{
	Clear();
	SetLen(m.MArray<RBKeyNode<MAP_KEY> *>::Len);
	nRank = MArray<RBKeyNode<MAP_KEY> *>::Len << iRank;

	Set(RBMMAPH::Nil());
	MAP_KEY *t;
	iterator it(m);
	for (t = it.begin(); t; t = it.next())
		Add(*t);
	return *this;
}
//-------------------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
void RBHashSet<MAP_KEY, CMP, LIST_ALLOC>::Clear()
{
	for (DINT i = 0; i < RBMALLOCH::Len; ++i)
	{
		if (RBMALLOCH::ptr[i] != RBMMAPH::Nil())
		{
			RBMMAPH::pRoot = RBMALLOCH::ptr[i];
			RBSet<MAP_KEY, CMP, LIST_ALLOC>::Clear();
			RBMALLOCH::ptr[i] = RBMMAPH::Nil();
		}
	}
	RBMMAPH::m_iNum = 0;
	RBMALLOCH::SetLen(mDefSize);
	nRank = RBMALLOCH::Len << iRank;

	RBMALLOCH::Set(RBMMAPH::Nil());
	
}
//-------------------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
void RBHashSet<MAP_KEY, CMP, LIST_ALLOC>::Delete(MAP_KEY key)
{
	DWORD index = GetHash(key);
	RBMMAPH::pRoot = RBMALLOCH::ptr[index];
	RBSet<MAP_KEY, CMP, LIST_ALLOC>::Delete(key);
	RBMALLOCH::ptr[index] = RBMMAPH::pRoot;
}
//-------------------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline bool RBHashSet<MAP_KEY, CMP, LIST_ALLOC>::Find(MAP_KEY key)
{
	if (!RBMMAPH::m_iNum) return false;
	RBMMAPH::pRoot = RBMALLOCH::ptr[GetHash(key)];
	return RBMMAPH::Find(key);
}
//-------------------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
bool RBHashSet<MAP_KEY, CMP, LIST_ALLOC>::Add(MAP_KEY key)
{
	if (RBMMAPH::m_iNum > nRank)	Rebuild();
	DWORD index = GetHash(key);
	RBMMAPH::pRoot = RBMALLOCH::ptr[index];
	bool fRet = RBMMAPH::Add(key, 0);
	 RBMALLOCH::ptr[index] = RBMMAPH::pRoot;
	return fRet;
}
//-------------------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline DWORD RBHashSet<MAP_KEY, CMP, LIST_ALLOC>::GetHash(LPCSTR p) const
{
	if (!p) return 0xFFFFFFFF;
	DWORD hash = 2166136261U;
	while (*p)
		hash = 16777619U * hash ^ *p++;
	return GetHash(hash);
}
//-------------------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline DWORD RBHashSet<MAP_KEY, CMP, LIST_ALLOC>::GetHash(LPCWSTR p) const
{
	if (!p) return 0xFFFFFFFF;
	DWORD hash = 2166136261U;
	while (*p)
		hash = 16777619U * hash ^ *p++;

	return GetHash(hash);
}
//-------------------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline DWORD RBHashSet<MAP_KEY, CMP, LIST_ALLOC>::GetHash(BYTE *p, int len)
{
	if (!p) return 0xFFFFFFFF;
	DWORD hash = 0x9e3779b9;
	while (len--)
		hash = 16777619U * hash ^ *p++;
	return GetHash(hash);
}
//-------------------------------------------------------
template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline DWORD RBHashSet<MAP_KEY, CMP, LIST_ALLOC>::_GetHash(DWORD ID) const
{
	return (DWORD)(( ID % (DWORD)MArray<RBKeyNode<MAP_KEY> *>::Len + ID / (DWORD)MArray<RBKeyNode<MAP_KEY> *>::Len ) % (DWORD)MArray<RBKeyNode<MAP_KEY> *>::Len );
}
template <class MAP_KEY, class CMP, class LIST_ALLOC>
inline DWORD RBHashSet<MAP_KEY, CMP, LIST_ALLOC>::GetHash(void *p)  const
{
	return GetHash((size_t)p);
}
//***********************************************************
//
//***********************************************************
template <class T >
void SetUnique(MArray<T> &arr)
{
	MHashSet<T> tmp;
	for (int i = 0; i < arr.GetLen(); ++i)
	{
		if (!tmp.Find(arr[i]))
			tmp.Add(arr[i]);
		else
		{
			arr.Remove(i, 1);
			--i;
		}
	}
}
template <class T >
void SetUnique(MVector<T> &arr)
{
	MHashSet<T> tmp;
	for (int i = 0; i < arr.GetLen(); ++i)
	{
		if (!tmp.Find(arr[i]))
			tmp.Add(arr[i]);
		else
		{
			arr.Remove(i, 1);
			--i;
		}
	}
}

#endif
