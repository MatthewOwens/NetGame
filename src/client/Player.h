#ifndef PLAYER_H
#define PLAYER_H
#include <SFML/Network/Packet.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "InputManager.h"
#include "PlayerData.h"
#include "Tile.h"

class Player
{
	public:
		Player();
		Player(InputManager* input);
		~Player();
		void update(Tile** tiles);
		void render(sf::RenderWindow& window);

	private:
		InputManager* inputManager;
		sf::Int32 playerID;
		PlayerData playerData;
		const float speed = 2.0f;
		bool falling = false;
		sf::RectangleShape sprite;
		sf::RectangleShape atkSprite;
		sf::Int32 previousState;
};
#endif//PLAYER_H
