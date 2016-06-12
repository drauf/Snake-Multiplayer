#include <cstdio>
#include "ClientGame.h"
#include "Networking.h"


ClientGame::ClientGame(char *ip, char *port, HWND hWnd)
{
	this->hWnd = hWnd;
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


void ClientGame::update(TileTypeEnum board[MAX_X][MAX_Y])
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
		case INIT_PACKET:
			handleInitPacket(packet.data, board);
			break;
		default:
			break;
		}
		
	}
}


void ClientGame::handleInitPacket(char data[], TileTypeEnum board[MAX_X][MAX_Y])
{
	int index = 0;
	client_id = data[index++];

	while (data[index] != -1)
	{
		char id = data[index++];
		char x = data[index++];
		char y = data[index++];

		if (id == client_id)
		{
			board[x][y] = CURRENT_PLAYER;
		}
		else
		{
			board[x][y] = ANOTHER_PLAYER;
		}

		Drawing::DrawSquare(hWnd, x, y, board[x][y]);
	}
}
