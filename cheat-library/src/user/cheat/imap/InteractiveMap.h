#pragma once

#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/config.h>
#include <cheat/game/Entity.h>
#include <cheat/game/IEntityFilter.h>

namespace cheat::feature 
{

	class InteractiveMap : public Feature
    {
	public:
		config::Field<config::ToggleHotkey> f_Enabled;
		config::Field<bool> f_SeparatedWindows;
		config::Field<bool> f_CompletionLogShow;

		config::Field<float> f_IconSize;
		config::Field<float> f_MinimapIconSize;
		config::Field<bool> f_DynamicSize;
		config::Field<bool> f_ShowHDIcons;
		
		config::Field<bool> f_ShowCompleted;
		config::Field<float> f_CompletePointTransparency;

		config::Field<bool> f_AutoDetectNewItems;
		config::Field<bool> f_NewItemsDetectOnlyShowed;
		config::Field<float> f_NewItemsDetectRange;
		config::Field<int> f_NewItemsDetectingDelay;

		config::Field<bool> f_AutoDetectGatheredItems;
		config::Field<float> f_GatheredItemsDetectRange;
		
		config::Field<Hotkey> f_CompleteNearestPoint;
		config::Field<Hotkey> f_RevertLatestCompletion;
		config::Field<bool> f_CompleteOnlyViewed;
		config::Field<float> f_PointFindRange;
		
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

			bool completed;
			int64_t completeTimestamp;

			bool isCustom;
			int64_t creationTimestamp;
		};

		// std::optional<PointData> GetSelectedPoint();
		PointData* GetHoveredPoint();

		std::vector<PointData*> GetEntityPoints(game::Entity* entity, bool completed = false, uint32_t sceneID = 0);
		PointData* FindNearestPoint(const app::Vector2& levelPosition, float range = 0.0f, bool onlyShowed = true, bool completed = false, uint32_t sceneID = 0);
		PointData* FindEntityPoint(game::Entity* entity, float range = 0.0f, uint32_t sceneID = 0);

		void CompletePoint(PointData* pointData);
		void UncompletePoint(PointData* pointData);
		void RevertLatestPointCompleting();

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
			config::Field<bool> enabled;

			std::map<uint32_t, PointData> points;
			uint32_t completedCount;

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
		config::Field<nlohmann::json> f_UserPointsData;
		config::Field<uint32_t> f_CustomPointIndex; // Stores last index for new custom points
		
		std::unordered_set<PointData*> m_CustomPoints;
		std::unordered_set<PointData*> m_CompletedPoints;

		std::mutex m_PointMutex;
		// PointData* m_SelectedPoint;
		PointData* m_HoveredPoint;

		std::string m_SearchText;
	
		// Parsing map data
		PointData ParsePointData(const nlohmann::json& data);
		void LoadLabelData(const nlohmann::json& data, uint32_t sceneID, uint32_t labelID);
		void LoadCategoriaData(const nlohmann::json& data, uint32_t sceneID);
		void LoadSceneData(const nlohmann::json& data, uint32_t sceneID);
		void LoadScenesData();

		void ApplyScaling();

		void InitializeEntityFilter(game::IEntityFilter* filter, const std::string& clearName);
		void InitializeEntityFilters();

		void InitializeGatherDetectItems();

		// Loading user data
		void LoadUserData();
		void SaveUserData();

		void LoadCompletedPointData(LabelData* labelData, const nlohmann::json& data);
		static void SaveCompletedPointData(nlohmann::json& jObject, PointData* point);
		
		void LoadCustomPointData(LabelData* labelData, const nlohmann::json& data);
		static void SaveCustomPointData(nlohmann::json& jObject, PointData* point);
		
		// Drawing
		void DrawMenu();
		void DrawFilters(const bool searchFixed = true);
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
		void OnItemGathered(game::Entity* entity);

		// Utility
		static PointData* FindNearestPoint(const LabelData& label, const app::Vector2& levelPosition, float range = 0.0f, bool completed = false);
		std::vector<InteractiveMap::LabelData*> FindLabelsByClearName(const std::string& clearName);

		// Hooks
		static void GadgetModule_OnGadgetInteractRsp_Hook(void* __this, app::GadgetInteractRsp* notify, MethodInfo* method);
		static void InLevelMapPageContext_UpdateView_Hook(app::InLevelMapPageContext* __this, MethodInfo* method);
		static void InLevelMapPageContext_ZoomMap_Hook(app::InLevelMapPageContext* __this, float value, MethodInfo* method);
		static void MonoMiniMap_Update_Hook(app::MonoMiniMap* __this, MethodInfo* method);
	};
}

