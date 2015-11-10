#ifndef SERVER_H
#define SERVER_H
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>
#include <list>
#include "PlayerData.h"

#define MAX_PLAYERS 4
#define SERVERPORT 5555
#define SERVERIP "127.0.0.1"

class Server
{
public:
	Server();
	~Server();
	int run();
	void die(const char *message);

private:
	void listen();
	void checkClients();

private:
	sf::IpAddress serverIP;
	sf::UdpSocket playerSockets[MAX_PLAYERS];
	std::list<sf::TcpSocket*> spectatorSockets;
	sf::TcpListener serverSocket;
	sf::SocketSelector selector;

	sf::Clock clock;
	PlayerData* players[MAX_PLAYERS];
};
#endif//SERVER_H
