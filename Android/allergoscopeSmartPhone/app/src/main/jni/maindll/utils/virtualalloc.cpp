#include "stdafx.h"
#include "virtualalloc.h"

//---------------------------------------------
//
//---------------------------------------------
void *TVirtualAlloc::Alloc(void *StartPtr, size_t &_size)
{
#ifdef __PLUK_LINUX__
    size_t dwPageSize = sysconf(_SC_PAGESIZE);
    size_t size = (_size / dwPageSize + (_size % dwPageSize != 0)) * dwPageSize;
    void *pBase = mmap(StartPtr, size, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (pBase == (void *) -1)
    {
        StdError(-1);
        pBase = 0;
    } else
        _size = size;

#else
    void *pBase = VirtualAlloc(StartPtr, _size, MEM_RESERVE, PAGE_NOACCESS);
#endif
    return pBase;

}

//---------------------------------------------
//
//---------------------------------------------
void *TVirtualAlloc::Commit(void *Memory, size_t size, int flag)
{
    void *ptr = 0;
#ifdef __PLUK_LINUX__
    ptr = Memory;
    if (StdError(mprotect(ptr, size, flag)))
        // StdError(mlock(ptr, size));
        //else
        ptr = 0;
#else
    ptr = VirtualAlloc(Memory, size, MEM_COMMIT, flag);
#endif
    return ptr;
}

//---------------------------------------------
//
//---------------------------------------------
bool TVirtualAlloc::Decommit(void *ptr, size_t size)
{
#ifdef __PLUK_LINUX__
    StdError(msync(ptr, size, MS_SYNC));
    return StdError(mprotect(ptr, size, PROT_NONE)) != -1;
#else
    return VirtualFree(ptr, size, MEM_DECOMMIT ) != 0;
#endif
}

//---------------------------------------------
//
//---------------------------------------------
bool TVirtualAlloc::Free(void *pBase, size_t Len)
{
    if (!pBase) return 0;
#ifdef __PLUK_LINUX__
    return StdError(munmap(pBase, Len)) != -1;
#else
    VirtualFree(pBase, 0, MEM_RELEASE);
#endif
    pBase = 0;
    Len = 0;
    return 1;
}

//---------------------------------------------
//
//---------------------------------------------
MVirtualAlloc::MVirtualAlloc()
{
    Len = 0;
    pBase = 0;
#ifdef __PLUK_LINUX__
    dwPageSize = sysconf(_SC_PAGESIZE);
#else
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    dwPageSize = si.dwPageSize;
#endif

}

//---------------------------------------------
//
//---------------------------------------------
void *MVirtualAlloc::Alloc(void *StartPtr, size_t size)
{
    Free();
    size = (size / dwPageSize + (size % dwPageSize != 0)) * dwPageSize;

    pBase = TVirtualAlloc::Alloc(StartPtr, size);
    Len = size;
    return pBase;

}

//---------------------------------------------
//
//---------------------------------------------
void *MVirtualAlloc::Commit(void *Memory, size_t size, int flag)
{
    void *ptr = 0;
    if (pBase)
    {
        ptr = TVirtualAlloc::Commit(Memory, size, flag);
    }
    return ptr;
}

//---------------------------------------------
//
//---------------------------------------------
bool MVirtualAlloc::Decommit(void *ptr, size_t size)
{
    if (!pBase) return 0;
    return TVirtualAlloc::Decommit(ptr, size);
}

//---------------------------------------------
//
//---------------------------------------------
bool MVirtualAlloc::Free()
{

    if (!pBase) return 0;
    TVirtualAlloc::Free(pBase, Len);
    pBase = 0;
    Len = 0;
    return 1;
}
