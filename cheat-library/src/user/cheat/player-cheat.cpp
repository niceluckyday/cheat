#include <pch-il2cpp.h>
#include "cheat.h"

#include <vector>

#include <magic_enum.hpp>

#include <il2cpp-appdata.h>
#include <helpers.h>

#include <util/HookManager.h>
#include <util/Config.h>


// Gamemode (return false when target is avatar, that mean avatar entity isn't attackable)
bool Miscs_CheckTargetAttackable_Hook(void* __this, app::EAPPPCHHMHO* attacker, app::EAPPPCHHMHO* target, MethodInfo* method)
{
    if (Config::cfgGodModEnable.GetValue() && target->fields.entityType == app::HCMIDDEBJOB__Enum::Avatar)
        return false;
    return callOrigin(Miscs_CheckTargetAttackable_Hook, __this, attacker, target, method);
}


void InitPlayerCheatHooks() 
{
    // Teleport to mark hooks
    HookManager::install(app::Miscs_CheckTargetAttackable, Miscs_CheckTargetAttackable_Hook);
    LOG_TRACE("Hooked Miscs_CheckTargetAttackable. Origin at 0x%p", HookManager::getOrigin(Miscs_CheckTargetAttackable_Hook));

    LOG_DEBUG("Hooks installed");
}
