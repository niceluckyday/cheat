#include "pch-il2cpp.h"
#include "ElementalSight.h"

#include <helpers.h>

namespace cheat::feature
{
    static void LevelSceneElementViewPlugin_Tick_Hook(app::LevelSceneElementViewPlugin* __this, float inDeltaTime, MethodInfo* method);

    ElementalSight::ElementalSight() : Feature(),
        NF(m_Enabled, "Permanent elemental sight", "ElementalSight", false)
    {
        HookManager::install(app::LevelSceneElementViewPlugin_Tick, LevelSceneElementViewPlugin_Tick_Hook);
    }

    const FeatureGUIInfo& ElementalSight::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "", "World", false };
        return info;
    }

    void ElementalSight::DrawMain()
    {
        ConfigWidget(m_Enabled, "Elemental sight is kept on even when moving.\n"
                     "To turn off, toggle off and use Elemental Sight again.");
    }

    bool ElementalSight::NeedStatusDraw() const
    {
        return m_Enabled;
    }

    void ElementalSight::DrawStatus()
    {
        ImGui::Text("Perma elemental sight");
    }

    ElementalSight& ElementalSight::GetInstance()
    {
        static ElementalSight instance;
        return instance;
    }

    static void LevelSceneElementViewPlugin_Tick_Hook(app::LevelSceneElementViewPlugin* __this, float inDeltaTime, MethodInfo* method)
    {
        ElementalSight& ElementalSight = ElementalSight::GetInstance();
        if (ElementalSight.m_Enabled)
            __this->fields.elementalSightActivated = true;
        callOrigin(LevelSceneElementViewPlugin_Tick_Hook, __this, inDeltaTime, method);
    }
}

