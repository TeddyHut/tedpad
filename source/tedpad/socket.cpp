#include "../../include/tedpad/socket.h"

#ifdef _WIN32
void socketService_init()
{
	WSAData	wsaData;
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		std::cerr << "WSAStartup failed." << std::endl;
		exit(1);
	}
}

void socketService_close()
{
	WSACleanup();
}

bool socket_setBlocking(SOCKET const & socket, bool const block)
{
	unsigned long mode = block ? 0 : 1;
	return (ioctlsocket(socket, FIONBIO, &mode) == 0) ? false : true;
}

#else
void socketService_init() {}
void socketService_close() {}

bool socket_setBlocking(SOCKET const & socket, bool const block)
{
	int flags = fcntl(socket, F_GETFL, 0);
	if (flags < 0) return true;
	flags = block ? (flags &~ O_NONBLOCK) : (flags | O_NONBLOCK);
	return (fcntl(socket, F_SETFL, flags) == 0) ? false : true;
}

#endif
