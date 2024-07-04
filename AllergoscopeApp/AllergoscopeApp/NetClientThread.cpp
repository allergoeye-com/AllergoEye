#include "stdafx.h"
#include "NetClientThread.h"
#ifndef WIN32
#include "com_allergoscope_app_wifi_SyncThread.h"
#ifndef EPOLLRDHUP
#define EPOLLRDHUP 0x2000
#endif
#else
#include <process.h>
#define MSG_DONTWAIT 1
#endif
void AddToLog(LPCSTR t);

int OnError();
//-----------------------------------------
//
//-----------------------------------------
LRESULT NetClientThread::OnMessage(UINT Msg, WPARAM wParam, LPARAM lParam)

{

	LRESULT fRet = 0;
	if (waitPack.IsValid())
	{
		Msg = iERROR;
	}
	switch (Msg)
	{
	case iERROR:
		result = iERROR;
		waitPack.Reset();
		mutex.SetLeave(1);
		break;
	
	case iREAD:
		fRet = (LRESULT)Read((PacketRead*)wParam);
		break;

	case iWRITE:
		fRet = (LRESULT)Write((PacketWrite*)wParam);
		break;
		case iWRITE_FILE:
			fRet = (LRESULT)WriteFile((PacketWriteFile*)wParam);
			break;
		case iFILE:
	    case iGETROI:
            fRet = (LRESULT)GetRoi((PacketWriteFile*)wParam);
	default:
		ASSERT(0);
	}

	return fRet;
}
//--------------------------------------------------------
//
//--------------------------------------------------------
#ifdef WIN32
DWORD WINAPI NetClientThread::MessageLoop(void* h)
{
	NetClientThread* hs = (NetClientThread*)h;
	WSAEVENT msg = WSACreateEvent();
	WSAEVENT handle[3] = { hs->eExit, msg, hs->eOnRead };
	DWORD dwEvent;


	if (WSAEventSelect(hs->fd, handle[1], FD_READ | FD_CLOSE) == SOCKET_ERROR)
		goto __exit_th;
	hs->mutex.Leave();
	while (TRUE)
	{
		if ((dwEvent = WSAWaitForMultipleEvents(3, handle, FALSE, WSA_INFINITE, FALSE)) == WSA_WAIT_FAILED)
			goto __exit_th;
		if (dwEvent == WSA_WAIT_TIMEOUT)
		{
			; // hs->mutex.SetLeave(1);
		}

		if (dwEvent == WSA_WAIT_EVENT_0)
		{
			WSAResetEvent(handle[0]);
			goto __exit_th;
		}
		else
			if (dwEvent == WSA_WAIT_EVENT_0 + 1)
			{

				if (WSAEnumNetworkEvents(hs->fd, handle[1], &hs->NetworkEvents) == SOCKET_ERROR)
					goto __exit_th;

				if ((hs->NetworkEvents.lNetworkEvents & FD_CLOSE) == FD_CLOSE)
					goto __exit_th;

				if ((hs->NetworkEvents.lNetworkEvents & FD_READ) == FD_READ)
				{
					WSAResetEvent(handle[1]);
					hs->OnRead(hs->fd);
				}



			}
		if (dwEvent == WSA_WAIT_EVENT_0 + 2)
		{
			WSAResetEvent(handle[2]);
			hs->critical.Enter();
			Msg m = hs->global;
			hs->critical.Leave();
			if (m.m == iTHREAD_EXIT)
				goto __exit_th;
			else
				hs->OnMessage(m.m, m.w, m.l);


		}
	}

__exit_th:
	WSACloseEvent(msg);

	hs->mutex.Leave();
	hs->Close();
	delete hs;
	return 0;

}
#else
//------------------------------------------------
//
//------------------------------------------------
void NetClientThread::MessageLoop()
{
	int error = 0;
	struct stat st;
	TString sserr;
	errno = 0;
	int error_hungup = 0;
	int res = fstat((int)fd, &st);
	if (res)
	{
		error = errno;
		sserr = strerror(error);
		errno = 0;
	}
	else
	{
#ifndef GLOBAL_POOL

		res = socketpair(AF_UNIX, SOCK_DGRAM, 0, sockMsg);
		if (res)
		{
			error = errno;
			sserr = strerror(error);
			errno = 0;
		}
#endif
	}
	if (!res)
	{
		MArray<epoll_event> aevents(EPOOL_SIZE);
		epoll_event* events = aevents.GetPtr();
		struct epoll_event ev;
#ifndef GLOBAL_POOL
		StdError(fcntl(sockMsg[0], F_SETFL, fcntl(sockMsg[0], F_GETFD, 0) | O_NONBLOCK));
#endif

		ev.events = EPOLLIN | EPOLLET | EPOLLERR;
		ev.data.fd = sockMsg[0];
		StdError(epoll_ctl(epfd, EPOLL_CTL_ADD, sockMsg[0], &ev));
		ev.events = EPOLLIN | EPOLLRDHUP | EPOLLERR;
	_start:
		ev.data.fd = fd;
		int ct = fcntl(fd, F_GETFD, 0);
		if ((ct & O_NONBLOCK) != O_NONBLOCK)
			fcntl(fd, F_SETFL, ct | O_NONBLOCK);
		epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
		if (!mutex.Count())mutex.SetLeave(1);
		while (instance && epfd != -1)
		{
			bzero(events, EPOOL_SIZE * sizeof(epoll_event));


			int count = epoll_wait(epfd, events, EPOOL_SIZE, -1);
			if (count < 0)
			{
				if (errno != EINTR)
					StdError(-1);
				else
					errno = 0;
				continue;

			}
			else
			{
				for (int i = 0; i < count; ++i)
				{

					if (events[i].data.fd == fd)
					{
						if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLRDHUP))
						{
							if ((events[i].events & EPOLLERR))
							{
								int  error = 0;
								socklen_t errlen = sizeof(error);
								if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void*)& error, &errlen) == 0)
								{
									sserr.Format("socket error = %s ( %d (%d) )\n", strerror(error), error, errno);
									errno = 0;
									if (error)
										AddToLog(sserr.GetPtr());

								}
							}
							else
								if (events[i].events & EPOLLRDHUP)
								{
									int  error = errno;
									errno = 0;
									AddToLog("EPOLLRDHUP\n");
									goto goto__exit;
								}

							continue;
						}

						if (events[i].events & EPOLLIN)
						{
							OnRead(fd);
						}

					}
				}

				for (int i = 0; i < count; ++i)
				{

					if (events[i].data.fd == sockMsg[0])
					{
						if ((events[i].events & EPOLLERR))
						{
							int  error = 0;
							socklen_t errlen = sizeof(error);
							if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void*)& error, &errlen) == 0)
							{
								sserr.Format("(UX_SOCKET) socket error = %s ( %d (%d) )\n", strerror(error), error, errno);
								errno = 0;
								AddToLog(sserr.GetPtr());
								epoll_ctl(epfd, EPOLL_CTL_DEL, sockMsg[0], 0);
								sockMsg[0] = sockMsg[1] = 0;
								int error, res = socketpair(AF_UNIX, SOCK_DGRAM, 0, sockMsg);
								TString sserr;
								if (res)
								{
									error = errno;
									sserr.Format("socketpair = %s ( %d (%d) )\n", strerror(error), error, errno);
									errno = 0;
									AddToLog(sserr.GetPtr());

								}
								fcntl(sockMsg[0], F_SETFL, fcntl(sockMsg[0], F_GETFD, 0) | O_NONBLOCK);
								ev.events = EPOLLIN | EPOLLET | EPOLLERR;
								ev.data.fd = sockMsg[0];
								epoll_ctl(epfd, EPOLL_CTL_ADD, sockMsg[0], &ev);

							}
						}
						else
						{
							Msg m;
							fcntl(sockMsg[0], F_SETFL, fcntl(sockMsg[0], F_GETFD, 0) & ~O_NONBLOCK);
							recv(sockMsg[0], &m, sizeof(Msg), 0);

							fcntl(sockMsg[0], F_SETFL, fcntl(sockMsg[0], F_GETFD, 0) | O_NONBLOCK);

							if (m.m == iTHREAD_EXIT)
							{
								AddToLog("USER MSG EXIT THREAD\n");

								goto goto__exit;
							}
							else
								OnMessage(m.m, m.w, m.l);
						}
					}
				}
			}
		}
	}
goto__exit:
	if (!mutex.Count())
	{
		instance = 0;
		mutex.SetLeave(1);
		Sleep(100);
	}
	mutex.SetLeave(0);
	instance = 0;
	Close();
	mutex.SetLeave(1);
	__pthread_cleanup_pop(&__cleanup, 1);
	self.Detach();
	AddToLog("EXIT EPOOL\n");

	delete this;

}
#endif
#ifdef WIN32 
int NetClientThread::send(SOCKET, Msg* buff, int s, int)
{
	critical.Enter();
	global = *buff;
	critical.Leave();
	WSASetEvent(eOnRead);
	return s;
}
#endif
//------------------------------------------------
//
//------------------------------------------------
int NetClientThread::Read(PacketRead* p)
{
	if (p)
	{
		int len = sizeof(FunCall) + sizeof(Query) - 1;
		MArray<char > buff(len);
		FunCall* call = (FunCall*)buff.GetPtr();
		if (call)
		{
			memset(call, 0, len);
			call->SetName("READ");
			call->len = len;
			call->num_param = 1;
			Param* _param = call->param;
			_param->len = sizeof(Query);
			memmove(_param->param, &p->cmd, sizeof(Query));
			if (write(call, len))
			{
				waitPack.Init(p);
				return 0;
			}
		}
	}
	result = iERROR;
	waitPack.Reset();
	mutex.SetLeave(1);

	return -1;

}
int NetClientThread::WriteFile(PacketWriteFile *p)
{
	int len = sizeof(FunCall) + sizeof(Query) + sizeof(Param) + p->dwLenBuff + sizeof(Param) + strlen(p->FilePath) - 2;
	int Len = len;
	MArray<char> buff(len);
	FunCall* call = (FunCall*)buff.GetPtr();
	if (call)
	{
		call->SetName("WRITE");
		call->len = len;
		call->num_param = 3;
		Param* _param = call->param;
		_param->len = sizeof(Query);
		memmove(_param->param, &p->cmd, sizeof(Query));
		Param* param2 = (Param*)(_param->param + _param->len);
		param2->len = strlen(p->FilePath) + 1;
		memmove(param2->param, p->FilePath, param2->len);
		Param* param3 = (Param *)(param2->param + param2->len);
		param3->len = p->dwLenBuff;
		memmove(param3->param, p->pBuff, p->dwLenBuff);

		if (write(call, len))
		{
			waitPack.Init(p);
			return 0;

		}

	}
	result = iERROR;
	waitPack.Reset();
	mutex.Leave();


	return -1;

}
int NetClientThread::GetRoi(PacketWriteFile *p)
{
    int len = sizeof(FunCall) + sizeof(Query) + sizeof(Param) + strlen(p->FilePath) - 1;
    int Len = len;
    MArray<char> buff(len);
    FunCall* call = (FunCall*)buff.GetPtr();
    if (call)
    {
        call->SetName("READ");
        call->len = len;
        call->num_param = 2;
        Param* _param = call->param;
        _param->len = sizeof(Query);
        memmove(_param->param, &p->cmd, sizeof(Query));
        Param* param2 = (Param*)(_param->param + _param->len);
        param2->len = strlen(p->FilePath) + 1;
        memmove(param2->param, p->FilePath, param2->len);

        if (write(call, len))
        {
            waitPack.Init(p);
            return 0;

        }

    }
    result = iERROR;
    waitPack.Reset();
    mutex.Leave();


    return -1;

}

//------------------------------------------------
//
//------------------------------------------------
int NetClientThread::Write(PacketWrite* p)
{
	int len = sizeof(FunCall) + sizeof(Query) + sizeof(Param) + p->dwLenBuff - 2;
	int Len = len;
	MArray<char> buff(len);
	FunCall* call = (FunCall*)buff.GetPtr();
	if (call)
	{
		call->SetName("WRITE");
		call->len = len;
		call->num_param = 2;
		Param* _param = call->param;
		_param->len = sizeof(Query);
		memmove(_param->param, &p->cmd, sizeof(Query));
		Param* param2 = (Param*)(_param->param + _param->len);
		param2->len = p->dwLenBuff;
		memmove(param2->param, p->pBuff, p->dwLenBuff);
		
		if (write(call, len))
		{
				waitPack.Init(p);
			return 0;
			
		}

	}
	result = iERROR;
	waitPack.Reset();
	mutex.Leave();


	return -1;
}
//------------------------------------------------
//
//------------------------------------------------
void NetClientThread::OnRead(SOCKET fd)
{
	if (fd == INVALID_SOCKET) return;
	
	if (mutex.TryEnter(0) || !waitPack.IsValid())
	{
		char p[1024];
		mutex.Leave();
		read(fd, p, 1024);
		return;
	}
	MArray<char> buffer;
	if (!RWThread::OnRead(fd, buffer))
		result = iERROR;
	else
	{
		FunCall* func = (FunCall*)buffer.GetPtr();
		if (!strcmp(func->name, "OK"))
			result = iOK;
		else
			if (!strcmp(func->name, "ERROR"))
				result = iERROR;
			else
				if (!strcmp(func->name, "READ") || !strcmp(func->name, "WRITE"))
				{
					Param* _param = func->param;

					bool fIsLock = !mutex.TryEnter(0);
					if (fIsLock && waitPack.pRead && func->num_param)
					{
						int lenbuff = _param->len;
						if (lenbuff)
						{
							MArray<char> buff1;
							buff1.Copy(_param->param, lenbuff);
							waitPack.pRead->pResult = (BYTE*)buff1.GetPtr();
							waitPack.pRead->dwBytesTransferred = lenbuff;
							buff1.Detach();

						}
						else
						{
							waitPack.pRead->pResult = 0;
							waitPack.pRead->dwBytesTransferred = 0;
						}

						result = iOK;
					}
					else
					{
						if (!fIsLock)
							mutex.SetLeave(1);
						result = iERROR;
					}
				}
	}
	if (result == iERROR)
		ASSERT(0);
	waitPack.Reset();
	mutex.Leave();



}


//-----------------------------------------
//
//-----------------------------------------
LRESULT NetClientThread::SendMessage(UINT msg, WPARAM wParam, LPARAM lParam, int timeout)
{
	if (!IsConnected() || !mutex.TryEnter()) return 0;
#ifndef WIN32
	if (!instance) return 0;
#endif
	Msg a(msg, wParam, lParam);
	if (msg == iTHREAD_EXIT)
	{
		AddToLog("SendMessage THREAD_EXIT");
	}
	static int try_enter_count = 0;
	send(sockMsg[1], &a, sizeof(Msg), MSG_DONTWAIT);
	if (!mutex.TryEnter(timeout))
	{
#ifndef WIN32

		if (instance)
#endif
			if (IsConnected())
			{
				AddToLog("Timeout SendMessage -> send THREAD_EXIT");

				Msg b(iTHREAD_EXIT, 0, 0);
				send(sockMsg[1], &b, sizeof(Msg), MSG_DONTWAIT);
				if (!mutex.TryEnter(15000))
					mutex.SetLeave(1);
#ifndef WIN32
				else
					self.Stop();
#else
				Close();
#endif

			}
		return 0;

	}
	mutex.SetLeave(1);
	return result == iOK;

}
//-----------------------------------------
//
//-----------------------------------------
LRESULT NetClientThread::PostMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (!this->IsConnected() || !mutex.TryEnter()) return 0;
	Msg a(msg, wParam, lParam);
	send(sockMsg[1], &a, sizeof(Msg), MSG_DONTWAIT);
	return 1;
}


int NetClientThread::Connect(SOCKET socketClient, const char* host, const char* port)
{
	struct addrinfo* result = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int ret = getaddrinfo(host, port, &hints, &result);
	if (ret) return ret;

	if (connect(socketClient, result->ai_addr, (int)result->ai_addrlen) != -1)
	{
		freeaddrinfo(result);
		return 0;
	}
	int err = errno;
	freeaddrinfo(result);
	if (err != EWOULDBLOCK && err != EINPROGRESS && err != EAGAIN)
	{
		TString sserr;
#ifdef WIN32
		OnError();
#else
		sserr.Format("socket error = %s ( %d (%d) )\n", strerror(err), err, errno);
		AddToLog(sserr.GetPtr());
#endif
		return err;
	}
	fd_set Write;
	FD_ZERO(&Write);
	FD_SET(socketClient, &Write);

	timeval	 Timeout;
	Timeout.tv_sec = 5;
	Timeout.tv_usec = 0;
#ifdef WIN32

	ret = select(0, NULL, &Write, 0, &Timeout);
#else
	ret = select(socketClient + 1, NULL, &Write, 0, &Timeout);
#endif
	if (ret == -1)
	{
		TString sserr;
#ifdef WIN32
		OnError();
#else
		err = errno;
		sserr.Format("socket error = %s ( %d)\n", strerror(err), err);
		errno = 0;
		AddToLog(sserr.GetPtr());
#endif
		return err;
	}

	if (!ret)
	{
		AddToLog("SOCKET TIMEOUT");
		return ETIMEDOUT;
	}
	// if (FD_ISSET(socketClient, &Err))
	{
        socklen_t l = sizeof(err);
		if (getsockopt(socketClient, SOL_SOCKET, SO_ERROR, (char*)& err, &l) == -1)
		{
			TString sserr;
#ifdef WIN32
			OnError();
#else
			err = errno;
			sserr.Format("socket error = %s (%d)\n", strerror(err), errno);
			AddToLog(sserr.GetPtr());
			errno = 0;
#endif
			return (int)err;
		}
		if (err)
		{
#ifdef WIN32
			OnError();
#else

			TString sserr;
			sserr.Format("SOL_SOCKET -> %s %d", strerror(err), err);
			AddToLog(sserr.GetPtr());
#endif
			return (int)err;
		}
	}

	return 0;
}

//-----------------------------------------
//
//-----------------------------------------
BOOL NetClientThread::Connect(const char* host, int _port)
{
	Close();
	if (!strlen(host)) return 0;
	strcpy_s(address, 32, host);
	port = _port;
#ifdef WIN32
	if (!strlen(address)) return 0;
	SOCKET sockfd = WSASocketA(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
	if (sockfd == INVALID_SOCKET)
	{
		TString str;
		str.Format("socket() failed: %d\n", WSAGetLastError());
		return 0;
	}

	sockaddr_in server;
	InetPtonA(AF_INET, address, &server.sin_addr.s_addr);
	server.sin_port = htons((USHORT)port);
	memset(&SendOverlapped, 0, sizeof(WSAOVERLAPPED));
	memset(&RecvOverlapped, 0, sizeof(WSAOVERLAPPED));
	TString p;
	p.Format("%d", port);
	if (!Connect(sockfd, address, p.GetPtr()))
	{
		TString s;
		s.Format("connect() failed: %d\n", WSAGetLastError());
		closesocket(sockfd);
		return 0;

	}
	RecvOverlapped.hEvent = WSACreateEvent();
	SendOverlapped.hEvent = WSACreateEvent();
	eExit = WSACreateEvent();
	eOnRead = WSACreateEvent();

	fd = sockfd;
	mutex.Enter();
	m_hThread = (HANDLE)_beginthreadex(0, 0, (unsigned(__stdcall*) (void*))MessageLoop, this, 0, &m_dwThreadID);
	mutex.Enter();
	mutex.Leave();

	return 1;


#else
	strcpy(address, host);
	if (!strlen(address)) return 0;
	int sockfd;
	int p_int = 1;


	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		TString sserr;
		int err = errno;
		sserr.Format("socket error = %s (%d)\n", strerror(err), errno);
		errno = 0;
		AddToLog(sserr.GetPtr());

		return 0;
	}
	int rcvbuf = 0;
    socklen_t len = sizeof(rcvbuf);
	int n = getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, &len);
	rcvbuf = 4048;
	n = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));
	rcvbuf = 0;
	len = sizeof(rcvbuf);
	n = getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, &len);
	p_int = 1;
	int act = TRUE;
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) == -1)
	{
		StdError(-1);
		close(sockfd);
		return FALSE;
	}
	TString p;
	p.Format("%d", port);
	if (Connect(sockfd, address, p.GetPtr()))
	{
		StdError(-1);
		close(sockfd);
		return FALSE;
	}

	fd = sockfd;

	int error, res = socketpair(AF_UNIX, SOCK_DGRAM, 0, sockMsg);
	TString sserr;
	if (res)
	{
		error = errno;
		sserr = strerror(error);
		errno = 0;
		AddToLog(sserr.GetPtr());

	}
	fcntl(sockMsg[0], F_SETFL, fcntl(sockMsg[0], F_GETFD, 0) | O_NONBLOCK);
	if (fd > 0)
	{
		mutex.SetLeave(0);
		__pthread_cleanup_push(&__cleanup, (NetClientThread::CleanUp), this);
#ifndef GLOBAL_POOL
		epfd = epoll_create(EPOOL_SIZE);
		if (epfd < 0)
		{
			StdError(-1);
			close(sockfd);
			fd = -1;
			return FALSE;
		}
#endif


	}
	return fd != -1;
#endif
}


//-----------------------------------------
//
//-----------------------------------------
BOOL NetClientThread::Start()
{
#ifndef WIN32
	self.Attach(false);
#endif
	return TRUE;
}
#ifndef WIN32
void NetClientThread::InitThread()
{
#ifdef GLOBAL_POOL

	if (epfd < 0)
	{
		epfd = epoll_create(EPOOL_SIZE);
		if (epfd < 0) return;
	}
#endif
}
void NetClientThread::ReleaseThread()
{
#ifdef GLOBAL_POOL

	if (epfd > 0)
	{
		close(epfd);

		epfd = -1;
	}
#endif
}
/*
 * Class:     com_allergoscope_app_wifi_SyncThread
 * Method:    DetachCppThread
 * Signature: ()V
 */
extern "C" JNIEXPORT void JNICALL Java_com_allergoscope_app_wifi_SyncThread_DetachCppThread
		(JNIEnv *, jobject)
{
    if (RWThread::instance)
        RWThread::instance->SendMessage(RWThread::iTHREAD_EXIT, 0, 0);

}

/*
 * Class:     com_allergoscope_app_wifi_SyncThread
 * Method:    TryConnect
 * Signature: (Ljava/lang/String;I)Z
 */
extern "C" JNIEXPORT jboolean JNICALL Java_com_allergoscope_app_wifi_SyncThread_TryConnect
        (JNIEnv *pJniEnv, jobject, jstring address, jint port)
{
    if (address == 0) return 0;
    jstring jnameobj = static_cast<jstring>(address);
    const char* jnamestr = pJniEnv->GetStringUTFChars(jnameobj, NULL);
    TString name;
    if (!RWThread::instance && jnamestr)
    {
        name =jnamestr;
        pJniEnv->ReleaseStringUTFChars(jnameobj, jnamestr);
        NetClientThread *test = new NetClientThread();
        test->InitThread();
        BOOL ret = test->Connect(name.GetPtr(), port);
        if (ret)
            RWThread::instance = test;
        else
            delete test;

    }
    return RWThread::instance != 0;
}
/*
 * Class:     com_allergoscope_app_wifi_SyncThread
 * Method:    LoopThread
 * Signature: ()V
 */
extern "C" JNIEXPORT void JNICALL Java_com_allergoscope_app_wifi_SyncThread_LoopThread
		(JNIEnv *, jobject)
{
    if (RWThread::instance)
        RWThread::instance->MessageLoop();

}
/*
 * Class:     com_allergoscope_app_wifi_SyncThread
 * Method:    Write
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
extern "C" JNIEXPORT jstring JNICALL Java_com_allergoscope_app_wifi_SyncThread_Write
		(JNIEnv *, jobject, jstring _cmd, jstring param)
{
	jstring result = 0;
	if (NetClientThread::instance)
	{

		JPtr tmp;
		tmp = _cmd;
		TString cmd = tmp.String();
		NetClientThread::PacketWrite pack;
		strcpy(pack.cmd.name, cmd.GetPtr());
		pack.dwBytesTransferred = 0;
		tmp = param;
		cmd = tmp.String();
		pack.pResult = 0;
		pack.dwLenBuff = cmd.GetLen();
		pack.pBuff = (BYTE *)cmd.GetPtr();
		if (NetClientThread::instance->SendMessage(NetClientThread::iWRITE, (WPARAM)&pack, 0))
		{
			if (pack.pResult && pack.dwBytesTransferred)
			{
				JVariable var;
				var = (LPCSTR)pack.pResult;
				result = var.str;
				var.Detach();

			}
		}
	}
	return result;
}
/*
 * Class:     com_allergoscope_app_wifi_SyncThread
 * Method:    SendFile
 * Signature: (Ljava/lang/String;Ljava/lang/String;Z)Ljava/lang/String;
 */
extern "C" JNIEXPORT jstring JNICALL Java_com_allergoscope_app_wifi_SyncThread_SendFile
        (JNIEnv *env, jobject, jstring _path, jstring path_send, jboolean fisNew)

{
    jstring result = 0;
    if (NetClientThread::instance)
    {

        JPtr tmp;
        tmp = _path;
        TString path = tmp.String();
        tmp = path_send;
        TString path2 = tmp.String();
        NetClientThread::PacketWriteFile pack;
        if (fisNew)
            strcpy(pack.cmd.name, "NEW");
        else
            strcpy(pack.cmd.name, "FILE");
        pack.FilePath = path2.GetPtr();
        path += path2;
        MFILE file(path.GetPtr());
        file.SetOpenMode(MFILE::F_READONLY);
        if (!file.Open(false)) return 0;
        MArray<char> buff(file.GetSize());
        if (file.Read(buff.GetPtr(), buff.GetLen()) != buff.GetLen()) return 0;
        file.Close();
        pack.pResult = 0;
        pack.dwBytesTransferred = 0;

        pack.dwLenBuff = buff.GetLen();
        pack.pBuff = (BYTE *)buff.GetPtr();
        if (NetClientThread::instance->SendMessage(NetClientThread::iWRITE_FILE, (WPARAM)&pack, 0))
        {
            if (pack.pResult && pack.dwBytesTransferred)
            {
                JVariable var;
                var = (LPCSTR)pack.pResult;
                result = var.str;
                var.Detach();

            }
        }
    }
    return result;
}

/*
 * Class:     com_allergoscope_app_wifi_SyncThread
 * Method:    Read
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
extern "C" JNIEXPORT jstring JNICALL Java_com_allergoscope_app_wifi_SyncThread_Read
		(JNIEnv *, jobject, jstring _cmd)
{

	jstring result = 0;
	if (NetClientThread::instance)
	{
		JPtr tmp;
		tmp = _cmd;
		TString cmd = tmp.String();
		NetClientThread::PacketWrite pack;
		strcpy(pack.cmd.name, cmd.GetPtr());
		pack.dwBytesTransferred = 0;
		pack.pResult = 0;
		if (NetClientThread::instance->SendMessage(NetClientThread::iREAD, (WPARAM)&pack, 0))
		{
			if (pack.pResult)
			{
				JVariable var;
				var = (LPCSTR)pack.pResult;
				result = var.str;
				var.Detach();

			}
		}
	}
	return result;
}
/*
 * Class:     com_allergoscope_app_wifi_SyncThread
 * Method:    GetFile
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
extern "C" JNIEXPORT jstring JNICALL Java_com_allergoscope_app_wifi_SyncThread_GetFile
		(JNIEnv *, jobject, jstring _cmd, jstring _path, jstring path_send)
{
	jstring result = 0;
	if (NetClientThread::instance)
	{

		JPtr tmp;
		TString cmd;
		tmp = _cmd;
		cmd = tmp.String();

		tmp = _path;
		TString path = tmp.String();
		tmp = path_send;
		TString path2 = tmp.String();
		NetClientThread::PacketWriteFile pack;

		strcpy(pack.cmd.name, cmd.GetPtr());
		path += path2;
		MFILE file(path.GetPtr());
		file.SetOpenMode(MFILE::F_READONLY);
		if (file.Open(false))
		{
			JVariable var;
			var = "OK";
			result = var.str;
			var.Detach();
			file.Close();
			return result;
		}
		if (cmd == "ROI")
			path2.Replace(".roi", ".bin");
		pack.FilePath = path2.GetPtr();

		pack.pResult = 0;
		pack.dwBytesTransferred = 0;
		if (NetClientThread::instance->SendMessage(NetClientThread::iFILE, (WPARAM)&pack, 0))
		{
			if (pack.pResult && pack.dwBytesTransferred)
			{
				JVariable var;
				if (pack.dwBytesTransferred > 4)
				{
					MFILE file(path.GetPtr());

					if (file.Open(true))
					{
						file.Write(pack.pResult, pack.dwBytesTransferred);
						file.Close();
						var = "OK";
					}
					else
					{
						var = "ERROR";
					}
				}
				else
					var = (LPCSTR)pack.pResult;
				result = var.str;
				var.Detach();

			}
		}
	}
	return result;
}

/*
 * Class:     com_allergoscope_app_wifi_SyncThread
 * Method:    GetROI
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
extern "C" JNIEXPORT jstring JNICALL Java_com_allergoscope_app_wifi_SyncThread_GetROI
		(JNIEnv *env, jobject, jstring _path, jstring path_send)
{
	jstring result = 0;
	if (NetClientThread::instance)
	{

		JPtr tmp;
		tmp = _path;
		TString path = tmp.String();
		tmp = path_send;
		TString path2 = tmp.String();
		NetClientThread::PacketWriteFile pack;
		strcpy(pack.cmd.name, "ROI");
		path += path2;
		MFILE file(path.GetPtr());
		file.SetOpenMode(MFILE::F_READONLY);
		if (file.Open(false))
		{
			JVariable var;
			var = "OK";
			result = var.str;
			var.Detach();
            file.Close();
            return result;
		}

		path2.Replace(".roi", ".bin");
        pack.FilePath = path2.GetPtr();

        pack.pResult = 0;
		pack.dwBytesTransferred = 0;
		if (NetClientThread::instance->SendMessage(NetClientThread::iGETROI, (WPARAM)&pack, 0))
		{
			if (pack.pResult && pack.dwBytesTransferred)
			{
				JVariable var;
                if (pack.dwBytesTransferred > 4)
                {
                    MFILE file(path.GetPtr());

                    if (file.Open(true))
                    {
                        file.Write(pack.pResult, pack.dwBytesTransferred);
                        file.Close();
                        var = "OK";
                    }
                    else
                    {
                        var = "ERROR";
                    }
                }
                else
    				var = (LPCSTR)pack.pResult;
				result = var.str;
				var.Detach();

			}
		}
	}
	return result;
}
#pragma pack(push)
#pragma pack(2)
struct RespHeader {
	USHORT ID;
	USHORT Flags;
	USHORT QuestionCount;
	USHORT AnswerCount;
	USHORT NameServerRecords;
	USHORT AdditionalRecords;
	void Init(int nRec)
	{
		ID = 0;
		Flags = htons(0x84);
		QuestionCount = 0;
		AnswerCount = 0;
		NameServerRecords = 0;
		AdditionalRecords = htons(nRec);
	}
	void Read(char *d)
	{
		memcpy(&ID, d, sizeof(USHORT));d += sizeof(USHORT);
		memcpy(&Flags, d, sizeof(USHORT));d += sizeof(USHORT);
		memcpy(&QuestionCount, d, sizeof(USHORT));d += sizeof(USHORT);
        memcpy(&AnswerCount, d, sizeof(USHORT));d += sizeof(USHORT);
		memcpy(&NameServerRecords, d, sizeof(USHORT));d += sizeof(USHORT);
		memcpy(&AdditionalRecords, d, sizeof(USHORT));d += sizeof(USHORT);
		QuestionCount = htons(QuestionCount);
		AnswerCount = htons(AnswerCount);
		NameServerRecords = htons(NameServerRecords);
		AdditionalRecords = htons(AdditionalRecords);
	}

};
struct  MdnsTextRecord {

	USHORT Type;
	int Class;
	int TTL;
	USHORT len;

	void Read(char *d)
	{
		memcpy(&Type, d, sizeof(USHORT)); d += sizeof(USHORT);
		memcpy(&Class, d, sizeof(int));d += sizeof(int);
		memcpy(&TTL, d, sizeof(int));d += sizeof(int);
		memcpy(&len, d, sizeof(USHORT));d += sizeof(USHORT);
		len = htons(len);
	}

};
#pragma pack(pop)
/*
 * Class:     com_allergoscope_app_wifi_SyncThread
 * Method:    ReadMdnsRecords
 * Signature: ([BI)[Ljava/lang/String;
 */
extern "C" JNIEXPORT jobjectArray JNICALL Java_com_allergoscope_app_wifi_SyncThread_ReadMdnsRecords
        (JNIEnv *env, jclass, jbyteArray arr, jint size)
{
    jobjectArray result = 0;
	if (arr)
	{
        long len = min(size, env->GetArrayLength(arr));
        if (MInstance::ClearException(env)) return 0;
		if (len)
		{
		    MVector<TString> sarr;
			char *data = (char *)env->GetByteArrayElements(arr, 0);
            if (MInstance::ClearException(env)) return 0;
			if (data)
			{
                char *rec = data;
				RespHeader head;
				head.Read(data);
				int nRec = head.AdditionalRecords;
                long pos = 0;
                TString str;
                head.Read(rec);
                rec += sizeof(RespHeader);
                while (*rec)
                {

                    char l = *rec;
                    if (l > 0)
                    {
                        ++rec;


                        str.Copy(rec, l);
                        str.Add() = 0;
                        if (str.StrLen())
                        sarr.Add(str);
                        rec = rec + l;

                    }
                    else
                        break;


                }
				while (nRec--)
				{
                    pos = (rec - data);
            		MdnsTextRecord record;
					char *res = 0;
					record.Read(rec);

                    if (pos + sizeof(MdnsTextRecord) + record.len > len)
                        break;
                    rec += sizeof(MdnsTextRecord);

                    if (record.Type == 0x1000)
                    {
                        res = (char *)malloc(record.len + 1);
                        memcpy(res, rec, record.len);
                        res[record.len] = 0;
                            sarr.Add() = res;
                        if (res)
                            free(res);
                    }
                    rec += record.len;
				}
			}
			env->ReleaseByteArrayElements(arr, (jbyte *)data, 0);
            if (MInstance::ClearException(env)) return 0;

            if ((len = sarr.GetLen()))
            {

                JPtr tp = env->FindClass("java/lang/String");
                if (MInstance::ClearException(env)) return 0;
                JPtr jarr = env->NewObjectArray(len,tp.cls,0);
                if (MInstance::ClearException(env)) return 0;
                for (int i = 0; i < len; ++i)
                {
                    JPtr s1 = env->NewStringUTF(sarr[i].GetPtr());
                    env->SetObjectArrayElement(jarr,i,s1);
                    if (MInstance::ClearException(env)) return 0;
                }
                result = jarr.arr;
                jarr.Detach();
            }
        }
	}

	return result;
}


#endif
