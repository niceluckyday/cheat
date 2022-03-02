#include <pch-il2cpp.h>
#include "cheat.h"

#include <il2cpp-appdata.h>
#include <helpers.h>

#include <util/HookManager.h>
#include <util/Config.h>

static int teleportStage = 0;
static app::Vector3 teleportPosition{};

void CreateCustomMapMark_Hook(void* __this, app::Vector2 position, app::NOIPNNCFAAH__Enum iconType, MethodInfo* method)
{
    if (!Config::cfgTpToMarkKey.GetValue().IsPressed())
        return callOrigin(CreateCustomMapMark_Hook, __this, position, iconType, method);

    auto worldPosition = app::Miscs_GenWorldPos(nullptr, position, nullptr);
    auto groundHeight = app::Miscs_CalcCurrentGroundHeight(nullptr, worldPosition.x, worldPosition.z, nullptr);
    std::cout << "[TP] Mark world position: " << il2cppi_to_string(worldPosition) << std::endl;

    teleportPosition = app::Vector3();
    teleportPosition.x = worldPosition.x;
    teleportPosition.y = groundHeight > 0 ? groundHeight + 20 : Config::cfgTpHeight.GetValue();
    teleportPosition.z = worldPosition.z;

    auto avatarPosition = app::ActorUtils_GetAvatarPos(nullptr, nullptr);
    if (app::Vector3_Distance(nullptr, avatarPosition, teleportPosition, nullptr) < 150)
    {
        app::ActorUtils_SetAvatarPos(nullptr, teleportPosition, nullptr);
        return callOrigin(CreateCustomMapMark_Hook, __this, position, iconType, method);
    }

    teleportStage = 3;
    auto someSingleton = GetSingleton(FJIDMGGJMEF);
    app::SendTeleportToWaypoint_FJIDMGGJMEF_CBBKNIJAKDE(someSingleton, 3, 6, nullptr, nullptr);

    return callOrigin(CreateCustomMapMark_Hook, __this, position, iconType, method);
}

app::MBHLOBDPKEC_BHKOIJIKDFG GetWaypointInformation_Hook(app::MBHLOBDPKEC* __this, uint32_t waypointType, uint32_t waypointId, MethodInfo* method)
{
    auto result = callOrigin(GetWaypointInformation_Hook, __this, waypointType, waypointId, method);
    if (teleportStage != 3)
        return result;

    std::cout << "[TP Stage 1/3] Changing waypoint location." << std::endl;

    result.waypointLocation->fields.vector3 = teleportPosition;

    teleportStage--;
    return result;
}

void DoTeleport_Hook(app::FJIDMGGJMEF* __this, app::Vector3 position, app::MEPEDAIKBDI__Enum someEnum,
    uint32_t someUint1, app::CMHGHBNDBMG_ECPNDLCPDIE__Enum teleportType, uint32_t someUint2, MethodInfo* method)
{
    if (teleportStage == 2)
    {
        std::cout << "[TP Stage 2/3] Changing loading location." << std::endl;
        position = teleportPosition;
        teleportStage--;
    }

    callOrigin(DoTeleport_Hook, __this, position, someEnum, someUint1, teleportType, someUint2, method);
}

void Entity_SetPosition_Hook(app::EAPPPCHHMHO* __this, app::Vector3 position, bool someBool, MethodInfo* method)
{
    auto avatarSingleton = GetSingleton(AOFGMGFKONM);
    auto avatarEntity = app::GetAvatar_Entity(avatarSingleton, nullptr);
    if (__this == avatarEntity && teleportStage == 1)
    {
        position = teleportPosition;
        std::cout << "[TP Stage 3/3] Changing avatar entity position." << std::endl;
        std::cout << "[TP] Teleport to mark finished." << std::endl;
        teleportStage--;
    }

    callOrigin(Entity_SetPosition_Hook, __this, position, someBool, method);
}

void InitMapTPHooks() {

    // Teleport to mark hooks
    HookManager::install(app::DoTeleport_ECJDHNFLNAI, DoTeleport_Hook);
    HookManager::install(app::GetWaypointInformation_MBHLOBDPKEC_BFKKBELGMLF, GetWaypointInformation_Hook);
    HookManager::install(app::Entity_SetPosition, Entity_SetPosition_Hook);
    HookManager::install(app::CreateCustomMapMark, CreateCustomMapMark_Hook);

    std::cout << "[TP-to-mark] Hooks installed." << std::endl;
}