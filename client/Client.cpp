#include "ClientGame.h"


ClientGame *client;


void clientLoop()
{
	while (true)
	{
		client->update();
	}
}


int main()
{
	client = new ClientGame();
	clientLoop();
}
