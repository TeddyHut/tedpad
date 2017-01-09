#include "../engine/include/eg/eg_engine.h"
#include "../include/tedpad/descriptionCentral.h"
#include "../include/tedpad/dataManipulator_file/file.h"
#include "../include/tedpad/modules/attribute.h"
#include "../include/tedpad/modules/gamepadDescription.h"
#include "../include/tedpad/packet/packet.h"

class Jack : public eg::Object {
public:
	void runover_init(void *rundata) override {
		tedpad::ToNetworkPacket packet;
		tedpad::Module::GamepadFullDescription desc;
		desc.briefDescription.gamepadName = "Dualshock2";
		desc.briefDescription.out_digitalCount = 0xa;
		desc.briefDescription.out_analogueCount = 0xb;
		desc.briefDescription.out_axisCount = 0xc;
		desc.briefDescription.out_bufferCount = 0xd;

		packet.add_module(desc.to_packetModule());
		auto data = packet.get_fullPacketData();

		eg::DataReference dataRef;
		dataRef.description[tedpad::Key::DataReference_Description] = tedpad::Value::DataReference_Description::File;
		std::string fileOut = "output.bin";
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

