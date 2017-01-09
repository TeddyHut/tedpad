#pragma once

#include <vector>
#include <algorithm>
#include <string>
#include <iterator>

#include "../socket.h"
#include "../definition.h"
#include "packetModule.h"

namespace tedpad {
	class ToNetworkPacket {
	public:
		//Add a module to the end of the packet
		void add_module(PacketModule const &p0);
		//Add modules to the end of the packet
		void add_module(std::vector<PacketModule> const &p0);
		//Sets the modules for the packet
		void set_packetModules(std::vector<PacketModule> const &p0);

		//Gets the size of the packet (in bytes), excluding the header and the footer
		size_t get_packetSize() const;
		//Gets the size of the packet (in bytes), including the header and the footer
		size_t get_fullPacketSize() const;
		//Gets the data for the packet, excluding the header and the footer
		std::vector<uint8_t> get_packetData() const;
		//Gets the data for the packet, including the header and the footer
		std::vector<uint8_t> get_fullPacketData() const;
	private:
		std::vector<PacketModule> vec_mod;
	};

	class FromNetworkPacket {
	public:
		//Add data to the packet
		void add_data(std::vector<uint8_t> const &p0);
		//Set the packet data
		void set_data(std::vector<uint8_t> const &p0);

		//Gets the size of the packet (in modules)
		size_t get_packetSize() const;
		//Gets the packet modules, excluding the check for header and footer
		std::vector<PacketModule> get_packetModules() const;
		//Gets the packet modules, including the check for the header and footer
		std::vector<PacketModule> get_packetModulesCheck() const;
	private:
		std::vector<uint8_t> vec_data;
	};
}
