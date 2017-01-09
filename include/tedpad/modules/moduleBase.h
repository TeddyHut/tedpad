#pragma once

#include "../../../engine/include/eg/eg_engine.h"
#include "../packet/packetModule.h"
#include "../moduleName.h"

namespace tedpad {
	namespace Module {
		class ModuleBase {
		public:
			virtual PacketModule to_packetModule() const = 0;
			virtual void from_packetModule(PacketModule const &p0) = 0;
			virtual operator PacketModule() const;
			virtual ModuleBase &operator=(PacketModule const &p0);

			std::string get_description() const;
			void set_description(std::string const &moduleName);
			eg::Descriptor<> description;

			ModuleBase();
		};
	}
}
