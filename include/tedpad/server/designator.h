#pragma once

#include <thread>
#include <mutex>
#include <sstream>
#include <chrono>
#include <vector>
#include <condition_variable>
#include "../../../engine/include/eg/eg_engine.h"

#include "../socket.h"
#include "serverCommon.h"

//There's some very common code running between these intern_server objects... Mabye a base class would be smart?

namespace tedpad {
	namespace intern_server {
		//Designator will listen for and accept clients
		class Designator {
		public:
			static constexpr uint16_t Default_port = 3100;
			//These are instructions that will tell the designator to do something
			enum class Instruction_e {
				RunInit = 0,
				//Stops the running thread
				StopThread,
			};
			//These are configurations that the designator will reference
			enum class Config_e {
			};
			//These are states that the designator is in
			enum class State_e {
				//The thread is running
				Thread_Running = 0,
				//A client is pending
				Client_Pending,
			};

			eg::Param<Instruction_e> m_instruction;
			mutable std::mutex mx_instruction;

			eg::Param<Config_e> m_config;
			mutable std::mutex mx_config;

			eg::Param<State_e> m_state;
			mutable std::mutex mx_state;

			void start();
			void stop();

			//Returns whether there is a pending client
			bool get_isPendingClient() const;
			//If forget = true, the pending client will be removed from the cue
			ImplementationClientInfo get_pendingClientInfo(bool const forget = true);
			ImplementationClientInfo get_pendingClientInfo() const;

			void set_port(uint16_t const port);
			uint16_t get_port() const;

			void set_updateRate(std::chrono::milliseconds const &updateRate);
			std::chrono::milliseconds get_updateRate() const;

			Designator(uint16_t const port = Default_port, UpdateSignal updateSignal = UpdateSignal(), std::chrono::milliseconds const &updateRate = std::chrono::milliseconds(50));
		private:
			uint16_t pm_port;
			mutable std::mutex pmx_port;

			std::chrono::milliseconds pm_updateRate;
			mutable std::mutex pmx_updateRate;
	
			std::vector<ImplementationClientInfo> pm_pendingClientInfo;
			mutable std::mutex pmx_pendingClientInfo;

			UpdateSignal pm_updateSignal;
			SOCKET pm_socket;
			std::thread pm_thread_main;

			void thread_main();
			void thread_init();
			void thread_close();

			//Returns true if pm_pendingClientInfo is not empty. Will not lock pm_pendingClientInfo
			bool client_pending() const;
		};
	}
}