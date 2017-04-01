#include "../include/tedpad.h"
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

//Usage: tedpad_vs <gamepad description name> <port>

int main(int argc, char *argv[]) {
	if (argc != 3) {
		std::cout << "incorrect number of arguments" << std::endl;
		exit(1);
	}

	std::ifstream file(argv[1], std::ios_base::in | std::ios_base::ate | std::ios_base::binary);
	if (!file.is_open()) {
		std::cout << "could not open file " << argv[1] << std::endl;
		exit(1);
	}
	std::vector<uint8_t> file_contents(file.tellg());
	file.seekg(std::ios_base::beg);
	file.read(reinterpret_cast<char *>(file_contents.data()), file_contents.size());
	file.close();

	std::stringstream ss_port(argv[2]);
	uint16_t port = 0;
	ss_port >> port;

	tedpad::Gamepad gamepad;
	gamepad.from_tpdFile(file_contents);
	tedpad::Server server(&gamepad);
	server.set_port(port);
	server.server_start();

	size_t connectedClient_num = 0;
	size_t connectedClient_numOld = 0;
	while (true) {
		gamepad.Set_attribute("DigitalOut", true);
		server.server_gamepadSync();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		gamepad.Set_attribute("DigitalOut", false);
		server.server_gamepadSync();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		connectedClient_num = server.get_connectedClients().size();
		if (connectedClient_num != connectedClient_numOld) {
			std::cout << "No. Clients Connected " << connectedClient_num << std::endl;
			connectedClient_numOld = connectedClient_num;
		}
	}
}
