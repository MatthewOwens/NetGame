#include "Utils.h"
#include <iostream>

#ifdef _WIN32
	#include <WinSock2.h>
	#include <Windows.h>
	#pragma comment(lib, "ws2_32.lib")
#elif __linux__
	#include <errno.h>
#endif 

void die(const char* message)
{
	#ifdef _WIN32
	std::cout << "Error: " << message << " (WSAGetLastError() = " << WSAGetLastError() << ")" << std::endl;
	#elif __linux__
	std::cout << "Error: " << message << " (errno = " << errno << ")" << std::endl;
	#endif

	exit(1);
}
