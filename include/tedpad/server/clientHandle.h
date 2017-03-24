#pragma once

#include <thread>
#include <mutex>
#include "../../../engine/include/eg/eg_engine.h"

#include "../socket.h"

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
			};
			
			eg::Param<Instruction_e> m_instruction;
			mutable std::mutex mx_instruction;
			
			eg::Param<Config_e> m_config;
			mutable std::mutex mx_config;

			eg::Param<State_e> m_state;
			mutable std::mutex mx_state;

			ClientHandle(SOCKET const pm_socket);
			~ClientHandle();
		private:
			std::thread pm_thread;
			SOCKET pm_socket;

			void thread_main();
			void thread_init();
		};
	}
}