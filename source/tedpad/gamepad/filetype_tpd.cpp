#include "../../../include/tedpad/gamepad/filetype_tpd.h"

std::map<tedpad::File::GamepadFullDescription_tpd::Token const, std::string const> const tedpad::File::GamepadFullDescription_tpd::map_token {
	{ Token::Name, "name" },
	{ Token::Type, "type" },
	{ Token::Direction, "direction" },
	{ Token::Assign, "=" },
	{ Token::AttributeBegin, "attribute_begin" },
	{ Token::AttributeEnd, "attribute_end" },
	{ Token::Quotation, "\""},
};

tedpad::File::TPD_to_GamepadFullDescription::TPD_to_GamepadFullDescription(uint8_t const * const ptr, size_t const len)
{
	generate_tokens(ptr, len);
	generate_gamepadFullDescription();
}

tedpad::File::TPD_to_GamepadFullDescription::operator tedpad::Module::GamepadFullDescription() const
{
	return(value);
}

tedpad::File::GamepadFullDescription_tpd::Token tedpad::File::TPD_to_GamepadFullDescription::str_to_token(std::string const & p0) const
{
	auto itr = std::find_if(map_token.begin(), map_token.end(), [&](std::pair<Token const, std::string const> const &p1) { return(p1.second == p0); });
	if (itr == map_token.end())
		return(Token::Invalid);
	return((*itr).first);
}

//Note: There is probably a glitch where a quote with a leading space " xx" will not register the leading space. There might also be a glitch where a proceeding whitespace isn't registered "xx "
void tedpad::File::TPD_to_GamepadFullDescription::generate_tokens(uint8_t const * const ptr, size_t const len)
{
	vec_token.clear();
	vec_str.clear();
	std::stringstream content(std::string(reinterpret_cast<char const *>(ptr), len));
	while (!content.eof()) {
		std::string segment;
		content >> segment;
		size_t quoteBegin_pos = segment.find(map_token.at(Token::Quotation));
		if (quoteBegin_pos != std::string::npos) {
			std::string quote_str = segment;
			size_t quoteEnd_pos = quote_str.rfind(map_token.at(Token::Quotation));
			if (quoteEnd_pos != quoteBegin_pos) {
				quote_str = quote_str.substr(quoteBegin_pos + 1, quoteEnd_pos - quoteBegin_pos - 1);
			}
			else {
				quote_str = quote_str.substr(quoteBegin_pos + 1);
				std::getline(content, segment, map_token.at(Token::Quotation).at(0));
				quote_str += segment;
			}
			vec_token.push_back(Token::Quotation);
			vec_token.push_back(Token::StrPlaceholder);
			vec_token.push_back(Token::Quotation);
			vec_str.push_back(quote_str);
		}
		else {
			auto token = str_to_token(segment);
			if (token == Token::Invalid) {
				vec_str.push_back(segment);
				token = Token::StrPlaceholder;
			}
			vec_token.push_back(token);
		}
	}
}

void tedpad::File::TPD_to_GamepadFullDescription::generate_gamepadFullDescription()
{
	//The current scope of the values (eg, could be inside an attribute with "attribute_begin")
	enum class Scope : uint8_t {
		Global,
		Attribute,
	} current_scope = Scope::Global;

	enum class QuotationState : uint8_t {
		Open,
		Closed,
	} current_quotationState = QuotationState::Closed;

	enum class Instruction : uint8_t {
		None,
		Assign,
	} last_instruction = Instruction::None;

	std::function<void(std::string const &)> assign_gamepadName = [&](std::string const &p0) {
		value.briefDescription.gamepadName = p0;
	};

	std::function<void(std::string const &)> assign_attributeName = [&](std::string const &p0) {
		(*(value.attribute.end() - 1)).attributeName = p0;
	};

	std::function<void(std::string const &)> assign_attributeType = [&](std::string const &p0) {
		std::map<std::string const, Module::Attribute::PlaceHolderType const> const map_type {
			{ "digital", Module::Attribute::PlaceHolderType::Digital },
			{ "analogue", Module::Attribute::PlaceHolderType::Analogue },
			{ "axis", Module::Attribute::PlaceHolderType::Axis },
			{ "buffer", Module::Attribute::PlaceHolderType::Buffer },
		};
		(*(value.attribute.end() - 1)).value = map_type.at(p0);
	};

	std::function<void(std::string const &)> assign_attributeDirection = [&](std::string const &p0) {
		std::map<std::string const, Module::Attribute::DataDirection const> map_direction {
			{ "out", Module::Attribute::DataDirection::ServerOutput},
			{ "in", Module::Attribute::DataDirection::ServerInput}
		};
		(*(value.attribute.end() - 1)).direction = map_direction.at(p0);
	};

	std::function<void(std::string const &)> *assign_function = nullptr;
	auto str_itr = vec_str.begin();
	std::string quote_string;

	auto assign_quoteStr = [&]() {
		(*assign_function)(quote_string);
		quote_string.clear();
	};

	auto invert_quotation = [&]() {
		if (current_quotationState == QuotationState::Open) {
			if (last_instruction == Instruction::Assign) {
				assign_quoteStr();
				last_instruction = Instruction::None;
				current_quotationState = QuotationState::Closed;
			}
			else
				current_quotationState = QuotationState::Open;
		}
	};

	for (auto &&token : vec_token) {
		switch (token) {
		case Token::Name:
			if (current_scope == Scope::Global)
				assign_function = &assign_gamepadName;
			else if (current_scope == Scope::Attribute)
				assign_function = &assign_attributeName;
			break;
		case Token::Type:
			if (current_scope == Scope::Attribute)
				assign_function = &assign_attributeType;
			break;
		case Token::Direction:
			if (current_scope == Scope::Attribute)
				assign_function = &assign_attributeDirection;
			break;
		case Token::Assign:
			last_instruction = Instruction::Assign;
			break;
		case Token::Quotation:
			invert_quotation();
			break;
		case Token::AttributeBegin:
		{
			Module::Attribute::PlaceHolder attr;
			value.attribute.push_back(attr);
			current_scope = Scope::Attribute;
		}
			break;
		case Token::AttributeEnd:
			current_scope = Scope::Global;
			break;
		case Token::StrPlaceholder:
			quote_string += *str_itr;
			str_itr++;
			if (current_quotationState == QuotationState::Closed) {
				if (last_instruction == Instruction::Assign) {
					assign_quoteStr();
					last_instruction = Instruction::None;
				}
			}
			break;
		default:
			break;
		}
	}
	value.update_briefDesciptionCount();
}
