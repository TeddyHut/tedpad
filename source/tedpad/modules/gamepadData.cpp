#include "../../../include/tedpad/modules/gamepadData.h"

tedpad::PacketModule tedpad::Module::GamepadData::to_packetModule() const
{
	PacketModule mod;
	mod.valid = true;
	mod.name = get_description();
	ToNetworkPacket packet;
	std::for_each(vec_attribute.begin(), vec_attribute.end(), [&](Attribute::Generic *const &p0) { if (p0->direction == direction) packet.add_module(p0->to_packetModule()); });

	mod.data.reserve(sizeof(Attribute::DataDirection) + gamepadName.size() + 1 + packet.get_packetSize());
	mod.data.push_back(static_cast<uint8_t>(direction));
	std::copy(gamepadName.begin(), gamepadName.end(), std::back_inserter(mod.data));
	mod.data.push_back('\0');
	auto packetData = packet.get_packetData();
	std::copy(packetData.begin(), packetData.end(), std::back_inserter(mod.data));
	return(mod);
}

void tedpad::Module::GamepadData::from_packetModule(PacketModule const & p0)
{
	std::for_each(vec_attribute.begin(), vec_attribute.end(), [](Attribute::Generic *const &p0) { delete p0; });
	vec_attribute.clear();

	set_description(p0.name);
	if (!(description[Key::Module] & Value::Module::GamepadData)) {
		description[Key::Module] = Value::Module::Invalid;
		return;
	}
	direction = static_cast<Attribute::DataDirection>(p0.data.at(0));
	auto itr = std::find(p0.data.begin() + 1, p0.data.end(), '\0');
	if (itr == p0.data.end()) {
		description[Key::Module] = Value::Module::Invalid;
		return;
	}
	gamepadName = std::string(p0.data.begin() + 1, itr);
	FromNetworkPacket packet;
	packet.set_data(std::vector<uint8_t>(++itr, p0.data.end()));

	auto packet_modules = packet.get_packetModules();
	vec_attribute.reserve(packet_modules.size());
	for (auto &&element : packet_modules) {
		Attribute::Generic *attr_ptr;
		auto attr_description = Name::get_description(element.name);
		if (attr_description[Key::Module] == Value::Module::Attribute_Digital)
			attr_ptr = new Attribute::Digital;
		else if (attr_description[Key::Module] == Value::Module::Attribute_Analogue)
			attr_ptr = new Attribute::Analogue;
		else if (attr_description[Key::Module] == Value::Module::Attribute_Axis)
			attr_ptr = new Attribute::Axis;
		else if (attr_description[Key::Module] == Value::Module::Attribute_Buffer)
			attr_ptr = new Attribute::Buffer;
		else
			continue;
		attr_ptr->from_packetModule(element);
		if (attr_ptr->direction != direction) {
			delete attr_ptr;
			continue;
		}
		vec_attribute.push_back(attr_ptr);
	}
}

void tedpad::Module::GamepadData::add_attribute(Attribute::Generic const * const p0)
{
	if (p0->description[Key::Module] & Value::Module::Attribute_Digital)
		add_attribute(*static_cast<Attribute::Digital const * const>(p0));
	else if (p0->description[Key::Module] & Value::Module::Attribute_Analogue)
		add_attribute(*static_cast<Attribute::Analogue const * const>(p0));
	else if (p0->description[Key::Module] & Value::Module::Attribute_Axis)
		add_attribute(*static_cast<Attribute::Axis const * const>(p0));
	else if (p0->description[Key::Module] & Value::Module::Attribute_Buffer)
		add_attribute(*static_cast<Attribute::Buffer const * const>(p0));
}

void tedpad::Module::GamepadData::add_attribute(Attribute::Digital const & p0)
{
	if (p0.direction == direction) {
		Attribute::Digital *attr = new Attribute::Digital;
		*attr = p0;
		vec_attribute.push_back(attr);
	}
}

void tedpad::Module::GamepadData::add_attribute(Attribute::Analogue const & p0)
{
	if (p0.direction == direction) {
		Attribute::Analogue *attr = new Attribute::Analogue;
		*attr = p0;
		vec_attribute.push_back(attr);
	}
}

void tedpad::Module::GamepadData::add_attribute(Attribute::Axis const & p0)
{
	if (p0.direction == direction) {
		Attribute::Axis *attr = new Attribute::Axis;
		*attr = p0;
		vec_attribute.push_back(attr);
	}
}

void tedpad::Module::GamepadData::add_attribute(Attribute::Buffer const & p0)
{
	if (p0.direction == direction) {
		Attribute::Buffer *attr = new Attribute::Buffer;
		*attr = p0;
		vec_attribute.push_back(attr);
	}
}

tedpad::Module::GamepadData & tedpad::Module::GamepadData::operator=(GamepadData const & p0)
{
	op_assign(p0);
	return(*this);
}

tedpad::Module::GamepadData & tedpad::Module::GamepadData::operator=(GamepadData && p0)
{
	op_assign(std::move(p0));
	return(*this);
}

tedpad::Module::GamepadData::GamepadData(GamepadData const & p0)
{
	op_assign(p0);
}

tedpad::Module::GamepadData::GamepadData(GamepadData && p0)
{
	op_assign(std::move(p0));
}

tedpad::Module::GamepadData::GamepadData()
{
	description[Key::Module] = Value::Module::GamepadData;
}

tedpad::Module::GamepadData::~GamepadData()
{
	std::for_each(vec_attribute.begin(), vec_attribute.end(), [](Attribute::Generic *const &p0) { delete p0; });
}

void tedpad::Module::GamepadData::op_assign(GamepadData const & p0)
{
	gamepadName = p0.gamepadName;
	direction = p0.direction;
	description = p0.description;
	std::for_each(vec_attribute.begin(), vec_attribute.end(), [](Attribute::Generic *const &p0) { delete p0; });
	vec_attribute.clear();
	std::for_each(p0.vec_attribute.begin(), p0.vec_attribute.end(), [&](Attribute::Generic const *const &p0) { add_attribute(p0); });
}

void tedpad::Module::GamepadData::op_assign(GamepadData && p0)
{
	gamepadName = std::move(p0.gamepadName);
	direction = std::move(p0.direction);
	description = std::move(p0.description);
	std::for_each(vec_attribute.begin(), vec_attribute.end(), [](Attribute::Generic *const &p0) { delete p0; });
	vec_attribute = std::move(p0.vec_attribute);
	p0.vec_attribute.clear();
}
