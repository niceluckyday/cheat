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
        if (app::Int32__TypeInfo == nullptr || *app::Int32__TypeInfo == nullptr)
            return;

        int32_t value = 0;
        auto object = il2cpp_value_box((Il2CppClass*)*app::Int32__TypeInfo, &value);
        auto notify = app::Notify_CreateNotify_1(nullptr, app::AJAPIFPNFKP__Enum::DialogSelectItemNotify, (app::Object*)object, nullptr);
        app::TalkDialogContext_OnDialogSelectItem(talkDialog, &notify, nullptr);
    }
    else
    {
        app::InLevelCutScenePageContext_OnFreeClick(__this, method);
    }
}

bool TalkDialogContext_get_canClick_Hook(app::TalkDialogContext* __this, MethodInfo* method)
{

    if (Config::cfgTalkSkipEnabled.GetValue())
    {

        //return true;

        //auto protectTime = __this->fields._interactableTime;
        //protectTime -= app::MonoTalkDialog_get_clickTipAniTime(__this->fields._dialogMono, nullptr);
        //LOG_DEBUG("Standart time %.03f changed %.03f. Diff: %.03f", __this->fields._interactableTime, protectTime,
        //    __this->fields._interactableTime - protectTime);
        //auto currentTime = app::Time_get_time(nullptr, nullptr);
        //return currentTime > protectTime;
    }

    return callOrigin(TalkDialogContext_get_canClick_Hook, __this, method);
}

void InitWorldCheats()
{
    HookManager::install(app::InLevelCutScenePageContext_UpdateView, InLevelCutScenePageContext_UpdateView_Hook);
    // HookManager::install(app::TalkDialogContext_get_canClick, TalkDialogContext_get_canClick_Hook);

    ToggleConfigField::OnChangedEvent += FREE_METHOD_HANDLER(OnToggleFieldChange);
}

