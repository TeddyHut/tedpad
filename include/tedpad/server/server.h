#pragma once

#include <inttypes.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <map>
#include <algorithm>
#include "../../../engine/include/eg/eg_engine.h"

#include "../socket.h"
#include "../gamepad/gamepad.h"
#include "../gamepad/filetype_tpd.h"
#include "../util/threadedObject.h"
#include "../modules/broadcast.h"

#include "serverCommon.h"
#include "clientHandle.h"
#include "broadcaster.h"
#include "designator.h"

//TODO: Maybe I should make a base class out of the server_intern objects. They have a bit of similar code.

namespace tedpad {
	class Server : public util::thread::ThreadedObject {
	public:
		//True = Enable broadcasting
		void config_broadcast(bool const value);

		//True = Automatically accept clients
		//void config_autoAccept(bool const value);
		//True = Automatically decline clients
		//void config_autoDecline(bool const value);

		//True if a gamepad has been assigned to the server
		bool status_gamepadSet() const;
		//True if a client is pending acception
		//bool status_clientPending() const;

		//Set the server broadcast rate
		void set_broadcastRate(std::chrono::milliseconds const &value);
		
		//Sets the gamepad that the server will modify on server_gamepadSync(). Will also set the gamepad direction to IODirection::Server
		void set_gamepad(Gamepad *const gamepad);
		//Gets the gamepad that the server will modify on server_gamepadSync()
		Gamepad *get_gamepad() const;

		//Set the port that the server will run on
		void set_port(uint16_t const port);
		//Get the port that the server is running on
		uint16_t get_port() const;

		//Retrieves a description of the client that is pending connection
		//ClientInfo get_pendingClient() const;

		//Gets information about all the connected clients
		std::vector<ClientInfo> get_connectedClients() const;
		
		//Starts all server operation (starts threads, opens sockets, etc)
		void server_start();
		//Stops all server operation
		void server_stop();
		//Syncronise the internal gamepad with the external gamepad
		void server_gamepadSync();

		Server(Gamepad *const gamepad = nullptr, bool const start = false);
		~Server();
	private:
		enum class Config_e {
			Broadcast = 0,
			//---THESE ARE NOT IMPLEMENTED YET
			AutoAccept,
			AutoDecline,
		};
		enum class State_e {
			GamepadSet = 0,
			ClientPending,
		};

		//---Single Threaded---
		//The gamepad that the server modifies on server_update
		Gamepad *pm_externalGamepad;

		//---Multi Threaded---

		//The condition variable informatoin used to alert the server to an event
		std::vector<intern_server::UpdateSignal::Event> pm_eventQueue;
		bool pm_request;
		std::condition_variable pm_signal;
		mutable std::mutex pm_lock;

		//The gamepad that the server modifies
		Gamepad pm_gamepad;
		mutable std::mutex pmx_gamepad;

		//The port that the server (or designator) will be running on
		uint16_t pm_port = intern_server::Designator::Default_port;
		mutable std::mutex pmx_port;

		//The config parameters
		eg::Param<Config_e> pm_config;
		mutable std::mutex pmx_config;

		//The status parameters
		eg::Param<State_e> pm_state;
		mutable std::mutex pmx_state;

		//Maybe use std::unique_ptr for these guys instead
		//The designator
		intern_server::Designator *pm_designator = nullptr;

		//The broadcaster
		intern_server::Broadcaster *pm_broadcaster = nullptr;

		//A vector of the connected clientHandles
		std::vector<intern_server::ClientHandle *> pm_connectedClient;
		mutable std::mutex pmx_connectedClient;

		//Need this to stop a deadlock when the thread stops: the thread will never leave thread_main without the updateSignal
		void instruction_stop() override;

		//Handles events from designator, broadcaster, and clientHandle.
		void thread_init() override;
		void thread_main() override;
		void thread_close() override;

		void eventCallback_Designator_NewClient();
		void eventCallback_ClientHandle_ClientDisconnected();
		void eventCallback_Server_ConfigUpdate_Broadcast();
		void eventCallback_Server_ValueUpdate_Port();

		static std::map<intern_server::UpdateSignal::Event, void (Server:: *)()> map_eventCallback;
	};
}
