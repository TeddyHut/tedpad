#pragma once

#include <inttypes.h>
#include <string>

#include "../packet/packetModule.h"
#include "moduleBase.h"

namespace tedpad {
	namespace Module {
		struct ClientDescription : public ModuleBase {
			PacketModule to_packetModule() const;
			void from_packetModule(PacketModule const &p0);

			uint32_t ipAddress;
			
			std::string name;
		};
	}
}
