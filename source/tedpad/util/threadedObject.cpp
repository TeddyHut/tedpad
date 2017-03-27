#include "..\..\..\include\tedpad\util\threadedObject.h"

void tedpad::util::thread::ThreadedObject::instruction_start()
{
	std::lock_guard<std::mutex> lx_instruction(pmx_instruction);
	std::lock_guard<std::mutex> lx_state(pmx_state);
	pm_instruction[Instruction_e::Run_thread_init] = true;
	pm_thread = std::thread(&thread_manage, this);
	pm_state[State_e::ThreadRunning] = true;
}

void tedpad::util::thread::ThreadedObject::instruction_stop()
{
	pmx_instruction.lock();
	pm_instruction[Instruction_e::Run_thread_close] = true;
	pmx_instruction.unlock();

	pm_thread.join();
	pm_state[State_e::ThreadRunning] = false;
}

bool tedpad::util::thread::ThreadedObject::state_threadRunning() const
{
	std::lock_guard<std::mutex> lx_state(pmx_state);
	return(pm_state[State_e::ThreadRunning]);
}

tedpad::util::thread::ThreadedObject::ThreadedObject()
{
	pm_instruction[Instruction_e::Run_thread_init] = false;
	pm_instruction[Instruction_e::Run_thread_close] = false;
	pm_state[State_e::ThreadRunning] = false;
}

tedpad::util::thread::ThreadedObject::~ThreadedObject()
{
	pmx_state.lock();
	if (pm_state[State_e::ThreadRunning]) {
		pmx_state.unlock();
		instruction_stop();
	}
}

void tedpad::util::thread::ThreadedObject::thread_manage()
{
	while (true) {
		pmx_instruction.lock();
		if (pm_instruction[Instruction_e::Run_thread_init]) {
			pm_instruction[Instruction_e::Run_thread_init] = false;
			pmx_instruction.unlock();
			thread_init();
		}
		else pmx_instruction.unlock();

		pmx_instruction.lock();
		if (pm_instruction[Instruction_e::Run_thread_close]) {
			pm_instruction[Instruction_e::Run_thread_close] = false;
			pmx_instruction.unlock();
			thread_close();
			break;
		}
		else pmx_instruction.unlock();

		thread_main();
	}
}

void tedpad::util::thread::SleepObject::set_updateRate(std::chrono::milliseconds const & updateRate)
{
	std::lock_guard<std::mutex> lx_updateRate(pmx_updateRate);
	pm_updateRate = updateRate;
}

std::chrono::milliseconds tedpad::util::thread::SleepObject::get_updateRate() const
{
	std::lock_guard<std::mutex> lx_updateRate(pmx_updateRate);
	return(pm_updateRate);
}

tedpad::util::thread::SleepObject::SleepObject(std::chrono::milliseconds const & updateRate) : pm_updateRate(updateRate)
{
}
