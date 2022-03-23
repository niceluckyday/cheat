#pragma once
#include <Windows.h>
#include <cheat-base/Event.h>

namespace renderer
{

	void Init(LPBYTE pFontData, DWORD dFontDataSize);

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
