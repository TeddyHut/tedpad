#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <functional>

#include "../modules/gamepadDescription.h"
#include "../../../engine/include/eg/eg_engine.h"

namespace tedpad {
	namespace File {
		namespace TPD_to_GamepadFullDescription_dec {
			enum class Token {
				Name,
				Type,
				Direction,
				Assign,
				AttributeBegin,
				AttributeEnd,
				Quotation,
				StrPlaceholder,
				Invalid,
			};
			extern std::map<Token const, std::string const> const map_token;
			
			class TPD_to_GamepadFullDescription : public eg::util::TextParse<Token, map_token> {
			public:
				TPD_to_GamepadFullDescription(std::string const &text);
				//TODO: Maybe an rvalue version for this?
				operator tedpad::Module::GamepadFullDescription() const;
			private:
				void generate_gamepadFullDescription();
				Module::GamepadFullDescription value;
			};
		}
		using TPD_to_GamepadFullDescription = TPD_to_GamepadFullDescription_dec::TPD_to_GamepadFullDescription;
	}
}
