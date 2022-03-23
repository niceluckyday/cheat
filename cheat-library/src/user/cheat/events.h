#pragma once

#include <il2cpp-appdata.h>
#include <common/Event.h>

namespace cheat 
{
	class events 
	{
	public:
		inline static TCancelableEvent<short> KeyUpEvent {};
		inline static TEvent<> GameUpdateEvent{};
		inline static TEvent<uint32_t, app::MotionInfo*> MoveSyncEvent{};
	};
}