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
#include "../util/threadedObject.h"

//There's some very common code running between these intern_server objects... Mabye a base class would be smart?

namespace tedpad {
	namespace intern_server {
		//Designator will listen for and accept clients
		class Designator : public util::thread::ThreadedObject, public util::thread::SleepObject {
		public:
			static constexpr uint16_t Default_port = 3100;

			//Returns whether there is a pending client
			bool state_clientPending() const;
			//If forget = true, the pending client will be removed from the cue
			ImplementationClientInfo get_pendingClientInfo(bool const forget = true);
			ImplementationClientInfo get_pendingClientInfo() const;

			void set_port(uint16_t const port);
			uint16_t get_port() const;

			Designator(UpdateSignal const &updateSignal = UpdateSignal(), uint16_t const port = Default_port, std::chrono::milliseconds const &updateRate = std::chrono::milliseconds(50));
		private:
			enum class State_e {
				ClientPending = 0,
			};

			eg::Param<State_e> pm_state;
			mutable std::mutex pmx_state;

			UpdateSignal pm_updateSignal;
			SOCKET pm_socket;

			uint16_t pm_port;
			mutable std::mutex pmx_port;
	
			std::vector<ImplementationClientInfo> pm_pendingClientInfo;
			mutable std::mutex pmx_pendingClientInfo;

			void thread_init() override;
			void thread_main() override;
			void thread_close() override;

			//Returns true if pm_pendingClientInfo is not empty. Will not lock pm_pendingClientInfo
			bool client_pending() const;
		};
	}
}