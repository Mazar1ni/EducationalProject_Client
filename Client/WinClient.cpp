#include "WinClient.h"

WinClient::WinClient()
{
	WSAData WSData;

	// initialize the use of the Winsock DLL process
	if (WSAStartup(MAKEWORD(2, 2), &WSData) != 0)
	{
		Log::print(Log::error, "main - WSASratup no succes");
		systemMessage("The library supplied with this software does not support this version of the socket window.\n"
			"Possible cause : incorrect software installation or loss of required files");
		cin.get();
		delete this;
		return;
	}

	createSocket();
}

WinClient::~WinClient()
{
	closesocket(Socket);
}

void WinClient::clear()
{
	system("cls");
}

