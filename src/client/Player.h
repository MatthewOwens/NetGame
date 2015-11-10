#ifndef PLAYER_H
#define PLAYER_H
#include <SFML/Network/Packet.hpp>
#include "InputManager.h"
#include "PlayerData.h"
class Player
{
	public:
		Player();
		Player(InputManager* input);
		~Player();

	private:
		InputManager* inputManager;
		sf::Int32 playerID;
		PlayerData playerData;
};
#endif//PLAYER_H
