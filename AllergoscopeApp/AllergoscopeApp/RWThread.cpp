#include "stdafx.h"
#include "RWThread.h"
#include "mstring.h"
#ifndef WIN32
	RWThread* RWThread::instance = 0;
	#ifndef EPOLLRDHUP
	#define EPOLLRDHUP 0x2000
#endif
#else
	#include <process.h>
	#define MSG_DONTWAIT 1
#endif
void AddToLog(const UString& t);

void trace(const char *pFormat, ...)
{
	int Len = 2048;
	char *ptr = (char *)malloc(Len);
	va_list args;
	va_start(args, pFormat);
#ifdef __PLUK_LINUX__
	vsprintf(ptr, pFormat, args);
#else
	vsprintf_s(ptr, Len, pFormat, args);
#endif
	va_end(args);
	DINT n;
	if ((n = (DINT)strlen(ptr)))
		ptr[n] = 0;
	AddToLog(ptr);
	free(ptr);
	
}
#ifndef WIN32
int OnError()
{

	int err = errno;
	printf("%s\r\n", strerror(err));
	errno = 0;
	return err;
}
void AddToLog(LPCSTR t)
{
    TRACE(t);
}

#else

int OnError();
#endif
#ifdef GLOBAL_POOL
SOCKET RWThread::epfd = INVALID_SOCKET;
#endif
//-----------------------------------------
//
//-----------------------------------------
#ifdef WIN32
RWThread::RWThread() :critical(1), mutex(1)
{
    eExit = 0;
    eOnRead = 0;
    memset(&SendOverlapped, 0, sizeof(WSAOVERLAPPED));
    memset(&RecvOverlapped, 0, sizeof(WSAOVERLAPPED));
#else
RWThread::RWThread()
{
#ifndef GLOBAL_POOL
    epfd = INVALID_SOCKET;
#endif
#endif
	sockMsg[0] = sockMsg[1] = INVALID_SOCKET;
	fd = INVALID_SOCKET;
	address[0] = 0;
}
//-----------------------------------------
//
//-----------------------------------------
RWThread::~RWThread()
{
	Close();
}
#ifdef WIN32

//-----------------------------------------
//
//-----------------------------------------
int RWThread::write(void* b, int size)
{
	if (fd == INVALID_SOCKET) return -1;
	WSABUF DataBuf;
	DWORD SendBytes = 0, Flags = 0;
	int rc;
	DataBuf.len = size;
	DataBuf.buf = (char*)b;
	Flags = 0;
	rc = WSASend(fd, &DataBuf, 1, &SendBytes, 0, &SendOverlapped, NULL);
	if (rc == SOCKET_ERROR)
	{
		if (WSA_IO_PENDING == WSAGetLastError())
			WSASetLastError(0);
		else
		{
			OnError();
			return 0;
		}
	}
	DWORD dw = WSAGetLastError();
	do
	{
		rc = WSAWaitForMultipleEvents(1, &SendOverlapped.hEvent, TRUE, INFINITE, TRUE);
		if (rc == SOCKET_ERROR)
		{
			if (WSA_IO_PENDING == WSAGetLastError())
			{
				WSASetLastError(0);
				continue;
			}
			OnError();
			return 0;
		}
		if (rc == WSA_WAIT_IO_COMPLETION)
			continue;

		rc = WSAGetOverlappedResult(fd, &SendOverlapped, &SendBytes, FALSE, &Flags);
		if (rc == FALSE)
		{
			if (WSA_IO_PENDING == WSAGetLastError())
			{
				WSASetLastError(0);
				continue;
			}

			OnError();
			return 0;
		}

	} while (size > (int)SendBytes);
	WSAResetEvent(SendOverlapped.hEvent);
	return SendBytes;

}
#else
//-----------------------------------------
//
//-----------------------------------------
int RWThread::write(void* _p, int size)
{

	if (fd == INVALID_SOCKET) return 0;
    int timeout = 100;
    char *p = (char *)_p;

    while (size > 0)
    {
        int n = send(fd, p, size, 0 );
        if (n == -1)
        {
            if ( errno == EAGAIN || errno == EWOULDBLOCK)
            {
                fd_set Write;
                FD_ZERO(&Write);
                FD_SET(fd, &Write);

                timeval	 Timeout;
                Timeout.tv_sec = 10;
                Timeout.tv_usec = 0;

                if (select(fd + 1, NULL, &Write, 0, &Timeout) >= 0)
                {
                    if(!FD_ISSET(fd, &Write))
                    {
                        if(--timeout < 0)
                            break;
                    }
                    else
                        timeout = 100;
                    continue;
                }
            }
            else
            if(errno == EINTR)
                continue;
            break;
        }
        p += n;
        size -= n;
    }
    if (size > 0)
        OnError();
    return size == 0;

}

#endif

//------------------------------------------------
//
//------------------------------------------------
bool RWThread::IsName(char* name)
{
	int n = 0;
	return !memcmp("WRITE", name, 5) || !memcmp("READ", name, 4);
/*	while ((*name >= 'A' && *name <= 'Z') && n < 8)
	{
		++n;
		++name;
	}
	return n >= 2 && n < 8;
	*/
}
#ifdef WIN32
int RWThread::read(SOCKET fd, void* buff, int len)
{
	if (fd == INVALID_SOCKET) return -1;
	WSABUF DataBuf;
	DWORD nBytesRead = 0, Flags = 0;
	int rc;
	DataBuf.len = len;
	DataBuf.buf = (char*)buff;
	Flags = 0;
	rc = WSARecv(fd, &DataBuf, 1, &nBytesRead, &Flags, &RecvOverlapped, NULL);
	if (rc == SOCKET_ERROR)
	{

		if (WSA_IO_PENDING == WSAGetLastError())
			WSASetLastError(0);
		else
		{
			OnError();
			return 0;
		}
	}
	while (1)
	{
		rc = WSAWaitForMultipleEvents(1, &RecvOverlapped.hEvent, TRUE, INFINITE, TRUE);
		WSAResetEvent(RecvOverlapped.hEvent);

		if (rc == SOCKET_ERROR)
		{
			DWORD err = WSAGetLastError();
			if (WSA_IO_PENDING == err)
			{
				WSASetLastError(0);
				continue;
			}
			OnError();
			return 0;
		}
		if (rc == WSA_WAIT_IO_COMPLETION)
			continue;

		wait_overlap:
		rc = WSAGetOverlappedResult(fd, &RecvOverlapped, &nBytesRead, FALSE, &Flags);
		if (!rc)
		{
			DWORD err = WSAGetLastError();
			if (WSA_IO_PENDING == err)
			{
				WSASetLastError(0);
				continue;
			}
			if (err == WSA_IO_INCOMPLETE)
				goto wait_overlap;
			OnError();
			nBytesRead = 0;
			break;
		}

		//	if (!nBytesRead || nBytesRead == len)
		break;
	}
	WSAResetEvent(RecvOverlapped.hEvent);
	return nBytesRead;
}
#endif
//------------------------------------------------
//
//------------------------------------------------
bool RWThread::OnRead(SOCKET fd, MArray<char> &buffer)
{
	if (fd == INVALID_SOCKET) return false;
	int nBytesRead = 0;
	buffer.SetLen(2048);
	char* pB = buffer.GetPtr();
	int sz = 1024;
	int fNoErr = 0;
	FunCall* func = 0;
	int receive = 0;
	bool result = false;
	while (sz > 0)
	{
		if ((nBytesRead = read(fd, pB, sz)) <= 0)
		{
#ifndef WIN32
			int err = errno;
			if (err == EAGAIN)
			{
				fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) & ~O_NONBLOCK);
				nBytesRead = recv(fd, pB, sz, 0);
				fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);

			}
			if (nBytesRead < 0)
			{
				
				break;
			}
#else
			
			break;
#endif
		}
		func = (FunCall*)buffer.GetPtr();
		if (!receive)
		while (nBytesRead > sizeof(func->name) && !IsName(func->name))
		{
			--nBytesRead;
			
			memmove(buffer.GetPtr(), buffer.GetPtr() + 1, nBytesRead);
		}
		receive += nBytesRead;

		if ((fNoErr = (receive >= sizeof(FunCall) && IsName(func->name) && func->len == receive)))
		{
			result = true;
			break;

		}
		else
		{
			if (receive < sizeof(FunCall))
			{
				pB += nBytesRead;
				sz -= nBytesRead;
				continue;
			}
			else
				if (receive > 8 && IsName(func->name))
				{
					if (buffer.GetLen() <= func->len)
					{
						buffer.SetLen(func->len + 1024);
						func = (FunCall*)buffer.GetPtr();
						sz = func->len - receive;
						pB = buffer.GetPtr() + receive;
					}
					else
					{
						sz = func->len - receive;
						if (sz < 0)
							return 0;
						pB = buffer.GetPtr() + receive;
					
					}
					continue;
				}
		}
		if (!fNoErr)
		{
			
			break;
		}
		
	}
	return result;

}


#ifdef WIN32
BOOL RWThread::Close()
{
	if (fd != INVALID_SOCKET)
		closesocket(fd);
	fd = INVALID_SOCKET;
	thread.Stop();
	if (SendOverlapped.hEvent)
		WSACloseEvent(SendOverlapped.hEvent);
	if (RecvOverlapped.hEvent)
		WSACloseEvent(RecvOverlapped.hEvent);
	if (eOnRead)
		WSACloseEvent(eOnRead);
	fd = INVALID_SOCKET;
	memset(&SendOverlapped, 0, sizeof(WSAOVERLAPPED));
	memset(&RecvOverlapped, 0, sizeof(WSAOVERLAPPED));
	
	address[0] = 0;
	eExit = eOnRead = 0;
	return 1;
}
#else
//-----------------------------------------
//
//-----------------------------------------
BOOL RWThread::Close()
{
	mutex.SetLeave(0);
	struct stat st;
	if (epfd != INVALID_SOCKET) // || fstat((int)epfd, &st ) == 0)
	{
		if (sockMsg[0] != INVALID_SOCKET)
		{
			epoll_ctl(epfd, EPOLL_CTL_DEL, sockMsg[0], 0);
		}
		if (fd != -1)
			epoll_ctl(epfd, EPOLL_CTL_DEL, fd, 0);
#ifndef GLOBAL_POOL
		close(epfd);
		epfd = -1;

#endif

	}
	if (sockMsg[1] != INVALID_SOCKET)
	{
		close(sockMsg[1]);
		close(sockMsg[0]);
	}
	sockMsg[1] = -1;
	sockMsg[0] = -1;
	if (fd != INVALID_SOCKET) close(fd);
	fd = -1;
	return 1; 
}
#endif
//-----------------------------------------
//
//-----------------------------------------
BOOL RWThread::IsConnected()
{
#ifdef WIN32
	return fd != INVALID_SOCKET && thread.IsRun();
#else
	struct stat st;
	return fd != -1 && !fstat((int)fd, &st);
#endif

}
//-----------------------------------------
//
//-----------------------------------------
void RWThread::CleanUp(void* p)
{

#ifndef WIN32

	if (instance)
	{
		instance = 0;
		RWThread* th = (RWThread*)p;
		th->Close();
		th->self.Detach();
		delete th;
	}
#endif
}
