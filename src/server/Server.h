#ifndef SERVER_H
#define SERVER_H
#include <SFML/UdpSocket.hpp>
#include <SFML/TcpSocket.hpp>
#include <SFML/Time.hpp>
#include <SFML/Clock.hpp>
#include <list>

#define MAX_PLAYERS 4

class Server
{
	Server();
	int run();

	private:
	sf::Clock clock;
	sf::UdpSocket playerSockets[MAX_PLAYERS];
	std::list<sf::TcpSocket> spectatorSockets;
	sf::TcpSocket serverSocket;
	PlayerData* players[MAX_PLAYERS];

	// Struct to describe the data for each player
	struct PlayerData
	{
		sf::Vector2i position;
		sf::Vector2f velocity;
		AnimationState state;
		sf::Time updateTime;	// When the last message from the player was recieved

		PlayerData()
		{
			position = sf::Vector2i(0,0);
			velocity = sf::Vector2f(0.0f, 0.0f);
			state = IDLE;
			updateTime = sf::Time::Zero;
		}
	};
};
#endif//SERVER_H
