#pragma once

#include <thread>
#include <mutex>
#include "../../../engine/include/eg/eg_engine.h"

#include "../socket.h"
#include "serverCommon.h"
#include "../modules/gamepadDescription.h"
#include "../packet/packet.h"
#include "../packet/packetModule.h"

//TODO: Make a base class out of these guys.
//TODO: The gamepad description is only retreived when "start" is called. It does not update after that. Make something that detects when there is a gamepad update, and update the broadcast accordingly.

namespace tedpad {
	namespace intern_server {
		class Broadcaster {
		public:
			static constexpr uint16_t Default_port = 3101;
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

			void start();
			void stop();

			void set_port(uint16_t const port);
			uint16_t get_port() const;

			void set_updateRate(std::chrono::milliseconds const &updateRate);
			std::chrono::milliseconds get_updateRate() const;

			Broadcaster(GamepadMutex const &gamepadMutex, uint16_t const port = Default_port, std::chrono::milliseconds const &updateRate = std::chrono::milliseconds(45));
			~Broadcaster();
		private:
			std::thread pm_thread;
			GamepadMutex const pm_gamepadMutex;

			uint16_t pm_port;
			mutable std::mutex pmx_port;

			void thread_main();
			void thread_init();
			void thread_close();
		};
	}
}
