#include <pch-il2cpp.h>
#include "cheat.h"

#include <vector>

#include <magic_enum.hpp>

#include <helpers.h>
#include <il2cpp-appdata.h>
#include <common/HookManager.h>
#include <common/Config.h>
#include <common/Logger.h>

static struct WaypointInfo {
    uint32_t groupId;
    uint32_t id;
    app::Vector3 position;
    app::MBHLOBDPKEC_BHKOIJIKDFG* data;
};

static int teleportStage = 0;
static app::Vector3 teleportPosition{};
//static WaypointInfo* targetWaypoint;

static std::vector<WaypointInfo> getUnlockedWaypoints() {
    auto singleton = GetSingleton(MBHLOBDPKEC);
    if (singleton == nullptr)
        return {};

    auto result = std::vector<WaypointInfo>();
    auto waypointGroups = (UniDict<uint32_t, UniDict<uint32_t, app::MBHLOBDPKEC_BHKOIJIKDFG>*>*)singleton->fields.waypoints;
    for (const auto& [groupId, waypoints] : getUniDictPairs(waypointGroups))
    {
        if (groupId != 3)
            continue;

        for (const auto& [id, waypoint] : getUniDictPairs((UniDict<uint32_t, app::MBHLOBDPKEC_BHKOIJIKDFG>*)waypoints))
        {
            if (waypoint.isUnlocked)
                result.push_back(WaypointInfo{ groupId, id, waypoint.waypointLocation->fields.tpPosition, (app::MBHLOBDPKEC_BHKOIJIKDFG*)&waypoint });
        }
    }
    return result;
}

 WaypointInfo FindNearestWaypoint(app::Vector3& position) 
{
    float minDistance = -1;
    WaypointInfo result {};
    for (const auto& info : getUnlockedWaypoints()) {
        float distance = app::Vector3_Distance(nullptr, position, info.position, nullptr);
        if (minDistance < 0 || distance < minDistance)
        {
            minDistance = distance;
            result = info;
        }
    }
    return result;
}

 void CreateCustomMapMark_Hook(void* __this, app::Vector2 position, app::NOIPNNCFAAH__Enum iconType, MethodInfo* method)
{
    if (!Config::cfgTeleportKey.GetValue().IsPressed())
        return callOrigin(CreateCustomMapMark_Hook, __this, position, iconType, method);

    auto worldPosition = app::Miscs_GenWorldPos(nullptr, position, nullptr);

    auto relativePos = app::WorldShiftManager_GetRelativePosition(nullptr, worldPosition, nullptr);
    auto groundHeight = app::Miscs_CalcCurrentGroundHeight(nullptr, relativePos.x, relativePos.z, nullptr);
    
    teleportPosition = app::Vector3();
    teleportPosition.x = worldPosition.x;
    teleportPosition.y = groundHeight > 0 ? groundHeight + 10 : Config::cfgTeleportHeight.GetValue();
    teleportPosition.z = worldPosition.z;
    LOG_DEBUG("Stage 0. Target location at %s", il2cppi_to_string(teleportPosition).c_str());

    auto avatarPosition = app::ActorUtils_GetAvatarPos(nullptr, nullptr);
    if (app::Vector3_Distance(nullptr, avatarPosition, teleportPosition, nullptr) < 150)
    {
        LOG_DEBUG("Stage 0. Distantion < 150m. Use fast tp.");
        app::ActorUtils_SetAvatarPos(nullptr, teleportPosition, nullptr);
        return callOrigin(CreateCustomMapMark_Hook, __this, position, iconType, method);
    }

    auto nearestWaypoint = FindNearestWaypoint(teleportPosition);

    if (nearestWaypoint.data == nullptr) {
        LOG_WARNING("Stage 0. Failed to find nearest unlocked waypoint. Using first unlockable waypoint.");
        nearestWaypoint.groupId = 3;
        nearestWaypoint.id = 6;
    }
    else 
    {
        float dist = app::Vector3_Distance(nullptr, teleportPosition, nearestWaypoint.position, nullptr);
        LOG_DEBUG("Stage 0. Found nearest waypoint { group: %d; id: %d } with distance %fm.", 
            nearestWaypoint.groupId, nearestWaypoint.id, dist);

        nearestWaypoint.data->waypointLocation->fields.tpPosition = teleportPosition;
    }

    //targetWaypoint = new WaypointInfo(nearestWaypoint);
    teleportStage = 2;

    auto someSingleton = GetSingleton(FJIDMGGJMEF);
    app::SendTeleportToWaypoint_FJIDMGGJMEF_CBBKNIJAKDE(someSingleton, nearestWaypoint.groupId, nearestWaypoint.id, nullptr, nullptr);

    return callOrigin(CreateCustomMapMark_Hook, __this, position, iconType, method);
}

static void DoTeleport_Hook(app::FJIDMGGJMEF* __this, app::Vector3 position, app::MEPEDAIKBDI__Enum someEnum,
    uint32_t someUint1, app::CMHGHBNDBMG_ECPNDLCPDIE__Enum teleportType, uint32_t someUint2, MethodInfo* method)
{
    if (teleportStage == 2)
    {
        LOG_DEBUG("Stage 1. Changing loading location.");
        position = teleportPosition;
        teleportStage--;
    }

    callOrigin(DoTeleport_Hook, __this, position, someEnum, someUint1, teleportType, someUint2, method);
}

static void Entity_SetPosition_Hook(app::EAPPPCHHMHO* __this, app::Vector3 position, bool someBool, MethodInfo* method)
{
    auto avatarSingleton = GetSingleton(AOFGMGFKONM);
    auto avatarEntity = app::GetAvatar_Entity(avatarSingleton, nullptr);
    if (__this == avatarEntity && teleportStage == 1)
    {
        position = teleportPosition;
        LOG_DEBUG("Stage 2. Changing avatar entity position.");

        auto relativePos = app::WorldShiftManager_GetRelativePosition(nullptr, position, nullptr);
        auto groundHeight = app::Miscs_CalcCurrentGroundWaterHeight(nullptr, relativePos.x, relativePos.z, nullptr);
        if (groundHeight > 0 && position.y != groundHeight)
        {
            position.y = groundHeight + 10;
            LOG_DEBUG("Stage 2. Changing height to %f", position.y);
        }

        //if (targetWaypoint->data != nullptr) 
        //    targetWaypoint->data->waypointLocation->fields.tpPosition = targetWaypoint->position;
        
        //delete targetWaypoint;

        LOG_DEBUG("Finish.  Teleport to mark finished.");
        teleportStage--;
    }

    callOrigin(Entity_SetPosition_Hook, __this, position, someBool, method);
}

void InitMapTPHooks() {

    // Teleport to mark hooks
    HookManager::install(app::DoTeleport_ECJDHNFLNAI, DoTeleport_Hook);
    LOG_TRACE("Hooked DoTeleport_ECJDHNFLNAI. Origin at 0x%p", HookManager::getOrigin(DoTeleport_Hook));

    HookManager::install(app::Entity_SetPosition, Entity_SetPosition_Hook);
    LOG_TRACE("Hooked Entity_SetPosition. Origin at 0x%p", HookManager::getOrigin(Entity_SetPosition_Hook));

    HookManager::install(app::CreateCustomMapMark, CreateCustomMapMark_Hook);
    LOG_TRACE("Hooked CreateCustomMapMark. Origin at 0x%p", HookManager::getOrigin(CreateCustomMapMark_Hook));

    LOG_DEBUG("Hooks installed");
}