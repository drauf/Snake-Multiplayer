#pragma once
#include <ctime>
#include "ServerNetwork.h"
#include "Networking.h"
#include "Player.h"


#define MAX_PLAYERS 4
// game board size in tiles
#define MAX_X 119
#define MAX_Y 86


class ServerGame
{
	ServerNetwork* network;
	bool game_started;
	unsigned int player_count;
	unsigned int ready_player_count;
	unsigned int alive_player_count;
	char network_data[MAX_PACKET_SIZE]; // data buffer

	Player players[MAX_PLAYERS];
	bool board[MAX_X][MAX_Y]; // game board for fast collision checking

public:
	explicit ServerGame(char *port);
	~ServerGame();

	void gameTick();
	void getNewClients();
	void receiveFromClients();

private:
	void restartGame();

	void movePlayers();

	void handleReadyPacket(unsigned char id);
	void handleActionPacket(unsigned char id, int direction);

	void initializePlayer(unsigned char id);

	void sendTickPacket() const;
	void sendRestartPacket() const;
	void sendInitialPacket(unsigned char id) const;
	void sendNewPlayerPacket(unsigned char id) const;

	void createRestartPacket(char packet_data[]) const;
	void createPacketWithPositions(unsigned char id, char packet_data[]) const;
	void createNewPlayerPacket(unsigned char id, char packet_data[]) const;
};
