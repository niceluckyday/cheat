#pragma once
#include <Windows.h>
#include <common/Event.h>

namespace renderer
{

	void Init(HMODULE hModule);

	class events 
	{
	public:
		inline static TEvent<> RenderEvent{};
	};

	class globals
	{
	public:
		inline static bool IsInputBlocked = false;
	};
}
