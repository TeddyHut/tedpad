#pragma once

#include <thread>
#include <mutex>
#include <map>
#include <vector>
#include <functional>
#include <chrono>
#include "../../../engine/include/eg/eg_engine.h"

#include "../socket.h"
#include "serverCommon.h"
#include "../modules/communication.h"
#include "../modules/gamepadData.h"
#include "../modules/gamepadDescription.h"
#include "../modules/broadcast.h"
#include "../packet/packet.h"
#include "../packet/packetModule.h"
#include "../util/threadedObject.h"

//TODO: Make it so that client can receive both Server::Out and Server::In information.

namespace tedpad {
	namespace intern_server {
		class ClientHandle : public util::thread::ThreadedObject, public util::thread::SleepObject {
		public:
			bool state_clientDisconnected() const;

			ImplementationClientInfo get_clientInfo() const;

			void set_serverDescription(Module::ServerDescription const &serverDescription);

			//Can't have any of these: see "threadedObject"
			ClientHandle &operator=(ClientHandle const &) = delete;
			ClientHandle(ClientHandle const &) = delete;
			ClientHandle &operator=(ClientHandle &&) = delete;
			ClientHandle(ClientHandle &&) = delete;

			ClientHandle(ImplementationClientInfo const &socket, UpdateSignal const &updateSignal, GamepadMutex const &gamepadMutex, Module::ServerDescription const &serverDescription, std::chrono::milliseconds const &updateRate = std::chrono::milliseconds(10));
			//Maybe make a destructor that calls instruction_stop. Get a crash for some reason though (there's a locking problem)
		private:
			enum class State_e {
				ClientDisconnected = 0,
			};

			eg::Param<State_e> pm_state;
			mutable std::mutex pmx_state;

			//Even though update signal isn't actuall used. Oh well.
			UpdateSignal const pm_updateSignal;
			GamepadMutex const pm_gamepadMutex;

			ImplementationClientInfo const pm_clientInfo;
			mutable std::mutex pmx_clientInfo;

			Module::ServerDescription pm_serverDescription;
			mutable std::mutex pmx_serverDescription;

			void thread_init() override;
			void thread_main() override;
			void thread_close() override;

			ToNetworkPacket requestCallback_Receive_GamepadFullDescription(FromNetworkPacket const &fromPacket);
			ToNetworkPacket requestCallback_Receive_ServerDescription(FromNetworkPacket const &fromPacket);
			ToNetworkPacket requestCallback_Receive_GamepadData_DirectionOut(FromNetworkPacket const &fromPacket);
			ToNetworkPacket requestCallback_Send_GamepadData_DirectionIn(FromNetworkPacket const &fromPacket);
			ToNetworkPacket requestCallback_Ping(FromNetworkPacket const &fromPacket);

			static std::map<Module::Communication::Request_e, ToNetworkPacket (ClientHandle::*)(FromNetworkPacket const &)> requestCallback_map;
		};
	}
}
