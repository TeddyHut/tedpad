#pragma once

#include <vector>
#include <string>
#include "../../../engine/include/eg/eg_engine.h"

#include "../modules/attribute.h"
#include "../modules/gamepadDescription.h"

namespace tedpad {
	class Gamepad {
		struct p_AttributeMatch {
			p_AttributeMatch(eg::Descriptor<> const description, Module::Attribute::DataDirection const direction, std::string const &name);
			bool operator() (Module::Attribute::Generic const *const p0) const;
			eg::Descriptor<> attr_description;
			Module::Attribute::DataDirection attr_direction;
			std::string attr_name;
		};
	public:
		enum class IODirection : uint8_t {
			Server,
			Client,
		};
		//Get the data of the specified attribute. Returns 0 for success.
		//Get a digital button attribute
		bool Get_attribute(std::string const &attribute, bool &out) const;
		//Get an analogue button attribute
		bool Get_attribute(std::string const &attribute, uint8_t &out) const;
		//Get an axis attribute
		bool Get_attribute(std::string const &attribute, int16_t &out) const;
		//Get a buffer attribute
		bool Get_attribute(std::string const &attribute, std::vector<uint8_t> &out) const;
		//Set the data of the specified attribute. Returns 0 for success.
		//Set a digital button attribute
		bool Set_attribute(std::string const &attribute, bool const in);
		//Set an analogue button attribute	
		bool Set_attribute(std::string const &attribute, uint8_t const in);
		//Set an axis attribute
		bool Set_attribute(std::string const &attribute, int16_t const in);
		//Set a buffer attribute
		bool Set_attribute(std::string const &attribute, std::vector<uint8_t> const &in);

		//Allocate attributes and set name to align with p0
		void from_gamepadFullDescription(Module::GamepadFullDescription const &p0);
		Module::GamepadBriefDescription to_gamepadBriefDescription() const;
		Module::GamepadFullDescription to_gamepadFullDescription() const;

		void set_IODirection(IODirection const p0);

		void add_attribute(Module::Attribute::Generic const *const p0);

		~Gamepad();
	private:
		Module::Attribute::DataDirection get_dataDirection(bool const getting) const;
		size_t get_attributeServerOutCount(eg::Descriptor<> const attr_desc) const;
		size_t get_attributeServerInCount(eg::Descriptor<> const attr_desc) const;

		IODirection ioDirection = IODirection::Server;
		std::string gamepadName;
		std::vector<Module::Attribute::Generic *> vec_attribute;
	};
}
