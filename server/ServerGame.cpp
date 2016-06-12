#include "ServerGame.h"


unsigned int ServerGame::client_id;


ServerGame::ServerGame(char *port)
{
	network = new ServerNetwork(port);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		Player p = Player(i + 1, RIGHT, Position(0, i));

#ifdef _DEBUG
		printf("Created player %d with direction %d and position (%d, %d)\n", p.id, p.direction, p.positions[0].x, p.positions[0].y);
#endif

		players[i] = p;
	}
}


ServerGame::~ServerGame()
{
}


void ServerGame::update()
{
	// get new clients
	if (network->acceptNewClient(client_id))
	{
		printf("client %d has been connected to the server\n", client_id);
		client_id++;
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
