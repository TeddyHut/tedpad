#include "../../../include/tedpad/server/serverCommon.h"

bool tedpad::intern_server::UpdateSignal::filled() const
{
	return((request != nullptr) && (lock != nullptr) && (signal != nullptr));
}

bool tedpad::intern_server::GamepadMutex::filled() const
{
	return((gamepad != nullptr) && (mx_gamepad != nullptr));
}
