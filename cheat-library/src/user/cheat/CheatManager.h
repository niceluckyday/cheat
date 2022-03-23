#pragma once
#include <common/Event.h>
#include <cheat/Feature.h>

#include <windows.h>

#include <string>
#include <vector>
#include <map>

namespace cheat 
{
	class CheatManager
	{
	public:
		static CheatManager& GetInstance();
		static bool IsMenuShowed();

		CheatManager(CheatManager const&) = delete;
		void operator=(CheatManager const&) = delete;

		void AddFeature(Feature* feature);
		void AddFeatures(std::vector<Feature*> features);

		void SetModuleOrder(std::vector<std::string> moduleOrder);

		void OnKeyUp(short key, bool& cancelled);
		void OnRender();

		void Init(HMODULE hModule);

	private:

		struct SectionInfo
		{
			std::string moduleName;
			std::string sectionName;
			std::vector<Feature*> features;
		};

		std::vector<Feature*> m_Features;
		std::vector<std::string> m_ModuleOrder;
		std::map<std::string, std::map<std::string, std::vector<Feature*>>> m_FeatureMap;
		std::vector<SectionInfo> m_Sections;

		bool m_IsMenuShowed;
		bool m_IsBlockingInput;
		bool m_IsPrevCursorActive;

		CheatManager() : m_IsBlockingInput(true), m_IsMenuShowed(false), m_IsPrevCursorActive(false) {}

		void DrawMenu();
		void DrawMenuSection(const std::string& sectionName, std::vector<Feature*>& features);

		void DrawStatus();
		void DrawInfo();

		void PushFeature(Feature* feature);
		void UpdateSectionList();
	};
}


