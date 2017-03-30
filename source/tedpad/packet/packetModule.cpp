#include "../../../include/tedpad/packet/packetModule.h"

void tedpad::PacketModule::set_dataSize(uint16_t const p0) {
	data.resize(p0);
}

uint16_t tedpad::PacketModule::get_dataSize() const {
	return(static_cast<uint16_t>(data.size()));
}

uint16_t tedpad::PacketModule::get_totalSize() const {
	return(static_cast<uint16_t>(ModuleStructure::begin.size() + 1 + name.size() + 1 + sizeof(uint16_t) + data.size() + ModuleStructure::end.size() + 1));
}

std::vector<uint8_t> tedpad::PacketModule::to_netbuf() const {
	//TODO: std::back_inserter
	std::vector<uint8_t> vec(get_totalSize(), 0);
	if (valid) {
		auto next_itr = std::copy(ModuleStructure::begin.cbegin(), ModuleStructure::begin.cend(), vec.begin());
		*(next_itr++) = '\0';
		next_itr = std::copy(name.cbegin(), name.cend(), next_itr);
		*(next_itr++) = '\0';
		uint16_t size = htons(static_cast<uint16_t>(data.size()));
		next_itr = std::copy_n(reinterpret_cast<uint8_t *>(&size), sizeof(size), next_itr);
		next_itr = std::copy(data.cbegin(), data.cend(), next_itr);
		next_itr = std::copy(ModuleStructure::end.cbegin(), ModuleStructure::end.cend(), next_itr);
		*next_itr = '\0';
	}
	else
		vec.clear();
	return(vec);
}

void tedpad::PacketModule::from_netbuf(std::vector<uint8_t> const &p0) {
	auto next_itr = p0.cbegin();
	if (std::string(next_itr, next_itr + ModuleStructure::begin.size()) != ModuleStructure::begin) {
		valid = false;
		name = ModuleStructure::invalid;
		return;
	}
	next_itr += ModuleStructure::begin.size();
	if (*next_itr != '\0') {
		valid = false;
		name = ModuleStructure::invalid;
		return;
	}
	next_itr++;
	auto nameEnd_itr = std::find(next_itr, p0.cend(), '\0');
	if ((nameEnd_itr == p0.end()) || (nameEnd_itr == std::find_end(next_itr, p0.cend(), nameEnd_itr, nameEnd_itr + 1))) {
		valid = false;
		return;
	}
	name = std::string(next_itr, nameEnd_itr);
	//Add 1 for the string termination
	next_itr = nameEnd_itr + 1;
	uint16_t size;
	std::copy_n(next_itr, sizeof(size), reinterpret_cast<uint8_t *>(&size));
	next_itr += sizeof(size);
	set_dataSize(ntohs(size));
	std::copy_n(next_itr, get_dataSize(), data.begin());
	next_itr += get_dataSize();
	if (std::string(next_itr, next_itr + ModuleStructure::end.size()) != ModuleStructure::end) {
		valid = false;
		name = ModuleStructure::invalid;
		return;
	}
	next_itr += ModuleStructure::end.size();
	if (get_totalSize() != p0.size()) {
		valid = false;
		name = ModuleStructure::invalid;
		return;
	}
	valid = true;
}
