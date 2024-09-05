#pragma once

// MDNS command target
#include "MDNSStruct.h"
using namespace MDNSStruct;
class MDNS
{

	
	
public:
	MDNS();
	virtual ~MDNS();
	bool Start();
	bool End();
	bool Write(SOCKADDR_IN &m_saHostGroup, MVector<WString>& msg, UString& num_connection, WString& num_changes);
	MArray<SOCKET> m_hSocket;

	


};


