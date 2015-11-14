#ifndef PLAYERDATA_H
#define PLAYERDATA_H
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Network/Packet.hpp>
class PlayerData
{
	friend class Player;
public:
	// Enumeration to describe the state of the player, placed here to access from the server
	enum AnimationState {IDLE, LEFT, RIGHT, SWING};

	PlayerData();

	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::Int32 state;
	sf::Int32 updateTime;	// When the last message from the player was recieved in milliseconds

	friend sf::Packet& operator >> (sf::Packet& packet, PlayerData& m);
	friend sf::Packet& operator << (sf::Packet& packet, const PlayerData& m);

};
#endif//PLAYERDATA_H
