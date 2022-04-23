#include "pch-il2cpp.h"
#include "InteractiveMap.h"

#include <helpers.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/util.h>
#include <cheat-base/render/renderer.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <misc/cpp/imgui_stdlib.h>

namespace cheat::feature
{

#define UPDATE_DELAY(type, name, delay) \
							static type name = {};                 \
							static ULONGLONG s_LastUpdate = 0;     \
                            ULONGLONG currentTime = GetTickCount();\
                            if (s_LastUpdate + delay > currentTime)\
                                return name;\
                            s_LastUpdate = currentTime;

    static void InLevelMapPageContext_UpdateView_Hook(app::InLevelMapPageContext* __this, MethodInfo* method);
	static void InLevelMapPageContext_ZoomMap_Hook(app::InLevelMapPageContext* __this, float value, MethodInfo* method);
	static void MonoMiniMap_Update_Hook(app::MonoMiniMap* __this, MethodInfo* method);

    InteractiveMap::InteractiveMap() : Feature(),
        NFF(m_Enabled, "Interactive map", "m_InteractiveMap", "InteractiveMap", false),
		NF(m_SeparatedWindows, "Separated windows", "InteractiveMap", true),
		NF(m_UnlockedLogShow, "Unlocked log show", "InteractiveMap", false),

		NF(m_IconSize,    "Icon size",      "InteractiveMap", 20.0f),
        NF(m_DynamicSize, "Dynamic size",   "InteractiveMap", false),
		NF(m_ShowHDIcons, "Show HD icons",  "InteractiveMap", false),

		NF(m_ShowUnlocked,         "Show unlocked", "InteractiveMap", false),
		NF(m_UnlockedTransparency, "Unlocked point transparency", "InteractiveMap", 0.5f),

		NF(m_UnlockNearestPoint, "Unlock nearest point", "InteractiveMap", Hotkey()),
		NF(m_RevertLatestUnlock, "Revert latest unlock", "InteractiveMap", Hotkey()),
		NF(m_UnlockOnlySelected, "Unlock only showed",   "InteractiveMap", true),
		NF(m_PointFindRange,     "Point finding range",  "InteractiveMap", 30.0f),
		NF(m_UnlockedPointsField,"Unlocked points",      "InteractiveMap", "{}")
    {
        cheat::events::WndProcEvent += MY_METHOD_HANDLER(InteractiveMap::OnWndProc);
		cheat::events::KeyUpEvent += MY_METHOD_HANDLER(InteractiveMap::OnKeyUp);

		HookManager::install(app::MonoMiniMap_Update, MonoMiniMap_Update_Hook);

        HookManager::install(app::InLevelMapPageContext_UpdateView, InLevelMapPageContext_UpdateView_Hook);
		HookManager::install(app::InLevelMapPageContext_ZoomMap, InLevelMapPageContext_ZoomMap_Hook);

        LoadScenesData();
		InitializeEntityFilters();
		ApplyScaling();
		LoadUnlockedPoints();
    }

	const FeatureGUIInfo& InteractiveMap::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ "", "World", false };
		return info;
	}

	void InteractiveMap::DrawMain() { }

	InteractiveMap& InteractiveMap::GetInstance()
	{
		static InteractiveMap instance;
		return instance;
	}

	InteractiveMap::PointData* InteractiveMap::GetHoveredPoint()
	{
		std::lock_guard<std::mutex> _guard(m_PointMutex);
		return m_HoveredPoint;
	}

	InteractiveMap::PointData* InteractiveMap::FindNearestPoint(app::Vector2 levelPosition, uint32_t sceneID)
	{
		if (m_ScenesData.count(sceneID) == 0)
			return nullptr;

		auto& labels = m_ScenesData[sceneID].labels;
		
		PointData* minDinstancePoint = nullptr;
		float minDistance = 0;
		for (auto& [labelID, label] : labels)
		{
			if (m_UnlockOnlySelected && !label.enabled->value())
				continue;

			for (auto& [pointID, point] : label.points)
			{
				if (point.unlocked)
					continue;

				float distance = app::Vector2_Distance(nullptr, levelPosition, point.levelPosition, nullptr);
				if (distance < minDistance || minDinstancePoint == nullptr)
				{
					minDistance = distance;
					minDinstancePoint = &point;
				}
			}
		}

		if (minDinstancePoint == nullptr || (m_PointFindRange > 0 && minDistance > m_PointFindRange))
			return nullptr;

		return minDinstancePoint;
	}

	InteractiveMap::PointData* InteractiveMap::FindEntityPoint(game::Entity* entity, uint32_t sceneID)
	{
		if (m_ScenesData.count(sceneID) == 0)
			return nullptr;

		auto levelPosition = entity->levelPosition();

		auto& labels = m_ScenesData[sceneID].labels;

		PointData* minDinstancePoint = nullptr;
		float minDistance = 0;
		for (auto& [labelID, label] : labels)
		{
			if (m_LabelToFilter.count(label.clearName) == 0)
				continue;

			auto& filter = m_LabelToFilter[label.clearName];
			if (!filter->IsValid(entity))
				continue;

			for (auto& [pointID, point] : label.points)
			{
				if (point.unlocked)
					continue;

				float distance = app::Vector2_Distance(nullptr, levelPosition, point.levelPosition, nullptr);
				if (distance < minDistance || minDinstancePoint == nullptr)
				{
					minDistance = distance;
					minDinstancePoint = &point;
				}
			}

			break; // We need only first valid value
		}

		if (minDinstancePoint == nullptr || (m_PointFindRange > 0 && minDistance > m_PointFindRange))
			return nullptr;

		return minDinstancePoint;
	}

	void InteractiveMap::LoadUnlockedPoints()
	{
		nlohmann::json jRoot;
		try
		{
			jRoot = nlohmann::json::parse(m_UnlockedPointsField.value());
		}
		catch (nlohmann::json::parse_error& _)
		{
			LOG_ERROR("Failed parse unlocked points.");
			return;
		}

		for (auto& [cSceneID, jLabels] : jRoot.items())
		{
			auto sceneID = std::stoul(cSceneID);
			if (m_ScenesData.count(sceneID) == 0)
			{
				LOG_WARNING("Scene %u don't exist. Maybe map data was updated.", sceneID);
				continue;
			}

			auto& labels = m_ScenesData[sceneID].labels;
			for (auto& [cLabelID, jPoints] : jLabels.items())
			{
				auto labelID = std::stoul(cLabelID);
				if (labels.count(labelID) == 0)
				{
					LOG_WARNING("Label %u:%u don't exist. Maybe data was updated.", sceneID, labelID);
					continue;
				}

				auto& points = labels[labelID].points;
				for (auto& unlockData : jPoints)
				{
					auto& pointID = unlockData["pointID"];
					
					if (points.count(pointID) == 0)
					{
						LOG_WARNING("Point %u:%u:%u don't exist. Maybe data was updated.", sceneID, labelID, pointID);
						continue;
					}

					auto& point = points[pointID];
					if (m_UnlockedPointsSet.count(&point) > 0)
					{
						LOG_WARNING("Unlocked point %u:%u:%u dublicate.", sceneID, labelID, pointID);
						continue;
					}

					point.unlocked = true;
					point.unlockTimestamp = unlockData["unlockTimestamp"];

					m_UnlockedPointsSet.insert(&point);
					m_UnlockedPoints.push_back(&point);
				}
			}
		}

		m_UnlockedPoints.sort([](PointData* first, PointData* second) { return first->unlockTimestamp > second->unlockTimestamp; });
	}

	void InteractiveMap::SaveUnlockedPoints()
	{
		nlohmann::json jRoot = {};

		for (auto& [sceneID, scene] : m_ScenesData)
		{
			auto cSceneID = std::to_string(sceneID);
			if (!jRoot.contains(cSceneID))
				jRoot[cSceneID] = nlohmann::json::object();

			auto& jLabels = jRoot[cSceneID];
			for (auto& [labelID, label] : scene.labels)
			{
				auto cLabelID = std::to_string(labelID);

				if (!jLabels.contains(cLabelID))
					jLabels[cLabelID] = nlohmann::json::array();

				auto& jPoints = jLabels[cLabelID];
				for (auto& [pointID, point] : label.points)
				{
					if (!point.unlocked)
						continue;

					auto jPoint = nlohmann::json::object();
					jPoint["pointID"] = point.id;
					jPoint["unlockTimestamp"] = point.unlockTimestamp;
					jPoints.push_back(jPoint);
				}

				if (jPoints.size() == 0)
					jLabels.erase(cLabelID);
			}

			if (jLabels.size() == 0)
				jRoot.erase(cSceneID);
		}

		*m_UnlockedPointsField.valuePtr() = jRoot.dump();
		m_UnlockedPointsField.Check();
	}

	void InteractiveMap::AddUnlockedPoint(PointData* pointData)
	{
		if (m_UnlockedPointsSet.count(pointData) > 0)
			return;

		pointData->unlocked = true;
		pointData->unlockTimestamp = util::GetCurrentTimeMillisec();

		m_UnlockedPointsSet.insert(pointData);
		m_UnlockedPoints.push_front(pointData);
		
		SaveUnlockedPoints();
	}

	void InteractiveMap::RemoveUnlockedPoint(PointData* pointData)
	{
		if (m_UnlockedPointsSet.count(pointData) == 0)
			return;

		pointData->unlocked = false;
		pointData->unlockTimestamp = 0;

		m_UnlockedPointsSet.erase(pointData);
		m_UnlockedPoints.remove_if([pointData](PointData* point) { return pointData == point; });

		SaveUnlockedPoints();
	}

	void InteractiveMap::RemoveLatestUnlockedPoint()
	{
		if (m_UnlockedPoints.size() == 0)
			return;

		PointData* pointData = m_UnlockedPoints.front();
		pointData->unlocked = false;
		pointData->unlockTimestamp = 0;

		m_UnlockedPoints.pop_front();
		m_UnlockedPointsSet.erase(pointData);

		SaveUnlockedPoints();
	}

	cheat::feature::InteractiveMap::PointData InteractiveMap::ParsePointData(const nlohmann::json& data)
	{
		return { data["id"], 0, 0, { data["x_pos"], data["y_pos"] }, false, 0};
	}

	void InteractiveMap::LoadLabelData(const nlohmann::json& data, uint32_t sceneID, uint32_t labelID)
	{
		auto& sceneData = m_ScenesData[sceneID];
		auto& labelEntry = sceneData.labels[labelID];

        labelEntry.name = data["name"];
        labelEntry.clearName = data["clear_name"];
        labelEntry.enabled = new config::field::BaseField<bool>(labelEntry.name,
            fmt::format("{}_{}", sceneID, labelEntry.clearName),
            "InteractiveMapFilters", false);

        config::AddField(*labelEntry.enabled);

        for (auto& pointJsonData : data["points"])
        {
			PointData data = ParsePointData(pointJsonData);
			data.labelID = labelID;
			data.sceneID = sceneID;

			labelEntry.points[data.id] = data;
        }

        sceneData.nameToLabel[labelEntry.clearName] = &labelEntry;
	}

	void InteractiveMap::LoadCategorieData(const nlohmann::json& data, uint32_t sceneID)
	{
        auto& sceneData = m_ScenesData[sceneID];
        auto& labels = sceneData.labels;
        auto& categories = sceneData.categories;
        
        categories.push_back({});
        auto& newCategory = categories.back();
        
        auto& children = newCategory.children;
        for (auto& child : data["children"])
        {
            if (labels.count(child) > 0)
                children.push_back(&labels[child]);
        }

        if (children.size() == 0)
        {
            categories.pop_back();
            return;
        }

        newCategory.name = data["name"];
	}

	void InteractiveMap::LoadSceneData(const nlohmann::json& data, uint32_t sceneID)
	{
		for (auto& [labelID, labelData] : data["labels"].items())
		{
			LoadLabelData(labelData, sceneID, std::stoi(labelID));
		}

        for (auto& categorie : data["categories"])
        {
            LoadCategorieData(categorie, sceneID);
        }
	}

	void InteractiveMap::LoadScenesData()
	{
        LoadSceneData(nlohmann::json::parse(ResourceLoader::Load("MapTeyvatData", RT_RCDATA)), 3);
        LoadSceneData(nlohmann::json::parse(ResourceLoader::Load("MapEnkanomiyaData", RT_RCDATA)), 5);
        LoadSceneData(nlohmann::json::parse(ResourceLoader::Load("MapUndegroundMinesData", RT_RCDATA)), 6);

        LOG_INFO("Interactive map data loaded successfully.");
    }

    struct ScalingData
    {
        float scale;
        float offset;
    };

    ScalingData ComputeScaling(app::Vector2 normal, app::Vector2 scaled)
    {
		// Just the equation: 
		//	s[0] * scale + offset = n[0]
		//	s[1] * scale + offset = n[1]
		// Where: s = scaled, n = normal

        ScalingData scalingData {};
        scalingData.scale = (normal.y - normal.x) / (scaled.y - scaled.x);
        scalingData.offset = normal.x - scaled.x * scalingData.scale;
        
        return scalingData;
    }

	void InteractiveMap::ApplyScaling()
	{
        // For find scaling we need two objects' correct & scaled coordinates
        // Better find objects with one point on map
        app::Vector2 NormalPos1 = { 1301.2f, 2908.4f }; // AnemoHypostasis
        app::Vector2 NormalPos2 = { 1942.3f, 1308.9f }; // ElectroHypostasis

        app::Vector2 ScalledPos1 = m_ScenesData[3].nameToLabel["AnemoHypostasis"]->points.begin()->second.levelPosition;
        app::Vector2 ScalledPos2 = m_ScenesData[3].nameToLabel["ElectroHypostasis"]->points.begin()->second.levelPosition;

        ScalingData xScale = ComputeScaling({ NormalPos1.x, NormalPos2.x }, { ScalledPos1.x, ScalledPos2.x });
        ScalingData yScale = ComputeScaling({ NormalPos1.y, NormalPos2.y }, { ScalledPos1.y, ScalledPos2.y });

        app::Vector2 scale = { xScale.scale, yScale.scale };
        app::Vector2 offset = { xScale.offset, yScale.offset };

        LOG_DEBUG("Position scaling: scale %0.3f %0.3f, offset %0.3f %0.3f", scale.x, scale.y, offset.x, offset.y);
        for (auto& [sceneID, sceneData] : m_ScenesData)
        {
            for (auto& [labelID, labelData] : sceneData.labels)
            {
                for (auto& [pointID, point] : labelData.points)
                {
                    point.levelPosition = point.levelPosition * scale + offset;
                }
            }
        }
	}
	
	void InteractiveMap::RefreshValidPoints()
	{

	}

    void InteractiveMap::DrawMenu()
    {
		BeginGroupPanel("General");
		{
			ConfigWidget("Enabled", m_Enabled);
			ConfigWidget(m_SeparatedWindows, "Config and filters will be in separate windows.");
			if (ImGui::Button(m_UnlockedLogShow ? "Show log window" : "Hide log window"))
			{
				*m_UnlockedLogShow.valuePtr() = !m_UnlockedLogShow;
				m_UnlockedLogShow.Check();
			}
		}
		EndGroupPanel();

		BeginGroupPanel("Icon view");
		{
			ConfigWidget(m_IconSize, 0.01f, 4.0f, 100.0f);
			ConfigWidget(m_DynamicSize, "Icons will be sized dynamically depend to zoom size.");
			ConfigWidget(m_ShowHDIcons, "Toggle icons to HD format.");
		}
		EndGroupPanel();

		BeginGroupPanel("Unlocked icon view");
		{
			ConfigWidget(m_ShowUnlocked, "Show unlocked points.");
			ConfigWidget(m_UnlockedTransparency, 0.01f, 0.0f, 1.0f, "Unlocked points transparency.");
		}
		EndGroupPanel();

		BeginGroupPanel("Unlock functionality");
		{
			ConfigWidget(m_UnlockNearestPoint, "When pressed, unlock the nearest to avatar point.");
			ConfigWidget(m_RevertLatestUnlock, "When pressed, revert latest unlock operation.");
			ConfigWidget(m_UnlockOnlySelected, "Unlock performed only to visible points.");
			ConfigWidget(m_PointFindRange, 0.5f, 0.0f, 200.0f, "Unlock performs within specified range. If 0 - unlimited.");
		}
		EndGroupPanel();
    }

	// Modified ImGui::CheckBox
	void InteractiveMap::DrawFilter(const LabelData& label)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(&label);
		const ImVec2 label_size = ImGui::CalcTextSize(label.name.c_str(), NULL, true);

		const float square_sz = ImGui::GetFrameHeight();
		const float image_sz = square_sz;

		const ImVec2 pos = window->DC.CursorPos;
		const ImRect total_bb(pos, 
			pos + ImVec2(square_sz + style.ItemInnerSpacing.x + image_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f),
				label_size.y + style.FramePadding.y * 2.0f));
		ImGui::ItemSize(total_bb, style.FramePadding.y);

		if (!ImGui::ItemAdd(total_bb, id))
		{
			IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
			return;
		}

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
		if (pressed)
		{
			*label.enabled->valuePtr() = !(*label.enabled);
			label.enabled->Check();

			ImGui::MarkItemEdited(id);
		}

		const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
		ImGui::RenderNavHighlight(total_bb, id);
		ImGui::RenderFrame(check_bb.Min, check_bb.Max, ImGui::GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
		ImU32 check_col = ImGui::GetColorU32(ImGuiCol_CheckMark);
		bool mixed_value = (g.LastItemData.InFlags & ImGuiItemFlags_MixedValue) != 0;
		if (mixed_value)
		{
			// Undocumented tristate/mixed/indeterminate checkbox (#2644)
			// This may seem awkwardly designed because the aim is to make ImGuiItemFlags_MixedValue supported by all widgets (not just checkbox)
			ImVec2 pad(ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)), ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)));
			window->DrawList->AddRectFilled(check_bb.Min + pad, check_bb.Max - pad, check_col, style.FrameRounding);
		}
		else if (*label.enabled)
		{
			const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
			ImGui::RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), check_col, square_sz - pad * 2.0f);
		}
		
		// --
		const ImVec2 image_pos(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y);
		const ImRect image_bb(image_pos, image_pos + ImVec2(image_sz, image_sz));

		auto image = ImageLoader::GetImage(label.clearName);
		if (image)
		{
			window->DrawList->AddImageRounded(image->textureID, image_bb.Min, image_bb.Max, 
				ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImColor(255, 255, 255), image_sz / 4);
		}
		// --

		ImVec2 label_pos = ImVec2(image_bb.Max.x + style.ItemInnerSpacing.x, image_bb.Min.y + style.FramePadding.y);
		if (g.LogEnabled)
			ImGui::LogRenderedText(&label_pos, mixed_value ? "[~]" : *label.enabled ? "[x]" : "[ ]");
		if (label_size.x > 0.0f)
			ImGui::RenderText(label_pos, label.name.c_str());

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
		return;
	}
	
	void InteractiveMap::DrawFilters()
	{
		auto sceneID = game::GetCurrentMapSceneID();
		if (m_ScenesData.count(sceneID) == 0)
			ImGui::Text("Sorry. Current scene is not supported.");

		ImGui::InputText("Search", &m_SearchText);

		auto& categories = m_ScenesData[sceneID].categories;
		for (auto& [categoryName, labels] : categories)
		{
			ImGui::PushID(categoryName.c_str());
			std::vector<LabelData*> validLabels;

			if (m_SearchText.empty())
			{
				validLabels = labels;
			}
			else
			{
				for (auto& label : labels)
				{
					std::string name = label->name;
					std::transform(name.begin(), name.end(), name.begin(), ::tolower);
					std::string search = m_SearchText;
					std::transform(search.begin(), search.end(), search.begin(), ::tolower);
					if (name.find(search) != std::string::npos)
						validLabels.push_back(label);
				}
			}

			if (validLabels.size() == 0)
				continue;

			SelectData selData
			{
				std::all_of(validLabels.begin(), validLabels.end(), [](const LabelData* label) { return label->enabled->value(); }),
				false
			};

			if (BeginGroupPanel(categoryName.c_str(), ImVec2(-1, 0), true, &selData))
			{
				ImGui::BeginTable("MarkFilters", 2);
				for (auto& label : validLabels)
				{
					ImGui::TableNextColumn();
					DrawFilter(*label);
				}
				ImGui::EndTable();

				EndGroupPanel();
			}

			if (selData.changed)
			{
				for (auto& label : validLabels)
				{
					*label->enabled->valuePtr() = selData.toggle;
				}
				validLabels[0]->enabled->Check();
			}
			ImGui::PopID();
		}
	}

	static bool IsMapActive()
	{
		auto uimanager = GET_SINGLETON(UIManager_1);
		if (uimanager == nullptr)
			return false;

		return app::UIManager_1_HasEnableMapCamera(uimanager, nullptr);
	}

	static app::Rect s_MapViewRect = { 0, 0, 1, 1 };
	static void InLevelMapPageContext_UpdateView_Hook(app::InLevelMapPageContext* __this, MethodInfo* method)
	{
		s_MapViewRect = __this->fields._mapViewRect;
	}

	static ImVec2 LevelToMapScreenPos(const app::Vector2& levelPosition)
	{
		if (s_MapViewRect.m_Width == 0 || s_MapViewRect.m_Height == 0)
			return {};

		ImVec2 screenPosition;

		// Got position from 0 to 1
		screenPosition.x = (levelPosition.x - s_MapViewRect.m_XMin) / s_MapViewRect.m_Width;
		screenPosition.y = (levelPosition.y - s_MapViewRect.m_YMin) / s_MapViewRect.m_Height;

		// Scaling to screen position
		screenPosition.x = screenPosition.x * app::Screen_get_width(nullptr, nullptr);
		screenPosition.y = (1.0f - screenPosition.y) * app::Screen_get_height(nullptr, nullptr);

		return screenPosition;
	}

    inline ImVec2 operator - (const ImVec2& A, const float k)
	{
		return { A.x - k, A.y - k };
	}

	inline ImVec2 operator + (const ImVec2& A, const float k)
	{
		return { A.x + k, A.y + k };
	}

	static std::mutex _windowRectsMutex;
	static std::vector<ImRect> _windowRects;

	static void AddWindowRect()
	{
		_windowRects.push_back(
			{
				ImGui::GetWindowPos(),
				ImGui::GetWindowPos() + ImGui::GetWindowSize()
			}
		);
	}

	static app::MonoMiniMap* _monoMiniMap;
	static bool IsMiniMapActive()
	{
		if (_monoMiniMap == nullptr)
			return false;

		SAFE_BEGIN();
		return app::Behaviour_get_isActiveAndEnabled(reinterpret_cast<app::Behaviour*>(_monoMiniMap), nullptr);
		SAFE_ERROR();
		_monoMiniMap = nullptr;
		return false;
		SAFE_END();
	}

	static float GetMinimapLevelDistance()
	{
		if (_monoMiniMap == nullptr)
			return {};

		return _monoMiniMap->fields._areaMinDistance;
	}


	static void DrawAreaTest()
	{
		auto& manager = game::EntityManager::instance();
		
		auto avatarLevelPos = manager.avatar()->levelPosition();
		auto avatarScreenPos = LevelToMapScreenPos(avatarLevelPos);

		auto areaLevelPos = avatarLevelPos;
		areaLevelPos.y += 175;
		auto areaSceenPos = LevelToMapScreenPos(areaLevelPos);

		auto draw = ImGui::GetBackgroundDrawList();
		draw->AddCircle(avatarScreenPos, abs(areaSceenPos.y - avatarScreenPos.y), ImColor(1.0f, 0.0f, 0.5f));
	}

	void InteractiveMap::DrawExternal()
	{
		if (IsMiniMapActive())
			DrawMinimapPoints();

        if (!IsMapActive())
            return;
        
		DrawAreaTest();

		auto mapManager = GET_SINGLETON(MapManager);
		if (mapManager == nullptr)
			return;

		// Draw windows
		{
			std::lock_guard _rectGuard(_windowRectsMutex);
			
			_windowRects.clear();

			bool menuOpened = ImGui::Begin("Interactive map", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
			AddWindowRect();

			if (menuOpened)
			{
				DrawMenu();

				if (!m_SeparatedWindows)
				{
					ImGui::Spacing();
					DrawFilters();
				}
				ImGui::End();
			}

			if (m_SeparatedWindows)
			{
				bool filtersOpened = ImGui::Begin("Filters", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
				AddWindowRect();

				if (filtersOpened)
				{
					DrawFilters();
					ImGui::End();
				}
			}
		}

		if (!m_Enabled)
			return;

        DrawPoints();
	}

	static bool IsRectInScreen(const ImRect& rect, const ImVec2& screenSize)
	{
		return rect.Min.x < screenSize.x && rect.Min.y < screenSize.y &&
			rect.Max.x > 0 && rect.Max.y > 0;
	}
	
	static void RenderPointCircle(const ImVec2& position, ImTextureID textureID, float transparency, float radius)
	{
		ImVec2 imageStartPos = position - radius;
		ImVec2 imageEndPos = position + radius;

		auto draw = ImGui::GetBackgroundDrawList();
		draw->AddCircleFilled(position, radius, ImColor(0.23f, 0.26f, 0.32f, transparency));

		if (textureID)
		{
			draw->AddImageRounded(textureID, imageStartPos + 2.0f, imageEndPos - 2.0f,
				ImVec2(0, 0), ImVec2(1, 1), ImColor(1.0f, 1.0f, 1.0f, transparency), radius);
		}

		draw->AddCircle(position, radius, ImColor(0.91f, 0.68f, 0.36f, transparency));
	}

	void InteractiveMap::DrawPoint(const PointData& pointData, const ImVec2& screenPosition, float radius, float radiusSquared, ImTextureID texture)
	{
		if (pointData.unlocked && !m_ShowUnlocked)
			return;

		float transparency = pointData.unlocked ? m_UnlockedTransparency : 1.0f;

		if (/* m_SelectedPoint == nullptr && */ m_HoveredPoint != nullptr)
		{
			RenderPointCircle(screenPosition, texture, transparency, radius);
			return;
		}

		ImVec2 mousePos = ImGui::GetMousePos();
		ImVec2 diffSize = screenPosition - mousePos;
		if (diffSize.x * diffSize.x + diffSize.y * diffSize.y > radiusSquared)
		{
			RenderPointCircle(screenPosition, texture, transparency, radius);
			return;
		}

		m_HoveredPoint = const_cast<PointData*>(&pointData);
		radius *= 1.2f;

		RenderPointCircle(screenPosition, texture, transparency, radius);

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			if (pointData.unlocked)
				RemoveUnlockedPoint(m_HoveredPoint);
			else
				AddUnlockedPoint(m_HoveredPoint);
		}	
	}

    void InteractiveMap::DrawPoints()
	{
		static uint32_t _lastSceneID = 0;
		static const float relativeSizeX = 821.0f;

		// TODO: Remove
		auto draw = ImGui::GetBackgroundDrawList();
		std::string fpsString = fmt::format("{:.1f}/{:.1f}", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		draw->AddText(ImVec2(100, 100), ImColor(0, 0, 0), fpsString.c_str());
		// 

		auto sceneID = game::GetCurrentMapSceneID();
		if (m_ScenesData.count(sceneID) == 0)
			return;
		
		if (sceneID != _lastSceneID)
		{
			_lastSceneID = sceneID;
			RefreshValidPoints();
		}

		ImVec2 screenSize = { static_cast<float>(app::Screen_get_width(nullptr, nullptr)),
			static_cast<float>(app::Screen_get_height(nullptr, nullptr)) };

		auto iconSize = m_DynamicSize ? m_IconSize * (relativeSizeX / s_MapViewRect.m_Width) : m_IconSize;
		auto radius = iconSize / 2;
		auto radiusSquared = radius * radius;

		std::lock_guard<std::mutex> _guard(m_PointMutex);
		// m_SelectedPoint = nullptr;
		m_HoveredPoint = nullptr;

		auto& labels = m_ScenesData[sceneID].labels;
		for (auto& [labelID, label] : labels)
		{
			if (!label.enabled->value())
				continue;

			auto image = ImageLoader::GetImage(m_ShowHDIcons ? "HD" + label.clearName : label.clearName);
			for (auto& [pointID, point] : label.points)
			{
				auto screenPosition = LevelToMapScreenPos(point.levelPosition);

				ImRect imageRect = { screenPosition - radius, screenPosition + radius };
				if (!IsRectInScreen(imageRect, screenSize))
					continue;

				//ImGui::PushID(&point);
				DrawPoint(point, screenPosition, radius, radiusSquared, image ? image->textureID : nullptr);
				//ImGui::PopID();
			}
		}
	}

	struct ImCircle
	{
		ImVec2 center;
		float radius;

		bool Contains(const ImCircle& b)
		{
			if (b.radius > radius)
				return false;

			auto diff = b.center - center;
			auto distanceSqrd = std::pow(diff.x, 2) + std::pow(diff.y, 2);
			auto radiusDiffSqrd = std::pow(radius - b.radius, 2);
			return radiusDiffSqrd > distanceSqrd;
		}
	};

	static ImCircle GetMinimapCircle()
	{
		if (_monoMiniMap == nullptr)
			return {};

		UPDATE_DELAY(ImCircle, _miniMapCircle, 2000);

		auto uiManager = GET_SINGLETON(UIManager_1);
		if (uiManager == nullptr || uiManager->fields._sceneCanvas == nullptr)
			return {};

		auto back = _monoMiniMap->fields._grpMapBack;
		if (back == nullptr)
			return {};

		auto mapPos = app::Transform_get_position(reinterpret_cast<app::Transform*>(back), nullptr);
		auto center = app::Camera_WorldToScreenPoint(uiManager->fields._uiCamera, mapPos, nullptr);
		center.y = app::Screen_get_height(nullptr, nullptr) - center.y;

		auto mapRect = app::RectTransform_get_rect(back, nullptr);
		float scaleFactor = app::Canvas_get_scaleFactor(uiManager->fields._sceneCanvas, nullptr);
		_miniMapCircle = {
			ImVec2(center.x, center.y),
			(mapRect.m_Width * scaleFactor) / 2
		};

		return _miniMapCircle;
	}

	static float GetMinimapRotation()
	{
		if (_monoMiniMap == nullptr)
			return {};

		auto back = _monoMiniMap->fields._grpMiniBackRotate;
		if (back == nullptr)
			return {};

		auto rotation = app::Transform_get_rotation(reinterpret_cast<app::Transform*>(back), nullptr);

		app::Quaternion__Boxed boxed = { nullptr, nullptr, rotation };
		return app::Quaternion_get_eulerAngles(&boxed, nullptr).z;
	}

	void InteractiveMap::DrawMinimapPoints()
	{
		// Found by hands. Only in Teyvat (3 scene), need also test another scenes.
		static const float minimapAreaLevelRadius = 175.0f;
		constexpr float PI = 3.14159265;

		auto sceneID = game::GetCurrentPlayerSceneID();
		if (m_ScenesData.count(sceneID) == 0)
			return;

		auto rotation = GetMinimapRotation();
		ImVec2 rotationMult = ImVec2(1.0f, 0.0f);
		if (rotation != 0)
		{
			auto rad =  ( (360.0f - rotation) * PI ) / 180.0f;
			rotationMult = { sin(rad), cos(rad) };
		}

		ImCircle minimapCircle = GetMinimapCircle();
		auto avatarLevelPos = game::EntityManager::instance().avatar()->levelPosition();
		auto scale = minimapCircle.radius / minimapAreaLevelRadius;
		
		auto iconRadius = (m_IconSize * scale * 2.0f) / 2;
		auto iconRadiusSqrd = std::pow(iconRadius, 2);

		auto& labels = m_ScenesData[sceneID].labels;
		for (auto& [labelID, label] : labels)
		{
			if (!label.enabled->value())
				continue;

			auto image = ImageLoader::GetImage(m_ShowHDIcons ? "HD" + label.clearName : label.clearName);
			for (auto& [pointID, point] : label.points)
			{
				ImVec2 positionDiff = { point.levelPosition.x - avatarLevelPos.x, avatarLevelPos.y - point.levelPosition.y };
				positionDiff = positionDiff * scale;
				if (rotation != 0.0f)
				{
					positionDiff = {
						positionDiff.x * rotationMult.y - positionDiff.y * rotationMult.x,
						positionDiff.x * rotationMult.x + positionDiff.y * rotationMult.y
					};
				}


				ImVec2 screenPos = minimapCircle.center + positionDiff;
				if (!minimapCircle.Contains({ screenPos, iconRadius }))
					continue;

				//ImGui::PushID(&point);
				DrawPoint(point, screenPos, iconRadius, iconRadiusSqrd, image ? image->textureID : nullptr);
				//ImGui::PopID();
			}
		}
	}

	// Blocking interacts when cursor on window

	static ImVec2 _lastMousePosition = {};
	static bool MouseInIMapWindow()
	{
		std::lock_guard _rectGuard(_windowRectsMutex);

		for (auto& rect : _windowRects)
		{
			if (rect.Contains(_lastMousePosition))
				return true;
		}
		return false;
	}

	static void InLevelMapPageContext_ZoomMap_Hook(app::InLevelMapPageContext* __this, float value, MethodInfo* method)
	{
		if (MouseInIMapWindow())
			return;

		return callOrigin(InLevelMapPageContext_ZoomMap_Hook, __this, value, method);
	}

	void InteractiveMap::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& cancelled)
	{
		if (!IsMapActive())
			return;

		POINT mPos;
		GetCursorPos(&mPos);
		ScreenToClient(hWnd, &mPos);
		ImVec2 cursorPos = { static_cast<float>(mPos.x), static_cast<float>(mPos.y) };
		_lastMousePosition = cursorPos;

		if (!MouseInIMapWindow())
			return;

		switch (uMsg)
		{
		case WM_MOUSEWHEEL:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
			cancelled = true;
			break;
		default:
			break;
		}
	}

	void InteractiveMap::OnKeyUp(short key, bool& cancelled)
	{
		if (m_UnlockNearestPoint.value().IsPressed(key))
		{
			auto& manager = game::EntityManager::instance();
			auto point = FindNearestPoint(manager.avatar()->levelPosition(), game::GetCurrentPlayerSceneID());
			if (point)
				AddUnlockedPoint(point);
		}

		if (m_RevertLatestUnlock.value().IsPressed(key))
		{
			RemoveLatestUnlockedPoint();
		}
	}

	void InteractiveMap::InitializeEntityFilters()
	{

	}

	void MonoMiniMap_Update_Hook(app::MonoMiniMap* __this, MethodInfo* method)
	{
		_monoMiniMap = __this;
		callOrigin(MonoMiniMap_Update_Hook, __this, method);
	}
}