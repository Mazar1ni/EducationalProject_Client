#pragma once
#include "INetwork.h"

class UnixClient : public INetwork
{
public:
	UnixClient();
	~UnixClient();
	void clear();
};

