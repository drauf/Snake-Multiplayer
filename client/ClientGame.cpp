#include <cstdio>
#include "ClientGame.h"
#include "Networking.h"


ClientGame::ClientGame(char *ip, char *port)
{
	network = new ClientNetwork(ip, port);
}


ClientGame::~ClientGame()
{
}


void ClientGame::sendActionPacket(DirectionEnum direction) const
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = ACTION_EVENT;
	packet.data[0] = direction;

	packet.serialize(packet_data);

	NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}


void ClientGame::update()
{
	Packet packet;
	int data_length = network->receivePackets(network_data);

	if (data_length <= 0)
	{
		// no data recieved
		return;
	}

	int i = 0;
	while (i < data_length)
	{
		packet.deserialize(&network_data[i]);
		i += sizeof(Packet);

		switch (packet.packet_type) {
		case INIT_CONNECTION:
			// TODO: interpret the init packet from server:
			// force redraw, set correct player's direction
			break;
		case ACTION_EVENT:
			// not yet implemented
			break;
		default:
			break;
		}
	}
}
