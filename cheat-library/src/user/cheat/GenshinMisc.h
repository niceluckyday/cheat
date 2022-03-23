#pragma once

#include <cheat-base/cheat/IGameMisc.h>

namespace cheat 
{
	class GenshinMisc : public IGameMisc
	{
	public:
		GenshinMisc(GenshinMisc const&) = delete;
		void operator=(GenshinMisc const&) = delete;

		static GenshinMisc& GetInstance();

		// Inherited via IGameMisc
		virtual void CursorSetVisibility(bool visibility) final;
		virtual bool CursorGetVisibility() final;

	protected:
		GenshinMisc() { };
	};
}
