#pragma once
#include "ClientNetwork.h"
#include "Networking.h"
#include "DirectionEnum.h"
#include "Drawing.h"


class ClientGame
{
	HWND hWnd;
	unsigned int tiles;
	unsigned int client_id;
	ClientNetwork* network;
	char network_data[MAX_PACKET_SIZE]; // data buffer

public:
	ClientGame(char *ip, char *port, HWND hWnd);
	~ClientGame();

	void sendReadyPacket() const;
	void sendActionPacket(DirectionEnum direction) const;
	void update(Tile board[MAX_X * MAX_Y]);

private:
	void handleInitPacket(char data[], Tile board[MAX_X * MAX_Y]);
	void handleTickPacket(char data[], Tile board[MAX_X * MAX_Y]);
	void handleRestartPacket(char data[], Tile board[MAX_X * MAX_Y]);
	void handleNewPlayerPacket(char data[], Tile board[MAX_X * MAX_Y]);
};
