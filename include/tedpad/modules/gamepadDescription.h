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
		//Structure: Module( gamepadName (?) -> out_*count (1 * 4) -> in_*count (1 * 4) )
		struct GamepadBriefDescription : public ModuleBase {
			PacketModule to_packetModule() const override;
			void from_packetModule(PacketModule const &p0) override;

			std::string gamepadName;
			uint8_t out_digitalCount = 0;
			uint8_t out_analogueCount = 0;
			uint8_t out_axisCount = 0;
			uint8_t out_bufferCount = 0;
			uint8_t in_digitalCount = 0;
			uint8_t in_analogueCount = 0;
			uint8_t in_axisCount = 0;
			uint8_t in_bufferCount = 0;

			GamepadBriefDescription();
		};

		//Structure: Module( Packet( GamepadBriefDescription -> attribute_modules ) )
		struct GamepadFullDescription : public ModuleBase  {
			PacketModule to_packetModule() const override;
			void from_packetModule(PacketModule const &p0) override;
			void update_briefDesciptionCount();

			GamepadBriefDescription briefDescription;
			std::vector<Attribute::PlaceHolder> attribute;

			GamepadFullDescription();
		private:
			uint8_t get_attributeServerOutCount(Attribute::PlaceHolderType const attr_desc) const;
			uint8_t get_attributeServerInCount(Attribute::PlaceHolderType const attr_desc) const;
		};
	}
}
