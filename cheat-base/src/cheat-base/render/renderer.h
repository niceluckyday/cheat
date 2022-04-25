#pragma once
#include <Windows.h>
#include <cheat-base/Event.h>

namespace renderer
{

	void Init(LPBYTE pFontData, DWORD dFontDataSize);

	void SetInputLock(void* id, bool value);
	void AddInputLocker(void* id);
	void RemoveInputLocker(void* id);
	bool IsInputLocked();

	class events 
	{
	public:
		inline static TEvent<> RenderEvent{};
	};
}
