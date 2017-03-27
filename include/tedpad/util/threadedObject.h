#pragma once

#include <thread>
#include <mutex>

namespace tedpad {
	class ThreadedObject {
	public:
		bool state_threadRunning() const;
		
	protected:
		virtual void thread_init() = 0;
		virtual void thread_main() = 0;
		virtual void thread_close() = 0;
	};
}