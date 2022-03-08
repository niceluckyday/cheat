#pragma once

#include <Windows.h>
#include <string>
#include <vector>

#include <il2cpp-appdata.h>

#include "Logger.h"

#define LogLastError(msg) LOG_ERROR("%s. Error: %s", msg, GetLastErrorAsString().c_str())

template<class type>
struct EntityFilterField
{
	bool enabled;
	type value;
};

struct EntityFilter 
{
	EntityFilterField<app::EntityType__Enum_1> typeFilter;
	EntityFilterField<std::vector<std::string>> nameFilter;
};

std::string GetLastErrorAsString();
bool GetResourceMemory(HINSTANCE hInstance, int resId, LPBYTE& pDest, DWORD& size);

// Position utils
app::Vector3 GetRelativePosition(app::BaseEntity* entity);
void SetRelativePosition(app::BaseEntity* entity, app::Vector3 position);
app::Vector3 GetAvatarRelativePosition();
void SetAvatarRelativePosition(app::Vector3 position);
float GetDistToAvatar(app::BaseEntity* entity);

// Entity fields
std::vector<app::BaseEntity*> FindEntities(const EntityFilter& filter);
app::BaseEntity* FindNearestEntity(const EntityFilter& filter);

const EntityFilter& GetFilterCrystalShell();
const EntityFilter& GetFilterChest();

bool IsEntityCrystalShell(app::BaseEntity* entity);
