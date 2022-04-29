#pragma once

#include <cheat-base/cheat/CheatManagerBase.h>

namespace cheat
{
	namespace internal
	{
		struct AccountData
		{
		public:
			uint32_t userID;
			//uint32_t regionID;
			std::string nickName;
			std::string pseudo;

			AccountData() : userID(0), /*regionID(0),*/ nickName(), pseudo() {}
		};

		struct AccountsData
		{
		public:

			std::unordered_map<uint32_t, AccountData> accounts;
			std::unordered_map<uint32_t, std::string> profiles;

			bool operator==(const AccountsData& other)
			{
				return accounts.size() == other.accounts.size() && profiles.size() == other.profiles.size();
			}

			AccountsData() : accounts(), profiles() {}
		};

		inline void to_json(nlohmann::json& j, const AccountData& p)
		{
			j = nlohmann::json{ { "nickName", p.nickName }, { "pseudo", p.pseudo }, { "userID", p.userID }/*,  {"regionID", p.regionID} */ };
		}

		inline void from_json(const nlohmann::json& j, AccountData& p)
		{
			j.at("nickName").get_to(p.nickName);
			j.at("pseudo").get_to(p.pseudo);
			j.at("userID").get_to(p.userID);
			//j.at("regionID").get_to(p.regionID);
		}

		inline void to_json(nlohmann::json& j, const AccountsData& p)
		{
			j = nlohmann::json{ { "accounts", nlohmann::json(p.accounts) }, { "profiles", nlohmann::json(p.profiles) } };
		}

		inline void from_json(const nlohmann::json& j, AccountsData& p)
		{
			j.at("accounts").get_to(p.accounts);
			j.at("profiles").get_to(p.profiles);
		}
	}

	class GenshinCM : public CheatManagerBase
	{
	public:
		static GenshinCM& instance();

		void CursorSetVisibility(bool visibility) final;
		bool CursorGetVisibility() final;

	protected:

		internal::AccountData m_CurrentAccount;
		config::Field<internal::AccountsData> f_AccountsData;
		GenshinCM();

		bool IsCurrentAccountAttached();

		void DrawProfileLine() final;
		void DrawAttachAccountToProfile(const std::string& profileName);

		void OnAccountChanged();
		void OnGameUpdate();
	};
}