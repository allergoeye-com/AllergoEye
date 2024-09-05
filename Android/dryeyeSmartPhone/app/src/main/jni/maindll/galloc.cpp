#include "stdafx.h"
#include "buffer.h"
#include <malloc.h>
#include <jni.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
//#define DEBUG_MEM



EXPORTNIX size_t HeapGetSize()
{
    struct mallinfo info = mallinfo();
    return info.usmblks;
}

EXPORTNIX size_t HeapAllocatedSize()
{
    struct mallinfo info = mallinfo();
    return info.uordblks;
}

EXPORTNIX size_t HeapFreeSize()
{
    struct mallinfo info = mallinfo();
    return info.fordblks;
}
#ifdef DEBUG_MEM
EXPORTNIX size_t GlobalSize (void *m_hMem)
{
    if (!m_hMem) return 0;
    size_t *ptr1 = (size_t *)m_hMem;
    size_t *size = ptr1 - 1;
    size_t *label = ptr1 - 2;
    size_t N = *size/sizeof(size_t) + (*size%sizeof(size_t) != 0);
    if (*label != M_MEMLABEL || *size == (size_t)-1 || ptr1[N] != M_MEMLABEL)
	{
            ASSERT(0);
			return 0;
	}
    return *size;
}
EXPORTNIX bool GlobalIsValid (HGLOBAL m_hMems)
{
    if (m_hMems && HIWORD((DWORD)(size_t)m_hMems))
    {
        size_t *ptr1 = (size_t *)m_hMems;
        size_t *size = ptr1 - 1;
        size_t *label = ptr1 - 2;
        size_t N = *size/sizeof(size_t) + (*size%sizeof(size_t) != 0);
        if (*label == M_MEMLABEL && *size != (size_t)-1 && ptr1[N] == M_MEMLABEL)
        {
                return 1;
        }
        else
             ASSERT(0);
    }
    return 0;
}

EXPORTNIX bool GlobalFree (HGLOBAL m_hMems)
{
    if (m_hMems)
    {
        size_t *ptr1 = (size_t *)m_hMems;
        size_t *size = ptr1 - 1;
        size_t *label = ptr1 - 2;
	size_t N = *size/sizeof(size_t) + (*size%sizeof(size_t) != 0);
        if (*label == M_MEMLABEL && *size != (size_t)-1 && ptr1[N] == M_MEMLABEL)
        {
                *size = (size_t)-1;
                free(label);
                return 1;
        }
        else
             ASSERT(0);
    }
    return 0;
}
EXPORTNIX HGLOBAL GlobalReAlloc (HGLOBAL m_hMem, size_t dwBytes, int dwFlags)
{
	if (!m_hMem) return 0;
    size_t *ptr1 = (size_t *)m_hMem;
    size_t *size = ptr1 - 1;
    size_t *label = ptr1 - 2;
	size_t N = *size/sizeof(size_t) + (*size%sizeof(size_t) != 0);
	if (*label != M_MEMLABEL || *size == (size_t)-1 || ptr1[N] != M_MEMLABEL)
     {
            ASSERT(0);
            return 0;
     }
    size_t oldsize = *size;
    N = dwBytes/sizeof(size_t) + (dwBytes%sizeof(size_t) != 0);

#ifndef ANDROID_NDK
    size_t real_size = malloc_usable_size(label)/sizeof(size_t) - 3;
    if (N <  real_size)
        ptr1 = label;
    else
#endif
    if (!(ptr1 = (size_t *)realloc(label, (N + 3) * sizeof(size_t))))
     {
            ASSERT(0);
            return 0;
     }
	ptr1[0] = M_MEMLABEL;
	ptr1[1] = dwBytes;
	ptr1 += 2;
	ptr1[N] = M_MEMLABEL;
    char *ptr = (char *)ptr1;
    if ((dwFlags & GMEM_ZEROINIT) && oldsize < (size_t)dwBytes) 
        bzero(ptr + oldsize, dwBytes - oldsize);
    return (HGLOBAL)ptr;

}
EXPORTNIX HGLOBAL GlobalAlloc (UINT dwFlags, size_t dwBytes)
{
    void *ptr = 0;
    int N = dwBytes/sizeof(size_t) + (dwBytes%sizeof(size_t) != 0);
    size_t *ptr1 = (size_t *)malloc((N + 3) * sizeof(size_t));
    if (ptr1)
    {
            ptr1[0] = M_MEMLABEL;
            ptr1[1] = dwBytes;
            ptr1 += 2;
            ptr1[N] = M_MEMLABEL;
            ptr = (void *)ptr1;
        if (dwFlags && GMEM_ZEROINIT) bzero(ptr, dwBytes);
    }
    else
   {
            ASSERT(0);
            return 0;
     }
    return (HGLOBAL)ptr;

}
#else

EXPORTNIX size_t GlobalSize (void *m_hMem)
{
    
    if (!m_hMem) return 0;
    size_t *ptr1 = (size_t *)m_hMem;
    size_t *size = ptr1 - 1;
    if (*size == (size_t)-1)
	{
            ASSERT(0);
			return 0;
	}
    return *size;
}
EXPORTNIX bool GlobalIsValid (HGLOBAL m_hMems)
{
    if (m_hMems && HIWORD((DWORD)(size_t)m_hMems))
    {
        size_t *ptr1 = (size_t *)m_hMems;
        size_t *size = ptr1 - 1;
        if (*size != (size_t)-1)
        {
                return 1;
        }
        else
             ASSERT(0);
    }
    return 0;
}

EXPORTNIX bool GlobalFree (HGLOBAL m_hMems)
{
    try {
    if (m_hMems)
    {
        size_t *ptr1 = (size_t *)m_hMems;
        size_t *size = ptr1 - 1;
        if (*size != (size_t)-1)
        {
                *size = (size_t)-1;
                free(size);
                return 1;
        }
        else
             ASSERT(0);
    }
    }
    catch (...)
    {
		JNIEnv *env = MInstance::GetLocalJNI();
		MInstance::ClearException(env);
    	return 0;
    }
    return 0;
}
EXPORTNIX HGLOBAL GlobalReAlloc (HGLOBAL m_hMem, size_t dwBytes, int dwFlags)
{
	if (!m_hMem) return 0;
    size_t *ptr1 = (size_t *)m_hMem;
    size_t *size = ptr1 - 1;
	if (*size == (size_t)-1)
     {
            ASSERT(0);
            return 0;
     }
    size_t oldsize = *size;
    size_t N = dwBytes/sizeof(size_t) + (dwBytes%sizeof(size_t) != 0);
#ifndef ANDROID_NDK
    size_t real_size = malloc_usable_size(size)/sizeof(size_t) - 1;
    if (N <  real_size)
        ptr1 = size;
    else
#endif
    if (!(ptr1 = (size_t *)realloc(size, (N + 1) * sizeof(size_t)))) 
     {
            ASSERT(0);
            return 0;
     }
	ptr1[0] = dwBytes;
	++ptr1;
    char *ptr = (char *)ptr1;
    if ((dwFlags & GMEM_ZEROINIT) && oldsize < (size_t)dwBytes) 
    {
        bzero(ptr + oldsize, dwBytes - oldsize);
    }
    return (HGLOBAL)ptr;

}
EXPORTNIX HGLOBAL GlobalAlloc (UINT dwFlags, size_t dwBytes)
{
    void *ptr = 0;
    size_t N = dwBytes/sizeof(size_t) + (dwBytes%sizeof(size_t) != 0);
    size_t *ptr1 = (size_t *)malloc((N + 1) * sizeof(size_t));
    if (ptr1)
    {
            ptr1[0] = dwBytes;
#ifdef ANDROID_NDK
            size_t us = (N + 1) * sizeof(size_t);
            #else
            size_t us = malloc_usable_size(ptr1);
#endif
            ++ptr1;
            ptr = (void *)ptr1;
        if (dwFlags && GMEM_ZEROINIT) bzero(ptr, us - sizeof(size_t));
    }
    else
   {
            ASSERT(0);
            return 0;
     }
    return (HGLOBAL)ptr;

}
#endif
extern "C" LPVOID HMALLOC(size_t dwBytes)
{
        return HeapAlloc (GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytes);
}
extern "C" LPVOID HCALLOC(size_t num, size_t size)
{
        return HeapAlloc (GetProcessHeap(), HEAP_ZERO_MEMORY, size * num);
}

extern "C" LPVOID HREALLOC(void *lpMem, size_t dwBytes)
{
        return HeapReAlloc (GetProcessHeap(), HEAP_ZERO_MEMORY, lpMem, dwBytes);
}
extern "C" void HFREE(void *lpMem)
{
        HeapFree(GetProcessHeap(), 0, lpMem);
}

