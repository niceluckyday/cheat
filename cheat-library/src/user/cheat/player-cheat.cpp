#include <pch-il2cpp.h>
#include "cheat.h"

#include <vector>

#include <magic_enum.hpp>

#include <il2cpp-appdata.h>
#include <helpers.h>

#include <util/HookManager.h>
#include <util/Config.h>


// Godmode (return false when target is avatar, that mean avatar entity isn't attackable)
bool Miscs_CheckTargetAttackable_Hook(void* __this, app::EAPPPCHHMHO* attacker, app::EAPPPCHHMHO* target, MethodInfo* method)
{
    if (Config::cfgGodModEnable.GetValue() && target->fields.entityType == app::HCMIDDEBJOB__Enum::Avatar)
        return false;
    return callOrigin(Miscs_CheckTargetAttackable_Hook, __this, attacker, target, method);
}

// Infinite stamina. Blocks changes for stamina property. 
// Note. Changes received from the server (not sure about this for current time), 
//       that means that server know our stamina, and changes it in client can be detected.
// TODO: Find where client send information about stamina, or stamina cost actions like sprint, climb or swim and block it.
void AvatarPropDictionary_SetItem_Hook(app::Dictionary_2_JNHGGGCKJNA_JKNLDEEBGLL_* __this, app::JNHGGGCKJNA key, app::JKNLDEEBGLL value, MethodInfo* method)
{
    app::PropType__Enum propType = app::AvatarProp_DecodePropType(nullptr, key, nullptr);
    if (Config::cfgInfiniteStaminaEnable.GetValue() &&
        (propType == app::PropType__Enum::PROP_MAX_STAMINA ||
            propType == app::PropType__Enum::PROP_CUR_PERSIST_STAMINA ||
            propType == app::PropType__Enum::PROP_CUR_TEMPORARY_STAMINA))
        return;

    // auto propValue = app::AvatarProp_DecodePropValue(nullptr, value, nullptr);
    callOrigin(AvatarPropDictionary_SetItem_Hook, __this, key, value, method);
}

void InitPlayerCheatHooks() 
{
    // Teleport to mark hooks
    HookManager::install(app::Miscs_CheckTargetAttackable, Miscs_CheckTargetAttackable_Hook);
    LOG_TRACE("Hooked Miscs_CheckTargetAttackable. Origin at 0x%p", HookManager::getOrigin(Miscs_CheckTargetAttackable_Hook));

    HookManager::install(app::AvatarPropDictionary_SetItem, AvatarPropDictionary_SetItem_Hook);
    LOG_TRACE("Hooked AvatarPropDictionary_SetItem. Origin at 0x%p", HookManager::getOrigin(AvatarPropDictionary_SetItem_Hook));

    LOG_DEBUG("Hooks installed");
}
