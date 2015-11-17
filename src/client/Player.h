#ifndef PLAYER_H
#define PLAYER_H
#include <SFML/Network/Packet.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Clock.hpp>
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
		sf::Int8& getID() {return playerData.clientID; }

	private:
		InputManager* inputManager;
		PlayerData playerData;
		const float speed = 2.0f;
		bool falling = false;
		bool attacking = false;
		sf::RectangleShape sprite;
		sf::RectangleShape atkSprite;
		sf::Int32 previousState;
		sf::Clock atkTimer;
};
#endif//PLAYER_H
