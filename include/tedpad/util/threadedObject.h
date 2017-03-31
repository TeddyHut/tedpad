#pragma once

#include <thread>
#include <mutex>
#include <chrono>
#include <utility>

#include "../../../engine/include/eg/eg_engine.h"

//Information:
//This object is used to help manage a thread
//Calling instruction_start will start the thread
//	The thread will begin execution in "thread_manage".
//	thread_manage will call thread_init if Run_thread_init is true
//	thread_manage will call main each cycle of the thread
//	thread_manage will call thead_close if Run_thread_close is true, and then termminate execution of the thread
//Calling instruction_stop while the thread is not running will do nothing
//Calling instruction_stop while the thread is running from a different thread will set run_thread_close to true, and block until the thread has finished execution. It will then set ThreadRunning to false.
//Calling instruction_stop while the thread is running from the same thread will cause the thread to set ThreadFinished to true, and block until instruction_stop is called from a different thread.
//It will then resume in thread main and when returning to thread manage execute the thread_close function and terminate execution of the thread.
//If the descructor is called before the thread stops, ATM it's undefined behaviour. The pure virtual functions will cause an exception (and normal virtual functions wont work) since it's a destructor.
//Will need to come up with a function object way to do this one it seems

//PLEASE BE AWARE that calling instruction_stop in thread main will return and continue execution in thread main. Only after it has left thread_main will the thread call thread_close and terminate.

//TODO: Fix the crash that occurs when the class is destructed without instruction_stop being called first (attempts to call pure virtual function thread_close)

namespace tedpad {
	namespace util {
		namespace thread {
			class ThreadedObject {
			public:
				virtual void instruction_start();
				virtual void instruction_stop();

				virtual bool state_threadRunning() const;

				//Can't have these because of std::thread.
				ThreadedObject &operator=(ThreadedObject const &) = delete;
				ThreadedObject(ThreadedObject const &) = delete;
				//Can't have these because the std::thread is bound to a particular object: moving the thread will still have it referencing the old object. Would need to stop the thread and start it again.
				//TODO: Make these work (it is possible with some interesting code)
				ThreadedObject &operator=(ThreadedObject &&p0) = delete;
				ThreadedObject(ThreadedObject &&p0) = delete;
				ThreadedObject();
				virtual ~ThreadedObject();
			protected:
				enum class Instruction_e {
					Run_thread_init = 0,
					Run_thread_close,
				};
				enum class State_e {
					ThreadRunning,
					ThreadFinished,
				};

				std::thread pm_thread;
				//IDK whether we even need this mutex. Doesn't hurt though.
				mutable std::mutex pmx_thread;

				eg::Param<Instruction_e> pm_instruction;
				mutable std::mutex pmx_instruction;

				eg::Param<State_e> pm_state;
				mutable std::mutex pmx_state;

				//Could probably put this into pm_state
				//Condition variable used to block the thread until instruction_stop is called from another thread
				bool pm_conditionVar_active = false;
				std::condition_variable pm_conditionVar_signal;
				mutable std::mutex pm_conditionVar_lock;

				//Managed when functions are called, is the main thread function
				virtual void thread_manage();
				//Called right before join in instruction_close. can be used to wake up the thread if it is waiting on a condition variable or some other event
				virtual void thread_close_preJoin();
				//Called by thread_manage when the thread starts
				virtual void thread_init() = 0;
				//The main thread function. Will be repeated.
				virtual void thread_main() = 0;
				//Called by thread_manage when the thread closes
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
