///////////////////////////////////////////////////
// mtree.h
// PALM Library for MotionTracking by Alexander L. Kalaidzidis
// Copyright (c) 2009
////////////////////////////////////////////////////
#ifndef __MRB_HEAP__AND_MAP_
#define __MRB_HEAP__AND_MAP_
#ifndef __PLUK_LINUX__
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#pragma warning(disable:4530)
#endif

#include "mstring.h"
#include "mallocator.h"
#include "mlist.h"
#include "rbmap.h"
#include "mapstr.h"
#include "mconstmap.h"
//#include "MHeap.h"
#include "mset.h"
#include "mconstset.h"
#include "dwordmap.h"

#define _RBMapSimple(A, B) RBMap<A, B, RBCmp<B>,  AllocatorSimple<RBTreeNode<A, B> > >
#define _RBSetSimple(A) RBSet<A, RBCmp<A>,  AllocatorSimple<RBKeyNode<A> > >

#include "mdim.h"

#endif
