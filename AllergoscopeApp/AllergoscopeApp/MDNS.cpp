// MDNS.cpp : implementation file
//

#include "stdafx.h"
#include "AllergoscopeApp.h"
#include "MDNS.h"
#include "_ffile.h"
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include "inaddr.h"

#pragma comment(lib, "iphlpapi")


int OnError();

MDNS::MDNS()
{
	m_hSocket = 0;
}

MDNS::~MDNS()
{
	End();
}
bool GetNetworkIP(MArray<IN_ADDR>& address)
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
		dwRetVal = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_ANYCAST,  NULL, pAddresses, &outBufLen);
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
							char host[NI_MAXHOST] = { 0 };
							char service[NI_MAXSERV] = { 0 };
							const int ret = getnameinfo((const struct sockaddr*)sa_in, sizeof(struct sockaddr_in), host, NI_MAXHOST, service, NI_MAXSERV,
								NI_NUMERICSERV | NI_NUMERICHOST);

							address.Add() = sa_in->sin_addr;
							fRet = true;

						}
						pUnicast = pUnicast->Next;
					}
			}
		}
		pCurrAddresses = pCurrAddresses->Next;
	}
	return fRet;
}
bool MDNS::Start()
{
	MArray<IN_ADDR> address;
	GetNetworkIP(address);
	BOOL fRes = FALSE;
	for(int i = 0; i < address.GetLen(); ++i)
	{
		SOCKET hSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (hSocket != INVALID_SOCKET)
		{
			int yes = 1;
			setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int));
			int val = 0x8000;
			setsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&val, sizeof(int));
			SOCKADDR_IN addr;
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_addr = address[i];
			
			if (bind(hSocket, (sockaddr*)&addr, sizeof(addr)) != SOCKET_ERROR)
			{
				m_hSocket.Add() = hSocket;

				fRes = TRUE;
			}
		}
	}
	if (!fRes)
		OnError();

	return fRes;

}
bool MDNS::End()
{
	for(int i = 0; i <m_hSocket.GetLen(); ++i)
	closesocket(m_hSocket[i]);
	m_hSocket.Clear();
	return true;
}

bool MDNS::Write(SOCKADDR_IN& m_saHostGroup, MVector<WString> &msg, UString& num, WString &num_changes)
{
	UString domain, local = "local";
	domain.SetLen(255);
	gethostname(domain.GetPtr(), 255);
	MArray<char> buff;
	buff.SetLen(sizeof(RespHeader) + 1);
	((RespHeader*)(buff.GetPtr()))->Init(2 + msg.GetLen() + (num_changes.StrLen() > 0));
	buff[sizeof(RespHeader)] = domain.StrLen();
	buff.Append(domain.GetPtr(), domain.StrLen());
	buff.Add() = local.StrLen();
	buff.Append(local.GetPtr(), local.StrLen());
	AddRecord(buff, num.GetPtr());
	AddRecord(buff, "com.allergoscope.app");
	if (num_changes.StrLen())
		AddRecord(buff, (BYTE *)num_changes.GetPtr(), 2 * (num_changes.StrLen() + 1));

	if (msg.GetLen())
	{
		for (int i = 0; i < msg.GetLen(); ++i)
		{
			AddRecord(buff, (BYTE *)msg[i].GetPtr(), (msg[i].StrLen () + 1)*2);
		}
	}
	for (int i = 0; i < m_hSocket.GetLen(); ++i)
	{
		if (sendto(m_hSocket[i], buff.GetPtr(), buff.GetLen(), 0, (SOCKADDR*)&m_saHostGroup, sizeof(SOCKADDR)) != buff.GetLen())
		{
			OnError();
			return false;
		}
	}
	return true;
}


