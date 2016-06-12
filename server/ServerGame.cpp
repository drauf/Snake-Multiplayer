#include "ServerGame.h"


unsigned int ServerGame::client_id;


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
	if (client_id < MAX_PLAYERS && network->acceptNewClient(client_id))
	{
		printf("client %d has connected to the server\n", ++client_id);
		initializePlayer(client_id);
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

	players[id] = p;
	board[x][y] = true;
}
