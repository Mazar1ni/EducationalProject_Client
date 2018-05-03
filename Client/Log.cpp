#define _CRT_SECURE_NO_WARNINGS
#include "Log.h"
#include <time.h>

Log::Log()
{
	//fout.open("log.txt");
}

Log::~Log()
{
	if (fout.is_open())
	{
		fout.close();
	}
}

void Log::print(Priority priorityMessage, string message)
{
	bool isPrint = false;
	string priorityName;
	switch (priority)
	{
	case error:
		if (priorityMessage == error)
		{
			isPrint = true;
			priorityName = "Error";
		}
		break;
	case warning:
		if (priorityMessage == warning)
		{
			isPrint = true;
			priorityName = "Warning";
		}
		break;
	case info:
		if (priorityMessage == info)
		{
			isPrint = true;
			priorityName = "Info";
		}
		break;
	case debug:
		if (priorityMessage == debug)
		{
			isPrint = true;
			priorityName = "Debug";
		}
		break;
	}

	if (isPrint == true)
	{
		// getting a date
		time_t     now = time(0);
		struct tm  tstruct;
		tstruct = *localtime(&now);

		char* c_time = new char[80];

		strftime(c_time, 80, "%d-%m-%Y.%X", &tstruct);

		// structure the data
		string str = c_time;
		str += " - ";
		str += priorityName;
		str += " : ";
		str += message;
		str += "\n";

		fout << str;
	}
}

void Log::setPriority(Priority prior)
{
	priority = prior;
	fout.open("log.txt");
}