#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#define EWOULDBLOCK WSAEWOULDBLOCK
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

typedef int SOCKET
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
//Come up with comething more C++ than define.
#define closesocket close
#endif

void socketService_init();
void socketService_close();

bool socket_setBlocking(SOCKET const &socket, bool const block);
