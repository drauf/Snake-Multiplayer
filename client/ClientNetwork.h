#pragma once
#include <winsock2.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512 // size of the buffer
#define DEFAULT_PORT "27015" // port to connect sockets through 


class ClientNetwork
{
	int iResult; // for error checking function calls in Winsock library

public:
	SOCKET ConnectSocket; // socket for client to connect to server

	ClientNetwork();
	~ClientNetwork();

	int receivePackets(char *);
};
