#pragma once

#include <inttypes.h>
#include "../socket.h"

#include <mutex>
#include <condition_variable>

namespace tedpad {
	struct ClientInfo {
		uint32_t ip;
		uint16_t port;
	};
	namespace intern_server {
		struct ImplementationClientInfo : public ClientInfo {
			SOCKET socket = -1;
		};
		struct UpdateSignal {
			bool *request = nullptr;
			std::unique_lock<std::mutex> *lock = nullptr;
			std::condition_variable *signal = nullptr;
			bool filled() const;
		};
	}
}
