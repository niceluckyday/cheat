#include "pch-il2cpp.h"
#include "DebugModule.h"

#include <imgui.h>
#include <magic_enum.hpp>

#include <helpers.h>
#include <il2cpp-appdata.h>
#include <gui/gui-util.h>
#include <common/Config.h>
#include <common/Logger.h>
#include <common/HookManager.h>
#include <common/util.h>
#include <cheat/cheat.h>

static app::AttackResult_1__Fields attackResult {};
static app::AttackResult_1__Fields targetResult {};

#define TOGGLEFIELD(field) static bool field ##_toggle = false
TOGGLEFIELD(attackerId_);
TOGGLEFIELD(defenseId_);
TOGGLEFIELD(animEventId_);
TOGGLEFIELD(abilityIdentifier_);
TOGGLEFIELD(damage_);
TOGGLEFIELD(isCrit_);
TOGGLEFIELD(hitPosType_);
TOGGLEFIELD(endureBreak_);
TOGGLEFIELD(resolvedDir_);
TOGGLEFIELD(hitRetreatAngleCompat_);
TOGGLEFIELD(hitEffResult_);
TOGGLEFIELD(elementType_);
TOGGLEFIELD(useGadgetDamageAction_);
TOGGLEFIELD(gadgetDamageActionIdx_);
TOGGLEFIELD(isResistText_);
TOGGLEFIELD(criticalRand_);
TOGGLEFIELD(elementAmplifyRate_);
TOGGLEFIELD(damageShield_);
TOGGLEFIELD(muteElementHurt_);
TOGGLEFIELD(amplifyReactionType_);
TOGGLEFIELD(addhurtReactionType_);
TOGGLEFIELD(bulletFlyTimeMs_);
TOGGLEFIELD(attackCount_);
TOGGLEFIELD(hashedAnimEventId_);
TOGGLEFIELD(attackTimestampMs_);
TOGGLEFIELD(endureDelta_);
TOGGLEFIELD(targetType_);
TOGGLEFIELD(elementDurabilityAttenuation_);
TOGGLEFIELD(HGENJBMMFDH);
#undef TOGGLEFIELD

static app::AttackResult_1* AttackResult_FillProtoAttackResult_Hook(app::AttackResult* __this, uint32_t attackerID, uint32_t defenseId, app::AttackResult_1* protoAttackResult, MethodInfo* method)
{
    auto result = callOrigin(AttackResult_FillProtoAttackResult_Hook, __this, attackerID, defenseId, protoAttackResult, method);
    // LOG_DEBUG("Proto attack event {attacker id: %d, defencer id: %d}", result->fields.attackerId_, result->fields.defenseId_);

    auto fields = result->fields;

#define CHANGE_FIELD(field) attackResult.## field ## = fields.## field ##; if (field ##_toggle) result->fields.## field ## = targetResult.## field
    CHANGE_FIELD(attackerId_);
    CHANGE_FIELD(defenseId_);
    CHANGE_FIELD(animEventId_);
    CHANGE_FIELD(abilityIdentifier_);
    CHANGE_FIELD(damage_);
    CHANGE_FIELD(isCrit_);
    CHANGE_FIELD(hitPosType_);
    CHANGE_FIELD(endureBreak_);
    CHANGE_FIELD(hitRetreatAngleCompat_);
    CHANGE_FIELD(hitEffResult_);
    CHANGE_FIELD(elementType_);
    CHANGE_FIELD(useGadgetDamageAction_);
    CHANGE_FIELD(gadgetDamageActionIdx_);
    CHANGE_FIELD(isResistText_);
    CHANGE_FIELD(criticalRand_);
    CHANGE_FIELD(elementAmplifyRate_);
    CHANGE_FIELD(damageShield_);
    CHANGE_FIELD(muteElementHurt_);
    CHANGE_FIELD(amplifyReactionType_);
    CHANGE_FIELD(addhurtReactionType_);
    CHANGE_FIELD(bulletFlyTimeMs_);
    CHANGE_FIELD(attackCount_);
    CHANGE_FIELD(hashedAnimEventId_);
    CHANGE_FIELD(attackTimestampMs_);
    CHANGE_FIELD(endureDelta_);
    CHANGE_FIELD(targetType_);
    CHANGE_FIELD(elementDurabilityAttenuation_);
    CHANGE_FIELD(HGENJBMMFDH);
#undef CHANGE_FIELD

    // LOG_DEBUG("after Proto attack event {attacker id: %d, defencer id: %d}", result->fields.attackerId_, result->fields.defenseId_);
    return result;
}

void DrawAttackInfo() 
{
#define drawField(field, imguistring) ImGui::Checkbox("## toggle_"#field, &field ##_toggle); ImGui::SameLine(); ImGui::PushItemWidth(100); imguistring; ImGui::PopItemWidth()
#define drawBool(field) drawField(field, ImGui::Checkbox("## "#field, &targetResult.## field)); ImGui::SameLine(); ImGui::Text("%s: %s", #field, attackResult.## field ? "true" : "false" )
// #define drawString(field) { char buffer[256]; drawField(field, ImGui::InputText(#field, buffer.c_str(), temp.size())); auto newString = new app::String(); }
#define drawUInt(field) drawField(field, ImGui::DragInt("## "#field, (int*)&targetResult.## field)); ImGui::SameLine(); ImGui::Text("%s: %d", #field, attackResult.## field)
#define drawFloat(field) drawField(field, ImGui::DragFloat("## "#field, &targetResult.## field)); ImGui::SameLine(); ImGui::Text("%s: %f", #field, attackResult.## field)
#define drawVector(field) if (attackResult.## field ## != nullptr) ImGui::Text("%s: %f %f %f", #field, attackResult.## field ##->fields.x, attackResult.## field ##->fields.y, attackResult.## field ##->fields.z)

    drawUInt(attackerId_);
    drawUInt(defenseId_);
    ImGui::Text("animEventId_: %s", il2cppi_to_string(attackResult.animEventId_).c_str());
    drawFloat(damage_);
    drawBool(isCrit_);
    drawBool(useGadgetDamageAction_);
    drawBool(isResistText_);
    drawUInt(hitPosType_);
    drawUInt(endureBreak_);
    drawVector(resolvedDir_);
    drawUInt(endureBreak_);
    drawUInt(hitRetreatAngleCompat_);
    drawUInt(elementType_);
    drawUInt(gadgetDamageActionIdx_);
    drawUInt(criticalRand_);
    drawFloat(elementAmplifyRate_);
    drawFloat(elementAmplifyRate_);
    drawBool(muteElementHurt_);
    drawUInt(amplifyReactionType_);
    drawUInt(addhurtReactionType_);
    drawUInt(bulletFlyTimeMs_);
    drawUInt(attackCount_);
    drawUInt(hashedAnimEventId_);
    drawUInt(attackTimestampMs_);
    drawFloat(endureDelta_);
    drawUInt(targetType_);
    drawFloat(elementDurabilityAttenuation_);
    drawUInt(HGENJBMMFDH);
#undef drawField
#undef drawBool
#undef drawUInt
#undef drawFloat
#undef drawVector
}

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

void DrawManagerData() {

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
        auto waypoints = GetUniDict(singleton->fields._scenePointDics, uint32_t, UniDict<uint32_t COMMA app::MapModule_ScenePointData>*);
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
        auto tagsArray = GetUniArray(entity->fields.jsonConfig->fields._entityTags->fields.KNOAKPHDIIK, app::String*);
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

    auto entities = GetUniList(entityManager->fields._entities, app::BaseEntity*);
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

#define ShowGroundHeight(method) ImGui::Text("%s (%d): %f", #method, app::Miscs_Get## method ##(nullptr, nullptr), app::Miscs_CalcCurrentGroundHeight_1(nullptr, pos.x, pos.z, pos.y, length, app::Miscs_Get## method ##(nullptr, nullptr), nullptr));

        auto vector3 = app::Vector3_get_down(nullptr, nullptr);
        ImGui::Text("Direction: %s", il2cppi_to_string(vector3).c_str());

        bool raycastResult = app::Physics_Raycast_3(nullptr, pos, vector3, length, nullptr);
        ImGui::Text("Raycast result: %s", raycastResult ? "true" : "false");

        ShowGroundHeight(HitLayerMask);
        ShowGroundHeight(SceneLayerMask);
        ShowGroundHeight(SceneLayerMaskWithBothDynamicBarrierLayer);
        ShowGroundHeight(SceneCameraLayerMask);
        ShowGroundHeight(StaticSceneLayerMask);
        ShowGroundHeight(HitSceneLayerMask);
        ShowGroundHeight(OccluderLayerMask);
        ShowGroundHeight(WaterLayerMask);
        ShowGroundHeight(SceneGroundLayerMask);
        ShowGroundHeight(SceneGroundLayerMaskWithoutTemp);
        ShowGroundHeight(CameraLayerMask);
        ShowGroundHeight(IKSceneLayerMask);
        ShowGroundHeight(CharacterLayerMask);
        ShowGroundHeight(BulletHitLayerMask);
        ShowGroundHeight(DynamicBarrierLayerMask);


        ImGui::Text("All: %f", app::Miscs_CalcCurrentGroundHeight_1(nullptr, avatarPos.x, avatarPos.z, avatarPos.y, length, 0xFFFFFFFF, nullptr));

#undef ShowGroundHeight

        ImGui::TreePop();
    }
}

void DebugModule::Draw()
{
    if (ImGui::CollapsingHeader("Entity manager", ImGuiTreeNodeFlags_None)) 
        DrawEntitiesData();
    
    if (ImGui::CollapsingHeader("Map manager", ImGuiTreeNodeFlags_None))
        DrawManagerData();

    if (ImGui::CollapsingHeader("Attack info", ImGuiTreeNodeFlags_None))
        DrawAttackInfo();

    if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_None)) 
        DrawPositionInfo();

}

std::string DebugModule::GetName()
{
    return "Debug";
}

DebugModule::DebugModule() 
{
    LOG_DEBUG("Hook initialized"); 
    HookManager::install(app::AttackResult_FillProtoAttackResult, AttackResult_FillProtoAttackResult_Hook);
}