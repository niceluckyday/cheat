#include "pch-il2cpp.h"
#include "AutoFish.h"

#include <helpers.h>
#include <algorithm>

#include <cheat/events.h>
#include <cheat/game/util.h>

namespace cheat::feature 
{

    static void FishingManager_Tick_Hook(app::FishingManager* __this, MethodInfo* method);

    AutoFish::AutoFish() : Feature(),
        NFF(m_Enabled, "Auto Fish", "m_AutoFish", "Debug", false)
    {
        HookManager::install(app::FishingManager_Tick, FishingManager_Tick_Hook);
    }

    const FeatureGUIInfo& AutoFish::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "Fishing", "Debug", true };
        return info;
    }

    void AutoFish::DrawMain()
    {
        ConfigWidget("Enabled", m_Enabled, "Automatically catch fish.");
    }

    bool AutoFish::NeedStatusDraw() const
	{
        return m_Enabled;
    }

    void AutoFish::DrawStatus() 
    {
        ImGui::Text("Auto Fish");
    }

    AutoFish& AutoFish::GetInstance()
    {
        static AutoFish instance;
        return instance;
    }

    static void FishingManager_Tick_Hook(app::FishingManager* __this, MethodInfo* method)
    {
        auto& af = AutoFish::GetInstance();

        if (af.m_Enabled && __this->fields._isFishing && __this->fields._dmgStart != 1)
        {
            app::FishingManager_get_forcePercent(__this, nullptr);
            //__this->fields._bonusRadius = 100.f; //increase radius
            //__this->fields._curDmg_k__BackingField = __this->fields._maxDmg; //auto finish
            __this->fields._curForce_k__BackingField = __this->fields._bonusCenter; //auto follow
        }

        callOrigin(FishingManager_Tick_Hook, __this, method);
    }
}

