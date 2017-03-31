#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <sstream>

#include "../socket.h"
#include "../gamepad/gamepad.h"
#include "../util/threadedObject.h"
#include "../modules/gamepadDescription.h"
#include "../modules/gamepadData.h"
#include "../modules/communication.h"
#include "../modules/broadcast.h"
#include "../server/broadcaster.h"
#include "../packet/packet.h"

//This whole thing is super simple. I should probably make it a little more complicated.
//It's also very C-like... probably should do something about that

namespace tedpad {
	struct ServerInfo {
		Module::GamepadBriefDescription gamepadDescription;
		uint32_t ip;
		uint16_t port;
	};
	class Client {
	public:
		struct ScanForTimeArgs {
			//The amount of time the function will scan for
			std::chrono::milliseconds scanTime = std::chrono::milliseconds(2000);
			//The rate at which the scan will update
			std::chrono::milliseconds updateRate = std::chrono::milliseconds(100);
			//The port to scan on
			uint16_t port = intern_server::Broadcaster::Default_port;
			//Whether to start and stop the socket service
			bool manageSocketService = false;
		};
		//Scan for servers for a duration of time
		static std::vector<ServerInfo> scanForTime(ScanForTimeArgs const &args);

		//Connect to a server and get a gamepad full description from the server
		static Module::GamepadFullDescription get_serverFullGamepadDescription(uint32_t const ip, uint16_t const port, bool const manageSocketService = false);
		static Module::GamepadFullDescription get_serverFullGamepadDescription(std::string const &ip, uint16_t const port, bool const manageSocketService = false);
		
		//Connect to a server
		void connectToServer(uint32_t const ip, uint16_t const port);
		void connectToServer(std::string const &ip, uint16_t const port);
		
		//Update and retreive gamepad values from the server
		void gamepadUpdate();
		
		//Get the server that the client is connected to
		ServerInfo get_connectedServerInfo() const;
		
		Gamepad gamepad;

		Client(bool const start_socket_service = true);
		~Client();
	private:
		static SOCKET get_connectedSocket(std::string const &ip, uint16_t const port);

		static Module::GamepadFullDescription serverRequest_Receive_GamepadFullDescription(SOCKET const &v_socket);
		static Module::GamepadData serverRequest_Receive_GamepadData_DirectionOut(SOCKET const &v_socket);
		static void serverRequest_Send_GamepadData_DirecitonIn(SOCKET const &v_socket, Module::GamepadData const &gamepadData);
		static PacketModule server_sendRecv(SOCKET const &v_socket, Module::Communication::Request_e const request, Module::Communication::Reply_e const reply, PacketModule const sendmodule = PacketModule());
		
		static std::string ip_to_str(uint32_t const ip);
		static uint32_t str_to_ip(std::string const &str);

		bool const pm_startedSocketService;
		bool pm_connected = false;
		SOCKET pm_socket;
		ServerInfo pm_serverInfo;
	};
}
