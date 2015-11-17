#include "Server.h"
#include <iostream>
#include <SFML/Window/Keyboard.hpp>
#include <cstdio>

int Server::run()
{
	std::cout << "Server starting..." << std::endl;
	std::cout << "The server can currently only be killed by the OS, sorry!" << std::endl;
	//std::cout << "Press 'q' to quit" << std::endl;

	// Creating the socket that the server will listen on
	sf::Socket::Status listenStatus = serverSocket.listen(SERVERPORT);

	if(listenStatus != sf::Socket::Done)
	{
		// Something's gone wrong with creating the socket, but what?
		if(listenStatus == sf::Socket::Error)
			die("listen failed!"); //TODO: Fix rather than just death
	}

	serverSocket.setBlocking(false);

	// Populating our socket selector
	selector.add(serverSocket);

	for(int i = 0; i < MAX_PLAYERS; ++i)
		selector.add(playerSockets[i]);

	while(true)
	{
		// Checking if we've quit
		// TODO: Non-blocking or threaded keybaord input
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
		{
			std::cout << "Quitting!" << std::endl;
			return 0;
		}

		//if(selector.wait(sf::miliseconds(100)))
		if(selector.wait())
		{
			// If we recieved something on the server socket
			if(selector.isReady(serverSocket))
			{
				listen();
			}
		}
		else
		{
			// Listener socket isn't ready, check the clients

			//Checking the players
			for(int i = 0; i < MAX_PLAYERS; ++i)
			{
				if(selector.isReady(playerSockets[i]))
				{
					/* A player has sent some data
					sf::Packet packet;

					// If everything has been recieved
					if(playerSockets[i].recieve(packet) == sf::Socket::Done)
					{
						// TODO: Pass data into playerData[i];
					}*/
				}
			}
		}
	}

	return 0;
}

void Server::listen()
{
	// Using SFML's int types to avoid issues with varying int sizes on
	// different machines.
	sf::Int8 clientID = 0;

	// Checking if we can connect any more players
	for(; clientID < MAX_PLAYERS; ++clientID)
	{
		if(players[clientID] == NULL)
		{
			break;
		}
	}

	// Socket is ready, there is an impending connection
	sf::TcpSocket* client = new sf::TcpSocket;
	if(serverSocket.accept(*client) == sf::Socket::Done)
	{
		std::cout << "New connection recieved from " << client->getRemoteAddress() << std::endl;
		client->setBlocking(false);

		// If we have player slots available
		if (clientID != MAX_PLAYERS)
		{
			std::cout << "Player slots are available, welcome player " << clientID + 1 << std::endl;
			// TODO: confirm connection type with client and swtich to UDP
			std::size_t sizeSent = 0;
			client->send((void*)clientID, sizeof(clientID), sizeSent);

			// If the data wasn't fully sent
			//if (sizeSent != sizeof(isPlayer))
			while (sizeSent != sizeof(clientID))
			{
				sizeSent = 0;
				client->send((void*)clientID, sizeof(clientID), sizeSent);
			}

			// Populate playerData
			players[clientID] = new PlayerData();
			players[clientID]->clientID = clientID;
		}
		else
		{
			// No player slots available, add the client as a spectator
			std::cout << "No player slots available, welcome spectator!";
			std::size_t sizeSent = 0;
			clientID = -1;

			client->send((void*)clientID, sizeof(clientID), sizeSent);

			while (sizeSent != sizeof(clientID))
			{
				sizeSent = 0;
				client->send((void*)clientID, sizeof(clientID), sizeSent);
			}

			spectatorSockets.push_back(client);
		}

			selector.add(*client);
	}
	else
	{
		// We didn't get a new connection, delete the socket
		delete client;
	}
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

	// Clearing up the spectator pointers
	for(auto i : spectatorSockets)
	{
		i->disconnect();
		delete(&i);
	}

	spectatorSockets.clear();

	// Ensuring that the socket is closed
	serverSocket.close();
}

void Server::die(const char* message)
{
	#ifdef _WIN32
	std::cout << "Error: " << message << " (WSAGetLastError() = " << WSAGetLastError() << ")" << std::endl;
	#elif __linux__
	std::cout << "Error: " << message << " (errno = " << errno << ")" << std::endl;
	#endif

	exit(1);
}
