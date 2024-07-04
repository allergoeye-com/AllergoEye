/* 
 * File:   Semaphore.cpp
 * Author: alex
 * 
 * Created on October 12, 2013, 4:01 PM
 */
#include "stdafx.h"

#include "CSemaphore.h"
//---------------------------
//
//---------------------------
BOOL Semaphore::Open(const char *name)
{
    
	Close();
//   sem_unlink(name);
    int ret = 0;
    UINT nn=S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH;
	if ((hSem = sem_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH, 1)) == SEM_FAILED)
    {
        if (errno == EEXIST)
        {
            ++ret;
            errno = 0;
            hSem = sem_open(name, 0);
        }
        else
            StdError(-1);
    }

  
	if (hSem !=  SEM_FAILED)
	{
        ++ret;
		pName = new char[strlen(name) + 1];
		strcpy(pName, name);
	}
    else
	    StdError(-1);
	return hSem != SEM_FAILED ? ret : 0;
}
//---------------------------
//
//---------------------------
void Semaphore::Close()
{
	if (hSem != SEM_FAILED)
	{
		StdError(sem_close(hSem));
		//sem_unlink(pName);
		delete [] pName;
	}
	hSem = SEM_FAILED;
}
//-------------------------------------------
//
//-------------------------------------------
BOOL LocalSemaphore::Create()
{
	hSem = new sem_t;
	if (!hSem || sem_init(hSem, 0, 0) != 0)
	{
		StdError(-1);
		if (hSem) delete hSem;
		hSem = 0;
	}
	return 0;
}

//-------------------------------------------
//
//-------------------------------------------
void LocalSemaphore::Close()
{
	if (hSem)
	{
		sem_destroy(hSem);
		delete hSem;
		hSem = 0;

	}
}

//---------------------------
//
//---------------------------
int GenSemaphore::Count()
{
	int value = 0;
	if (hSem != SEM_FAILED)
	StdError(sem_getvalue(hSem, &value));
    return value;
}
//---------------------------
//
//---------------------------
BOOL GenSemaphore::Leave(int N)
{
    int n = 0;
	if (hSem != SEM_FAILED)
    {
		for (int i = 0; i < N; ++i)
		if (sem_post(hSem) != 0)
			StdError(-1);
		else
			++n;
	}
    return n == N;
}
BOOL GenSemaphore::SetLeave(int N)
{
	static int i = 0;
	i++;
    int n = 0;
    if (N == Count()) return TRUE;
	if (hSem != SEM_FAILED)
    {
		if (N < Count())
		{
			while (Count() < N)
				Enter();
			return TRUE;
		}
		else
		{
			for (int i = Count(); i < N; ++i)
			if (sem_post(hSem) != 0)
				StdError(-1);
			else
				++n;
		}
	}
    return n == N;
}

//-------------------------------------------
//
//-------------------------------------------
BOOL GenSemaphore::TryEnter(UINT milisecond)
{
	if (hSem == SEM_FAILED) return -1;

			timespec tsp;
        timeval now;
        clock_gettime(CLOCK_REALTIME, &tsp);
        tsp.tv_sec += milisecond/1000;
        tsp.tv_nsec += (milisecond%1000) * 1000000;
    errno = 0;
    int ret = sem_timedwait(hSem, &tsp);
	if (ret < 0)
    {
        ret = errno;
         
   		 if (ret == EINTR || ret == ETIMEDOUT)
			errno = 0;
         else
            StdError(ret);
    }
	return ret == 0;
}

//---------------------------
//
//---------------------------
BOOL GenSemaphore::TryEnter()
{
	int ret = -1;
	if (hSem != SEM_FAILED)
	for(;;)
	{
		errno = 0;
		ret = sem_trywait(hSem);
		if (ret < 0)
		{
			 int err = errno;
			 if (err == EINTR)
			 {
				errno = 0;
				continue;
			}
		}
		break;
	}
	return ret == 0;
}
//---------------------------
//
//---------------------------
BOOL GenSemaphore::Enter()
{
	int ret = -1;
	if (hSem != SEM_FAILED)
	for(;;)
	{
		errno = 0;
		ret = sem_wait(hSem);
		if (ret < 0)
		{
			 int err = errno;
			 if (err == EINTR)
			 {
				errno = 0;
				continue;
			}
		}
		break;
	}
	return ret == 0;
}
