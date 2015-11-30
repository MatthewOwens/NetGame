#ifndef PLAYER_H
#define PLAYER_H
#include <SFML/Network/Packet.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Clock.hpp>
#include <vector>
#include "InputManager.h"
#include "PlayerData.h"
#include "Tile.h"

class Player
{
	public:
		Player();
		Player(InputManager* input);
		~Player();
		bool update(Tile** tiles, std::vector<sf::RectangleShape>& collisions);
		void render(sf::RenderWindow& window);
		void setColour();
		sf::Int8& getID() {return playerData.clientID; }
		PlayerData& getData() { return playerData; }
		float getAtkTime() { return atkTimer.getElapsedTime().asSeconds(); }
		bool getAttacking() { return attacking; }

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
