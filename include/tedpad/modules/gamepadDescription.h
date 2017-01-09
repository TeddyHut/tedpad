#pragma once

#include <inttypes.h>
#include <string>
#include <vector>

#include "../socket.h"
#include "../packet/packet.h"
#include "moduleBase.h"
#include "attribute.h"

namespace tedpad {
	namespace Module {
		struct GamepadBriefDescription : public ModuleBase {
			PacketModule to_packetModule() const override;
			void from_packetModule(PacketModule const &p0) override;

			std::string gamepadName;
			size_t out_digitalCount;
			size_t out_analogueCount;
			size_t out_axisCount;
			size_t out_bufferCount;
			size_t in_digitalCount;
			size_t in_analogueCount;
			size_t in_axisCount;
			size_t in_bufferCount;

			GamepadBriefDescription();
		};

		struct GamepadFullDescription : public ModuleBase  {
			PacketModule to_packetModule() const override;
			void from_packetModule(PacketModule const &p0) override;

			GamepadBriefDescription briefDescription;
			std::vector<Attribute::PlaceHolder> attribute;

			GamepadFullDescription();
		};
	}
}
