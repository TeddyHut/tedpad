#include "../engine/include/eg/eg_engine.h"
#include "../include/tedpad/descriptionCentral.h"
#include "../include/tedpad/dataManipulator_file/file.h"
#include "../include/tedpad/modules/attribute.h"
#include "../include/tedpad/modules/gamepadDescription.h"
#include "../include/tedpad/modules/gamepadData.h"
#include "../include/tedpad/gamepad/gamepad.h"
#include "../include/tedpad/packet/packet.h"
#include "../include/tedpad/gamepad/filetype_tpd.h"
#include "../include/tedpad/server/server.h"

class Jack : public eg::Object {
protected:
	tedpad::Gamepad gamepad;
	tedpad::Server server;
public:
	void runover_init(void *rundata) override {
		eg::DataReference ref;
		ref.description[tedpad::Key::DataReference_Description] = tedpad::Value::DataReference_Description::File;
		std::string input = "../../doc/GamepadExample.tpd";
		std::hash<std::string> hash;
		ref.alteration = hash(input);
		ref.dataPointer = static_cast<void*>(&input);
		loadData(ref);
		gamepad.from_tpdFile(static_cast<uint8_t const *>(ref.dataPointer), ref.dataSize);
		freeData(ref);

		server.set_gamepad(&gamepad);
		server.config_broadcast(true);
		server.server_start();
	}

	void runover(void *rundata) override {
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
	while (true) {
		king.runover_fullObject();
	}
}

