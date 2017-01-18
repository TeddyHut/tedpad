#pragma once

#include <vector>
#include <string>

#include "moduleBase.h"
#include "attribute.h"
#include "../packet/packet.h"

//TOOD: Determine whether there is a way to call the base class copy/move constructor/assignment so that description can be taken care of in base class

namespace tedpad {
	namespace Module {
		//Structure: Module( dataDirection (1) -> gamepadName (?) -> Packet( attribute_modules ) )
		struct GamepadData : public ModuleBase {
			PacketModule to_packetModule() const override;
			void from_packetModule(PacketModule const &p0) override;

			void add_attribute(Attribute::Generic const * const p0);
			void add_attribute(Attribute::Digital const &p0);
			void add_attribute(Attribute::Analogue const &p0);
			void add_attribute(Attribute::Axis const &p0);
			void add_attribute(Attribute::Buffer const &p0);

			std::string gamepadName;
			std::vector<Attribute::Generic *> vec_attribute;
			Attribute::DataDirection direction;

			GamepadData &operator=(GamepadData const &p0);
			GamepadData &operator=(GamepadData &&p0);
			GamepadData(GamepadData const &p0);
			GamepadData(GamepadData &&p0);

			GamepadData();
			~GamepadData();
		private:
			void op_assign(GamepadData const &p0);
			void op_assign(GamepadData &&p0);
		};
	}
}