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

mutex mtx;
bool isConnected = false;

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

bool connected()
{
	int count = 0;
	// check if the connection to the server is established
	while (true)
	{
		mtx.lock();
		// connection successfully or time out
		if (isConnected == true || count > 75)
		{
			mtx.unlock();
			return isConnected;
		}
		else
		{
			count++;
		}
		mtx.unlock();
		//Sleep(200);
	}
}

void systemMessage(string message)
{
	// block mutex, clear screen and write message
	lock_guard<mutex> lock(mtx);
	clear();
	cout << message << endl;
}

void readMessages(int socket, Log& log)
{
	// array for messages received from the client
	char* buffer = new char[1024];
	memset(buffer, 0, 1024);

	while (true)
	{
		// wait message from client
		int k = recv(socket, buffer, 1024, 0);

		// if k <= 0 then connection loss
		if (k > 0)
		{
			string message = buffer;

			int identificator;

			// authorization successfully
			if (message.find("@logged") != -1)
			{
				log.print(Log::info, "readMessages - You have successfully logged in");
				systemMessage("You have successfully logged in");
			}
			// authorization not successfully
			else if (message.find("@not_logged") != -1)
			{
				log.print(Log::debug, "readMessages - Identification error: this is name not available");
				systemMessage("Identification error: this is name not available, please enter other");
			}
			// not authorized
			else if (message.find("@not_auth") != -1)
			{
				log.print(Log::debug, "readMessages - Authorization error: you are not logged in");
				systemMessage("Authorization error: you are not logged in\n"
					"To use the capabilities of the application\n"
					"use @auth your_name");
			}
			// already authorized
			else if (message.find("@alreadyAuth") != -1)
			{
				log.print(Log::debug, "readMessages - You are already authorized");
				systemMessage("You are already authorized\n"
					"use @help");
			}
			// not created room
			else if (message.find("@not_create") != -1)
			{
				log.print(Log::debug, "readMessages - This is name room not available");
				systemMessage("This is name room not available, please enter other");
			}
			// created room
			else if (message.find("@create") != -1)
			{
				log.print(Log::debug, "readMessages - Room successfully created");
				systemMessage("Room successfully created");
			}
			// not open or created room
			else if (message.find("@not_room") != -1)
			{
				log.print(Log::debug, "readMessages - You are not open or create in room in to use this feature");
				systemMessage("You are not open or create in room in to use this feature");
			}
			// not open room
			else if (message.find("@not_open") != -1)
			{
				log.print(Log::debug, "readMessages - Room with this name does not exist");
				systemMessage("Room with this name does not exist");
			}
			// leave room successfully
			else if (message.find("@leave") != -1)
			{
				log.print(Log::debug, "readMessages - You have successfully left the room");
				systemMessage("You have successfully left the room");
			}
			// view room
			else if ((identificator = message.find("@view_room")) != -1)
			{
				log.print(Log::debug, "readMessages - view_room");
				lock_guard<mutex> lock(mtx);
				message.erase(identificator, message.length());
				cout << message << endl;
			}
			// open room
			else if ((identificator = message.find("@open")) != -1)
			{
				log.print(Log::debug, "readMessages - open room");
				systemMessage("You have successfully open the room");
			}
			// view clients
			else if ((identificator = message.find("@view_clients")) != -1)
			{
				log.print(Log::debug, "readMessages - view_clients");
				lock_guard<mutex> lock(mtx);
				message.erase(identificator, message.length());
				cout << message << endl;
			}
			// history message
			else if ((identificator = message.find("@history")) != -1)
			{
				log.print(Log::debug, "readMessages - history");
				lock_guard<mutex> lock(mtx);
				message.erase(identificator, message.length());
				cout << message << endl;
			}
			// receiving a message
			else if ((identificator = message.find("@send")) != -1)
			{
				log.print(Log::debug, "readMessages - send message");
				lock_guard<mutex> lock(mtx);
				message.erase(identificator, message.length());

				time_t     now = time(0);
				struct tm  tstruct;
				tstruct = *localtime(&now);

				char* c_time = new char[80];

				strftime(c_time, 80, "%d-%m-%Y.%X", &tstruct);

				cout << c_time << " - " << message << endl;
			}
			// connected successfully
			else if (message.find("@connected") != -1)
			{
				log.print(Log::info, "readMessages - connected");
				lock_guard<mutex> lock(mtx);
				isConnected = true;
			}
		}

		memset(buffer, 0, 1024);
	}
}

bool checkSyntax(string identificator, string message)
{
	// check the syntax of the message
	smatch match;
	regex regSystem("^" + identificator + "\\s\\S+");

	return regex_search(message, match, regSystem);
}

int main(int argc, char** argv)
{
	Log log;
	if (argc > 1)
	{
		if (strcmp(argv[1], "debug") == 0)
			log.setPriority(Log::debug);
		else if (strcmp(argv[1], "info") == 0)
			log.setPriority(Log::info);
		else if (strcmp(argv[1], "warning") == 0)
			log.setPriority(Log::warning);
		else if (strcmp(argv[1], "error") == 0)
			log.setPriority(Log::error);
	}

#if defined(_WIN64) || defined(_WIN32)

	WSAData WSData;

	// initialize the use of the Winsock DLL process
	if (WSAStartup(MAKEWORD(2, 2), &WSData) != 0)
	{
		log.print(Log::error, "main - WSASratup no succes");
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
	int Socket = socket(AF_INET, SOCK_STREAM, 0);

	if (Socket < 0)
	{
		log.print(Log::error, "main - Socket not created");
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
	if (connect(Socket, (struct sockaddr*)&SA, sizeof(SA)) == 0)
	{
		log.print(Log::error, "main - Socket succed connected");
		systemMessage("The server does not respond, please try again after a while.");
		cin.get();
		return -1;
	}

	try 
	{
		thread readMessage(readMessages, ref(Socket), ref(log));
		readMessage.detach();	
	}
	catch (...) 
	{
		log.print(Log::warning, "main - Failed to create thread");
		cout << "Failed to create thread, try restarting the application" << endl;
		cin.get();
		return -1;
	}

	// thread for check connection server
	future<bool> ft = async(connected);

	// wait until the thread ends
	if (!ft.get())
	{
		log.print(Log::warning, "main - At the moment the server does not respond, or is full, please try again after a while");
		systemMessage("At the moment the server does not respond, or is full, please try again after a while");
		cin.get();
		return -1;
	}

	cout << "Hello, to use the capabilities of the application" << endl;
	cout << "use @auth your_name" << endl;
	cout << "for reference use @help" << endl;

	string message;

	regex regMessage("^\\s*\\w+\\s*");

	while (true)
	{
		getline(cin, message);

		string identificator = message.substr(0, 1);

		// this is system command
		smatch match;
		if (identificator == "@")
		{

			if (message.find("@auth") != -1 && checkSyntax("@auth", message))
			{
				// check for the number of symbols
				if (message.size() > 15)
				{
					log.print(Log::info, "main - The name must not be more than 15 symbols");
					systemMessage("The name must not be more than 15 symbols");
					continue;
				}

				// checking the spaces in the name
				if (message.end() != remove(message.begin() + 6, message.end(), ' '))
				{
					log.print(Log::info, "main - Name can not contain spaces");
					systemMessage("Name can not contain spaces");
					continue;
				}
				clear();
				send(Socket, message.c_str(), message.length(), 0);
			}
			// check for the number of symbols
			else if (message.size() > 500)
			{
				log.print(Log::info, "main - The message must not be more than 500 symbols");
				systemMessage("The message must not be more than 500 symbols");
				continue;
			}
			else if ((message.find("@create") != -1 && checkSyntax("@create", message)) ||
					(message.find("@open") != -1 && checkSyntax("@open", message)) ||
					(message == "@history") || (message == "@leave"))
			{
				clear();
				send(Socket, message.c_str(), message.length(), 0);
			}
			else if (message == "@view_room")
			{
				systemMessage("Rooms\n");
				send(Socket, message.c_str(), message.length(), 0);
			}
			else if (message == "@view_clients")
			{
				systemMessage("Clients\n");
				send(Socket, message.c_str(), message.length(), 0);
			}
			else if (message == "@help")
			{
				systemMessage("@auth  -  authorization.  Example: @auth your_name\n"
					"@view_room  -  get list of all valid rooms.\n"
					"@create  -  create room.  Example: @create name_room\n"
					"@open  -  open room.  Example: @open name_room\n"
					"@view_clients  -  get a list of all valid customers.\n"
					"@history  -  get a history messages in room.\n"
					"@leave  -  leave room.\n"
					"@help  -  reference.\n"
					"To send a message, just write and press Enter, while you should be in the room");
			}
			else if (message == "@quit")
			{
				log.print(Log::info, "main - quit");
				lock_guard<mutex> lock(mtx);
				#if defined(_WIN64) || defined(_WIN32)
				closesocket(Socket);
				#endif
				cin.get();
				return 0;
			}
			else
			{
				log.print(Log::info, "main - There is no such command, use help");
				systemMessage("There is no such command, use help (@help)");
			}
		}
		// this is message
		else if(regex_search(message, match, regMessage))
		{
			if (message.size() > 500)
			{
				systemMessage("The message must not be more than 500 symbols");
				continue;
			}

			message += "@send";
			send(Socket, message.c_str(), message.length(), 0);
		}
		else
		{
			systemMessage("Unable to send empty message");
		}

		message.clear();
	}

#if defined(_WIN64) || defined(_WIN32)
	closesocket(Socket);
#endif

	return 0;
}
