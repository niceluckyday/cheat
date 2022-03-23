#pragma once

namespace cheat 
{
	struct FeatureGUIInfo
	{
		const char* name;
		const char* moduleName;
		bool isGroup;
	};

	class Feature
	{
	public:
		Feature(Feature const&) = delete;
		void operator=(Feature const&) = delete;

		// GUI handlers
		virtual const FeatureGUIInfo& GetGUIInfo() const = 0;
		
		virtual void DrawMain() = 0;

		virtual bool NeedStatusDraw() const = 0;
		virtual void DrawStatus() = 0;
		
		virtual bool NeedInfoDraw() const = 0;
		virtual void DrawInfo() = 0;
	
	protected:
		Feature() { };
	};
}


