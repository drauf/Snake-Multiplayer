#include "ServerGame.h"


ServerGame *server;


void serverLoop()
{
	while (true)
	{
		server->update();
	}
}


int main(int argc, char *argv[])
{
	srand(time(nullptr));

	if (argc < 2) server = new ServerGame("27015");
	else server = new ServerGame(argv[1]);
	serverLoop();
}
