#pragma once
#include "RWThread.h"
#include "RWLock.h"
class NetServerThread : public RWThread {
public:
	NetServerThread() { }
	virtual ~NetServerThread() {}
#ifdef WIN32
	static DWORD WINAPI MessageLoop(void* h);
#else
	void MessageLoop();
	static NetClientThread* instance;
#endif
	void OnRead(SOCKET fd);
	virtual BOOL Close();
	bool WriteResponce(LPCSTR cmd, BYTE* param, int bufLen);
	bool WriteResponce(LPCSTR cmd, MVector< MArray<BYTE> > &param);

	bool Connect(HostData &client);
	
	HostData m_HostData;

	static RWLock *rwLock;

};

