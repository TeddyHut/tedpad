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

//TODO: Make it so that client can receive both Server::Out and Server::In information.

namespace tedpad {
	namespace intern_server {
		class ClientHandle {
		public:
			enum class Instruction_e {
				RunInit = 0,
				StopThread,
			};
			enum class Config_e {

			};
			enum class State_e {
				Thread_Running = 0,
				GamepadUpdate,
				ClientDisconnected,
			};
			
			eg::Param<Instruction_e> m_instruction;
			mutable std::mutex mx_instruction;
			
			eg::Param<Config_e> m_config;
			mutable std::mutex mx_config;

			eg::Param<State_e> m_state;
			mutable std::mutex mx_state;

			ImplementationClientInfo get_clientInfo() const;

			void set_updateRate(std::chrono::milliseconds const &updateRate);
			std::chrono::milliseconds get_updateRate() const;

			ClientHandle(ImplementationClientInfo const &socket, UpdateSignal const &updateSignal, GamepadMutex const &gamepadMutex, std::chrono::milliseconds const &updateRate = std::chrono::milliseconds(10));
			~ClientHandle();
		private:
			std::thread pm_thread;
			UpdateSignal const pm_updateSignal;
			GamepadMutex const pm_gamepadMutex;

			ImplementationClientInfo const pm_clientInfo;
			mutable std::mutex pmx_clientInfo;

			std::chrono::milliseconds pm_updateRate;
			mutable std::mutex pmx_updateRate;

			void thread_main();
			void thread_init();
			void thread_close();

			ToNetworkPacket requestCallback_Receive_GamepadFullDescription(FromNetworkPacket const &fromPacket);
			ToNetworkPacket requestCallback_Receive_GamepadData_DirectionOut(FromNetworkPacket const &fromPacket);
			ToNetworkPacket requestCallback_Send_GamepadData_DirectionIn(FromNetworkPacket const &fromPacket);
			ToNetworkPacket requestCallback_Ping(FromNetworkPacket const &fromPacket);

			static std::map<Module::Communication::Request_e, ToNetworkPacket (ClientHandle::*)(FromNetworkPacket const &)> requestCallback_map;
		};
	}
}
