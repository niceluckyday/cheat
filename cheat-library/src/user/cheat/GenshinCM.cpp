#include "pch-il2cpp.h"
#include "GenshinCM.h"

#include <helpers.h>
#include <cheat/events.h>

cheat::GenshinCM& cheat::GenshinCM::instance()
{
	static GenshinCM instance;
	return instance;
}

void cheat::GenshinCM::CursorSetVisibility(bool visibility)
{
	app::Cursor_set_visible(nullptr, visibility, nullptr);
	app::Cursor_set_lockState(nullptr, visibility ? app::CursorLockMode__Enum::None : app::CursorLockMode__Enum::Locked, nullptr);
}

bool cheat::GenshinCM::CursorGetVisibility()
{
	return app::Cursor_get_visible(nullptr, nullptr);
}

cheat::GenshinCM::GenshinCM() :
	NFS(f_AccountsData, "Accounts data", "General", internal::AccountsData())
{
	events::GameUpdateEvent += MY_METHOD_HANDLER(cheat::GenshinCM::OnGameUpdate);
}

void cheat::GenshinCM::DrawProfileLine()
{
	auto& currentProfile = config::CurrentProfileName();
	DrawAttachAccountToProfile(currentProfile);

	CheatManagerBase::DrawProfileLine();
}

bool cheat::GenshinCM::IsCurrentAccountAttached()
{
	auto& profiles = f_AccountsData.value().profiles;
	return profiles.count(m_CurrentAccount.userID) > 0;
}

void cheat::GenshinCM::DrawAttachAccountToProfile(const std::string& profileName)
{
	if (m_CurrentAccount.userID == 0)
		return;

	if (IsCurrentAccountAttached())
	{
		if (ImGui::Button("Deattach"))
		{
			f_AccountsData.value().profiles.erase(m_CurrentAccount.userID);
			f_AccountsData.value().accounts.erase(m_CurrentAccount.userID);
			f_AccountsData.FireChanged();
		}
	}
	else
	{
		if (ImGui::Button("Attach"))
		{
			f_AccountsData.value().accounts[m_CurrentAccount.userID] = m_CurrentAccount;
			f_AccountsData.value().profiles[m_CurrentAccount.userID] = profileName;
			f_AccountsData.FireChanged();
		}
	}
	ImGui::SameLine();
}

void cheat::GenshinCM::OnAccountChanged()
{
	auto& profiles = f_AccountsData.value().profiles;
	if (!IsCurrentAccountAttached())
		return;

	config::ChangeProfile(profiles[m_CurrentAccount.userID]);

	auto& settings = feature::Settings::GetInstance();

	ImGuiToast toast(ImGuiToastType_Info, settings.f_NotificationsDelay.value(), "Account was updated.\nConfig profile was changed.");
	toast.set_title("Config multi-account");
	ImGui::InsertNotification(toast);
}

#define UPDATE_DELAY(delay) \
							static ULONGLONG s_LastUpdate = 0;       \
                            ULONGLONG currentTime = GetTickCount64();\
                            if (s_LastUpdate + (delay) > currentTime)  \
                                return;                              \
							s_LastUpdate = currentTime;

void cheat::GenshinCM::OnGameUpdate()
{
	UPDATE_DELAY(2000U);

	auto playerModule = GET_SINGLETON(PlayerModule);
	if (playerModule == nullptr || playerModule->fields._accountData_k__BackingField == nullptr)
	{
		m_CurrentAccount.userID = 0;
		return;
	}

	auto& accountData = playerModule->fields._accountData_k__BackingField->fields;

	bool accountChanged = m_CurrentAccount.userID != accountData.userId;

	m_CurrentAccount.nickName = il2cppi_to_string(accountData.nickName);
	m_CurrentAccount.userID = accountData.userId;

	if (accountChanged)
		OnAccountChanged();
}
