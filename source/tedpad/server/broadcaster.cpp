#include "../../../include/tedpad/server/broadcaster.h"

void tedpad::intern_server::Broadcaster::set_port(uint16_t const port)
{
	std::lock_guard<std::mutex> lx_port(pmx_port);
	pm_port = port;
}

uint16_t tedpad::intern_server::Broadcaster::get_port() const
{
	std::lock_guard<std::mutex> lx_port(pmx_port);
	return(pm_port);
}

tedpad::intern_server::Broadcaster::Broadcaster(GamepadMutex const & gamepadMutex, uint16_t const port, std::chrono::milliseconds const & updateRate) : pm_gamepadMutex(gamepadMutex), pm_port(port), SleepObject(updateRate)
{
}
