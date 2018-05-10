#include "INetwork.h"
#include "Log.h"

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

	INetwork* client = INetwork::getNetwork();

	return 0;
}