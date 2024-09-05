/*
 * mdir.cpp
 *
 *  Created on: Mar 23, 2012
 *      Author: alex
 */
#include "stdafx.h"
#ifdef __PLUK_LINUX__
#include "mdir.h"
MDIR::MDIR(const char *namedir):MIO(T_DIR)
{
	ptr = 0;
        TString _name;
	if (namedir) _name = namedir;
	if (!_name.StrLen())
	{
	  _name.SetLen(255);
	  _name.Set(0);
		while(!getcwd(_name.GetPtr(), _name.GetLen() - 1))
		{
			_name.SetLen(_name.GetLen() * 2);
			_name.Set(0);
		}
  	}
    name = _name.GetPtr();
    _name.Detach();
	

}
struct tm *MDIR::GetTime()
{
       mktime(&Time);
 	#ifdef __PLUK_LINUX__
	struct stat buf;
	if (!stat(name, &buf))
        	memcpy(&Time, &buf.st_mtime, sizeof(struct tm));
        #else
	pl_finddata_t ff;
	DINT handle = pl_findfirst(RawDir, &ff);
	if (handle != -1)
        {
		_findclose(handle);
        	memcpy(&Time, &ff.time_write, sizeof(struct tm));
        }        
        #endif
        localtime((time_t *)&Time);
        return &Time;
}
off_t MDIR::GetSize()
{
    off_t res = 0;
  #ifdef WIN32
	pl_finddata_t ff;
	DINT handle = pl_findfirst(RawDir, &ff);

	if (handle != -1)
	{
		_findclose(handle);
		res = ff.size;
	}
#else
        DIR *ptr = opendir(name);
        if (ptr)
        {    
            TString dn;
            struct stat buf;
            dirent *dp;
            while ((dp = readdir(ptr)))
            {
                dn = name;
                dn += dp->d_name;
		if (!StdError(stat(dn.GetPtr(), &buf)))
                {
                    if (S_ISDIR(buf.st_mode) && !S_ISREG(buf.st_mode)) continue;
                    res +=buf.st_size;
                }
            }
            StdError(closedir(ptr));
	}
#endif
        return res;
}
bool MDIR::Close() { bool fRet = false; if (ptr) fRet = StdError(closedir(ptr)) == 0; return fRet; }

bool MDIR::Open(bool fCreate)
{
	if (!(ptr = opendir(name)))
	{
		if(fCreate)
		{
			if (!StdError(mkdir(name, S_IRWXU|S_IRWXG|S_IRWXO)))
				ptr = opendir(name);
		}
	}
	return ptr != 0;
}
#ifndef ANDROID_NDK

off_t MDIR::SetSeek(off_t offset)
{
	if (ptr)
	{
		seekdir(ptr, offset);
		return telldir(ptr);
	}
	return -1;
}
#endif
off_t MDIR::Read(void *s, off_t size)    { return Read(-1, s, size); }

off_t MDIR::Read(off_t offset, void *s, off_t size)
{
	if (!ptr) return false;
	if (offset != (off_t)-1)
		offset = SetSeek(offset);
        else
        	offset = GetSeek();    
	dirent *dp = readdir(ptr);
	if (dp)
	{
		off_t l;
		if ((l = strlen(dp->d_name)) < size)
			strcpy((char *)s, dp->d_name);
		else
			memcpy(s, dp->d_name, l);
	}
        else
            return 0;
	return GetSeek() - offset;
}

#endif
