#include "../../../include/tedpad/modules/moduleBase.h"

tedpad::Module::ModuleBase &tedpad::Module::ModuleBase::operator=(PacketModule const &p0) {
	from_packetModule(p0);
	return(*this);
}

std::string tedpad::Module::ModuleBase::get_description() const
{
	return(Module::Name::nameMap.at(description[Key::Module]));
}

void tedpad::Module::ModuleBase::set_description(std::string const & moduleName)
{
	description[Key::Module] = Name::get_description(moduleName);
}

tedpad::Module::ModuleBase::ModuleBase()
{
	description[Key::Module] = Value::Module::Invalid;
}

tedpad::Module::ModuleBase::~ModuleBase()
{
}

tedpad::Module::ModuleBase::operator tedpad::PacketModule() const {
	return(to_packetModule());
}
