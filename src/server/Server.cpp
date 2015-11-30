#include "Server.h"
#include <iostream>
#include <SFML/Window/Keyboard.hpp>
#include <cstdio>

#define UNCONFIRMED  -1

int Server::run()
{
	std::cout << "Server starting..." << std::endl;
	std::cout << "The server can currently only be killed by the OS, sorry!" << std::endl;
	std::cout << std::endl;
	//std::cout << "Press 'q' to quit" << std::endl;

	// Creating the socket that the server will listen on
	sf::Socket::Status listenStatus = serverSocket.listen(SERVERPORT);

	if (listenStatus != sf::Socket::Done)
	{
		// Something's gone wrong with creating the socket, but what?
		if (listenStatus == sf::Socket::Error)
			die("listen failed!"); //TODO: Fix rather than just death
	}

	serverSocket.setBlocking(false);

	// Populating our socket selector
	selector.add(serverSocket);

	selector.add(playerSocket);

	while (true)
	{
		std::cout << clock.getElapsedTime().asMilliseconds() << std::endl;
		//if(selector.wait(sf::miliseconds(100)))
		if (selector.wait())
		{
			// If we recieved something on the server socket
			if (selector.isReady(serverSocket))
			{
				listen();
			}
			else
			{
				checkClients();
			}
		}

		frameClock.restart();
	}

	return 0;
}

void Server::checkClients()
{
	// Updating our timers & checking for disconnects
	for(int i = 0; i < MAX_PLAYERS; ++i)
	{
		if(machines[i] != NULL)
		{
			if(machines[i]->timeSinceUpdate >= DISCONNECT_TIME_MS)
			{
				std::cout << "Client " << i + 1 << " has timed out!" << std::endl;

				delete players[i];
				delete machines[i];

				players[i] = NULL;
				machines[i] = NULL;
			}
		}
	}

	//Checking the players
	if(selector.isReady(playerSocket))
	{
		std::size_t recieved = 0;
		sf::IpAddress sender;
		unsigned short port = 0;

		sf::Socket::Status status = sf::Socket::NotReady;
		sf::Packet packet;
		//status = playerSocket.receive(buffer, sizeof(buffer), recieved, sender, port);
		status = playerSocket.receive(packet, sender, port);

		// If everything has been recieved
		if (status == sf::Socket::Done)
		{
			recieved = packet.getDataSize();
			// If the client disconnected
			if (recieved == 0)
			{
				std::cout << "Client disconnected" << std::endl;

				for (int i = 0; i < MAX_PLAYERS; ++i)
				{
					// Freeing up the player and machine data of the
					// client that disconnected.

					// TODO: Handle random disconnects
					if (machines[i] != NULL)
					{
						if (sender == machines[i]->ip &&
							port == machines[i]->port)
						{
							delete machines[i];
							delete players[i];

							machines[i] = 0;
							players[i] = 0;
						}
					}
				}
			}
			else
			{
				// Client connected alright
				PlayerData incomingData;

				if (packet >> incomingData)
				{
					int id = incomingData.clientID;

					// Updating our data for a client if this packet was newer
					if (incomingData.updateTime > players[id]->updateTime)
					{
						*players[id] = incomingData;

						// Pushing out the update to our remaining clients
						for (int i = 0; i < MAX_PLAYERS; ++i)
						{
							// Only sending the update to valid clients
							if (i != id && players[i] != NULL)
							{
								status = playerSocket.send(packet, machines[i]->ip, machines[i]->port);

								if (status == sf::Socket::NotReady)
								{
									std::cout << "NOT_READY" << std::endl;
									status = playerSocket.send(packet, machines[i]->ip, machines[i]->port);
								}

								if (status == sf::Socket::Error)
								{
									// TODO: Error handling
									std::cout << "Error sending sf::packet" << std::endl;
								}
							}
						}
					}
					else
					{
						std::cout << "Old packet!" << std::endl;
						std::cout << "\t Current: " << players[id]->updateTime << std::endl;
						std::cout << "\t New: " << incomingData.updateTime << std::endl;
					}
				}
				else
				{
					// Error extracting packet, check if it was from a client
					int errClient = 0;
					for (; errClient < MAX_PLAYERS; ++errClient)
					{
						if (machines[errClient] != NULL)
						{
							if (sender == machines[errClient]->ip && port == machines[errClient]->port)
								break;
						}
					}
					if (errClient == MAX_PLAYERS)
					{
						std::cout << "Received an odd packet from someone else!" << std::endl;
					}
					else
						std::cout << "Error extracting packet from player " << errClient + 1 << std::endl;
				}
			}
		}
	}
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
			std::size_t sizeSent = 0;
			std::size_t sizeReceived = 0;
			sf::Int32 conf = UNCONFIRMED;
			sf::Clock timeout;
			sf::Clock pingClock;

			client->send(&clientID, sizeof(clientID), sizeSent);

			// If the data wasn't fully sent
			if (sizeSent != sizeof(clientID))
			{
				sizeSent = 0;
				client->send(&clientID, sizeof(clientID), sizeSent);
			}

			// Populate playerData
			players[clientID] = new PlayerData();
			players[clientID]->clientID = clientID;

			// Pupulating our machines list
			machines[clientID] = new Machine();
			machines[clientID]->ip = client->getRemoteAddress();
			machines[clientID]->port = 0;

			sf::Socket::Status status = sf::Socket::NotReady;
			std::cout << "Confirming client's UDP port... " << std::flush;

			// Ensuring that the packet was recieved
			timeout.restart();
			while (status == sf::Socket::NotReady)
			{
				if(timeout.getElapsedTime().asMilliseconds() >= 1000)
				{
					std::cerr << "ERROR: Confirming the client's UDP port timed out!" << std::endl;
					return;	// TODO: Proper error handling rather than just bailing
				}

				status = playerSocket.receive(&conf, sizeof(conf), sizeReceived,
						 machines[clientID]->ip, machines[clientID]->port);
			}

			std::cout << "OK!" << std::endl;
			//std::cout << "Sending the current server time to the client..." << std::flush;
			std::cout << "Sending " << conf << " as the current server time..." << std::flush;

			status = sf::Socket::NotReady;
			timeout.restart();
			pingClock.restart();

			while(status == sf::Socket::NotReady)
			{
				// Updating if we need to resend for accuracy.
				conf = clock.getElapsedTime().asMilliseconds();
				if(timeout.getElapsedTime().asMilliseconds() >= 1000)
				{
					std::cerr << "ERROR: Sending the server time to the client timed out!" << std::endl;
					return;
				}

				status = playerSocket.send(&conf, sizeof(conf),
						 machines[clientID]->ip, machines[clientID]->port);
			}

			std::cout << "OK!" << std::endl;
			std::cout << "Getting client ping time..." << std::flush;
			status = sf::Socket::NotReady;
			timeout.restart();

			while(status == sf::Socket::NotReady)
			{
				if(timeout.getElapsedTime().asMilliseconds() >= 1000)
				{
					std::cerr << "ERROR: Reply from the client timed out!" << std::endl;
					return;
				}

				status = playerSocket.receive(&conf, sizeof(conf), sizeReceived,
						 machines[clientID]->ip, machines[clientID]->port);
			}

			machines[clientID]->pingTime = pingClock.getElapsedTime().asMilliseconds();

			std::cout << conf << " ms!" << std::endl;
			std::cout << "Server measured a ping of " << machines[clientID]->pingTime << " ms." << std::endl;
			/*

			conf = UNCONFIRMED;
			status = sf::Socket::NotReady;
			std::cout << "OK!" << std::endl;
			std::cout << "Sending confirmation to the client... " << std::flush;

			// Sending a confirmation to the client
			while (status == sf::Socket::NotReady)
			{
				status = playerSocket.send(&conf, sizeof(conf),
						 machines[clientID]->ip, machines[clientID]->port);
			}

			std::cout << "OK!" << std::endl;
			std::cout << "Waiting for client acknowledgement... " << std::flush;

			while (status == sf::Socket::NotReady || conf == UNCONFIRMED)
			{
				status = playerSocket.receive(&conf, sizeof(conf), sizeReceived,
						 machines[clientID]->ip, machines[clientID]->port);
			}

			std::cout << "OK!" << std::endl;
			*/

		}
		else
		{
			// No player slots available, add the client as a spectator
			std::cout << "No player slots available, welcome spectator!" << std::endl;
			std::size_t sizeSent = 0;
			clientID = -1;

			client->send(&clientID, sizeof(clientID), sizeSent);

			while (sizeSent != sizeof(clientID))
			{
				sizeSent = 0;
				client->send(&clientID, sizeof(clientID), sizeSent);
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

	// Initilising our udp socket
	playerSocket.bind(SERVERPORT);

	// Ensuring our player data is all voided
	for(int i = 0; i < MAX_PLAYERS; ++i)
	{
		machines[i] = NULL;
		players[i] = NULL;
	}
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
