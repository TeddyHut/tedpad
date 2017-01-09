#include "../../../include/tedpad/gamepad/gamepad.h"

tedpad::Gamepad::p_AttributeMatch::p_AttributeMatch(eg::Descriptor<> const description, Module::Attribute::DataDirection const direction, std::string const & name) : attr_description(description), attr_direction(direction), attr_name(name)
{
}

bool tedpad::Gamepad::p_AttributeMatch::operator()(Module::Attribute::Generic const * const p0) const
{
	return((attr_description & p0->description) && (attr_direction == p0->direction) && (p0->attributeName == attr_name));
}

bool tedpad::Gamepad::Get_attribute(std::string const & attribute, bool & out) const
{
	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(eg::Descriptor<>({ { Key::Module, { Value::Module::Attribute_Digital } } }), get_dataDirection(true), attribute));
	if (itr == vec_attribute.end())
		return(true);
	out = dynamic_cast<Module::Attribute::Digital *>((*itr))->value;
	return(false);
}

bool tedpad::Gamepad::Get_attribute(std::string const & attribute, uint8_t & out) const
{
	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(eg::Descriptor<>({ { Key::Module,{ Value::Module::Attribute_Analogue } } }), get_dataDirection(true), attribute));
	if (itr == vec_attribute.end())
		return(true);
	out = dynamic_cast<Module::Attribute::Analogue *>((*itr))->value;
	return(false);
}

bool tedpad::Gamepad::Get_attribute(std::string const & attribute, int16_t & out) const
{
	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(eg::Descriptor<>({ { Key::Module,{ Value::Module::Attribute_Axis } } }), get_dataDirection(true), attribute));
	if (itr == vec_attribute.end())
		return(true);
	out = dynamic_cast<Module::Attribute::Axis *>((*itr))->value;
	return(false);
}

bool tedpad::Gamepad::Get_attribute(std::string const & attribute, std::vector<uint8_t>& out) const
{
	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(eg::Descriptor<>({ { Key::Module,{ Value::Module::Attribute_Buffer } } }), get_dataDirection(true), attribute));
	if (itr == vec_attribute.end())
		return(true);
	out = dynamic_cast<Module::Attribute::Buffer *>((*itr))->value;
	return(false);
}

bool tedpad::Gamepad::Set_attribute(std::string const & attribute, bool const in)
{
	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(eg::Descriptor<>({ { Key::Module,{ Value::Module::Attribute_Digital } } }), get_dataDirection(false), attribute));
	if (itr == vec_attribute.end())
		return(true);
	dynamic_cast<Module::Attribute::Digital *>((*itr))->value = in;
	return(false);
}

bool tedpad::Gamepad::Set_attribute(std::string const & attribute, uint8_t const in)
{
	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(eg::Descriptor<>({ { Key::Module,{ Value::Module::Attribute_Analogue } } }), get_dataDirection(false), attribute));
	if (itr == vec_attribute.end())
		return(true);
	dynamic_cast<Module::Attribute::Analogue *>((*itr))->value = in;
	return(false);
}

bool tedpad::Gamepad::Set_attribute(std::string const & attribute, int16_t const in)
{
	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(eg::Descriptor<>({ { Key::Module,{ Value::Module::Attribute_Axis } } }), get_dataDirection(false), attribute));
	if (itr == vec_attribute.end())
		return(true);
	dynamic_cast<Module::Attribute::Axis *>((*itr))->value = in;
	return(false);
}

bool tedpad::Gamepad::Set_attribute(std::string const & attribute, std::vector<uint8_t> const & in)
{
	auto itr = std::find_if(vec_attribute.begin(), vec_attribute.end(), p_AttributeMatch(eg::Descriptor<>({ { Key::Module,{ Value::Module::Attribute_Buffer } } }), get_dataDirection(false), attribute));
	if (itr == vec_attribute.end())
		return(true);
	dynamic_cast<Module::Attribute::Buffer *>((*itr))->value = in;
	return(false);
}

void tedpad::Gamepad::from_gamepadFullDescription(Module::GamepadFullDescription const & p0)
{
	vec_attribute.clear();
	gamepadName = p0.briefDescription.gamepadName;
	for (auto &&element : p0.attribute) {
		Module::Attribute::Generic *attr_ptr;
		if (element.value == Module::Attribute::PlaceHolderType::Digital)
			attr_ptr = new Module::Attribute::Digital;
		else if (element.value == Module::Attribute::PlaceHolderType::Analogue)
			attr_ptr = new Module::Attribute::Analogue;
		else if (element.value == Module::Attribute::PlaceHolderType::Axis)
			attr_ptr = new Module::Attribute::Axis;
		else if (element.value == Module::Attribute::PlaceHolderType::Buffer)
			attr_ptr = new Module::Attribute::Buffer;
		attr_ptr->attributeName = element.attributeName;
		attr_ptr->direction = element.direction;
		vec_attribute.push_back(dynamic_cast<Module::Attribute::Generic *>(attr_ptr));
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
		if (element->description[Key::Module] & Value::Module::Attribute_Digital)
			attr_placeHolder.value = Module::Attribute::PlaceHolderType::Digital;
		else if (element->description[Key::Module] & Value::Module::Attribute_Analogue)
			attr_placeHolder.value = Module::Attribute::PlaceHolderType::Analogue;
		else if (element->description[Key::Module] & Value::Module::Attribute_Axis)
			attr_placeHolder.value = Module::Attribute::PlaceHolderType::Axis;
		else if (element->description[Key::Module] & Value::Module::Attribute_Buffer)
			attr_placeHolder.value = Module::Attribute::PlaceHolderType::Buffer;
		rtrn.attribute.push_back(attr_placeHolder);
	}
	return(rtrn);
}

void tedpad::Gamepad::set_IODirection(IODirection const p0)
{
	ioDirection = p0;
}

void tedpad::Gamepad::add_attribute(Module::Attribute::Generic const * const p0)
{
	Module::Attribute::Generic *attr_ptr;
	if (p0->description[Key::Module] & Value::Module::Attribute_Digital)
		attr_ptr = new Module::Attribute::Digital;
	else if (p0->description[Key::Module] & Value::Module::Attribute_Analogue)
		attr_ptr = new Module::Attribute::Analogue;
	else if (p0->description[Key::Module] & Value::Module::Attribute_Axis)
		attr_ptr = new Module::Attribute::Axis;
	else if (p0->description[Key::Module] & Value::Module::Attribute_Buffer)
		attr_ptr = new Module::Attribute::Buffer;
	attr_ptr->attributeName = p0->attributeName;
	attr_ptr->direction = p0->direction;
	vec_attribute.push_back(dynamic_cast<Module::Attribute::Generic *>(attr_ptr));
}

tedpad::Gamepad::~Gamepad()
{
	std::for_each(vec_attribute.begin(), vec_attribute.end(), [](Module::Attribute::Generic *const p0) { delete p0; });
}

tedpad::Module::Attribute::DataDirection tedpad::Gamepad::get_dataDirection(bool const getting) const
{
	if (ioDirection == IODirection::Server)
		if (getting)
			return(Module::Attribute::DataDirection::ServerInput);
		else
			return(Module::Attribute::DataDirection::ServerOutput);
	else
		if (getting)
			return(Module::Attribute::DataDirection::ServerOutput);
		else
			return(Module::Attribute::DataDirection::ServerInput);
}

size_t tedpad::Gamepad::get_attributeServerOutCount(eg::Descriptor<> const attr_desc) const
{
	return(std::count_if(vec_attribute.begin(), vec_attribute.end(), [&](Module::Attribute::Generic const *const p0)->bool { return((attr_desc & p0->description) && (p0->direction == Module::Attribute::DataDirection::ServerOutput)); }));
}

size_t tedpad::Gamepad::get_attributeServerInCount(eg::Descriptor<> const attr_desc) const
{
	return(std::count_if(vec_attribute.begin(), vec_attribute.end(), [&](Module::Attribute::Generic const *const p0)->bool { return((attr_desc & p0->description) && (p0->direction == Module::Attribute::DataDirection::ServerInput)); }));
}
