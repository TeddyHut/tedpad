#include "../../include/tedpad/moduleName.h"

namespace tedpad {
	std::map<eg::Descriptor<> const, std::string const> const tedpad::Module::Name::nameMap{
		{ eg::Descriptor<>::Descriptor({ { Key::Module, { Value::Module::Attribute_Generic } } }), "AttGen" },
		{ eg::Descriptor<>::Descriptor({ { Key::Module, { Value::Module::Attribute_PlaceHolder } } }), "AttPla" },
		{ eg::Descriptor<>::Descriptor({ { Key::Module, { Value::Module::Attribute_Digital } } }), "AttDig" },
		{ eg::Descriptor<>::Descriptor({ { Key::Module, { Value::Module::Attribute_Analogue } } }), "AttAna" },
		{ eg::Descriptor<>::Descriptor({ { Key::Module, { Value::Module::Attribute_Axis } } }), "AttAxi" },
		{ eg::Descriptor<>::Descriptor({ { Key::Module, { Value::Module::Attribute_Buffer } } }), "AttBuf" },
		{ eg::Descriptor<>::Descriptor({ { Key::Module, { Value::Module::GamepadBriefDescription } } }), "GameBr" },
		{ eg::Descriptor<>::Descriptor({ { Key::Module, { Value::Module::GamepadFullDescription } } }), "GameFu" },
		{ eg::Descriptor<>::Descriptor({ { Key::Module, { Value::Module::GamepadData } } }), "GameDa" },
		{ eg::Descriptor<>::Descriptor({ { Key::Module, { Value::Module::Request } } }), "Reques" },
		{ eg::Descriptor<>::Descriptor({ { Key::Module, { Value::Module::Reply } } }), "Reply" },
	};
}

eg::Descriptor<> tedpad::Module::Name::get_description(std::string const & moduleName)
{
	//Probs could compact this down a little more
	eg::Descriptor<> rtrn;
	auto itr = std::find_if(nameMap.begin(), nameMap.end(), [&](std::pair<eg::Descriptor<> const, std::string const> const &p0) { return (p0.second == moduleName); });
	if (itr == nameMap.end())
		rtrn[Key::Module] = Value::Module::Invalid;
	else
		rtrn = (*itr).first;
	return(rtrn);
}
