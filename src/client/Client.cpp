#include "Client.h"

Client::Client() : windowBounds(1280, 1280)
{
	window.create(sf::VideoMode(windowBounds.x ,windowBounds.y), "NetGame - Client");
	window.setFramerateLimit(60);
}

Client::~Client()
{

}

int Client::run()
{

	return 0;
}

int Client::getVectorPos(int x, int y)
{
	return (Tile::getSize() * y) + x;
}
