#include "../../../include/tedpad/modules/attribute.h"

tedpad::PacketModule tedpad::Module::Attribute::Generic::to_packetModule() const {
	PacketModule mod;
	//Add some exception checking in here
	mod.name = get_description();
	mod.data.push_back(static_cast<uint8_t>(direction));
	std::copy(attributeName.begin(), attributeName.end(), std::back_inserter(mod.data));
	mod.data.push_back('\0');
	auto buf = get_packetModuleData();
	std::copy(buf.begin(), buf.end(), std::back_inserter(mod.data));
	mod.valid = true;
	return(mod);
}

void tedpad::Module::Attribute::Generic::from_packetModule(PacketModule const &p0) {
	set_description(p0.name);
	if (description[Key::Module] & Value::Module::Invalid)
		return;
	direction = static_cast<DataDirection>(p0.data.at(0));
	auto itr = std::find(p0.data.begin() + 1, p0.data.end(), '\0');
	if (itr == p0.data.end()) {
		description[Key::Module] = Value::Module::Invalid;
		return;
	}
	attributeName = std::string(p0.data.begin(), itr);
	set_packetModuleData(std::vector<uint8_t>(++itr, p0.data.end()));
}

tedpad::Module::Attribute::Generic::Generic()
{
	description[Key::Module] = Value::Module::Attribute_Generic;
}

tedpad::Module::Attribute::Generic::~Generic()
{
}

tedpad::Module::Attribute::PlaceHolder::operator tedpad::Module::Attribute::PlaceHolderType() const
{
	return(value);
}

tedpad::Module::Attribute::PlaceHolder & tedpad::Module::Attribute::PlaceHolder::operator=(PlaceHolderType const p0)
{
	value = p0;
	return(*this);
}

tedpad::Module::Attribute::PlaceHolder::PlaceHolder()
{
	description[Key::Module] = Value::Module::Attribute_PlaceHolder;
	value_ptr = &value;
}

std::vector<uint8_t> tedpad::Module::Attribute::PlaceHolder::get_packetModuleData() const
{
	return(std::vector<uint8_t>({ static_cast<uint8_t>(value) }));
}

void tedpad::Module::Attribute::PlaceHolder::set_packetModuleData(std::vector<uint8_t> const & p0)
{
	value = static_cast<PlaceHolderType>(p0.at(0));
}

tedpad::Module::Attribute::Digital::operator bool() const {
	return(value);
}

tedpad::Module::Attribute::Digital &tedpad::Module::Attribute::Digital::operator=(bool const p0) {
	value = p0;
	return(*this);
}

tedpad::Module::Attribute::Digital::Digital() {
	description[Key::Module] = Value::Module::Attribute_Digital;
	value_ptr = &value;
}

std::vector<uint8_t> tedpad::Module::Attribute::Digital::get_packetModuleData() const
{
	return(std::vector<uint8_t>({ static_cast<uint8_t>(value) }));
}

void tedpad::Module::Attribute::Digital::set_packetModuleData(std::vector<uint8_t> const & p0)
{
	value = static_cast<bool>(p0.at(0));
}

tedpad::Module::Attribute::Analogue::operator uint8_t() const {
	return(value);
}

tedpad::Module::Attribute::Analogue &tedpad::Module::Attribute::Analogue::operator=(uint8_t const p0) {
	value = p0;
	return(*this);
}

tedpad::Module::Attribute::Analogue::Analogue() {
	description[Key::Module] = Value::Module::Attribute_Analogue;
	value_ptr = &value;
}

std::vector<uint8_t> tedpad::Module::Attribute::Analogue::get_packetModuleData() const
{
	return(std::vector<uint8_t>({ static_cast<uint8_t>(value) }));
}

void tedpad::Module::Attribute::Analogue::set_packetModuleData(std::vector<uint8_t> const & p0)
{
	value = p0.at(0);
}

tedpad::Module::Attribute::Axis::operator int16_t() const {
	return(value);
}

tedpad::Module::Attribute::Axis &tedpad::Module::Attribute::Axis::operator=(int16_t const p0) {
	value = p0;
	return(*this);
}

tedpad::Module::Attribute::Axis::Axis() {
	description[Key::Module] = Value::Module::Attribute_Axis;
	value_ptr = &value;
}

std::vector<uint8_t> tedpad::Module::Attribute::Axis::get_packetModuleData() const
{
	std::vector<uint8_t> rtrn;
	uint16_t val = htons(value + (-INT16_MIN));
	std::copy_n(reinterpret_cast<uint8_t*>(&val), sizeof(uint16_t), std::back_inserter(rtrn));
	return(rtrn);
}

void tedpad::Module::Attribute::Axis::set_packetModuleData(std::vector<uint8_t> const & p0)
{
	uint16_t val;
	std::copy_n(p0.begin(), sizeof(uint16_t), reinterpret_cast<uint8_t*>(&val));
	value = (ntohs(val) - (-INT16_MIN));
}

tedpad::Module::Attribute::Buffer::operator uint8_t*() {
	return(value.data());
}

tedpad::Module::Attribute::Buffer::operator uint8_t const *() const
{
	return(value.data());
}

tedpad::Module::Attribute::Buffer::Buffer() {
	description[Key::Module] = Value::Module::Attribute_Buffer;
	value_ptr = &value;
}

std::vector<uint8_t> tedpad::Module::Attribute::Buffer::get_packetModuleData() const
{
	return(value);
}

void tedpad::Module::Attribute::Buffer::set_packetModuleData(std::vector<uint8_t> const & p0)
{
	value = p0;
}
