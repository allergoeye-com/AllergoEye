///////////////////////////////////////////////////
// mapstr.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////
#ifndef _M_MAP_STRING__
#define _M_MAP_STRING__

#include "hashmap.h"

template <class MAP_VALUE, class MAP_KEY = MString, class CMP = MMapCmp, class LIST_ALLOC = AllocatorSimple<RBTreeNode<MAP_VALUE, MAP_KEY> > >
class MMap : public RBHashMap<MAP_VALUE,MAP_KEY, CMP, LIST_ALLOC> {
public:
	MMap (int size = 32):RBHashMap<MAP_VALUE,MAP_KEY, CMP, LIST_ALLOC>(size) { RBHashMap<MAP_VALUE,MAP_KEY, CMP, LIST_ALLOC>::KeyFromCompareFunc(true); };
	MMap(MMap &a) { *this = a; } 
	~MMap() { }
	MMap &operator = (MMap &a) { RBHashMap<MAP_VALUE,MAP_KEY, CMP, LIST_ALLOC>::operator = (a); return *this; } 
};
#endif
