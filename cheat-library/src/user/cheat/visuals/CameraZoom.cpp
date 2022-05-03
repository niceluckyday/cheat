#include "pch-il2cpp.h"
#include "CameraZoom.h"

#include <helpers.h>
#include <cheat/events.h>

namespace cheat::feature
{
    static void SCameraModuleInitialize_SetWarningLocateRatio_Hook(app::SCameraModuleInitialize* __this, double deltaTime, app::CameraShareData* data, MethodInfo* method);

    CameraZoom::CameraZoom() : Feature(),
        NFEX(f_Enabled, "Camera Zoom", "CameraZoom", "Visuals", false, false),
        NF(f_Zoom, "Zoom", "CameraZoom", 200)
    {
        HookManager::install(app::SCameraModuleInitialize_SetWarningLocateRatio, SCameraModuleInitialize_SetWarningLocateRatio_Hook);
    }

    const FeatureGUIInfo& CameraZoom::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "CameraZoom", "Visuals", false };
        return info;
    }

    void CameraZoom::DrawMain()
    {
        ConfigWidget(f_Enabled, "Increases camera zoom.");
        ConfigWidget(f_Zoom, 1.0f, 1.0f, 500.0f, "Zoom distance");
    }

    bool CameraZoom::NeedStatusDraw() const
    {
        return f_Enabled;
    }

    void CameraZoom::DrawStatus()
    {
        ImGui::Text("CameraZoom: %1.f", f_Zoom.value());
    }

    CameraZoom& CameraZoom::GetInstance()
    {
        static CameraZoom instance;
        return instance;
    } 

    void SCameraModuleInitialize_SetWarningLocateRatio_Hook(app::SCameraModuleInitialize* __this, double deltaTime, app::CameraShareData* data, MethodInfo* method)
    {
        CameraZoom& cameraZoom = CameraZoom::GetInstance();
        if (cameraZoom.f_Enabled)
        {
            data->currentWarningLocateRatio= static_cast<double>(cameraZoom.f_Zoom);
            //data->isRadiusSqueezing;
        }
        else
            data->currentWarningLocateRatio = 1.0;
        
        callOrigin(SCameraModuleInitialize_SetWarningLocateRatio_Hook, __this, deltaTime, data, method);
    }
}

