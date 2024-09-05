#ifndef __NodeTreeIt__
#define __NodeTreeIt__
#include "mtree.h"
#include "mstack.h"
#pragma pack(push)
#pragma pack(4)

struct GNode {
	union {
	DINT num;
	DINT pv;
	float pwt;
	};
	DINT pw;
	GNode(float v, DINT w) {pwt = v; pw = w; } 
	GNode(DINT v, DINT w) {num = v; pw = w; } 
	GNode() { pv = pw = 0; }
	void Set(DINT v, DINT w) {num = v; pw = w; } 
	void Set(float v, DINT w) { pwt = v; pw = w; }

};
struct GNodeW:public GraphEdge  {
//	DINT pv;
//	DINT pw;
	double pwt;
	void Set(DINT v, DINT w, double _pwt) {pv = v; pw = w; pwt = _pwt; } 
	
};
class HeapEdge : protected MArray<GNodeW>, protected MHashMap<DINT,  GraphEdge>
{
public:
	HeapEdge() : ptr(MArray<GNodeW>::ptr) { iRank = 1; ffFix = false; N = 0;}
	~HeapEdge() {}
	DINT IsEmpty() const { return N - ffFix == 0; }
	GNodeW *Extract();
	void Delete(DINT ie, DINT w);
	void Decrease(DINT ie, DINT w, float P);
	void Insert(DINT ie, DINT w, float P);
	void Clear() { MHashMap<DINT,  GraphEdge>::Clear(); iRank = 1; ffFix = false; N = 0; }
protected:
	void FixUp(DINT k);
	void FixDown(DINT _i);
	DINT &Index(GraphEdge &key);
	void SetIndex(GraphEdge &key, const DINT &val);
	void Swap(DINT a, DINT b);

	DINT N;
	bool ffFix;
	GNodeW *&ptr;
};
//------------------------------------------------------------
//
//------------------------------------------------------------
inline DINT &HeapEdge::Index(GraphEdge &key)
{
	HMapTreeNode<DINT,  GraphEdge> *node = MMalloc<HMapTreeNode<DINT,  GraphEdge> *>::ptr[GetHash(key)];
	while (node && node->key != key)
		node = node->key > key ? node->left : node->right;
	if (node)
		return node->value;
	return nil.value;
}
//------------------------------------------------------------
//
//------------------------------------------------------------
inline void HeapEdge::SetIndex(GraphEdge &key, const DINT &val)
{
	HMapTreeNode<DINT,  GraphEdge> *node = MMalloc<HMapTreeNode<DINT,  GraphEdge> *>::ptr[GetHash(key)];
	while (node && node->key != key)
		node = node->key > key ? node->left : node->right;
	if (node)
		node->value = val;
}

//------------------------------------------------------------
//
//------------------------------------------------------------
inline void HeapEdge::FixUp(DINT k)
{ 
	DINT iParent = k >> 1;
	while (iParent && (ptr + iParent)->pwt > (ptr + k)->pwt)
	{ 
		Swap(k, iParent);
		k = iParent;
		iParent >>= 1; 
	}		
}
//------------------------------------------------------------------
//
//------------------------------------------------------------------
inline void HeapEdge::FixDown(DINT _i)
{ 
	DINT mini, l;
	while((l = _i << 1) <= N)
	{
		mini = (ptr + l)->pwt < (ptr + _i)->pwt ? l : _i;
		++l;
		if (l <= N && (ptr + l)->pwt < (ptr + mini)->pwt)
			mini = l;
		if (mini != _i)
		{
			Swap(_i, mini);
			_i = mini;
		}
		else
			break;
	}

}
//------------------------------------------------------------------
//
//------------------------------------------------------------------
inline void HeapEdge::Delete(DINT ie, DINT w)
{
	DINT t = 0;
	GraphEdge pv(ie, w);
	if (MHashMap<DINT,  GraphEdge>::Lookup(pv, t))
	{	
		if (ffFix)
		{
		
			ffFix = 0;
			ptr[1] = ptr[N];
			--N; 
			SetIndex(ptr[1], 1); 
			FixDown(1);
			t = Index(pv);
		}
		DINT iParent = t >> 1;
		while (iParent)
		{ 
			Swap(t, iParent);
			t = iParent;
			iParent >>= 1; 
		}
		Swap(1, N);
		MHashMap<DINT,  GraphEdge>::Delete(ptr[N]);
		--N;
		FixDown(1);
	}
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
inline void HeapEdge::Decrease(DINT e, DINT w, float P)  
{ 
	DINT t = 0;
	GraphEdge pv(e, w);
	if (MHashMap<DINT,  GraphEdge>::Lookup(pv, t))
	{	
		if (ffFix)
		{
		
			ffFix = 0;
			ptr[1] = ptr[N];
			--N; 
			SetIndex(ptr[1], 1); 
			FixDown(1);
			t = Index(pv);
		}
		ptr[t].pwt = P;
		FixUp(t);
	}
	else
		Insert(e, w, P);
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
inline void HeapEdge::Insert(DINT e, DINT w, float P)  
{ 
	if (ffFix)
	{
		ptr[1].Set(e, w, P);
		ffFix = 0;
		MHashMap<DINT,  GraphEdge>::Value(ptr[1]) = 1;
		FixDown(1);
	}
	else
	{
		if (N + 1 >= MMalloc<GNodeW>::Len)
			MMalloc<GNodeW>::ReAlloc(MMalloc<GNodeW>::Len + 1000);
		ptr[++N].Set(e, w, P); 
		MHashMap<DINT,  GraphEdge>::Value(ptr[N]) = N; 
		FixUp(N); 
	}
}
//------------------------------------------------------------------
//
//------------------------------------------------------------------
inline void HeapEdge::Swap(DINT a, DINT b)
{
	*ptr = *(ptr + a); 
	*(ptr + a) = *(ptr + b); 
	*(ptr + b) = *ptr;
	SetIndex(*(ptr + a), a); 
	SetIndex(*(ptr + b), b); 
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
inline GNodeW *HeapEdge::Extract()
{ 
	
	if (ffFix)
	{
		ptr[1] = ptr[N];
		--N;
		ffFix = 0;
		SetIndex(ptr[1], 1); 
		FixDown(1);
	}
	if ((ffFix = N > 2) || N == 1)
	{
		MHashMap<DINT,  GraphEdge>::Delete(ptr[1]);
		if (!ffFix) --N;
		return ptr + 1;
	}
	Swap(1, N);
	MHashMap<DINT,  GraphEdge>::Delete(ptr[N]);

	--N;
	FixDown(1);
	
	ffFix = 0;
	return ptr + N + 1;
}

class HeapVertex : protected MArray<GNodeW>, protected MHashMap<DINT,  DINT>
{
public:
	HeapVertex() : ptr(MArray<GNodeW>::ptr) { iRank = 1; ffFix = false; N = 0;}
	~HeapVertex() {}
	void FixUp(DINT k);
	void FixDown(DINT _i);
	DINT IsEmpty() const { return N - ffFix == 0; }
	GNodeW *Extract();
	void Decrease(DINT ie, DINT w, float P);
	void Insert(DINT ie, DINT w, float P);
protected:
	//void ReMake(DINT _i, GNodeW *_ptr, DINT _Len);
	DINT &Index(const DINT &key);
	void SetIndex(const DINT &key, const DINT &val);
	void Swap(DINT a, DINT b);

	DINT N;
	bool ffFix;
	GNodeW *&ptr;
	friend class Dijkstra;
};

inline DINT &HeapVertex::Index(const DINT &key)
{
#ifdef __MASK_HASH__
	HMapTreeNode<DINT,  DINT> *node = MMalloc<HMapTreeNode<DINT,  DINT> *>::ptr[key & iMask];
#else
	HMapTreeNode<DINT,  DINT> *node = MMalloc<HMapTreeNode<DINT,  DINT> *>::ptr[GetHash(key)];
#endif
	while (node && node->key != key)
		node = node->key > key ? node->left : node->right;
	if (node)
		return node->value;
	return nil.value;
}
inline void HeapVertex::SetIndex(const DINT &key, const DINT &val)
{
#ifdef __MASK_HASH__
	HMapTreeNode<DINT,  DINT> *node = MMalloc<HMapTreeNode<DINT,  DINT> *>::ptr[key & iMask];
#else
	HMapTreeNode<DINT,  DINT> *node = MMalloc<HMapTreeNode<DINT,  DINT> *>::ptr[GetHash(key)];
#endif
	while (node && node->key != key)
		node = node->key > key ? node->left : node->right;
	if (node)
		node->value = val;
}

inline void HeapVertex::FixUp(DINT k)
{ 
	DINT iParent = k >> 1;
	while (iParent && (ptr + iParent)->pwt > (ptr + k)->pwt)
	{ 
		Swap(k, iParent);
		k = iParent;
		iParent >>= 1; 
	}		
}
//------------------------------------------------------------------
//
//------------------------------------------------------------------
inline void HeapVertex::FixDown(DINT _i)
{ 
	DINT mini, l;
	while((l = _i << 1) <= N)
	{
		mini = (ptr + l)->pwt < (ptr + _i)->pwt ? l : _i;
		++l;
		if (l <= N && (ptr + l)->pwt < (ptr + mini)->pwt)
			mini = l;
		if (mini != _i)
		{
			Swap(_i, mini);
			_i = mini;
		}
		else
			break;
	}

}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
inline void HeapVertex::Decrease(DINT e, DINT w, float P)  
{ 
	DINT t = 0;
	if (MHashMap<DINT,  DINT>::Lookup(w, t))
	{	
		if (ffFix)
		{
		
			ffFix = 0;
			ptr[1] = ptr[N];
			--N; 
			SetIndex(ptr[1].pw, 1); 
			FixDown(1);
			t = Index(w);
		}
		
		ptr[t].pv = e;
		ptr[t].pwt = P;
		FixUp(t);
	}
	else
		Insert(e, w, P);
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
inline void HeapVertex::Insert(DINT e, DINT w, float P)  
{ 
	if (ffFix)
	{
		ptr[1].Set(e, w, P);
		ffFix = 0;
		MHashMap<DINT,  DINT>::Value(w) = 1;
		FixDown(1);
	}
	else
	{
		if (N + 1 >= MMalloc<GNodeW>::Len)
			MMalloc<GNodeW>::ReAlloc(MMalloc<GNodeW>::Len + 1000);
		ptr[++N].Set(e, w, P); 
		MHashMap<DINT,  DINT>::Value(w) = N; 
		FixUp(N); 
	}
}
//------------------------------------------------------------------
//
//------------------------------------------------------------------
inline void HeapVertex::Swap(DINT a, DINT b)
{
	*ptr = *(ptr + a); 
	*(ptr + a) = *(ptr + b); 
	*(ptr + b) = *ptr;
	SetIndex((ptr + a)->pw, a); 
	SetIndex((ptr + b)->pw, b); 
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
inline GNodeW *HeapVertex::Extract()
{ 
	
	if (ffFix)
	{
		ptr[1] = ptr[N];
		--N;
		ffFix = 0;
		SetIndex(ptr[1].pw, 1); 
		FixDown(1);
	}
	if ((ffFix = N > 2) || N == 1)
	{
		MHashMap<DINT,  DINT>::Delete(ptr[1].pw);
		if (!ffFix) --N;
		return ptr + 1;
	}
	Swap(1, N);
	MHashMap<DINT,  DINT>::Delete(ptr[N].pw);

	--N;
	FixDown(1);
	
	ffFix = 0;
	return ptr + N + 1;
}
class HeapVertexInv : protected MArray<GNodeW>, protected MHashMap<DINT,  DINT>
{
public:
	HeapVertexInv() : ptr(MArray<GNodeW>::ptr) { iRank = 1; ffFix = false; N = 0;}
	~HeapVertexInv() {}
	void FixUp(DINT k);
	void FixDown(DINT _i);
	DINT IsEmpty() const { return N - ffFix == 0; }
	GNodeW *Extract();
	void Decrease(DINT ie, DINT w, float P);
	void Insert(DINT ie, DINT w, float P);
	void Reset() { iRank = 1; ffFix = false; N = 0;}
protected:
	//void ReMake(DINT _i, GNodeW *_ptr, DINT _Len);
	DINT &Index(const DINT &key);
	void SetIndex(const DINT &key, const DINT &val);
	void Swap(DINT a, DINT b);

	DINT N;
	bool ffFix;
	GNodeW *&ptr;
	friend class Dijkstra;
};

inline DINT &HeapVertexInv::Index(const DINT &key)
{
#ifdef __MASK_HASH__
	HMapTreeNode<DINT,  DINT> *node = MMalloc<HMapTreeNode<DINT,  DINT> *>::ptr[key & iMask];
#else
	HMapTreeNode<DINT,  DINT> *node = MMalloc<HMapTreeNode<DINT,  DINT> *>::ptr[GetHash(key)];
#endif
	while (node && node->key != key)
		node = node->key > key ? node->left : node->right;
	if (node)
		return node->value;
	return nil.value;
}
inline void HeapVertexInv::SetIndex(const DINT &key, const DINT &val)
{
#ifdef __MASK_HASH__
	HMapTreeNode<DINT,  DINT> *node = MMalloc<HMapTreeNode<DINT,  DINT> *>::ptr[key & iMask];
#else
	HMapTreeNode<DINT,  DINT> *node = MMalloc<HMapTreeNode<DINT,  DINT> *>::ptr[GetHash(key)];
#endif
	while (node && node->key != key)
		node = node->key > key ? node->left : node->right;
	if (node)
		node->value = val;
}

inline void HeapVertexInv::FixUp(DINT k)
{ 
	DINT iParent = k >> 1;
	while (iParent && (ptr + iParent)->pwt < (ptr + k)->pwt)
	{ 
		Swap(k, iParent);
		k = iParent;
		iParent >>= 1; 
	}		
}
//------------------------------------------------------------------
//
//------------------------------------------------------------------
inline void HeapVertexInv::FixDown(DINT _i)
{ 
	DINT mini, l;
	while((l = _i << 1) <= N)
	{
		mini = (ptr + l)->pwt > (ptr + _i)->pwt ? l : _i;
		++l;
		if (l <= N && (ptr + l)->pwt > (ptr + mini)->pwt)
			mini = l;
		if (mini != _i)
		{
			Swap(_i, mini);
			_i = mini;
		}
		else
			break;
	}

}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
inline void HeapVertexInv::Decrease(DINT e, DINT w, float P)  
{ 
	DINT t = 0;
	if (MHashMap<DINT,  DINT>::Lookup(w, t))
	{	
		if (ffFix)
		{
		
			ffFix = 0;
			ptr[1] = ptr[N];
			--N; 
			SetIndex(ptr[1].pw, 1); 
			FixDown(1);
			t = Index(w);
		}
		
		ptr[t].pv = e;
		ptr[t].pwt = P;
		FixUp(t);
	}
	else
		Insert(e, w, P);
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
inline void HeapVertexInv::Insert(DINT e, DINT w, float P)  
{ 
	if (ffFix)
	{
		ptr[1].Set(e, w, P);
		ffFix = 0;
		MHashMap<DINT,  DINT>::Value(w) = 1;
		FixDown(1);
	}
	else
	{
		if (N + 1 >= MMalloc<GNodeW>::Len)
			MMalloc<GNodeW>::ReAlloc(MMalloc<GNodeW>::Len + 1000);
		ptr[++N].Set(e, w, P); 
		MHashMap<DINT,  DINT>::Value(w) = N; 
		FixUp(N); 
	}
}
//------------------------------------------------------------------
//
//------------------------------------------------------------------
inline void HeapVertexInv::Swap(DINT a, DINT b)
{
	*ptr = *(ptr + a); 
	*(ptr + a) = *(ptr + b); 
	*(ptr + b) = *ptr;
	SetIndex((ptr + a)->pw, a); 
	SetIndex((ptr + b)->pw, b); 
}

//------------------------------------------------------------------
//
//------------------------------------------------------------------
inline GNodeW *HeapVertexInv::Extract()
{ 
	
	if (ffFix)
	{
		ptr[1] = ptr[N];
		--N;
		ffFix = 0;
		SetIndex(ptr[1].pw, 1); 
		FixDown(1);
	}
	if ((ffFix = N > 2) || N == 1)
	{
		MHashMap<DINT,  DINT>::Delete(ptr[1].pw);
		if (!ffFix) --N;
		return ptr + 1;
	}
	Swap(1, N);
	MHashMap<DINT,  DINT>::Delete(ptr[N].pw);

	--N;
	FixDown(1);
	
	ffFix = 0;
	return ptr + N + 1;
}
//------------------------------------------------------------------
//
//------------------------------------------------------------------

struct NodeTreeIt {
	DINT parent;
	DINT child;
	DINT next;
	void Map(NodeTreeIt &p, DINT *i)
	{
		p.parent = parent != (DINT)-1 ? i[parent] : (DINT)-1;
		p.child = child != (DINT)-1 ? i[child] : (DINT)-1;
		p.next = next != (DINT)-1 ? i[next] : (DINT)-1;
	}
	void Reset() {
		parent = child = next = (DINT)-1;
	}
};
class GNode_iterator {
public:
	GNode_iterator(GNode *_adj, DINT _n, DINT _pv = 0)
	{ 
		N = _n;
		adj = _adj; 
		start = 0; 
		num = 0;
		pv = _pv;
		if (pv < N) 
		{ 
			num = adj[pv].num;
			DINT pw = adj[pv].pw;
			start = pw < N ? adj + pw : 0;  
		} 
	}
	GNode *begin() { return begin(pv); }

	GNode *begin(DINT _pv) 
	{ 
		num = i = 0; 
		 pv = _pv;
		if (pv < N) 
		{ 
			num = adj[pv].num;
			DINT pw = adj[pv].pw;
			start = pw < N ? adj + pw : 0;  
		} 
		return next(); 
	}
	GNode *next() { 
		if (start && i < num) {  ++i; return start + i - 1; } return 0; }
	DINT Num() const { return start ? num : 0; }
private:
	GNode *adj;
	GNode *start;
	DINT num;
	DINT pv;
	
	DINT i, N;
};
class NodeTreeIt_iterator {
public:
	NodeTreeIt_iterator(NodeTreeIt *_tree, DINT len, DINT _pv) { N = len; tree = _tree; pv = _pv; }
	DINT begin(DINT _pv, NodeTreeIt *_tree, DINT _n)
	{
		N = _n;
		pv = _pv;
		tree = _tree;
		return begin(); 
	} 

	DINT begin(DINT _pv) 
	{	
		pv = _pv;
		return begin(); 
	} 

	DINT begin() 
	{	 
		stack.Clear(); 
		DINT n = pv; 
		while (n != (DINT)-1 && n < N) 
		{ 
			stack.Push(n);
			n = tree[n].next; 
		}  
		return next(); 
	} 
	DINT nChild() {
		DINT n = pv; 
		DINT ret = tree[n].child != (DINT)-1;
		if ((n = tree[n].child) != (DINT)-1)
		{ 
			while (n != (DINT)-1)
			{
				n = tree[n].next; 
				++ret;
			}
		}  
		return ret;
	}
	
	DINT next() {
		if (stack.IsEmpty()) return (DINT)-1; 		
		DINT n, i = stack.Pop();
		if ((n = tree[i].child) != (DINT)-1)
		{
			while(n != (DINT)-1)			
			{
				stack.Push(n);
				n = tree[n].next;
			}
		}
		return i; 
	}
	DINT Len()
	{
		DINT i, n, len = 0;
		MLifo<DINT> stack;
		n = pv;
		stack.Clear();
		while (n != (DINT)-1 && n < N) 
		{ 
			stack.Push(n);
			n = tree[n].next; 
		}
		while(!stack.IsEmpty())
		{ 		
			i = stack.Pop();
			if ((n = tree[i].child) != (DINT)-1)
			{
				while(n != (DINT)-1 && n < N)			
				{
					stack.Push(n);
					n = tree[n].next;
				}
			}
			++len;
			
		}
		return len; 
	}
private:
	NodeTreeIt *tree;
	DINT N;
	DINT pv;
	MLifo<DINT> stack;

};
#pragma pack(pop)
#include "mvect.h"
#endif

