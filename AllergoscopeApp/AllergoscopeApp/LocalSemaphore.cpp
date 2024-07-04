#include "stdafx.h"
#include "LocalSemaphore.h"
//----------------------------------------------
//
//----------------------------------------------
LocalSemaphore::LocalSemaphore(int num)
{
	count = num;
	hSem = CreateSemaphore(0, num, num, 0);
}
//----------------------------------------------
//
//----------------------------------------------
LocalSemaphore::~LocalSemaphore()
{
	Close();

}
//----------------------------------------------
//
//----------------------------------------------
int LocalSemaphore::Count()
{
	return count;
}
//----------------------------------------------
//
//----------------------------------------------
BOOL LocalSemaphore::TryEnter()
{
	return TryEnter(0);

}
//----------------------------------------------
//
//----------------------------------------------
BOOL LocalSemaphore::TryEnter(UINT milisecond)
{
	int n = 0;
	if (hSem)
	{

		InterlockedExchange(&count, 0);
		n = WaitForSingleObject(hSem, milisecond) == WAIT_OBJECT_0;
	}
	return n;

}
//----------------------------------------------
//
//----------------------------------------------
BOOL LocalSemaphore::Enter()
{
	return TryEnter(INFINITE);
}
//----------------------------------------------
//
//----------------------------------------------
BOOL LocalSemaphore::Leave(int n)
{
	if (hSem)
	{

		::ReleaseSemaphore(hSem, n, &count);
		InterlockedExchange(&count, 1);
	}
	return count >= n;
}
//----------------------------------------------
//
//----------------------------------------------
BOOL LocalSemaphore::SetLeave(int n)
{
	if (count == n) return false;
	if (!n) return Enter();
	return Leave(n);
}
//----------------------------------------------
//
//----------------------------------------------
void LocalSemaphore::Close()
{
	if (hSem)
		CloseHandle(hSem);
	hSem = 0;
}


