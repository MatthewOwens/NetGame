#ifndef TILE_H
#define TILE_H
#include <SFML/Graphics.hpp>

class Tile
{
	public:
		Tile(int id, sf::Texture& texture);
		Tile();
		inline sf::Sprite& getSprite() { return sprite; }
		inline int getID() { return identifier; }
		inline static const int getSize() { return 32; }
	private:
		sf::Sprite sprite;
		int identifier;
};
#endif//TILE_H
