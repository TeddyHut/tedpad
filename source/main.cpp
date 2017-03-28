#include "../engine/include/eg/eg_engine.h"
#include "../include/tedpad/descriptionCentral.h"
#include "../include/tedpad/dataManipulator_file/file.h"
#include "../include/tedpad/modules/attribute.h"
#include "../include/tedpad/modules/gamepadDescription.h"
#include "../include/tedpad/modules/gamepadData.h"
#include "../include/tedpad/gamepad/gamepad.h"
#include "../include/tedpad/packet/packet.h"
#include "../include/tedpad/gamepad/filetype_tpd.h"

class Jack : public eg::Object {
public:
	void runover_init(void *rundata) override {
		tedpad::Gamepad gamepad("TestPad");
		/*
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Digital, tedpad::Gamepad::AttributeDirection::Set, "A");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Digital, tedpad::Gamepad::AttributeDirection::Set, "B");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Digital, tedpad::Gamepad::AttributeDirection::Set, "X");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Digital, tedpad::Gamepad::AttributeDirection::Set, "Y");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Digital, tedpad::Gamepad::AttributeDirection::Get, "Up");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Digital, tedpad::Gamepad::AttributeDirection::Get, "Down");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Digital, tedpad::Gamepad::AttributeDirection::Get, "Left");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Digital, tedpad::Gamepad::AttributeDirection::Get, "Right");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Analogue, tedpad::Gamepad::AttributeDirection::Set, "A");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Analogue, tedpad::Gamepad::AttributeDirection::Set, "B");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Analogue, tedpad::Gamepad::AttributeDirection::Set, "X");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Analogue, tedpad::Gamepad::AttributeDirection::Set, "Y");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Analogue, tedpad::Gamepad::AttributeDirection::Get, "Up");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Analogue, tedpad::Gamepad::AttributeDirection::Get, "Down");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Analogue, tedpad::Gamepad::AttributeDirection::Get, "Left");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Analogue, tedpad::Gamepad::AttributeDirection::Get, "Right");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Axis, tedpad::Gamepad::AttributeDirection::Set, "A");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Axis, tedpad::Gamepad::AttributeDirection::Set, "B");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Axis, tedpad::Gamepad::AttributeDirection::Set, "X");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Axis, tedpad::Gamepad::AttributeDirection::Set, "Y");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Axis, tedpad::Gamepad::AttributeDirection::Get, "Up");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Axis, tedpad::Gamepad::AttributeDirection::Get, "Down");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Axis, tedpad::Gamepad::AttributeDirection::Get, "Left");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Axis, tedpad::Gamepad::AttributeDirection::Get, "Right");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Buffer, tedpad::Gamepad::AttributeDirection::Set, "A");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Buffer, tedpad::Gamepad::AttributeDirection::Set, "B");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Buffer, tedpad::Gamepad::AttributeDirection::Set, "X");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Buffer, tedpad::Gamepad::AttributeDirection::Set, "Y");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Buffer, tedpad::Gamepad::AttributeDirection::Get, "Up");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Buffer, tedpad::Gamepad::AttributeDirection::Get, "Down");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Buffer, tedpad::Gamepad::AttributeDirection::Get, "Left");
		gamepad.add_newAttribute(tedpad::Gamepad::AttributeType::Buffer, tedpad::Gamepad::AttributeDirection::Get, "Right");
		auto description = gamepad.to_gamepadFullDescription();
		*/

		eg::DataReference ref;
		ref.description[tedpad::Key::DataReference_Description] = tedpad::Value::DataReference_Description::File;
		std::string input = "../../doc/GamepadExample.tpd";
		std::hash<std::string> hash;
		ref.alteration = hash(input);
		ref.dataPointer = static_cast<void*>(&input);
		loadData(ref);
		tedpad::Module::GamepadFullDescription description = tedpad::File::TPD_to_GamepadFullDescription(static_cast<uint8_t const *>(ref.dataPointer), ref.dataSize);
		freeData(ref);

		tedpad::ToNetworkPacket packet;
		packet.add_module(description.to_packetModule());
		auto data = packet.get_fullPacketData();

		eg::DataReference dataRef;
		dataRef.description[tedpad::Key::DataReference_Description] = tedpad::Value::DataReference_Description::File;
		std::string fileOut = "../../doc/output.bin";
		dataRef.alteration = reinterpret_cast<size_t>(&fileOut);
		dataRef.dataPointer = data.data();
		dataRef.dataSize = data.size();
		writeData(dataRef);
	}
};

class King : public eg::FullObject {
public:
	void runover_init(void *rundata) override {
		dataFile.set_boundFullObject(this);
		jack.set_boundFullObject(this);
	}
	Jack jack;
	tedpad::DataManipulator::File dataFile;
};

int main() {
	King king;
	for (size_t i = 0; i < 4; i++) {
		king.runover_fullObject();
	}
}

