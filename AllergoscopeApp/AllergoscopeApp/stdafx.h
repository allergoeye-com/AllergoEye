// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define NOPLUK
#define EXPORTNIX
#define _PEXPORT
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "targetver.h"

#include <afxsock.h>            // MFC socket extensions

#define EXPORTNIX
#define EXPMDC
#define _PEXPORT
#define _FEXPORT32
#include <afxwin.h>         // MFC core and standard components
//#include <windows.h>         // MFC core and standard components
#include <afxext.h>   
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "mstring.h"
#include <afxsock.h>
#include <afxwin.h>

#ifndef UNICODE
#define mstrcmpi strcmpi
#define mstrcmp  strcmp
#define mstrrchr strrchr
#define mstrcpy	 strcpy
#define mstrlen	 strlen 
#define mstrcat	 strcat
#define mstrtok  strtok
#define mstrrchr strrchr
#define mstrchr	 strchr

#else
#define mstrcmpi _wcsicmp
#define mstrcmp  wcscmp
#define mstrrchr wcsrchr
#define mstrcpy	 wcscpy
#define mstrlen	 wcslen 
#define mstrcat	 wcscat
#define mstrtok  wcstok
#define mstrrchr wcsrchr
#define mstrchr	 wcschr
#endif
#include "mstring.h"
#include "mvect.h"
#include "POINT_RECT.h"

#include "NodeTreeIt.h"

inline LPVOID HMALLOC(size_t dwBytes)
{
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytes);
}
inline LPVOID HMALLOCZ(size_t dwBytes)
{
	int* _Buffer = (int*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytes);

	if (!_Buffer)
		return 0;
	memset(_Buffer, 0, dwBytes);
	return _Buffer;
}
inline LPVOID HCALLOC(size_t num, size_t size)
{
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size * num);
}

inline LPVOID HREALLOC(void* lpMem, size_t dwBytes)
{
	return HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, lpMem, dwBytes);
}
inline void HFREE(void* lpMem)
{
	HeapFree(GetProcessHeap(), 0, lpMem);
}


// TODO: reference additional headers your program requires here
