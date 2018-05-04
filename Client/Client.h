#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <time.h>
#include <regex>
#include <future>
#include "Log.h"

#if defined(__MACH__) || defined(__APPLE__)
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#if defined(_WIN64) || defined(_WIN32)
#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")
#endif

using namespace std;

class Client
{
public:
	Client(int s, Log& l);
	~Client();
	void clear();
	bool connected();
	void systemMessage(string message);
	void readMessages();
	bool checkSyntax(string identificator, string message);
	void sendMessage();

private:
	int socket;
	Log& log;

	mutex mtx;
	bool isConnected = false;
};

