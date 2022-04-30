#include "pch-il2cpp.h"
#include "Fog.h"

#include <helpers.h>
#include <cheat/events.h>

namespace cheat::feature
{
    Fog::Fog() : Feature(),
        NF(f_Enabled, "No Fog", "Fog", false),
        NF(f_Fps, "Fps", "Fog", 240)
    {
        events::GameUpdateEvent += MY_METHOD_HANDLER(Fog::OnGameUpdate);
    }

    const FeatureGUIInfo& Fog::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "Fog", "Debug", false };
        return info;
    }

    void Fog::DrawMain()
    {
        ConfigWidget("No Fog", f_Enabled, "Removes the fog");
        ConfigWidget("Fps", f_Fps, 1, 30, 360, "Fps unlocker");
    }

    bool Fog::NeedStatusDraw() const
    {
        return f_Enabled;
    }

    void Fog::DrawStatus()
    {
        ImGui::Text("No Fog");
    }

    Fog& Fog::GetInstance()
    {
        static Fog instance;
        return instance;
    }

    void Fog::OnGameUpdate()
    {
        Fog& Fog = Fog::GetInstance();
        app::RenderSettings_set_fog(nullptr, !Fog.f_Enabled, nullptr);
        app::Application_set_targetFrameRate(nullptr, Fog.f_Fps, nullptr);
    }
}

