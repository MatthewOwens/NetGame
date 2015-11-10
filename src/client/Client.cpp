#include "Client.h"
#include "SFML/Graphics/Texture.hpp"
#include <iostream>

Client::Client() : windowBounds(640, 640)
{
	window.create(sf::VideoMode(windowBounds.x ,windowBounds.y), "NetGame - Client");
	window.setFramerateLimit(60);
	
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
			tiles[i][j] = Tile(ids[i][j], imageManager.getTexture("tileSheet"));
			tiles[i][j].getSprite().setPosition(j * tileSize, i * tileSize);
		}
	}
}

Client::~Client()
{
	imageManager.unloadImage("tileSheet");
	delete player;
	player = NULL;
}

int Client::run()
{
	while(!close)
	{
		update();
		render();
	}

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
