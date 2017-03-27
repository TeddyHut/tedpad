#include "../../../include/tedpad/modules/communication.h"

std::map<tedpad::Module::Communication::Request_e, tedpad::Module::Communication::Reply_e> requestReply_map = {
	{ tedpad::Module::Communication::Request_e::Receive_GamepadFullDescription, tedpad::Module::Communication::Reply_e::Send_GamepadFullDescription},
	{ tedpad::Module::Communication::Request_e::Receive_GamepadData_DirectionOut, tedpad::Module::Communication::Reply_e::Send_GamepadData_DirectionOut },
	{ tedpad::Module::Communication::Request_e::Send_GamepadData_DirectionIn, tedpad::Module::Communication::Reply_e::Receive_GamepadData_DirectionIn },
	{ tedpad::Module::Communication::Request_e::Ping, tedpad::Module::Communication::Reply_e::Acknowledge }
};

tedpad::PacketModule tedpad::Module::Communication::Request::to_packetModule() const
{
	PacketModule mod;
	mod.name = get_description();
	mod.data.push_back(static_cast<uint8_t>(request));
	mod.valid = true;
	return(mod);
}

void tedpad::Module::Communication::Request::from_packetModule(PacketModule const & p0)
{
	set_description(p0.name);
	if (!(description[Key::Module] & Value::Module::Request)) {
		description[Key::Module] = Value::Module::Invalid;
		return;
	}

	request = static_cast<Request_e>(p0.data.at(0));
}

tedpad::Module::Communication::Request::Request(Request_e const request) : request(request)
{
	description[Key::Module] = Value::Module::Request;
}

tedpad::PacketModule tedpad::Module::Communication::Reply::to_packetModule() const
{
	PacketModule mod;
	mod.name = get_description();
	mod.data.push_back(static_cast<uint8_t>(reply));
	mod.valid = true;
	return(mod);
}

void tedpad::Module::Communication::Reply::from_packetModule(PacketModule const & p0)
{
	set_description(p0.name);
	if (!(description[Key::Module] & Value::Module::Reply)) {
		description[Key::Module] = Value::Module::Invalid;
		return;
	}

	reply = static_cast<Reply_e>(p0.data.at(0));
}

tedpad::Module::Communication::Reply::Reply(Reply_e const reply) : reply(reply)
{
	description[Key::Module] = Value::Module::Reply;
}
