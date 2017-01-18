#include "../../../include/tedpad/modules/gamepadDescription.h"

tedpad::PacketModule tedpad::Module::GamepadBriefDescription::to_packetModule() const
{
	PacketModule mod;
	mod.name = get_description();
	std::copy(gamepadName.begin(), gamepadName.end(), std::back_inserter(mod.data));
	mod.data.push_back('\0');
	uint32_t count = htonl(static_cast<uint32_t>(out_digitalCount));
	std::copy_n(reinterpret_cast<uint8_t*>(&count), sizeof(uint32_t), std::back_inserter(mod.data));
	count = htonl(static_cast<uint32_t>(out_analogueCount));
	std::copy_n(reinterpret_cast<uint8_t*>(&count), sizeof(uint32_t), std::back_inserter(mod.data));
	count = htonl(static_cast<uint32_t>(out_axisCount));
	std::copy_n(reinterpret_cast<uint8_t*>(&count), sizeof(uint32_t), std::back_inserter(mod.data));
	count = htonl(static_cast<uint32_t>(out_bufferCount));
	std::copy_n(reinterpret_cast<uint8_t*>(&count), sizeof(uint32_t), std::back_inserter(mod.data));
	count = htonl(static_cast<uint32_t>(in_digitalCount));
	std::copy_n(reinterpret_cast<uint8_t*>(&count), sizeof(uint32_t), std::back_inserter(mod.data));
	count = htonl(static_cast<uint32_t>(in_analogueCount));
	std::copy_n(reinterpret_cast<uint8_t*>(&count), sizeof(uint32_t), std::back_inserter(mod.data));
	count = htonl(static_cast<uint32_t>(in_axisCount));
	std::copy_n(reinterpret_cast<uint8_t*>(&count), sizeof(uint32_t), std::back_inserter(mod.data));
	count = htonl(static_cast<uint32_t>(in_bufferCount));
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

void tedpad::Module::GamepadFullDescription::update_briefDesciptionCount()
{
	briefDescription.out_digitalCount = get_attributeServerOutCount(Attribute::PlaceHolderType::Digital);
	briefDescription.out_analogueCount = get_attributeServerOutCount(Attribute::PlaceHolderType::Analogue);
	briefDescription.out_axisCount = get_attributeServerOutCount(Attribute::PlaceHolderType::Axis);
	briefDescription.out_bufferCount = get_attributeServerOutCount(Attribute::PlaceHolderType::Buffer);
	briefDescription.in_digitalCount = get_attributeServerInCount(Attribute::PlaceHolderType::Digital);
	briefDescription.in_analogueCount = get_attributeServerInCount(Attribute::PlaceHolderType::Analogue);
	briefDescription.in_axisCount = get_attributeServerInCount(Attribute::PlaceHolderType::Axis);
	briefDescription.in_bufferCount = get_attributeServerInCount(Attribute::PlaceHolderType::Buffer);
}

tedpad::Module::GamepadFullDescription::GamepadFullDescription()
{
	description[Key::Module] = Value::Module::GamepadFullDescription;
}

size_t tedpad::Module::GamepadFullDescription::get_attributeServerOutCount(Attribute::PlaceHolderType const attr_desc) const
{
	return(std::count_if(attribute.begin(), attribute.end(), [&](Module::Attribute::PlaceHolder const &p0)->bool { return((attr_desc == p0.value) && (p0.direction == Module::Attribute::DataDirection::ServerOutput)); }));
}

size_t tedpad::Module::GamepadFullDescription::get_attributeServerInCount(Attribute::PlaceHolderType const attr_desc) const
{
	return(std::count_if(attribute.begin(), attribute.end(), [&](Module::Attribute::PlaceHolder const &p0)->bool { return((attr_desc == p0.value) && (p0.direction == Module::Attribute::DataDirection::ServerInput)); }));
}
