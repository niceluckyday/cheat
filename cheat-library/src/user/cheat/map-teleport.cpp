#include <pch-il2cpp.h>
#include "cheat.h"

#include <vector>

#include <magic_enum.hpp>

#include <helpers.h>
#include <il2cpp-appdata.h>
#include <common/HookManager.h>
#include <common/Config.h>
#include <common/Logger.h>
#include <common/GlobalEvents.h>
#include <common/util.h>

struct WaypointInfo {
    uint32_t sceneId;
    uint32_t waypointId;
    app::Vector3 position;
app::MapModule_ScenePointData* data;
};

struct TeleportTaskInfo 
{
    bool waitingThread = false;
    bool needHeightCalculation = false;
    int currentStage = 0;
    app::Vector3 targetPosition;
    uint32_t sceneId;
    uint32_t waypointId;
};

TeleportTaskInfo taskInfo;
//static WaypointInfo* targetWaypoint;

static std::vector<WaypointInfo> getUnlockedWaypoints() 
{    
    if (!IsSingletonLoaded(MBHLOBDPKEC))
        return {};

    auto singleton = GetSingleton(MBHLOBDPKEC);
    if (singleton == nullptr)
        return {};

    auto result = std::vector<WaypointInfo>();

    auto waypointGroups = GetUniDict(singleton->fields._scenePointDics, uint32_t, UniDict<uint32_t COMMA app::MapModule_ScenePointData>*);
    for (const auto& [sceneId, waypoints] : waypointGroups->pairs())
    {
        if (sceneId != 3)
            continue;

        for (const auto& [waypointId, waypoint] : waypoints->pairs())
        {
            if (waypoint.isUnlocked)
                result.push_back(WaypointInfo{ sceneId, waypointId, waypoint.config->fields._tranPos, (app::MapModule_ScenePointData*)&waypoint });
        }
    }
    return result;
}

static WaypointInfo FindNearestWaypoint(app::Vector3& position)
{
    float minDistance = -1;
    WaypointInfo result{};
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

static void CreateCustomMapMark_Hook(void* __this, app::Vector2 position, app::NOIPNNCFAAH__Enum iconType, MethodInfo* method)
{
    if (!Config::cfgMapTPEnable.GetValue() || !Config::cfgTeleportKey.GetValue().IsPressed())
        return callOrigin(CreateCustomMapMark_Hook, __this, position, iconType, method);

    auto worldPosition = app::Miscs_GenWorldPos(nullptr, position, nullptr);

    auto relativePos = app::WorldShiftManager_GetRelativePosition(nullptr, worldPosition, nullptr);
    auto groundHeight = app::Miscs_CalcCurrentGroundHeight(nullptr, relativePos.x, relativePos.z, nullptr);

    TeleportToPosition({ worldPosition.x, groundHeight > 0 ? groundHeight + 5 : Config::cfgTeleportHeight.GetValue(), worldPosition.z }, true);

    return callOrigin(CreateCustomMapMark_Hook, __this, position, iconType, method);
}

static void GameManager_Update_Hook(app::GameManager* __this, MethodInfo* method) 
{
    if (taskInfo.waitingThread)
    {
        taskInfo.waitingThread = false;
        auto someSingleton = GetSingleton(LoadingManager);
        app::LoadingManager_RequestSceneTransToPoint(someSingleton, taskInfo.sceneId, taskInfo.waypointId, nullptr, nullptr);
    }
    callOrigin(GameManager_Update_Hook, __this, method);
}

void TeleportToPosition(app::Vector3 position, bool calcHeight) {
    LOG_DEBUG("Stage 0. Target location at %s", il2cppi_to_string(position).c_str());

    auto avatarPosition = app::ActorUtils_GetAvatarPos(nullptr, nullptr);
    auto nearestWaypoint = FindNearestWaypoint(position);

    if (nearestWaypoint.data == nullptr)
    {
        LOG_WARNING("Stage 0. Failed to find nearest unlocked waypoint. Using first unlockable waypoint.");
        nearestWaypoint.sceneId = 3;
        nearestWaypoint.waypointId = 6;
    }
    else
    {
        float dist = app::Vector3_Distance(nullptr, position, nearestWaypoint.position, nullptr);
        LOG_DEBUG("Stage 0. Found nearest waypoint { sceneId: %d; waypointId: %d } with distance %fm.",
            nearestWaypoint.sceneId, nearestWaypoint.waypointId, dist);
    }
    taskInfo = { true, calcHeight, 3, position, nearestWaypoint.sceneId, nearestWaypoint.waypointId };
}

static app::Vector3 LocalEntityInfoData_GetTargetPos_Hook(app::LocalEntityInfoData* __this, MethodInfo* method)
{
    auto result = callOrigin(LocalEntityInfoData_GetTargetPos_Hook, __this, method);
    if (taskInfo.currentStage == 3)
    {
        result = taskInfo.targetPosition;
        taskInfo.currentStage--;
        LOG_DEBUG("Stage 1. Replace waypoint tp position.");
    }
    return result;
}

static bool LoadingManager_IsFarTeleport_Hook(app::LoadingManager* __this, uint32_t sceneId, app::Vector3 position, MethodInfo* method)
{
    auto result = callOrigin(LoadingManager_IsFarTeleport_Hook, __this, sceneId, position, method);
    if (!result && taskInfo.currentStage == 2)
    {
        LOG_DEBUG("Stage 1. Distance <60m. Performing fast tp.");
        taskInfo.currentStage = 0;
    }
    return result;
}

static void DoTeleport_Hook(app::LoadingManager* __this, app::Vector3 position, app::EnterType__Enum someEnum,
    uint32_t someUint1, app::CMHGHBNDBMG_ECPNDLCPDIE__Enum teleportType, uint32_t someUint2, MethodInfo* method)
{
    if (taskInfo.currentStage == 2)
    {
        LOG_DEBUG("Stage 2. Changing loading location.");
        position = taskInfo.targetPosition;
        taskInfo.currentStage--;
    }

    callOrigin(DoTeleport_Hook, __this, position, someEnum, someUint1, teleportType, someUint2, method);
}

static void Entity_SetPosition_Hook(app::BaseEntity* __this, app::Vector3 position, bool someBool, MethodInfo* method)
{
    auto entityManager = GetSingleton(EntityManager);
    auto avatarEntity = app::EntityManager_GetCurrentAvatar(entityManager, nullptr);
    if (__this == avatarEntity && taskInfo.currentStage == 1)
    {
        position = taskInfo.targetPosition;
        LOG_DEBUG("Stage 3. Changing avatar entity position.");

        if (taskInfo.needHeightCalculation)
        {
            auto relativePos = app::WorldShiftManager_GetRelativePosition(nullptr, position, nullptr);
            auto groundHeight = app::Miscs_CalcCurrentGroundWaterHeight(nullptr, relativePos.x, relativePos.z, nullptr);
            if (groundHeight > 0 && position.y != groundHeight)
            {
                position.y = groundHeight + 5;
                LOG_DEBUG("Stage 3. Changing height to %f", position.y);
            }
        }

        LOG_DEBUG("Finish.  Teleport to mark finished.");
        taskInfo.currentStage--;
    }

    callOrigin(Entity_SetPosition_Hook, __this, position, someBool, method);
}


static void OnKeyUp(short key, bool& cancelled)
{
    if (Config::cfgCheatWindowShowed.GetValue())
        return;

    if (Config::cfgTeleportToOculi.GetValue().IsPressed(key)) 
    {
        auto entity = FindNearestEntity(GetFilterCrystalShell());
        if (entity != nullptr)
        {
            auto relPos = GetRelativePosition(entity);
            auto absPos = app::WorldShiftManager_GetAbsolutePosition(nullptr, relPos, nullptr);
            TeleportToPosition(absPos);
        }
    }

    if (Config::cfgTeleportToChest.GetValue().IsPressed(key))
    {
        auto entity = FindNearestEntity(GetFilterChest());
        if (entity != nullptr)
        {
            auto relPos = GetRelativePosition(entity);
            auto absPos = app::WorldShiftManager_GetAbsolutePosition(nullptr, relPos, nullptr);
            TeleportToPosition(absPos);
        }
    }
}

void InitMapTPHooks() {

    HookManager::install(app::CreateCustomMapMark, CreateCustomMapMark_Hook);
    LOG_TRACE("Hooked CreateCustomMapMark. Origin at 0x%p", HookManager::getOrigin(CreateCustomMapMark_Hook));
    
    HookManager::install(app::GameManager_Update, GameManager_Update_Hook);
    LOG_TRACE("Hooked GameManager_Update. Origin at 0x%p", HookManager::getOrigin(GameManager_Update_Hook));

    // Stage 1
    HookManager::install(app::LocalEntityInfoData_GetTargetPos, LocalEntityInfoData_GetTargetPos_Hook);
    LOG_TRACE("Hooked LocalEntityInfoData_GetTargetPos. Origin at 0x%p", HookManager::getOrigin(LocalEntityInfoData_GetTargetPos_Hook));

    HookManager::install(app::LoadingManager_IsFarTeleport, LoadingManager_IsFarTeleport_Hook);
    LOG_TRACE("Hooked LoadingManager_IsFarTeleport. Origin at 0x%p", HookManager::getOrigin(LoadingManager_IsFarTeleport_Hook));

    // Stage 2
    HookManager::install(app::LoadingManager_PerformPlayerTransmit, DoTeleport_Hook);
    LOG_TRACE("Hooked DoTeleport_ECJDHNFLNAI. Origin at 0x%p", HookManager::getOrigin(DoTeleport_Hook));

    // Stage 3
    HookManager::install(app::Entity_SetPosition, Entity_SetPosition_Hook);
    LOG_TRACE("Hooked Entity_SetPosition. Origin at 0x%p", HookManager::getOrigin(Entity_SetPosition_Hook));

    GlobalEvents::KeyUpEvent += FREE_METHOD_HANDLER(OnKeyUp);
    
    LOG_DEBUG("Hooks installed");
}