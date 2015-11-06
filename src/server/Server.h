#ifndef SERVER_H
#define SERVER_H
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <list>
#include "Utils.h"

#define MAX_PLAYERS 4

class Server
{
public:
	Server();
	~Server();
	int run();

private:
	// Struct to describe the data for each player
	struct PlayerData
	{
		sf::Vector2i position;
		sf::Vector2f velocity;
		sf::Int32 state;
		sf::Time updateTime;	// When the last message from the player was recieved

		PlayerData()
		{
			position = sf::Vector2i(0,0);
			velocity = sf::Vector2f(0.0f, 0.0f);
			state = (sf::Int32)AnimationState::IDLE;
			updateTime = sf::Time::Zero;
		}
	};

	sf::IpAddress serverIP;
	sf::UdpSocket playerSockets[MAX_PLAYERS];
	std::list<sf::TcpSocket> spectatorSockets;
	sf::TcpListener serverSocket;

	sf::Clock clock;
	PlayerData* players[MAX_PLAYERS];

};
#endif//SERVER_H
