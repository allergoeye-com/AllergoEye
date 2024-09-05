#pragma once
class LocalSemaphore {
public:
	LocalSemaphore(int num = 100);
	~LocalSemaphore();
	BOOL IsValid() { return hSem != 0; }
	int Count();
	BOOL TryEnter();

	BOOL TryEnter(UINT milisecond);
	BOOL Enter();
	BOOL Leave(int n = 1);
	BOOL SetLeave(int N);

protected:
	virtual void Close();
	BOOL Create();
	HANDLE hSem;
	long count;
};

