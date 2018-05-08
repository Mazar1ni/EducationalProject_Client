#include "Client.h"

using namespace std;

#if defined(_WIN64) || defined(_WIN32)

void clear()
{
	system("cls");
}

#else

void clear()
{
	cout << "\x1B[2J\x1B[H";
}

#endif

void systemMessage(string message)
{
	// clear screen and write message
	clear();
	cout << message << endl;
}

int main(int argc, char** argv)
{
	if (argc > 1)
	{
		if (strcmp(argv[1], "debug") == 0)
			Log::setPriority(Log::debug);
		else if (strcmp(argv[1], "info") == 0)
			Log::setPriority(Log::info);
		else if (strcmp(argv[1], "warning") == 0)
			Log::setPriority(Log::warning);
		else if (strcmp(argv[1], "error") == 0)
			Log::setPriority(Log::error);
	}

#if defined(_WIN64) || defined(_WIN32)

	WSAData WSData;

	// initialize the use of the Winsock DLL process
	if (WSAStartup(MAKEWORD(2, 2), &WSData) != 0)
	{
		Log::print(Log::error, "main - WSASratup no succes");
		systemMessage("The library supplied with this software does not support this version of the socket window.\n"
			"Possible cause : incorrect software installation or loss of required files");
		cin.get();
		return -1;
	}

#endif

	// socket 
	// семейство адресов (IPv4 / IPv6 / (IPX/SPX) / NetBIOS / AppleTalk / Bluetooth etc)
	// тип сокета (tcp / udp etc)
	// организация заголовков (tcp / udp / icmp / igmp etc)

	// create socket
	int Socket = socket(AF_INET, SOCK_STREAM, 6);

	if (Socket < 0)
	{
		Log::print(Log::error, "main - Socket not created");
		systemMessage("Socket was not created, please try again after a while.");
		cin.get();
		return -1;
	}

	// create struct for bind to her socket
	struct sockaddr_in SA;
	SA.sin_family = AF_INET;
	SA.sin_port = htons(7070);
	SA.sin_addr.s_addr = inet_addr("127.0.0.1");

	// connect 
	// socket
	// указатель на структуру, к которой должно быть установлено соединение (семейство адресов, порт, ip)
	// длина структуры

	// connection socket and struct
	if (connect(Socket, (struct sockaddr*)&SA, sizeof(SA)) != 0)
	{
		Log::print(Log::error, "main - Socket succed connected");
		systemMessage("The server does not respond, please try again after a while.");
		cin.get();
		return -1;
	}

	Client* client = new Client(Socket);

	return 0;
}