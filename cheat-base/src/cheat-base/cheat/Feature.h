#pragma once

#include <string>

namespace cheat 
{
	struct FeatureGUIInfo
	{
		std::string name;
		std::string moduleName;
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


