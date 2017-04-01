#pragma once

#include <algorithm>

#include "../packet/packetModule.h"
#include "moduleBase.h"

namespace tedpad {
	namespace Module {
		struct ServerDescription : public ModuleBase {
			PacketModule to_packetModule() const override;
			void from_packetModule(PacketModule const &p0) override;

			uint32_t ip;
			uint16_t port;
			uint16_t number_clientsConnected;

			ServerDescription();
		};
	}
}
