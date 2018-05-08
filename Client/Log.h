#pragma once
#include <iostream>
#include <fstream>
#include <string>

class Log
{
public:
	enum Priority {null, error, warning, info, debug};
	Log();
	~Log();

	static void print(Priority priorityMessage, std::string message);
	static void setPriority(Priority prior);

private:
	static std::ofstream fout;
	static Priority priority;
};
