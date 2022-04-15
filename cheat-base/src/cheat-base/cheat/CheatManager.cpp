#include <pch.h>
#include "CheatManager.h"

#include <cheat-base/Event.h>

#include <cheat-base/render/renderer.h>
#include <cheat-base/render/gui-util.h>
#include <cheat-base/cheat/misc/Settings.h>

namespace cheat 
{
	namespace events 
	{
		TCancelableEvent<short> KeyUpEvent{};
		TEvent<> WndProcEvent{};
	}

	void CheatManager::Init(LPBYTE pFontData, DWORD dFontDataSize, IGameMisc* gameMisc)
	{
		this->gameMisc = gameMisc;
		renderer::events::RenderEvent += MY_METHOD_HANDLER(CheatManager::OnRender);
		cheat::events::KeyUpEvent += MY_METHOD_HANDLER(CheatManager::OnKeyUp);
		cheat::events::WndProcEvent += MY_METHOD_HANDLER(CheatManager::OnWndProc);

		renderer::Init(pFontData, dFontDataSize);
	}

	void CheatManager::DrawExternal()
	{
		for (auto& feature : m_Features)
		{
			ImGui::PushID(&feature);
			feature->DrawExternal();
			ImGui::PopID();
		}
	}

	void CheatManager::DrawMenu()
	{
		if (m_ModuleOrder.size() == 0)
			return;

		static std::string* current = &m_ModuleOrder[m_SelectedSection];

		ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin("CCGenshin (By Callow)"))
		{
			ImGui::End();
			return;
		}

		ImGui::BeginGroup();

		if (ImGui::Checkbox("Block key/mouse", &m_IsBlockingInput))
		{
			renderer::globals::IsInputBlocked = m_IsBlockingInput;
		}

		if (ImGui::BeginListBox("##listbox 2", ImVec2(175, -FLT_MIN)))
		{
			size_t index = 0;
			for (auto& moduleName : m_ModuleOrder)
			{
				
				const bool is_selected = (current == &moduleName);
				if (ImGui::Selectable(moduleName.c_str(), is_selected))
				{
					current = &moduleName;
					*m_SelectedSection.valuePtr() = index;
					m_SelectedSection.Check();
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
				index++;
			}
			ImGui::EndListBox();
		}

		ImGui::EndGroup();

		ImGui::SameLine();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("ChildR", ImVec2(0, 0), true, window_flags);

		ImGui::Text(current->c_str());
		ImGui::Separator();

		auto& sections = m_FeatureMap[*current];
		auto emptyName = std::string();
		if (sections.count(emptyName) > 0)
			DrawMenuSection(emptyName, sections[""]);

		for (auto& [sectionName, features] : sections)
		{
			if (sectionName.empty())
				continue;

			DrawMenuSection(sectionName, features);
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void CheatManager::DrawMenuSection(const std::string& sectionName, std::vector<Feature*>& features)
	{
		if (!sectionName.empty())
			BeginGroupPanel(sectionName.c_str(), ImVec2(-1, 0));

		for (auto& feature : features)
		{
			ImGui::PushID(&feature);
			feature->DrawMain();
			ImGui::PopID();
		}

		if (!sectionName.empty())
			EndGroupPanel();
	}

	void CheatManager::DrawStatus()
	{
		// Drawing status window
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;

		auto& settings = feature::Settings::GetInstance();
		if (!settings.m_StatusMove)
			flags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;

		ImGui::Begin("Cheat status", 0, flags);

		static ImGuiTableFlags tabFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

		if (ImGui::BeginTable("activesTable", 1, tabFlags))
		{
			ImGui::TableSetupColumn("Active features");
			ImGui::TableHeadersRow();

			int row = 0;

			for (auto& feature : m_Features)
			{
				if (feature->NeedStatusDraw())
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					feature->DrawStatus();
					
					ImU32 row_bg_color = ImGui::GetColorU32(ImVec4(0.2f + row * 0.1f, 0.1f + row * 0.05f, 0.1f + row * 0.03f, 0.85f));
					ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, row_bg_color);
					row++;
				}
			}
			ImGui::EndTable();
		}

		ImGui::End();
	}

	void CheatManager::DrawInfo()
	{
		auto& settings = feature::Settings::GetInstance();

		// Drawing status window
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;

		if (!settings.m_StatusMove)
			flags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;

		auto checkLambda = [](const Feature* feat) { return feat->NeedInfoDraw(); };
		bool showAny = std::any_of(m_Features.begin(), m_Features.end(), checkLambda);
		if (!showAny && !settings.m_StatusMove)
			return;

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.04f, 0.05f, 0.05f, 0.90f));
		ImGui::Begin("Info window", 0, flags);
		ImGui::PopStyleColor();

		if (!showAny)
		{
			ImGui::Text("Nothing here");
			ImGui::End();
			return;
		}

		for (auto& moduleName : m_ModuleOrder)
		{
			auto& sections = m_FeatureMap[moduleName];
			bool moduleShowAny = std::any_of(sections.begin(), sections.end(), 
				[](const auto& iter) { 
					return std::any_of(iter.second.begin(), iter.second.end(), 
						[](const auto feat) { 
							return feat->NeedInfoDraw(); 
						}); 
				}
			);
			if (!moduleShowAny)
				continue;

			BeginGroupPanel(moduleName.c_str(), ImVec2(-1, 0));

			for (auto& [sectionName, features]: sections)
			{
				for (auto& feature : features) 
				{
					if (!feature->NeedInfoDraw())
						continue;

					ImGui::PushID(&feature);
					feature->DrawInfo();
					ImGui::PopID();
				}
			}

			EndGroupPanel();
		}

		ImGui::End();

	}


	void CheatManager::OnRender()
	{
		auto& settings = feature::Settings::GetInstance();

		DrawExternal();

		if (m_IsMenuShowed)
			DrawMenu();

		if (settings.m_StatusShow)
			DrawStatus();

		if (settings.m_InfoShow)
			DrawInfo();

		if (settings.m_MenuKey.value().IsReleased() && !ImGui::IsAnyItemActive())
			ToggleMenuShow();
	}

	void CheatManager::CheckToggles(short key)
	{
		if (m_IsMenuShowed || renderer::globals::IsInputBlocked)
			return;

		for (auto& field : config::GetToggleFields())
		{
			if (field->GetHotkeyField().value().IsPressed(key))
			{
				bool& value = *field->valuePtr();
				value = !value;
				field->Check();
			}
		}
	}

	bool menuToggled = false;
	void CheatManager::ToggleMenuShow()
	{
		m_IsMenuShowed = !m_IsMenuShowed;
		renderer::globals::IsInputBlocked = m_IsMenuShowed && m_IsBlockingInput;
		menuToggled = true;
	}

	void CheatManager::OnKeyUp(short key, bool& cancelled)
	{
		auto& settings = feature::Settings::GetInstance();
		if (!settings.m_MenuKey.value().IsPressed(key))
		{
			CheckToggles(key);
			return;
		}
	}
	
	void CheatManager::OnWndProc()
	{
		if (!menuToggled)
			return;

		menuToggled = false;

		if (m_IsMenuShowed)
		{
			m_IsPrevCursorActive = gameMisc->CursorGetVisibility();
			if (!m_IsPrevCursorActive)
				gameMisc->CursorSetVisibility(true);

		}
		else if (!m_IsPrevCursorActive)
			gameMisc->CursorSetVisibility(false);
	}

	bool CheatManager::IsMenuShowed()
	{
		auto& cheatManager = CheatManager::GetInstance();
		return cheatManager.m_IsMenuShowed;
	}


	CheatManager& CheatManager::GetInstance()
	{
		static CheatManager cheatManager;
		return cheatManager;
	}

	void CheatManager::PushFeature(Feature* feature)
	{
		m_Features.push_back(feature);

		auto& info = feature->GetGUIInfo();
		if (m_FeatureMap.count(info.moduleName) == 0)
		{
			m_FeatureMap[info.moduleName] = {};
			m_ModuleOrder.push_back(info.moduleName);
		}

		auto& sectionMap = m_FeatureMap[info.moduleName];
		std::string sectionName = info.isGroup ? info.name : std::string();
		if (sectionMap.count(sectionName) == 0)
			sectionMap[sectionName] = {};

		auto& featureList = sectionMap[sectionName];
		featureList.push_back(feature);
	}

	void CheatManager::AddFeature(Feature* feature)
	{
		PushFeature(feature);
	}

	void CheatManager::AddFeatures(std::vector<Feature*> features)
	{
		for (auto& feature : features)
		{
			PushFeature(feature);
		}
	}
	
	void CheatManager::SetModuleOrder(std::vector<std::string> moduleOrder)
	{
		std::unordered_set<std::string> moduleSet;
		moduleSet.insert(m_ModuleOrder.begin(), m_ModuleOrder.end());

		m_ModuleOrder.clear();

		for (auto& moduleName : moduleOrder) 
		{
			if (m_FeatureMap.count(moduleName) == 0)
				continue;

			m_ModuleOrder.push_back(moduleName);
			moduleSet.erase(moduleName);
		}

		for (auto& moduleName : moduleSet) 
		{
			m_ModuleOrder.push_back(moduleName);
		}
	}
}