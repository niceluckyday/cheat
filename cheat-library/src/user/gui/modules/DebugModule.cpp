#include "pch-il2cpp.h"
#include "DebugModule.h"

#include <imgui.h>
#include <magic_enum.hpp>

#include <helpers.h>
#include <il2cpp-appdata.h>
#include <gui/gui-util.h>
#include <common/Config.h>
#include <gcclib/Logger.h>
#include <common/HookManager.h>
#include <common/util.h>
#include <cheat/cheat.h>

void DrawWaypoints(UniDict<uint32_t, UniDict<uint32_t, app::MapModule_ScenePointData>*>* waypointsGrops) 
{
    if (waypointsGrops == nullptr)
    {
        ImGui::Text("Waypoint data doesn't exist.");
        return;
    }

    for (const auto& [sceneId, waypoints] : waypointsGrops->pairs())
    {
        if (ImGui::TreeNode(("WTD " + std::to_string(sceneId)).c_str(), "Waypoint group id %d", sceneId))
        {
            for (const auto& [waypointId, waypoint] : waypoints->pairs())
            {
                if (ImGui::TreeNode(("WD " + std::to_string(waypointId)).c_str(), "Waypoint id %d", waypointId))
                {
                    ImGui::Text("IsGroupLimit: %s", waypoint.isGroupLimit ? "true" : "false");
                    ImGui::Text("IsUnlocked: %s", waypoint.isUnlocked ? "true" : "false");
                    
                    if (waypoint.config != nullptr)
                    {
                        auto location = waypoint.config->fields;
                        ImGui::Text("Waypoint type: %s", magic_enum::enum_name(location._type).data());
                        ImGui::Text("Trans position: %s", il2cppi_to_string(location._tranPos).c_str());
                        ImGui::Text("Object position: %s", il2cppi_to_string(location._pos).c_str());
                        ImGui::Text("Is unlocked: %s", location._unlocked ? "true" : "false");
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }
}

void DrawManagerData() 
{
    if (!IsSingletonLoaded(MBHLOBDPKEC))
        return;

    auto singleton = GetSingleton(MBHLOBDPKEC);
    if (singleton == nullptr)
    {
        ImGui::Text("Manager not initialized.");
        return;
    }

    if (ImGui::TreeNode("Waypoints"))
    {
        auto waypoints = ToUniDict(singleton->fields._scenePointDics, uint32_t, UniDict<uint32_t COMMA app::MapModule_ScenePointData>*);
        DrawWaypoints(waypoints);
        ImGui::TreePop();
    }
}

void DrawEntity(struct app::BaseEntity* entity)
{
    if (entity == nullptr)
    {
        ImGui::Text("Entity doesn't exist.");
        return;
    }
    auto fields = entity->fields;
    ImGui::Text("Entity type: %s", magic_enum::enum_name(fields.entityType).data());
    ImGui::Text("Entity shared position: %s", il2cppi_to_string(fields._sharedPosition).c_str());
    ImGui::Text("Entity config id: %d", fields._configID_k__BackingField);
    ImGui::Text("Entity name: %s", il2cppi_to_string(app::BaseEntity_ToStringRelease(entity, nullptr)).c_str());

    if (entity->fields.jsonConfig != nullptr && entity->fields.jsonConfig->fields._entityTags != nullptr)
    {
        auto tagsArray = ToUniArray(entity->fields.jsonConfig->fields._entityTags->fields.KNOAKPHDIIK, app::String*);
        if (tagsArray->length() > 0)
        {
            ImGui::Text("Entity tags:");
            for (auto& tag : *tagsArray) {
                ImGui::Text("%s", il2cppi_to_string(tag).c_str());
            }
        }
    }
}

static void DrawEntitiesData() 
{
    static bool typeFilters[0x63] = {};
    static bool typeFiltersInitialized = false;

    if (!typeFiltersInitialized) {
        std::fill_n(typeFilters, 0x63, true);
        typeFiltersInitialized = true;
    }

    static bool useObjectNameFilter = false;
    static char objectNameFilter[128] = {};

    static bool checkOnlyShells = false;

    if (!IsSingletonLoaded(EntityManager))
        return;

    auto entityManager = GetSingleton(EntityManager);
    if (entityManager == nullptr)
        return;

    auto entities = ToUniList(entityManager->fields._entities, app::BaseEntity*);
    if (entities == nullptr)
        return;

    ImGui::Text("Entity count %d", entities->size);
    
    ImGui::Checkbox("## Enable object name filter", &useObjectNameFilter); ImGui::SameLine();
    if (!useObjectNameFilter)
        ImGui::BeginDisabled();
    ImGui::InputText("Entity name filter", objectNameFilter, 128);
    if (!useObjectNameFilter)
        ImGui::EndDisabled();

    ImGui::Checkbox("Show only oculi", &checkOnlyShells);

    if (ImGui::TreeNode("Type filter"))
    {
        if (ImGui::Button("Select all"))
            std::fill_n(typeFilters, 0x63, true);
        ImGui::SameLine();

        if (ImGui::Button("Deselect all"))
            std::fill_n(typeFilters, 0x63, false);

        int columns = 2;
        if (ImGui::BeginTable("Type filter table", columns, ImGuiTableFlags_NoBordersInBody)) 
        {
            auto entries = magic_enum::enum_entries<app::EntityType__Enum_1>();
            for (const auto& [value, name] : entries)
            {
                ImGui::TableNextColumn();
                ImGui::Checkbox(name.data(), &typeFilters[(int)value]);
            }
            ImGui::EndTable();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Entity list")) 
    {
        for (const auto& entity : *entities) {
            if (entity == nullptr)
                continue;

            if (!typeFilters[(int)entity->fields.entityType])
                continue;
            
            if (checkOnlyShells && !IsEntityCrystalShell(entity))
                continue;

            if (useObjectNameFilter && il2cppi_to_string(app::BaseEntity_ToStringRelease(entity, nullptr)).find(objectNameFilter) == -1)
                continue;

            if (ImGui::TreeNode(&entity, "Entity 0x%p; Dist %.3fm", entity, GetDistToAvatar(entity)))
            {
                if (ImGui::Button("Teleport"))
                {
                    auto rpos = GetRelativePosition(entity);
                    auto apos = app::WorldShiftManager_GetAbsolutePosition(nullptr, rpos, nullptr);
                    TeleportToPosition(apos);
                }

                DrawEntity(entity);
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }
}

#define DRAW_UINT(owner, fieldName) ImGui::Text("%s: %u", #fieldName, owner##->fields.##fieldName );
#define DRAW_FLOAT(owner, fieldName) ImGui::Text("%s: %f", #fieldName, owner##->fields.##fieldName );
#define DRAW_BOOL(owner, fieldName) ImGui::Text("%s: %s", #fieldName, owner##->fields.##fieldName ? "true" : "false");

static void DrawBaseInteraction(app::BaseInterAction* inter)
{
    ImGui::Text("_type: %s", magic_enum::enum_name(inter->fields._type).data());
    DRAW_UINT(inter, _mainQuestId);
    DRAW_BOOL(inter, _isFromExternal);
    DRAW_BOOL(inter, _isStarted);
    DRAW_BOOL(inter, _isFinished);
    auto cfg = inter->fields._cfg;
    if (cfg == nullptr)
        return;

    ImGui::Text("Config: ");
    ImGui::Text("_type: %s", magic_enum::enum_name(cfg->fields._type).data());
    DRAW_FLOAT(cfg, _delayTime);
    DRAW_FLOAT(cfg, _duration);
    DRAW_FLOAT(cfg, _checkNextImmediately);
}

static void DrawInteractionManagerInfo() 
{
    if (!IsSingletonLoaded(InteractionManager))
    {
        ImGui::Text("Manager not loaded.");
        return;
    }
    auto interactionManager = GetSingleton(InteractionManager);

    DRAW_UINT(interactionManager, _keyInterCnt);
    DRAW_FLOAT(interactionManager, _endFadeInTime);
    DRAW_FLOAT(interactionManager, _endFadeOutTime);
    DRAW_BOOL(interactionManager, _hasKeyPre);
    DRAW_BOOL(interactionManager, _havEndFade);
    DRAW_BOOL(interactionManager, _inEndFade);
    DRAW_BOOL(interactionManager, _inStartFade);
    DRAW_BOOL(interactionManager, _talkLoading);
    DRAW_BOOL(interactionManager, _voiceLoading);
    DRAW_BOOL(interactionManager, _isLockGameTime);
    DRAW_BOOL(interactionManager, _isInteeReadyChecking);
    DRAW_BOOL(interactionManager, _isDelayClear);
    DRAW_BOOL(interactionManager, _isFromPerformConfig);
    DRAW_BOOL(interactionManager, _edtTalkWaiting);
    DRAW_BOOL(interactionManager, _isManulAttackMode);
    DRAW_BOOL(interactionManager, _canShowAvatarEffectWhenTalkStart);


    auto keyList = ToUniLinkList(interactionManager->fields._keyInterList, app::InterActionGrp*);
    if (keyList != nullptr && ImGui::TreeNode("KeyList")) 
    {
        auto reminder = keyList->count;
        auto current = keyList->first;
        while (reminder > 0 && current != nullptr) 
        {
            auto item = current->item;
            if (ImGui::TreeNode(item, "Key item: gid %d", item->fields.groupId)) 
            {
                DRAW_UINT(item, groupId);
                DRAW_UINT(item, nextGroupId);
                DRAW_BOOL(item, isKeyList);
                DRAW_BOOL(item, _isStarted);

                if (item->fields._interActionList != nullptr && ImGui::TreeNode("Interactions"))
                {
                    auto interactions = ToUniList(item->fields._interActionList, app::BaseInterAction*);
                    for (auto& interaction : *interactions) 
                    {
                        if (interaction == nullptr)
                            continue;

                        if (ImGui::TreeNode(interaction, "Base interaction"))
                        {
                            DrawBaseInteraction(interaction);
                            ImGui::TreePop();
                        }
                    }
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            current = current->forward;
            reminder--;
        }
        ImGui::TreePop();

    }
}

#undef DRAW_UINT
#undef DRAW_FLOAT
#undef DRAW_BOOL

void DrawPositionInfo() 
{
    auto avatarPos = app::ActorUtils_GetAvatarPos(nullptr, nullptr);
    ImGui::Text("Avatar position: %s", il2cppi_to_string(avatarPos).c_str());

    auto relativePos = app::WorldShiftManager_GetRelativePosition(nullptr, avatarPos, nullptr);
    ImGui::Text("Relative position: %s", il2cppi_to_string(relativePos).c_str());

    auto levelPos = app::Miscs_GenLevelPos_1(nullptr, avatarPos, nullptr);
    ImGui::Text("Level position: %s", il2cppi_to_string(levelPos).c_str());


    if (ImGui::TreeNode("Ground pos info"))
    {
        auto groundNormal = app::Miscs_CalcCurrentGroundNorm(nullptr, avatarPos, nullptr);
        ImGui::Text("Ground normal: %s", il2cppi_to_string(groundNormal).c_str());

        static app::Vector3 pos{};
        static bool fixedToPos;
        ImGui::Checkbox("## Fixed to position", &fixedToPos); ImGui::SameLine();
        if (fixedToPos) {
            pos = relativePos;
            pos.y = 1000;
        }

        ImGui::DragFloat3("Checked pos", (float*)&pos, 1.0f, -4000.0f, 4000.0f);

        static float length = 1000;
        ImGui::DragFloat("Raycast length", &length, 1.0f, -2000.0f, 2000.0f);

        ImGui::Text("All: %f", app::Miscs_CalcCurrentGroundHeight_1(nullptr, avatarPos.x, avatarPos.z, avatarPos.y, length, 0xFFFFFFFF, nullptr));

        ImGui::TreePop();
    }
}

void DebugModule::Draw()
{
    if (ImGui::CollapsingHeader("Entity manager", ImGuiTreeNodeFlags_None))
        DrawEntitiesData();

    if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_None))
        DrawPositionInfo();

    if (ImGui::CollapsingHeader("Interaction manager", ImGuiTreeNodeFlags_None))
        DrawInteractionManagerInfo();
    
    if (ImGui::CollapsingHeader("Map manager", ImGuiTreeNodeFlags_None))
        DrawManagerData();
}

std::string DebugModule::GetName()
{
    return "Debug";
}