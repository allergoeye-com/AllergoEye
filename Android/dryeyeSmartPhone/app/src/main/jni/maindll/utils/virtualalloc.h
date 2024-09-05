#ifndef __TVirtualAlloc_H
#define    __TVirtualAlloc_H

#include "mpthread.h"
#include "mfile.h"

#ifdef __PLUK_LINUX__
#define PAGE_READWRITE (PROT_READ | PROT_WRITE)
#define PAGE_EXECUTE_READWRITE  (PROT_READ | PROT_WRITE | PROT_EXEC)
#endif
#pragma pack(push) //NADA
#pragma pack(4)

class _PEXPORT TVirtualAlloc {
public:
    TVirtualAlloc()
    {}

    virtual ~TVirtualAlloc()
    {}

    void *Alloc(void *StartPtr, size_t &size);

    void *Alloc(void *StartPtr, INT64 &size)
    {
        size_t _size = (size_t) size;
        void *p = Alloc(StartPtr, _size);
        size = _size;
        return p;
    }

    void *Alloc(void *StartPtr, INT32 &size)
    {
        size_t _size = (size_t) size;
        void *p = Alloc(StartPtr, _size);
        size = (INT32) _size;
        return p;
    }

    bool Free(void *p, size_t sz);

    void *Commit(void *Memory, size_t size, int flag);

    bool Decommit(void *ptr, size_t size);
};

class _PEXPORT MVirtualAlloc : protected TVirtualAlloc {
public:
    MVirtualAlloc();

    virtual ~MVirtualAlloc()
    {
        Free();
    }

    void *Alloc(void *StartPtr, size_t size);

    off_t Size()
    { return Len; }

    bool Free();

    void *Commit(void *Memory, size_t size, int flag);

    bool Decommit(void *ptr, size_t size);

    void *Base()
    { return pBase; }

    void Detach()
    {
        pBase = 0;
        Len = 0;
    }

    void Attach(void *p, off_t l)
    {
        pBase = p;
        Len = l;
    }

protected:
    DWORD dwPageSize;
    off_t Len;
    void *pBase;
};

#pragma pack(pop) //NADA

#endif
