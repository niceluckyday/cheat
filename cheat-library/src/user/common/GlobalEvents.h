#pragma once

#include "Event.h"
#include "Config.h"

class GlobalEvents {
public:
	inline static TCancelableEvent<short> KeyUpEvent{};
};