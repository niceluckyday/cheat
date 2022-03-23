#pragma  once
namespace cheat 
{
	class IGameMisc
	{
	public:
		virtual void CursorSetVisibility(bool visibility) = 0;
		virtual bool CursorGetVisibility() = 0;
	};
}