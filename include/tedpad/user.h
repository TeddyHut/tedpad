#pragma once

#include "modules/gamepadDescription.h"
#include "modules/broadcast.h"

namespace tedpad {
	struct ServerInfo {
		//Could put this in a constructor/overload of operator =
		//Does not full out the gameapdDescription.
		void fillFromServerDescription(Module::ServerDescription const &serverDescription);

		Module::GamepadBriefDescription gamepadDescription;
		uint32_t ip = 0;
		uint16_t port = 0;
		//Just to make it more confusing between this and Module::ServerDescription...
		uint16_t numberOfClients = 0;
	};
}
