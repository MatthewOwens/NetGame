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

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#define MAX_PLAYERS 4
#define DISCONNECT_TIME_MS 5000
#define INTERP_TIME_MS 2500

class Server
{
public:
	struct Machine
	{
		sf::IpAddress ip;
		unsigned short port;
		float timeSinceUpdate;
	};

	Server();
	~Server();
	int run();
	void die(const char *message);

private:
	void listen();
	void checkClients();

private:
	sf::IpAddress serverIP;
	sf::UdpSocket playerSocket;
	std::list<sf::TcpSocket*> spectatorSockets;
	sf::TcpListener serverSocket;
	sf::SocketSelector selector;

	sf::Clock clock;
	sf::Clock frameClock;
	Machine* machines[MAX_PLAYERS];
	PlayerData* players[MAX_PLAYERS];
};
#endif//SERVER_H
