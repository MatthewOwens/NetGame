#include "Server.h"
#include <iostream>

int Server::run()
{
	std::cout << "Server starting..." << std::endl;
	std::cout << "The server can currently only be killed by the OS" << std::endl;

	// Creating the socket that the server will listen on
	sf::Socket::Status listenStatus = serverSocket.listen(SERVERPORT);

	if(listenStatus != sf::Socket::Done)
	{
		// Something's gone wrong with creating the socket, but what?
		if(listenStatus == sf::Socket::Error)
			die("listen failed!");
	}

	while(true)
	{

	}

	return 0;
}

Server::Server()
{
	// Setting up the SFML ip
	serverIP = sf::IpAddress(SERVERIP);

	// Ensuring our player data is all voided
	for(int i = 0; i < MAX_PLAYERS; ++i)
		players[i] = NULL;
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
