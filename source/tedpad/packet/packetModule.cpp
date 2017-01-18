#include "../../../include/tedpad/packet/packetModule.h"

void tedpad::PacketModule::set_dataSize(size_t const p0) {
	data.resize(p0);
}

size_t tedpad::PacketModule::get_dataSize() const {
	return(data.size());
}

size_t tedpad::PacketModule::get_totalSize() const {
	return(ModuleStructure::begin.size() + 1 + name.size() + 1 + sizeof(uint32_t) + data.size() + ModuleStructure::end.size() + 1);
}

std::vector<uint8_t> tedpad::PacketModule::to_netbuf() const {
	//TODO: std::back_inserter
	std::vector<uint8_t> vec(get_totalSize(), 0);
	if (valid) {
		auto next_itr = std::copy(ModuleStructure::begin.cbegin(), ModuleStructure::begin.cend(), vec.begin());
		*(next_itr++) = '\0';
		next_itr = std::copy(name.cbegin(), name.cend(), next_itr);
		*(next_itr++) = '\0';
		uint32_t size = htonl(static_cast<uint32_t>(data.size()));
		next_itr = std::copy_n(reinterpret_cast<uint8_t *>(&size), sizeof(uint32_t), next_itr);
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
	if (std::string(next_itr, next_itr += ModuleStructure::begin.size()) != ModuleStructure::begin) {
		valid = false;
		name = ModuleStructure::invalid;
		return;
	}
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
	next_itr = nameEnd_itr;
	uint32_t size;
	std::copy(next_itr, next_itr += sizeof(uint32_t), reinterpret_cast<uint8_t *>(&size));
	set_dataSize(ntohl(size));
	std::copy(next_itr, next_itr += get_dataSize(), data.begin());
	if (std::string(next_itr, next_itr += ModuleStructure::end.size()) != ModuleStructure::end) {
		valid = false;
		name = ModuleStructure::invalid;
		return;
	}
	if (get_totalSize() != p0.size()) {
		valid = false;
		name = ModuleStructure::invalid;
		return;
	}
	valid = true;
}
