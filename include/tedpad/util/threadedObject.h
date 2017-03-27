#pragma once

#include <thread>
#include <mutex>
#include <chrono>

#include "../../../engine/include/eg/eg_engine.h"

namespace tedpad {
	namespace util {
		namespace thread {
			class ThreadedObject {
			public:
				virtual void instruction_start();
				virtual void instruction_stop();

				virtual bool state_threadRunning() const;

				ThreadedObject();
				virtual ~ThreadedObject();
			protected:
				enum class Instruction_e {
					Run_thread_init = 0,
					Run_thread_close,
				};
				enum class State_e {
					ThreadRunning,
				};

				std::thread pm_thread;

				eg::Param<Instruction_e> pm_instruction;
				mutable std::mutex pmx_instruction;

				eg::Param<State_e> pm_state;
				mutable std::mutex pmx_state;

				virtual void thread_manage();
				virtual void thread_init() = 0;
				virtual void thread_main() = 0;
				virtual void thread_close() = 0;
			};

			class SleepObject {
			public:
				virtual void set_updateRate(std::chrono::milliseconds const &updateRate);
				virtual std::chrono::milliseconds get_updateRate() const;

				SleepObject(std::chrono::milliseconds const &updateRate = std::chrono::milliseconds(50));
			protected:
				std::chrono::milliseconds pm_updateRate;
				mutable std::mutex pmx_updateRate;
			};
		}
	}
}
