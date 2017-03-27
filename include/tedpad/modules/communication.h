#pragma once

#include <map>

#include "moduleBase.h"
#include "../packet/packetModule.h"

namespace tedpad {
	namespace Module {
		namespace Communication {
			enum class Request_e : uint8_t {
				Receive_GamepadFullDescription = 0,
				Receive_GamepadData_DirectionOut,
				Send_GamepadData_DirectionIn,
				Ping,
			};
			enum class Reply_e : uint8_t {
				Send_GamepadFullDescription = 0,
				Send_GamepadData_DirectionOut,
				Receive_GamepadData_DirectionIn,
				Acknowledge,
			};
			extern std::map<Request_e, Reply_e> requestReply_map;
			//Structure: Module( request (1) )
			struct Request : public ModuleBase {
				PacketModule to_packetModule() const override;
				void from_packetModule(PacketModule const &p0) override;

				Request_e request;

				Request(Request_e const request = Request_e::Ping);
			};
			//Structure: Module ( reply (1) )
			struct Reply : public ModuleBase {
				PacketModule to_packetModule() const override;
				void from_packetModule(PacketModule const &p0) override;

				Reply_e reply;

				Reply(Reply_e const reply = Reply_e::Acknowledge);
			};
		}
	}
}
