#include "pch-il2cpp.h"
#include "DebugModule.h"

#include <imgui.h>
#include <magic_enum.hpp>

#include <il2cpp-appdata.h>
#include <helpers.h>

#include <gui/gui-util.h>
#include <util/Config.h>
#include <util/Logger.h>

void DrawEntity(struct app::EAPPPCHHMHO* entity) 
{
    if (entity == nullptr) 
    {
        ImGui::Text("Entity doesn't exist.");
        return;
    }
    auto fields = entity->fields;
    ImGui::Text("Entity type: %s", magic_enum::enum_name(fields.entityType).data());
    ImGui::Text("Entity vec1: %s", il2cppi_to_string(fields.vec1).c_str());
    ImGui::Text("Entity vec2: %s", il2cppi_to_string(fields.vec2).c_str());
    ImGui::Text("Entity vec3: %s", il2cppi_to_string(fields.vec3).c_str());
    ImGui::Text("Entity vec4: %s", il2cppi_to_string(fields.vec4).c_str());
    ImGui::Text("Entity some string: %s", il2cppi_to_string(fields.someNameMaybe).c_str());
}

void DrawWaypoints(UniDict<uint32_t, UniDict<uint32_t, app::MBHLOBDPKEC_BHKOIJIKDFG>*>* waypoints) 
{
    if (waypoints == nullptr)
    {
        ImGui::Text("Waypoint data doesn't exist.");
        return;
    }

    for (const auto& wTypeDataPair : getUniDictPairs(waypoints))
    {
        if (ImGui::TreeNode(("WTD " + std::to_string(wTypeDataPair.first)).c_str(), "Waypoint type %d", wTypeDataPair.first))
        {

            for (const auto& wDataPair : getUniDictPairs((UniDict<uint32_t, app::MBHLOBDPKEC_BHKOIJIKDFG>*)wTypeDataPair.second))
            {
                if (ImGui::TreeNode(("WD " + std::to_string(wDataPair.first)).c_str(), "Waypoint id %d", wDataPair.first))
                {
                    auto wData = wDataPair.second;
                    ImGui::Text("IsForbidden: %s", wData.isForbidden ? "true" : "false");
                    ImGui::Text("IsUnlocked: %s", wData.isUnlocked ? "true" : "false");

                    auto location = wData.waypointLocation;
                    if (location != nullptr) 
                    {
                        ImGui::Text("Waypoint type: %s", magic_enum::enum_name(location->fields.JAAAEGMMPIF).data());
                        ImGui::Text("Waypoint position: %s", il2cppi_to_string(location->fields.tpPosition).c_str());
                        ImGui::Text("Vector1: %s", il2cppi_to_string(location->fields.vector1).c_str());
                        ImGui::Text("Vector2: %s", il2cppi_to_string(location->fields.vector2).c_str());
                        ImGui::Text("Vector4: %s", il2cppi_to_string(location->fields.vector4).c_str());
                        ImGui::Text("Some string: %s", il2cppi_to_string(location->fields.someString).c_str());
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }
}

void DrawAreaInfo(app::BKNNJMLFFMG* areaInfo) 
{
    if (areaInfo == nullptr)
    {
        ImGui::Text("Effect is not initilized.");
        return;
    }

    ImGui::Text("Effect type: %s", magic_enum::enum_name(areaInfo->fields.effectType).data());
    ImGui::Text("Level: %s", magic_enum::enum_name(areaInfo->fields.areaTerrain).data());
    ImGui::Text("Terrain type: %s", magic_enum::enum_name(areaInfo->fields.level).data());
}

void DrawManagerData() {
    auto singleton = GetSingleton(MBHLOBDPKEC);
    if (singleton == nullptr)
    {
        ImGui::Text("Manager not initialized.");
        return;
    }

    ImGui::Text("Some string1: %s", il2cppi_to_string(singleton->fields.string1));
    ImGui::Text("Some string2: %s", il2cppi_to_string(singleton->fields.string2));

    if (ImGui::CollapsingHeader("Area info", ImGuiTreeNodeFlags_None)) 
    {
        DrawAreaInfo(singleton->fields.areaInfo);
    }

    if (ImGui::CollapsingHeader("Waypoints", ImGuiTreeNodeFlags_None))
    {
        auto waypoints = (UniDict<uint32_t, UniDict<uint32_t, app::MBHLOBDPKEC_BHKOIJIKDFG>*>*)singleton->fields.waypoints;
        DrawWaypoints(waypoints);
    }

    if (ImGui::CollapsingHeader("Entity")) 
    {
        DrawEntity(singleton->fields.someEntity);
        if (ImGui::CollapsingHeader("dict[uint32_t -> Entity]")) 
        {
            auto dict = (UniDict<int32_t, app::EAPPPCHHMHO*>*)singleton->fields.someEntities;
            for (const auto& entityPair : getUniDictPairs(dict)) 
            {
                if (ImGui::TreeNode((std::to_string(entityPair.first) + " entity").c_str(), "%d -> Entity 0x%p", entityPair.first, entityPair.second)) 
                {
                    DrawEntity(entityPair.second);
                    ImGui::TreePop();
                }
            }
        }

        if (ImGui::CollapsingHeader("dict[String -> Entity]")) {
            auto dict = (UniDict<app::String*, app::EAPPPCHHMHO*>*)singleton->fields.someEntities;
            for (const auto& entityPair : getUniDictPairs(dict))
            {
                auto key = il2cppi_to_string(entityPair.first);
                if (ImGui::TreeNode((key + " entity").c_str(), "%s -> Entity 0x%p", key.c_str(), entityPair.second))
                {
                    DrawEntity(entityPair.second);
                    ImGui::TreePop();
                }
            }
        }
    }
}


void DebugModule::Draw()
{
    if (ImGui::CollapsingHeader("Manager", ImGuiTreeNodeFlags_None))
    {
        DrawManagerData();
    }
    if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_None)) {

        
        auto avatarPos = app::ActorUtils_GetAvatarPos(nullptr, nullptr);
        ImGui::Text("Avatar position: %s", il2cppi_to_string(avatarPos).c_str());

        auto relativePos = app::WorldShiftManager_GetRelativePosition(nullptr, avatarPos, nullptr);
        ImGui::Text("Relative position: %s", il2cppi_to_string(relativePos).c_str());

        auto levelPos = app::Miscs_GenLevelPos_1(nullptr, avatarPos, nullptr);
        ImGui::Text("Level position: %s", il2cppi_to_string(levelPos).c_str());


        if (ImGui::TreeNode("Ground Info"))
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



        //DO_APP_FUNC(0x014026D0, Vector3, Miscs_CalcCurrentGroundNorm, (void* __this, Vector3 pos, MethodInfo * method));
    }

}

std::string DebugModule::GetName()
{
    return "Debug";
}