#ifndef __CriticalSection__
#define __CriticalSection__
#pragma pack(push) //NADA
#pragma pack(4)

#ifdef __PLUK_LINUX__
#ifndef CRITICAL_SECTION
//	typedef pthread_mutex_t CRITICAL_SECTION;

struct CRITICAL_SECTION {
    pthread_mutex_t crit;
    pthread_mutexattr_t attr;
};
#endif
//#ifndef DeleteCriticalSection
//	#define DeleteCriticalSection pthread_mutex_destroy
//#endif
#ifndef GetCurrentThreadId
#define GetCurrentThreadId pthread_self
#endif
#ifndef GetCurrentProcessId
#define GetCurrentProcessId getpid
#endif
#ifndef GetCurrentThread
#define GetCurrentThread pthread_self
#endif

inline void LeaveCriticalSection(CRITICAL_SECTION *a)
{
    pthread_mutex_unlock(&a->crit);
}

inline void EnterCriticalSection(CRITICAL_SECTION *a)
{
    pthread_mutex_lock(&a->crit);
}

inline int TryEnterCriticalSection(CRITICAL_SECTION *m_crit)
{ return pthread_mutex_trylock(&m_crit->crit) == 0; }

inline void InitializeCriticalSection(CRITICAL_SECTION *p)
{
    pthread_mutexattr_init(&p->attr);
    pthread_mutexattr_settype(&p->attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&p->crit, &p->attr);
}

inline int InitializeCriticalSectionAndSpinCount(CRITICAL_SECTION *p, int t)
{
    pthread_mutexattr_init(&p->attr);
    pthread_mutexattr_settype(&p->attr, PTHREAD_MUTEX_RECURSIVE_NP);
    return pthread_mutex_init(&p->crit, &p->attr);
}


inline void DeleteCriticalSection(CRITICAL_SECTION *p)
{
    pthread_mutex_destroy(&p->crit);
    pthread_mutexattr_destroy(&p->attr);
}

inline pthread_mutex_t *PointerCritical(CRITICAL_SECTION *p)
{
    return &p->crit;
}

struct RWCriticalSection {
    RWCriticalSection()
    { pthread_rwlock_init(&wrlock, NULL); };

    ~RWCriticalSection()
    { pthread_rwlock_destroy(&wrlock); }

    bool ReadEnter()
    {
        return pthread_rwlock_rdlock(&wrlock) == 0;
    }

    bool WriteEnter()
    {
        return pthread_rwlock_wrlock(&wrlock) == 0;
    }

    bool TryReadEnter()
    {
        return pthread_rwlock_tryrdlock(&wrlock) == 0;
    }

    bool TryWriteEnter()
    {
        return pthread_rwlock_trywrlock(&wrlock) == 0;
    }

    bool RWLeave()
    {
        return pthread_rwlock_unlock(&wrlock) == 0;
    }

    pthread_rwlock_t wrlock;
};

#else
#include <windows.h>
#ifndef pthread_t
typedef HANDLE pthread_t;
#endif
#ifndef pthread_mutex_t
typedef CRITICAL_SECTION pthread_mutex_t;
#endif
#ifndef PointerCritical
#define PointerCritical(p) (p)
#endif
#endif

class CriticalSection {
public:
    CriticalSection()
    {
        self = 0;

        InitializeCriticalSection(&m_crit);

    }

    ~CriticalSection()
    {
        DeleteCriticalSection(&m_crit);
    }

    pthread_mutex_t *Handle()
    { return PointerCritical(&m_crit); }

    void Leave()
    {
        //  if (Check())
        {
            self = 0;
            LeaveCriticalSection(&m_crit);
        }
    }

    bool Check()
    {
        return self == GetCurrentThread();
    }

    void Enter()
    {
        EnterCriticalSection(&m_crit);
        self = GetCurrentThread();

    }

    BOOL TryEnter()
    {
        if (TryEnterCriticalSection(&m_crit))
        {
            self = GetCurrentThread();
            return true;
        }
        return false;
    }

protected:
    CRITICAL_SECTION m_crit;
    pthread_t self;

};

#pragma pack(pop) //NADA

#include "CSemaphore.h"

#endif
