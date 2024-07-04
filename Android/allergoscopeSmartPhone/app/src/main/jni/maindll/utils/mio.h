#ifndef ___MIO__
#define ___MIO__
#ifndef WIN32

#include "std.h"

#endif
#ifdef __PLUK_LINUX__

#include    <dirent.h>

#endif

#include    <sys/stat.h>

#ifndef WIN32

#include "base_error.h"

#endif
#pragma pack(push) //NADA
#pragma pack(4)
EXPORTNIX class _PEXPORT MIO {
public:
    enum MIO_Type {
        T_UNK, T_FILE, T_DIR, T_BLK, T_CHAR_BLK, T_PIPE, T_SYM, T_SOCK
    };

    MIO(MIO_Type t)
    {
        tp = t;
        name = 0;
    }

    virtual ~MIO()
    { if (name) HFREE(name); }

    virtual bool Open(const char *n, bool fCreate = false)
    {
        if (n)
        {
            name = (char *) HMALLOC(strlen(n) + 1);
#ifndef WIN32
            strcpy(name, n);
#else
            strcpy_s(name, strlen(n) + 1, n);
#endif
            return Open(fCreate);
        }
        return 0;
    }

    virtual bool Open(bool fCreate = false)
    { return false; }

    virtual bool Write(off_t offset, const void *s, off_t size)
    { return 0; }

    virtual off_t Read(off_t offset, void *s, off_t size)
    { return 0; }

    virtual bool Write(const void *s, off_t size)
    { return Write(-1, s, size); }

    virtual off_t Read(void *s, off_t size)
    { return Read(-1, s, size); }

    virtual bool Close()
    { return 0; }

    virtual off_t GetSeek()
    { return false; }

    virtual off_t SetSeek(off_t offset)
    { return 0; }

    virtual off_t GetSize()
    { return 0; }

    virtual bool SetSize(off_t size)
    { return false; }

    virtual bool Flush()
    { return false; }

    MIO_Type Type()
    { return tp; }

    virtual const char *Name()
    { return name; }

protected:
    MIO_Type tp;
    char *name;

};
#pragma pack(pop) //NADA

#endif
