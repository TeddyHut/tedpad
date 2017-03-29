#include "../../../include/tedpad/server/clientHandle.h"

bool tedpad::intern_server::ClientHandle::state_clientDisconnected() const
{
	std::lock_guard<std::mutex> lx_state(pmx_state);
	return(pm_state[State_e::ClientDisconnected]);
}

tedpad::intern_server::ImplementationClientInfo tedpad::intern_server::ClientHandle::get_clientInfo() const
{
	std::lock_guard<std::mutex> lx_clientInfo(pmx_clientInfo);
	return(pm_clientInfo);
}

tedpad::intern_server::ClientHandle::ClientHandle(ImplementationClientInfo const &clientInfo, UpdateSignal const &updateSignal, GamepadMutex const &gamepadMutex, std::chrono::milliseconds const &updateRate) :
	pm_state(1),
	pm_clientInfo(clientInfo),
	pm_updateSignal(updateSignal),
	pm_gamepadMutex(gamepadMutex),
	SleepObject(updateRate)
{
	if (pm_updateSignal.filled() && pm_gamepadMutex.filled())
		instruction_start();
	else {
		std::cerr << "tedpad::intern_sever::ClientHandle::ClientHandle(): uninitialized constructor arguments" << std::endl;
		exit(1);
	}
}

void tedpad::intern_server::ClientHandle::thread_main()
{
	//Make the 2048 more universal (eg give it a name)
	std::vector<uint8_t> recv_data(2048);
	int bufferLen;
	bufferLen = recv(pm_clientInfo.socket, reinterpret_cast<char *>(recv_data.data()), static_cast<int>(recv_data.size()), 0);
	if (bufferLen > 0) {
		recv_data.resize(bufferLen);
		FromNetworkPacket fromPacket;
		fromPacket.set_data(recv_data);
		auto packetModules = fromPacket.get_packetModulesCheck();
		if (packetModules.size() > 0) {
			Module::Communication::Request requestModule;
			if (packetModules.at(0).name == requestModule.get_description()) {
				requestModule.from_packetModule(packetModules.at(0));
				ToNetworkPacket toPacket = (this->*requestCallback_map.at(requestModule.request))(fromPacket);
				send(pm_clientInfo.socket, reinterpret_cast<char const *>(toPacket.get_fullPacketData().data()), static_cast<int>(toPacket.get_fullPacketSize()), 0);
			}
		}
	}
	else if (bufferLen == 0) {
		pmx_state.lock();
		pm_state[State_e::ClientDisconnected] = true;
		pmx_state.unlock();
		instruction_stop();
	}
	else if ((socket_service::get_lastError() != socket_service::ERROR_EWOULDBLOCK) && (socket_service::get_lastError() != socket_service::ERROR_WAGAIN)) {
		std::cerr << "tedpad::intern_server::ClientHandle::thread_main(): recv error" << std::endl;
		exit(1);
	}
	else {
		pmx_updateRate.lock();
		auto sleep_time = pm_updateRate;
		pmx_updateRate.unlock();
		std::this_thread::sleep_for(sleep_time);
	}
}

void tedpad::intern_server::ClientHandle::thread_init()
{
	socket_service::socket_setBlocking(pm_clientInfo.socket, false);
}

void tedpad::intern_server::ClientHandle::thread_close()
{
	std::lock_guard<std::mutex> lx_updateSignal(*pm_updateSignal.lock);
	pm_updateSignal.eventQueue->push_back(UpdateSignal::Event::ClientHandle_ClientDisconnected);
	*pm_updateSignal.request = true;
	pm_updateSignal.signal->notify_all();

	closesocket(pm_clientInfo.socket);
}

tedpad::ToNetworkPacket tedpad::intern_server::ClientHandle::requestCallback_Receive_GamepadFullDescription(FromNetworkPacket const & fromPacket)
{
	std::lock_guard<std::mutex> lx_gamepad(*pm_gamepadMutex.mx_gamepad);
	ToNetworkPacket toPacket;
	toPacket.add_module(Module::Communication::Reply(Module::Communication::Reply_e::Send_GamepadFullDescription).to_packetModule());
	toPacket.add_module(pm_gamepadMutex.gamepad->to_gamepadFullDescription().to_packetModule());
	return(toPacket);
}

tedpad::ToNetworkPacket tedpad::intern_server::ClientHandle::requestCallback_Receive_GamepadData_DirectionOut(FromNetworkPacket const & fromPacket)
{
	std::lock_guard<std::mutex> lx_gamepad(*pm_gamepadMutex.mx_gamepad);
	ToNetworkPacket toPacket;
	toPacket.add_module(Module::Communication::Reply(Module::Communication::Reply_e::Send_GamepadData_DirectionOut).to_packetModule());
	toPacket.add_module(pm_gamepadMutex.gamepad->get_gamepadData().to_packetModule());
	return(toPacket);
}

tedpad::ToNetworkPacket tedpad::intern_server::ClientHandle::requestCallback_Send_GamepadData_DirectionIn(FromNetworkPacket const & fromPacket)
{
	ToNetworkPacket toPacket;
	toPacket.add_module(Module::Communication::Reply(Module::Communication::Reply_e::Receive_GamepadData_DirectionIn));

	auto fromModules = fromPacket.get_packetModulesCheck();
	Module::GamepadData gamepadData;
	auto gamepadDataItr = std::find_if(fromModules.begin(), fromModules.end(), [&](PacketModule const &p0) { return(p0.valid && (p0.name == gamepadData.get_description())); });
	if (gamepadDataItr != fromModules.end()) {
		std::lock_guard<std::mutex> lx_state(pmx_state);
		std::lock_guard<std::mutex> lx_gamepad(*pm_gamepadMutex.mx_gamepad);
		gamepadData.from_packetModule(*gamepadDataItr);
		pm_gamepadMutex.gamepad->set_gamepadData(gamepadData);
	}
	return(toPacket);
}

tedpad::ToNetworkPacket tedpad::intern_server::ClientHandle::requestCallback_Ping(FromNetworkPacket const & fromPacket)
{
	ToNetworkPacket toPacket;
	toPacket.add_module(Module::Communication::Reply(Module::Communication::Reply_e::Acknowledge).to_packetModule());
	return(toPacket);
}

std::map <tedpad::Module::Communication::Request_e, tedpad::ToNetworkPacket(tedpad::intern_server::ClientHandle::*)(tedpad::FromNetworkPacket const &)> tedpad::intern_server::ClientHandle::requestCallback_map = {
	{ tedpad::Module::Communication::Request_e::Receive_GamepadFullDescription, &tedpad::intern_server::ClientHandle::requestCallback_Receive_GamepadFullDescription},
	{ tedpad::Module::Communication::Request_e::Receive_GamepadData_DirectionOut, &tedpad::intern_server::ClientHandle::requestCallback_Receive_GamepadData_DirectionOut },
	{ tedpad::Module::Communication::Request_e::Send_GamepadData_DirectionIn, &tedpad::intern_server::ClientHandle::requestCallback_Send_GamepadData_DirectionIn },
	{ tedpad::Module::Communication::Request_e::Ping, &tedpad::intern_server::ClientHandle::requestCallback_Ping }
};
