#pragma once
#include <optional>
#include <list>
#include <set>

#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/Config.h>
#include <cheat-base/config/field/StringField.h>
#include <cheat/game/Entity.h>
#include <cheat/game/IEntityFilter.h>

namespace cheat::feature 
{

	class InteractiveMap : public Feature
    {
	public:
		config::field::ToggleField m_Enabled;
		config::field::BaseField<bool> m_SeparatedWindows;
		config::field::BaseField<bool> m_UnlockedLogShow;

		config::field::BaseField<float> m_IconSize;
		config::field::BaseField<float> m_MinimapIconSize;
		config::field::BaseField<bool> m_DynamicSize;
		config::field::BaseField<bool> m_ShowHDIcons;
		
		config::field::BaseField<bool> m_ShowUnlocked;
		config::field::BaseField<float> m_UnlockedTransparency;

		config::field::BaseField<bool> m_AutoDetectNewItems;
		config::field::BaseField<float> m_NewItemsDetectRange;
		config::field::BaseField<int> m_NewItemsDetectingDelay;

		config::field::BaseField<bool> m_AutoDetectGatheredItems;
		config::field::BaseField<float> m_GatheredItemsDetectRange;
		
		config::field::HotkeyField m_UnlockNearestPoint;
		config::field::HotkeyField m_RevertLatestUnlock;
		config::field::BaseField<bool> m_UnlockOnlySelected;
		config::field::BaseField<float> m_PointFindRange;
		
		static InteractiveMap& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		void DrawExternal() override;

		struct PointData
		{
			uint32_t id;

			uint32_t sceneID;
			uint32_t labelID;

			app::Vector2 levelPosition;

			bool unlocked;
			int64_t unlockTimestamp;

			bool isCustom;
			int64_t creationTimestamp;
		};

		// std::optional<PointData> GetSelectedPoint();
		InteractiveMap::PointData* GetHoveredPoint();

		InteractiveMap::PointData* FindNearestPoint(app::Vector2 levelPosition, uint32_t sceneID = 0);
		InteractiveMap::PointData* FindEntityPoint(game::Entity* entity, uint32_t sceneID = 0);

		void UnlockPoint(PointData* pointData);
		void LockPoint(PointData* pointData);
		void RevertLatestUnlocking();

		void AddCustomPoint(uint32_t sceneID, uint32_t labelID, app::Vector2 levelPosition);
		void RemoveCustomPoint(PointData* pointData);

	private:

		InteractiveMap();

		struct LabelData
		{
			uint32_t id;
			uint32_t sceneID;

			std::string name;
			std::string clearName;
			config::field::BaseField<bool>* enabled;

			std::map<uint32_t, PointData> points;
			uint32_t unlockedCount;

			game::IEntityFilter* filter;
			bool supportGatherDetect;
		};

		struct CategoryData
		{
			std::string name;
			std::vector<LabelData*> children;
		};

		struct SceneData
		{
			std::map<uint32_t, LabelData> labels;
			std::map<std::string, LabelData*> nameToLabel;
			std::vector<CategoryData> categories;
		};

		std::map<uint32_t, SceneData> m_ScenesData;

		std::mutex m_UserDataMutex; // Support multithread
		config::field::StringField m_UserPointsData;
		config::field::BaseField<uint32_t> m_CustomPointIndex; // Stores last index for new custom points
		
		struct _UnlockTimestampCmp {
			bool operator() (PointData* const& lhs, PointData* const& rhs) const
			{
				return lhs->unlockTimestamp > rhs->unlockTimestamp;
			}
		};
		std::set<PointData*, _UnlockTimestampCmp> m_CustomPoints;

		struct _CreationTimestampCmp {
			bool operator() (PointData* const& lhs, PointData* const& rhs) const
			{
				return lhs->creationTimestamp > rhs->creationTimestamp;
			}
		};
		std::set<PointData*, _CreationTimestampCmp> m_UnlockedPoints;

		std::mutex m_PointMutex;
		// PointData* m_SelectedPoint;
		PointData* m_HoveredPoint;

		std::string m_SearchText;
	
		// Parsing map data
		PointData ParsePointData(const nlohmann::json& data);
		void LoadLabelData(const nlohmann::json& data, uint32_t sceneID, uint32_t labelID);
		void LoadCategorieData(const nlohmann::json& data, uint32_t sceneID);
		void LoadSceneData(const nlohmann::json& data, uint32_t sceneID);
		void LoadScenesData();

		void ApplyScaling();

		void InitializeEntityFilter(game::IEntityFilter* filter, const std::string& clearName);
		void InitializeEntityFilters();

		// Loading user data
		void LoadUserData();
		void SaveUserData();

		void LoadUnlockPointData(LabelData* labelData, const nlohmann::json& data);
		void SaveUnlockPointData(nlohmann::json& jObject, PointData* pointData);
		
		void LoadCustomPointData(LabelData* labelData, const nlohmann::json& data);
		void SaveCustomPointData(nlohmann::json& jObject, PointData* pointData);
		
		// Drawing
		void DrawMenu();
		void DrawFilters(bool searchFixed = true);
		void DrawFilter(LabelData& label);

		void DrawPoint(const PointData& pointData, const ImVec2& screenPosition, float radius, float radiusSquared, ImTextureID texture, bool selectable = true);
		void DrawPoints();

		void DrawMinimapPoints();
		
		// Block interact
		void OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& cancelled);
		void OnKeyUp(short key, bool& cancelled);

		// Detecting stuff
		void OnGameUpdate();
		void NewItemsDetect();
	};
}

