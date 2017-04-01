#include "../../include/tedpad/user.h"

void tedpad::ServerInfo::fillFromServerDescription(Module::ServerDescription const & serverDescription)
{
	ip = serverDescription.ip;
	port = serverDescription.port;
	numberOfClients = serverDescription.number_clientsConnected;
}
