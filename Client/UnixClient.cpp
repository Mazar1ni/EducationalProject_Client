#include "UnixClient.h"
#include <iostream>
/*#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
*/

using namespace std;

UnixClient::UnixClient()
{
	createSocket();
}

UnixClient::~UnixClient()
{
	//close(Socket);
}

void UnixClient::clear()
{
	cout << "\x1B[2J\x1B[H";
}
