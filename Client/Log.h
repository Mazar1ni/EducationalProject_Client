#pragma once
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Log
{
public:
	enum Priority {null, error, warning, info, debug};
	Log();
	~Log();

	void print(Priority priorityMessage, string message);
	void setPriority(Priority prior);

private:
	ofstream fout;
	Priority priority;
};
