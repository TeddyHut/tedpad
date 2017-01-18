#pragma once

#include <string>

namespace tedpad {
	namespace ModuleStructure {
		//Seems like C++11 isn't setup to deal with constexpr strings
		extern std::string const begin;
		extern std::string const end;
		extern std::string const invalid;
	}
	namespace PacketStructure {
		extern std::string const begin;
		extern std::string const end;
	}
}

//Idea: Create a base class with an attribute vector containing a bunch of handy attribute functions (eg making new ones, counting types, etc)
