#pragma once
#include "ClientNetwork.h"
#include "Networking.h"
#include "DirectionEnum.h"


class ClientGame
{
	ClientNetwork* network;
	char network_data[MAX_PACKET_SIZE]; // data buffer

public:
	ClientGame();
	~ClientGame();

	void sendActionPackets(DirectionEnum direction) const;
	void update();
};
