#pragma once
#include <string.h>
#include <winsock2.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#ifdef COMPILE_NETWORKING
#define NETWORKING_EXPORT __declspec(dllexport)
#else
#define NETWORKING_EXPORT __declspec(dllimport)
#endif


#define MAX_PACKET_SIZE 1000000
#define DEFAULT_DATA_SIZE 32


class NETWORKING_EXPORT NetworkServices
{
public:
	static int sendMessage(SOCKET curSocket, char *message, int messageSize);
	static int receiveMessage(SOCKET curSocket, char *buffer, int bufSize);
};


enum NETWORKING_EXPORT PacketTypes {
	INIT_CONNECTION = 0,
	INIT_PACKET = 1,
	NEW_PLAYER_CONNECTED = 2,
	ACTION_EVENT = 3,
	READY_PACKET = 4,
};


struct NETWORKING_EXPORT Packet {
	unsigned int packet_type;
	char data[DEFAULT_DATA_SIZE];

	void serialize(char * data) const
	{
		memcpy(data, this, sizeof(Packet));
	}

	void deserialize(char * data) {
		memcpy(this, data, sizeof(Packet));
	}
};
