/*
 * base_error.h
 *
 *  Created on: Mar 24, 2012
 *      Author: alex
 */
#ifndef BOOL_WARNING_H_
#define BOOL_WARNING_H_

#include "std.h"

_FEXPORT32 int __BoolWarning(int err, const char *str, const char *pFile, int line);

#ifdef BoolWarning
#undef BoolWarning
#endif
#if(0) //def NDEBUG
#define BoolWarning(a) (a)
#else
#define BoolWarning(a) (__BoolWarning(((a) != 0), 0, __FILE__ , __LINE__))
#endif
#ifdef TRACE
#undef TRACE
#endif

#ifndef ANDROID_NDK

#ifdef NODEBUG
#define TRACE
#else
#define TRACE printf
#endif
#else

#include "Loger.h"

#endif
#endif
