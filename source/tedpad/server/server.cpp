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
	server_gamepadSync();
}

tedpad::Gamepad * tedpad::Server::get_gamepad() const
{
	return(pm_externalGamepad);
}

std::vector<tedpad::ClientInfo> tedpad::Server::get_connectedClients() const
{
	std::lock_guard<std::mutex> lx_connectedClient(pmx_connectedClient);
	std::vector<ClientInfo> rtrn;
	std::for_each(pm_connectedClient.begin(), pm_connectedClient.end(), [&](intern_server::ClientHandle const &p0) {rtrn.push_back(p0.get_clientInfo()); });
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
	std::lock_guard<std::mutex> lx_config(pmx_config);
	std::lock_guard<std::mutex> lx_updateSignal(pm_lock);
	if (!status_gamepadSet())
		return;
	
	pm_broadcaster = new intern_server::Broadcaster(intern_server::GamepadMutex{ &pm_gamepad, &pmx_gamepad });
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
	//Wait until there is an event
	pm_signal.wait(lx_lock, [&]() { return(pm_request); });
}

void tedpad::Server::thread_close()
{
	delete pm_broadcaster;
	delete pm_designator;
	pm_connectedClient.clear();
}

void tedpad::Server::eventCallback_Designator_NewClient()
{
	std::lock_guard<std::mutex> lx_state(pmx_state);
	std::lock_guard<std::mutex> lx_connectedClient(pmx_connectedClient);
	while (pm_designator->state_clientPending()) {
		pm_connectedClient.emplace_back(
			pm_designator->get_pendingClientInfo(true),
			intern_server::UpdateSignal{ &pm_eventQueue, &pm_request, &pm_lock, &pm_signal },
			intern_server::GamepadMutex{ &pm_gamepad, &pmx_gamepad });
	}
	pm_state[State_e::ClientPending] = false;
}

void tedpad::Server::eventCallback_ClientHandle_ClientDisconnected()
{
	std::lock_guard<std::mutex> lx_connectedClient(pmx_connectedClient);
	pm_connectedClient.erase(std::remove_if(pm_connectedClient.begin(), pm_connectedClient.end(),
		[](intern_server::ClientHandle const &p0) { return(p0.state_clientDisconnected()); }), pm_connectedClient.end());
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

tedpad::Server::Server(Gamepad * const gamepad, bool const start)
{
	pm_config[Config_e::Broadcast] = true;
	pm_config[Config_e::AutoAccept] = true;
	pm_config[Config_e::AutoDecline] = false;
	pm_state[State_e::GamepadSet] = false;

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
	{ intern_server::UpdateSignal::Event::Server_ConfigUpdate_Broadcast, &eventCallback_Server_ConfigUpdate_Broadcast }
};
