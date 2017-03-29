#include "../../../include/tedpad/modules/gamepadDescription.h"

tedpad::PacketModule tedpad::Module::GamepadBriefDescription::to_packetModule() const
{
	PacketModule mod;
	mod.name = get_description();
	std::copy(gamepadName.begin(), gamepadName.end(), std::back_inserter(mod.data));
	mod.data.push_back('\0');
	mod.data.push_back(out_digitalCount);
	mod.data.push_back(out_analogueCount);
	mod.data.push_back(out_axisCount);
	mod.data.push_back(out_bufferCount);
	mod.data.push_back(in_digitalCount);
	mod.data.push_back(in_analogueCount);
	mod.data.push_back(in_axisCount);
	mod.data.push_back(in_bufferCount);
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
	out_digitalCount = *itr++;
	out_analogueCount = *itr++;
	out_axisCount = *itr++;
	out_bufferCount = *itr++;
	in_bufferCount = *itr++;
	in_analogueCount = *itr++;
	in_axisCount = *itr++;
	in_bufferCount = *itr++;
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

uint8_t tedpad::Module::GamepadFullDescription::get_attributeServerOutCount(Attribute::PlaceHolderType const attr_desc) const
{
	return(static_cast<uint8_t>(std::count_if(attribute.begin(), attribute.end(), [&](Module::Attribute::PlaceHolder const &p0)->bool { return((attr_desc == p0.value) && (p0.direction == Module::Attribute::DataDirection::ServerOutput)); })));
}

uint8_t tedpad::Module::GamepadFullDescription::get_attributeServerInCount(Attribute::PlaceHolderType const attr_desc) const
{
	return(static_cast<uint8_t>(std::count_if(attribute.begin(), attribute.end(), [&](Module::Attribute::PlaceHolder const &p0)->bool { return((attr_desc == p0.value) && (p0.direction == Module::Attribute::DataDirection::ServerInput)); })));
}
