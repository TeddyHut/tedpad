#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

namespace tedpad {
	namespace socket_service {
		typedef ::SOCKET SOCKET;
		constexpr int ERROR_EWOULDBLOCK = WSAEWOULDBLOCK;
		constexpr int ERROR_WAGAIN = WSAEWOULDBLOCK;
	}
}

#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

namespace tedpad {
	namespace socket_service {
		typedef int SOCKET;
		constexpr int INVALID_SOCKET = -1;
		constexpr int SOCKET_ERROR - 1;

		constexpr int ERROR_EWOULDBLOCK = EWOULDBLOCK;
		constexpr int ERROR_EAGAIN = EAGAIN;
	}
}
#endif

namespace tedpad {
	namespace socket_service {
		void startup();
		void shutdown();

		int get_lastError();
		bool socket_setBlocking(SOCKET const &socket, bool const block);
	}
}
