#include "Server.hh"

#if DEBUG
# define NETWORK 1
#else
# define NETWORK 0 // No network in release!
#endif

#if NETWORK

#include "ProjectLoader.hh"
#include "engine/core/Debug.hh"
#include "engine/timeline/Variable.hh"
#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace Tool;

const int bufferSize = 32000;
const int port = 2442;

SOCKET serverSocket = 0;
SOCKET clientSocket = 0;

void Server::Init()
{
	WSADATA WSAData;
	SOCKADDR_IN sin;
	WSAStartup(MAKEWORD(2,0), &WSAData);
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	bind(serverSocket, (SOCKADDR *)&sin, sizeof(sin));
	listen(serverSocket, 0);
}

void Server::DoIO(long currentTime)
{
	fd_set readfds;

	FD_ZERO(&readfds);
	// Set server socket to set
	FD_SET(serverSocket, &readfds);
	// Insert client sockets to set
	if (clientSocket != 0)
	{
		FD_SET(clientSocket, &readfds);
	}

	if (clientSocket != 0)
	{
		char message[50] = {0};
		sprintf(message, "t=%d\n", currentTime);

		int err = send(clientSocket, message, strlen(message), 0);
		if (err == SOCKET_ERROR)
		{
			LOG_ERROR("socket error");
		}
	}

	// Timeout parameter
	timeval tv = { 0, 0 };

	int ret = select(0, &readfds, NULL, NULL, &tv);
	if (ret > 0)
	{
		if (FD_ISSET(serverSocket, &readfds))
		{
			// Accept incoming connection and add new socket to list
			SOCKET newSocket = accept(serverSocket, NULL, NULL);
			clientSocket = newSocket;
			LOG_INFO("[NW] New client");
		}

		if (FD_ISSET(clientSocket, &readfds))
		{
			// Handle client request
			char buffer[bufferSize];
			int nb = recv(clientSocket, buffer, bufferSize, 0);
			if (nb <= 0)
			{
				// disconnect
				clientSocket = 0;
				LOG_INFO("[NW] client exit");
				return;
			}
			buffer[nb] = 0;
			//LOG_DEBUG("[NW] recv: %s", buffer);
			ProjectLoader::readInput(buffer);
		}
	}
	else
	{
		//LOG_DEBUG("[NW] Nothing to do");
	}
}

#endif // NETWORK
