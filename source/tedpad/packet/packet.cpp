#include "..\..\..\include\tedpad\packet\packet.h"

void tedpad::ToNetworkPacket::add_module(PacketModule const & p0)
{
	vec_mod.push_back(p0);
}

void tedpad::ToNetworkPacket::add_module(std::vector<PacketModule> const & p0)
{
	std::copy(p0.begin(), p0.end(), std::back_inserter(vec_mod));
}

void tedpad::ToNetworkPacket::set_packetModules(std::vector<PacketModule> const & p0)
{
	vec_mod = p0;
}

size_t tedpad::ToNetworkPacket::get_packetSize() const
{
	size_t rtrn = 0;
	for (auto &&element : vec_mod) {
		rtrn += element.get_totalSize();
	}
	return(rtrn);
}

size_t tedpad::ToNetworkPacket::get_fullPacketSize() const
{
	return(get_packetSize() + PacketStructure::begin.size() + 1 + PacketStructure::end.size() + 1 + sizeof(uint32_t));
}

std::vector<uint8_t> tedpad::ToNetworkPacket::get_packetData() const
{
	std::vector<uint8_t> rtrn;
	rtrn.reserve(get_packetSize());
	for (auto &&element : vec_mod) {
		if (element.valid) {
			auto buf = element.to_netbuf();
			std::copy(buf.begin(), buf.end(), std::back_inserter(rtrn));
		}
	}
	return(rtrn);
}

std::vector<uint8_t> tedpad::ToNetworkPacket::get_fullPacketData() const
{
	std::vector<uint8_t> rtrn;
	rtrn.reserve(get_fullPacketSize());
	auto buf = get_packetData();
	std::copy(PacketStructure::begin.begin(), PacketStructure::begin.end(), std::back_inserter(rtrn));
	rtrn.push_back('\0');
	uint32_t size = htonl(static_cast<uint32_t>(get_packetSize()));
	std::copy_n(reinterpret_cast<uint8_t *>(&size), sizeof(uint32_t), std::back_inserter(rtrn));
	std::copy(buf.begin(), buf.end(), std::back_inserter(rtrn));
	std::copy(PacketStructure::end.begin(), PacketStructure::end.end(), std::back_inserter(rtrn));
	rtrn.push_back('\0');
	return(rtrn);
}

void tedpad::FromNetworkPacket::add_data(std::vector<uint8_t> const & p0)
{
	std::copy(p0.begin(), p0.end(), std::back_inserter(vec_data));
}

void tedpad::FromNetworkPacket::set_data(std::vector<uint8_t> const & p0)
{
	vec_data = p0;
}

size_t tedpad::FromNetworkPacket::get_packetSize() const
{
	//Note: This method must be used to avoid counting submodules
	size_t rtrn = 0;
	std::string const str_moduleBegin = ModuleStructure::begin + '\0';
	std::string const str_moduleEnd = ModuleStructure::end + '\0';
	auto last_itr = vec_data.begin();
	while (true) {
		//Search for the module begin string in the unsearched data (last_itr)
		last_itr = std::search(last_itr, vec_data.end(), str_moduleBegin.begin(), str_moduleEnd.end());
		//If there are no more begins, break
		if (last_itr == vec_data.end())
			break;
		//Get the size of the located module data
		uint32_t mod_size;
		std::copy_n((last_itr += str_moduleBegin.size()), sizeof(uint32_t), reinterpret_cast<uint8_t *>(&mod_size));
		mod_size = ntohl(mod_size);
		//Increment the iterator the where there should be the module end string (+= sizeof(uint32_t) for when the module size was read)
		last_itr += (sizeof(uint32_t) + mod_size);
		//If the ending string is found, increase the modules
		if (std::string(last_itr, last_itr + str_moduleEnd.size()) == str_moduleEnd)
			rtrn++;
		//Increase the iterator past the ending string
		last_itr += str_moduleEnd.size();
		//Break if the iterator is at the end
		if (last_itr == vec_data.end())
			break;
	}
	return(rtrn);
}

std::vector<tedpad::PacketModule> tedpad::FromNetworkPacket::get_packetModules() const
{
	//Note: This method must be used to avoid counting submodules
	std::vector<PacketModule> rtrn;
	std::string const str_moduleBegin = ModuleStructure::begin + '\0';
	std::string const str_moduleEnd = ModuleStructure::end + '\0';
	auto itr_lastEnd = vec_data.begin();
	while (true) {
		//Search for the module begin string in the unsearched data (itr_lastEnd onwards)
		auto itr_moduleBegin = std::search(itr_lastEnd, vec_data.end(), str_moduleBegin.begin(), str_moduleEnd.end());
		//If there are no more begins, break
		if (itr_moduleBegin >= vec_data.end())
			break;

		//Create an iterator that can be used for data parsing
		auto itr_moduleItr = itr_moduleBegin;

		//Get the size of the located module data
		uint32_t mod_size;
		std::copy_n((itr_moduleItr += str_moduleBegin.size()), sizeof(uint32_t), reinterpret_cast<uint8_t *>(&mod_size));
		mod_size = ntohl(mod_size);

		//Increment the iterator the where there should be the module end string (+= sizeof(uint32_t) for when the module size was read)
		itr_moduleItr += (sizeof(uint32_t) + mod_size);
		//Create a iterator to where the end of the module (including end string) should be
		auto itr_moduleEnd = itr_moduleItr + str_moduleEnd.size();
		//If the ending string is found, add the module to rtrn
		if (std::string(itr_moduleItr, itr_moduleEnd) == str_moduleEnd) {
			PacketModule mod;
			mod.from_netbuf(std::vector<uint8_t>(itr_moduleBegin, itr_moduleEnd));
			rtrn.push_back(mod);
		}

		//Break if the iterator is at the end of the data (or somehow over)
		if (itr_moduleEnd >= vec_data.end())
			break;

		//Set itr_lastEnd to the module end iterator
		itr_lastEnd = itr_moduleEnd;
	}
	return(rtrn);
}

std::vector<tedpad::PacketModule> tedpad::FromNetworkPacket::get_packetModulesCheck() const
{
	std::string const str_packetBegin = PacketStructure::begin + '\0';
	std::string const str_packetEnd = PacketStructure::end + '\0';
	auto itr = vec_data.begin() + str_packetBegin.size();
	//Check that the begin string is at the beginning
	if (std::string(vec_data.begin(), itr) != str_packetBegin)
		return(std::vector<PacketModule>());
	//Get the data size
	uint32_t packet_size;
	std::copy_n(itr, sizeof(uint32_t), reinterpret_cast<uint8_t *>(&packet_size));
	packet_size = ntohl(packet_size);
	itr += (sizeof(uint32_t) + packet_size);
	//Make sure that itr is still within the bounds of the data
	if (itr >= vec_data.end())
		return(std::vector<PacketModule>());
	//Check that the ending string is after the data
	if (std::string(itr, itr + str_packetEnd.size()) != str_packetEnd)
		return(std::vector<PacketModule>());
	return(get_packetModules());
}
