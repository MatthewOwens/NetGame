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
		std::cout << "Sending UDP packet" << std::endl;
		// Init the UdpSocket if we're not a spectator
		udpSocket = new sf::UdpSocket();
		//udpSocket->bind(udpSocket->AnyPort);
		udpSocket->bind(55576);

		// Clearing the TCP socket since we won't be using it
		delete tcpSocket;
		tcpSocket = 0;

		std::string message = "Client connected via udp\n";
		udpSocket->send(message.c_str(), message.size() + 1, SERVERIP, SERVERPORT);
	}


	while(!close)
	{
		update();
		render();
	}

	sf::Int8 message = 0;
	std::cout << "message size is " << sizeof(message) << std::endl;
	status = udpSocket->send(&message, std::size_t(message), SERVERIP, SERVERPORT);

	while (status == sf::Socket::NotReady)
		status = udpSocket->send(&message, std::size_t(message), SERVERIP, SERVERPORT);

	return 0;
}

void Client::update()
{
	inputManager.update(window);

	// Checking if we should close the window
	if (inputManager.pressedOnce("close"))
		close = true;

	player->update(tiles);
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
	player->render(window);

	window.display();
}

int Client::getVectorPos(int x, int y)
{
	return (Tile::getSize() * y) + x;
}
