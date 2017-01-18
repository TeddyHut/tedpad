#pragma once

#include <map>
#include <string>
#include "../../engine/include/eg/eg_engine.h"

#include "descriptionCentral.h"

namespace tedpad {
	namespace Module {
		namespace Name {
			extern std::map<eg::Descriptor<> const, std::string const> const nameMap;
			eg::Descriptor<> get_description(std::string const &p0);
		}
	}
}
