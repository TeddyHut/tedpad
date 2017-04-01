#include "../../../include/tedpad/client/client.h"

std::vector<tedpad::ServerInfo> tedpad::Client::scanForTime(ScanForTimeArgs const &args)
{
	if (args.manageSocketService)
		socket_service::startup();

	addrinfo hints;
	addrinfo *ppResult;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	std::stringstream ss_port;
	ss_port << args.port;

	int result = getaddrinfo(NULL, ss_port.str().c_str(), &hints, &ppResult);
	if (result != 0) {
		std::cout << "tedpad::Client::scanForTime(): getaddrinfo error: " << gai_strerror(result) << std::endl;
		exit(1);
	}

	SOCKET v_socket = socket(ppResult->ai_family, ppResult->ai_socktype, ppResult->ai_protocol);
	if (v_socket == INVALID_SOCKET) {
		std::cout << "tedpad::Client::scanForTime(): socket error" << std::endl;
		exit(1);
	}

	result = bind(v_socket, ppResult->ai_addr, static_cast<int>(ppResult->ai_addrlen));
	if (result == SOCKET_ERROR) {
		std::cout << "tedpad::Client::scanForTime(): bind error" << std::endl;
		exit(1);
	}

	freeaddrinfo(ppResult);

	if (socket_service::socket_setBlocking(v_socket, false)) {
		std::cout << "tedpad::Client::scanForTime(): socket_setBlocking error" << std::endl;
		exit(1);
	}

	std::vector<ServerInfo> rtrn;
	std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();
	while (std::chrono::steady_clock::now() < (start_time + args.scanTime)) {
	//for(size_t i = 0; i < 10; i++) {
		//Collect packets as long as they are there in recvfrom
		while (true) {
			std::vector<uint8_t> vec_data(2048);
			int recv_len = recvfrom(v_socket, reinterpret_cast<char *>(vec_data.data()), static_cast<int>(vec_data.size()), 0, nullptr, nullptr);
			if (recv_len > 0) {
				vec_data.resize(recv_len);
				//Make the data a packet
				FromNetworkPacket packet;
				packet.set_data(vec_data);
				//Get the packetmodules
				auto packetModules = packet.get_packetModulesCheck();
				//If the packet was valid
				if (packetModules.size() > 1) {
					Module::ServerDescription serverDescription;
					Module::GamepadBriefDescription gamepadDescription;
					serverDescription.from_packetModule(packetModules.at(0));
					//If the server hasn't already been found, add it to rtrn
					if (std::find_if(rtrn.begin(), rtrn.end(), [&](ServerInfo const &p0) { return((p0.ip == serverDescription.ip) && (p0.port == serverDescription.port)); }) == rtrn.end()) {
						gamepadDescription.from_packetModule(packetModules.at(1));
						rtrn.push_back(ServerInfo{ gamepadDescription, serverDescription.ip, serverDescription.port });
					}
				}
			}
			else if (recv_len == 0) {
				break;
			}
			else if ((socket_service::get_lastError() == socket_service::ERROR_EWOULDBLOCK) || (socket_service::get_lastError() == socket_service::ERROR_WAGAIN)) {
				break;
			}
			else {
				std::cout << "tedpad::Client::scanForTime(): recvfrom error" << std::endl;
				exit(1);
			}
		}
		//Sleep until the next update
		std::this_thread::sleep_for(args.updateRate);
	}

	closesocket(v_socket);

	if (args.manageSocketService)
		socket_service::shutdown();

	return(rtrn);
}

std::vector<tedpad::ServerInfo> tedpad::Client::filterScanResults(std::vector<ServerInfo> scanResults, FilterScanResultsArgs const & args)
{
	auto removeIf_p = [&](ServerInfo const &p0) {
		return(!(
			((args.filter & args.IP) ? (p0.ip == args.ip) : true) &&
			((args.filter & args.PORT) ? (p0.port == args.port) : true) &&
			((args.filter & args.NUMBEROFCLIENTS) ? (p0.numberOfClients == args.numberOfClients) : true) &&
			((args.filter & args.GAMEPADNAME) ? (p0.gamepadDescription.gamepadName == args.gamepadName) : true)));
	};
	scanResults.erase(std::remove_if(scanResults.begin(), scanResults.end(), removeIf_p), scanResults.end());
	return(scanResults);
}

tedpad::Module::GamepadFullDescription tedpad::Client::get_serverFullGamepadDescription(uint32_t const ip, uint16_t const port, bool const manageSocketService)
{
	return(get_serverFullGamepadDescription(ip_to_str(ip), port, manageSocketService));
}

tedpad::Module::GamepadFullDescription tedpad::Client::get_serverFullGamepadDescription(std::string const & ip, uint16_t const port, bool const manageSocketService)
{
	if (manageSocketService)
		socket_service::startup();

	SOCKET v_socket = get_connectedSocket(ip, port);
	Module::GamepadFullDescription rtrn = serverRequest_Receive_GamepadFullDescription(v_socket);
	closesocket(v_socket);

	if (manageSocketService)
		socket_service::shutdown();
	
	return(rtrn);
}

void tedpad::Client::connectToServer(uint32_t const ip, uint16_t const port)
{
	connectToServer(ip_to_str(ip), port);
}

void tedpad::Client::connectToServer(std::string const & ip, uint16_t const port)
{
	pm_socket = get_connectedSocket(ip, port);
	gamepad.from_gamepadFullDescription(serverRequest_Receive_GamepadFullDescription(pm_socket));
	pm_connected = true;
}

void tedpad::Client::disconnect()
{
	if (pm_connected) {
		closesocket(pm_socket);
		pm_socket = INVALID_SOCKET;
		pm_connected = false;
	}
}

void tedpad::Client::gamepadUpdate()
{
	if (pm_connected) {
		gamepad.set_gamepadData_dataDirection(serverRequest_Receive_GamepadData_DirectionOut(pm_socket));
		serverRequest_Send_GamepadData_DirecitonIn(pm_socket, gamepad.get_gamepadData_dataDirection(Module::Attribute::DataDirection::ServerInput));
	}
}

tedpad::ServerInfo tedpad::Client::get_connectedServerInfo()
{
	//TODO: Add an exception here that will throw if the client is not connected to a server
	ServerInfo rtrn;
	if (pm_connected) {
		Module::ServerDescription mod_serverDescription;
		mod_serverDescription.from_packetModule(server_sendRecv(pm_socket, Module::Communication::Request_e::Receive_ServerDescription, Module::Communication::Reply_e::Send_ServerDescription));
		rtrn.fillFromServerDescription(mod_serverDescription);
		rtrn.gamepadDescription = gamepad.to_gamepadBriefDescription();
	}
	return(rtrn);
}

tedpad::Client::Client(bool const start_socket_service) : pm_startedSocketService(start_socket_service)
{
	if (start_socket_service)
		socket_service::startup();
	gamepad.set_IODirection(Gamepad::IODirection::Client);
}

tedpad::Client::~Client()
{
	if (pm_connected)
		closesocket(pm_socket);
	if (pm_startedSocketService)
		socket_service::shutdown();
}

SOCKET tedpad::Client::get_connectedSocket(std::string const & ip, uint16_t const port)
{
	addrinfo hints;
	addrinfo *ppResult;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	std::stringstream ss_port;
	ss_port << port;

	int result = getaddrinfo(ip.c_str(), ss_port.str().c_str(), &hints, &ppResult);
	if (result != 0) {
		std::cout << "tedpad::Client::get_connectedSocket(): getaddrinfo error: " << gai_strerror(result) << std::endl;
		exit(1);
	}

	SOCKET v_socket = socket(ppResult->ai_family, ppResult->ai_socktype, ppResult->ai_protocol);
	if (v_socket == INVALID_SOCKET) {
		std::cout << "tedpad::Client::get_connectedSocket(): socket error" << std::endl;
		exit(1);
	}

	result = connect(v_socket, ppResult->ai_addr, static_cast<int>(ppResult->ai_addrlen));
	if (result == SOCKET_ERROR) {
		std::cout << "tedpad::Client::get_connectedSocket(): connect error" << std::endl;
		exit(1);
	}

	freeaddrinfo(ppResult);

	return(v_socket);
}

tedpad::Module::GamepadFullDescription tedpad::Client::serverRequest_Receive_GamepadFullDescription(SOCKET const & v_socket)
{
	Module::GamepadFullDescription rtrn;
	rtrn.from_packetModule(server_sendRecv(v_socket, Module::Communication::Request_e::Receive_GamepadFullDescription, Module::Communication::Reply_e::Send_GamepadFullDescription));
	return(rtrn);
}

tedpad::Module::GamepadData tedpad::Client::serverRequest_Receive_GamepadData_DirectionOut(SOCKET const & v_socket)
{
	Module::GamepadData rtrn;
	rtrn.from_packetModule(server_sendRecv(v_socket, Module::Communication::Request_e::Receive_GamepadData_DirectionOut, Module::Communication::Reply_e::Send_GamepadData_DirectionOut));
	return(rtrn);
}

void tedpad::Client::serverRequest_Send_GamepadData_DirecitonIn(SOCKET const & v_socket, Module::GamepadData const & gamepadData)
{
	server_sendRecv(v_socket, Module::Communication::Request_e::Send_GamepadData_DirectionIn, Module::Communication::Reply_e::Receive_GamepadData_DirectionIn, gamepadData.to_packetModule());
}

tedpad::PacketModule tedpad::Client::server_sendRecv(SOCKET const & v_socket, Module::Communication::Request_e const request, Module::Communication::Reply_e const reply, PacketModule const sendmodule)
{
	//The packetmodule to be returned
	PacketModule rtrn;

	//Create the send packet
	ToNetworkPacket toPacket;
	//Add the request
	Module::Communication::Request mod_request;
	mod_request.request = request;
	toPacket.add_module(mod_request);
	
	//If there is an extra packet to send with the request, add it on
	if (!(Module::Name::get_description(sendmodule.name)[Key::Module] & Value::Module::Invalid)) {
		toPacket.add_module(sendmodule);
	}

	//Send and check for errors
	int result = send(v_socket, reinterpret_cast<char const *>(toPacket.get_fullPacketData().data()), static_cast<int>(toPacket.get_fullPacketSize()), 0);
	if (result == SOCKET_ERROR) {
		std::cout << "tedpad::Client::server_sendRecv(): send error" << std::endl;
		exit(1);
	}

	//Create a receive buffer
	std::vector<uint8_t> recvData(2048);
	//Receive and check for errors
	result = recv(v_socket, reinterpret_cast<char *>(recvData.data()), static_cast<int>(recvData.size()), 0);
	//If data was received
	if (result > 0) {
		//Create a packet and set the data to what was received
		FromNetworkPacket fromPacket;
		fromPacket.set_data(recvData);
		//Get the packetmodules from the packet
		auto packetModules = fromPacket.get_packetModulesCheck();
		//If there was at least one packet module
		if (packetModules.size() >= 1) {
			//Check to make sure that the reply is valid, and that it was the correct reply
			Module::Communication::Reply mod_reply;
			mod_reply.from_packetModule(packetModules.at(0));
			if ((mod_reply.description[Key::Module] == Value::Module::Invalid) || (mod_reply.reply != reply)) {
				std::cout << "tedpad::Client::serverRequest_Receive_GamepadFullDescription(): invalid reply" << std::endl;
				exit(1);
			}
		}
		//If there was not at least one packet module
		else {
			std::cout << "tedpad::Client::serverRequest_Receive_GamepadFullDescription(): incorrect packet data" << std::endl;
			exit(1);
		}
		//If there were 2 packetmodules
		if(packetModules.size() == 2) {
			//Set the return value to the extra packetmodule
			rtrn = packetModules.at(1);
		}
	}
	//If data wasn't received, and the server closed the connection
	else if (result == 0) {
		std::cout << "tedpad::Client::serverRequest_Receive_GamepadFullDescription(): the server closed the connection" << std::endl;
		exit(1);
	}
	//If there was an error
	else {
		std::cout << "tedpad::Client::serverRequest_Receive_GamepadFullDescription(): recv error" << std::endl;
		exit(1);
	}

	//Return the packetmodule that was received
	return(rtrn);
}

std::string tedpad::Client::ip_to_str(uint32_t const ip)
{
	in_addr ip_address;
	ip_address.s_addr = htonl(ip);
	char ipStr[128];
	inet_ntop(AF_INET, &ip_address, ipStr, sizeof(ipStr));
	return(ipStr);
}

uint32_t tedpad::Client::str_to_ip(std::string const & str)
{
	in_addr ip_addr;
	inet_pton(AF_INET, str.c_str(), &ip_addr);
	return(ntohl(ip_addr.s_addr));
}
