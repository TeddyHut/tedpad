#pragma once

#include <thread>
#include <mutex>
#include "../../../engine/include/eg/eg_engine.h"

#include "../socket.h"
#include "serverCommon.h"
#include "../modules/gamepadDescription.h"
#include "../modules/broadcast.h"
#include "../packet/packet.h"
#include "../packet/packetModule.h"
#include "../util/threadedObject.h"

//TODO: Make a base class out of these guys.
//TODO: The gamepad description is only retreived when "start" is called. It does not update after that. Make something that detects when there is a gamepad update, and update the broadcast accordingly.

namespace tedpad {
	namespace intern_server {
		class Broadcaster : public util::thread::ThreadedObject, public util::thread::SleepObject {
		public:
			static constexpr uint16_t Default_port = 3101;

			void set_serverDescription(Module::ServerDescription const &serverDescription);

			Broadcaster(GamepadMutex const &gamepadMutex, Module::ServerDescription const &serverDescription, uint16_t const port = Default_port, std::chrono::milliseconds const &updateRate = std::chrono::milliseconds(1000));
		private:
			//Used to get the server gamepad information during thread_init
			GamepadMutex const pm_gamepadMutex;

			//The port that the broadcaster is broadcasting from
			uint16_t pm_port;
			//The socket that the broadcaster uses
			SOCKET pm_socket;
			//The destination information that the broadcaster uses. Is filled out during thread_init
			sockaddr_in pm_sockaddrDest;
			//The gamepad description that the broadcaster uses. Is filled out from gamepadMutex during thread_init
			Module::GamepadBriefDescription pm_gamepadDescription;

			//The server description that is broadcasted in the packet. Can be changed by the server
			Module::ServerDescription pm_serverDescription;
			mutable std::mutex pmx_serverDescription;
			
			void thread_init() override;
			void thread_main() override;
			void thread_close() override;
		};
	}
}
