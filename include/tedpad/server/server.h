#pragma once

#include <inttypes.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "../../../engine/include/eg/eg_engine.h"

#include "../socket.h"
#include "../gamepad/gamepad.h"
#include "../gamepad/filetype_tpd.h"

#include "serverCommon.h"
#include "clientHandle.h"
#include "broadcaster.h"
#include "designator.h"

//TODO: Maybe I should make a base class out of the server_intern objects. They have a bit of similar code.

namespace tedpad {
	class Server {
	public:
		enum class Config_e {
			//Enables broadcasting of packets describing the server to any possible clients
			Broadcast_Enable = 0,
			//Enables an auto-accept and connecting of clients. Overrides AutoDecline
			AutoAccept_Enable,
			//Will automatically decline clients that attempt to connect
			AutoDecline_Enable,
		};
		enum class Status_e {
			//The server has had a gamepad assigned to it via set_gamepad
			Gamepad_Set = 0,
			//There is a client awaiting approval for connection
			Client_Pending,
		};
		
		//Starts all server operation (starts threads, opens sockets, etc)
		void server_start();
		//Stops all server operation
		void server_stop();
		//Updates the public values for the server
		void server_update();

		//Sets the gamepad that the server will modify on server_update()
		void set_gamepad(Gamepad *const gamepad);
		//Gets the gamepad that the server will modify on server_update()
		Gamepad *get_gamepad() const;

		//Retrieves a description of the client that is pending connection (Client_Pending is set)
		ClientInfo get_pendingClient() const;

		//Config parameters
		eg::Param<Config_e> config;
		//Status parameters
		eg::Param<Status_e> status;
		//Vector of the connected clients
		std::vector<ClientInfo> connectedClient;

	private:
		//---Single Threaded---
		//The gamepad that the server modifies on server_update
		Gamepad *m_gamepad;

		//---Multi Threaded---

		//The gamepad that the server modifies
		Gamepad pm_gamepad;
		std::mutex pmx_gamepad;

		//The config parameters
		eg::Param<Config_e> pm_config;
		std::mutex pmx_config;

		//The status parameters
		eg::Param<Status_e> pm_status;
		std::mutex pmx_status;

		//A vector of the connected clientHandles
		std::vector<intern_server::ClientHandle> pm_connectedClient;
		std::mutex pmx_connectedClient;

		//The thread that the server runs on
		std::thread pm_thread_server;

		//Handles events from designator, broadcaster, and clientHandle.
		void thread_server_main();
	};
}
