#include "Tile.h"

// Default constructor, just ensures that ID is a valid int
Tile::Tile()
{
	identifier = 0;
}

Tile::Tile(int id, sf::Texture& texture)
{
	identifier = id;

	// Setting the texture
	sprite.setTexture(texture);
	sf::IntRect rect;
	rect.width = getSize();
	rect.height = getSize();
	rect.top = 0;
	rect.left = id * rect.width;
	sprite.setTextureRect(rect);
}
