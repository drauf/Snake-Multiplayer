#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include "ServerNetwork.h"
#include "Networking.h"


void printServerAddress(char *port)
{
	char szBuffer[1024];

	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 0);
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
		return;

	if (gethostname(szBuffer, sizeof(szBuffer)) == SOCKET_ERROR)
	{
		WSACleanup();
		return;
	}

	hostent *host = gethostbyname(szBuffer);
	if (host == nullptr)
	{
		WSACleanup();
		return;
	}

	//Obtain the computer's IP
	auto b1 = reinterpret_cast<struct in_addr *>((host->h_addr))->S_un.S_un_b.s_b1;
	auto b2 = reinterpret_cast<struct in_addr *>((host->h_addr))->S_un.S_un_b.s_b2;
	auto b3 = reinterpret_cast<struct in_addr *>((host->h_addr))->S_un.S_un_b.s_b3;
	auto b4 = reinterpret_cast<struct in_addr *>((host->h_addr))->S_un.S_un_b.s_b4;

	printf("IP: %d.%d.%d.%d:%s\n", b1, b2, b3, b4, port);

	WSACleanup();
}


ServerNetwork::ServerNetwork(char *port)
{
	WSADATA wsaData;
	ListenSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;
	struct addrinfo *result = nullptr, hints;

	// initialize winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		exit(1);
	}

	// set address information
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP; // TCP connection
	hints.ai_flags = AI_PASSIVE;

	// resolve the server address and port
	iResult = getaddrinfo(nullptr, port, &hints, &result);

	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		exit(1);
	}

	// print server address and port
	printServerAddress(port);

	// create a socket for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		exit(1);
	}

	// set the socket to be nonblocking
	u_long iMode = 1;
	iResult = ioctlsocket(ListenSocket, FIONBIO, &iMode);

	if (iResult == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}

	// setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, int(result->ai_addrlen));

	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}

	// no longer need address information
	freeaddrinfo(result);

	// start listening for new clients attempting to connect
	iResult = listen(ListenSocket, SOMAXCONN);

	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}
}


ServerNetwork::~ServerNetwork(void)
{
}


bool ServerNetwork::acceptNewClient(unsigned int &id)
{
	// accept new connection and save the socket
	ClientSocket = accept(ListenSocket, nullptr, nullptr);

	if (ClientSocket != INVALID_SOCKET)
	{
		// disable Nagle's algorithm to send actions immediately instead of combining packets
		char value = 1;
		setsockopt(ClientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

		// save new client's in session id table
		sessions.insert(std::pair<unsigned int, SOCKET>(id + 1, ClientSocket));

		return true;
	}

	return false;
}


int ServerNetwork::receiveData(unsigned int client_id, char *recvbuf)
{
	if (sessions.find(client_id) != sessions.end())
	{
		SOCKET currentSocket = sessions[client_id];
		iResult = NetworkServices::receiveMessage(currentSocket, recvbuf, MAX_PACKET_SIZE);

		if (iResult == 0)
		{
			printf("Connection closed\n");
			closesocket(currentSocket);
		}

		return iResult;
	}

	return 0;
}


void ServerNetwork::sendToOne(unsigned int id, char* packets, int totalSize)
{
	SOCKET socket = sessions.at(id);

	int iSendResult = NetworkServices::sendMessage(socket, packets, totalSize);

	if (iSendResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(socket);
	}
}


void ServerNetwork::sendToAll(char *packets, int totalSize)
{
	SOCKET currentSocket;
	std::map<unsigned int, SOCKET>::iterator iter;
	int iSendResult;

	for (iter = sessions.begin(); iter != sessions.end(); ++iter)
	{
		currentSocket = iter->second;
		iSendResult = NetworkServices::sendMessage(currentSocket, packets, totalSize);

		if (iSendResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(currentSocket);
		}
	}
}
