#include <pch-il2cpp.h>
#include "cheat.h"

#include <vector>

#include <magic_enum.hpp>

#include <helpers.h>
#include <il2cpp-appdata.h>
#include <common/HookManager.h>
#include <common/Config.h>
#include <gcclib/Logger.h>
#include <common/Patch.h>
#include <common/GlobalEvents.h>

// Raised when monster trying to do skill. Attack also is skill.
// We just block if dumb mob enabled, so mob will not attack player.
static void VCMonsterAIController_TryDoSkill_Hook(void* __this, uint32_t skillID, MethodInfo* method) 
{
    if (Config::cfgDumbEnemiesEnabled.GetValue())
        return;
    callOrigin(VCMonsterAIController_TryDoSkill_Hook, __this, skillID, method);
}

// Raised when dialog view updating
// We call free click, if autotalk enabled, that means we just emulate user click
// When appear dialog choose we create notify with dialog select first item.
void InLevelCutScenePageContext_UpdateView_Hook(app::InLevelCutScenePageContext* __this, MethodInfo* method)
{
    callOrigin(InLevelCutScenePageContext_UpdateView_Hook, __this, method);

    if (!Config::cfgAutoTalkEnabled.GetValue())
        return;

    auto talkDialog = __this->fields._talkDialog;
    if (talkDialog == nullptr)
        return;

    if (talkDialog->fields._inSelect)
    {
        int32_t value = 0;
        auto object = il2cpp_value_box((Il2CppClass*)*app::Int32__TypeInfo, &value);
        auto notify = app::Notify_CreateNotify_1(nullptr, app::AJAPIFPNFKP__Enum::DialogSelectItemNotify, (app::Object*)object, nullptr);
        app::TalkDialogContext_OnDialogSelectItem(talkDialog, &notify, nullptr);
    }
    else
        app::InLevelCutScenePageContext_OnFreeClick(__this, method);
}

void InitWorldCheats()
{
    // Dialog skip
    HookManager::install(app::InLevelCutScenePageContext_UpdateView, InLevelCutScenePageContext_UpdateView_Hook);

    // Dumb enemies
    HookManager::install(app::VCMonsterAIController_TryDoSkill, VCMonsterAIController_TryDoSkill_Hook);

    LOG_DEBUG("Initialized");
}

