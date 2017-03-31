#include "../../../include/tedpad/util/threadedObject.h"

void tedpad::util::thread::ThreadedObject::instruction_start()
{
	std::lock_guard<std::mutex> lx_instruction(pmx_instruction);
	std::lock_guard<std::mutex> lx_state(pmx_state);
	pm_instruction[Instruction_e::Run_thread_init] = true;
	pm_thread = std::thread(&ThreadedObject::thread_manage, this);
	pm_state[State_e::ThreadRunning] = true;
}

void tedpad::util::thread::ThreadedObject::instruction_stop()
{
	//If the managed thread is the caller
	pmx_thread.lock();
	if (pm_thread.get_id() == std::this_thread::get_id()) {
		pmx_thread.unlock();
		pmx_state.lock();
		pm_state[State_e::ThreadFinished] = true;
		pmx_state.unlock();

		std::unique_lock<std::mutex> lX_condtionVar(pm_conditionVar_lock);
		pm_conditionVar_signal.wait(lX_condtionVar, [&]() {return(pm_conditionVar_active); });
	}
	else {
		pmx_thread.unlock();
		pmx_instruction.lock();
		pm_instruction[Instruction_e::Run_thread_close] = true;
		pmx_instruction.unlock();

		//If the thread is blocking execution because it finished, tell it to unblock
		pmx_state.lock();
		if (pm_state[State_e::ThreadFinished]) {
			std::lock_guard<std::mutex> lx_conditionVar(pm_conditionVar_lock);
			pm_conditionVar_active = true;
			pm_conditionVar_signal.notify_all();
		}
		pmx_state.unlock();

		//Call thread_close_preJoin to wake the thread up if needed
		thread_close_preJoin();

		//Wait for the thread to finish execution
		pm_thread.join();
		pm_state[State_e::ThreadRunning] = false;
	}
}

bool tedpad::util::thread::ThreadedObject::state_threadRunning() const
{
	std::lock_guard<std::mutex> lx_state(pmx_state);
	return(pm_state[State_e::ThreadRunning]);
}

tedpad::util::thread::ThreadedObject::ThreadedObject() : pm_instruction(2), pm_state(2)
{
	pm_instruction[Instruction_e::Run_thread_init] = false;
	pm_instruction[Instruction_e::Run_thread_close] = false;
	pm_state[State_e::ThreadRunning] = false;
	pm_state[State_e::ThreadFinished] = false;
}

tedpad::util::thread::ThreadedObject::~ThreadedObject()
{
	pmx_state.lock();
	if (pm_state[State_e::ThreadRunning]) {
		pmx_state.unlock();
		instruction_stop();
	}
	pmx_state.unlock();
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

void tedpad::util::thread::ThreadedObject::thread_close_preJoin()
{
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
