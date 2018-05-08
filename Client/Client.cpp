#include "Client.h"

#if defined(__MACH__) || defined(__APPLE__)
#include <unistd.h>
#endif

Client::Client(int s) : socket(s)
{
	try
	{
		thread t([&](Client* view) { view->readMessages(); }, this);
		t.detach();
		//thread readMessage(&Client::readMessages);
		//readMessage.detach();
	}
	catch (...)
	{
		Log::print(Log::warning, "Client::Client - Failed to create thread");
		cout << "Failed to create thread, try restarting the application" << endl;
		cin.get();
		delete this;
	}

	// thread for check connection server
	// wait until the thread ends
	if (!connected())
	{
		Log::print(Log::warning, "Client::Client - At the moment the server does not respond, or is full, please try again after a while");
		systemMessage("At the moment the server does not respond, or is full, please try again after a while");
		cin.get();
		delete this;
	}

	cout << "Hello, to use the capabilities of the application" << endl;
	cout << "use @auth your_name" << endl;
	cout << "for reference use @help" << endl;

	sendMessage();
}

Client::~Client()
{
#if defined(_WIN64) || defined(_WIN32)
	closesocket(socket);
#endif
}

#if defined(_WIN64) || defined(_WIN32)

void Client::clear()
{
	system("cls");
}

#else

void Client::clear()
{
	cout << "\x1B[2J\x1B[H";
}

#endif

bool Client::connected()
{
	int count = 0;
	// check if the connection to the server is established
	while (true)
	{
		mtx.lock();
		// connection successfully or time out
		if (isConnected == true || count > 10)
		{
			mtx.unlock();
			return isConnected;
		}
		else
		{
			count++;
		}
		mtx.unlock();
	#if defined(__MACH__) || defined(__APPLE__)
		sleep(1);
	#else
		Sleep(1000);
	#endif
	}
}

void Client::systemMessage(string message)
{
	// block mutex, clear screen and write message
	lock_guard<mutex> lock(mtx);
	clear();
	cout << message << endl;
}

void Client::readMessages()
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
				Log::print(Log::info, "Client::readMessages - You have successfully logged in");
				systemMessage("You have successfully logged in");
			}
			// authorization not successfully
			else if (message.find("@not_logged") != -1)
			{
				Log::print(Log::debug, "Client::readMessages - Identification error: this is name not available");
				systemMessage("Identification error: this is name not available, please enter other");
			}
			// not authorized
			else if (message.find("@not_auth") != -1)
			{
				Log::print(Log::debug, "Client::readMessages - Authorization error: you are not logged in");
				systemMessage("Authorization error: you are not logged in\n"
					"To use the capabilities of the application\n"
					"use @auth your_name");
			}
			// already authorized
			else if (message.find("@alreadyAuth") != -1)
			{
				Log::print(Log::debug, "Client::readMessages - You are already authorized");
				systemMessage("You are already authorized\n"
					"use @help");
			}
			// not created room
			else if (message.find("@not_create") != -1)
			{
				Log::print(Log::debug, "Client::readMessages - This is name room not available");
				systemMessage("This is name room not available, please enter other");
			}
			// created room
			else if (message.find("@create") != -1)
			{
				Log::print(Log::debug, "Client::readMessages - Room successfully created");
				systemMessage("Room successfully created");
			}
			// not open or created room
			else if (message.find("@not_room") != -1)
			{
				Log::print(Log::debug, "Client::readMessages - You are not open or create in room in to use this feature");
				systemMessage("You are not open or create in room in to use this feature");
			}
			// not open room
			else if (message.find("@not_open") != -1)
			{
				Log::print(Log::debug, "Client::readMessages - Room with this name does not exist");
				systemMessage("Room with this name does not exist");
			}
			// leave room successfully
			else if (message.find("@leave") != -1)
			{
				Log::print(Log::debug, "Client::readMessages - You have successfully left the room");
				systemMessage("You have successfully left the room");
			}
			// view room
			else if ((identificator = message.find("@view_room")) != -1)
			{
				Log::print(Log::debug, "Client::readMessages - view_room");
				lock_guard<mutex> lock(mtx);
				message.erase(identificator, message.length());
				cout << message << endl;
			}
			// open room
			else if ((identificator = message.find("@open")) != -1)
			{
				Log::print(Log::debug, "Client::readMessages - open room");
				systemMessage("You have successfully open the room");
			}
			// view clients
			else if ((identificator = message.find("@view_clients")) != -1)
			{
				Log::print(Log::debug, "Client::readMessages - view_clients");
				lock_guard<mutex> lock(mtx);
				message.erase(identificator, message.length());
				cout << message << endl;
			}
			// history message
			else if ((identificator = message.find("@history")) != -1)
			{
				Log::print(Log::debug, "Client::readMessages - history");
				lock_guard<mutex> lock(mtx);
				message.erase(identificator, message.length());
				cout << message << endl;
			}
			// receiving a message
			else if ((identificator = message.find("@send")) != -1)
			{
				Log::print(Log::debug, "Client::readMessages - send message");
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
				Log::print(Log::info, "Client::readMessages - connected");
				lock_guard<mutex> lock(mtx);
				isConnected = true;
			}
			else if (message.find("@close_server") != -1)
			{
				Log::print(Log::warning, "Client::readMessages - close server");
				systemMessage("Server closed");
			}
		}

		memset(buffer, 0, 1024);
	}
}

bool Client::checkSyntax(string identificator, string message)
{
	// check the syntax of the message
	smatch match;
	regex regSystem("^" + identificator + "\\s\\S+");

	return regex_search(message, match, regSystem);
}

void Client::sendMessage()
{
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
					Log::print(Log::info, "Client::sendMessage - The name must not be more than 15 symbols");
					systemMessage("The name must not be more than 15 symbols");
					continue;
				}

				// checking the spaces in the name
				if (message.end() != remove(message.begin() + 6, message.end(), ' '))
				{
					Log::print(Log::info, "Client::sendMessage - Name can not contain spaces");
					systemMessage("Name can not contain spaces");
					continue;
				}
				clear();
				send(socket, message.c_str(), message.length(), 0);
			}
			// check for the number of symbols
			else if (message.size() > 500)
			{
				Log::print(Log::info, "Client::sendMessage - The message must not be more than 500 symbols");
				systemMessage("The message must not be more than 500 symbols");
				continue;
			}
			else if ((message.find("@create") != -1 && checkSyntax("@create", message)) ||
				(message.find("@open") != -1 && checkSyntax("@open", message)) ||
				(message == "@history") || (message == "@leave"))
			{
				clear();
				send(socket, message.c_str(), message.length(), 0);
			}
			else if (message == "@view_room")
			{
				systemMessage("Rooms\n");
				send(socket, message.c_str(), message.length(), 0);
			}
			else if (message == "@view_clients")
			{
				systemMessage("Clients\n");
				send(socket, message.c_str(), message.length(), 0);
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
					"@quit  -  close application.\n"
					"To send a message, just write and press Enter, while you should be in the room");
			}
			else if (message == "@quit")
			{
				Log::print(Log::info, "main - quit");
				mtx.lock();
				cin.get();
				mtx.unlock();
				delete this;
			}
			else
			{
				Log::print(Log::info, "Client::sendMessage - There is no such command, use help");
				systemMessage("There is no such command, use help (@help)");
			}
		}
		// this is message
		else if (regex_search(message, match, regMessage))
		{
			if (message.size() > 500)
			{
				systemMessage("The message must not be more than 500 symbols");
				continue;
			}

			message += "@send";
			send(socket, message.c_str(), message.length(), 0);
		}
		else
		{
			systemMessage("Unable to send empty message");
		}

		message.clear();
	}
}
