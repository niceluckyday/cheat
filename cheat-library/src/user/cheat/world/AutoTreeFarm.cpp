#include "pch-il2cpp.h"
#include "AutoTreeFarm.h"

#include <helpers.h>
#include <algorithm>

#include <cheat/events.h>
#include <cheat/game.h>

namespace cheat::feature 
{
    AutoTreeFarm::AutoTreeFarm() : Feature(),
        NF(m_Enabled,      "Auto tree farm",  "AutoTreeFarm", false),
		NF(m_AttackDelay,  "Attack delay",    "AutoTreeFarm", 150),
		NF(m_RepeatDelay,  "Repeat delay",    "AutoTreeFarm", 500),
		NF(m_AttackPerTree,"Attack per tree", "AutoTreeFarm", 5),
        NF(m_Range,        "Range",           "AutoTreeFarm", 15.0f)
    { 
		events::GameUpdateEvent += MY_METHOD_HANDLER(AutoTreeFarm::OnGameUpdate);
	}

    const FeatureGUIInfo& AutoTreeFarm::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "Auto tree farm", "World", true };
        return info;
    }

    void AutoTreeFarm::DrawMain()
    {
		ImGui::TextColored(ImColor(255, 165, 0, 255), "Note. This feature not tested in detectable aspect.\n"
			"\tDon't recommend use it in main account in first days after release.");
		
		ConfigWidget("Enabled", m_Enabled, "Automatically attack trees in range.");
		ConfigWidget(m_AttackDelay, 1, 0, 1000, "Delay before next tree attack.");
		ConfigWidget(m_RepeatDelay, 1, 500, 1000, "Delay before next attack same tree.");

		ConfigWidget(m_AttackPerTree, 1, 0, 100, "Counts of attack to one tree.\n" 
			"It needs to avoid unnecessary attacks to empty tree.\n" 
			"Note:   0 - Unlimited\n"
			"Note.2: Memorized trees' attacks reset after game restart."
		);

		ImGui::TextColored(ImColor(255, 165, 0, 255), "In current version range limited ~15m.");
		ConfigWidget(m_Range, 0.1f, 1.0f, 15.0f);
    }

    bool AutoTreeFarm::NeedStatusDraw() const
	{
        return m_Enabled;
    }

    void AutoTreeFarm::DrawStatus() 
    { 
        ImGui::Text("Auto tree farm [%dms]", m_RepeatDelay.value());
    }

    AutoTreeFarm& AutoTreeFarm::GetInstance()
    {
        static AutoTreeFarm instance;
        return instance;
    }


	std::unordered_set<app::SceneTreeObject*> GetTreeSet()
	{
		auto scenePropManager = GetSingleton(ScenePropManager);
		if (scenePropManager == nullptr)
			return {};

		auto scenePropDict = ToUniDict(scenePropManager->fields._scenePropDict, int32_t, app::Object*);
		if (scenePropDict == nullptr)
			return {};

		std::unordered_set<app::SceneTreeObject*> trees;
		for (auto& [id, propObject] : scenePropDict->pairs())
		{
			auto tree = game::CastTo<app::SceneTreeObject>(propObject, *app::SceneTreeObject__TypeInfo);
			if (tree == nullptr)
				continue;

			trees.insert(tree);
		}

		return trees;
	}


	struct Vector3d
	{
		float x, y, z;

		Vector3d(const app::Vector3& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
		}

		bool operator==(const Vector3d& b) const
		{
			return x == b.x && y == b.y && z == b.z;
		}
	};

	struct hash_fn
	{
		std::size_t operator() (const Vector3d& vector) const
		{
			return std::hash<float>()(vector.x) ^ std::hash<float>()(vector.y) ^ std::hash<float>()(vector.z);
		}
	};



	void AutoTreeFarm::OnGameUpdate()
	{
		static std::unordered_map<Vector3d, uint32_t, hash_fn> s_AttackCountMap;

		static std::queue<app::SceneTreeObject*> s_AttackQueue;
		static std::unordered_set<app::SceneTreeObject*> s_AttackQueueSet;
		static uint64_t s_LastAttackTimestamp = 0;

		uint64_t timestamp = app::GetTimestamp(nullptr, nullptr);
		if (!m_Enabled || s_LastAttackTimestamp + m_AttackDelay > timestamp)
			return;

		auto scenePropManager = GetSingleton(ScenePropManager);
		auto networkManager = GetSingleton(NetworkManager_1);
		if (networkManager == nullptr || scenePropManager == nullptr)
			return;

		auto treeSet = GetTreeSet();
		for (auto& tree : treeSet)
		{
			if (s_AttackQueueSet.count(tree) > 0)
				continue;

			if (tree->fields._lastTreeDropTimeStamp + m_RepeatDelay > timestamp)
				continue;

			auto position = tree->fields._.realBounds.m_Center;
			if (game::GetDistToAvatar(app::WorldShiftManager_GetRelativePosition(nullptr, position, nullptr)) > m_Range)
				continue;

			s_AttackQueueSet.insert(tree);
			s_AttackQueue.push(tree);
		}
		
		while (!s_AttackQueue.empty())
		{
			auto tree = s_AttackQueue.front();
			s_AttackQueue.pop();
			s_AttackQueueSet.erase(tree);
			
			if (treeSet.count(tree) == 0)
				continue;

			auto position = tree->fields._.realBounds.m_Center;
			if (game::GetDistToAvatar(app::WorldShiftManager_GetRelativePosition(nullptr, position, nullptr)) > m_Range)
				continue;

			app::ECGLPBEEEAA__Enum treeType;
			auto pattern = tree->fields._config->fields._._.scenePropPatternName;
			if (!app::ScenePropManager_GetTreeTypeByPattern(scenePropManager, pattern, &treeType, nullptr))
				continue;

			if (m_AttackPerTree > 0)
			{
				if (s_AttackCountMap.count(position) == 0)
					s_AttackCountMap[position] = 0;

				auto& attackCount = s_AttackCountMap[position];
				attackCount++;
				if (attackCount > m_AttackPerTree)
					continue;
			}

			tree->fields._lastTreeDropTimeStamp = timestamp;
			app::NetworkManager_1_RequestHitTreeDropNotify(networkManager, position, position, treeType, nullptr);
			break;
		}

		if (s_AttackCountMap.size() > 1000)
			s_AttackCountMap.clear();
	}
}

