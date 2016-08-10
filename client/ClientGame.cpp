#include <cstdio>
#include "ClientGame.h"
#include "Networking.h"


ClientGame::ClientGame(char *ip, char *port, HWND hWnd)
{
	this->hWnd = hWnd;
	tiles = 0;
	network = new ClientNetwork(ip, port);
}


ClientGame::~ClientGame()
{
}


void ClientGame::sendReadyPacket() const
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = READY_PACKET;

	packet.serialize(packet_data);
	NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
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


void ClientGame::update(Tile board[MAX_X * MAX_Y])
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
		case TICK_PACKET:
			handleTickPacket(packet.data, board);
			break;
		case RESTART_PACKET:
			handleRestartPacket(packet.data, board);
			break;
		case NEW_PLAYER_CONNECTED:
			handleNewPlayerPacket(packet.data, board);
			break;
		default:
			break;
		}
	}

	InvalidateRect(hWnd, nullptr, FALSE);
	UpdateWindow(hWnd);
}


void ClientGame::handleInitPacket(char data[], Tile board[MAX_X * MAX_Y])
{
	int index = 0;
	client_id = data[index++];

	while (data[index] != -1)
	{
		char id = data[index++];
		char x = data[index++];
		char y = data[index++];
		auto type = (id == client_id) ? CURRENT_PLAYER : ANOTHER_PLAYER;

		board[tiles++] = Tile(x, y, type);
	}
}


void ClientGame::handleTickPacket(char data[], Tile board[MAX_X * MAX_Y])
{
	int index = 1;

	while (data[index] != -1)
	{
		char id = data[index++];
		char x = data[index++];
		char y = data[index++];
		auto type = (id == client_id) ? CURRENT_PLAYER : ANOTHER_PLAYER;

		board[tiles++] = Tile(x, y, type);
	}
}


void ClientGame::handleRestartPacket(char data[], Tile board[MAX_X * MAX_Y])
{
	int index = tiles = 0;
	memset(board, 0, sizeof(board[0]) * MAX_X * MAX_Y); // clear the board

	while (data[index] != -1)
	{
		char id = data[index++];
		char x = data[index++];
		char y = data[index++];
		auto type = (id == client_id) ? CURRENT_PLAYER : ANOTHER_PLAYER;

		board[tiles++] = Tile(x, y, type);
	}
}


void ClientGame::handleNewPlayerPacket(char data[], Tile board[MAX_X * MAX_Y])
{
	char x = data[0];
	char y = data[1];

	board[tiles++] = Tile(x, y, ANOTHER_PLAYER);
}
