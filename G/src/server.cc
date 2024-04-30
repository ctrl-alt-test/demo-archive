#include <stdio.h>
#include <winsock2.h>

#include "intro.hh"
#include "project_loader.hh"
#include "variable.hh"

#include "sys/msys_debug.h"

#if DEBUG
# define NETWORK 1
#else
# define NETWORK 0 // pas de réseau en release !
#endif

#if NETWORK
#pragma comment(lib, "ws2_32.lib")

const int bufferSize = 32000;
const int port = 2442;

namespace Server
{
  SOCKET sock;	    // socket du serveur
  SOCKET csock = 0; // socket du client

  void init()
  {
    WSADATA WSAData;
    SOCKADDR_IN sin;
    WSAStartup(MAKEWORD(2,0), &WSAData);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    bind(sock, (SOCKADDR *)&sin, sizeof(sin));
    listen(sock, 0);
  }

  void doIO()
  {
    fd_set readfds;

    FD_ZERO(&readfds);
    // Set server socket to set
    FD_SET(sock, &readfds);
    // Insert client sockets to set
    if (csock != 0) FD_SET(csock, &readfds);

    if (csock != 0)
    {
      char message[50] = {0};
      sprintf(message, "t=%d\n", intro.now.story);

      int err = send(csock, message, strlen(message), 0);
      if (err == SOCKET_ERROR) {
        DBG("socket error");
      }
    }

    // Timeout parameter
    timeval tv = { 0, 0 };

    int ret = select(0, &readfds, NULL, NULL, &tv);
    if (ret > 0) {
      if (FD_ISSET(sock, &readfds)) {
	  // Accept incoming connection and add new socket to list
	  SOCKET newSocket = accept(sock, NULL, NULL);
	  csock = newSocket;
	  DBG("[NW] New client");
      }

      if (FD_ISSET(csock, &readfds)) {
	  // Handle client request
	  char buffer[bufferSize];
	  int nb = recv(csock, buffer, bufferSize, 0);
	  if (nb <= 0)
	  {
	    // disconnect
	    csock = 0;
	    DBG("[NW] client exit");
	    return;
	  }
	  buffer[nb] = 0;
	  //DBG("[NW] recv: %s", buffer);
          ProjectLoader::readInput(buffer);
      }
    } else {
      //DBG("nothing to do\n");
    }
  }

}
#endif
