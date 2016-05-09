#include "ServerGame.h"


ServerGame *server;


void serverLoop()
{
	while (true)
	{
		server->update();
	}
}


int main()
{
	server = new ServerGame();
	serverLoop();
}
