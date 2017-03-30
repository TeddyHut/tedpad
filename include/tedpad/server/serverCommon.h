#pragma once

#include <inttypes.h>
#include <mutex>
#include <condition_variable>

#include "../socket.h"
#include "../gamepad/gamepad.h"

namespace tedpad {
	//Given to user with "Server::get_pendingClient()"
	struct ClientInfo {
		uint32_t ip;
		uint16_t port;
	};
	namespace intern_server {
		//Used as an exchange between the designator and the clientHandle
		struct ImplementationClientInfo : public ClientInfo {
			socket_service::SOCKET socket = -1;
		};
		//Used to update the server running thread 
		struct UpdateSignal {
			enum class Event : uint8_t {
				Designator_NewClient,
				ClientHandle_ClientDisconnected,
				Server_ConfigUpdate_Broadcast,
				Server_ValueUpdate_Port,
			};
			std::vector<Event> *eventQueue;
			bool *request = nullptr;
			//Maybe use std::shared_lock (or whatever it is) instead of a pointer to a unique_lock
			std::mutex *lock = nullptr;
			std::condition_variable *signal = nullptr;
			bool filled() const;
		};
		struct GamepadMutex {
			Gamepad *gamepad = nullptr;
			std::mutex *mx_gamepad = nullptr;
			bool filled() const;
		};
	}
}
