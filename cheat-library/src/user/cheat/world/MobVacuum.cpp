#include "pch-il2cpp.h"
#include "MobVacuum.h"

#include <imgui.h>
#include <common/util.h>
#include <helpers.h>
#include <gui/gui-util.h>

#include <cheat/events.h>

namespace cheat::feature 
{
    MobVacuum::MobVacuum() : Feature(),
        NF(m_Enabled,    "Mob vacuum",    "MobVacuum", false),
        NF(m_Speed,      "Speed",         "MobVacuum", 2.5f),
        NF(m_Distance,   "Distance",      "MobVacuum", 1.5f),
        NF(m_Radius,     "Radius",        "MobVacuum", 10.0f),
        NF(m_OnlyTarget, "Only targeted", "MobVacuum", true),
        NF(m_Instantly,  "Instantly",     "MobVacuum", false)
    {
        events::GameUpdateEvent += MY_METHOD_HANDLER(MobVacuum::OnGameUpdate);
        events::MoveSyncEvent += MY_METHOD_HANDLER(MobVacuum::OnMoveSync);
    }

    const FeatureGUIInfo& MobVacuum::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "Mob vacuum", "World", true };
        return info;
    }

    void MobVacuum::DrawMain()
    {
        ConfigWidget("Enabled", m_Enabled, "Enables mob vacuum.\n\
            Mob in specified radius will be moved front of player in specified distance.");
        ConfigWidget(m_Instantly, "Move performs m_Instantly.");
        ConfigWidget(m_Speed, 0.1f, 1.0f, 15.0f, "If 'Instantly' not checked, regulate speed of move.");
        ConfigWidget(m_Radius, 1, 5, 150, "Radius of mob vacuum work area.");
        ConfigWidget(m_Distance, 0.1f, 0.5f, 10.0f, "Distance beetwen player and monster to move.");
        ConfigWidget(m_OnlyTarget, "Mob vacuum will be work only on entity targeted at the player.");
    }

    bool MobVacuum::NeedStatusDraw() const
{
        return m_Enabled;
    }

    void MobVacuum::DrawStatus() 
    { 
        ImGui::Text("Mob vacuum [%.01fm]", m_Radius.value());
    }

    MobVacuum& MobVacuum::GetInstance()
    {
        static MobVacuum instance;
        return instance;
    }

    // Check if entity valid for mob vacuum.
    bool MobVacuum::IsEntityForVac(app::BaseEntity* entity)
    {
        if (!IsEntityFilterValid(entity, GetMonsterFilter()))
            return false;

        if (m_OnlyTarget)
        {
            auto monsterCombat = app::BaseEntity_GetBaseCombat(entity, *app::BaseEntity_GetBaseCombat__MethodInfo);
            if (monsterCombat == nullptr || monsterCombat->fields._attackTarget.runtimeID != GetAvatarRuntimeId())
                return false;
        }

        auto distance = GetDistToAvatar(entity);
        if (distance > m_Radius)
            return false;

        return true;
    }

    // Calculate mob vacuum target position.
    app::Vector3 MobVacuum::CalcMobVacTargetPos()
    {
        auto avatarEntity = GetAvatarEntity();
        if (avatarEntity == nullptr)
            return {};

        auto avatarRelPos = GetRelativePosition(avatarEntity);
        auto avatarForward = app::BaseEntity_GetForward(avatarEntity, nullptr);
        return avatarRelPos + avatarForward * m_Distance;
    }

    // Mob vacuum update function.
    // Changes position of monster, if mob vacuum enabled.
    void MobVacuum::OnGameUpdate()
    {
        static auto positions = new std::map<uint32_t, app::Vector3>();

        if (!m_Enabled)
            return;

        app::Vector3 targetPos = CalcMobVacTargetPos();
        if (IsVectorZero(targetPos))
            return;

        auto newPositions = new std::map<uint32_t, app::Vector3>();
        for (const auto& monster : FindEntities(GetMonsterFilter()))
        {
            if (!IsEntityForVac(monster))
                return;

            if (m_Instantly)
            {
                SetRelativePosition(monster, targetPos);
                continue;
            }

            uint32_t monsterId = monster->fields._runtimeID_k__BackingField;
            app::Vector3 monsterRelPos = positions->count(monsterId) ? (*positions)[monsterId] : GetRelativePosition(monster);
            app::Vector3 newPosition = {};
            if (app::Vector3_Distance(nullptr, monsterRelPos, targetPos, nullptr) < 0.1)
            {
                newPosition = targetPos;
            }
            else
            {
                app::Vector3 dir = GetVectorDirection(monsterRelPos, targetPos);
                float deltaTime = app::Time_get_deltaTime(nullptr, nullptr);
                newPosition = monsterRelPos + dir * m_Speed * deltaTime;
            }

            (*newPositions)[monsterId] = newPosition;
            SetRelativePosition(monster, newPosition);
        }

        delete positions;
        positions = newPositions;
    }

    // Mob vacuum sync packet replace.
    // Replacing move sync speed and motion state.
    //   Callow: I think it is more safe method, 
    //           because for server monster don't change position m_Instantly.
    void MobVacuum::OnMoveSync(uint32_t entityId, app::MotionInfo* syncInfo)
    {
        if (!m_Enabled || m_Instantly)
            return;

        auto entityManager = GetSingleton(EntityManager);
        if (entityManager == nullptr)
            return;

        auto entity = app::EntityManager_GetValidEntity(entityManager, entityId, nullptr);
        if (entity == nullptr)
            return;

        if (!IsEntityForVac(entity))
            return;

        app::Vector3 targetPos = CalcMobVacTargetPos();
        app::Vector3 entityPos = GetRelativePosition(entity);
        if (app::Vector3_Distance(nullptr, targetPos, entityPos, nullptr) < 0.2)
            return;

        app::Vector3 dir = GetVectorDirection(targetPos, entityPos);
        app::Vector3 scaledDir = dir * m_Speed;

        syncInfo->fields.speed_->fields.x = scaledDir.x;
        syncInfo->fields.speed_->fields.y = scaledDir.y;
        syncInfo->fields.speed_->fields.z = scaledDir.z;

        switch (syncInfo->fields.motionState)
        {
        case app::MotionState__Enum::MotionStandby:
        case app::MotionState__Enum::MotionStandbyMove:
        case app::MotionState__Enum::MotionWalk:
        case app::MotionState__Enum::MotionDangerDash:
            syncInfo->fields.motionState = app::MotionState__Enum::MotionRun;
        }
    }
}

