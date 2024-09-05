#ifndef __MapMemFile_H
#define    __MapMemFile_H

#include "mpthread.h"
#include "mfile.h"
#include "CSemaphore.h"

#pragma pack(push) //NADA
#pragma pack(4)
EXPORTNIX class _PEXPORT MapMemFile : protected MFILE {
public:
    MapMemFile();

    virtual ~MapMemFile()
    {
    }

    bool Open(const char *FileName);

    bool CreateMapView(int size);

    bool CloseMapView();

    virtual off_t GetSize()
    { return MFILE::GetSize(); }

    virtual bool Close();

    void *Map(size_t offset, size_t size);

    bool UnMap(void *_ptr, size_t size);

    virtual BYTE *Detach()
    {
        hFileMapping = 0;
        MFILE::Detach();
        return 0;
    }

    MAPHANDLE Handle()
    { return hFileMapping; }

    virtual const char *Name();

protected:
    void SetName(const char *FileName);

protected:
    DWORD dwPageSize;
    MAPHANDLE hFileMapping;
    size_t Len;
    int SizeMap;
};
EXPORTNIX class _PEXPORT ShmMemView {
public:
    ShmMemView();

    ~ShmMemView();

    bool Open(LPCSTR, int);

    int Close();

    int Destroy();

#ifdef __PLUK_LINUX__

    void Sync()
    { if (hFile > 0) fsync(hFile); }

#endif
    char ShareName[128];
    int Size;
    MAPHANDLE hFileMapping;
    BYTE *FileBase;
    FILEHANDLE hFile;
    Semaphore sem;
};
#pragma pack(pop) //NADA

#endif


