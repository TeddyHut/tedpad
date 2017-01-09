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
		{ eg::Descriptor<>::Descriptor({ { Key::Module, { Value::Module::GamepadFullDescription } } }), "GameFu" }
	};
}