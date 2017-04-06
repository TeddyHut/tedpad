#include "../../../include/tedpad/gamepad/gamepad.h"

tedpad::Gamepad::p_AttributeMatch::p_AttributeMatch(eg::Descriptor<> const description, Module::Attribute::DataDirection const direction, std::string const & name) : attr_description(description), attr_direction(direction), attr_name(name)
{
}

std::map<eg::Descriptor<> const, tedpad::Module::Attribute::PlaceHolderType const> const tedpad::Gamepad::map_Description_PlaceHolderType {
	{ eg::Descriptor<>::Descriptor({ { Key::Module,{ Value::Module::Attribute_Digital } } }), Module::Attribute::PlaceHolderType::Digital },
	{ eg::Descriptor<>::Descriptor({ { Key::Module,{ Value::Module::Attribute_Analogue } } }), Module::Attribute::PlaceHolderType::Analogue },
	{ eg::Descriptor<>::Descriptor({ { Key::Module,{ Value::Module::Attribute_Axis } } }), Module::Attribute::PlaceHolderType::Axis },
	{ eg::Descriptor<>::Descriptor({ { Key::Module,{ Value::Module::Attribute_Buffer } } }), Module::Attribute::PlaceHolderType::Buffer },
};

bool tedpad::Gamepad::p_AttributeMatch::operator()(Module::Attribute::Generic const * const p0) const
{
	return((attr_description & p0->description) && (attr_direction == p0->direction) && (p0->attributeName == attr_name));
}

tedpad::Gamepad::p_AttributeDetermine_PlaceHolderType::p_AttributeDetermine_PlaceHolderType(Module::Attribute::Generic const * const p0)
{
	if (p0 != nullptr)
		value = (*this)(p0);
}

tedpad::Module::Attribute::PlaceHolderType tedpad::Gamepad::p_AttributeDetermine_PlaceHolderType::operator()(Module::Attribute::Generic const * const p0) const
{
	return((p0->description[Key::Module] & Value::Module::Attribute_PlaceHolder) ? dynamic_cast<Module::Attribute::PlaceHolder const *>(p0)->value : map_Description_PlaceHolderType.at(p0->description[Key::Module]));
}

tedpad::Gamepad::p_AttributeDetermine_PlaceHolderType::operator tedpad::Module::Attribute::PlaceHolderType() const
{
	return(value);
}

tedpad::Module::Attribute::PlaceHolderType tedpad::Gamepad::p_AttributeDetermine_Description::operator()(Module::Attribute::Generic const * const p0) const
{
	return(map_Description_PlaceHolderType.at(p0->description[Key::Module]));
}


bool tedpad::Gamepad::Get_attribute(std::string const & attribute, bool & out) const
{
	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(eg::Descriptor<>({ { Key::Module, { Value::Module::Attribute_Digital } } }), conv_attributeDirection_to_dataDirection(AttributeDirection::Get), attribute));
	if (itr == vec_attribute.end())
		return(true);
	out = dynamic_cast<Module::Attribute::Digital *>((*itr))->value;
	return(false);
}

bool tedpad::Gamepad::Get_attribute(std::string const & attribute, uint8_t & out) const
{
	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(eg::Descriptor<>({ { Key::Module,{ Value::Module::Attribute_Analogue } } }), conv_attributeDirection_to_dataDirection(AttributeDirection::Get), attribute));
	if (itr == vec_attribute.end())
		return(true);
	out = dynamic_cast<Module::Attribute::Analogue *>((*itr))->value;
	return(false);
}

bool tedpad::Gamepad::Get_attribute(std::string const & attribute, int16_t & out) const
{
	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(eg::Descriptor<>({ { Key::Module,{ Value::Module::Attribute_Axis } } }), conv_attributeDirection_to_dataDirection(AttributeDirection::Get), attribute));
	if (itr == vec_attribute.end())
		return(true);
	out = dynamic_cast<Module::Attribute::Axis *>((*itr))->value;
	return(false);
}

bool tedpad::Gamepad::Get_attribute(std::string const & attribute, std::vector<uint8_t>& out) const
{
	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(eg::Descriptor<>({ { Key::Module,{ Value::Module::Attribute_Buffer } } }), conv_attributeDirection_to_dataDirection(AttributeDirection::Get), attribute));
	if (itr == vec_attribute.end())
		return(true);
	out = dynamic_cast<Module::Attribute::Buffer *>((*itr))->value;
	return(false);
}

tedpad::Gamepad::AttributeType tedpad::Gamepad::get_attributeType(std::string const & attribute) const
{

	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), [&](Module::Attribute::Generic const *const p0) { return(p0->attributeName == attribute); });
	if (itr == vec_attribute.end())
		return(AttributeType::Invalid);
	return(conv_placeHolderType_to_attributeType(p_AttributeDetermine_PlaceHolderType(*itr)));
}

bool tedpad::Gamepad::Set_attribute(std::string const & attribute, bool const in)
{
	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(eg::Descriptor<>({ { Key::Module,{ Value::Module::Attribute_Digital } } }), conv_attributeDirection_to_dataDirection(AttributeDirection::Set), attribute));
	if (itr == vec_attribute.end())
		return(true);
	dynamic_cast<Module::Attribute::Digital *>((*itr))->value = in;
	return(false);
}

bool tedpad::Gamepad::Set_attribute(std::string const & attribute, uint8_t const in)
{
	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(eg::Descriptor<>({ { Key::Module,{ Value::Module::Attribute_Analogue } } }), conv_attributeDirection_to_dataDirection(AttributeDirection::Set), attribute));
	if (itr == vec_attribute.end())
		return(true);
	dynamic_cast<Module::Attribute::Analogue *>((*itr))->value = in;
	return(false);
}

bool tedpad::Gamepad::Set_attribute(std::string const & attribute, int16_t const in)
{
	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(eg::Descriptor<>({ { Key::Module,{ Value::Module::Attribute_Axis } } }), conv_attributeDirection_to_dataDirection(AttributeDirection::Set), attribute));
	if (itr == vec_attribute.end())
		return(true);
	dynamic_cast<Module::Attribute::Axis *>((*itr))->value = in;
	return(false);
}

bool tedpad::Gamepad::Set_attribute(std::string const & attribute, std::vector<uint8_t> const & in)
{
	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(eg::Descriptor<>({ { Key::Module,{ Value::Module::Attribute_Buffer } } }), conv_attributeDirection_to_dataDirection(AttributeDirection::Set), attribute));
	if (itr == vec_attribute.end())
		return(true);
	dynamic_cast<Module::Attribute::Buffer *>((*itr))->value = in;
	return(false);
}

void tedpad::Gamepad::from_tpdFile(std::string const & fileContent)
{
	from_gamepadFullDescription(File::TPD_to_GamepadFullDescription(fileContent));
}

void tedpad::Gamepad::from_tpdFile(std::vector<uint8_t> const & fileContent)
{
	from_tpdFile(std::string(fileContent.begin(), fileContent.end()));
}

void tedpad::Gamepad::from_tpdFile(char const * const fileContent, size_t const len)
{
	from_tpdFile(std::string(fileContent, len));
}

void tedpad::Gamepad::from_tpdFile(uint8_t const * const fileContent, size_t const len)
{
	from_tpdFile(std::string(reinterpret_cast<char const *>(fileContent), len));
}

void tedpad::Gamepad::from_gamepadFullDescription(Module::GamepadFullDescription const & p0)
{
	std::for_each(vec_attribute.begin(), vec_attribute.end(), [](Module::Attribute::Generic *const p0) { delete p0; });
	vec_attribute.clear();

	gamepadName = p0.briefDescription.gamepadName;
	for (auto &&element : p0.attribute) {
		//TODO: Work out why this didn't work in a lambda in std::for_each
		add_attribute<p_AttributeDetermine_PlaceHolderType>(dynamic_cast<Module::Attribute::Generic const *>(&element));
	}
}

tedpad::Module::GamepadBriefDescription tedpad::Gamepad::to_gamepadBriefDescription() const
{
	Module::GamepadBriefDescription rtrn;
	rtrn.gamepadName = gamepadName;
	rtrn.out_digitalCount = get_attributeServerOutCount(eg::Descriptor<>({ {Key::Module, {Value::Module::Attribute_Digital}} }));
	rtrn.out_analogueCount = get_attributeServerOutCount(eg::Descriptor<>({ { Key::Module, { Value::Module::Attribute_Analogue } } }));
	rtrn.out_axisCount = get_attributeServerOutCount(eg::Descriptor<>({ { Key::Module, { Value::Module::Attribute_Axis } } }));
	rtrn.out_bufferCount = get_attributeServerOutCount(eg::Descriptor<>({ { Key::Module, { Value::Module::Attribute_Buffer } } }));
	rtrn.in_digitalCount = get_attributeServerInCount(eg::Descriptor<>({ { Key::Module, { Value::Module::Attribute_Digital } } }));
	rtrn.in_analogueCount = get_attributeServerInCount(eg::Descriptor<>({ { Key::Module, { Value::Module::Attribute_Analogue } } }));
	rtrn.in_axisCount = get_attributeServerInCount(eg::Descriptor<>({ { Key::Module, { Value::Module::Attribute_Axis } } }));
	rtrn.in_bufferCount = get_attributeServerInCount(eg::Descriptor<>({ { Key::Module, { Value::Module::Attribute_Buffer } } }));
	return(rtrn);
}

tedpad::Module::GamepadFullDescription tedpad::Gamepad::to_gamepadFullDescription() const
{
	Module::GamepadFullDescription rtrn;
	rtrn.briefDescription = to_gamepadBriefDescription();
	for (auto &&element : vec_attribute) {
		Module::Attribute::PlaceHolder attr_placeHolder;
		attr_placeHolder.attributeName = element->attributeName;
		attr_placeHolder.direction = element->direction;
		attr_placeHolder.value = map_Description_PlaceHolderType.at(element->description[Key::Module]);
		rtrn.attribute.push_back(attr_placeHolder);
	}
	return(rtrn);
}

void tedpad::Gamepad::set_gamepadData(Module::GamepadData const & p0)
{
	if (gamepadName == p0.gamepadName) {
		//Check that read only values are not trying to be set
		if (p0.direction == conv_attributeDirection_to_dataDirection(AttributeDirection::Set)) {
			for (auto &&element : p0.vec_attribute) {
				if (element->direction != p0.direction)
					continue;
				auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(element->description[Key::Module], element->direction, element->attributeName));
				if (itr == vec_attribute.end())
					continue;
				set_attributeItr(itr, element);
			}
		}
	}
}

tedpad::Module::GamepadData tedpad::Gamepad::get_gamepadData() const
{
	Module::GamepadData rtrn;
	rtrn.gamepadName = gamepadName;
	rtrn.direction = conv_attributeDirection_to_dataDirection(AttributeDirection::Get);
	std::for_each(vec_attribute.begin(), vec_attribute.end(), [&](Module::Attribute::Generic const *const p0) { if(p0->direction == conv_attributeDirection_to_dataDirection(AttributeDirection::Get)) rtrn.add_attribute(p0); });
	return(rtrn);
}

void tedpad::Gamepad::set_gamepadData_dataDirection(Module::GamepadData const & p0)
{
	if (gamepadName == p0.gamepadName) {
		for (auto &&element : p0.vec_attribute) {
			if (element->direction != p0.direction)
				continue;
			auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(element->description[Key::Module], element->direction, element->attributeName));
			if (itr == vec_attribute.end())
				continue;
			set_attributeItr(itr, element);
		}
	}
}

tedpad::Module::GamepadData tedpad::Gamepad::get_gamepadData_dataDirection(Module::Attribute::DataDirection const dataDirection) const
{
	Module::GamepadData rtrn;
	rtrn.gamepadName = gamepadName;
	rtrn.direction = dataDirection;
	std::for_each(vec_attribute.begin(), vec_attribute.end(), [&](Module::Attribute::Generic const *const p0) { if (p0->direction == dataDirection) rtrn.add_attribute(p0); });
	return(rtrn);
}

void tedpad::Gamepad::set_IODirection(IODirection const p0)
{
	ioDirection = p0;
}

template <typename predicate_t>
void tedpad::Gamepad::add_attribute(Module::Attribute::Generic const * const p0, predicate_t p)
{
	Module::Attribute::Generic *attr_ptr;
	if (p(p0) == Module::Attribute::PlaceHolderType::Digital)
		attr_ptr = new Module::Attribute::Digital;
	else if (p(p0) == Module::Attribute::PlaceHolderType::Analogue)
		attr_ptr = new Module::Attribute::Analogue;
	else if (p(p0) == Module::Attribute::PlaceHolderType::Axis)
		attr_ptr = new Module::Attribute::Axis;
	else if (p(p0) == Module::Attribute::PlaceHolderType::Buffer)
		attr_ptr = new Module::Attribute::Buffer;
	else
		return;
	attr_ptr->attributeName = p0->attributeName;
	attr_ptr->direction = p0->direction;
	vec_attribute.push_back(attr_ptr);
}

void tedpad::Gamepad::add_newAttribute(AttributeType const type, AttributeDirection const direction, std::string const & name)
{
	Module::Attribute::PlaceHolder attr;
	attr.attributeName = name;
	attr.direction = conv_attributeDirection_to_dataDirection(direction);
	attr.value = conv_attributeType_to_placeHolderType(type);
	add_attribute<p_AttributeDetermine_PlaceHolderType>(dynamic_cast<Module::Attribute::Generic const *>(&attr));
}

tedpad::Gamepad & tedpad::Gamepad::operator=(Gamepad const & p0)
{
	op_assign(p0);
	return(*this);
}

tedpad::Gamepad & tedpad::Gamepad::operator=(Gamepad && p0)
{
	op_assign(std::move(p0));
	return(*this);
}

tedpad::Gamepad::Gamepad(Gamepad const & p0)
{
	op_assign(p0);
}

tedpad::Gamepad::Gamepad(Gamepad && p0)
{
	op_assign(std::move(p0));
}

tedpad::Gamepad::Gamepad()
{
}

tedpad::Gamepad::Gamepad(std::string const & name, IODirection ioDirection) : gamepadName(name), ioDirection(ioDirection)
{
}

tedpad::Gamepad::~Gamepad()
{
	std::for_each(vec_attribute.begin(), vec_attribute.end(), [](Module::Attribute::Generic *const p0) { delete p0; });
}

std::map<tedpad::Gamepad::AttributeType const, tedpad::Module::Attribute::PlaceHolderType const> const tedpad::Gamepad::map_AttributeType_PlaceHolderType{
	{ AttributeType::Digital, Module::Attribute::PlaceHolderType::Digital },
	{ AttributeType::Analogue, Module::Attribute::PlaceHolderType::Analogue },
	{ AttributeType::Axis, Module::Attribute::PlaceHolderType::Axis },
	{ AttributeType::Buffer, Module::Attribute::PlaceHolderType::Buffer }
};

void tedpad::Gamepad::op_assign(Gamepad const & p0)
{
	gamepadName = p0.gamepadName;
	ioDirection = p0.ioDirection;
	std::for_each(vec_attribute.begin(), vec_attribute.end(), [](Module::Attribute::Generic *const p0) { delete p0; });
	vec_attribute.clear();
	std::for_each(p0.vec_attribute.begin(), p0.vec_attribute.end(), [&](Module::Attribute::Generic const *const &p0) { add_attribute(p0); });
}

void tedpad::Gamepad::op_assign(Gamepad && p0)
{
	gamepadName = std::move(p0.gamepadName);
	ioDirection = std::move(p0.ioDirection);
	std::for_each(vec_attribute.begin(), vec_attribute.end(), [](Module::Attribute::Generic *const p0) { delete p0; });
	vec_attribute = std::move(p0.vec_attribute);
	p0.vec_attribute.clear();
}

tedpad::Module::Attribute::DataDirection tedpad::Gamepad::conv_attributeDirection_to_dataDirection(AttributeDirection const direction) const
{
	if (ioDirection == IODirection::Server)
		if (direction == AttributeDirection::Get)
			return(Module::Attribute::DataDirection::ServerInput);
		else
			return(Module::Attribute::DataDirection::ServerOutput);
	else
		if (direction == AttributeDirection::Get)
			return(Module::Attribute::DataDirection::ServerOutput);
		else
			return(Module::Attribute::DataDirection::ServerInput);
}

tedpad::Module::Attribute::PlaceHolderType tedpad::Gamepad::conv_attributeType_to_placeHolderType(AttributeType const type) const
{
	return(map_AttributeType_PlaceHolderType.at(type));
}

tedpad::Gamepad::AttributeType tedpad::Gamepad::conv_placeHolderType_to_attributeType(Module::Attribute::PlaceHolderType const type) const
{
	auto itr = std::find_if(map_AttributeType_PlaceHolderType.begin(), map_AttributeType_PlaceHolderType.end(), [&](std::pair<AttributeType const, Module::Attribute::PlaceHolderType const> const &p0) { return(p0.second == type); });
	if (itr == map_AttributeType_PlaceHolderType.end())
		return(AttributeType::Invalid);
	return((*itr).first);
}

uint8_t tedpad::Gamepad::get_attributeServerOutCount(eg::Descriptor<> const attr_desc) const
{
	return(static_cast<uint8_t>(std::count_if(vec_attribute.begin(), vec_attribute.end(), [&](Module::Attribute::Generic const *const p0)->bool { return((attr_desc & p0->description) && (p0->direction == Module::Attribute::DataDirection::ServerOutput)); })));
}

uint8_t tedpad::Gamepad::get_attributeServerInCount(eg::Descriptor<> const attr_desc) const
{
	return(static_cast<uint8_t>(std::count_if(vec_attribute.begin(), vec_attribute.end(), [&](Module::Attribute::Generic const *const p0)->bool { return((attr_desc & p0->description) && (p0->direction == Module::Attribute::DataDirection::ServerInput)); })));
}

void tedpad::Gamepad::set_attributeItr(std::vector<Module::Attribute::Generic*>::iterator const itr, Module::Attribute::Generic const * const p0)
{
	if ((*itr)->description == p0->description) {
		if (p0->description[Key::Module] == Value::Module::Attribute_Digital)
			dynamic_cast<Module::Attribute::Digital *>(*itr)->value = dynamic_cast<Module::Attribute::Digital const *>(p0)->value;
		else if (p0->description[Key::Module] == Value::Module::Attribute_Analogue)
			dynamic_cast<Module::Attribute::Analogue *>(*itr)->value = dynamic_cast<Module::Attribute::Analogue const *>(p0)->value;
		else if (p0->description[Key::Module] == Value::Module::Attribute_Axis)
			dynamic_cast<Module::Attribute::Axis *>(*itr)->value = dynamic_cast<Module::Attribute::Axis const *>(p0)->value;
		else if (p0->description[Key::Module] == Value::Module::Attribute_Buffer)
			dynamic_cast<Module::Attribute::Buffer *>(*itr)->value = dynamic_cast<Module::Attribute::Buffer const *>(p0)->value;
	}
}

void tedpad::Gamepad::get_attributeItr(std::vector<Module::Attribute::Generic*>::const_iterator const itr, Module::Attribute::Generic * const p0) const
{
	if ((*itr)->description == p0->description) {
		if (p0->description[Key::Module] == Value::Module::Attribute_Digital)
			dynamic_cast<Module::Attribute::Digital *>(p0)->value = dynamic_cast<Module::Attribute::Digital const *>(*itr)->value;
		else if (p0->description[Key::Module] == Value::Module::Attribute_Analogue)
			dynamic_cast<Module::Attribute::Analogue *>(p0)->value = dynamic_cast<Module::Attribute::Analogue const *>(*itr)->value;
		else if (p0->description[Key::Module] == Value::Module::Attribute_Axis)
			dynamic_cast<Module::Attribute::Axis *>(p0)->value = dynamic_cast<Module::Attribute::Axis const *>(*itr)->value;
		else if (p0->description[Key::Module] == Value::Module::Attribute_Buffer)
			dynamic_cast<Module::Attribute::Buffer *>(p0)->value = dynamic_cast<Module::Attribute::Buffer const *>(*itr)->value;
	}
}
