#include "Client.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <iostream>

Client::Client() : windowBounds(640, 640)
{
	window.create(sf::VideoMode(windowBounds.x ,windowBounds.y), "NetGame - Client");
	window.setFramerateLimit(60);

	tcpSocket = NULL;
	udpSocket = NULL;
	pingTime = -1;
	serverStartTime = -1;
	
	// Loading the assets that we'll be needing
	imageManager.loadImage("assets/tileSheet.png", "tileSheet");

	// Setting up the player
	player = new Player(&inputManager);

	// Ensuring that the other players are null
	for(int i = 0; i < 3; ++i)
		otherPlayers[i] = NULL;

	// Grabbing the tileSheet texture ref to avoid calling getImage 400 times
	//sf::Texture& tileSheet = imageManager.getTexture("tileSheet");
	const int tileSize = Tile::getSize();

	//Array to init tiles from
	int ids[20][20] =
	{
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};

	// Defining the tile array
	tiles = new Tile*[20];

	for (int i = 0; i < 20; ++i)
		tiles[i] = new Tile[20];

	for(int i = 0; i < 20; i++)
	{
		for(int j = 0; j < 20; j++)
		{
			tiles[j][i] = Tile(ids[i][j], imageManager.getTexture("tileSheet"));
			tiles[j][i].getSprite().setPosition(j * tileSize, i * tileSize);
		}
	}
}

Client::~Client()
{
	imageManager.unloadImage("tileSheet");

	delete player;
	player = NULL;

	delete tcpSocket;
	tcpSocket = NULL;

	delete udpSocket;
	udpSocket = NULL;
}

int Client::run()
{
	// Connecting to the server
	std::cout << "Connecting... " << std::endl;
	tcpSocket = new sf::TcpSocket();
	tcpSocket->setBlocking(false);
	tcpSocket->connect(sf::IpAddress(SERVERIP), SERVERPORT);

	// Getting our ID from the server
	std::size_t received = 0;
	sf::Socket::Status status;
	sf::Clock tcpTimeout;

	status = tcpSocket->receive(&player->getID(), sizeof(player->getID()), received);

	while(status != sf::Socket::Done)
	{
		status = tcpSocket->receive(&player->getID(), sizeof(player->getID()), received);

		if(status == sf::Socket::Error)
		{
			#ifdef __linux__
			std::cout << "Error: " << errno << std::endl;
			#elif __WIN32
			std::cout << "aah!" << std::endl;
			#endif
		   	exit(-1);
		}

		if(tcpTimeout.getElapsedTime().asMilliseconds() >= 5000)
		{
			std::cout << "Error: Could not establish connection with the server!" << std::endl;
			exit(-1);
		}
	}

	std::cout << "Connected successfully, client ID of " << (int)player->getID() << " was assigned!" << std::endl;

	if ((int)player->getID() != -1)
	{
		sf::IpAddress confirmedIP;
		unsigned short confirmedPort;
		sf::Clock timeout;
		sf::Clock pingClock;

		// Init the UdpSocket if we're not a spectator
		udpSocket = new sf::UdpSocket();
		udpSocket->setBlocking(false);
		udpSocket->bind(udpSocket->AnyPort);

		// Clearing the TCP socket since we won't be using it
		delete tcpSocket;
		tcpSocket = 0;
		sf::Int32 conf = 0;

		// Sending a packet to tell the server what UDP port the client is using
		status = udpSocket->send(&conf, sizeof(conf), SERVERIP, SERVERPORT);
		timeout.restart();
		pingClock.restart();

		while (status == sf::Socket::NotReady)
		{
			if(timeout.getElapsedTime().asMilliseconds() >= 1000)
			{
				std::cerr << "Sending UDP confirm packet timed out!" << std::endl;
				exit(-1); // TODO: Proper error handling rather than just bailing.
			}
			status = udpSocket->send(&conf, sizeof(conf), SERVERIP, SERVERPORT);
		}

		if (status == sf::Socket::Error)
		{
			// TODO: Handle Errors
		}

		// Getting the current time from the server
		timeout.restart();
		status = sf::Socket::NotReady;
		while (status == sf::Socket::NotReady)
		{
			if(timeout.getElapsedTime().asMilliseconds() >= 1000)
			{
				std::cerr << "Receiveing server time packet timed out!" << std::endl;
				exit(-1);
			}

			status = udpSocket->receive(&serverStartTime, sizeof(serverStartTime), received,
					 confirmedIP, confirmedPort);
		}

		std::cout << "Received a current time of " << serverStartTime << " from the server!" << std::endl;
		
		// Establishing a rough ping time with the server
		pingTime = (pingClock.getElapsedTime().asMilliseconds() / 2);
		std::cout << "Ping time: " << pingTime << " ms" << std::endl;

		if (status == sf::Socket::Error)
		{
			// TODO: Handle Errors
		}

		// Final response
		timeout.restart();
		status = sf::Socket::NotReady;
		while (status == sf::Socket::NotReady)
		{
			if(timeout.getElapsedTime().asMilliseconds() >= 1000)
			{
				std::cerr << "Receiveing server time packet timed out!" << std::endl;
				exit(-1);
			}

			status = udpSocket->send(&pingTime, sizeof(pingTime), SERVERIP, SERVERPORT);
		}

	}

	player->setColour();
	clock.restart();

	// Our actual game loop
	while(!close)
	{
		update();
		render();
	}

	// Sending our disconnect packet
	sf::Packet message;
	status = udpSocket->send(message, SERVERIP, SERVERPORT);

	// Ensuring that it's actually sent
	while (status == sf::Socket::NotReady)
		status = udpSocket->send(message, SERVERIP, SERVERPORT);


	return 0;
}

void Client::update()
{
	sf::Socket::Status status;
	sf::Packet packet;
	std::vector<sf::RectangleShape> collisions;

	sf::IpAddress remoteIP;
	unsigned short remotePort;
	updateClock.restart();

	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			close = true;
		else if (event.type == sf::Event::GainedFocus)
			windowFocus = true;
		else if (event.type == sf::Event::LostFocus)
			windowFocus = false;
	}

	inputManager.update(window, windowFocus);

	// Checking if we should close the window
	if (inputManager.pressedOnce("close"))
		close = true;

	// Populating our collisions array
	for(int i = 0; i < 3; ++i)
	{
		if(otherPlayers[i] != NULL)
		{
			/* Slight difference in the attack timings here, since our timer will never
			 * _actually_ reach 0.7f, as when it does it's reset on the other client,
			 * pushed to the server and to here.
			 */
			if(otherPlayers[i]->getData().atkTimer >= 0.6f)
			{
				collisions.push_back(otherPlayers[i]->getAtkSprite());
			}
		}
	}

	sendUpdate = player->update(tiles, collisions);

	// Checking if we should send an update packet
	//if (updateClock.getElapsedTime().asMilliseconds() >= 20)
	if(sendUpdate)
	{
		sf::Int8 header = 1;

		player->getData().updateTime = serverTime();

		if (player->getAttacking())
			player->getData().atkTimer = player->getAtkTime();
		else
			player->getData().atkTimer = 0.0f;

		packet << header << player->getData();
		status = udpSocket->send(packet, SERVERIP, SERVERPORT);

		if (status == sf::Socket::Done)
		{
			updateClock.restart();
		}
		else if (status == sf::Socket::Error)
		{
			std::cout << "Error occured while sending update packet!" << std::endl;
		}
	}

	// Checking to see if we've got any updates from the server
	status = udpSocket->receive(packet, remoteIP, remotePort);

	if (status == sf::Socket::Done)
	{
		// Only bother with the packet if it was actually from the server
		if (remoteIP == SERVERIP && remotePort == SERVERPORT)
		{
			sf::Int8 header = -1;

			if(packet >> header)
			{
				switch(header)
				{
					case(0):
					{
						sf::Int8 disconnectedClient;
						packet >> disconnectedClient;
						std::cout << "Client " << (int)disconnectedClient + 1<< " has disconnected!" << std::endl;

						delete otherPlayers[disconnectedClient];
						otherPlayers[disconnectedClient] = NULL;
						break;
					}
					case(1):
					{
						PlayerData incomingData;
						packet >> incomingData;

						int id = incomingData.clientID;
						// Creating a new NetworkedPlayer if we've got a new client
						if (otherPlayers[id] == NULL)
							otherPlayers[id] = new NetworkedPlayer(incomingData);

						// Updating our data for the other player if it's newer
						if (incomingData.updateTime > otherPlayers[id]->getUpdateTime())
						{
							otherPlayers[id]->updateData(incomingData, serverTime());
						}
						break;
					}
					case(2):
					{
						// Ping packet, extract the info and throw it back
						status = sf::Socket::NotReady;
						sf::Clock replyTimer;
						packet >> pingTime;

						if(!(packet << header << pingTime))
							std::cout << "Error creating ping packet!" << std::endl;

						while(status == sf::Socket::NotReady)
						{
							status = udpSocket->send(packet, SERVERIP, SERVERPORT);

							if(replyTimer.getElapsedTime().asMilliseconds() == 1000)
								std::cout << "Ping reply took way too long!" << std::endl;
						}
						break;
					}
					default:
					{
						std::cout << "Error: " << (int)header << " is an unknown header!" << std::endl;
						break;
					}
				};
			}
		}
	}

	// Updating our simulated players
	for(int i = 0; i < 3; ++i)
	{
		if(otherPlayers[i] != NULL)
			otherPlayers[i]->update(serverTime(), updateClock.getElapsedTime().asMilliseconds());
	}
}

void Client::render()
{
	window.clear(sf::Color(0, 50, 200));

	// Drawing the tiles
	for(int i = 0; i < 20; ++i)
	{
		for(int j = 0; j < 20; ++j)
			window.draw(tiles[i][j].getSprite());
	}

	// Drawing the players
	for (int i = 0; i < 3; ++i)
	{
		if (otherPlayers[i] != NULL)
			otherPlayers[i]->render(window);
	}
	player->render(window);

	window.display();
}

int Client::getVectorPos(int x, int y)
{
	return (Tile::getSize() * y) + x;
}
