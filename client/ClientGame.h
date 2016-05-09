#pragma once
#include "ClientNetwork.h"
#include "Networking.h"


class ClientGame
{
	ClientNetwork* network;
	char network_data[MAX_PACKET_SIZE]; // data buffer

public:
	ClientGame();
	~ClientGame();

	void sendActionPackets() const;
	void update();
};
