#include <pch-il2cpp.h>
#include "util.h"

#include <Windows.h>
#include <helpers.h>

// Getting resource from the memory
bool GetResourceMemory(HINSTANCE hInstance, int resId, LPBYTE& pDest, DWORD& size) 
{
    HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resId), RT_RCDATA);
    if (hResource) {
        HGLOBAL hGlob = LoadResource(hInstance, hResource);
        if (hGlob) {
            size = SizeofResource(hInstance, hResource);
            pDest = static_cast<LPBYTE>(LockResource(hGlob));
            if (size > 0 && pDest)
                return true;
        }
    }
    return false;
}

// Game utils

app::Vector3 GetRelativePosition(app::BaseEntity* entity)
{
    if (entity == nullptr)
        return {};

    return app::BaseEntity_GetRelativePosition(entity, nullptr);
}

void SetRelativePosition(app::BaseEntity* entity, app::Vector3 position)
{
    if (entity == nullptr)
        return;

    app::BaseEntity_SetRelativePosition(entity, position, true, nullptr);
}

app::BaseEntity* GetAvatarEntity()
{
    auto entityManager = GetSingleton(EntityManager);
    if (entityManager == nullptr)
        return nullptr;

    auto avatarEntity = app::EntityManager_GetCurrentAvatar(entityManager, nullptr);
    return avatarEntity;
}

app::Vector3 GetAvatarRelativePosition()
{
    return app::BaseEntity_GetRelativePosition(GetAvatarEntity(), nullptr);
}

void SetAvatarRelativePosition(app::Vector3 position)
{
    SetRelativePosition(GetAvatarEntity(), position);
}

float GetDistToAvatar(app::BaseEntity* entity)
{
    if (entity == nullptr || entity->fields._transform_k__BackingField == nullptr)
        return 0;

    auto dist = app::Vector3_Distance(nullptr, GetAvatarRelativePosition(), GetRelativePosition(entity), nullptr);
    return dist;
}

bool IsEntityFilterValid(app::BaseEntity* entity, const SimpleEntityFilter& filter) 
{
    if (entity == nullptr)
        return false;

    if (filter.typeFilter.enabled && entity->fields.entityType != filter.typeFilter.value)
        return false;

    if (filter.nameFilter.enabled)
    {
        bool found = false;
        auto name = il2cppi_to_string(app::BaseEntity_ToStringRelease(entity, nullptr));
        for (auto& pattern : filter.nameFilter.value)
        {
            if (name.find(pattern) != -1) {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }
    return true;
}

std::vector<app::BaseEntity*> GetEntities() 
{
    auto entityManager = GetSingleton(EntityManager);
    if (entityManager == nullptr)
        return {};

    auto entities = ToUniList(entityManager->fields._entities, app::BaseEntity*);
    if (entities == nullptr)
        return {};

    std::vector<app::BaseEntity*> aliveEntities;
    aliveEntities.reserve(entities->size);

    for (const auto& entity : *entities) 
    {
        if (entity != nullptr)
            aliveEntities.push_back(entity);
    }
    return aliveEntities;
}

uint32_t GetAvatarRuntimeId() 
{
    auto entityManager = GetSingleton(EntityManager);
    if (entityManager == nullptr)
        return 0;

    return entityManager->fields._curAvatarEntityID;
}

app::BaseEntity* GetEntityByRuntimeId(uint32_t runtimeId) 
{
    auto entityManager = GetSingleton(EntityManager);
    if (entityManager == nullptr)
        return nullptr;

    return app::EntityManager_GetValidEntity(entityManager, runtimeId, nullptr);
}

std::vector<app::BaseEntity*> FindEntities(const SimpleEntityFilter& filter)
{
    std::vector<app::BaseEntity*> result{};
    for (auto& entity : GetEntities())
    {
        if (IsEntityFilterValid(entity, filter))
            result.push_back(entity);
    }

    return result;
}

std::vector<app::BaseEntity*> FindEntities(FilterFunc func) 
{
    std::vector<app::BaseEntity*> result{};
    for (auto& entity : GetEntities()) 
    {
        if (func(entity))
            result.push_back(entity);
    }
    return result;
}

app::BaseEntity* FindNearestEntity(const SimpleEntityFilter& filter)
{
    auto entities = FindEntities(filter);
    app::BaseEntity* minDistEntity = nullptr;
    float minDistance = 100000;
    for (auto& entity : entities) 
    {
        auto dist = GetDistToAvatar(entity);
        if ( dist < minDistance)
        {
            minDistance = dist;
            minDistEntity = entity;
        }
    }
    return minDistEntity;
}

const SimpleEntityFilter& GetFilterCrystalShell() 
{
    static const SimpleEntityFilter crystallShellFilter = {
        {true, app::EntityType__Enum_1::GatherObject},
        {true, {
            /*Anemoculus, Geoculus*/ "CrystalShell" ,
            /*Crimson Agate*/ "Prop_Essence_01",
            /*Electroculus*/ "ElectricCrystal"
            }
        }
    };
    return crystallShellFilter;
}

const SimpleEntityFilter& GetFilterChest() 
{
    static const SimpleEntityFilter filter = {
        {true, app::EntityType__Enum_1::Chest}
    };
    return filter;
}

const SimpleEntityFilter& GetMonsterFilter()
{
    static const SimpleEntityFilter filter = {
        {true, app::EntityType__Enum_1::Monster}
    };
    return filter;
}

bool IsEntityCrystalShell(app::BaseEntity* entity) {

    return IsEntityFilterValid(entity, GetFilterCrystalShell());
}

