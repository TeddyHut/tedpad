#include "../../../include/tedpad/gamepad/filetype_tpd.h"

std::map<tedpad::File::TPD_to_GamepadFullDescription_dec::Token const, std::string const> const tedpad::File::TPD_to_GamepadFullDescription_dec::map_token {
	{ tedpad::File::TPD_to_GamepadFullDescription_dec::Token::Name, "name" },
	{ tedpad::File::TPD_to_GamepadFullDescription_dec::Token::Type, "type" },
	{ tedpad::File::TPD_to_GamepadFullDescription_dec::Token::Direction, "direction" },
	{ tedpad::File::TPD_to_GamepadFullDescription_dec::Token::Assign, "=" },
	{ tedpad::File::TPD_to_GamepadFullDescription_dec::Token::AttributeBegin, "attribute_begin" },
	{ tedpad::File::TPD_to_GamepadFullDescription_dec::Token::AttributeEnd, "attribute_end" },
	{ tedpad::File::TPD_to_GamepadFullDescription_dec::Token::Quotation, "\""},
};

tedpad::File::TPD_to_GamepadFullDescription_dec::TPD_to_GamepadFullDescription::TPD_to_GamepadFullDescription(std::string const &text)
{
	generate_tokens(text);
	generate_gamepadFullDescription();
}

tedpad::File::TPD_to_GamepadFullDescription_dec::TPD_to_GamepadFullDescription::operator tedpad::Module::GamepadFullDescription() const
{
	return(value);
}

void tedpad::File::TPD_to_GamepadFullDescription_dec::TPD_to_GamepadFullDescription::generate_gamepadFullDescription()
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
