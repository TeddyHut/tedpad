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
	std::lock_guard<std::mutex> lx_port(pmx_port);
	std::lock_guard<std::mutex> lx_lock(pm_lock);
	pm_port = port;
	pm_eventQueue.push_back(intern_server::UpdateSignal::Event::Server_ValueUpdate_Port);
	pm_request = true;
	pm_signal.notify_all();
}

uint16_t tedpad::Server::get_port() const
{
	std::lock_guard<std::mutex> lx_port(pmx_port);
	return(pm_port);
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

void tedpad::Server::instruction_stop()
{
	ThreadedObject::pmx_instruction.lock();
	ThreadedObject::pm_instruction[ThreadedObject::Instruction_e::Run_thread_close] = true;
	ThreadedObject::pmx_instruction.unlock();

	pm_lock.lock();
	pm_request = true;
	pm_signal.notify_all();
	pm_lock.unlock();

	ThreadedObject::pm_thread.join();
	ThreadedObject::pm_state[ThreadedObject::State_e::ThreadRunning] = false;
}

void tedpad::Server::thread_init()
{
	socket_service::startup();
	std::lock_guard<std::mutex> lx_config(pmx_config);
	std::lock_guard<std::mutex> lx_updateSignal(pm_lock);
	if (!status_gamepadSet())
		return;
	
	Module::ServerDescription serverDescription;
	serverDescription.port = pm_port;
	//Going in all C -_-
	char hostName[128];
	if (gethostname(hostName, sizeof(hostName)) == -1) {
		std::cout << "tedpad::Server::thread_init(): gethostname error" << std::endl;
		exit(1);
	}
	addrinfo hints, *ppResult;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;
	int result = getaddrinfo(hostName, NULL, &hints, &ppResult);

	if (result != 0) {
		std::cout << "tedpad::Server::thread_init(): getaddrinfo error: " << gai_strerror(result) << std::endl;
		exit(1);
	}

	serverDescription.ip = reinterpret_cast<sockaddr_in *>(ppResult->ai_addr)->sin_addr.s_addr;

	pm_broadcaster = new intern_server::Broadcaster(intern_server::GamepadMutex{ &pm_gamepad, &pmx_gamepad }, serverDescription);
	//This will cause the event to be handled (and therefore the broadcaster setting set) in thread_main
	pm_eventQueue.push_back(intern_server::UpdateSignal::Event::Server_ConfigUpdate_Broadcast);

	pm_designator = new intern_server::Designator(intern_server::UpdateSignal{ &pm_eventQueue, &pm_request, &pm_lock, &pm_signal });
	pm_designator->instruction_start();
}

void tedpad::Server::thread_main()
{
	std::unique_lock<std::mutex> lx_lock(pm_lock);
	//Make it so that there is only one of each event
	std::sort(pm_eventQueue.begin(), pm_eventQueue.end());
	pm_eventQueue.erase(std::unique(pm_eventQueue.begin(), pm_eventQueue.end()), pm_eventQueue.end());
	//Apply the respective callback functions to the events
	std::for_each(pm_eventQueue.begin(), pm_eventQueue.end(), [&](intern_server::UpdateSignal::Event const &p0) { (this->*map_eventCallback.at(p0))(); });
	//Reset the request indicator
	pm_request = false;
	//Wait until there is an event
	pm_signal.wait(lx_lock, [&]() { return(pm_request); });
}

void tedpad::Server::thread_close()
{
	delete pm_broadcaster;
	delete pm_designator;
	//Delete all clientHandles
	std::for_each(pm_connectedClient.begin(), pm_connectedClient.end(),
		[](intern_server::ClientHandle * const p0) { delete p0; });
	socket_service::shutdown();
}

void tedpad::Server::eventCallback_Designator_NewClient()
{
	std::lock_guard<std::mutex> lx_state(pmx_state);
	std::lock_guard<std::mutex> lx_connectedClient(pmx_connectedClient);
	//This will run for as many clientsPending as there are, since one is removed each tiem get_pendingClientInfo is called
	while (pm_designator->state_clientPending()) {
		//Create a new dynamically allocated clienthandle and add it to the connecteClient vector
		pm_connectedClient.emplace_back( new intern_server::ClientHandle(
			pm_designator->get_pendingClientInfo(true),
			intern_server::UpdateSignal{ &pm_eventQueue, &pm_request, &pm_lock, &pm_signal },
			intern_server::GamepadMutex{ &pm_gamepad, &pmx_gamepad }));
	}
	pm_state[State_e::ClientPending] = false;
}

void tedpad::Server::eventCallback_ClientHandle_ClientDisconnected()
{
	std::lock_guard<std::mutex> lx_connectedClient(pmx_connectedClient);
	//Get an iterator to the clients to be removed
	auto remove_startItr = std::remove_if(pm_connectedClient.begin(), pm_connectedClient.end(),
		[](intern_server::ClientHandle * const p0) { return(p0->state_clientDisconnected()); });
	//Delete the clients to be removed
	std::for_each(remove_startItr, pm_connectedClient.end(),
		[](intern_server::ClientHandle * const p0) { delete p0; });
	//Erease the pointers that were deleted
	pm_connectedClient.erase(remove_startItr, pm_connectedClient.end());
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
	std::lock_guard<std::mutex> lx_port(pmx_port);
	if (pm_designator != nullptr) {
		pm_designator->set_port(pm_port);
	}
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
