#include "stdafx.h"
#include "MDNSReader.h"
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi")
int OnError();


MDNSReader::MDNSReader(HWND hWnd, int message, LPCSTR address, int port)
{
	mThread = 0;
	data = new SockData();
	data->hWnd = hWnd;
	data->message = message;
	data->addr_srv = address;

	if (GetNetworkIP(data->addr, port))
	{
		mThread = AfxBeginThread(SockFunc, data);
		mThread->m_bAutoDelete = 0;
	}



}
BOOL MDNSReader::Receive(SendData &res)
{
	bool b = false;
	data->sec.Enter();
	if (data->_in.Len())
	{
		res = data->_in.PopFront();
		b = true;
	}
	data->sec.Leave();

	return b;
}

void MDNSReader::Stop()
{
	if (data)
	{
		if (mThread)
		{
			DWORD dwRetCode = 0;
			GetExitCodeThread(mThread->m_hThread, &dwRetCode);
			if (dwRetCode == STILL_ACTIVE)
				SetEvent(data->hExitEvent);
			if (WaitForSingleObject(mThread->m_hThread, 1000) != WAIT_OBJECT_0)
			{
				dwRetCode = 0;
				GetExitCodeThread(mThread->m_hThread, &dwRetCode);
				if (dwRetCode == STILL_ACTIVE)
					TerminateThread(mThread->m_hThread, 1);
			}
			delete mThread;
			mThread = 0;

		}
		delete data;
		data = 0;

	}
}

MDNSReader::~MDNSReader()
{
	Stop();

}
bool MDNSReader::GetNetworkIP(MArray<sockaddr_in>& address, int port)
{

	MArray<char> buff(15000);
	IP_ADAPTER_ADDRESSES* pAddresses = (IP_ADAPTER_ADDRESSES*)buff.GetPtr();
	DWORD dwRetVal;
	ULONG outBufLen = 0;
	int i;
	bool fRet = false;
	for (i = 0; i < 3; ++i)
	{
		buff.Set(0);
		dwRetVal = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_ANYCAST, NULL, pAddresses, &outBufLen);
		if (dwRetVal == ERROR_BUFFER_OVERFLOW)
		{
			buff.SetLen(outBufLen);
			pAddresses = (IP_ADAPTER_ADDRESSES*)buff.GetPtr();
			continue;
		}
		break;
	}
	if (dwRetVal != NO_ERROR) return false;
	PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;

	while (pCurrAddresses)
	{
		if (pCurrAddresses->FirstDnsServerAddress && pCurrAddresses->Dhcpv4Enabled && pCurrAddresses->TunnelType != TUNNEL_TYPE_TEREDO && IfOperStatusUp == pCurrAddresses->OperStatus &&
			((IP_ADAPTER_NO_MULTICAST & pCurrAddresses->Flags) == 0))
		{
			IP_ADAPTER_UNICAST_ADDRESS* pUnicast = pCurrAddresses->FirstUnicastAddress;
			if (pUnicast != NULL)
			{

				for (i = 0; pUnicast != NULL; i++)
				{
					if (pUnicast->Address.lpSockaddr->sa_family == AF_INET)
					{
						sockaddr_in* sa_in = (sockaddr_in*)pUnicast->Address.lpSockaddr;
						//	char host[NI_MAXHOST] = { 0 };
							//char service[NI_MAXSERV] = { 0 };
							//const int ret = getnameinfo((const struct sockaddr*)sa_in, sizeof(struct sockaddr_in), host, NI_MAXHOST, service, NI_MAXSERV,
								//NI_NUMERICSERV | NI_NUMERICHOST);
						sockaddr_in &saddr = address.Add();
						memcpy(&saddr, sa_in, sizeof(sockaddr_in));
						saddr.sin_port = htons((unsigned short)port);
						fRet = true;

					}
					pUnicast = pUnicast->Next;
				}
			}
		}
		pCurrAddresses = pCurrAddresses->Next;
	}
	sockaddr_in &saddr = address.Add();
	memset(&saddr, 0, sizeof(sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_addr = in4addr_any;
	saddr.sin_port = htons(port);
	return fRet;
}
//--------------------------------
//
//--------------------------------
bool MDNSReader::IsValid()
{
	if (mThread && data)
	{
		DWORD dwRetCode = 0;
		GetExitCodeThread(mThread->m_hThread, &dwRetCode);
		return dwRetCode == STILL_ACTIVE;

	}
	return false;
}

SOCKET MDNSReader::RestartSocket(SOCKET hSock, sockaddr_in* in, LPCSTR address_srv)
{
	struct ip_mreq req;
	memset(&req, 0, sizeof(req));
	ULONG saddr = 0;
	InetPtonA(AF_INET, address_srv, &saddr);
	req.imr_multiaddr.s_addr = saddr;
	req.imr_interface = in->sin_addr;

	if (hSock != INVALID_SOCKET)
	{
		setsockopt(hSock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&req, sizeof(req));
		closesocket(hSock);
	}
	if ((hSock = WSASocket(PF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		TRACE(L"socket() failed with error %d", WSAGetLastError());

		closesocket(hSock);
		return INVALID_SOCKET;
	}
	unsigned int reuseaddr = 1;
	setsockopt(hSock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseaddr, sizeof(reuseaddr));
	int val = 0x8000;
	unsigned char ttl = 7;
	unsigned char loopback = 1;

	setsockopt(hSock, SOL_SOCKET, SO_RCVBUF, (char*)&val, sizeof(int));
	setsockopt(hSock, IPPROTO_IP, IP_MULTICAST_TTL, (const char*)&ttl, sizeof(ttl));
	setsockopt(hSock, IPPROTO_IP, IP_MULTICAST_LOOP, (const char*)&loopback, sizeof(loopback));


	if (bind(hSock, (PSOCKADDR)in, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		TRACE(L"bind() failed with error %d", WSAGetLastError());

		closesocket(hSock);
		hSock = INVALID_SOCKET;
		return hSock;
	}
	setsockopt(hSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&req, sizeof(req));

	return hSock;


}

int MDNSReader::Read(SOCKET fd, void* buff, int len, SOCKADDR_IN* addr)
{
	WSAOVERLAPPED RecvOverlapped;
	memset(&RecvOverlapped, 0, sizeof(WSAOVERLAPPED));
	int s_size = sizeof(SOCKADDR_IN);

	RecvOverlapped.hEvent = WSACreateEvent();
	WSABUF DataBuf;
	DWORD nBytesRead = 0, Flags = 0;
	int rc;
	DataBuf.len = len;
	DataBuf.buf = (char*)buff;
	Flags = 0;
	rc = WSARecvFrom(fd, &DataBuf, 1, &nBytesRead, &Flags, (SOCKADDR*)addr, &s_size, &RecvOverlapped, NULL);
	if (rc == SOCKET_ERROR)
	{

		if (WSA_IO_PENDING == WSAGetLastError())
			WSASetLastError(0);
		else
		{
			OnError();
			goto wait_exit;
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
			nBytesRead = 0;
			break;
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
		}
		break;
	}
wait_exit:
	WSAResetEvent(RecvOverlapped.hEvent);
	WSACloseEvent(RecvOverlapped.hEvent);
	return nBytesRead;
}
UINT AFX_CDECL MDNSReader::SockFunc(void* h)
{

	SockData* hs = (SockData*)h;
	HWND hWnd = hs->hWnd;
	MArray<WSAEVENT> handles;
	int i, l = hs->addr.GetLen();
	for (i = 0; i < l; ++i)
	{
		SOCKET sock = RestartSocket(0, &hs->addr[i], hs->addr_srv.GetPtr());
		if (sock != INVALID_SOCKET)
		{
			hs->hSock.Add() = sock;
			handles.Add() = WSACreateEvent();
		}
		else
		{
			hs->addr.Remove(i, 1);
			--l;
			--i;
		}

	}
	BOOL fExit = 0;
	if (!hs->hSock.GetLen()) return 0;
	handles.Add() = hs->hExitEvent;
	for (i = 0; i < l; ++i)
	{
		WSAResetEvent(handles[i]);
		WSAEventSelect(hs->hSock[i], handles[i], FD_READ);
	}
	ResetEvent(handles[i]);
	while (1)
	{
		i = WSAWaitForMultipleEvents(handles.GetLen(), handles.GetPtr(), FALSE, INFINITE, 0);
		if (i >= 0 && i < hs->hSock.GetLen())
		{
			WSAResetEvent(handles[i]);
			SendData data;
			WSANETWORKEVENTS NetworkEvents;
			int Recved = 0;
			if (WSAEnumNetworkEvents(hs->hSock[i], handles[i], &NetworkEvents) != SOCKET_ERROR && NetworkEvents.lNetworkEvents & FD_READ)
			{

				data.rsend.SetLen(0x10000);
				Recved = Read(hs->hSock[i], data.rsend.GetPtr(), 0xFFFF, &data.addr);
			}

			if (!Recved)
			{
				SOCKET sock = RestartSocket(hs->hSock[i], &hs->addr[i], hs->addr_srv.GetPtr());
				if (sock != INVALID_SOCKET)
					hs->hSock[i] = sock;
				else
				{
					hs->hSock.Remove(i, 1);
					hs->addr.Remove(i, 1);
					handles.Remove(i, 1);
					continue;
				}
			}
			else
			{
				bool fOk = false;
				for (int i = 0, l = data.rsend.GetLen(); i < l; ++i)
				{
					if (!memcmp(data.rsend.GetPtr() + i, "dryeye.client", 12))
					{
						fOk = true;
						break;
					}
				}
				if (fOk)
				{
					hs->sec.Enter();
					data.rsend.SetLen(Recved);
					hs->_in.PushBack(data);
					hs->sec.Leave();
					PostMessage(hs->hWnd, hs->message, 0, FD_READ);
				}
			}

			WSAEventSelect(hs->hSock[i], handles[i], FD_READ);
		}
		else
		{


			if (!(i - hs->hSock.GetLen()))
			{
				ResetEvent(handles[i]);

				break;
			}

		}

	}
	hs->sec.Enter();

	l = hs->addr.GetLen();
	struct ip_mreq req;
	memset(&req, 0, sizeof(req));
	ULONG saddr = 0;
	InetPtonA(AF_INET, hs->addr_srv.GetPtr(), &saddr);
	req.imr_multiaddr.s_addr = saddr;

	for (int i = 0; i < l; ++i)
	{
		WSACloseEvent(handles[i]);
		req.imr_interface = hs->addr[i].sin_addr;
		setsockopt(hs->hSock[i], IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&req, sizeof(req));
		closesocket(hs->hSock[i]);

	}
	hs->_in.Clear();
	hs->sec.Leave();
	

	return 0;
}
