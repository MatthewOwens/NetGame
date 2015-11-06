#include "Server.h"
#include <iostream>

void run()
{
	std::cout << "Server starting." << std::endl;

	
	return 0;
}

Server::Server()
{
	// Ensuring our player data is all voided
	for(int i = 0; i < MAX_PLAYERS; ++i)
		players = NULL;
}

Server::~Server()
{
	// Clearing up the player pointers
	for(int i = 0; i < MAX_PLAYERS; ++i)
	{
		delete players[i];
		players[i] = NULL;
	}
}
