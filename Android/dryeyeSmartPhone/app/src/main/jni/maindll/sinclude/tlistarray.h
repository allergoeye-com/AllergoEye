///////////////////////////////////////////////////
// TListArray.h: interface for the TListArray class.
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TLISTARRAY_H__51D04273_4F0E_4F8D_9D55_543C71445F2C__INCLUDED_)
#define AFX_TLISTARRAY_H__51D04273_4F0E_4F8D_9D55_543C71445F2C__INCLUDED_

template <class T>
struct _TListArray {
	struct node {
		node()
		{
		}
		void Init(node *a)
		{
			next = prev = 0;
			if (a)
			{
				prev = a;
				a->next = this;	
			}
		}
		void Reset() 
		{
			if (prev) prev->next = next;
			if (next) next->prev = prev;
			next = prev = 0;
		}
		void Init(T _val, node *_prev = 0, node *_next = 0)
		{
			val = _val;
			next = _next;
			prev = _prev;

		} 
		node *prev, *next;
		T val;
	};


	T &Add(T a, node *entry)
	{					
		entry->Init(last);
		entry->val = a;
		if (!first) first = entry;
		last = entry;
		++num;
		return entry->val;
	}

	T &Add(node *entry)
	{
		entry->Init(last);
		if (!first) first = entry;
		last = entry;
		++num;
		return entry->val;
	}
	void Add(node *entry, int n)
	{
		for (int i = 0; i < n; ++i)
		Add(entry + i);
	}

	void Reset() {first = last = 0; num = 0; }
	void MoveTo(node *entry, _TListArray *dst)
	{
		node *tmp = entry;
		if (last == tmp) last = tmp->prev;
		if (first == tmp) first = tmp->next;
		tmp->Reset();
		--num;
		dst->Add(tmp);
	}
	void MoveTo(_TListArray *dst)
	{
		node *tmp, *entry = first;
		while(entry)
		{
			tmp = entry->next;
			MoveTo(entry, dst);
			entry = tmp;
		}
	}
	void UpdateNum()
	{
		node *entry = first;
		num = 0;
		while(entry)
		{
			entry = entry->next;
			++num;
		}
	}

	void Del(node *entry)
	{
		node *tmp = entry;
		if (last == tmp) last = tmp->prev;
		if (first == tmp) first = tmp->next;
		tmp->Reset();
		--num;
	}
		
	node *first, *last;
	DINT num;
};

template <class T>
struct TListArray : public _TListArray<T> {

	TListArray() { _TListArray<T>::first = _TListArray<T>::last = 0; _TListArray<T>::num = 0;};
	~TListArray() {  }
};
template <class Vector3, class Box3>
struct SearchPoint3Node  
{
	Box3 rc;
	TListArray<DWORD> pos;
	SearchPoint3Node *child;
	SearchPoint3Node *next;
	Vector3 *rec;

	SearchPoint3Node() { next = child = 0; rec = 0; rc.Reset(); }
	void Add(Box3 &_rc, Allocator<SearchPoint3Node<Vector3, Box3> > &mAlloc);
};


template <class Vector3, class Box3>
class SearchPoint3:public SearchPoint3Node<Vector3, Box3>
{
public:
	SearchPoint3(MArray<Vector3> &p):pt(p) {iMaxNumNode = 20; }
	~SearchPoint3() {}
	void Build(Box3 &box);
	INT64 Search(Vector3 &v, Box3 &find, double &dist);
	void Clear() 
	{
		list.Clear();
		mAlloc.Clear();
	
	}
protected:
	void Split();
protected:
	DINT iMaxNumNode;
	MArray<TListArray<DWORD>::node> list;
	Allocator<SearchPoint3Node<Vector3, Box3> > mAlloc;
	MArray<Vector3> &pt;

};
template <class Vector3, class Box3>
void SearchPoint3Node<Vector3, Box3>::Add(Box3 &_rc, Allocator<SearchPoint3Node<Vector3, Box3> > &mAlloc)
{
	
	SearchPoint3Node *a = mAlloc.Alloc(1);

	a->rc = _rc;
	a->rec = rec;
	TListArray<DWORD>::node *entry = pos.first;
	DINT prev_n = a->pos.num; 	
	while(entry)
	{
		if (a->rc.PtInBox(rec[entry->val])) 
		{
			TListArray<DWORD>::node *tmp = entry;
			tmp = entry->prev;
			  
			pos.MoveTo(entry, &a->pos);
			if (!tmp)
			{
				entry = pos.first;
				continue;
			}
			else
				entry = tmp;
		}
		entry = entry->next; 	
	}
	if (prev_n == a->pos.num)
	{

		mAlloc.Free(a);
	}
	else
	{
		a->next = child;
		child = a;	
	} 	
}


template <class Vector3, class Box3>
INT64 SearchPoint3<Vector3, Box3>::Search(Vector3 &ps, Box3 &find, double &dist)
{
	SearchPoint3Node<Vector3, Box3> *tree = this;
	MArray<SearchPoint3Node<Vector3, Box3> *> stack(100);
	DINT len_stack = 100;
	DINT pos_stack = 0;
	INT64 pos_i = -1;
	double d;
	dist = INT_MAX;
	
	SearchPoint3Node<Vector3, Box3> **pStack = stack.GetPtr();
        stack[0] = this;
        pos_stack = 1;
	for(;tree->child && pos_stack;)
	{	
		if (tree->rc.Intersect(find))
		{		
			SearchPoint3Node<Vector3, Box3> *node = tree->child;
			while (node)
			{
				if (node->rc.Intersect(find))
				{
					pStack[pos_stack] = node;
					++pos_stack;
				}
				node = node->next;
			 }
		}
		if (pos_stack)
		{
			--pos_stack;
			tree = pStack[pos_stack];
		}
	}

	for(;;)
	{	
		if (!tree->child && tree->pos.num)
		{
			TListArray<DWORD>::node *entry = tree->pos.first;
			while(entry)
			{
				if (find.PtInBox(pt[entry->val]))
				{
					if ((d = (pt[entry->val] - ps).LenQ()) < dist)
					{
						dist = d;
						pos_i = entry->val;
					}
				}
				entry = entry->next;
							 
			}	 	
		}
		if (pos_stack)
		{
			--pos_stack;
			tree = pStack[pos_stack];
			continue;
		}
		break;
	}
	
	return pos_i;
}
template <class Vector3, class Box3>
void SearchPoint3<Vector3, Box3>::Build(Box3 &box)
{
	Vector3 *p = pt.GetPtr();
	DINT len = pt.GetLen();
	SearchPoint3Node<Vector3, Box3>::rc = box;
	list.Clear();
	list.SetLen(len);
	TListArray<DWORD>::node *plist = list.GetPtr();
	SearchPoint3Node<Vector3, Box3>::rec = p;
	DINT i = 0;
	Box3 db;
	for (; i < len; ++i, ++p, ++plist)
	{
		plist->Init(i);
		SearchPoint3Node<Vector3, Box3>::pos.Add(plist);
	}
	Split();

}
template <class Vector3, class Box3>
void SearchPoint3<Vector3, Box3>::Split()
{
	SearchPoint3Node<Vector3, Box3> *tree = this;
	MArray<SearchPoint3Node<Vector3, Box3> *> stack(100);
	DINT len_stack = 100;
	DINT pos_stack = 0;
	SearchPoint3Node<Vector3, Box3> **pStack = stack.GetPtr();
	Vector3 cp;
	Box3 rc;
	DINT oldNum;
	for(;;)
	{	
		
		oldNum = tree->pos.num;
		cp.Set(tree->rc.CenterPoint()); 
		rc.Set(tree->rc[0], cp);
		tree->Add(rc, mAlloc);
		rc.Set(tree->rc[1], cp);
		tree->Add(rc, mAlloc);
		rc.Set(tree->rc[2], cp);
		tree->Add(rc, mAlloc);
		rc.Set(tree->rc[3], cp);
		tree->Add(rc, mAlloc);
		rc.Set(tree->rc[4], cp);
		tree->Add(rc, mAlloc);
		rc.Set(tree->rc[5], cp);
		tree->Add(rc, mAlloc);
		rc.Set(tree->rc[6], cp);
		tree->Add(rc, mAlloc);
		rc.Set(tree->rc[7], cp);
		tree->Add(rc, mAlloc);
		if (oldNum > tree->pos.num)
		{
		
			SearchPoint3Node<Vector3, Box3> *node = tree->child;
			while (node)
			{
				if (node->pos.num > iMaxNumNode)
				{
					if (pos_stack == len_stack)
					{
						len_stack <<= 1;
						stack.SetLen(len_stack);
						pStack = stack.GetPtr();
					}
					pStack[pos_stack] = node;
					++pos_stack;
				}

				
				node = node->next;
			 }
		}
		if (pos_stack)
		{
			--pos_stack;
			tree = pStack[pos_stack];
			continue;
		}
		break;


	}
}

#endif // !defined(AFX_TLISTARRAY_H__51D04273_4F0E_4F8D_9D55_543C71445F2C__INCLUDED_)
