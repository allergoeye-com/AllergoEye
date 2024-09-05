//
// Created by alex on 18.03.2018.
//

#ifndef FUTURA_RWLOCK_H
#define FUTURA_RWLOCK_H


#ifndef WIN32
#include <sys/types.h>
#include <pthread.h>
#else
#include <synchapi.h>
#endif

class RWLock
{
public:
	RWLock()
	{
#ifdef WIN32
		InitializeSRWLock(&rwlock);

#else
		pthread_rwlockattr_init(&attr);
		pthread_rwlock_init(&rwlock, &attr);
#endif
	}

	~RWLock()
	{
#ifndef WIN32
		pthread_rwlock_destroy(&rwlock);
#endif

	}

	void ReadEnter()
	{

#ifdef WIN32
		AcquireSRWLockShared(&rwlock);
#else
		pthread_rwlock_rdlock(&rwlock);
#endif

	}

	void ReadLeave()
	{
#ifdef WIN32
		ReleaseSRWLockShared(&rwlock);
#else
		pthread_rwlock_unlock(&rwlock);
#endif
	}

	void WriteEnter()
	{
#ifdef WIN32
		AcquireSRWLockExclusive(&rwlock);
#else
		pthread_rwlock_wrlock(&rwlock);
#endif
	}

	void WriteLeave()
	{
#ifdef WIN32
		ReleaseSRWLockExclusive(&rwlock);
#else
		pthread_rwlock_unlock(&rwlock);
#endif


	}

private:

#ifdef WIN32
    RTL_SRWLOCK rwlock;
#else
    pthread_rwlock_t rwlock;
    pthread_rwlockattr_t attr;
#endif
};

#endif //FUTURA_RWLOCK_H
