#include <process.h>
#include "ServerGame.h"


ServerGame *server;


void __cdecl getNewClientsThread(void *args)
{
	while (true)
	{
		server->getNewClients();
	}
}


void __cdecl receiveFromClientsThread(void *args)
{
	while (true)
	{
		server->receiveFromClients();
	}
}


void __cdecl gameLoop(void *args)
{
	float tickLength = 80;

	while (true)
	{
		Sleep(tickLength);
		tickLength *= 0.999;
		server->gameTick();
	}
}


int main(int argc, char *argv[])
{
	srand(time(nullptr));

	if (argc < 2) server = new ServerGame("27015");
	else server = new ServerGame(argv[1]);

	auto hThread1 = HANDLE(_beginthread(getNewClientsThread, 0, nullptr));
	auto hThread2 = HANDLE(_beginthread(receiveFromClientsThread, 0, nullptr));
	gameLoop(nullptr);
}
