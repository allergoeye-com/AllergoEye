///////////////////////////////////////////////////
// mheap.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////
#ifndef __M_HEAP__
#define __M_HEAP__
#include "rbmap.h"
/***************************************************
****************************************************/
template <class T>
class TBHeap : public MArray<T *> {
public:
	TBHeap(bool fNewDelele = false) { defAdded = 1024; m_bDir = true; m_fNewDelele = fNewDelele; };
	~TBHeap() {};
	void CreateNew(bool fNewDelele) { m_fNewDelele = fNewDelele; } 
	void Insert(T);
	bool DirectHight(bool dir) { bool old = m_bDir; m_bDir = dir; if (old != dir) Sort(); return old; } 
	T Extract();
	void Erase();
	void Sort();
protected:
	void ReMake(int i);
	Allocator<T> m_Alloc;
	bool m_bDir;
	bool m_fNewDelele;
};


/***************************************************
****************************************************/
template <class DYNAMIC_KEY, class STATIC_KEY >
struct TFVertix {
	TFVertix *left;
	TFVertix *right;
	TFVertix *parent;
	TFVertix *child;
	DYNAMIC_KEY dynamic_key;
	STATIC_KEY static_key;
	int nChilds;
	int mark;
	TFVertix() { left = right = parent = child = this; nChilds = 0; mark = false;  };
	void Init(DYNAMIC_KEY _key, TFVertix *_left, TFVertix *_right, TFVertix *_parent,
				TFVertix *_child, int _degree = 0, bool _mark = false)
				{ dynamic_key = _key; left = _left; right = _right; parent = _parent; 
				child = _child;	nChilds = _degree; mark = _mark; };
	TFVertix(TFVertix &b)
	{
		dynamic_key = b.dynamic_key; 
		left = b.left; 
		right = b.right; 
		parent = b.parent; 
		child = b.child;	
		nChilds = b.nChilds; 
		mark = b.mark; 
		static_key = b.static_key;
	}
	~TFVertix() {}
	TFVertix &operator = (const TFVertix &b)
	{
		dynamic_key = b.dynamic_key; 
		left = b.left; 
		right = b.right; 
		parent = b.parent; 
		child = b.child;	
		nChilds = b.nChilds; 
		mark = b.mark; 
		static_key = b.static_key;
		return *this;
	}
	

};

#define FRBMap RBMap< TFVertix<DYNAMIC_KEY, STATIC_KEY> *, STATIC_KEY>
template <class DYNAMIC_KEY, class STATIC_KEY >
class TFHeap : public FRBMap  {
public:
	TFHeap (DYNAMIC_KEY minimum, int iMaxVertix = 100, bool fNewDelele = true ) 
	{ m_MIN = minimum; m_fNewDelele = fNewDelele; SetMaxVertix(iMaxVertix);
			m_Nil.Init(0, &m_Nil, &m_Nil, &m_Nil, &m_Nil, 0, 0); 
		pMin = &m_Nil;  SetMaxVertix(100); }
	void Reset() { Clear(); m_Nil.Init(0, &m_Nil, &m_Nil, &m_Nil, &m_Nil, 0, 0); 
		pMin = &m_Nil; }

	~TFHeap() {};
	//------------------------------------------------------
	void SetMinKey(DYNAMIC_KEY minimum) { m_MIN = minimum; };
	void SetMaxVertix(int iMaxVertix);
	void CreateNew(bool fNewDelele) { m_fNewDelele = fNewDelele; } 
	//------------------------------------------------------------
	bool Add (STATIC_KEY static_key, DYNAMIC_KEY dynamic_key);
	DYNAMIC_KEY operator [] (STATIC_KEY dynamic_key) { TFVertix<DYNAMIC_KEY, STATIC_KEY> **p; return Lookup(dynamic_key, &p) ?  (*p)->dynamic_key : m_MIN; }
	bool GetMin(STATIC_KEY &out_value, DYNAMIC_KEY &out_key);
	bool DeleteMin(STATIC_KEY &out_value, DYNAMIC_KEY &out_key);
	bool ToChange(STATIC_KEY static_key, DYNAMIC_KEY k);
	bool Delete(STATIC_KEY static_key);
private:
	TFVertix<DYNAMIC_KEY, STATIC_KEY> *ExtractMin();
	void Delete(TFVertix<DYNAMIC_KEY, STATIC_KEY> *x);
	void ToChange(TFVertix<DYNAMIC_KEY, STATIC_KEY> *x, DYNAMIC_KEY k);
	void Insert (TFVertix<DYNAMIC_KEY, STATIC_KEY> *p, TFVertix<DYNAMIC_KEY, STATIC_KEY> *x);
	
	void RemoveInTree (TFVertix<DYNAMIC_KEY, STATIC_KEY> *x);
	void Link(TFVertix<DYNAMIC_KEY, STATIC_KEY> *x, TFVertix<DYNAMIC_KEY, STATIC_KEY> *y);
	void Consolidate();
	void Cut(TFVertix<DYNAMIC_KEY, STATIC_KEY> *x, TFVertix<DYNAMIC_KEY, STATIC_KEY> *y);
	void CascadingCut(TFVertix<DYNAMIC_KEY, STATIC_KEY> *y);
	TFVertix<DYNAMIC_KEY, STATIC_KEY> *pMin;
	TFVertix<DYNAMIC_KEY, STATIC_KEY> m_Nil;
	MArray<TFVertix<DYNAMIC_KEY, STATIC_KEY> *> m_tmp;
	
	DYNAMIC_KEY m_MIN;
	bool m_fNewDelele;
	int m_iFibValue;
	Allocator<TFVertix<DYNAMIC_KEY, STATIC_KEY> > m_Alloc;
};
//---------------------------------------
//
//---------------------------------------
template <class T>
inline void TBHeap<T>::Sort() 
{

	if (Len > 1)
	{
		int j = Len >> 1;
		do		
			ReMake(j);
		while(j--);
	}
}

//---------------------------------------
//
//---------------------------------------
template <class T>
inline T TBHeap<T>::Extract()
{
	if (Len--)
	{
		T *n = *ptr;
		if (Len)
		{
			*ptr = *(ptr + Len);
			ReMake(0);
		}
		T tt = *n;
		if (m_fNewDelele)
			n->~T();
		m_Alloc.Free(n);
		return tt;
	}
	return T();
}
//---------------------------------------
//
//---------------------------------------
template <class T>
void TBHeap<T>::Insert(T data)
{
	T *t;
	if (m_fNewDelele)
	{
		void *f = m_Alloc.Alloc();
		t = new (f) T(); 
	}
	else
		t = m_Alloc.Alloc();


	t = new (t) T();

	*t = data;
	int l = Len;
	if (RealSize > Len + 1)
		++Len;
	else
		if (!ReAlloc(Len + 1)) return;

	int iParent = l >> 1;
	if (!m_bDir)
	{	
		while (l && *t < *(*(ptr + iParent)))
		{
			*(ptr + l) = *(ptr + iParent);
			l = iParent;
			*(ptr + l) = t;
			iParent >>= 1;
			
		} 
	}
	else
	{
		while (l && *t > *(*(ptr + iParent)))
		{
			*(ptr + l) = *(ptr + iParent);
			l = iParent;
			*(ptr + l) = t;
			iParent >>= 1;
			
		} 
	}
	*(ptr + l) = t;
}
template <class T>
void TBHeap<T>::ReMake(int i)
{
	if (Len)
	{	
		T *tmp;
		
		if (!m_bDir)
		{
			int mini, l, r;
			while(true)
			{
				l =  i << 1;
				r =  l + 1;
				if (l < Len  && *(*(ptr + l)) < *(*(ptr + i)))
					mini = l;
				else
					mini = i;
				if (r < Len && *(*(ptr + r)) < *(*(ptr + mini)))
					mini = r;
				if (mini != i)
				{
					tmp = *(ptr + i);
					*(ptr + i) = *(ptr + mini);
					i = mini;
					*(ptr + i) = tmp;
				}
				else
					break;
			}
		}
		else
		{
			int maxi, l, r;
			while(true)
			{
				l =  i << 1;
				r =  l + 1;
				if (l < Len  && *(*(ptr + l)) > *(*(ptr + i)))
					maxi = l;
				else
					maxi = i;
				if (r < Len && *(*(ptr + r)) > *(*(ptr + mini)))
					maxi = r;
				if (maxi != i)
				{
					tmp = *(ptr + i);
					*(ptr + i) = *(ptr + maxi);
					i = maxi;
					*(ptr + i) = tmp;
				}
				else
					break;
			}
		}
	}
}
//---------------------------------------
//
//---------------------------------------
template <class DYNAMIC_KEY, class STATIC_KEY >
inline void TFHeap<DYNAMIC_KEY, STATIC_KEY>::Delete(TFVertix<DYNAMIC_KEY, STATIC_KEY> *x)
{
	ToChange(x, m_MIN);
	TFVertix<DYNAMIC_KEY, STATIC_KEY> *c = ExtractMin();
	if (c != &m_Nil)
	{
		FRBMap::Delete(c->static_key);
		if (m_fNewDelele)
			c->~TFVertix<DYNAMIC_KEY, STATIC_KEY>();
		m_Alloc.Free(c);
	}
	
}
//---------------------------------------
//
//---------------------------------------
template <class DYNAMIC_KEY, class STATIC_KEY >
inline bool TFHeap<DYNAMIC_KEY, STATIC_KEY>::GetMin(STATIC_KEY &static_key, DYNAMIC_KEY &dynamic_key)
{
	if (pMin != &m_Nil)
	{
		dynamic_key = pMin->dynamic_key;
		static_key = pMin->static_key;
		return 1;
	}
	return 0;
}
//---------------------------------------
//
//---------------------------------------
template <class DYNAMIC_KEY, class STATIC_KEY >
bool TFHeap<DYNAMIC_KEY, STATIC_KEY>::DeleteMin(STATIC_KEY &static_key, DYNAMIC_KEY &dynamic_key)
{
	TFVertix<DYNAMIC_KEY, STATIC_KEY> *c = ExtractMin();
	if (c != &m_Nil)
	{
		dynamic_key = c->dynamic_key;
		static_key = c->static_key;
		FRBMap::Delete(c->static_key);
		if (m_fNewDelele)
			c->~TFVertix<DYNAMIC_KEY, STATIC_KEY>();
		m_Alloc.Free(c);
		return 1;
	}
	return 0;
}

//---------------------------------------
//
//---------------------------------------
template <class DYNAMIC_KEY, class STATIC_KEY >
inline void TFHeap<DYNAMIC_KEY, STATIC_KEY>::Insert (TFVertix<DYNAMIC_KEY, STATIC_KEY> *p, TFVertix<DYNAMIC_KEY, STATIC_KEY> *x)
{
	p->left->right = x->right;
	x->right->left = p->left;
	p->left = x;
	x->right = p;
}
//---------------------------------------
//
//---------------------------------------
template <class DYNAMIC_KEY, class STATIC_KEY >
inline void TFHeap<DYNAMIC_KEY, STATIC_KEY>:: RemoveInTree (TFVertix<DYNAMIC_KEY, STATIC_KEY> *x)
{
	x->left->right = x->right;
	x->right->left = x->left;
}
//---------------------------------------
//
//---------------------------------------
template <class DYNAMIC_KEY, class STATIC_KEY >
bool TFHeap<DYNAMIC_KEY, STATIC_KEY>::Add (STATIC_KEY static_key, DYNAMIC_KEY dynamic_key)
{
	TFVertix<DYNAMIC_KEY, STATIC_KEY> *x = 0, **p= 0;
	if (Lookup(static_key, &p)) return 0;
	if (m_fNewDelele)
	{
		void *f = m_Alloc.Alloc();
		x = new (f) TFVertix<DYNAMIC_KEY, STATIC_KEY>(); 
	}
	else
		x = m_Alloc.Alloc();
	FRBMap::operator [] (static_key) = x; 
	


	x->Init(dynamic_key, x, x, &m_Nil, &m_Nil, 0, false);
	x->static_key = static_key; 
	if (pMin != &m_Nil)
		Insert (pMin, x);
	if (pMin == &m_Nil || x->dynamic_key < pMin->dynamic_key)
		pMin = x;

	
	return 1;
}
//---------------------------------------
//
//---------------------------------------
template <class DYNAMIC_KEY, class STATIC_KEY >
inline void TFHeap<DYNAMIC_KEY, STATIC_KEY>::Link(TFVertix<DYNAMIC_KEY, STATIC_KEY> *y, TFVertix<DYNAMIC_KEY, STATIC_KEY> *x)
{
	 RemoveInTree(y);
	y->parent = x;
	y->left = y->right = y;
	if (x->child == &m_Nil)
		x->child = y;
	else
		Insert(x->child, y);
	++x->nChilds;
}
//---------------------------------------
//
//---------------------------------------
template <class DYNAMIC_KEY, class STATIC_KEY >
inline bool TFHeap<DYNAMIC_KEY, STATIC_KEY>::Delete(STATIC_KEY static_key)
{
	TFVertix<DYNAMIC_KEY, STATIC_KEY> **x = 0;
	if (!Lookup(static_key, &x)) return 0;
	Delete(*x);
	return true;

}
//---------------------------------------
//
//---------------------------------------
template <class DYNAMIC_KEY, class STATIC_KEY >
bool TFHeap<DYNAMIC_KEY, STATIC_KEY>::ToChange(STATIC_KEY static_key, DYNAMIC_KEY k)
{
	TFVertix<DYNAMIC_KEY, STATIC_KEY> **x = 0;
	if (!Lookup(static_key, &x)) return 0;
	if (k > (*x)->dynamic_key)
	{ 
		if (!(*x)->nChilds)
			(*x)->dynamic_key = k;
		else
		{
			ToChange(*x, m_MIN);
			TFVertix<DYNAMIC_KEY, STATIC_KEY> *x1 = ExtractMin();
			ASSERT(x1 == *x);
			(*x)->Init(k, *x, *x, &m_Nil, &m_Nil, 0, false);
			if (pMin != &m_Nil)
				Insert (pMin, *x);
			if (pMin == &m_Nil || (*x)->dynamic_key < pMin->dynamic_key)
				pMin = *x;
		}
	}
	else
		ToChange(*x, k);
	return true;

}
//---------------------------------------
//
//---------------------------------------
template <class DYNAMIC_KEY, class STATIC_KEY >
inline void TFHeap<DYNAMIC_KEY, STATIC_KEY>::ToChange(TFVertix<DYNAMIC_KEY, STATIC_KEY> *x, DYNAMIC_KEY k)
{
	if (k > x->dynamic_key)
		return;
	x->dynamic_key = k;
	TFVertix<DYNAMIC_KEY, STATIC_KEY> *y = x->parent;
	if (y != &m_Nil && x->dynamic_key < y->dynamic_key)
	{
		Cut(x, y);
		CascadingCut(y);
	}
	if (x->dynamic_key < pMin->dynamic_key)
		pMin = x;
}
//---------------------------------------
//
//---------------------------------------
template <class DYNAMIC_KEY, class STATIC_KEY >
inline void TFHeap<DYNAMIC_KEY, STATIC_KEY>::Cut(TFVertix<DYNAMIC_KEY, STATIC_KEY> *x, TFVertix<DYNAMIC_KEY, STATIC_KEY> *y)
{
	if (y->nChilds == 1)
		y->child = &m_Nil;
	else
		 RemoveInTree(x);
	--y->nChilds;
	x->parent = &m_Nil;
	x->mark = false;
	x->left = x->right = x;
	Insert(pMin, x);
}
//---------------------------------------
//
//---------------------------------------
template <class DYNAMIC_KEY, class STATIC_KEY >
inline void TFHeap<DYNAMIC_KEY, STATIC_KEY>::CascadingCut(TFVertix<DYNAMIC_KEY, STATIC_KEY> *y)
{
	TFVertix<DYNAMIC_KEY, STATIC_KEY> *z = y->parent;
	while (z != &m_Nil)
	{
		if (!y->mark)
		{
			y->mark = true;
			z = &m_Nil;
		}
		else
		{
			Cut(y, z);
			z = z->parent;
		}
	}
}
//---------------------------------------
//
//---------------------------------------
template <class DYNAMIC_KEY, class STATIC_KEY >
inline void TFHeap<DYNAMIC_KEY, STATIC_KEY>::SetMaxVertix(int m) 
{ 
	
	m_iFibValue = int(1 + 1.44 * log(m)/log(2.0)) + 1; 
	m_tmp.SetLen(m_iFibValue * sizeof(TFVertix<DYNAMIC_KEY, STATIC_KEY> *));
	int i = m_iFibValue;
	TFVertix<DYNAMIC_KEY, STATIC_KEY> **b = m_tmp.GetPtr();

	while(i--)
		*b++ = &m_Nil;
	
} 
//---------------------------------------
//
//---------------------------------------
template <class DYNAMIC_KEY, class STATIC_KEY >
TFVertix<DYNAMIC_KEY, STATIC_KEY> *TFHeap<DYNAMIC_KEY, STATIC_KEY>::ExtractMin()
{
	TFVertix<DYNAMIC_KEY, STATIC_KEY> *z = pMin;
	if (z != &m_Nil)
	{
		if (z->child != &m_Nil)
		{
			TFVertix<DYNAMIC_KEY, STATIC_KEY> *x = z->child;
			do
				x->parent = &m_Nil;
			while((x = x->right) != z->child);
			Insert (pMin, z->child);
		}
		
		
		if (z == z->right)
			pMin = &m_Nil;
		else
		{
			pMin = z->right;
			z->left->right = z->right;
			z->right->left = z->left;
			Consolidate();
		}
		return z;
	}
	return z;	
}
//---------------------------------------
//
//---------------------------------------
template <class DYNAMIC_KEY, class STATIC_KEY >
void TFHeap<DYNAMIC_KEY, STATIC_KEY>::Consolidate()
{
	TFVertix<DYNAMIC_KEY, STATIC_KEY> *p1, *p, *w, *y, *x, **b;
	TFVertix<DYNAMIC_KEY, STATIC_KEY> **ptr = m_tmp.GetPtr();
	int n, d, i = 0;
    p1 = pMin->left;
	w = pMin;
	do
	{
		x = w;
		w = w->right;
		d = x->nChilds;
		while(*(ptr + d) != &m_Nil)
		{
			y = *(ptr + d);
			if (x->dynamic_key > y->dynamic_key)
			{
				p = x;
				x = y;
				y = p;
			}
			if (w == y)
				w = y->right;
			y->left->right = y->right;
			y->right->left = y->left;
			y->parent = x;
			y->left = y->right = y;
			if (x->child == &m_Nil)
				x->child = y;
			else
				Insert(x->child, y);
			++x->nChilds;
			*(ptr + d) = &m_Nil;
			++d;
		} 
		*(ptr + d) = x;
		i = max(i, d);
		
	}while(w != p1);
	b = ptr;
	do 
		*b++ = w;
	while((w = w->right) != p1);
	*b = pMin = &m_Nil;
	n = 0;
	b = ptr;
	
	while(*b != &m_Nil)
	{
		(*b)->left = (*b)->right = *b;
		if (pMin != &m_Nil)
		{
			Insert (pMin, *b);
			if ((*b)->dynamic_key < pMin->dynamic_key)
				pMin = *b;
		}
		else
			pMin = *b;
		++n;
		*b++ = &m_Nil;
	}
	++i;
	i -= n;
	if (i > 0)
		while(i--)
			*b++ = &m_Nil;
}
#endif
