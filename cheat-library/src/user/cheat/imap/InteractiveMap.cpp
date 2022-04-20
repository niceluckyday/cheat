#include "pch-il2cpp.h"
#include "InteractiveMap.h"

#include <helpers.h>
#include <cheat/game/EntityManager.h>
#include <cheat-base/render/renderer.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

namespace cheat::feature
{

    static void InLevelMapPageContext_UpdateView_Hook(app::InLevelMapPageContext* __this, MethodInfo* method);

    InteractiveMap::InteractiveMap() : Feature(),
        NFF(m_Enabled, "Interactive map", "m_InteractiveMap", "InteractiveMap", false),
        NF(m_IconSize, "Icon size", "InteractiveMap", 14.0f),
        NF(m_DynamicSize, "Dynamic size", "IteractiveMap", true),
        NF(m_ShowUnlocked, "Show unlocked", "InteractiveMap", false),
        NF(m_ShowHDIcons, "Show HD icons", "InteractiveMap", false)
    {
        HookManager::install(app::InLevelMapPageContext_UpdateView, InLevelMapPageContext_UpdateView_Hook);
        LoadScenesData();
    }

	cheat::feature::InteractiveMap::PointData InteractiveMap::ParsePointData(const nlohmann::json& data)
	{
        return { data["x_pos"], data["y_pos"] };
	}

	void InteractiveMap::LoadLabelData(const nlohmann::json& data, uint32_t sceneID, uint32_t labelID)
	{
        LabelData labelData = {};
        labelData.name = data["name"];
        labelData.clearName = data["clear_name"];
        labelData.points = {};

        for (auto& pointJsonData : data["points"])
        {
            labelData.points.push_back(ParsePointData(pointJsonData));
        }

        auto& sceneData = m_ScenesData[sceneID];
        auto& labelEntry = sceneData.labels[labelID];
        labelEntry = labelData;
        sceneData.name2Label[labelData.clearName] = &labelEntry;
	}

	void InteractiveMap::LoadSceneData(const nlohmann::json& data, uint32_t sceneID)
	{
		for (auto& [labelID, labelData] : data.items())
		{
			LoadLabelData(labelData, sceneID, std::stoi(labelID));
		}
	}

	void InteractiveMap::LoadScenesData()
	{
        LoadSceneData(nlohmann::json::parse(ResourceLoader::Load("MapTeyvatData", RT_RCDATA)), 3);
        LoadSceneData(nlohmann::json::parse(ResourceLoader::Load("MapEnkanomiyaData", RT_RCDATA)), 5);
        LoadSceneData(nlohmann::json::parse(ResourceLoader::Load("MapUndegroundMinesData", RT_RCDATA)), 6);

        ApplyScaling();
        LOG_INFO("Interactive map data loaded successfully.");
    }

    struct ScalingData
    {
        float scale;
        float offset;
    };

    ScalingData ComputeScaling(app::Vector2 normal, app::Vector2 scaled)
    {
        ScalingData scalingData {};
        scalingData.scale = (normal.y - normal.x) / (scaled.y - scaled.x);
        scalingData.offset = normal.x - scaled.x * scalingData.scale;
        
        return scalingData;
    }

	void InteractiveMap::ApplyScaling()
	{
        // For find scaling we need two objects' correct & scaled coordinates
        // Better find objects with one point on map
        app::Vector2 NormalPos1 = { 1301.2f, 2908.4f };
        app::Vector2 NormalPos2 = { 1942.3f, 1308.9f };

        app::Vector2 ScalledPos1 = m_ScenesData[3].name2Label["AnemoHypostasis"]->points[0].pointLocation;
        app::Vector2 ScalledPos2 = m_ScenesData[3].name2Label["ElectroHypostasis"]->points[0].pointLocation;

        ScalingData xScale = ComputeScaling({ NormalPos1.x, NormalPos2.x }, { ScalledPos1.x, ScalledPos2.x });
        ScalingData yScale = ComputeScaling({ NormalPos1.y, NormalPos2.y }, { ScalledPos1.y, ScalledPos2.y });

        app::Vector2 scale = { xScale.scale, yScale.scale };
        app::Vector2 offset = { xScale.offset, yScale.offset };

        LOG_DEBUG("Position scaling: scale %0.3f %0.3f, offset %0.3f %0.3f", scale.x, scale.y, offset.x, offset.y);
        for (auto& [sceneID, sceneData] : m_ScenesData)
        {
            for (auto& [labelID, labelData] : sceneData.labels)
            {
                for (auto& point : labelData.points)
                {
                    point.pointLocation = point.pointLocation * scale + offset;
                }
            }
        }
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

    void InteractiveMap::DrawMenu()
    {
        ConfigWidget("Enabled", m_Enabled);
		ConfigWidget(m_IconSize, 0.01f, 4.0f, 100.0f);
		ConfigWidget(m_DynamicSize, "Icons will be sized dynamically depend to zoom size.");
        ConfigWidget(m_ShowUnlocked, "Show unlocked positions.");
        ConfigWidget(m_ShowHDIcons, "Show HD icons.");
		ImGui::End();
		
    }

    inline ImVec2 operator - (const ImVec2& A, const float k)
	{
		return { A.x - k, A.y - k };
	}

	inline ImVec2 operator + (const ImVec2& A, const float k)
	{
		return { A.x + k, A.y + k };
	}

    const float relativeSizeX = 821.0f;
	void InteractiveMap::DrawExternal()
	{
        if (!IsMapActive())
            return;
        
		auto mapManager = GET_SINGLETON(MapManager);
		if (mapManager == nullptr)
			return;

		if (ImGui::Begin("Interactive map", nullptr, ImGuiWindowFlags_NoFocusOnAppearing))
		{
			DrawMenu();
		}

		if (!m_Enabled)
			return;

        auto draw = ImGui::GetBackgroundDrawList();
        auto size = m_DynamicSize ? m_IconSize * (relativeSizeX / s_MapViewRect.m_Width) : m_IconSize;
        auto radius = size / 2;

        int i = 0;
        for (auto& [labelID, label]: m_ScenesData[3].labels)
        {
            if (i == 10)
                break;
            i++;

			auto image = ImageLoader::GetImage(m_ShowHDIcons ? "HD" + label.clearName : label.clearName);

			for (auto& point : label.points)
			{
				auto screenPosition = LevelToMapScreenPos(point.pointLocation);
				draw->AddCircleFilled(screenPosition, radius + 2, ImColor(59, 67, 84));
				if (image)
				{
					draw->AddImageRounded(image->textureID, screenPosition - radius, screenPosition + radius,
						ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255), radius);
				}

				draw->AddCircle(screenPosition, radius + 2, ImColor(233, 175, 92));
			}
        }
	}

}

