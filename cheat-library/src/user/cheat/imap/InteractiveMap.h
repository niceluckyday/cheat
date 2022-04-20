#pragma once
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/config/Config.h>

namespace cheat::feature 
{

	class InteractiveMap : public Feature
    {
	public:
		config::field::ToggleField m_Enabled;
		config::field::BaseField<float> m_IconSize;
		config::field::BaseField<bool> m_DynamicSize;
		config::field::BaseField<bool> m_ShowUnlocked;
		config::field::BaseField<bool> m_ShowHDIcons;
		
		static InteractiveMap& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

		void DrawExternal() override;
	
	private:

		InteractiveMap();

		struct PointData
		{
			app::Vector2 pointLocation;
		};
		using Points = std::vector<PointData>;

		struct LabelData
		{
			std::string name;
			std::string clearName;
			Points points;
		};

		struct SceneData
		{
			std::map<uint32_t, LabelData> labels;
			std::map<std::string, LabelData*> name2Label;
		};

		using ScenesData = std::map<uint32_t, SceneData>;

		ScenesData m_ScenesData;
		std::vector<std::pair<LabelData*, std::vector<PointData*>>> m_ValidPointsCache;
		std::map<LabelData*, config::field::BaseField<bool>> m_Filters;

		PointData ParsePointData(const nlohmann::json& data);
		void LoadLabelData(const nlohmann::json& data, uint32_t sceneID, uint32_t labelID);
		void LoadSceneData(const nlohmann::json& data, uint32_t sceneID);
		void LoadScenesData();

		void ApplyScaling();

		void DrawMenu();
		void DrawFilterCheck();
	};
}

