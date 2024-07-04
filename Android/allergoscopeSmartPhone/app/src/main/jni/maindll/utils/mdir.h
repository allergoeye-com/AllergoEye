/*
 * mdir.h
 *
 *  Created on: Mar 23, 2012
 *      Author: alex
 */

#ifndef MDIR_H_
#define MDIR_H_

#include "mio.h"
#include <time.h>

#pragma pack(push) //NADA
#pragma pack(4)

extern "C++" class _PEXPORT MDIR : public MIO {
public:
    MDIR(const char *namedir = 0);

    virtual ~MDIR()
    {}

    virtual bool Open(bool fCreate = false);

#ifndef ANDROID_NDK

    virtual off_t SetSeek(off_t offset);
    virtual off_t GetSeek() { return ptr ? telldir(ptr) : -1;  }
#endif

    virtual off_t Read(off_t offset, void *s, off_t size);

    virtual bool Close();

    bool Make()
    { return mkdir(name, S_IRWXU | S_IRWXG | S_IRWXO) != -1; }

    bool Remove()
    {
        Close();
        return rmdir(name) != -1;
    };

    bool ChangeDir()
    { return chdir(name) != -1; }

    virtual off_t GetSize();

    virtual off_t Read(void *s, off_t size);


    struct tm *GetTime();

protected:
#ifdef __PLUK_LINUX__
    DIR *ptr;
#else
    pl_finddata_t ptr;
#endif
    struct tm Time;
};
#pragma pack(pop) //NADA


#endif /* MDIR_H_ */
