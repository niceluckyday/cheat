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

static void OnToggleFieldChange(ToggleConfigField* field)
{
    TogglePatch(field, Config::cfgDumbEnemiesEnabled, 0x4692062, { 0x75 });
}

static uint32_t currentTalk = -1;

void InteractionManager_RequestTalkFinish_Hook(app::InteractionManager* __this, app::LCBaseIntee* intee, uint32_t talkId, MethodInfo* method)
{
    LOG_DEBUG("Request to finish talk %u", talkId);
    callOrigin(InteractionManager_RequestTalkFinish_Hook, __this, intee, talkId, method);
}


bool InterActionGrp_get_IsFinished_Hook(app::InterActionGrp* __this, MethodInfo* method) 
{
    return callOrigin(InterActionGrp_get_IsFinished_Hook, __this, method);
}

//void InLevelCutScenePageContext_OnFreeClick_Hook(InLevelCutScenePageContext* __this, MethodInfo* method));
void InLevelCutScenePageContext_UpdateView_Hook(void* __this, MethodInfo* method) 
{
    if (Config::cfgAutoTalkEnabled.GetValue() || Config::cfgTalkSkipEnabled.GetValue())
        app::InLevelCutScenePageContext_OnFreeClick(__this, method);
    callOrigin(InLevelCutScenePageContext_UpdateView_Hook, __this, method);
}

bool TalkDialogContext_get_canClick_Hook(void* __this, MethodInfo* method)
{
    if (Config::cfgTalkSkipEnabled.GetValue())
        return true;

    return callOrigin(TalkDialogContext_get_canClick_Hook, __this, method);
}

void InitWorldCheats()
{
    HookManager::install(app::InLevelCutScenePageContext_UpdateView, InLevelCutScenePageContext_UpdateView_Hook);
    HookManager::install(app::TalkDialogContext_get_canClick, TalkDialogContext_get_canClick_Hook);

    ToggleConfigField::OnChangedEvent += FREE_METHOD_HANDLER(OnToggleFieldChange);
}

