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
#include "../user.h"

//This whole thing is super simple. I should probably make it a little more complicated.
//It's also very C-like... probably should do something about that
//TODO: Consider adding GamepadBriefDescription to Module::ServerDescription. That way the GamepadBriefDescription doesn't have to be sourced throught strange approaches.

namespace tedpad {
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
		//TODO: Make this more C++ like and less C like
		struct FilterScanResultsArgs {
			enum e {
				IP = (1 << 0),
				PORT = (1 << 1),
				NUMBEROFCLIENTS = (1 << 2),
				GAMEPADNAME = (1 << 3),
			};
			uint32_t ip = 0;
			uint16_t port = 0;
			uint16_t numberOfClients = 0;
			std::string gamepadName;
			uint8_t filter = 0;
		};
		//Scan for servers for a duration of time
		static std::vector<ServerInfo> scanForTime(ScanForTimeArgs const &args);
		//Maybe this should just take a reference and modify scanResults without returning anything.
		static std::vector<ServerInfo> filterScanResults(std::vector<ServerInfo> scanResults, FilterScanResultsArgs const &args);

		//Useful functions
		static std::string ip_to_str(uint32_t const ip);
		static uint32_t str_to_ip(std::string const &str);

		//Connect to a server and get a gamepad full description from the server
		static Module::GamepadFullDescription get_serverFullGamepadDescription(uint32_t const ip, uint16_t const port, bool const manageSocketService = false);
		static Module::GamepadFullDescription get_serverFullGamepadDescription(std::string const &ip, uint16_t const port, bool const manageSocketService = false);
		
		//Connect to a server
		void connectToServer(uint32_t const ip, uint16_t const port);
		void connectToServer(std::string const &ip, uint16_t const port);

		//Disconnect from a server if currently connected
		void disconnect();
		
		//Update and retreive gamepad values from the server
		void gamepadUpdate();
		
		//Request and update the ServerInfo from the server. Will return the serverInfo
		ServerInfo get_connectedServerInfo();
		
		Gamepad gamepad;

		Client(bool const start_socket_service = true);
		~Client();
	private:
		static SOCKET get_connectedSocket(std::string const &ip, uint16_t const port);

		static Module::GamepadFullDescription serverRequest_Receive_GamepadFullDescription(SOCKET const &v_socket);
		static Module::GamepadData serverRequest_Receive_GamepadData_DirectionOut(SOCKET const &v_socket);
		static void serverRequest_Send_GamepadData_DirecitonIn(SOCKET const &v_socket, Module::GamepadData const &gamepadData);
		static PacketModule server_sendRecv(SOCKET const &v_socket, Module::Communication::Request_e const request, Module::Communication::Reply_e const reply, PacketModule const sendmodule = PacketModule());

		bool const pm_startedSocketService;
		bool pm_connected = false;
		SOCKET pm_socket;
	};
}
