///////////////////////////////////////////////////
// hashmap.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2010
////////////////////////////////////////////////////

#ifndef __MARHashMap__
#define	 __MARHashMap__

#include "rbmap.h"
#include "ArHeap.h"
#ifndef __PLUK_LINUX__
#pragma warning (disable : 4786)
#endif


inline int __cdecl _comp_DINT(const void *elem1, const void *elem2 )
{
	if (*(DINT *)elem1 > *(DINT *)elem2)
		return 1;
	if (*(DINT *)elem1 == *(DINT *)elem2)
		return 0;
	return -1;
} 


struct ArHashTreeNode {
	DINT left;
	DINT right;
	bool color;
	ArHashTreeNode () { left = right = -1; };
	~ArHashTreeNode () {};
	void Init(DINT _left, DINT _right)
	{
		left = _left;
		right = _right;
	}
};

template <class MAP_VALUE, class MAP_KEY,  class ARRAY = MMalloc<DINT> >
class ArHashMap : protected ARRAY {
public:
	ArHashMap(int _iRank = 2, int max_alloc_block = 1024, int min_alloc_block = 4, int _iStartSize = 128) {  
			iStartSize = _iStartSize;
			iRank = _iRank;
			iMaxSize = 0x100000;
			ARRAY::ReAlloc(iStartSize);
#ifdef __MASK_HASH__
				iMask = 0x7f; 
#endif
				m_iNum = 0;
				m_fNewDelele = true;
				stack.SetLen(16);
				tab.SetLen(16);	
				ARRAY::Set(-1);
				Data.Add();
				Tree.Add();
				bData = Data.GetPtr() + 1;
				bTree = Tree.GetPtr() + 1;
				bTree[-1].Init(-1, -1);
		
			}
	virtual ~ArHashMap();
	ArHashMap(ArHashMap &m) { 
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
	ARRAY::Set(-1);
	Data.Add();
	Tree.Add();
	bData = Data.GetPtr() + 1;
	bTree = Tree.GetPtr() + 1;
	bTree[-1].Init(-1, -1);
	*this = m;
	}
	ArHashMap &operator = (ArHashMap &m);  
	void CreateNew(bool fNewDelele) { m_fNewDelele = fNewDelele; } 
	MAP_VALUE &Add(MAP_KEY &key, MAP_VALUE &value);
	MAP_VALUE &Value(MAP_KEY key);
	MAP_VALUE &operator[] (MAP_KEY key) { return Value(key); }
	void Build(MArray<RBData<MAP_VALUE, MAP_KEY> > &ar);
	void GetData(MArray<RBData<MAP_VALUE, MAP_KEY> > &res);
	bool operator == (ArHashMap &m);
	DINT *GetRoot() { return  ARRAY::ptr; }
	DINT GetLenRoot() { return  ARRAY::Len; } 
	
	bool Lookup(MAP_KEY key, MAP_VALUE &b);
	bool Lookup(MAP_KEY key, MAP_VALUE **b = 0);
	void Delete(MAP_KEY key);
	void Clear();
	void SetAddedRealSize(int a) { Data.SetAddedRealSize(a); Tree.SetAddedRealSize(a);}
	void PreAlloc(DINT len) { Data.SetLen(len + 1); Data.SetLen(1); Tree.SetLen(len + 1); Tree.SetLen(1);
								bData = Data.GetPtr() + 1;
								bTree = Tree.GetPtr() + 1;
								DINT l = len/16;
								ARRAY::ReAlloc(l <  512 ? 512 : l);
								ARRAY::Set(-1);
								Tree[0].Init(-1, -1);

							}
	void Reset();

	DINT Len() const { return m_iNum; }
	int &MaxSizeTable() { return iMaxSize; }
	void Compact();
	void Swap(ArHashMap &h)
	{
		ARRAY::Swap(h);
		Tree.Swap(h.Tree);
		Data.Swap(h.Data);
		bDeleted.Swap(h.bDeleted);
		bData = Data.GetPtr() + 1;
		bTree = Tree.GetPtr() + 1;
		h.bData = h.Data.GetPtr() + 1;
		h.bTree = h.Tree.GetPtr() + 1;
		DINT n = m_iNum; 
		m_iNum = h.m_iNum;
		h.m_iNum = n;
		bool b = m_fNewDelele;
		m_fNewDelele = h.m_fNewDelele;
		h.m_fNewDelele = b;
	}
#ifndef NOPLUK
	BOOL Serialize(Variable &var, bool fClearMap = true);
	BOOL UnSerialize(Variable &var);
#endif
protected:
	bool AllocNode(DINT *node);
	void FreeNode(DINT  node);
	bool SetRoot(DINT *root, MAP_KEY &key, MAP_VALUE *value, DINT *node);
	bool AllocNode(MAP_KEY &key, MAP_VALUE *value, DINT *node);
	bool Insert(DINT *root, MAP_KEY &key, MAP_VALUE *value, DINT *node);
	void GetData(DINT root, MArray<DINT> &stack, MArray<DINT> &res);
	void Delete(DINT *root, MAP_KEY &key);
	void Rebuild();
	int GetHash(LPCSTR p) const;
	int GetHash(LPCWSTR p) const;
	int GetHash(void *p) const;
	int GetHash(POINT &p) { return GetHash(p.x + p.y); }
	int GetHash(GraphEdge &p) { return GetHash(p.pv + p.pw); }
	int _GetHash(DWORD p) const;
	int GetHash(size_t p) const { return _GetHash((DWORD)p); }
	int GetHash(BYTE *p, int len) const;
	int GetHash(float &p) const;
	bool Insert(DINT *root, DINT node);
public:
	RBData<MAP_VALUE, MAP_KEY> *VData(int &len) { len = Data.GetLen() - 1; return bData; }
	ArHashTreeNode *VTree(int &len) { len = Tree.GetLen() - 1; return bTree; }
protected:
	DINT iStartSize, iRank, iMaxSize;
#ifdef __MASK_HASH__
	int iMask;
#endif

	MArray<DINT> tab, stack;
	bool m_fNewDelele;
public:
/*******************from serialize class SwapFile only *************************************************/
	BOOL MoveToRoot(DINT *buffer, DINT len) { ARRAY::ptr = buffer; ARRAY::Len = len;  return true; }
	MArray<RBData<MAP_VALUE, MAP_KEY> > Data;
	MArray<ArHashTreeNode > Tree;
	RBData<MAP_VALUE, MAP_KEY> *bData;
	ArHashTreeNode *bTree;
	DINT m_iNum;
//*************************************************************************/
	ArHeap bDeleted;
protected:
friend class iteratorIT;

class iteratorIT 
{ 
	DINT t, node;
	MArray<DINT> stack;
	DINT *test, *pStack;
	bool fCheck;
	DINT root;
	int len;
	ArHashMap &M;
public:
	iteratorIT(ArHashMap &m):M(m) { t = 0; root = -1; node = 0; pStack = 0; fCheck = false; len = 0; test = 0; }
	RBData<MAP_VALUE, MAP_KEY> *begin(DINT _root)
	{ 
		root = _root;
		if (root == -1) return 0;	
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
			node = M.bTree[node].left;
		}
		while(node != -1);
		node = *(--pStack);
		t = node;
		if((fCheck = M.bTree[node].right  != -1))
			node = M.bTree[node].right;

		return M.bData + t; 
	}
	RBData<MAP_VALUE, MAP_KEY> *next()
	{ 
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
					node = M.bTree[node].left;
				}
				while(node != -1);
			}
			node = *(--pStack);
			t = node;
			if((fCheck = M.bTree[node].right != -1))
				node = M.bTree[node].right;
		}
		else
			t = -1;

		return t == -1 ? NULL : M.bData + t;
	}
	bool end()
	{ return t == -1; }
};


public:

	class iterator;
	friend class iterator;

class iterator {
public:
	iterator(ArHashMap &m): M(m), it(m) { hi = M.ARRAY::GetLen(); 
		len = M.ARRAY::GetLen(); }
	~iterator() {  }
	RBData<MAP_VALUE, MAP_KEY> *begin() 
	{
		hi = 0;
		while(hi < len && M.ptr[hi] == -1) ++hi;
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
			while(hi < len && M.ptr[hi] == -1) ++hi;
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
	ArHashMap &M;

};


};
#ifndef NOPLUK
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
BOOL ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::Serialize(Variable &Res, bool fClearMap/* = true*/)
{
	Compact();
	Res.Init(VAR_VECTOR, 7);
	Variable *var = Res.GetArray();
	var[0] = m_iNum;
	var[1] = ARRAY::Len;
	var[2].Init(VAR_STRING, ARRAY::Len * sizeof(DINT));
	String *str = var[2].GetString();
	if (!str) return 0;
	memcpy(str->buffer, ARRAY::ptr, str->len);
	if (fClearMap)
	{
		ARRAY::Free();
		ARRAY::ReAlloc(iStartSize);
		#ifdef __MASK_HASH__
			iMask = iStartSize - 1; 
		#endif
		ARRAY::Set(-1);
	}
	
	
	var[3] = Data.GetLen();
	var[4].Init(VAR_STRING, Data.GetLen() * sizeof(RBData<MAP_VALUE, MAP_KEY>));
	str = var[4].GetString();
	if (!str) return 0;
	memcpy(str->buffer, Data.GetPtr(), str->len);
	if (fClearMap)
	{
		Data.Clear();
		Data.Add();
		bData = Data.GetPtr() + 1;
	}
	var[5] = Tree.GetLen();
	var[6].Init(VAR_STRING, Tree.GetLen() * sizeof(ArHashTreeNode));
	str = var[6].GetString();
	if (!str) return 0;
	memcpy(str->buffer, Tree.GetPtr(), str->len);
	if (fClearMap)
	{
		Tree.Clear();
		Tree.Add();
		bTree = Tree.GetPtr() + 1;
		bTree[-1].Init(-1, -1);
		m_iNum = 0;
	}
	return TRUE;	
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
BOOL ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::UnSerialize(Variable &Res)
{
	Clear();
	if (Res.GetDim() < 7) return 0;
	Variable *var = Res.GetArray();
	ARRAY::ReAlloc((DINT)var[1]);
	String *str = var[2].GetString();
	if (!str)
	{
		Clear();
		return 0;
	}
	memcpy(ARRAY::ptr, str->buffer, str->len);
	
	Data.SetLen((DINT)var[3]);
	str = var[4].GetString();
	if (!str)
	{
		Clear();
		return 0;
	}
	memcpy(Data.GetPtr(), str->buffer, str->len);

	Tree.SetLen((DINT)var[5]);
	str = var[6].GetString();
	if (!str)
	{
		Clear();
		return 0;
	}
	memcpy(Tree.GetPtr(), str->buffer, str->len);
	m_iNum = var[0];
	return TRUE;	
}
#endif
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::~ArHashMap() 
{ 	
	Clear(); 
	ARRAY::Free();
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline void ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::FreeNode(DINT node)
{
	RBData<MAP_VALUE, MAP_KEY> *d = bData + node;
	if (m_fNewDelele) 
	{
		d->~RBData<MAP_VALUE, MAP_KEY>();
	}		
	//memset(d, 0xff, sizeof(RBData<MAP_VALUE, MAP_KEY>));
	bDeleted.Push(node);
	--m_iNum;

}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline bool ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::AllocNode(DINT *node)
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
	bTree[*node].Init(-1, -1);
	++m_iNum;
	return true;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline bool ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::SetRoot(DINT *root, MAP_KEY &key, MAP_VALUE *value, DINT *node)
{
	if (*node != -1)
		*root = *node;
	else
	{
		if (!AllocNode(root)) return 0;
		bData[*root].key = key;
		if (value)
			bData[*root].value = *value;
		*node = *root;

	}
	return true;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline bool ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::AllocNode(MAP_KEY &key, MAP_VALUE *value, DINT *node)
{
	if (*node == -1)
	{
		if (!AllocNode(node)) return 0;
		bData[*node].key = key;
		if (value)
			bData[*node].value = *value;
	}
	return true;
}
//-------------------------------------
//
//-------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
void ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::Delete(DINT *root, MAP_KEY &key)
{
	DINT parentY = 0, node = *root;
	DINT tmpX, tmpY;
	while (node != -1 && bData[node].key != key)
	{
		parentY = node;
		node = bData[node].key > key ? bTree[node].left : bTree[node].right;
	}
	if (node == -1) return;
	if (bTree[node].left == -1 || bTree[node].right == -1)
		tmpY = 	node;
	else
	{
		parentY = node;
		tmpY = bTree[node].right;
		while (bTree[tmpY].left != -1) 
		{
			parentY = tmpY;
			tmpY = bTree[tmpY].left;
		}
	}
	tmpX = bTree[tmpY].left ? bTree[tmpY].left : bTree[tmpY].right;
	if (parentY == -1)
		*root = tmpX;
	else
	{
		if (bTree[parentY].left == tmpY)
			bTree[parentY].left = tmpX;
		else
			bTree[parentY].right = tmpX;
	}
	if (tmpY != node)
	{
		bData[node].key = bData[tmpY].key;	
		bData[node].value = bData[tmpY].value;	
	}
	FreeNode(tmpY);
}

//----------------------------------------------
//
//----------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
bool ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::Insert(DINT *root, MAP_KEY &key, MAP_VALUE *value, DINT *node)
{
	if (*root == -1) return SetRoot(root, key, value, node);
	DINT nodeParent, temp = *root;
	int n = 0;
	while (temp != -1)
	{
		nodeParent = temp;
		if ((n = (key < bData[temp].key)))
			temp = bTree[temp].left;
		else
		if (key > bData[temp].key)
			temp = bTree[temp].right;
		else 
		{
			*node = temp;
			return false;
		}
	}
	if (AllocNode(key, value, node))
	{
		if (n)
			bTree[nodeParent].left = *node;
		else
			bTree[nodeParent].right = *node;
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
bool ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::Insert(DINT *root, DINT node)
{
	if (*root == -1) 
	{
		*root = node;
		bTree[node].Init(-1, -1);
		return  true;
	}
	DINT nodeParent, temp = *root;
	int n = 0;
	while (temp != -1)
	{
		nodeParent = temp;
		if ((n = (bData[node].key < bData[temp].key)))
			temp = bTree[temp].left;
		else
		if (bData[node].key > bData[temp].key)
			temp = bTree[temp].right;
		else 
			return false;
	}
	if (n)
		bTree[nodeParent].left = node;
	else
		bTree[nodeParent].right = node;
	return true;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline void ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::Build(MArray<RBData<MAP_VALUE, MAP_KEY> > &ar)
{
	Clear();
	DINT len = ar.GetLen();
	Tree.SetLen(len + 1);
	Tree[0].Init(-1, -1);
	Data.SetLen(1);
	Data += ar;
	bData = Data.GetPtr();
	bData = Data.GetPtr() + 1;
	bTree = Tree.GetPtr() + 1;
	DINT l = len/16;
	ARRAY::ReAlloc(l <  512 ? 512 : l);
	ARRAY::Set(-1);
	for (DINT i = 0; i < len; ++i)
	{
		bTree[i].Init(-1, -1);
		Insert(ARRAY::ptr + GetHash(bData[i].key), i);
	}

	/*
	RBData<MAP_VALUE, MAP_KEY> *p = ar.GetPtr();
	int len = ar.GetLen();
	while(len--)
	{
		Value(p->key) = p->value;
		++p;
	}
	*/
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline void ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::GetData(MArray<RBData<MAP_VALUE, MAP_KEY> > &res) 
{
	res.SetLen(m_iNum);
	RBData<MAP_VALUE, MAP_KEY> *ptab = res.GetPtr();
	iterator it(*this);
	for (RBData<MAP_VALUE, MAP_KEY> *t = it.begin(); t; t = it.next())
			*ptab++ = *t;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
void ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::GetData(DINT root, MArray<DINT> &stack, MArray<DINT> &res) 
{
		bool fCheck = true;
		DINT  *test, *pStack = stack.GetPtr();
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
					root = bTree[root].left;
				}
				while(root != -1);
			root = *(--pStack);
			res.Add(root);
			if((fCheck = bTree[root].right != -1))
				root = bTree[root].right;
		}
		while (root != -1 && (fCheck || pStack > test));
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
void ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::Compact()
{
	if (!ARRAY::ptr) return;
	if (!bDeleted.IsEmpty())
	{
		MArray<RBData<MAP_VALUE, MAP_KEY> > tmp_data(Data.GetLen());
		RBData<MAP_VALUE, MAP_KEY> *data = tmp_data.GetPtr() + 1;
		RBData<MAP_VALUE, MAP_KEY> *start = data;
		DINT i = 0, l = Data.GetLen() - 1;
		while(!bDeleted.IsEmpty())
		{
			DINT j = bDeleted.Pop();
			memcpy(data, bData + i, (j - i) * sizeof(RBData<MAP_VALUE, MAP_KEY>) );
			data += (j - i);
			i = j + 1;
		}
		if (l > i)
			memcpy(data, bData + i, (l - i) * sizeof(RBData<MAP_VALUE, MAP_KEY>) );
		tmp_data.SetLen(m_iNum); 
		Data.Swap(tmp_data);
		bData = Data.GetPtr() + 1;
		for (i = 0; i < m_iNum; ++i)
		{
			bTree[i].Init(-1, -1);
			Insert(ARRAY::ptr + GetHash(bData[i].key), i);
		}
	}
	bDeleted.Clear();
	ARRAY::Compact();
	Data.Compact();
	Tree.Compact();
	bData = Data.GetPtr() + 1;
	bTree = Tree.GetPtr() + 1;
	bTree[-1].Init(-1, -1);
	tab.SetLen(16);
	tab.Compact();
	stack.SetLen(16);
	stack.Compact();
	
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
void ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::Rebuild() 
{
	DINT *ptab, *ptmp = ARRAY::ptr, *tmp = ARRAY::ptr;
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
	tab.SetLen(m_iNum);
	ARRAY::Set(-1); 
	DINT n = m_iNum;
	MArray<ArHashTreeNode > cTree(Tree.GetLen());
	
	while (len--)
	{
		if (*ptmp != -1)
		{
			tab.SetLen(0);
			bTree = Tree.GetPtr() + 1;
			GetData(*ptmp, stack, tab);
			ptab = tab.GetPtr();
			j = tab.GetLen();
			bTree = cTree.GetPtr() + 1;
			while(j--)
			{
				bTree[*ptab].Init(-1, -1);
				Insert(ARRAY::ptr + GetHash(bData[*ptab].key), *ptab);
				++ptab;
			}
		}
		++ptmp;
	}
	m_iNum = n;
	Tree.Swap(cTree);
	ARRAY::Free(tmp);
	bTree = Tree.GetPtr() + 1;

	
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline ArHashMap<MAP_VALUE, MAP_KEY, ARRAY> &ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::operator = (ArHashMap<MAP_VALUE, MAP_KEY, ARRAY> &m)
{
	Clear();
	iMaxSize = m.iMaxSize;
	ARRAY::ReAlloc(m.ARRAY::Len);
#ifdef __MASK_HASH__
	iMask = m.iMask;
#endif
	iRank = m.iRank;
	memcpy(ARRAY::ptr, m.ARRAY::ptr, ARRAY::Len * sizeof(DINT)); 
	m_fNewDelele = m.m_fNewDelele;
	Tree = m.Tree;
	Data = m.Data;
	bDeleted = m.bDeleted;
	bData = Data.GetPtr() + 1;
	bTree = Tree.GetPtr() + 1;
	if (m_fNewDelele)
	{
		DINT *Deleted = bDeleted.GetPtr();
		DINT kl = bDeleted.GetLen();
		DINT k, i = 0, l = Data.GetLen() - 1;
		
		for (k = 0; k < kl; ++k)
		{
			DINT j = Deleted[k];
			for (;i < j; ++i)
			{
				RBData<MAP_VALUE, MAP_KEY> *d = bData + i;
				d = new (d) RBData<MAP_VALUE, MAP_KEY>(m.bData[i].key, m.bData[i].value);
			}
			++i;
		}
		for (;i < l; ++i)
		{
			RBData<MAP_VALUE, MAP_KEY> *d = bData + i;
			d = new (d) RBData<MAP_VALUE, MAP_KEY>(m.bData[i].key, m.bData[i].value);
		}
	}
	
	m_iNum =m.m_iNum;
	return *this;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline bool ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::operator == (ArHashMap<MAP_VALUE, MAP_KEY, ARRAY> &m) 
{ 
	if (m_iNum == m.m_iNum)
	{ 
		iterator it(m);
		MAP_VALUE *val;
		for (RBData<MAP_VALUE, MAP_KEY> *t = it.begin(); t; t = it.next())
			if (!Lookup(t->key, &val) || t->value != *val) return false;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline void ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::Clear()
{
	if (m_fNewDelele)
	{
		DINT i = 0, l = Data.GetLen() - 1;
		
		while(!bDeleted.IsEmpty())
		{
			DINT j = bDeleted.Pop();
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
	}
	Data.Clear();
	Tree.Clear();
	Data.Add();
	Tree.Add();
	bDeleted.Clear();
	bData = Data.GetPtr() + 1;
	bTree = Tree.GetPtr() + 1;
	bTree[-1].Init(-1, -1);
	m_iNum = 0;
	ARRAY::Free();
	ARRAY::ReAlloc(iStartSize);
#ifdef __MASK_HASH__
	iMask = iStartSize - 1; 
#endif
	ARRAY::Set(-1);
}
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline void ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::Reset()
{
	if (m_fNewDelele)
	{
		DINT i = 0, l = Data.GetLen() - 1;
		
		while(!bDeleted.IsEmpty())
		{
			DINT j = bDeleted.Pop();
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
	}
	Data.SetLen(1);
	Tree.SetLen(1);
	bDeleted.Clear();
	bData = Data.GetPtr() + 1;
	bTree = Tree.GetPtr() + 1;
	bTree[-1].Init(-1, -1);
	m_iNum = 0;
	ARRAY::Free();
	ARRAY::ReAlloc(iStartSize);
#ifdef __MASK_HASH__
	iMask = iStartSize - 1; 
#endif
	ARRAY::Set(-1);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline MAP_VALUE &ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::Add(MAP_KEY &key, MAP_VALUE &value)
{
	MAP_VALUE &val = Value(key);
	val = value;
	return val;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline void ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::Delete(MAP_KEY key)
{
	Delete(ARRAY::ptr + GetHash(key), key);
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline bool ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::Lookup(MAP_KEY key, MAP_VALUE &b)
{
	DINT node = ARRAY::ptr[GetHash(key)];
		while (node != -1 && bData[node].key != key)
			node = bData[node].key > key ? bTree[node].left : bTree[node].right;
	if (node != -1) 
	{ 
		b = bData[node].value; 
		return 1; 
	} 
	return false;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline bool ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::Lookup(MAP_KEY key, MAP_VALUE **b)
{
	DINT node = ARRAY::ptr[GetHash(key)];
		while (node != -1 && bData[node].key != key)
			node = bData[node].key > key ? bTree[node].left : bTree[node].right;
	if (node != -1) 
	{ 
		if (b) 
			*b = &bData[node].value; 
		return 1; 
	} 
	return false;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline MAP_VALUE &ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::Value(MAP_KEY key)
{
#ifdef __MASK_HASH__
	if (ARRAY::Len < iMaxSize && (m_iNum >> iRank) > iMask) Rebuild();
#else
	if (ARRAY::Len < iMaxSize && (m_iNum >> iRank) > ARRAY::Len) Rebuild();
#endif
	DINT node = -1;
	Insert(ARRAY::ptr + GetHash(key), key, 0, &node);
	return bData[node].value;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline int ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::GetHash(LPCSTR p) const
{
	if (!p) return 0xFFFFFFFF;
	DWORD hash = 2166136261U;
	while (*p)
		hash = 16777619U * hash ^ *p++;
	return GetHash(hash);
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline int ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::GetHash(LPCWSTR p) const
{
	if (!p) return 0xFFFFFFFF;
	DWORD hash = 2166136261U;
	while (*p)
		hash = 16777619U * hash ^ *p++;
	return GetHash(hash);
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline int ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::GetHash(BYTE *p, int len) const
{
	if (!p) return 0xFFFFFFFF;
//	DWORD hash = 0x9e3779b9;
	DWORD hash = 2166136261U;
	while (len--)
		hash = 16777619U * hash ^ *p++;
	return GetHash(hash);
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline int ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::GetHash(float &p) const
{
	return GetHash((BYTE *)&p, sizeof(float));
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline int ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::_GetHash(DWORD ID) const
{

#ifdef __MASK_HASH__
	return (int)(ID & iMask);
#else
	return (DWORD)(( ID % (DWORD)ARRAY::Len + ID / (DWORD)ARRAY::Len ) % (DWORD)ARRAY::Len );
#endif
}
template <class MAP_VALUE, class MAP_KEY, class ARRAY>
inline int ArHashMap<MAP_VALUE, MAP_KEY, ARRAY>::GetHash(void *ID) const
{
	return GetHash((size_t)ID);
}


#endif
