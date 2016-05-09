#pragma once
#include "ServerNetwork.h"
#include "Networking.h"


class ServerGame
{
	ServerNetwork* network;
	static unsigned int client_id; // IDs for the clients connecting
	char network_data[MAX_PACKET_SIZE]; // data buffer

public:
	ServerGame();
	~ServerGame();

	void update();
	void receiveFromClients();
	void sendActionPackets() const;
};
