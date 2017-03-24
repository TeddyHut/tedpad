#include "../../../include/tedpad/server/clientHandle.h"

tedpad::intern_server::ClientHandle::ClientHandle(SOCKET const pm_socket) : pm_socket(pm_socket)
{
	std::lock_guard<std::mutex> lx_instruction(mx_instruction);
	std::lock_guard<std::mutex> lx_state(mx_state);
	m_instruction[Instruction_e::RunInit] = true;
	pm_thread = std::thread(&thread_main, this);
	m_state[State_e::Thread_Running] = true;
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
		std::lock_guard<std::mutex> lx_instruction(mx_instruction);
		std::lock_guard<std::mutex> lx_config(mx_config);
		if (m_instruction[Instruction_e::RunInit]) {
			thread_init();
			m_instruction[Instruction_e::RunInit] = false;
		}
	}
}
