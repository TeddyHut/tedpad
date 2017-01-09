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
	auto itr = std::find_if(Module::Name::nameMap.begin(), Module::Name::nameMap.end(), [&](std::pair<eg::Descriptor<>, std::string> const &p0) { return (p0.second == moduleName); });
	if (itr == Module::Name::nameMap.end())
		description[Key::Module] = Value::Module::Invalid;
	description[Key::Module] = (*itr).first;
}

tedpad::Module::ModuleBase::ModuleBase()
{
	description[Key::Module] = Value::Module::Invalid;
}

tedpad::Module::ModuleBase::operator tedpad::PacketModule() const {
	return(to_packetModule());
}
