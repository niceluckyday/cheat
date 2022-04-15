#include "pch-il2cpp.h"
#include "Debug.h"

#include <misc/cpp/imgui_stdlib.h>
#include <filesystem>
#include <fstream>

#include <cheat/events.h>
#include <cheat/teleport/MapTeleport.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/util.h>
#include <cheat/game/filters.h>
#include <cheat/esp/ESPRender.h>
#include <cheat/game/CacheFilterExecutor.h>
#include <cheat-base/render/renderer.h>
#include <helpers.h>

// This module is for debug purpose, and... well.. it's shit coded ^)
namespace cheat::feature 
{
    static bool ActorAbilityPlugin_OnEvent_Hook(void* __this, app::BaseEvent* e, MethodInfo* method);
    void OnGameUpdate();

	Debug::Debug() : Feature()
	{
        events::GameUpdateEvent += FREE_METHOD_HANDLER(OnGameUpdate);
		HookManager::install(app::ActorAbilityPlugin_OnEvent, ActorAbilityPlugin_OnEvent_Hook);
		// HookManager::install(app::LuaShellManager_ReportLuaShellResult, LuaShellManager_ReportLuaShellResult_Hook);
		// HookManager::install(app::LuaShellManager_DoString, LuaShellManager_DoString_Hook);
		// HookManager::install(app::LuaEnv_DoString, LuaEnv_DoString_Hook);
		// HookManager::install(app::Lua_xlua_pushasciistring, Lua_xlua_pushasciistring_Hook);

		// HookManager::install(app::GameLogin_SendInfo_2, SendInfo_Hook);
		// LOG_DEBUG("Hooked GameLogin::SendGameInfo. Origin at 0x%p", HookManager::getOrigin(SendInfo_Hook));
	}

	const FeatureGUIInfo& Debug::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ "DebugInfo", "Debug", false };
		return info;
	}

	Debug& Debug::GetInstance()
	{
		static Debug instance;
		return instance;
	}


    // Raise when player start game log in (after press a door)
    // Contains information about player system and game integrity
    static void SendInfo_Hook(app::NetworkManager_1* __this, app::GKOJAICIOPA* info, MethodInfo* method)
    {
        LOG_TRACE("Game sending game info to server.");
        LOG_TRACE("Content: ");

#define printString(i) if (info->fields.string_ ## i > (void *)1 && info->fields.string_ ## i ##->fields.length > 0)\
    LOG_TRACE("\tfield#%d: %s", i ,il2cppi_to_string(info->fields.string_ ## i).c_str());

        printString(1);
        printString(2);
        printString(3);
        printString(4);
        printString(5);
        printString(6);
        printString(7);
        printString(8);
        printString(9);
        printString(10);
        printString(11);
        printString(12);
        printString(13);
        printString(14);
        printString(15);
        printString(16);

#undef printString

        callOrigin(SendInfo_Hook, __this, info, method);
    }

    static void Lua_xlua_pushasciistring_Hook(void* __this, void* L, app::String* str, MethodInfo* method)
    {
        LOG_DEBUG("Pushed string: %s", il2cppi_to_string(str).c_str());
        callOrigin(Lua_xlua_pushasciistring_Hook, __this, L, str, method);
    }

    static int checkCount = 0;
    static void* LuaEnv_DoString_Hook(void* __this, app::Byte__Array* chunk, app::String* chunkName, void* env, MethodInfo* method)
    {
        if (checkCount > 0)
        {
            LOG_DEBUG("After size %d; name: %s", chunk->bounds == nullptr ? chunk->max_length : chunk->bounds->length, il2cppi_to_string(chunkName).c_str());
            checkCount--;
        }
        return callOrigin(LuaEnv_DoString_Hook, __this, chunk, chunkName, env, method);
    }

    static void LuaShellManager_DoString_Hook(void* __this, app::Byte__Array* byteArray, MethodInfo* method)
    {
        LOG_DEBUG("Size %d", byteArray->bounds == nullptr ? byteArray->max_length : byteArray->bounds->length);
        checkCount = 10;
        callOrigin(LuaShellManager_DoString_Hook, __this, byteArray, method);
    }

    static void LuaShellManager_ReportLuaShellResult_Hook(void* __this, app::String* type, app::String* value, MethodInfo* method)
    {
        std::cout << "Type: " << il2cppi_to_string(type) << std::endl;
        std::cout << "Value: " << il2cppi_to_string(value) << std::endl;
        callOrigin(LuaShellManager_ReportLuaShellResult_Hook, __this, type, value, method);
    }

    static bool ActorAbilityPlugin_OnEvent_Hook(void* __this, app::BaseEvent* e, MethodInfo* method)
    {
        // LOG_DEBUG("Fire event: %s, targetID %u", magic_enum::enum_name(e->fields.eventID).data(), e->fields.targetID);
        return callOrigin(ActorAbilityPlugin_OnEvent_Hook, __this, e, method);
    }

    static void DrawWaypoints(UniDict<uint32_t, UniDict<uint32_t, app::MapModule_ScenePointData>*>* waypointsGrops)
    {
        if (waypointsGrops == nullptr)
        {
            ImGui::Text("Waypoint data doesn't exist.");
            return;
        }

        auto singleton = GET_SINGLETON(MBHLOBDPKEC);

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
                        ImGui::Text("Level: %u", waypoint.level);
                        ImGui::Text("EntityId: %u", waypoint.entityId);
                        ImGui::Text("ModelHiden: %s", waypoint.isModelHidden ? "true" : "false");

                        if (waypoint.config != nullptr)
                        {
                            auto location = waypoint.config->fields;
                            ImGui::Text("Waypoint type: %s", magic_enum::enum_name(location._type).data());
                            ImGui::Text("Trans position: %s", il2cppi_to_string(location._tranPos).c_str());
                            ImGui::Text("Object position: %s", il2cppi_to_string(location._pos).c_str());
                            ImGui::Text("_unlocked: %s", location._unlocked ? "true" : "false");
                            ImGui::Text("_groupLimit: %s", location._groupLimit ? "true" : "false");
                            uint16_t areaId = app::SimpleSafeUInt16_get_Value(nullptr, location.areaIdRawNum, nullptr);
                            ImGui::Text("areaId: %u", areaId);
                            ImGui::Text("areaUnlocked: %s", app::MapModule_IsAreaUnlock(singleton, sceneId, areaId, nullptr) ? "true" : "false");
                            ImGui::Text("gadgetIdRawNum: %u", location.gadgetIdRawNum);
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
        auto singleton = GET_SINGLETON(MBHLOBDPKEC);
        if (singleton == nullptr)
        {
            ImGui::Text("Manager not initialized.");
            return;
        }

        if (ImGui::TreeNode("Waypoints"))
        {
            auto waypoints = TO_UNI_DICT(singleton->fields._scenePointDics, uint32_t, UniDict<uint32_t COMMA app::MapModule_ScenePointData>*);
            DrawWaypoints(waypoints);
            ImGui::TreePop();
        }
    }

    void DrawEntity(game::Entity* entity)
    {
        if (entity == nullptr)
        {
            ImGui::Text("Entity doesn't exist.");
            return;
        }
        ImGui::Text("Entity type: %s", magic_enum::enum_name(entity->type()).data());
        ImGui::Text("Entity name: %s", entity->name().c_str());
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

        auto& manager = game::EntityManager::instance();
        auto entities = manager.entities();

        ImGui::Text("Entity count %d", entities.size());

        ImGui::Checkbox("## Enable object name filter", &useObjectNameFilter); ImGui::SameLine();
        if (!useObjectNameFilter)
            ImGui::BeginDisabled();
        ImGui::InputText("Entity name filter", objectNameFilter, 128);
        if (!useObjectNameFilter)
            ImGui::EndDisabled();

        ImGui::Checkbox("Show only oculi", &checkOnlyShells);

        if (ImGui::TreeNode("Type Filter"))
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

        if (ImGui::TreeNode("Entity List"))
        {
            auto entries = magic_enum::enum_entries<app::EntityType__Enum_1>();
            for (const auto& [value, name] : entries)
            {
                if (!typeFilters[int(value)])
                    continue;

                if (ImGui::TreeNode(name.data()))
                {
                    for (const auto& entity : entities) {
                        if (entity == nullptr)
                            continue;

                        if (!typeFilters[(int)entity->type()])
                            continue;

                        if (checkOnlyShells && !game::filters::combined::Oculies.IsValid(entity))
                            continue;

                        if (useObjectNameFilter && entity->name().find(objectNameFilter) == -1)
                            continue;

                        uintptr_t id = entity->runtimeID();
                        if (ImGui::TreeNode(reinterpret_cast<void*>(id), "Entity 0x%p : %u; Dist %.3fm", entity, entity->runtimeID(), manager.avatar()->distance(entity)))
                        {
                            if (ImGui::Button("Teleport"))
                            {
                                auto& mapTeleport = MapTeleport::GetInstance();
                                mapTeleport.TeleportTo(entity->absolutePosition());
                            }

                            ImGui::SameLine();
                            if (ImGui::Button("Teleport to void"))
                                entity->setRelativePosition({ 0, 0, 0 });

                            ImGui::SameLine();
                            if (ImGui::Button("Teleport to me"))
                                entity->setRelativePosition(manager.avatar()->relativePosition());

                            DrawEntity(entity);
                            ImGui::TreePop();
                        }
                    }
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
        auto interactionManager = GET_SINGLETON(InteractionManager);
        if (interactionManager == nullptr)
        {
            ImGui::Text("Manager not loaded.");
            return;
        }

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


        auto keyList = TO_UNI_LINK_LIST(interactionManager->fields._keyInterList, app::InterActionGrp*);
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
                        auto interactions = TO_UNI_LIST(item->fields._interActionList, app::BaseInterAction*);
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

    void DrawMapManager()
    {
		auto mapManager = GET_SINGLETON(MapManager);
        if (mapManager == nullptr)
            return;

        int temp = mapManager->fields.playerSceneID;
        ImGui::InputInt("Player scene id", &temp);

		temp = mapManager->fields.mapSceneID;
		ImGui::InputInt("Map scene id", &temp);
    }

    void DrawImGuiFocusTest()
    {
		ImGui::Text("Is any item active: %s", ImGui::IsAnyItemActive() ? "true" : "false");
		ImGui::Text("Is any item focused: %s", ImGui::IsAnyItemFocused() ? "true" : "false");

		ImGui::Button("Test");
		auto hk = Hotkey();
		int temp = 0;
		InputHotkey("Test hotkey", &hk, false);
		ImGui::InputInt("Test input", &temp);
    }

    std::map<std::string, std::string> chestNames;
    std::unordered_set<std::string> notWrittenChests;
    bool showNotWritten = false;
    void OnGameUpdate()
    {
        if (!showNotWritten)
            return;

        auto& entityManager = game::EntityManager::instance();

        notWrittenChests.clear();
        for (auto& entity : entityManager.entities(game::filters::combined::Chests))
        {
            auto& entityName = entity->name();
            if (chestNames.count(entityName) == 0)
                notWrittenChests.insert(entityName);
        }
    }

    void DrawChestPlugin()
    {
        static std::map<std::string, std::string> tempNames;

        auto& entityManager = game::EntityManager::instance();
        ImGui::Checkbox("Show not written", &showNotWritten);
        for (auto& entity : entityManager.entities(game::filters::combined::Chests))
        {
            auto& entityName = entity->name();
            if (showNotWritten && chestNames.count(entityName) > 0)
                continue;

            app::LCChestPlugin* chestPlugin = entity->plugin<app::LCChestPlugin>(*app::LCChestPlugin__TypeInfo);
            if (chestPlugin == nullptr)
                continue;

            if (!ImGui::TreeNode(entity, "Chest 0x%p, Distance: %f", entity, entityManager.avatar()->distance(entity)))
                continue;

            auto& pluginData = chestPlugin->fields;
            auto& owner = pluginData._owner->fields;
            auto& ownerData = owner._dataItem->fields;
            app::GadgetState__Enum chestState = static_cast<app::GadgetState__Enum>(ownerData.gadgetState);
            ImGui::Text("Is ability locked: %s", pluginData._isLockByAbility ? "true" : "false");
            ImGui::Text("State: %s", magic_enum::enum_name(chestState).data());
            
            bool added = chestNames.count(entityName) > 0;

            if (tempNames.count(entityName) == 0)
                tempNames[entityName] = added ? chestNames[entityName] : std::string();

            auto& tempName = tempNames[entityName];
            ImGui::PushID(entity);

            ImGui::Text("Name: %s", entityName.c_str());
            ImGui::InputText("Friendly name", &tempName);

            if (ImGui::Button(added ? "Update" : "Add"))
                chestNames[entityName] = tempName;

            if (ImGui::Button("Teleport"))
            {
                auto& mapTeleport = MapTeleport::GetInstance();
                mapTeleport.TeleportTo(entity->absolutePosition());
            }

            ImGui::PopID();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Chest dictionary"))
        {
            std::stringstream text;
            text << "{\n";
            for (auto& [rawName, friendlyName] : chestNames)
            {
                text << "\t\"" << friendlyName << "\" : \"" << rawName << "\",\n";
            }
            text << "}";
            std::string textStr = text.str();
            ImGui::InputTextMultiline("Dict", &textStr);
            ImGui::TreePop();
        }
		
	}

    void DrawScenePropManager()
    {
        auto scenePropManager = GET_SINGLETON(ScenePropManager);
        if (scenePropManager == nullptr)
        {
            ImGui::Text("Scene prop manager not loaded.");
            return;
        }

        auto scenePropDict = TO_UNI_DICT(scenePropManager->fields._scenePropDict, int32_t, app::Object*);
        if (scenePropDict == nullptr)
        {
            ImGui::Text("Scene prop dict is nullptr.");
            return;
        }

        ImGui::Text("Prop count: %d", scenePropDict->count);
        
        auto& manager = game::EntityManager::instance();
        for (auto& [id, propObject] : scenePropDict->pairs())
        {
            auto tree = CastTo<app::SceneTreeObject>(propObject, *app::SceneTreeObject__TypeInfo);
            if (tree == nullptr)
                continue;

            auto pos = tree->fields._.realBounds.m_Center;
            auto config = tree->fields._config->fields;

            auto pattern = config._._.scenePropPatternName;
            app::ECGLPBEEEAA__Enum value;
            bool result = app::ScenePropManager_GetTreeTypeByPattern(scenePropManager, pattern, &value, nullptr);
            if (!result)
                continue;

            ImGui::Text("Tree at %s, type: %s, distance %0.3f", il2cppi_to_string(pos).c_str(), magic_enum::enum_name(value).data(),
                manager.avatar()->distance(app::WorldShiftManager_GetRelativePosition(nullptr, pos, nullptr)));
        }
    }

    class ItemFilter : game::IEntityFilter
    {
    public:
        ItemFilter() : ItemFilter(app::EntityType__Enum_1::None, "") 
        {}

        ItemFilter(app::EntityType__Enum_1 type, const std::string& name) : m_Type(type), m_Name(name)
        {

        }

        bool IsValid(game::Entity* entity) const override
        {
            return entity->type() == m_Type && entity->name() == m_Name;
        }

        app::EntityType__Enum_1 m_Type;
        std::string m_Name;
    };

    static bool filtersIsLoaded = false;
    static std::map<std::string, ItemFilter> simpleFilters;
    static std::vector<ItemFilter> removedItems;

    static const std::string filename = "picked_filters.json";
    static bool filterItemPickerEnabled = false;

    static ItemFilter tempFilter;
    static std::string tempName;
    static std::string tempSectionName;

    static bool addingFilter;
    static game::CacheFilterExecutor executor;

    void FilterItemPickerLoad()
    {
        filtersIsLoaded = true;

        std::ifstream fs(filename, std::ios::in);
        if (!fs.is_open())
            return;

        nlohmann::json jRoot;
        try {
            jRoot =nlohmann::json::parse(fs);
        }
        catch (nlohmann::detail::parse_error& parseError)
        {
            LOG_ERROR("Failed to parse json");
        }
        
        for (auto& [key, value] : jRoot["filters"].items())
            simpleFilters[key] = ItemFilter(value["type"], value["name"]);

		for (auto& value : jRoot["excluded"])
            removedItems.push_back(ItemFilter(value["type"], value["name"]));
    }

    void FiltetItemPickerSave()
    {
        std::ofstream fs(filename, std::ios::out);
        if (!fs.is_open())
        {
            LOG_ERROR("Failed to save changes.");
            return;
        }

        nlohmann::json jRoot = {};
        jRoot["filters"] = {};
        for (auto& [key, value] : simpleFilters)
        {
            jRoot["filters"][key] = {};
            jRoot["filters"][key]["name"] = value.m_Name;
            jRoot["filters"][key]["type"] = value.m_Type;
        }

        jRoot["excluded"] = {};
        for (auto& value : removedItems)
        {
            nlohmann::json item = {};
            item["name"] = value.m_Name;
            item["type"] = value.m_Type;

            jRoot["excluded"].push_back(item);
        }

        fs << jRoot.dump(4);
        fs.close();
    }

    void DrawFilterItemPicker()
    {
        ImGui::Checkbox("Enable ## itemPicker", &filterItemPickerEnabled);
        if (!filterItemPickerEnabled)
            return;

        if (!filtersIsLoaded)
            FilterItemPickerLoad();

        for (auto& [key, filter] : simpleFilters)
        {
            ImGui::PushID(key.c_str());
            ImGui::PushItemWidth(250);

            std::string keyText = key;
            ImGui::InputText("## Name", &keyText);
            ImGui::SameLine();

			ImGui::InputText("## ItemName", &filter.m_Name);
			ImGui::SameLine();
            
            std::string typeName = std::string(magic_enum::enum_name(filter.m_Type));
			ImGui::InputText("## ItemType", &typeName);

            ImGui::PopItemWidth();
            ImGui::PopID();
        }
    }

	void Debug::DrawExternal()
	{
        if (!filterItemPickerEnabled)
            return;

        auto& manager = game::EntityManager::instance();

        game::Entity* selectedEntity = nullptr;
        esp::render::PrepareFrame();

        for (auto& entity : manager.entities())
        {
            bool unexplored = true;
            for (auto& [_, filter] : simpleFilters)
            {
                if (executor.ApplyFilter(entity, reinterpret_cast<game::IEntityFilter*>(&filter)))
                {
                    unexplored = false;
                    break;
                }
            }

			for (auto& filter : removedItems)
			{
				if (executor.ApplyFilter(entity, reinterpret_cast<game::IEntityFilter*>(&filter)))
				{
					unexplored = false;
					break;
				}
			}

            if (!unexplored)
                continue;

            bool isSelected = esp::render::DrawEntity(entity->name(), entity, ImColor(255, 0, 0, 255));
            if (isSelected && selectedEntity == nullptr)
            {
                esp::render::DrawEntity(entity->name(), entity, ImColor(0, 255, 0, 255));
                selectedEntity = entity;
            }
        }


        bool updated = false;

        if (!addingFilter)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_R, false) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
            {
                if (removedItems.size() > 0)
                {
                    removedItems.pop_back();
                    updated = true;
                }
            }
			else if (selectedEntity != nullptr && ImGui::IsKeyPressed(ImGuiKey_R, false))
			{
			    removedItems.push_back(ItemFilter(selectedEntity->type(), selectedEntity->name()));
			    updated = true;
			}

            if (selectedEntity != nullptr && ImGui::IsKeyPressed(ImGuiKey_T, false))
            {
                tempFilter = ItemFilter(selectedEntity->type(), selectedEntity->name());
                addingFilter = true;
                tempName = "";
                renderer::globals::IsInputBlocked = true;
            }
        }

        if (addingFilter)
        {
            ImGui::Begin("Input name", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::PushItemWidth(500);
            ImGui::InputText("Section", &tempSectionName);
			if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
				ImGui::SetKeyboardFocusHere(0);
            ImGui::InputText("Name", &tempName);
            ImGui::PopItemWidth();
            ImGui::End();

            if (ImGui::IsKeyPressed(ImGuiKey_Enter, false))
            {
                simpleFilters[fmt::format("{}::{}", tempSectionName, tempName)] = tempFilter;
                renderer::globals::IsInputBlocked = false;
                addingFilter = false;
                updated = true;
            }

			if (ImGui::IsKeyPressed(ImGuiKey_Escape, false))
			{
				renderer::globals::IsInputBlocked = false;
				addingFilter = false;
			}
        }

        if (updated)
            FiltetItemPickerSave();
	}


	void Debug::DrawMain()
	{
        if (ImGui::CollapsingHeader("Filter item picker"))
            DrawFilterItemPicker();
        
        if (ImGui::CollapsingHeader("ScenePropManager"))
            DrawScenePropManager();

		if (ImGui::CollapsingHeader("Chest plugin", ImGuiTreeNodeFlags_None))
			DrawChestPlugin();

		if (ImGui::CollapsingHeader("Imgui focus test", ImGuiTreeNodeFlags_None))
            DrawImGuiFocusTest();

		if (ImGui::CollapsingHeader("Scene id info", ImGuiTreeNodeFlags_None))
            DrawMapManager();

		if (ImGui::CollapsingHeader("Entity manager", ImGuiTreeNodeFlags_None))
			DrawEntitiesData();

		if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_None))
			DrawPositionInfo();

		if (ImGui::CollapsingHeader("Interaction manager", ImGuiTreeNodeFlags_None))
			DrawInteractionManagerInfo();

		if (ImGui::CollapsingHeader("Map manager", ImGuiTreeNodeFlags_None))
			DrawManagerData();
	}

	bool Debug::NeedInfoDraw() const
	{
        return showNotWritten && notWrittenChests.size() > 0;
	}

	void Debug::DrawInfo()
	{
        for (auto& name : notWrittenChests)
        {
            ImGui::Text("%s", name.c_str());
        }
	}



}