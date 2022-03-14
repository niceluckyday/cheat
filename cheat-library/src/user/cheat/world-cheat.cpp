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
    // Patch for dumb enemies
    // Inverse condition with player visibility for monsters
    TogglePatch(field, Config::cfgDumbEnemiesEnabled, 0x4692062, { 0x75 });
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
    HookManager::install(app::InLevelCutScenePageContext_UpdateView, InLevelCutScenePageContext_UpdateView_Hook);
    ToggleConfigField::OnChangedEvent += FREE_METHOD_HANDLER(OnToggleFieldChange);

    LOG_DEBUG("Initialized");
}

