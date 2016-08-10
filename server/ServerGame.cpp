#include "ServerGame.h"


ServerGame::ServerGame(char *port)
{
	game_started = false;
	player_count = 0;
	ready_player_count = 0;
	alive_player_count = 0;
	network = new ServerNetwork(port);
}


ServerGame::~ServerGame()
{
}


void ServerGame::gameTick()
{
	if (game_started && alive_player_count < 2) restartGame();

	if (!game_started) return;

	movePlayers();
	sendTickPacket();
}


void ServerGame::getNewClients()
{
	if (game_started) return;

	if (player_count < MAX_PLAYERS && network->acceptNewClient(player_count))
	{
		initializePlayer(player_count++);
		sendInitialPacket(player_count);
		sendNewPlayerPacket(player_count);
		Log("client %d has connected to the server\n", player_count);
	}
}


void ServerGame::receiveFromClients()
{
	Packet packet;
	std::map<unsigned int, SOCKET>::iterator iter;
	auto localSessionsCopy = network->sessions;

	for (iter = localSessionsCopy.begin(); iter != localSessionsCopy.end(); ++iter)
	{
		int data_length = network->receiveData(iter->first, network_data);

		if (data_length <= 0)
		{
			//no data recieved
			continue;
		}

		int i = 0;
		while (i < data_length)
		{
			packet.deserialize(&network_data[i]);
			i += sizeof(Packet);

			switch (packet.packet_type) {
			case ACTION_EVENT:
				handleActionPacket(iter->first - 1, network_data[4]);
				break;
			case READY_PACKET:
				handleReadyPacket(iter->first - 1);
				break;
			default:
				Log("error in packet types\n");
				LogInDebugOnly("%s\n", packet.data);
				break;
			}
		}
	}
}


void ServerGame::restartGame()
{
	game_started = false;
	ready_player_count = alive_player_count = 0;
	memset(board, 0, sizeof(board[0][0]) * MAX_X * MAX_Y); // clear the board

	for (int id = 0; id < player_count; id++)
	{
		initializePlayer(id);
	}

	sendRestartPacket();
	Log("game has been restarted\n");
}


void ServerGame::movePlayers()
{
	for (int id = 0; id < player_count; id++)
	{
		if (!players[id].is_alive) continue;

		auto pos = players[id].position;
		switch (players[id].direction)
		{
		case UP:
			if (pos.y != 0 && !board[pos.x][pos.y - 1])
			{
				players[id].move(0, -1);
				board[pos.x][pos.y - 1] = true;
				continue;
			}
			break;
		case RIGHT:
			if (pos.x != MAX_X && !board[pos.x + 1][pos.y])
			{
				players[id].move(1, 0);
				board[pos.x + 1][pos.y] = true;
				continue;
			}
			break;
		case DOWN:
			if (pos.y != MAX_Y - 1 && !board[pos.x][pos.y + 1])
			{
				players[id].move(0, 1);
				board[pos.x][pos.y + 1] = true;
				continue;
			}
			break;
		case LEFT:
			if (pos.x != 0 && !board[pos.x - 1][pos.y])
			{
				players[id].move(-1, 0);
				board[pos.x - 1][pos.y] = true;
				continue;
			}
			break;
		default:
			continue;
		}

		alive_player_count--;
		players[id].is_alive = false;
		Log("client %d was killed at %d, %d\n", id + 1, players[id].position.x, players[id].position.y);
	}
}


void ServerGame::handleReadyPacket(unsigned char id)
{
	if (game_started) return;

	// toggle the flag
	if (players[id].is_ready)
	{
		ready_player_count--;
		players[id].is_ready = false;
	}
	else
	{
		ready_player_count++;
		players[id].is_ready = true;
	}

	Log("client %d changed state to %s\n", id + 1, players[id].is_ready ? "ready" : "not ready");

	if (player_count > 1 && ready_player_count == player_count)
	{
		game_started = true;
		Log("All clients are ready, starting the game\n");
	}
}


void ServerGame::handleActionPacket(unsigned char id, int direction)
{
	auto prevDirection = players[id].direction;
	auto curDirection = DirectionEnum(direction);

	if (prevDirection == curDirection) return;

	switch (curDirection)
	{
	case UP:
		if (prevDirection == DOWN) return;
		break;
	case RIGHT:
		if (prevDirection == LEFT) return;
		break;
	case DOWN:
		if (prevDirection == UP) return;
		break;
	case LEFT:
		if (prevDirection == RIGHT) return;
		break;
	default:
		break;
	}

	players[id].direction = curDirection;
	LogInDebugOnly("client %d changed direction to %d\n", id + 1, curDirection);
}


void ServerGame::initializePlayer(unsigned char id)
{
	auto dir = DirectionEnum(rand() % LEFT);
	unsigned char x = MAX_X / 5 + rand() % MAX_X / 5 + 2 * (id % 2) * MAX_X / 5;
	unsigned char y = MAX_Y / 5 + rand() % MAX_Y / 5;
	if (id >= 2) y += 2 * MAX_Y / 5;

	Player p = Player(id + 1, dir, Position(x, y));

	LogInDebugOnly("Created player %d with direction %d and position (%d, %d)\n", p.id, p.direction, p.position.x, p.position.y);

	players[id] = p;
	board[x][y] = true;
	alive_player_count++;
}


void ServerGame::sendTickPacket() const
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = TICK_PACKET;
	createPacketWithPositions(0, packet.data);

	packet.serialize(packet_data);
	network->sendToAll(packet_data, packet_size);
}


void ServerGame::sendRestartPacket() const
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = RESTART_PACKET;
	createRestartPacket(packet.data);

	packet.serialize(packet_data);
	network->sendToAll(packet_data, packet_size);
}


void ServerGame::sendInitialPacket(unsigned char id) const
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = INIT_PACKET;
	createPacketWithPositions(id, packet.data);

	packet.serialize(packet_data);
	network->sendToOne(id, packet_data, packet_size);
}


void ServerGame::sendNewPlayerPacket(unsigned char id) const
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = NEW_PLAYER_CONNECTED;
	createNewPlayerPacket(id, packet.data);

	packet.serialize(packet_data);
	network->sendToAllButOne(id, packet_data, packet_size);
}


void ServerGame::createRestartPacket(char packet_data[]) const
{
	int index = 0;

	// store data about each player's position
	for (unsigned int i = 0; i < player_count; i++)
	{
		packet_data[index++] = players[i].id;
		packet_data[index++] = players[i].position.x;
		packet_data[index++] = players[i].position.y;
	}

	// add two -1s at the end for easier parsing on client side
	packet_data[index + 1] = packet_data[index] = -1;

	LogInDebugOnly("Created restart packet: %s\n", packet_data);
}


void ServerGame::createPacketWithPositions(unsigned char id, char packet_data[]) const
{
	int index = 0;

	packet_data[index++] = id; // first bit -> player's id

	// store data about each player's position
	for (unsigned int i = 0; i < player_count; i++)
	{
		packet_data[index++] = players[i].id;
		packet_data[index++] = players[i].position.x;
		packet_data[index++] = players[i].position.y;
	}

	// add two -1s at the end for easier parsing on client side
	packet_data[index + 1] = packet_data[index] = -1;

	LogInDebugOnly("Created packet with positions: %s\n", packet_data);
}


void ServerGame::createNewPlayerPacket(unsigned char id, char packet_data[]) const
{
	packet_data[0] = players[id - 1].position.x;
	packet_data[1] = players[id - 1].position.y;
	LogInDebugOnly("Created new player packet: %d, %d\n", packet_data[0], packet_data[1]);
}
