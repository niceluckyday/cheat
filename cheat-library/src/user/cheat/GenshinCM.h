#pragma once

#include <cheat-base/cheat/CheatManagerBase.h>
namespace cheat
{
	class GenshinCM : public CheatManagerBase
	{
	public:
		static GenshinCM& instance();

		void CursorSetVisibility(bool visibility) final;
		bool CursorGetVisibility() final;

	protected:
		using CheatManagerBase::CheatManagerBase;

		void DrawProfileLine() final;
	};
}
