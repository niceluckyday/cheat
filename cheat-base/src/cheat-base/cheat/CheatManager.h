#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/cheat/IGameMisc.h>
#include <cheat-base/config/Config.h>
#include <cheat-base/Event.h>

#include <vector>
#include <map>
#include <string>

#include <Windows.h>

namespace cheat 
{
	namespace events
	{
		extern TCancelableEvent<short> KeyUpEvent;
		extern TCancelableEvent<HWND, UINT, WPARAM, LPARAM> WndProcEvent;
	}

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
		void OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& cancelled);
		void OnRender();

		void Init(LPBYTE pFontData, DWORD dFontDataSize, IGameMisc* gameMisc);

	private:

		config::field::BaseField<int> m_SelectedSection;

		std::vector<Feature*> m_Features;
		std::vector<std::string> m_ModuleOrder;
		std::map<std::string, std::map<std::string, std::vector<Feature*>>> m_FeatureMap;
		IGameMisc* gameMisc;

		bool m_IsMenuShowed;
		bool m_IsBlockingInput;
		bool m_IsPrevCursorActive;

		CheatManager() : m_IsBlockingInput(true), m_IsMenuShowed(false), m_IsPrevCursorActive(false), gameMisc(nullptr),
			NF(m_SelectedSection, "", "General", 0)
		{}

		void DrawExternal();

		void DrawMenu();
		void DrawMenuSection(const std::string& sectionName, std::vector<Feature*>& features);

		void DrawStatus();
		void DrawInfo();
		void DrawFps();
		void DrawNotifications();

		void PushFeature(Feature* feature);
		void CheckToggles(short key);

		void ToggleMenuShow();
	};
}


