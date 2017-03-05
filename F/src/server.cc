#include <stdio.h>
#include <winsock2.h>

#include "array.hh"
#include "sys/msys_debug.h"
#include "variable.hh"

#if DEBUG
# define NETWORK 1
#else
# define NETWORK 0 // pas de réseau en release !
#endif

#if NETWORK
#pragma comment(lib, "ws2_32.lib")

const int port = 2442;

namespace Server
{
  SOCKET sock;	    // socket du serveur
  SOCKET csock = 0; // socket du client

  char* readArray(char* input, Array<float>& arr)
  {
    int len;
    for (int i = 0; i < arr.max_size; i++)
      {
	float f;
	int ret = sscanf(input, "%f%n", &f, &len);
	assert(ret == 1);
	arr.add(f);
	input += len;
      }
    sscanf(input, " ;%n", &len);
    assert(len > 0);
    input += len;
    return input;
  }

  void readInput(char* input)
  {
    char name[80];
    int dim, size, len;
    sscanf(input, "%80[^ \n] %d %d %n", name, &size, &dim, &len);
    input += len;

    Array<float> times_f(size);
    input = readArray(input, times_f);
    Array<float> data(size * dim);
    input = readArray(input, data);

    // conversion float* -> int*
    int *times = (int*) msys_mallocAlloc(sizeof(int) * times_f.size);
    for (int i = 0; i < times_f.size; i++)
      times[i] = msys_ifloorf(times_f[i]);

    Variable *v = Variables::find(name);
    if (v == NULL)
    {
      DBG("Variable '%s' inconnue. Ajout dans la table", name);
      v = new Variable(times, data.elt, dim, size);
      Variables::define(name, v);
    }
    else
    {
      v->update(times, data.elt, dim, size);
    }
    msys_mallocFree(times);
  }

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

    // Timeout parameter
    timeval tv = { 0, 0 };

    int ret = select(0, &readfds, NULL, NULL, &tv);
    if (ret > 0) {
      if (FD_ISSET(sock, &readfds)) {
	  // Accept incoming connection and add new socket to list
	  SOCKET newSocket = accept(sock, NULL, NULL);
	  csock = newSocket;
	  printf("New client\n");
      }

      if (FD_ISSET(csock, &readfds)) {
	  // Handle client request
	  char buffer[100];
	  int nb = recv(csock, buffer, 100, 0);
	  if (nb < 0)
	  {
	    // disconnect
	    csock = 0;
	    printf("client exit\n");
	    return;
	  }
	  buffer[nb] = 0;
	  printf("recv: %s\n", buffer);
	  //send(csock, "foobar", 6, 0);
      }
    } else {
      printf("nothing to do\n");
    }
  }

}
#endif
