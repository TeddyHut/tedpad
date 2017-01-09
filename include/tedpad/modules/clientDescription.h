#pragma once

#include <inttypes.h>
#include <string>

namespace tedpad {
	namespace Module {
		struct ClientDescription {
			uint32_t ipAddress;
			std::string name;
		};
	}
}
