
#include "stdafx.h"
#include "mapmemfile.h"

#ifndef ANDROID_NDK

#ifdef __PLUK_LINUX__
#include "error.h"
#endif
#else

#include <linux/ashmem.h>

#define ASHMEM_DEVICE    "/dev/ashmem"
#endif

//-------------------------------------------
//
//-------------------------------------------
MapMemFile::MapMemFile()
{
    Len = 0;
    SizeMap = 0;
    hFileMapping = 0;
#ifdef __PLUK_LINUX__
    dwPageSize = sysconf(_SC_PAGESIZE);
#else
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    dwPageSize = si.dwPageSize;
#endif
}

//-------------------------------------------
//
//-------------------------------------------
void MapMemFile::SetName(const char *FileName)
{
    if (FileName)
    {
        if (name) HFREE(name);
        name = (char *) HMALLOC(strlen(FileName) + 1);
        strcpy(name, FileName);
    }
}

const char *MapMemFile::Name()
{
    return name;
}

//-------------------------------------------
//
//-------------------------------------------
bool MapMemFile::Open(const char *FileName)
{
    Close();
    SetName(FileName);
#ifdef __PLUK_LINUX__
    return MFILE::Open(!name || ACCESS(name, F_OK) != 0);
#else
    return MFILE::Open(!name || ACCESS(name, 6)!= 0);
#endif
}

//-------------------------------------------
//
//-------------------------------------------
bool MapMemFile::CreateMapView(int size)
{
    Len = max((int) GetSize(), size);
    fShare = 0;
    size = (size / (int) dwPageSize + (size < (int) dwPageSize || size % (int) dwPageSize != 0)) *
           (int) dwPageSize;
    if (hFile != INVALID_HANDLE_VALUE)
    {
        if (GetSize() < size)
        {
            Len = GetSize();
            SetSize(size);
        }
        SizeMap = size;
#ifdef __PLUK_LINUX__
        hFileMapping = (char *) mmap(0, size, PROT_NONE, MAP_SHARED | MAP_NORESERVE, hFile, 0);
        if (hFileMapping == (void *) -1)
        {
            StdError(-1);
            hFileMapping = 0;
        }
#else
        SQWORD s;
        s.x = Len;
        hFileMapping = CreateFileMapping(hFile, 0, PAGE_READWRITE | SEC_COMMIT | SEC_NOCACHE, s.HDW, s.LDW, 0);
#endif
    }
    return hFileMapping != 0;
}

//-------------------------------------------
//
//-------------------------------------------
bool MapMemFile::CloseMapView()
{
    if (hFileMapping)
    {
#ifdef __PLUK_LINUX__
        StdError(munmap(hFileMapping, SizeMap));
#else
        CloseHandle(hFileMapping);
#endif
        hFileMapping = 0;
    }
    if (hFile != INVALID_HANDLE_VALUE)
        SetSize(Len);
    return true;
}

//-------------------------------------------
//
//-------------------------------------------
bool MapMemFile::Close()
{
    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseMapView();
        MFILE::Close();
    }
    return true;
}

//-------------------------------------------
//
//-------------------------------------------
void *MapMemFile::Map(size_t offset, size_t size)
{
    if (hFileMapping)
    {
#ifdef __PLUK_LINUX__
        if (offset == 0 && size == 0)
            size = MFILE::GetSize();
        size_t pos = offset / dwPageSize * dwPageSize;
        size_t _off = offset - pos;
        size_t sz =
                ((offset + size) / dwPageSize + ((offset + size) % dwPageSize != 0)) * dwPageSize -
                pos;
        char *ptr = (char *) hFileMapping + pos;
        if (!StdError(mprotect(ptr, sz, PROT_READ | PROT_WRITE | PROT_EXEC)) &&
            !StdError(mlock(ptr, sz)))
#else
            size_t _off = 0;
            SQWORD s;
            s.x = offset;
            char *ptr = (char *)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, s.HDW, s.LDW, size);

#endif
            return ptr + _off;
    }
    return 0;

}


//-------------------------------------------
//
//-------------------------------------------
bool MapMemFile::UnMap(void *_ptr, size_t size)
{
    if (hFileMapping)
    {
#ifdef __PLUK_LINUX__
        size_t offset = (size_t) ((char *) _ptr - (char *) hFileMapping);
        size_t pos = offset / dwPageSize * dwPageSize;
        size = ((offset + size) / dwPageSize + ((offset + size) % dwPageSize != 0)) * dwPageSize -
               pos;
        void *ptr = (char *) hFileMapping + pos;
        if (!msync(ptr, size, MS_SYNC) && !mprotect(ptr, size, PROT_NONE))
            return true;
        StdError(-1);
#else
        UnmapViewOfFile(_ptr);
#endif
    }
    return 1;
}

ShmMemView::~ShmMemView()
{
    Close();
}

ShmMemView::ShmMemView()
{
    hFile = INVALID_HANDLE_VALUE;
    hFileMapping = 0;
    FileBase = 0;
    ShareName[0] = 0;
    Size = 0;

}

bool ShmMemView::Open(LPCSTR _ShareName, int size)
{
    Close();

    hFile = INVALID_HANDLE_VALUE;
    hFileMapping = 0;
    FileBase = 0;
    DWORD dwPageSize;
#ifdef __PLUK_LINUX__
    dwPageSize = sysconf(_SC_PAGESIZE);
#else
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    dwPageSize = si.dwPageSize;
#endif
    TString n;
    if (_ShareName[0] != FD)
        n = SFD;
    n += _ShareName;
    strcpy(ShareName, n.GetPtr());
    if (size)
    {
        size = (size / dwPageSize + (size % dwPageSize != 0)) * dwPageSize;
        Size = size;
    }
    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    BOOL fCreate = 0;
#ifdef __PLUK_LINUX__
#ifdef ANDROID_NDK
    hFile = open(ASHMEM_DEVICE, O_RDWR);
    int ret;
    if (hFile != INVALID_HANDLE_VALUE)
    {
        ret = ioctl(hFile, ASHMEM_SET_NAME, ShareName);
        if (ret < 0)
        {
            close(hFile);
            hFile = INVALID_HANDLE_VALUE;
        } else
        {
            StdError(fstat(hFile, &st));
            Size = st.st_size;
            if (!st.st_size)
            {
                ret = ioctl(hFile, ASHMEM_SET_SIZE, size);
                if (ret < 0)
                {
                    close(hFile);
                    hFile = INVALID_HANDLE_VALUE;
                }
                fCreate = 1;
            } else
                fCreate = 0;
            if (hFile != INVALID_HANDLE_VALUE)
            {
                hFileMapping = mmap(0, Size, PROT_WRITE | PROT_READ, MAP_SHARED, hFile, 0);
                if (hFileMapping == (void *) -1)
                {
                    StdError(-1);
                    hFileMapping = 0;
                    close(hFile);
                    hFile = INVALID_HANDLE_VALUE;
                    return 0;
                } else
                {
                    FileBase = (BYTE *) hFileMapping;
                    if (fCreate)
                        bzero(FileBase, Size);
                    return 1;
                }
            }
        }
    }
#else

    hFile = shm_open(ShareName, O_RDWR, 0);
 if (size)
    if ( hFile == INVALID_HANDLE_VALUE )
    {
         hFile = shm_open(ShareName, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG |  S_IRWXO);
        fCreate = true;
    }
if ( hFile != INVALID_HANDLE_VALUE )
{
        TString nm =  ShareName;
        nm += "_sem";
        sem.Open(nm.GetPtr());
     sem.Leave();
     int count = sem.Count();
    if (size && fCreate)
    StdError(ftruncate(hFile, size));
    StdError(fstat(hFile, &st));
    Size = st.st_size;
    hFileMapping = mmap(0, Size, PROT_WRITE | PROT_READ, MAP_SHARED, hFile, 0);
     if (hFileMapping == (void *)-1)
     {
         StdError(-1);
        hFileMapping = 0;
        close(hFile);
        shm_unlink(ShareName);
        hFile = INVALID_HANDLE_VALUE;
        return 0;
     }
     else
     {
        FileBase = (BYTE *)hFileMapping;
        if (fCreate)
            bzero(FileBase, Size);
        return 1;
     }
}
else
   StdError(-1);
#endif
#else
    hFileMapping = OpenFileMapping( FILE_MAP_ALL_ACCESS, FALSE, ShareName );
    if (hFileMapping == 0 && size)
    {
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = FALSE;
        SECURITY_DESCRIPTOR *sd = (SECURITY_DESCRIPTOR *)HMALLOC(sizeof(SECURITY_DESCRIPTOR));
        if ( sd == 0)
            return 0;
        if (InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION) == 0)
        {
            HFREE(sd);
            return 0;
        }
        sa.lpSecurityDescriptor = sd;
        BOOL InitFlg = SetSecurityDescriptorDacl( sd, TRUE, 0L, TRUE);
        if ( InitFlg == FALSE )
        {
            HFREE(sd);
            char buff[80];
            TRACE(buff, "Object %s Access Error #%d, PID #%d", ShareName, GetLastError(), _getpid());
            return 0;
        }

        hFileMapping = CreateFileMapping( INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, size, ShareName);
         if ( hFileMapping == 0 )
         {

            return 0;
        }
    }
    if (FileBase = (BYTE *)MapViewOfFile( hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, size))
        return 1;
    Close();
#endif
    return 0;
}

int ShmMemView::Destroy()
{

    int count;
    if (strlen(ShareName) && (count = sem.Count()) == 1)
    {
        TString nm = sem.pName;

        sem.Close();
#ifndef ANDROID_NDK
        shm_unlink(ShareName);
#endif
        sem_unlink(nm.GetPtr());
        return true;
    }
    return false;


}

int ShmMemView::Close()
{
#ifdef __PLUK_LINUX__
    if (FileBase)
        StdError(munmap(FileBase, Size));
    if (hFile != INVALID_HANDLE_VALUE)
    {
        close(hFile);
        sem.TryEnter();
        int count = sem.Count();

        //shm_unlink(ShareName);
        Destroy();
    }
#else
    if (FileBase)
        UnmapViewOfFile(FileBase);
    if (hFileMapping)
        CloseHandle(hFileMapping);
#endif
    hFileMapping = 0;
    FileBase = 0;
    hFile = INVALID_HANDLE_VALUE;
    return 1;
}
