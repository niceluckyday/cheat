#include "pch-il2cpp.h"
#include "ESPSectionBase.h"

#include "ESPRender.h"

#include <cheat/esp/ESP.h>
#include <cheat/events.h>
#include <cheat/game/EntityManager.h>

namespace cheat::feature::esp
{
	ESPSectionBase::ESPSectionBase(std::string filterName) : m_Name(filterName)
	{
		cheat::events::KeyUpEvent += MY_METHOD_HANDLER(ESPSectionBase::OnKeyUp);
	}

	void ESPSectionBase::AddFilter(const std::string& name, game::IEntityFilter* filter)
	{
		m_Filters.push_back({ new config::field::ESPItemField(name, name, m_Name), filter });
		auto& last = m_Filters.back();
		config::AddField(*last.first);
	}

	const cheat::FeatureGUIInfo& ESPSectionBase::GetGUIInfo() const
	{
		static const FeatureGUIInfo info { m_Name + " filters", "ESP", true};
		return info;
	}

	void ESPSectionBase::DrawMain()
	{
		for (auto& [field, filter] : m_Filters)
		{
			ImGui::PushID(field);
			DrawFilterField(*field);
			ImGui::PopID();
		}

		if (ImGui::TreeNode(this, "Hotkeys"))
		{
			for (auto& [field, filter] : m_Filters)
			{
				ImGui::PushID(field);

				auto& hotkey = field->valuePtr()->m_EnabledHotkey;
				if (InputHotkey(field->GetName().c_str(), &hotkey, true))
					field->Check();

				ImGui::PopID();
			}

			ImGui::TreePop();
		}
	}

	void ESPSectionBase::DrawFilterField(config::field::ESPItemField& field)
	{
		auto& entry = *field.valuePtr();
		bool changed = false;

		if (ImGui::Checkbox("## FilterEnabled", &entry.m_Enabled))
			changed = true;

		ImGui::SameLine();
		
		bool pickerChanged = ImGui::ColorEdit4("## ColorPick", reinterpret_cast<float*>(&entry.m_Color));
		if (IsValueChanged(&entry, pickerChanged))
			changed = true;

		ImGui::SameLine();
		
		ImGui::Text("%s", field.GetName().c_str());

		if (changed)
			field.Check();
	}

	void ESPSectionBase::DrawExternal()
	{
		auto draw = ImGui::GetBackgroundDrawList();

		std::string fpsString = fmt::format("{:.1f}/{:.1f}", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		draw->AddText( ImVec2(100, 100), ImColor(0, 0, 0), fpsString.c_str());

		auto& esp = ESP::GetInstance();
		if (!esp.m_Enabled)
			return;
		
		render::PrepareFrame();

		auto& entityManager = game::EntityManager::instance();

		for (auto& entity : entityManager.entities())
		{
			for (auto& [field, filter] : m_Filters)
			{
				auto& entry = *field->valuePtr();
				if (!entry.m_Enabled || !m_FilterExecutor.ApplyFilter(entity, filter))
					continue;

				render::DrawEntity(entry.m_Name, entity, entry.m_Color);
				break;
			}
		}
	}

	void ESPSectionBase::OnKeyUp(short key, bool& cancelled)
	{
		for (auto& [field, filter] : m_Filters)
		{
			auto& entry = *field->valuePtr();
			if (entry.m_EnabledHotkey.IsPressed(key))
			{
				entry.m_Enabled = !entry.m_Enabled;
				field->Check();
			}
		}
	}
}