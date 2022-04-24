#include "pch-il2cpp.h"
#include "DialogSkip.h"

#include <helpers.h>
#include <cheat/game/EntityManager.h>

namespace cheat::feature 
{
    static void InLevelCutScenePageContext_UpdateView_Hook(app::InLevelCutScenePageContext* __this, MethodInfo* method);
    static void InLevelCutScenePageContext_ClearView_Hook(app::InLevelCutScenePageContext* __this, MethodInfo* method);

    DialogSkip::DialogSkip() : Feature(),
        NF(m_Enabled,               "Auto talk",                "AutoTalk", false),
        NF(m_AutoSelectDialog,      "Auto select dialog",       "AutoTalk", true),
        NF(m_ExcludeImportant,      "Exclude Katheryne/Tubby",  "AutoTalk", true),
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
        ConfigWidget("Auto-select Dialog", m_AutoSelectDialog, "Automatically select dialog choices.");
        if (m_AutoSelectDialog)
        {
            ImGui::Indent();
            ConfigWidget("Exclude Katheryne/Tubby", m_ExcludeImportant, "Exclude Kath/Tubby from auto-select.");
            ImGui::Unindent();
        }
        ConfigWidget("Fast Dialog", m_FastDialog, "Speeds up dialog (includes crafting/cooking/cutscenes).");
    }

    bool DialogSkip::NeedStatusDraw() const
{
        return m_Enabled;
    }

    void DialogSkip::DrawStatus() 
    {
        ImGui::Text("Dialog [%s%s%s%s%s]",
            m_AutoSelectDialog ? "Auto" : "Manual",
            m_AutoSelectDialog && (m_ExcludeImportant || m_FastDialog) ? "|" : "",
            m_ExcludeImportant ? "Exc" : "",
            m_ExcludeImportant && m_FastDialog ? "|" : "",
            m_FastDialog ? "Fast" : "Normal");
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

        bool isImportant = false;
        if (m_ExcludeImportant)
        {
            // TODO: Add a custom filter in the future where users can
            // add their own name substrings of entities to avoid
            // speeding up dialog on.
            std::vector<std::string> impEntitiesNames = {
                "Djinn", 
                "Katheryne"
            };
            auto dialogPartnerID = context->fields._inteeID;
            auto& manager = game::EntityManager::instance();
            auto dialogPartner = manager.entity(dialogPartnerID);
            auto dialogPartnerName = dialogPartner->name();
            for (auto impEntityName : impEntitiesNames)
            {
                if (dialogPartnerName.find(impEntityName) != -1) {
                    LOG_DEBUG("%s %s %d", dialogPartnerName.c_str(), impEntityName, dialogPartnerName.find(impEntityName));
                    isImportant = true;
                    break;
                }
            }
        }

		if (talkDialog->fields._inSelect && m_AutoSelectDialog && !isImportant)
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

