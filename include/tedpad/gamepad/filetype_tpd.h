#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <functional>

#include "../modules/gamepadDescription.h"

namespace tedpad {
	namespace File {
		class GamepadFullDescription_tpd {
		protected:
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
			static std::map<Token const, std::string const> const map_token;

			std::vector<Token> vec_token;
			std::vector<std::string> vec_str;
		};

		class TPD_to_GamepadFullDescription : public GamepadFullDescription_tpd {
		public:
			TPD_to_GamepadFullDescription(uint8_t const *const ptr, size_t const len);
			//TODO: Maybe an rvalue version for this?
			operator tedpad::Module::GamepadFullDescription() const;
		private:
			Token str_to_token(std::string const &p0) const;
			void generate_tokens(uint8_t const *const ptr, size_t const len);
			void generate_gamepadFullDescription();

			Module::GamepadFullDescription value;
		};
	}
}
