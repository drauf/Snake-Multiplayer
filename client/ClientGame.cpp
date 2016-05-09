#include <cstdio>
#include "ClientGame.h"
#include "Networking.h"


ClientGame::ClientGame()
{
	network = new ClientNetwork();

	// send initial packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = INIT_CONNECTION;

	packet.serialize(packet_data);

	NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}


ClientGame::~ClientGame()
{
}


void ClientGame::sendActionPackets() const
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = ACTION_EVENT;

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
			printf("client received init packet from server\n");
			sendActionPackets();
			break;
		case ACTION_EVENT:
			printf("client received action event packet from server\n");
			sendActionPackets();
			break;
		default:
			printf("error in packet types\n");
			break;
		}
	}
}
