#include "pch-il2cpp.h"
#include "MobAnimalVacuum.h"

#include <helpers.h>
#include <cheat/events.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/util.h>
#include <cheat/game/filters.h>

namespace cheat::feature
{
    MobAnimalVacuum::MobAnimalVacuum() : Feature(),
        NF(m_Enabled, "Mob Animal vacuum", "MobAnimalVacuum", false),
        NF(m_Speed, "Speed", "MobAnimalVacuum", 2.5f),
        NF(m_Distance, "Distance", "MobAnimalVacuum", 1.5f),
        NF(m_Radius, "Radius", "MobAnimalVacuum", 15.0f),
        NF(m_Instantly, "Instantly", "MobAnimalVacuum", true)
    {
        events::GameUpdateEvent += MY_METHOD_HANDLER(MobAnimalVacuum::OnGameUpdate);
        events::MoveSyncEvent += MY_METHOD_HANDLER(MobAnimalVacuum::OnMoveSync);
    }

    const FeatureGUIInfo& MobAnimalVacuum::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "Mob Animal vacuum", "World", true };
        return info;
    }

    void MobAnimalVacuum::DrawMain()
    {
        ConfigWidget("Enabled", m_Enabled, "Enables mob vacuum.\n\
            Mobs within the specified radius will move to the specified distance in front of the player.");
        ConfigWidget(m_Instantly, "Vacuum the Animal Instanly.");
        ConfigWidget(m_Speed, 0.1f, 1.0f, 15.0f, "If 'Instantly' not checked, mob will be moved in balanced specified speed.");
        ConfigWidget(m_Radius, 1, 5, 150, "Radius of Mob Vacuum.");
        ConfigWidget(m_Distance, 0.1f, 0.5f, 10.0f, "Distance between the player and the Animal.");
    }

    bool MobAnimalVacuum::NeedStatusDraw() const
    {
        return m_Enabled;
    }

    void MobAnimalVacuum::DrawStatus()
    {
        ImGui::Text("Mob Animal vacuum [%.01fm]", m_Radius.value());
    }

    MobAnimalVacuum& MobAnimalVacuum::GetInstance()
    {
        static MobAnimalVacuum instance;
        return instance;
    }

    // Check if entity valid for mob vacuum.
    bool MobAnimalVacuum::IsEntityForVac(game::Entity* entity)
    {

        if (!game::filters::combined::Animals.IsValid(entity))
            return false;

        auto& manager = game::EntityManager::instance();
        /*if (m_OnlyTarget)
        {
            auto monsterCombat = entity->combat();
            if (monsterCombat == nullptr || monsterCombat->fields._attackTarget.runtimeID != manager.avatar()->runtimeID())
                return false;
        }*/

        auto distance = manager.avatar()->distance(entity);
        return distance <= m_Radius;
    }

    // Calculate mob vacuum target position.
    app::Vector3 MobAnimalVacuum::CalcMobVacTargetPos()
    {
        auto& manager = game::EntityManager::instance();
        auto avatarEntity = manager.avatar();
        if (avatarEntity == nullptr)
            return {};

        return avatarEntity->relativePosition() + avatarEntity->forward() * m_Distance;
    }

    // Mob vacuum update function.
    // Changes position of monster, if mob vacuum enabled.
    void MobAnimalVacuum::OnGameUpdate()
    {
        static auto positions = new std::map<uint32_t, app::Vector3>();

        if (!m_Enabled)
            return;

        app::Vector3 targetPos = CalcMobVacTargetPos();
        if (IsVectorZero(targetPos))
            return;

        auto& manager = game::EntityManager::instance();
        auto newPositions = new std::map<uint32_t, app::Vector3>();
        for (const auto& animal : manager.entities(game::filters::combined::Animals))
        {
            if (!IsEntityForVac(animal))
                continue;

            if (m_Instantly)
            {
                animal->setRelativePosition(targetPos);
                continue;
            }

            uint32_t animalId = animal->runtimeID();
            app::Vector3 animalRelPos = positions->count(animalId) ? (*positions)[animalId] : animal->relativePosition();
            app::Vector3 newPosition = {};
            if (app::Vector3_Distance(nullptr, animalRelPos, targetPos, nullptr) < 0.1)
            {
                newPosition = targetPos;
            }
            else
            {
                app::Vector3 dir = GetVectorDirection(animalRelPos, targetPos);
                float deltaTime = app::Time_get_deltaTime(nullptr, nullptr);
                newPosition = animalRelPos + dir * m_Speed * deltaTime;
            }

            (*newPositions)[animalId] = newPosition;
            animal->setRelativePosition(newPosition);
        }

        delete positions;
        positions = newPositions;
    }

    // Mob vacuum sync packet replace.
    // Replacing move sync speed and motion state.
    //   Callow: I think it is more safe method, 
    //           because for server monster don't change position m_Instantly.
    void MobAnimalVacuum::OnMoveSync(uint32_t entityId, app::MotionInfo* syncInfo)
    {
        if (!m_Enabled || m_Instantly)
            return;

        auto& manager = game::EntityManager::instance();
        auto entity = manager.entity(entityId);
        if (!IsEntityForVac(entity))
            return;

        app::Vector3 targetPos = CalcMobVacTargetPos();
        app::Vector3 entityPos = entity->relativePosition();
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

