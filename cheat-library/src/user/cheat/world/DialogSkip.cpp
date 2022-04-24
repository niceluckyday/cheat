#include "pch-il2cpp.h"
#include "DialogSkip.h"

#include <helpers.h>

namespace cheat::feature 
{
    static void InLevelCutScenePageContext_UpdateView_Hook(app::InLevelCutScenePageContext* __this, MethodInfo* method);
    static void InLevelCutScenePageContext_ClearView_Hook(app::InLevelCutScenePageContext* __this, MethodInfo* method);

    DialogSkip::DialogSkip() : Feature(),
        NF(m_Enabled,               "Auto talk",                "AutoTalk", false),
        NF(m_AutoSelectDialog,      "Auto select dialog",       "AutoTalk", true),
        NF(m_FastDialog,            "Fast dialog",              "AutoTalk", false)
    {
        HookManager::install(app::InLevelCutScenePageContext_UpdateView, InLevelCutScenePageContext_UpdateView_Hook);
        HookManager::install(app::InLevelCutScenePageContext_ClearView, InLevelCutScenePageContext_ClearView_Hook);
    }

    const FeatureGUIInfo& DialogSkip::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "Auto Talk", "World", true };
        return info;
    }

    void DialogSkip::DrawMain()
    {
        ConfigWidget("Enabled", m_Enabled, "Automatically continue the dialog.");
        ConfigWidget("Auto-select Dialog", m_AutoSelectDialog, "Automatically select dialogs.");
        ConfigWidget("Fast Dialog", m_FastDialog, "Speeds up dialog (includes crafting/cooking/cutscenes).");
    }

    bool DialogSkip::NeedStatusDraw() const
{
        return m_Enabled;
    }

    void DialogSkip::DrawStatus() 
    {
        ImGui::Text("Auto Talk: %s", m_AutoSelectDialog ? "Auto" : "Manual");
    }

    DialogSkip& DialogSkip::GetInstance()
    {
        static DialogSkip instance;
        return instance;
    }

	// Raised when dialog view updating
    // We call free click, if auto talk enabled, that means we just emulate user click
    // When appear dialog choose we create notify with dialog select first item.
    void DialogSkip::OnCutScenePageUpdate(app::InLevelCutScenePageContext* context) 
    {
        if (!m_Enabled)
            return;

        auto talkDialog = context->fields._talkDialog;
        if (talkDialog == nullptr)
            return;

        if (m_FastDialog)
            app::Time_set_timeScale(nullptr, 5.0f, nullptr);

		if (talkDialog->fields._inSelect && m_AutoSelectDialog)
		{
			int32_t value = 0;
			auto object = il2cpp_value_box((Il2CppClass*)*app::Int32__TypeInfo, &value);
			auto notify = app::Notify_CreateNotify_1(nullptr, app::AJAPIFPNFKP__Enum::DialogSelectItemNotify, (app::Object*)object, nullptr);
			app::TalkDialogContext_OnDialogSelectItem(talkDialog, &notify, nullptr);
		}
		else if (!talkDialog->fields._inSelect)
			app::InLevelCutScenePageContext_OnFreeClick(context, nullptr);
    }

	static void InLevelCutScenePageContext_UpdateView_Hook(app::InLevelCutScenePageContext* __this, MethodInfo* method)
	{
		callOrigin(InLevelCutScenePageContext_UpdateView_Hook, __this, method);

        DialogSkip& dialogSkip = DialogSkip::GetInstance();
        dialogSkip.OnCutScenePageUpdate(__this);
	}
    
    // Raised when exiting a dialog. We try to hackishly return to normal value.
    // Should be a better way to store the pre-dialog speed using Time_get_timeScale.
    static void InLevelCutScenePageContext_ClearView_Hook(app::InLevelCutScenePageContext* __this, MethodInfo* method)
    {
        float gameSpeed = app::Time_get_timeScale(nullptr, nullptr);
        if (gameSpeed > 1.0f)
            app::Time_set_timeScale(nullptr, 1.0f, nullptr);
        callOrigin(InLevelCutScenePageContext_ClearView_Hook, __this, method);
    }
}

