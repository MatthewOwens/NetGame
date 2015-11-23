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

	status = tcpSocket->receive(&player->getID(), sizeof(player->getID()), received);

	while(status != sf::Socket::Done)
		status = tcpSocket->receive(&player->getID(), sizeof(player->getID()), received);

	std::cout << "Connected successfully, client ID of " << (int)player->getID() << " was assigned!" << std::endl;

	if ((int)player->getID() != -1)
	{
		sf::IpAddress confirmedIP;
		unsigned short confirmedPort;
		std::cout << "UDP handshake... ";

		// Init the UdpSocket if we're not a spectator
		udpSocket = new sf::UdpSocket();
		udpSocket->setBlocking(false);
		udpSocket->bind(udpSocket->AnyPort);

		// Clearing the TCP socket since we won't be using it
		delete tcpSocket;
		tcpSocket = 0;

		sf::Uint16 conf = 0;
		status = udpSocket->send(&conf, sizeof(conf), SERVERIP, SERVERPORT);

		while (status == sf::Socket::NotReady)
		{
			status = udpSocket->send(&conf, sizeof(conf), SERVERIP, SERVERPORT);
		}

		if (status == sf::Socket::Error)
		{
			// TODO: Handle Errors
		}

		while (status == sf::Socket::NotReady || conf == 0)
		{
			status = udpSocket->receive(&conf, sizeof(conf), received,
					 confirmedIP, confirmedPort);
		}
		if (status == sf::Socket::Error)
		{
			// TODO: Handle Errors
		}

		status = sf::Socket::NotReady;
		conf = 0;

		while (status == sf::Socket::NotReady)
		{
			status = udpSocket->send(&conf, sizeof(conf), SERVERIP, SERVERPORT);
		}
		if (status == sf::Socket::Error)
		{
			// TODO: Handle Errors
		}

		std::cout << "OK!";
	}

	player->setColour();

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

	updateClock.restart();

	return 0;
}

void Client::update()
{
	sf::Socket::Status status;
	sf::Packet packet;

	sf::IpAddress remoteIP;
	unsigned short remotePort;

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

	player->update(tiles);

	// Checking if we should send an update packet
	if (updateClock.getElapsedTime().asMilliseconds() >= 20)
	{
		player->getData().updateTime += updateClock.getElapsedTime().asMilliseconds();
		if (player->getAttacking())
			player->getData().atkTimer = player->getAtkTime();
		else
			player->getData().atkTimer = 0.0f;

		packet << player->getData();
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
			PlayerData incomingData;

			if (packet >> incomingData)
			{
				int id = incomingData.clientID;

				// Creating a new NetworkedPlayer if we've got a new client
				if (otherPlayers[id] == NULL)
					otherPlayers[id] = new NetworkedPlayer(incomingData);

				// Updating our data for the other player if it's newer
				if (incomingData.updateTime > otherPlayers[id]->getUpdateTime())
				{
					otherPlayers[id]->update(incomingData);
				}
			}
		}
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
