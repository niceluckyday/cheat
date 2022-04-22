#pragma once
#include <optional>
#include <list>
#include <unordered_set>

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
		config::field::BaseField<bool> m_DynamicSize;
		config::field::BaseField<bool> m_ShowHDIcons;
		
		config::field::BaseField<bool> m_ShowUnlocked;
		config::field::BaseField<float> m_UnlockedTransparency;
		
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
		};

		// std::optional<PointData> GetSelectedPoint();
		InteractiveMap::PointData* GetHoveredPoint();

		InteractiveMap::PointData* FindNearestPoint(app::Vector2 levelPosition, uint32_t sceneID = 0);
		InteractiveMap::PointData* FindEntityPoint(game::Entity* entity, uint32_t sceneID = 0);

		void AddUnlockedPoint(PointData* pointData);
		void RemoveUnlockedPoint(PointData* pointData);
		void RemoveLatestUnlockedPoint();

	private:

		InteractiveMap();

		struct LabelData
		{
			std::string name;
			std::string clearName;
			config::field::BaseField<bool>* enabled;

			std::map<uint32_t, PointData> points;
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

		config::field::StringField m_UnlockedPointsField;
		std::list<PointData*> m_UnlockedPoints;
		std::unordered_set<PointData*> m_UnlockedPointsSet;
		
		std::map<std::string, game::IEntityFilter*> m_LabelToFilter; // Used to determine 
		std::vector<PointData*> m_ValidPointsCache;

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

		void LoadUnlockedPoints();
		void SaveUnlockedPoints();
		void InitializeEntityFilters();

		// Drawing
		void DrawMenu();
		void DrawFilters();
		void DrawFilter(const LabelData& label);

		void DrawPoint(const PointData& pointData, const ImVec2& screenPosition, float radius, float radiusSquared, ImTextureID texture);
		void DrawPoints();
		
		// Block interact
		void OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& cancelled);
		void OnKeyUp(short key, bool& cancelled);

		// Cache valid points
		void RefreshValidPoints();
	};
}

