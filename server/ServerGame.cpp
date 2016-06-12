#include "ServerGame.h"


unsigned int ServerGame::player_count;


ServerGame::ServerGame(char *port)
{
	network = new ServerNetwork(port);
}


ServerGame::~ServerGame()
{
}


void ServerGame::update()
{
	// get new clients
	if (player_count < MAX_PLAYERS && network->acceptNewClient(player_count))
	{
		printf("client %d has connected to the server\n", ++player_count);
		initializePlayer(player_count);
		sendInitialPacket(player_count);
	}

	receiveFromClients();
}


void ServerGame::receiveFromClients()
{
	Packet packet;
	std::map<unsigned int, SOCKET>::iterator iter;

	for (iter = network->sessions.begin(); iter != network->sessions.end(); ++iter)
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
			case INIT_CONNECTION:
				printf("server received init packet from client\n");
				sendActionPackets();
				break;
			case ACTION_EVENT:
				printf("server received action event packet from client\n");
				sendActionPackets();
				break;
			default:
				printf("error in packet types\n");
				break;
			}
		}
	}
}


void ServerGame::sendActionPackets() const
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = ACTION_EVENT;

	packet.serialize(packet_data);

	network->sendToAll(packet_data, packet_size);
}


void ServerGame::initializePlayer(unsigned char id)
{
	auto dir = DirectionEnum(rand() % LEFT);
	unsigned char x = MAX_X / 5 + rand() % MAX_X / 5 + 2 * ((id + 1) % 2) * MAX_X / 5;
	unsigned char y = MAX_Y / 5 + rand() % MAX_Y / 5;
	if (id > 2) y += 2 * MAX_Y / 5;

	Player p = Player(id, dir, Position(x, y));

#ifdef _DEBUG
	printf("Created player %d with direction %d and position (%d, %d)\n", p.id, p.direction, p.positions[0].x, p.positions[0].y);
#endif

	players[id - 1] = p;
	board[x][y] = true;
}


void ServerGame::sendInitialPacket(unsigned char id) const
{
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	createInitialPacket(id, packet_data);

	Packet packet;
	packet.packet_type = INIT_PACKET;
	packet.serialize(packet_data);
	network->sendToOne(id, packet_data, packet_size);
}


void ServerGame::createInitialPacket(unsigned char id, char packet_data[]) const
{
	int index = 0;

	packet_data[index++] = id; // first bit -> player's id

	// store data about every player's position
	for (unsigned int i = 0; i < player_count; i++)
	{
		packet_data[index++] = players[i].id;
		packet_data[index++] = players[i].positions[0].x;
		packet_data[index++] = players[i].positions[0].y;
	}

	// add two -1s at the end for easie parsing on client side
	packet_data[index + 1] = packet_data[index] = -1;

#ifdef _DEBUG
	printf("Created initial packet to client %d:\n", id);
	for (unsigned int i = 0; i < (1 + player_count) * 3; i++)
		printf("%d,", packet_data[i]);
	printf("\n");
#endif
}
