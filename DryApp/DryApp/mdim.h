#ifndef __M_DIM_EX__
#define __M_DIM_EX__
#include "mtree.h"
#ifdef WIN32
#define MASKFF00 0xFFFFFFFF00000000L
#else
#define MASKFF00 0xFFFFFFFF00000000LLU
#endif
template <class T, class LIST_ALLOC = AllocatorSimple<RBTreeNode<T, __int64> > > //ListNotNew<RBTreeNode<T, int> > >
class MDimExt : public _RBMapSimple(T, __int64) , protected MArray<__int64> {
public:

	typedef _RBMapSimple(T, __int64) RB;

	MDimExt() { iDim = 0;pDim = pMin = pMax = 0; }
	~MDimExt() {  Clear(); }
	MDimExt(MDimExt &d) : _RBMapSimple(T, __int64)(d), MArray<__int64>(d) {  }
	bool SetDim(ULONG _iDim, ...); 
	void Clear() { _RBMapSimple(T, __int64)::CRB::lear(); MArray<__int64>::Clear(); iDim = 0;pDim = pMin = pMax = 0; }
	T &Add(int index0, ...);
	T *operator () (int index0, ...);
	void Delete(int index0, ...);
	int DimN(int i) { return i < iDim ? (int)ptr[i] : 0; }
	int MaxN(int i) { return i < iDim ? (int)pMax[i] : 0; }
	int MinN(int i) { return i < iDim ? (int)pMin[i] : 0; }
	int Dim() { return iDim; }
protected:
	void Init();
protected:
	__int64 *pDim;
	__int64 *pMax;
	__int64 *pMin;
	int iDim;

};
//------------------------------------------------//-----------
//
//-----------------------------------------------------------
template <class T, class LIST_ALLOC>
void MDimExt<T, LIST_ALLOC>::Init()
{
	pDim = ptr + iDim;
	pMin = ptr + iDim * 2;
	pMax = ptr + iDim * 3;

}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class T, class LIST_ALLOC>
bool MDimExt<T, LIST_ALLOC>::SetDim(ULONG _iDim, ...) 
{ 
	va_list argList;
	va_start(argList, _iDim);
	MArray<__int64>::SetLen(_iDim * 4);
	pDim = pMin = pMax = 0;
	iDim = _iDim;	

	if (!ptr) return 0;
	Init();
	int i;
	memset(ptr, 0, sizeof(__int64) * iDim);
	for (i = 0; i < iDim; ++i)
		ptr[i] = va_arg(argList, ULONG);
	va_end(argList);
	__int64 k = 1;
	for (i = iDim - 1; i >= 0; --i)
	{
		   pDim[i] = k;
		   *(pMin + i) = k;
		   k *= ptr[i];
	}	
	_RBMapSimple(T, __int64)::Clear(); 

	return true; 
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------

template <class T, class LIST_ALLOC>
inline T &MDimExt<T, LIST_ALLOC>::Add(int index0, ...) 
{ 
	if (!MArray<__int64>::Len) return RB::m_Nil.value;
	va_list argList;
	va_start(argList, index0);
	__int64 i, n, index = 0;
	index += (__int64)index0 * (*pDim);
	*pMin = min((__int64)index0, *pMin);
	*pMax = max((__int64)index0, *pMax);
	for (i = 1; i < iDim; ++i)
	{
		n = va_arg(argList, int);
		if (n < 0 || n >= *(ptr + i))
		{ 
			
			BoolWarning(0);
			n = ptr[i] - 1;
		}
		*(pMin + i) = min(n, *(pMin + i));
		*(pMax + i) = max(n, *(pMax + i));

		index += n * (*(pDim + i));
	}
	va_end(argList);
	return  RB::Value(index);
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class T, class LIST_ALLOC>
inline void MDimExt<T, LIST_ALLOC>::Delete(int index0, ...) 
{ 
	if (!MArray<__int64>::Len) return;
	va_list argList;
	va_start(argList, index0);
	__int64 i, n, index = 0;
	index += (__int64)index0 * (*pDim);
	for (i = 1; i < iDim; ++i)
	{
		n = va_arg(argList, int);
		if (n < 0 || n >= *(ptr + i) || n > *(pMax + i) || n < *(pMin + i))
		{ 
			va_end(argList);
			BoolWarning(0);
			return;
		}
		index += n * (*(pDim + i));
	}
	va_end(argList);
	RBMap<T, __int64, RBCmp<__int64>,  LIST_ALLOC>::Delete(index);
}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class T, class LIST_ALLOC>
inline T *MDimExt<T, LIST_ALLOC>::operator () (int index0, ...) 
{ 
	if (!MArray<__int64>::Len) return 0;
	va_list argList;
	va_start(argList, index0);
	__int64 i, n, index = 0;
	index += (__int64)index0 * (*pDim);
	for (i = 1; i < iDim; ++i)
	{
		n = va_arg(argList, int);
		if (n < 0 || n >= *(ptr + i) || n > *(pMax + i) || n < *(pMin + i))
		{ 
			va_end(argList);
			if (n > *(pMax + i)) return 0;
			BoolWarning(0);
		
			return 0;
		}
		index += n * (*(pDim + i));
	}
	va_end(argList);
	RBTreeNode<T, __int64> *node = Search(RB::pRoot, index);
	return node != RB::Nil() ? &node->value : 0;
}
template <class T, class LIST_ALLOC = AllocatorSimple<RBTreeNode<T, __int64> > >
class MDim2d : public _RBMapSimple(T, __int64) , protected MArray<int>  {
	union AA{
		struct {
			int  x;
			int  y;
		};
		__int64 i; 
	};

public:
	typedef _RBMapSimple(T, __int64) RB;


	MDim2d() { pMin = pMax = 0; }
	~MDim2d() {  Clear(); }
	MDim2d(MDim2d &d) : _RBMapSimple(T, __int64)(d), MArray<int>(d) { pMin = pMax = 0;  Init(); }
	bool SetDim(ULONG _yDim, ULONG _xDim); 
	void Clear() { 
		if (ptr)
		{
			pMin[0] = ptr[0];
			pMin[1] = ptr[1];
			pMax[0] = 0;
			pMax[1] = 0;
			_RBMapSimple(T, __int64)::Clear(); 
		}
	 }
	T &Add(int indexY, int indexX);
	bool IsEmpty() { return MArray<int>::Len == 0 || _RBMapSimple(T, __int64)::m_iNum == 0; }
	T *operator () (int indexY, int indexX);
	void Delete(int indexY, int indexX);
	void DeleteColumn(int indexX);
	void InsertColumn(int indexX);
	void DeleteRow(int indexY);
	void InsertRow(int indexY);
	int DimN(int i) { return i < 2 ? ptr[i] : 0; }
	int MaxN(int i) { return i < 2 ? pMax[i] : 0; }
	int MinN(int i) { return i < 2 ? pMin[i] : 0; }
protected:
	void Init();
protected:
	int *pMax;
	int *pMin;
public:


};
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class T, class LIST_ALLOC>
void MDim2d<T, LIST_ALLOC>::Init()
{
	pMin = ptr + 2;
	pMax = ptr + 4;

}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class T, class LIST_ALLOC>
bool MDim2d<T, LIST_ALLOC>::SetDim(ULONG _yDim, ULONG _xDim) 
{ 
	if (MArray<int>::GetLen() != 6)
	{
		MArray<int>::SetLen(6);
		pMin = pMax = 0;

		if (!ptr) return 0;
		Init();

		memset(ptr, 0, sizeof(int) * 6);
		ptr[0] = _yDim;
		ptr[1] = _xDim;

		pMin[0] = ptr[0];
		pMin[1] = ptr[1];
		pMax[0] = 0;
		pMax[1] = 0;

		_RBMapSimple(T, __int64)::Clear(); 
	}
	else
	{
		ptr[0] = _yDim;
		ptr[1] = _xDim;

	}
	return true; 
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------

template <class T, class LIST_ALLOC>
inline T &MDim2d<T, LIST_ALLOC>::Add(int indexY, int indexX) 
{ 
	if (!MArray<int>::Len) return RB::m_Nil.value;
	if (indexY < 0 || indexY >= *ptr || indexX < 0 || indexX >= ptr[1])
	{ 
		
		BoolWarning(0);
		return RB::m_Nil.value;
	}
	pMin[0] = min(pMin[0], indexY);
	pMax[0] = max(pMax[0], indexY);
	pMin[1] = min(pMin[1], indexX);
	pMax[1] = max(pMax[1], indexX);

	__int64 index	= ((((__int64)indexY)  << 32) & MASKFF00) | (__int64)indexX;
	return  RB::Value(index);
}
template <class T, class LIST_ALLOC>
void MDim2d<T, LIST_ALLOC>::InsertColumn(int indexX)
{
	if (!MArray<int>::Len) return;
	if (indexX > pMax[1])
	{
		++ptr[1];
		return;
	}
	if (RB::Root() == RB::Nil()) return;
	RBTreeNode<T, __int64> *node = RB::pRoot;
	RBTreeNode<T, AA> *mp;
	int len = 16;
	MArray<RBTreeNode<T, __int64> *> stack(16);
	RBTreeNode<T, __int64> **test, **pStack = stack.GetPtr();
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
				node = node->right;
			}
			while(node != RB::Nil());
		}
		node = *(--pStack);
		mp = (RBTreeNode<T, AA> *)node;
		if (mp->key.x >= indexX)
			++mp->key.x;
		if((fCheck = node->left != RB::Nil()))
			node = node->left;
	}
	while (fCheck || pStack > test);


	++ptr[1];
	if (indexX <= pMax[1]) ++pMax[1];
}

template <class T, class LIST_ALLOC>
void MDim2d<T, LIST_ALLOC>::DeleteColumn(int indexX)
{
	if (!MArray<int>::Len) return;
	if (indexX > pMax[1])
	{
		if (indexX < ptr[1])
			--ptr[1];
		return;
	}
 	__int64 index, indexY;
	
	RBTreeNode<T, __int64> *node;
	for (indexY = pMin[0]; indexY <= pMax[0]; ++indexY)
	{
		index	= ((indexY  << 32) & MASKFF00) | (__int64)indexX;
		node = RB::pRoot;
		while (node != RB::Nil() && node->key != index)
				node = node->key > index ? node->left : node->right;
		if (node != RB::Nil()) RB::DeleteNode(node);
	}

	if (RB::Root() == RB::Nil()) return;
	node = RB::pRoot;
	RBTreeNode<T, AA> *mp;
	int len = 16;
	MArray<RBTreeNode<T, __int64> *> stack(16);
	RBTreeNode<T, __int64> **test, **pStack = stack.GetPtr();
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
				node = node->right;
			}
			while(node != RB::Nil());
		}
		node = *(--pStack);
		mp = (RBTreeNode<T, AA> *)node;
		if (mp->key.x > indexX)
			--mp->key.x;
		if((fCheck = node->left != RB::Nil()))
			node = node->left;
	}
	while (fCheck || pStack > test);


	--ptr[1];
	if (ptr[1] < pMax[1]) --pMax[1];
}
template <class T, class LIST_ALLOC>
void MDim2d<T, LIST_ALLOC>::DeleteRow(int indexY) 
{ 
	if (!MArray<int>::Len) return;
	if (indexY > pMax[0])
	{
		if (indexY < ptr[0])
			--ptr[0];
		return;
	}
	if (indexY < 0) indexY = 0;
	__int64 index;
	RBTreeNode<T, __int64> *node;
	for (int indexX = pMin[1]; indexX <= pMax[1]; ++indexX)
	{
		
		index	= ((((__int64)indexY)  << 32) & MASKFF00) | (__int64)indexX;
		node = RB::pRoot;
		while (node != RB::Nil() && node->key != index)
				node = node->key > index ? node->left : node->right;
		if (node != RB::Nil()) RB::DeleteNode(node);
	}
	if (RB::Root() == RB::Nil()) return;
	node = RB::pRoot;
	RBTreeNode<T, AA> *mp;
	int len = 16;
	MArray<RBTreeNode<T, __int64> *> stack(16);
	RBTreeNode<T, __int64> **test, **pStack = stack.GetPtr();
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
				node = node->right;
			}
			while(node != RB::Nil());
		}
		node = *(--pStack);
		mp = (RBTreeNode<T, AA> *)node;
		if (mp->key.y > indexY)
			--mp->key.y;
		else
			break;  
		if((fCheck = node->left != RB::Nil()))
			node = node->left;
	}
	while (fCheck || pStack > test);
	--ptr[0];
	if (ptr[0] < pMax[0]) --pMax[0];
	

}
template <class T, class LIST_ALLOC>
void MDim2d<T, LIST_ALLOC>::InsertRow(int indexY) 
{ 
	if (!MArray<int>::Len) return;
	if (indexY > pMax[0])
	{
		++ptr[0];
		return;
	}
	if (indexY < 0) indexY = 0;
	if (RB::Root() == RB::Nil()) return;
	RBTreeNode<T, __int64> *node = RB::pRoot;
	RBTreeNode<T, AA> *mp;
	int len = 16;
	MArray<RBTreeNode<T, __int64> *> stack(16);
	RBTreeNode<T, __int64> **test, **pStack = stack.GetPtr();
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
				node = node->right;
			}
			while(node != RB::Nil());
		}
		node = *(--pStack);
		mp = (RBTreeNode<T, AA> *)node;
		if (mp->key.y >= indexY)
			++mp->key.y;
		else
			break;  
		if((fCheck = node->left != RB::Nil()))
			node = node->left;
	}
	while (fCheck || pStack > test);
	++ptr[0];
	if (indexY <= pMax[0]) ++pMax[0];
	

}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class T, class LIST_ALLOC>
inline void MDim2d<T, LIST_ALLOC>::Delete(int indexY, int indexX) 
{ 
	if (!MArray<int>::Len ||
		indexY < pMin[0] || indexY > pMax[0] || indexX < pMin[1] || indexX > pMax[1])
		return;
	__int64 index	= ((((__int64)indexY)  << 32) & MASKFF00) | (__int64)indexX;
	RBTreeNode<T, __int64> *node = RB::pRoot;
	while (node != RB::Nil() && node->key != index)
			node = node->key > index ? node->left : node->right;
	if (node != RB::Nil()) RB::DeleteNode(node);
}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
template <class T, class LIST_ALLOC>
inline T *MDim2d<T, LIST_ALLOC>::operator () (int indexY, int indexX) 
{ 
	if (!MArray<int>::Len || indexY < pMin[0] || indexY > pMax[0] || indexX < pMin[1] || indexX > pMax[1]) return 0;
	__int64 index	= ((((__int64)indexY)  << 32) & MASKFF00) | (__int64)indexX;
	RBTreeNode<T, __int64> *node = RB::pRoot;
	while (node != RB::Nil() && node->key != index)
			node = node->key > index ? node->left : node->right;
	return node != RB::Nil() ? &node->value : 0;
}


class MBool2d : public _RBSetSimple(__int64) , protected MArray<int>  {
	union AA{
		struct {
			int  x;
			int  y;
		};
		__int64 i; 
	};

public:
	MBool2d() { pMin = pMax = 0; }
	~MBool2d() {  Clear(); }
	MBool2d(MBool2d &d) : _RBSetSimple(__int64)(d), MArray<int>(d) { pMin = pMax = 0;  Init(); }
	BOOL SetDim(ULONG _yDim, ULONG _xDim); 
	void Clear()
	 { 
		if (MArray<int>::Len)
		{
			pMin[1] = ptr[1];
			pMax[0] = 0;
			pMax[1] = 0;
			pMin[0] = ptr[0];
			_RBSetSimple(__int64)::Clear(); 
		}
	}
	bool IsEmpty() { return MArray<int>::Len == 0 || _RBSetSimple(__int64)::m_iNum == 0; }

	void Set(int indexY, int indexX, BOOL data);
	void SetRow(int indexY, BOOL data);
	void SetColumn(int indexX, BOOL data);
	BOOL operator () (int indexY, int indexX);
	int DimN(int i) { return i < 2 ? ptr[i] : 0; }
	int MaxN(int i) { return i < 2 ? pMax[i] : 0; }
	int MinN(int i) { return i < 2 ? pMin[i] : 0; }
	void InsertRow(int indexY);
	void InsertColumn(int indexX);

	void DeleteRow(int indexY);
	void DeleteColumn(int indexX);
protected:
	void Init();
protected:
	int *pMax;
	int *pMin;
public:


};

//-----------------------------------------------------------
//
//-----------------------------------------------------------

inline BOOL MBool2d::SetDim(ULONG _yDim, ULONG _xDim) 
{ 
	if (MArray<int>::Len != 6)
	{
		MArray<int>::SetLen(6);
		pMin = pMax = 0;

		if (!ptr) return 0;
		pMin = ptr + 2;
		pMax = ptr + 4;

		memset(ptr, 0, sizeof(int) * 6);
		ptr[0] = _yDim;
		ptr[1] = _xDim;
		pMin[0] = _yDim;
		pMin[1] = _xDim;

		_RBSetSimple(__int64)::Clear(); 
	}
	else
	{
		ptr[0] = _yDim;
		ptr[1] = _xDim;
	}
	return true; 
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
inline void MBool2d::SetRow(int indexY, BOOL data)
{
	if (!MArray<int>::Len || !ptr || indexY < 0 || indexY >= ptr[0])
		return;
	pMax[0] = max(pMax[0], indexY);
	pMin[0] = min(pMin[0], indexY);
	pMax[1] = ptr[1] - 1;
	pMin[1] = 0;
	for (__int64 i = 0; i < ptr[1]; ++i)
	if (data)
		_RBSetSimple(__int64)::Add(((((__int64)indexY)  << 32) & MASKFF00) | i);
	else
		_RBSetSimple(__int64)::Delete(((((__int64)indexY)  << 32) & MASKFF00) | i);
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
inline void MBool2d::SetColumn(int indexX, BOOL data)
{
	if (!MArray<int>::Len || !*ptr || !ptr[1]) return; 
	if (data)
	{
		pMax[1] = max(pMax[1], indexX);
		pMin[1] = min(pMin[1], indexX);
		pMax[0] = ptr[0] - 1;
		pMin[0] = 0;
		for (__int64 i = 0; i < ptr[0]; ++i)
			_RBSetSimple(__int64)::Add(((i << 32) & MASKFF00) | indexX);
	}
	else
		for (__int64 i = pMin[0]; i <= pMax[0]; ++i)
			_RBSetSimple(__int64)::Delete(((i << 32) & MASKFF00) | indexX);

}


//-----------------------------------------------------------
//
//-----------------------------------------------------------
inline void MBool2d::Set(int indexY, int indexX, BOOL data) 
{ 
	if (!MArray<int>::Len || indexY < 0 || indexY >= *ptr || indexX < 0 || indexX >= ptr[1])
	{ 
		
		//BoolWarning(0);
		return;
	}
	pMin[0] = min(pMin[0], indexY);
	pMax[0] = max(pMax[0], indexY);
	pMin[1] = min(pMin[1], indexX);
	pMax[1] = max(pMax[1], indexX);

	if (data)
		_RBSetSimple(__int64)::Add(((((__int64)indexY)  << 32) & MASKFF00) | (__int64)indexX);
	else
		_RBSetSimple(__int64)::Delete(((((__int64)indexY)  << 32) & MASKFF00) | (__int64)indexX);

}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
inline void MBool2d::DeleteRow(int indexY) 
{ 
	if (!MArray<int>::Len) return;
	if (indexY > pMax[0])
	{
		if (indexY < ptr[0])
			--ptr[0];
		return;
	}
	if (indexY < 0) indexY = 0;
	RBKeyNode<__int64> *node;
	for (__int64 index, indexX = pMin[1]; indexX <= pMax[1]; ++indexX)
	{
		
		index	= ((((__int64)indexY)  << 32) & MASKFF00) | (__int64)indexX;
		node = pRoot;
		while (node != &m_Nil && node->key != index)
				node = node->key > index ? node->left : node->right;
		if (node != &m_Nil) _RBSetSimple(__int64)::DeleteNode(node);
	}
	if (Root() == Nil()) return;	
	node = pRoot;
	RBKeyNode<AA> *mp;
	int len = 16;
	MArray<RBKeyNode<__int64> *> stack(16);
	RBKeyNode<__int64> **test, **pStack = stack.GetPtr();
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
				node = node->right;
			}
			while(node != Nil());
		}
		node = *(--pStack);
		mp = (RBKeyNode< AA> *)node;
		if (mp->key.y > indexY)
			--mp->key.y;
		else
			break;  
		if((fCheck = node->left != Nil()))
			node = node->left;
	}
	while (fCheck || pStack > test);
	--ptr[0];
	if (ptr[0] < pMax[0]) --pMax[0];
	

}
//-----------------------------------------------------------
inline void MBool2d::Init()
{
	pMin = ptr + 2;
	pMax = ptr + 4;

}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
inline void MBool2d::InsertRow(int indexY) 
{ 
	if (!MArray<int>::Len) return;
	if (indexY > pMax[0])
	{
		++ptr[0];
		return;
	}
	if (indexY < 0) indexY = 0;
	if (Root() == Nil()) return;	
	RBKeyNode<__int64> *node = pRoot;
	RBKeyNode<AA> *mp;
	int len = 16;
	MArray<RBKeyNode<__int64> *> stack(16);
	RBKeyNode<__int64> **test, **pStack = stack.GetPtr();
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
				node = node->right;
			}
			while(node != Nil());
		}
		node = *(--pStack);
		mp = (RBKeyNode< AA> *)node;
		if (mp->key.y >= indexY)
			++mp->key.y;
		else
			break;  
		if((fCheck = node->left != Nil()))
			node = node->left;
	}
	while (fCheck || pStack > test);
	++ptr[0];
	if (indexY <= pMax[0]) ++pMax[0];
	

}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
inline void MBool2d::DeleteColumn(int indexX) 
{ 
	if (!MArray<int>::Len) return;
	if (indexX > pMax[1])
	{
		if (indexX < ptr[1])
			--ptr[1];
		return;
	}
	if (indexX < 0) indexX = 0;
 	__int64 index, indexY;
	
	RBKeyNode<__int64> *node;
	for (indexY = pMin[0]; indexY <= pMax[0]; ++indexY)
	{
		index	= ((indexY  << 32) & MASKFF00) | (__int64)indexX;
		node = pRoot;
		while (node != &m_Nil && node->key != index)
				node = node->key > index ? node->left : node->right;
		if (node != &m_Nil) _RBSetSimple(__int64)::DeleteNode(node);
	}
	if (Root() == Nil()) return;	
	node = pRoot;
	RBKeyNode<AA> *mp;
	int len = 16;
	MArray<RBKeyNode<__int64> *> stack(16);
	RBKeyNode<__int64> **test, **pStack = stack.GetPtr();
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
				node = node->right;
			}
			while(node != Nil());
		}
		node = *(--pStack);
		mp = (RBKeyNode< AA> *)node;
		if (mp->key.x > indexX)
			--mp->key.x;
		if((fCheck = node->left != Nil()))
			node = node->left;
	}
	while (fCheck || pStack > test);
	--ptr[1];
	if (ptr[1] < pMax[1]) --pMax[1];
	

}
//-----------------------------------------------------------
//
//-----------------------------------------------------------
inline void MBool2d::InsertColumn(int indexX) 
{ 
	if (!MArray<int>::Len) return;
	if (indexX > pMax[1])
	{
		++ptr[1];
		return;
	}
	if (indexX < 0) indexX = 0;
	if (Root() == Nil()) return;	
	RBKeyNode<__int64> *node = pRoot;
	RBKeyNode<AA> *mp;
	int len = 16;
	MArray<RBKeyNode<__int64> *> stack(16);
	RBKeyNode<__int64> **test, **pStack = stack.GetPtr();
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
				node = node->right;
			}
			while(node != Nil());
		}
		node = *(--pStack);
		mp = (RBKeyNode< AA> *)node;
		if (mp->key.x >= indexX)
			++mp->key.x;
		if((fCheck = node->left != Nil()))
			node = node->left;
	}
	while (fCheck || pStack > test);
	++ptr[1];
	if (indexX <= pMax[1]) ++pMax[1];
	

}

//-----------------------------------------------------------
//
//-----------------------------------------------------------

inline BOOL MBool2d::operator () (int indexY, int indexX) 
{ 
	if (!MArray<int>::Len ||
		indexY < pMin[0] || indexY > pMax[0] || indexX < pMin[1] || indexX > pMax[1]) return 0;

	RBKeyNode<__int64> *node = pRoot;
	__int64 index = ((((__int64)indexY)  << 32) & MASKFF00) | (__int64)indexX; 
	while (node != &m_Nil && node->key != index)
			node = node->key > index ? node->left : node->right;
	return node != &m_Nil;
}


#endif
