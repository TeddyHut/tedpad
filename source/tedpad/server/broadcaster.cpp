#include "../../../include/tedpad/server/broadcaster.h"

tedpad::intern_server::Broadcaster::Broadcaster(GamepadMutex const & gamepadMutex, Module::ServerDescription const &serverDescription, uint16_t const port, std::chrono::milliseconds const & updateRate) :
	pm_gamepadMutex(gamepadMutex),
	pm_serverDescription(serverDescription),
	pm_port(port),
	SleepObject(updateRate)
{
}

void tedpad::intern_server::Broadcaster::thread_init()
{
	std::lock_guard<std::mutex> lx_port(pmx_port);
	std::lock_guard<std::mutex> lx_gamepad(*pm_gamepadMutex.mx_gamepad);

	int result;
	
	memset(&pm_sockaddrDest.sin_zero, 0, sizeof(pm_sockaddrDest.sin_zero));
	pm_sockaddrDest.sin_family = AF_INET;
	pm_sockaddrDest.sin_port = htons(pm_port);
	inet_pton(AF_INET, "255.255.255.255", &pm_sockaddrDest.sin_addr);

	pm_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (pm_socket == INVALID_SOCKET) {
		std::cerr << "tedpad::intern_server::Broadcaster::thread_init(): socket error" << std::endl;
		exit(1);
	}

	char opt_broadcast = '1';
	result = setsockopt(pm_socket, SOL_SOCKET, SO_BROADCAST, &opt_broadcast, sizeof(opt_broadcast));
	if (result == -1) {
		std::cerr << "tedpad::inter_server::Broadcaster::thread_init(): setsockopt error" << std::endl;
		exit(1);
	}

	pm_gamepadDescription = pm_gamepadMutex.gamepad->to_gamepadBriefDescription();
}

void tedpad::intern_server::Broadcaster::thread_main()
{
	ToNetworkPacket toPacket;
	
	toPacket.add_module(pm_serverDescription.to_packetModule());
	toPacket.add_module(pm_gamepadDescription.to_packetModule());
	auto data = toPacket.get_fullPacketData();
	int result = sendto(pm_socket, reinterpret_cast<char const *>(data.data()), static_cast<int>(data.size()), 0, reinterpret_cast<sockaddr const *>(&pm_sockaddrDest), sizeof(sockaddr));
	if (result == -1) {
		std::cerr << "tedpad::intern_server::Broadcaster::thread_main(): sendto error" << std::endl;
		exit(1);
	}

	pmx_updateRate.lock();
	auto sleep_time = pm_updateRate;
	pmx_updateRate.unlock();

	std::this_thread::sleep_for(sleep_time);
}

void tedpad::intern_server::Broadcaster::thread_close()
{
	closesocket(pm_socket);
}
