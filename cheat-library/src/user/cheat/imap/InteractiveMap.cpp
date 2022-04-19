#include "pch-il2cpp.h"
#include "InteractiveMap.h"

#include <helpers.h>
#include <cheat/game/EntityManager.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

namespace cheat::feature
{

    static void InLevelMapPageContext_UpdateView_Hook(app::InLevelMapPageContext* __this, MethodInfo* method);

    InteractiveMap::InteractiveMap() : Feature(),
        NFF(m_Enabled, "Interactive map", "m_InteractiveMap", "InteractiveMap", false),
        NF(m_IconSize, "Icon size", "InteractiveMap", 14.0f)
    {
        HookManager::install(app::InLevelMapPageContext_UpdateView, InLevelMapPageContext_UpdateView_Hook);
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

    std::string ImVec2ToStr(const ImVec2& vec)
    {
        return fmt::format("x: {:.3f}, y: {:.3f}", vec.x, vec.y);
    }

	void InteractiveMap::DrawExternal()
	{
        if (!IsMapActive())
            return;

        auto mapManager = GET_SINGLETON(MapManager);
        if (mapManager == nullptr)
            return;

        auto absolutePos = game::EntityManager::instance().avatar()->absolutePosition();
		auto levelPos = app::Miscs_GenLevelPos_1(nullptr, absolutePos, nullptr);
        
        auto draw = ImGui::GetBackgroundDrawList();

        auto screenPosition = LevelToMapScreenPos(levelPos);
        draw->AddRectFilled(screenPosition, screenPosition + ImVec2(20, 20), ImColor(255, 0, 0));
	}

}

