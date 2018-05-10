#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Log.h"
#include "INetwork.h"

#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

using namespace std;

class WinClient : public INetwork
{
public:
	WinClient();
	~WinClient();
	void clear();
};

