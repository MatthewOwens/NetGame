#ifndef CLIENT_H
#define CLIENT_H
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include "InputManager.h"
#include "ImageManager.h"
#include "Tile.h"
class Client
{
	public:
		Client();
		~Client();
		int run();
	private:
		// Returns the position in the vector for the tile (x,y) onscreen
		int getVectorPos(int x, int y);
		void update();
		void render();

	private:
		bool close = false;
		const sf::Vector2i windowBounds;

		InputManager inputManager;
		ImageManager imageManager;
		sf::RenderWindow window;
		sf::Event event;
		//std::vector<Tile> tiles;
		Tile tiles[20][20];
};
#endif//CLIENT_H
