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

			ClientHandle &operator=(ClientHandle const &) = delete;
			ClientHandle(ClientHandle const &) = delete;
			ClientHandle &operator=(ClientHandle &&p0);
			ClientHandle(ClientHandle &&);
			ClientHandle(ImplementationClientInfo const &socket, UpdateSignal const &updateSignal, GamepadMutex const &gamepadMutex, std::chrono::milliseconds const &updateRate = std::chrono::milliseconds(10));
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

			void thread_init() override;
			void thread_main() override;
			void thread_close() override;

			ToNetworkPacket requestCallback_Receive_GamepadFullDescription(FromNetworkPacket const &fromPacket);
			ToNetworkPacket requestCallback_Receive_GamepadData_DirectionOut(FromNetworkPacket const &fromPacket);
			ToNetworkPacket requestCallback_Send_GamepadData_DirectionIn(FromNetworkPacket const &fromPacket);
			ToNetworkPacket requestCallback_Ping(FromNetworkPacket const &fromPacket);

			static std::map<Module::Communication::Request_e, ToNetworkPacket (ClientHandle::*)(FromNetworkPacket const &)> requestCallback_map;
		};
	}
}
