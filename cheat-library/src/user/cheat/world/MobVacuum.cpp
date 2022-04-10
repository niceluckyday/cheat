#include "pch-il2cpp.h"
#include "MobVacuum.h"

#include <helpers.h>
#include <cheat/events.h>
#include <cheat/game.h>

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
            Mobs within the specified radius will move to the specified distance in front of the player.");
        ConfigWidget(m_Instantly, "Vacuum the Enemies Instanly.");
        ConfigWidget(m_Speed, 0.1f, 1.0f, 15.0f, "If 'Instantly' not checked, mob will be moved in balanced specified speed.");
        ConfigWidget(m_Radius, 1, 5, 150, "Radius of Mob Vacuum.");
        ConfigWidget(m_Distance, 0.1f, 0.5f, 10.0f, "Distance between the player and the monster.");
        ConfigWidget(m_OnlyTarget, "Mob vacuum will only work on entities that target the player.");
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
        if (!game::IsEntityFilterValid(entity, game::GetMonsterFilter()))
            return false;

        
        if (m_OnlyTarget)
        {
            auto monsterCombat = app::BaseEntity_GetBaseCombat(entity, *app::BaseEntity_GetBaseCombat__MethodInfo);
            if (monsterCombat == nullptr || monsterCombat->fields._attackTarget.runtimeID != game::GetAvatarRuntimeId())
                return false;
        }

		auto distance = game::GetDistToAvatar(entity);
        return distance <= m_Radius;
    }

    // Calculate mob vacuum target position.
    app::Vector3 MobVacuum::CalcMobVacTargetPos()
    {
        auto avatarEntity = game::GetAvatarEntity();
        if (avatarEntity == nullptr)
            return {};

        auto avatarRelPos = game::GetRelativePosition(avatarEntity);
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
        for (const auto& monster : game::FindEntities(game::GetMonsterFilter()))
        {
            if (!IsEntityForVac(monster))
                continue;

            if (m_Instantly)
            {
                game::SetRelativePosition(monster, targetPos);
                continue;
            }

            uint32_t monsterId = monster->fields._runtimeID_k__BackingField;
            app::Vector3 monsterRelPos = positions->count(monsterId) ? (*positions)[monsterId] : game::GetRelativePosition(monster);
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
            game::SetRelativePosition(monster, newPosition);
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
        app::Vector3 entityPos = game::GetRelativePosition(entity);
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

