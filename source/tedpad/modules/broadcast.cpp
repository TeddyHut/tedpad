#include "..\..\..\include\tedpad\modules\broadcast.h"

tedpad::PacketModule tedpad::Module::ServerDescription::to_packetModule() const
{
	PacketModule rtrn;
	rtrn.name = get_description();
	uint32_t net_ip = htonl(ip);
	std::copy_n(reinterpret_cast<uint8_t const *>(&net_ip), sizeof(net_ip), std::back_inserter(rtrn.data));
	uint16_t net_port = htons(port);
	std::copy_n(reinterpret_cast<uint8_t const *>(&net_port), sizeof(net_port), std::back_inserter(rtrn.data));
	uint16_t net_clientsConnected = htons(number_clientsConnected);
	std::copy_n(reinterpret_cast<uint8_t const *>(&net_clientsConnected), sizeof(net_clientsConnected), std::back_inserter(rtrn.data));
	rtrn.valid = true;
	return(rtrn);
}

void tedpad::Module::ServerDescription::from_packetModule(PacketModule const & p0)
{
	set_description(p0.name);
	if (!(description[Key::Module] & Value::Module::ServerDescription)) {
		description[Key::Module] = Value::Module::Invalid;
		return;
	}
	uint32_t net_ip;
	uint16_t net_port;
	uint16_t net_clientsConnected;
	auto dataItr = p0.data.begin();
	std::copy_n(dataItr, sizeof(net_ip), reinterpret_cast<uint8_t *>(&net_ip));
	dataItr += sizeof(net_ip);
	std::copy_n(dataItr, sizeof(net_port), reinterpret_cast<uint8_t *>(&net_port));
	dataItr += sizeof(net_port);
	std::copy_n(dataItr, sizeof(net_clientsConnected), reinterpret_cast<uint8_t *>(&net_clientsConnected));
	dataItr += sizeof(net_clientsConnected);
	ip = ntohl(net_ip);
	port = ntohs(net_port);
	number_clientsConnected = ntohs(net_clientsConnected);
}

tedpad::Module::ServerDescription::ServerDescription()
{
	description[Key::Module] = Value::Module::ServerDescription;
}
