#include "../../../include/tedpad/modules/gamepadDescription.h"

tedpad::PacketModule tedpad::Module::GamepadBriefDescription::to_packetModule() const
{
	PacketModule mod;
	mod.name = get_description();
	std::copy(gamepadName.begin(), gamepadName.end(), std::back_inserter(mod.data));
	mod.data.push_back('\0');
	uint32_t count = htonl(out_digitalCount);
	std::copy_n(reinterpret_cast<uint8_t*>(&count), sizeof(uint32_t), std::back_inserter(mod.data));
	count = htonl(out_analogueCount);
	std::copy_n(reinterpret_cast<uint8_t*>(&count), sizeof(uint32_t), std::back_inserter(mod.data));
	count = htonl(out_axisCount);
	std::copy_n(reinterpret_cast<uint8_t*>(&count), sizeof(uint32_t), std::back_inserter(mod.data));
	count = htonl(out_bufferCount);
	std::copy_n(reinterpret_cast<uint8_t*>(&count), sizeof(uint32_t), std::back_inserter(mod.data));
	count = htonl(in_digitalCount);
	std::copy_n(reinterpret_cast<uint8_t*>(&count), sizeof(uint32_t), std::back_inserter(mod.data));
	count = htonl(in_analogueCount);
	std::copy_n(reinterpret_cast<uint8_t*>(&count), sizeof(uint32_t), std::back_inserter(mod.data));
	count = htonl(in_axisCount);
	std::copy_n(reinterpret_cast<uint8_t*>(&count), sizeof(uint32_t), std::back_inserter(mod.data));
	count = htonl(in_bufferCount);
	std::copy_n(reinterpret_cast<uint8_t*>(&count), sizeof(uint32_t), std::back_inserter(mod.data));
	mod.valid = true;
	return(mod);
}

void tedpad::Module::GamepadBriefDescription::from_packetModule(PacketModule const & p0)
{
	set_description(p0.name);
	if (!(description[Key::Module] & Value::Module::GamepadBriefDescription)) {
		description[Key::Module] = Value::Module::Invalid;
		return;
	}
	auto itr = std::find(p0.data.begin(), p0.data.end(), '\0');
	if (itr == p0.data.end()) {
		description[Key::Module] = Value::Module::Invalid;
		return;
	}
	gamepadName = std::string(p0.data.begin(), itr);
	uint32_t count;
	std::copy_n(++itr, sizeof(uint32_t), reinterpret_cast<uint8_t*>(&count));
	out_digitalCount = ntohl(count);
	std::copy_n((itr += sizeof(uint32_t)), sizeof(uint32_t), reinterpret_cast<uint8_t*>(&count));
	out_analogueCount = ntohl(count);
	std::copy_n((itr += sizeof(uint32_t)), sizeof(uint32_t), reinterpret_cast<uint8_t*>(&count));
	out_axisCount = ntohl(count);
	std::copy_n((itr += sizeof(uint32_t)), sizeof(uint32_t), reinterpret_cast<uint8_t*>(&count));
	out_bufferCount = ntohl(count);
	std::copy_n((itr += sizeof(uint32_t)), sizeof(uint32_t), reinterpret_cast<uint8_t*>(&count));
	in_digitalCount = ntohl(count);
	std::copy_n((itr += sizeof(uint32_t)), sizeof(uint32_t), reinterpret_cast<uint8_t*>(&count));
	in_analogueCount = ntohl(count);
	std::copy_n((itr += sizeof(uint32_t)), sizeof(uint32_t), reinterpret_cast<uint8_t*>(&count));
	in_axisCount = ntohl(count);
	std::copy_n((itr += sizeof(uint32_t)), sizeof(uint32_t), reinterpret_cast<uint8_t*>(&count));
	in_bufferCount = ntohl(count);
}

tedpad::Module::GamepadBriefDescription::GamepadBriefDescription()
{
	description[Key::Module] = Value::Module::GamepadBriefDescription;
}

tedpad::PacketModule tedpad::Module::GamepadFullDescription::to_packetModule() const
{
	PacketModule mod;
	mod.name = get_description();
	ToNetworkPacket packet;
	packet.add_module(briefDescription.to_packetModule());
	for (auto &&element : attribute) {
		packet.add_module(element.to_packetModule());
	}
	mod.data = packet.get_packetData();
	mod.valid = true;
	return(mod);
}

void tedpad::Module::GamepadFullDescription::from_packetModule(PacketModule const & p0)
{
	attribute.clear();
	set_description(p0.name);
	if (!(description[Key::Module] & Value::Module::GamepadFullDescription)) {
		description[Key::Module] = Value::Module::Invalid;
		return;
	}
	FromNetworkPacket packet;
	packet.set_data(p0.data);
	auto mods = packet.get_packetModules();
	briefDescription.from_packetModule(mods[0]);
	if (briefDescription.description[Key::Module] & Value::Module::Invalid) {
		description[Key::Module] = Value::Module::Invalid;
		return;
	}
	mods.erase(mods.begin());
	attribute.resize(mods.size());
	auto itr_mods = mods.begin();
	for (auto &&element : attribute) {
		element.from_packetModule(*itr_mods);
		if (element.description[Key::Module] & Value::Module::Invalid) {
			description[Key::Module] = Value::Module::Invalid;
			attribute.clear();
			return;
		}
		itr_mods++;
	}
}

tedpad::Module::GamepadFullDescription::GamepadFullDescription()
{
	description[Key::Module] = Value::Module::GamepadFullDescription;
}
