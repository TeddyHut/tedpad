#include "../../../include/tedpad/server/clientHandle.h"

tedpad::intern_server::ImplementationClientInfo tedpad::intern_server::ClientHandle::get_clientInfo() const
{
	std::lock_guard<std::mutex> lx_clientInfo(pmx_clientInfo);
	return(pm_clientInfo);
}

void tedpad::intern_server::ClientHandle::set_updateRate(std::chrono::milliseconds const & updateRate)
{
	std::lock_guard<std::mutex> lx_updateRate(pmx_updateRate);
	pm_updateRate = updateRate;
}

std::chrono::milliseconds tedpad::intern_server::ClientHandle::get_updateRate() const
{
	std::lock_guard<std::mutex> lx_updateRate(pmx_updateRate);
	return(pm_updateRate);
}

tedpad::intern_server::ClientHandle::ClientHandle(ImplementationClientInfo const &clientInfo, UpdateSignal const &updateSignal, GamepadMutex const &gamepadMutex, std::chrono::milliseconds const &updateRate) :
	pm_clientInfo(clientInfo),
	pm_updateSignal(updateSignal),
	pm_gamepadMutex(gamepadMutex),
	pm_updateRate(updateRate)
{
	std::lock_guard<std::mutex> lx_instruction(mx_instruction);
	std::lock_guard<std::mutex> lx_state(mx_state);
	if (pm_updateSignal.filled() && pm_gamepadMutex.filled()) {
		m_instruction[Instruction_e::RunInit] = true;
		pm_thread = std::thread(&thread_main, this);
		m_state[State_e::Thread_Running] = true;
	}
	else {
		std::cerr << "tedpad::intern_sever::ClientHandle::ClientHandle(): uninitialized constructor arguments" << std::endl;
		exit(1);
	}
}

tedpad::intern_server::ClientHandle::~ClientHandle()
{
	mx_instruction.lock();
	m_instruction[Instruction_e::StopThread] = true;
	mx_instruction.unlock();

	pm_thread.join();
	m_state[State_e::Thread_Running] = false;
}

void tedpad::intern_server::ClientHandle::thread_main()
{
	while (true) {
		mx_instruction.lock();
		if (m_instruction[Instruction_e::RunInit]) {
			m_instruction[Instruction_e::RunInit] = false;
			mx_instruction.unlock();
			thread_init();
		}
		else mx_instruction.unlock();

		mx_instruction.lock();
		if (m_instruction[Instruction_e::StopThread]) {
			mx_instruction.unlock();
			thread_close();
			break;
		}
		else mx_instruction.unlock();

		//Make the 2048 more universal (eg give it a name)
		std::vector<uint8_t> recv_data(2048);
		int bufferLen = recv(pm_clientInfo.socket, reinterpret_cast<char *>(recv_data.data()), recv_data.size(), 0);
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
					send(pm_clientInfo.socket, reinterpret_cast<char const *>(toPacket.get_fullPacketData().data()), toPacket.get_fullPacketSize(), 0);
				}
			}
		}
		else if (bufferLen == 0) {
			std::lock_guard<std::mutex> lx_state(mx_state);
			m_state[State_e::ClientDisconnected] = true;
			break;
		}
		else if ((errno != EWOULDBLOCK) && (errno != EAGAIN)) {
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
}

void tedpad::intern_server::ClientHandle::thread_init()
{
	socket_setBlocking(pm_clientInfo.socket, false);
}

void tedpad::intern_server::ClientHandle::thread_close()
{
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
		std::lock_guard<std::mutex> lx_state(mx_state);
		std::lock_guard<std::mutex> lx_gamepad(*pm_gamepadMutex.mx_gamepad);
		std::lock_guard<std::unique_lock<std::mutex>> lx_updateSignal(*pm_updateSignal.lock);
		gamepadData.from_packetModule(*gamepadDataItr);
		pm_gamepadMutex.gamepad->set_gamepadData(gamepadData);

		m_state[State_e::GamepadUpdate] = true;
		*pm_updateSignal.request = true;
		pm_updateSignal.signal->notify_all();
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
