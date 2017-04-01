#include "../../../include/tedpad/server/server.h"

void tedpad::Server::config_broadcast(bool const value)
{
	std::lock_guard<std::mutex> lx_config(pmx_config);
	if (pm_config[Config_e::Broadcast] != value) {
		std::lock_guard<std::mutex> lx_updateSignal(pm_lock);
		pm_config[Config_e::Broadcast] = value;
		pm_eventQueue.push_back(intern_server::UpdateSignal::Event::Server_ConfigUpdate_Broadcast);
		pm_request = true;
		pm_signal.notify_all();
	}
}

/*
void tedpad::Server::config_autoAccept(bool const value)
{
	std::lock_guard<std::mutex> lx_config(pmx_config);
	pm_config[Config_e::AutoAccept] = value;
}

void tedpad::Server::config_autoDecline(bool const value)
{
	std::lock_guard<std::mutex> lx_config(pmx_config);
	pm_config[Config_e::AutoDecline] = value;
}
*/

bool tedpad::Server::status_gamepadSet() const
{
	std::lock_guard<std::mutex> lx_state(pmx_state);
	return(pm_state[State_e::GamepadSet]);
}

void tedpad::Server::set_broadcastRate(std::chrono::milliseconds const & value)
{
	if (pm_broadcaster != nullptr)
		pm_broadcaster->set_updateRate(value);
}

void tedpad::Server::set_gamepad(Gamepad * const gamepad)
{
	pmx_state.lock();
	pm_externalGamepad = gamepad;
	pm_state[State_e::GamepadSet] = gamepad != nullptr;
	pmx_state.unlock();
	if (gamepad != nullptr) {
		gamepad->set_IODirection(Gamepad::IODirection::Server);
		pmx_gamepad.lock();
		pm_gamepad.from_gamepadFullDescription(gamepad->to_gamepadFullDescription());
		pmx_gamepad.unlock();
		server_gamepadSync();
	}
}

tedpad::Gamepad * tedpad::Server::get_gamepad() const
{
	return(pm_externalGamepad);
}

void tedpad::Server::set_port(uint16_t const port)
{
	std::lock_guard<std::mutex> lx_state(ThreadedObject::pmx_state);
	//Make sure that the server isn't already runnning
	if (!ThreadedObject::pm_state[ThreadedObject::State_e::ThreadRunning]) {
		std::lock_guard<std::mutex> lx_lock(pm_lock);
		pm_port = port;
	}
	//Add an exception here (when learned) that will throw if the server was already running
}

uint16_t tedpad::Server::get_port() const
{
	return(pm_port);
}

tedpad::ServerInfo tedpad::Server::get_serverInfo() const
{
	std::lock_guard<std::mutex> lx_gamepad(pmx_gamepad);
	ServerInfo rtrn;
	auto serverDescription = generate_serverDescription();
	rtrn.ip = serverDescription.ip;
	rtrn.port = serverDescription.port;
	rtrn.numberOfClients = serverDescription.number_clientsConnected;
	rtrn.gamepadDescription = pm_gamepad.to_gamepadBriefDescription();
	return(rtrn);
}

std::vector<tedpad::ClientInfo> tedpad::Server::get_connectedClients() const
{
	std::lock_guard<std::mutex> lx_connectedClient(pmx_connectedClient);
	std::vector<ClientInfo> rtrn;
	std::for_each(pm_connectedClient.begin(), pm_connectedClient.end(), [&](intern_server::ClientHandle const  * const p0) {rtrn.push_back(p0->get_clientInfo()); });
	return(rtrn);
}

void tedpad::Server::server_start()
{
	instruction_start();
}

void tedpad::Server::server_stop()
{
	instruction_stop();
}

void tedpad::Server::server_gamepadSync()
{
	std::lock_guard<std::mutex> lx_gamepad(pmx_gamepad);
	pm_externalGamepad->set_gamepadData_dataDirection(pm_gamepad.get_gamepadData_dataDirection(Module::Attribute::DataDirection::ServerInput));
	pm_gamepad.set_gamepadData(pm_externalGamepad->get_gamepadData_dataDirection(Module::Attribute::DataDirection::ServerOutput));
}

tedpad::Module::ServerDescription tedpad::Server::generate_serverDescription() const
{
	Module::ServerDescription rtrn;
	
	//Get the IP address of the server/host
	char hostName[128];
	if (gethostname(hostName, sizeof(hostName)) == -1) {
		std::cout << "tedpad::Server::generate_serverDescription(): gethostname error" << std::endl;
		exit(1);
	}
	addrinfo hints, *ppResult;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;
	int result = getaddrinfo(hostName, NULL, &hints, &ppResult);
	if (result != 0) {
		std::cout << "tedpad::Server::generate_serverDescription(): getaddrinfo error: " << gai_strerror(result) << std::endl;
		exit(1);
	}

	//Set the values
	rtrn.ip = ntohl(reinterpret_cast<sockaddr_in *>(ppResult->ai_addr)->sin_addr.s_addr);
	rtrn.port = pm_port;
	//Might need to lock connected client here.
	rtrn.number_clientsConnected = static_cast<uint16_t>(get_connectedClients().size());
	return(rtrn);
}

void tedpad::Server::thread_close_preJoin()
{
	pm_lock.lock();
	pm_request = true;
	pm_signal.notify_all();
	pm_lock.unlock();
}

void tedpad::Server::thread_init()
{
	socket_service::startup();
	std::lock_guard<std::mutex> lx_config(pmx_config);
	std::lock_guard<std::mutex> lx_updateSignal(pm_lock);
	if (!status_gamepadSet())
		return;

	pm_broadcaster = new intern_server::Broadcaster(intern_server::GamepadMutex{ &pm_gamepad, &pmx_gamepad }, generate_serverDescription());
	//This will cause the event to be handled (and therefore the broadcaster setting set) in thread_main
	pm_eventQueue.push_back(intern_server::UpdateSignal::Event::Server_ConfigUpdate_Broadcast);

	pm_designator = new intern_server::Designator(intern_server::UpdateSignal{ &pm_eventQueue, &pm_request, &pm_lock, &pm_signal }, pm_port);
	pm_designator->instruction_start();
}

void tedpad::Server::thread_main()
{
	std::unique_lock<std::mutex> lx_lock(pm_lock);
	while (pm_eventQueue.size() > 0) {
		//Make it so that there is only one of each event
		std::sort(pm_eventQueue.begin(), pm_eventQueue.end());
		pm_eventQueue.erase(std::unique(pm_eventQueue.begin(), pm_eventQueue.end()), pm_eventQueue.end());
		//Get the iterator to the current end of the queue
		auto queueEndItr = pm_eventQueue.end();
		//Apply the respective callback functions to the events
		std::for_each(pm_eventQueue.begin(), pm_eventQueue.end(), [&](intern_server::UpdateSignal::Event const &p0) {
			(this->*map_eventCallback.at(p0))(); });
		//Erase the event that were handled
		pm_eventQueue.erase(pm_eventQueue.begin(), queueEndItr);
		//If more events were added in the callback functions, they should still be there ready to be handled and the loop will repeat until there are not more events.
	}
	//Reset the request indicator
	pm_request = false;
	//Wait until there is an event
	pm_signal.wait(lx_lock, [&]() { return(pm_request); });
}

void tedpad::Server::thread_close()
{
	std::lock_guard<std::mutex> lx_connectedClient(pmx_config);
	pm_broadcaster->instruction_stop();
	delete pm_broadcaster;
	pm_designator->instruction_stop();
	delete pm_designator;
	//Delete all clientHandles
	std::for_each(pm_connectedClient.begin(), pm_connectedClient.end(),
		[](intern_server::ClientHandle * const p0) { p0->instruction_stop();  delete p0; });
	socket_service::shutdown();
}

void tedpad::Server::eventCallback_Designator_NewClient()
{
	std::lock_guard<std::mutex> lx_state(pmx_state);
	std::lock_guard<std::mutex> lx_connectedClient(pmx_connectedClient);
	//This will run for as many clientsPending as there are, since one is removed each time get_pendingClientInfo is called
	while (pm_designator->state_clientPending()) {
		//Create a new dynamically allocated clienthandle and add it to the connecteClient vector
		pm_connectedClient.emplace_back(new intern_server::ClientHandle (
			pm_designator->get_pendingClientInfo(true),
			intern_server::UpdateSignal{ &pm_eventQueue, &pm_request, &pm_lock, &pm_signal },
			intern_server::GamepadMutex{ &pm_gamepad, &pmx_gamepad },
			generate_serverDescription()));
	}

	pm_state[State_e::ClientPending] = false;
	//Only need to do push back since this is a callback function
	pm_eventQueue.push_back(intern_server::UpdateSignal::Event::Server_ValueUpdate_ServerDescription);
}

void tedpad::Server::eventCallback_ClientHandle_ClientDisconnected()
{
	std::lock_guard<std::mutex> lx_connectedClient(pmx_connectedClient);
	//Get an iterator to the clients to be removed
	auto remove_startItr = std::remove_if(pm_connectedClient.begin(), pm_connectedClient.end(),
		[](intern_server::ClientHandle * const p0) { return(p0->state_clientDisconnected()); });
	//Delete the clients to be removed
	std::for_each(remove_startItr, pm_connectedClient.end(),
		[](intern_server::ClientHandle * const p0) { p0->instruction_stop(); delete p0; });
	//Erease the pointers that were deleted
	pm_connectedClient.erase(remove_startItr, pm_connectedClient.end());
	//Changing the number of clients means an update to the server description.
	pm_eventQueue.push_back(intern_server::UpdateSignal::Event::Server_ValueUpdate_ServerDescription);
}

void tedpad::Server::eventCallback_Server_ConfigUpdate_Broadcast()
{
	std::lock_guard<std::mutex> lx_config(pmx_config);
	if (pm_broadcaster != nullptr) {
		//Could use ternary operator here
		if (pm_config[Config_e::Broadcast])
			pm_broadcaster->instruction_start();
		else
			pm_broadcaster->instruction_stop();
	}
}

void tedpad::Server::eventCallback_Server_ValueUpdate_Port()
{
	//Well this is redunant, since it can't be changed once the thread is runnigng. Keep in case I make it able to change later.
}

void tedpad::Server::eventCallback_Server_ValueUpdate_ServerDescription()
{
	std::lock_guard<std::mutex> lx_connectedClient(pmx_connectedClient);
	auto serverDescription = generate_serverDescription();
	pm_broadcaster->set_serverDescription(serverDescription);
	std::for_each(pm_connectedClient.begin(), pm_connectedClient.end(), [&](intern_server::ClientHandle * const p0) { p0->set_serverDescription(serverDescription); });
}

tedpad::Server::Server(Gamepad * const gamepad, bool const start) : pm_config(3), pm_state(2)
{
	pm_config[Config_e::Broadcast] = true;
	pm_config[Config_e::AutoAccept] = true;
	pm_config[Config_e::AutoDecline] = false;
	pm_state[State_e::GamepadSet] = false;

	pm_gamepad.set_IODirection(Gamepad::IODirection::Server);

	set_gamepad(gamepad);
	if (start)
		server_start();
}

tedpad::Server::~Server()
{
	server_stop();
}

std::map<tedpad::intern_server::UpdateSignal::Event, void (tedpad::Server:: *)()> tedpad::Server::map_eventCallback = {
	{ intern_server::UpdateSignal::Event::Designator_NewClient, &eventCallback_Designator_NewClient },
	{ intern_server::UpdateSignal::Event::ClientHandle_ClientDisconnected, &eventCallback_ClientHandle_ClientDisconnected },
	{ intern_server::UpdateSignal::Event::Server_ConfigUpdate_Broadcast, &eventCallback_Server_ConfigUpdate_Broadcast },
	{ intern_server::UpdateSignal::Event::Server_ValueUpdate_Port, &eventCallback_Server_ValueUpdate_Port }
};
