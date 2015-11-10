#include "Server.h"
#include <iostream>
#include <SFML/Window/Keyboard.hpp>

int Server::run()
{
	std::cout << "Server starting..." << std::endl;
	std::cout << "The server can currently only be killed by the OS" << std::endl;
	//std::cout << "Press 'q' to quit" << std::endl;

	// Creating the socket that the server will listen on
	sf::Socket::Status listenStatus = serverSocket.listen(SERVERPORT);

	if(listenStatus != sf::Socket::Done)
	{
		// Something's gone wrong with creating the socket, but what?
		if(listenStatus == sf::Socket::Error)
			die("listen failed!");
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
	bool playersAccept = false;

	// Checking if we can connect any more players
	for(int i = 0; i < MAX_PLAYERS; ++i)
	{
		if(players[i] == NULL)
		{
			playersAccept = true;
			break;
		}
	}

	// Socket is ready, there is an impending connection
	sf::TcpSocket* client = new sf::TcpSocket;
	if(serverSocket.accept(*client) == sf::Socket::Done)
	{
		if(playersAccept)
		{
			// Player connection available
			// TODO: confirm connection type with client and swtich to UDP
		}
		else
		{
			client->setBlocking(false);
			spectatorSockets.push_back(client);
			selector.add(*client);
		}
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
