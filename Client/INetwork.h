#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <time.h>
#include <regex>
#include <future>

using namespace std;

class INetwork
{
public:
	INetwork();
	virtual ~INetwork();
	virtual void clear() = 0;

	static INetwork* getNetwork();

	void systemMessage(string message);
	bool checkSyntax(string identificator, string message);
	void readMessages();
	void sendMessage();
	bool connected();
	void createSocket();

	int Socket;

	mutex mtx;
	bool isConnected = false;
	bool quit = false;
};

