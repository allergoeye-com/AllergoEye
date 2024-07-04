/* 
 * File:   CSemaphore.h
 * Author: alex
 *
 * Created on October 12, 2013, 4:01 PM
 */

#ifndef SEMAPHORE_H
#define	SEMAPHORE_H

#ifdef __PLUK_LINUX__
#pragma pack(push) //NADA
#pragma pack(4)
#include "semaphore.h"
EXPORTNIX class GenSemaphore {
public :
	GenSemaphore() { hSem = 0; }
	virtual ~GenSemaphore() { }
	BOOL IsValid() { return hSem != 0; }
	int Count();
	BOOL TryEnter();
    BOOL TryEnter(UINT milisecond);
	BOOL Enter();
	BOOL Leave(int n = 1);
	sem_t *Handle() { return hSem; }
	BOOL SetLeave(int N);

protected:
	virtual void Close() = 0;
	BOOL Create();
	sem_t *hSem;


};


EXPORTNIX struct Semaphore : public GenSemaphore {
	Semaphore() { pName = 0;hSem = SEM_FAILED; }
	virtual ~Semaphore() { Close(); }
	BOOL Open(const char *name);
	virtual void Close();

	char *pName;
	
};
EXPORTNIX class LocalSemaphore: public GenSemaphore {
public :
	LocalSemaphore() { hSem = 0; Create(); }
	virtual ~LocalSemaphore() {Close(); }
protected:
	BOOL Create();
	virtual void Close();
};
#pragma pack(pop) //NADA

#endif

#endif	/* SEMAPHORE_H */

