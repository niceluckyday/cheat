#include "pch-il2cpp.h"
#include "InteractiveMap.h"

#include <helpers.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/util.h>
#include <cheat-base/render/renderer.h>
#include <cheat/game/filters.h>
#include <cheat/events.h>
#include <cheat/game/CacheFilterExecutor.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <misc/cpp/imgui_stdlib.h>

namespace cheat::feature
{

#define UPDATE_DELAY(delay) \
							static ULONGLONG s_LastUpdate = 0;       \
                            ULONGLONG currentTime = GetTickCount64();\
                            if (s_LastUpdate + delay > currentTime)  \
                                return;                              \
							s_LastUpdate = currentTime;

#define UPDATE_DELAY_VAR(type, name, delay) \
							static type name = {};                   \
							static ULONGLONG s_LastUpdate = 0;       \
                            ULONGLONG currentTime = GetTickCount64();\
                            if (s_LastUpdate + delay > currentTime)  \
                                return name;                         \
                            s_LastUpdate = currentTime;

	InteractiveMap::InteractiveMap() : Feature(),
		NFF(m_Enabled, "Interactive map", "m_InteractiveMap", "InteractiveMap", false),
		NF(m_SeparatedWindows, "Separated windows", "InteractiveMap", true),
		NF(m_CompletionLogShow, "Completion log show", "InteractiveMap", false),

		NF(m_IconSize, "Icon size", "InteractiveMap", 20.0f),
		NF(m_MinimapIconSize, "Minimap icon size", "InteractiveMap", 14.0f),
		NF(m_DynamicSize, "Dynamic size", "InteractiveMap", false),
		NF(m_ShowHDIcons, "Show HD icons", "InteractiveMap", false),

		NF(m_ShowCompleted, "Show completed", "InteractiveMap", false),
		NF(m_CompletePointTransparency, "Completed point transparency", "InteractiveMap", 0.5f),

		NF(m_AutoDetectNewItems, "Detect new items", "InteractiveMap", true),
		NF(m_NewItemstDetectOnlyShowed, "Detect only showed", "InteractMap", true),
		NF(m_NewItemsDetectRange, "Detect range", "InteractiveMap", 20.0f),
		NF(m_NewItemsDetectingDelay, "Detect delay (ms)", "InteractiveMap", 2000),

		NF(m_AutoDetectGatheredItems, "Detect gathered items", "InteractiveMap", true),
		NF(m_GatheredItemsDetectRange, "Detect range", "InteractiveMap", 20.0f),

		NF(m_CompleteNearestPoint, "Complete nearest point", "InteractiveMap", Hotkey()),
		NF(m_RevertLatestCompletion, "Revert latest completion", "InteractiveMap", Hotkey()),
		NF(m_CompleteOnlyViewed, "Complete only showed", "InteractiveMap", true),
		NF(m_PointFindRange, "Point finding range", "InteractiveMap", 30.0f),

		NF(m_UserPointsData, "User points data", "InteractiveMap", "{}"),
		NF(m_CustomPointIndex, "Custom point index", "InteractiveMap", 1000000)
	{
		// Eventing
		cheat::events::GameUpdateEvent += MY_METHOD_HANDLER(InteractiveMap::OnGameUpdate);
		cheat::events::WndProcEvent    += MY_METHOD_HANDLER(InteractiveMap::OnWndProc);
		cheat::events::KeyUpEvent      += MY_METHOD_HANDLER(InteractiveMap::OnKeyUp);

		// Hooking
		HookManager::install(app::MonoMiniMap_Update,                  InteractiveMap::MonoMiniMap_Update_Hook);
		HookManager::install(app::GadgetModule_OnGadgetInteractRsp,    InteractiveMap::GadgetModule_OnGadgetInteractRsp_Hook);
		HookManager::install(app::InLevelMapPageContext_UpdateView,    InteractiveMap::InLevelMapPageContext_UpdateView_Hook);
		HookManager::install(app::InLevelMapPageContext_ZoomMap,       InteractiveMap::InLevelMapPageContext_ZoomMap_Hook);

		// Initializing
		LoadScenesData();
		ApplyScaling();
		LoadUserData();

		InitializeEntityFilters();
		InitializeGatherDetectItems();
	}

	const FeatureGUIInfo& InteractiveMap::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ "", "World", false };
		return info;
	}

	void InteractiveMap::DrawMain() { }

	void InteractiveMap::DrawMenu()
	{
		BeginGroupPanel("General");
		{
			ConfigWidget("Enabled", m_Enabled);
			ConfigWidget(m_SeparatedWindows, "Config and filters will be in separate windows.");
			//if (ImGui::Button(m_CompletionLogShow ? "Show log window" : "Hide log window"))
			//{
			//	*m_CompletionLogShow.valuePtr() = !m_CompletionLogShow;
			//	m_CompletionLogShow.Check();
			//}
		}
		EndGroupPanel();

		BeginGroupPanel("Icon view");
		{
			ConfigWidget(m_IconSize, 0.01f, 4.0f, 100.0f);
			ConfigWidget(m_MinimapIconSize, 0.01f, 4.0f, 100.0f);
			ConfigWidget(m_DynamicSize, "Icons will be sized dynamically depend to zoom size.\nMinimap icons don't affected.");
			ConfigWidget(m_ShowHDIcons, "Toggle icons to HD format.");
		}
		EndGroupPanel();

		BeginGroupPanel("Completed icon view");
		{
			ConfigWidget(m_ShowCompleted, "Show completed points.");
			ConfigWidget(m_CompletePointTransparency, 0.01f, 0.0f, 1.0f, "Completed points transparency.");
		}
		EndGroupPanel();

		BeginGroupPanel("New item detecting");
		{
			ConfigWidget(m_AutoDetectNewItems, "Enables detecting items what are not in interactive map data.\n"
				"Only items with green line support this function.");

			ConfigWidget(m_NewItemstDetectOnlyShowed, "Detect new items only for showed filters.");

			ConfigWidget(m_NewItemsDetectRange, 0.1f, 5.0f, 30.0f,
				"Only if item not found in this range about entity position,\n\t it be detected as new.");

			ConfigWidget(m_NewItemsDetectingDelay, 10, 100, 100000, "Detect new items is power consumption operation.\n"
				"So rescanning will happen with specified delay.");
		}
		EndGroupPanel();

		BeginGroupPanel("Gather detecting");
		{
			ConfigWidget(m_AutoDetectGatheredItems, "Enables detecting gathered items.\n"
				"It works only items what will be gathered after enabling this function.\n"
				"Only items with blue line support this function.");

			ConfigWidget(m_GatheredItemsDetectRange, 0.1f, 5.0f, 30.0f,
				"When entity was gathered finding nearest point in this range.");
		}
		EndGroupPanel();

		BeginGroupPanel("Manual completing");
		{
			ConfigWidget(m_CompleteNearestPoint, "When pressed, complete the nearest to avatar point.");
			ConfigWidget(m_RevertLatestCompletion, "When pressed, revert latest complete operation.");
			ConfigWidget(m_CompleteOnlyViewed, "Complete performed only to visible points.");
			ConfigWidget(m_PointFindRange, 0.5f, 0.0f, 200.0f, "Complete performs within specified range. If 0 - unlimited.");
		}
		EndGroupPanel();
	}

	void InteractiveMap::DrawFilters(bool searchFixed)
	{
		auto sceneID = game::GetCurrentMapSceneID();
		if (m_ScenesData.count(sceneID) == 0)
			ImGui::Text("Sorry. Current scene is not supported.");

		ImGui::InputText("Search", &m_SearchText);

		if (searchFixed)
			ImGui::BeginChild("FiltersList", ImVec2(-1, 0), false, ImGuiWindowFlags_NoBackground);

		auto& categories = m_ScenesData[sceneID].categories;
		for (auto& [categoryName, labels] : categories)
		{
			std::vector<LabelData*> validLabels;

			if (m_SearchText.empty())
			{
				validLabels = labels;
			}
			else
			{
				for (auto& label : labels)
				{
					std::string name = label->name;
					std::transform(name.begin(), name.end(), name.begin(), ::tolower);
					std::string search = m_SearchText;
					std::transform(search.begin(), search.end(), search.begin(), ::tolower);
					if (name.find(search) != std::string::npos)
						validLabels.push_back(label);
				}
			}

			if (validLabels.size() == 0)
				continue;

			SelectData selData
			{
				std::all_of(validLabels.begin(), validLabels.end(), [](const LabelData* label) { return label->enabled->value(); }),
				false
			};

			if (BeginGroupPanel(categoryName.c_str(), ImVec2(-1, 0), true, &selData))
			{
				if (ImGui::BeginTable("MarkFilters", 2))
				{
					for (auto& label : validLabels)
					{
						ImGui::TableNextColumn();
						DrawFilter(*label);
					}
					ImGui::EndTable();
				}

				EndGroupPanel();
			}

			if (selData.changed)
			{
				for (auto& label : validLabels)
				{
					*label->enabled->valuePtr() = selData.toggle;
				}
				config::UpdateAll();
			}
			
		}

		if (searchFixed)
			ImGui::EndChild();
	}

	// Modified ImGui::CheckBox
	void InteractiveMap::DrawFilter(LabelData& label)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(&label);
		const ImVec2 label_size = ImGui::CalcTextSize(label.name.c_str(), NULL, true);

		const float square_sz = ImGui::GetFrameHeight();
		const float image_sz = square_sz;

		const bool haveFilter = label.filter != nullptr;
		const bool haveGatherDetect = label.supportGatherDetect;

		float markWidth = 5.0f;
		float marksSize = 0.0f;
		float halfSpacing = style.ItemInnerSpacing.x / 2;

		if (haveFilter || haveGatherDetect)
			marksSize += halfSpacing;

		if (haveFilter)
			marksSize += halfSpacing + markWidth;

		if (haveGatherDetect)
			marksSize += halfSpacing + markWidth;

		std::string progress_text = fmt::format("{}/{}", label.completedCount, label.points.size());
		const ImVec2 progress_text_size = ImGui::CalcTextSize(progress_text.c_str());

		const ImVec2 pos = window->DC.CursorPos;
		const ImRect total_bb(pos,
			pos + ImVec2(square_sz + style.ItemInnerSpacing.x + image_sz + marksSize + style.ItemInnerSpacing.x + progress_text_size.x + style.ItemInnerSpacing.x +
				(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f),
				label_size.y + style.FramePadding.y * 2.0f));
		ImGui::ItemSize(total_bb, style.FramePadding.y);

		if (!ImGui::ItemAdd(total_bb, id))
		{
			IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
			return;
		}

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
		if (pressed)
		{
			*label.enabled->valuePtr() = !(*label.enabled);
			label.enabled->Check();

			ImGui::MarkItemEdited(id);
		}

		const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
		ImGui::RenderNavHighlight(total_bb, id);
		ImGui::RenderFrame(check_bb.Min, check_bb.Max, ImGui::GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
		ImU32 check_col = ImGui::GetColorU32(ImGuiCol_CheckMark);
		bool mixed_value = (g.LastItemData.InFlags & ImGuiItemFlags_MixedValue) != 0;
		if (mixed_value)
		{
			// Undocumented tristate/mixed/indeterminate checkbox (#2644)
			// This may seem awkwardly designed because the aim is to make ImGuiItemFlags_MixedValue supported by all widgets (not just checkbox)
			ImVec2 pad(ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)), ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)));
			window->DrawList->AddRectFilled(check_bb.Min + pad, check_bb.Max - pad, check_col, style.FrameRounding);
		}
		else if (*label.enabled)
		{
			const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
			ImGui::RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), check_col, square_sz - pad * 2.0f);
		}

		// --
		const ImVec2 image_pos(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y);
		const ImRect image_bb(image_pos, image_pos + ImVec2(image_sz, image_sz));

		auto image = ImageLoader::GetImage(label.clearName);
		if (image)
		{
			window->DrawList->AddImageRounded(image->textureID, image_bb.Min, image_bb.Max,
				ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImColor(255, 255, 255), image_sz / 4);
		}
		// --

		// --
		float cursorX = image_bb.Max.x;
		bool markHovered = false;
		if (marksSize != 0.0f)
		{
			ImVec2 mark_pos = ImVec2(cursorX + halfSpacing, image_bb.Min.y + style.FramePadding.y);
			ImVec2 mark_size = ImVec2(markWidth, image_bb.Max.y - image_bb.Min.y - 2 * style.FramePadding.y);
			
			if (haveFilter)
			{ 
				ImRect mark_bb = { mark_pos, mark_pos + mark_size };
				if (ImGui::IsMouseHoveringRect(mark_bb.Min, mark_bb.Max))
				{
					markHovered = true;
					ShowHelpText("New items detect supported");
				}

				ImGui::RenderFrame(mark_bb.Min, mark_bb.Max, ImColor(0.0f, 1.0f, 0.0f), false, 3.0f);
				mark_pos.x += markWidth + halfSpacing;
			}

			if (haveGatherDetect)
			{
				ImRect mark_bb = { mark_pos, mark_pos + mark_size };
				if (ImGui::IsMouseHoveringRect(mark_bb.Min, mark_bb.Max))
				{
					markHovered = true;
					ShowHelpText("Gather detect supported");
				}

				ImGui::RenderFrame(mark_bb.Min, mark_bb.Max, ImColor(0.0f, 0.0f, 1.0f), false, 3.0f);
				mark_pos.x += markWidth + halfSpacing;
			}

			cursorX = mark_pos.x;
		}
		// --
		
		// --
		ImVec2 label_progress_pos = ImVec2(cursorX + style.ItemInnerSpacing.x, image_bb.Min.y + style.FramePadding.y);
		ImGui::RenderText(label_progress_pos, progress_text.c_str());

		cursorX += style.ItemInnerSpacing.x + progress_text_size.x;
		// --		

		ImVec2 label_pos = ImVec2(cursorX + style.ItemInnerSpacing.x, image_bb.Min.y + style.FramePadding.y);
		if (g.LogEnabled)
			ImGui::LogRenderedText(&label_pos, mixed_value ? "[~]" : *label.enabled ? "[x]" : "[ ]");
		if (label_size.x > 0.0f)
			ImGui::RenderText(label_pos, label.name.c_str());

		if (!markHovered && ImGui::IsItemHovered())
			ShowHelpText(label.name.c_str());

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
		return;
	}

	InteractiveMap& InteractiveMap::GetInstance()
	{
		static InteractiveMap instance;
		return instance;
	}

	InteractiveMap::PointData* InteractiveMap::GetHoveredPoint()
	{
		std::lock_guard<std::mutex> _guard(m_PointMutex);
		return m_HoveredPoint;
	}


	std::vector<InteractiveMap::PointData*> InteractiveMap::GetEnitityPoints(game::Entity* entity, bool completed /*= false*/, uint32_t sceneID /*= 0*/)
	{
		sceneID = sceneID == 0 ? game::GetCurrentPlayerSceneID() : sceneID;
		if (m_ScenesData.count(sceneID) == 0)
			return {};

		auto& labels = m_ScenesData[sceneID].labels;

		std::vector<PointData*> points;
		static game::CacheFilterExecutor filterExecutor(2000U);
		for (auto& [labelID, label] : labels)
		{
			if (label.filter == nullptr)
				continue;

			if (!filterExecutor.ApplyFilter(entity, label.filter))
				continue;

			if (completed)
				points.reserve(label.points.size());

			for (auto& [pointID, point] : label.points)
			{
				if (completed || !point.completed)
					points.push_back(&point);
			}
			break;
		}
		return points;
	}

	InteractiveMap::PointData* InteractiveMap::FindNearestPoint(const app::Vector2& levelPosition, float range, bool onlyShowed, bool completed, uint32_t sceneID)
	{
		sceneID = sceneID == 0 ? game::GetCurrentPlayerSceneID() : sceneID;
		if (m_ScenesData.count(sceneID) == 0)
			return nullptr;

		auto& labels = m_ScenesData[sceneID].labels;
		
		PointData* minDinstancePoint = nullptr;
		float minDistance = 0;
		for (auto& [labelID, label] : labels)
		{
			if (onlyShowed && !label.enabled->value())
				continue;

			PointData* nearestLabelPoint = FindNearestPoint(label, levelPosition, range, completed);
			if (nearestLabelPoint == nullptr)
				continue;

			float distance = app::Vector2_Distance(nullptr, levelPosition, nearestLabelPoint->levelPosition, nullptr);
			if (distance < minDistance || minDinstancePoint == nullptr)
			{
				minDistance = distance;
				minDinstancePoint = nearestLabelPoint;
			}
		}

		if (minDinstancePoint == nullptr || (range > 0 && minDistance > range))
			return nullptr;

		return minDinstancePoint;
	}

	cheat::feature::InteractiveMap::PointData* InteractiveMap::FindNearestPoint(const LabelData& label, const app::Vector2& levelPosition, float range /*= 0.0f*/, float completed /*= false*/)
	{
		PointData* minDinstancePoint = nullptr;
		float minDistance = 0;
		for (auto& [pointID, point] : label.points)
		{
			if (!completed && point.completed)
				continue;

			float distance = app::Vector2_Distance(nullptr, levelPosition, point.levelPosition, nullptr);
			if (distance < minDistance || minDinstancePoint == nullptr)
			{
				minDistance = distance;
				minDinstancePoint = const_cast<PointData*>(&point);
			}
		}

		if (minDinstancePoint == nullptr || (range > 0 && minDistance > range))
			return nullptr;

		return minDinstancePoint;
	}

	InteractiveMap::PointData* InteractiveMap::FindEntityPoint(game::Entity* entity, float range /*= 20.0f*/, uint32_t sceneID /*= 0*/)
	{
		sceneID = sceneID == 0 ? game::GetCurrentPlayerSceneID() : sceneID;
		if (m_ScenesData.count(sceneID) == 0)
			return nullptr;

		auto levelPosition = entity->levelPosition();
		auto& labels = m_ScenesData[sceneID].labels;
		for (auto& [labelID, label] : labels)
		{
			if (label.filter == nullptr)
				continue;

			if (!label.filter->IsValid(entity))
				continue;

			auto nearestPoint = FindNearestPoint(label, entity->levelPosition(), range, false);
			if (nearestPoint == nullptr)
				return nullptr;

			return nearestPoint;
		}

		return nullptr;
	}


	void InteractiveMap::CompletePoint(PointData* pointData)
	{
		std::lock_guard<std::mutex> _userDataLock(m_UserDataMutex);
		LOG_WARNING("Complete point at %.0f.", game::EntityManager::instance().avatar()->distance(pointData->levelPosition));

		if (m_CompletedPoints.count(pointData) > 0)
			return;

		pointData->completed = true;
		pointData->completeTimestamp = util::GetCurrentTimeMillisec();
		m_ScenesData[pointData->sceneID].labels[pointData->labelID].completedCount++;
		m_CompletedPoints.insert(pointData);
		
		SaveUserData();
	}

	void InteractiveMap::UncompletePoint(PointData* pointData)
	{
		std::lock_guard<std::mutex> _userDataLock(m_UserDataMutex);

		if (m_CompletedPoints.count(pointData) == 0)
			return;

		pointData->completed = false;
		pointData->completeTimestamp = 0;
		m_ScenesData[pointData->sceneID].labels[pointData->labelID].completedCount--;
		m_CompletedPoints.erase(pointData);

		SaveUserData();
	}

	void InteractiveMap::RevertLatestPointCompleting()
	{
		std::lock_guard<std::mutex> _userDataLock(m_UserDataMutex);
		if (m_CompletedPoints.size() == 0)
			return;

		PointData* pointData = *m_CompletedPoints.begin();
		pointData->completed = false;
		pointData->completeTimestamp = 0;
		m_ScenesData[pointData->sceneID].labels[pointData->labelID].completedCount--;
		m_CompletedPoints.erase(pointData);

		SaveUserData();
	}

	void InteractiveMap::AddCustomPoint(uint32_t sceneID, uint32_t labelID, app::Vector2 levelPosition)
	{
		std::lock_guard<std::mutex> _userDataLock(m_UserDataMutex);
		if (m_ScenesData.count(sceneID) == 0)
			return;

		auto& sceneData = m_ScenesData[sceneID];
		if (sceneData.labels.count(labelID) == 0)
			return;

		auto& points = sceneData.labels[labelID].points;

		// TODO: Fix uint32_t overflow.
		// Callow: I think that will never happen, but who knows, who knows...
		while (points.count(m_CustomPointIndex) > 0)
			(*m_CustomPointIndex.valuePtr())++;

		auto& newPoint = points[m_CustomPointIndex];
		newPoint.id = m_CustomPointIndex;
		newPoint.isCustom = true;
		newPoint.creationTimestamp = util::GetCurrentTimeMillisec();
		newPoint.labelID = labelID;
		newPoint.sceneID = sceneID;
		newPoint.levelPosition = levelPosition;
		m_CustomPoints.insert(&newPoint);

		(*m_CustomPointIndex.valuePtr())++;
		m_CustomPointIndex.Check();

		SaveUserData();
	}

	void InteractiveMap::RemoveCustomPoint(PointData* pointData)
	{
		std::lock_guard<std::mutex> _userDataLock(m_UserDataMutex);
		if (m_CustomPoints.size() == 0)
			return;

		m_CustomPoints.erase(pointData);
		m_ScenesData[pointData->sceneID].labels[pointData->labelID].points.erase(pointData->id);
		SaveUserData();
	}

	void InteractiveMap::OnGameUpdate()
	{
		NewItemsDetect(); // Calling it from game update thread to avoid screen freezes
	}

	// For now this use straightforward method
	// More advanced method description here: https://github.com/CallowBlack/genshin-cheat/issues/176
	void InteractiveMap::NewItemsDetect()
	{
		UPDATE_DELAY(m_NewItemsDetectingDelay);
		
		auto sceneID = game::GetCurrentPlayerSceneID();
		if (m_ScenesData.count(sceneID) == 0)
			return;

		auto& labels = m_ScenesData[sceneID].labels;
		static game::CacheFilterExecutor filterExecutor(2000U);
		//std::lock_guard<std::mutex> _userDataLock(m_UserDataMutex);

		auto& manager = game::EntityManager::instance();

		std::vector<std::pair<LabelData*, std::unordered_set<game::Entity*>>> supportedEntities;
		for (auto& [labelID, label] : labels)
		{
			if (label.filter == nullptr)
				continue;

			if (m_NewItemstDetectOnlyShowed && !label.enabled->value())
				continue;

			auto& newElement = supportedEntities.emplace_back(&label, std::unordered_set<game::Entity*>{});
			auto& entities = newElement.second;
			for (auto& entity : manager.entities())
			{
				if (filterExecutor.ApplyFilter(entity, label.filter))
					entities.insert(entity);
			}

			if (entities.size() == 0)
				supportedEntities.pop_back();
		}

		for (auto& [label, entities] : supportedEntities)
		{
			std::unordered_set<PointData*> pointsSet;
			for (auto& [pointID, point] : label->points)
				pointsSet.insert(&point);
			
			for (auto& entity : entities)
			{
				PointData* nearestPoint = nullptr;
				float minDistance = 0.0f;
				for (auto& point : pointsSet)
				{
					auto distance = entity->distance(point->levelPosition);
					if (nearestPoint == nullptr || distance < minDistance)
					{
						nearestPoint = point;
						minDistance = distance;
					}
				}

				if (nearestPoint == nullptr)
					break;

				if (minDistance > m_NewItemsDetectRange)
				{
					AddCustomPoint(nearestPoint->sceneID, nearestPoint->labelID, entity->levelPosition());
					continue;
				}

				pointsSet.erase(nearestPoint);
			}
		}
	}

	void InteractiveMap::GadgetModule_OnGadgetInteractRsp_Hook(void* __this, app::GadgetInteractRsp* notify, MethodInfo* method)
	{
		if (notify->fields.opType_ == app::InterOpType__Enum::InterOpType__Enum_InterOpFinish)
		{
			auto entity = game::EntityManager::instance().entity(notify->fields.gadgetEntityId_);
			auto& interactiveMap = GetInstance();
			switch (notify->fields.interactType_)
			{
			case app::InteractType__Enum::InteractGather:
			case app::InteractType__Enum::InteractOpenChest:
				interactiveMap.OnItemGathered(entity);
			default:
				break;
			}
		}

		callOrigin(GadgetModule_OnGadgetInteractRsp_Hook, __this, notify, method);
	}

	void InteractiveMap::OnItemGathered(game::Entity* entity)
	{
		auto sceneID = game::GetCurrentPlayerSceneID();
		if (m_ScenesData.count(sceneID) == 0)
			return;

		auto& labels = m_ScenesData[sceneID].labels;
		for (auto& [labelID, label] : labels)
		{
			if (!label.supportGatherDetect || label.filter == nullptr)
				continue;

			if (!label.filter->IsValid(entity))
				continue;

			auto nearestPoint = FindNearestPoint(label, entity->levelPosition(), m_GatheredItemsDetectRange, sceneID);
			if (nearestPoint == nullptr)
			{
				LOG_WARNING("Failed to find uncompleted point for this object.");
				return;
			}
			CompletePoint(nearestPoint);
			return;
		}
	}

	void InteractiveMap::LoadUserData()
	{
		nlohmann::json jRoot = nlohmann::json::parse(m_UserPointsData.value(), nullptr, false);

		if (jRoot.is_discarded())
		{
			LOG_ERROR("Failed parse user points.");
			return;
		}

		for (auto& [cSceneID, jLabels] : jRoot.items())
		{
			auto sceneID = std::stoul(cSceneID);
			if (m_ScenesData.count(sceneID) == 0)
			{
				LOG_WARNING("Scene %u don't exist. Maybe map data was updated.", sceneID);
				continue;
			}

			auto& labels = m_ScenesData[sceneID].labels;
			for (auto& [cLabelID, jLabelUserData] : jLabels.items())
			{
				auto labelID = std::stoul(cLabelID);
				if (labels.count(labelID) == 0)
				{
					LOG_WARNING("Label %u:%u don't exist. Maybe data was .", sceneID, labelID);
					continue;
				}

				auto& label = labels[labelID];
				
				// NOTE. Custom points should finding first.
				auto customPoints = jLabelUserData.value("custom_points", nlohmann::json::array());
				for (auto& customPoint : customPoints)
					LoadCustomPointData(&label, customPoint);

				auto completedPoints = jLabelUserData.value("completed_points", nlohmann::json::array());
				for (auto& completeData : completedPoints)
					LoadCompletedPointData(&label, completeData);
			}
		}
	}

	void InteractiveMap::LoadCustomPointData(LabelData* labelData, const nlohmann::json& data)
	{
		auto customPoint = ParsePointData(data);
		if (labelData->points.count(customPoint.id) > 0)
		{
			LOG_ERROR("Failed to load custom point for label `%u:%s` with position %.1f, %.1f. ID already exist.",
				labelData->sceneID, labelData->name.c_str(), customPoint.levelPosition.x, customPoint.levelPosition.y);
			return;
		}
		
		auto& newPointEntry = labelData->points[customPoint.id];
		newPointEntry = customPoint;
		newPointEntry.sceneID = labelData->sceneID;
		newPointEntry.labelID = labelData->id;
		newPointEntry.isCustom = true;
		newPointEntry.creationTimestamp = data["creation_timestamp"];

		m_CustomPoints.insert(&newPointEntry);
	}

	void InteractiveMap::LoadCompletedPointData(LabelData* labelData, const nlohmann::json& data)
	{
		auto& points = labelData->points;
		auto& pointID = data["point_id"];

		if (points.count(pointID) == 0)
		{
			LOG_WARNING("Point %u don't exist. Maybe data was updated.", pointID);
			return;
		}

		auto& point = points[pointID];
		if (m_CompletedPoints.count(&point) > 0)
		{
			LOG_WARNING("Completed point %u dublicate.", pointID);
			return;
		}

		point.completed = true;
		point.completeTimestamp = data["complete_timestamp"];
		labelData->completedCount++;

		m_CompletedPoints.insert(&point);
	}

	void InteractiveMap::SaveUserData()
	{
		nlohmann::json jRoot = {};

		for (auto& [sceneID, scene] : m_ScenesData)
		{
			auto cSceneID = std::to_string(sceneID);
			jRoot[cSceneID] = nlohmann::json::object();

			auto& jLabels = jRoot[cSceneID];
			for (auto& [labelID, label] : scene.labels)
			{
				auto cLabelID = std::to_string(labelID);

				jLabels[cLabelID] = nlohmann::json::object();
				jLabels[cLabelID]["completed_points"] = nlohmann::json::array();
				jLabels[cLabelID]["custom_points"] = nlohmann::json::array();

				auto& jCompletedPoints = jLabels[cLabelID]["completed_points"];
				auto& jCustomPoints = jLabels[cLabelID]["custom_points"];
				for (auto& [pointID, point] : label.points)
				{
					SaveCustomPointData(jCustomPoints, &point);
					SaveCompletedPointData(jCompletedPoints, &point);
				}

				if (jCustomPoints.size() == 0)
					jLabels[cLabelID].erase("custom_points");

				if (jCompletedPoints.size() == 0)
					jLabels[cLabelID].erase("completed_points");
				
				if (jLabels[cLabelID].size() == 0)
					jLabels.erase(cLabelID);
			}

			if (jLabels.size() == 0)
				jRoot.erase(cSceneID);
		}

		*m_UserPointsData.valuePtr() = jRoot.dump();
		m_UserPointsData.Check();
	}

	void InteractiveMap::SaveCustomPointData(nlohmann::json& jObject, PointData* point)
	{
		if (!point->isCustom)
			return;

		auto jPoint = nlohmann::json::object();
		jPoint["id"] = point->id;
		jPoint["x_pos"] = point->levelPosition.x;
		jPoint["y_pos"] = point->levelPosition.y;
		jPoint["creation_timestamp"] = point->creationTimestamp;
		jObject.push_back(jPoint);
	}

	void InteractiveMap::SaveCompletedPointData(nlohmann::json& jObject, PointData* point)
	{
		if (!point->completed)
			return;

		auto jPoint = nlohmann::json::object();
		jPoint["point_id"] = point->id;
		jPoint["complete_timestamp"] = point->completeTimestamp;
		jObject.push_back(jPoint);
	}

	cheat::feature::InteractiveMap::PointData InteractiveMap::ParsePointData(const nlohmann::json& data)
	{
		return { data["id"], 0, 0, { data["x_pos"], data["y_pos"] }, false, 0};
	}

	void InteractiveMap::LoadLabelData(const nlohmann::json& data, uint32_t sceneID, uint32_t labelID)
	{
		auto& sceneData = m_ScenesData[sceneID];
		auto& labelEntry = sceneData.labels[labelID];

		labelEntry.id = labelID;
		labelEntry.sceneID = sceneID;
        labelEntry.name = data["name"];
        labelEntry.clearName = data["clear_name"];
        labelEntry.enabled = new config::field::BaseField<bool>(labelEntry.name,
            fmt::format("{}_{}", sceneID, labelEntry.clearName),
            "InteractiveMapFilters", false);

        config::AddField(*labelEntry.enabled);

        for (auto& pointJsonData : data["points"])
        {
			PointData data = ParsePointData(pointJsonData);
			data.labelID = labelID;
			data.sceneID = sceneID;

			labelEntry.points[data.id] = data;
        }

        sceneData.nameToLabel[labelEntry.clearName] = &labelEntry;
	}

	void InteractiveMap::LoadCategorieData(const nlohmann::json& data, uint32_t sceneID)
	{
        auto& sceneData = m_ScenesData[sceneID];
        auto& labels = sceneData.labels;
        auto& categories = sceneData.categories;
        
        categories.push_back({});
        auto& newCategory = categories.back();
        
        auto& children = newCategory.children;
        for (auto& child : data["children"])
        {
            if (labels.count(child) > 0)
                children.push_back(&labels[child]);
        }

        if (children.size() == 0)
        {
            categories.pop_back();
            return;
        }

        newCategory.name = data["name"];
	}

	void InteractiveMap::LoadSceneData(const nlohmann::json& data, uint32_t sceneID)
	{
		for (auto& [labelID, labelData] : data["labels"].items())
		{
			LoadLabelData(labelData, sceneID, std::stoi(labelID));
		}

        for (auto& categorie : data["categories"])
        {
            LoadCategorieData(categorie, sceneID);
        }
	}

	void InteractiveMap::LoadScenesData()
	{
        LoadSceneData(nlohmann::json::parse(ResourceLoader::Load("MapTeyvatData", RT_RCDATA)), 3);
        LoadSceneData(nlohmann::json::parse(ResourceLoader::Load("MapEnkanomiyaData", RT_RCDATA)), 5);
        LoadSceneData(nlohmann::json::parse(ResourceLoader::Load("MapUndegroundMinesData", RT_RCDATA)), 6);

        LOG_INFO("Interactive map data loaded successfully.");
    }

    struct ScalingData
    {
        float scale;
        float offset;
    };

    ScalingData ComputeScaling(app::Vector2 normal, app::Vector2 scaled)
    {
		// Just the equation system: 
		//	s[0] * scale + offset = n[0]
		//	s[1] * scale + offset = n[1]
		// Where: s = scaled, n = normal

        ScalingData scalingData {};
        scalingData.scale = (normal.y - normal.x) / (scaled.y - scaled.x);
        scalingData.offset = normal.x - scaled.x * scalingData.scale;
        
        return scalingData;
    }

	void InteractiveMap::ApplyScaling()
	{
        // For find scaling we need two objects' correct & scaled coordinates
        // Better find objects with one point on map
        app::Vector2 NormalPos1 = { 1301.2f, 2908.4f }; // AnemoHypostasis
        app::Vector2 NormalPos2 = { 1942.3f, 1308.9f }; // ElectroHypostasis

        app::Vector2 ScalledPos1 = m_ScenesData[3].nameToLabel["AnemoHypostasis"]->points.begin()->second.levelPosition;
        app::Vector2 ScalledPos2 = m_ScenesData[3].nameToLabel["ElectroHypostasis"]->points.begin()->second.levelPosition;

        ScalingData xScale = ComputeScaling({ NormalPos1.x, NormalPos2.x }, { ScalledPos1.x, ScalledPos2.x });
        ScalingData yScale = ComputeScaling({ NormalPos1.y, NormalPos2.y }, { ScalledPos1.y, ScalledPos2.y });

        app::Vector2 scale = { xScale.scale, yScale.scale };
        app::Vector2 offset = { xScale.offset, yScale.offset };

        LOG_DEBUG("Position scaling: scale %0.3f %0.3f, offset %0.3f %0.3f", scale.x, scale.y, offset.x, offset.y);
        for (auto& [sceneID, sceneData] : m_ScenesData)
        {
            for (auto& [labelID, labelData] : sceneData.labels)
            {
                for (auto& [pointID, point] : labelData.points)
                {
                    point.levelPosition = point.levelPosition * scale + offset;
                }
            }
        }
	}

	static bool IsMapActive()
	{
		auto uimanager = GET_SINGLETON(UIManager_1);
		if (uimanager == nullptr)
			return false;

		return app::UIManager_1_HasEnableMapCamera(uimanager, nullptr);
	}

	static app::Rect s_MapViewRect = { 0, 0, 1, 1 };
	void InteractiveMap::InLevelMapPageContext_UpdateView_Hook(app::InLevelMapPageContext* __this, MethodInfo* method)
	{
		callOrigin(InLevelMapPageContext_UpdateView_Hook, __this, method);
		s_MapViewRect = __this->fields._mapViewRect;
	}

	static ImVec2 LevelToMapScreenPos(const app::Vector2& levelPosition)
	{
		if (s_MapViewRect.m_Width == 0 || s_MapViewRect.m_Height == 0)
			return {};

		ImVec2 screenPosition;

		// Got position from 0 to 1
		screenPosition.x = (levelPosition.x - s_MapViewRect.m_XMin) / s_MapViewRect.m_Width;
		screenPosition.y = (levelPosition.y - s_MapViewRect.m_YMin) / s_MapViewRect.m_Height;

		// Scaling to screen position
		screenPosition.x = screenPosition.x * app::Screen_get_width(nullptr, nullptr);
		screenPosition.y = (1.0f - screenPosition.y) * app::Screen_get_height(nullptr, nullptr);

		return screenPosition;
	}

    inline ImVec2 operator - (const ImVec2& A, const float k)
	{
		return { A.x - k, A.y - k };
	}

	inline ImVec2 operator + (const ImVec2& A, const float k)
	{
		return { A.x + k, A.y + k };
	}

	static std::mutex _windowRectsMutex;
	static std::vector<ImRect> _windowRects;

	static void AddWindowRect()
	{
		_windowRects.push_back(
			{
				ImGui::GetWindowPos(),
				ImGui::GetWindowPos() + ImGui::GetWindowSize()
			}
		);
	}

	static app::MonoMiniMap* _monoMiniMap;
	void InteractiveMap::MonoMiniMap_Update_Hook(app::MonoMiniMap* __this, MethodInfo* method)
	{
		_monoMiniMap = __this;
		callOrigin(MonoMiniMap_Update_Hook, __this, method);
	}

	static bool IsMiniMapActive()
	{
		if (_monoMiniMap == nullptr)
			return false;

		SAFE_BEGIN();
		return app::Behaviour_get_isActiveAndEnabled(reinterpret_cast<app::Behaviour*>(_monoMiniMap), nullptr);
		SAFE_ERROR();
		_monoMiniMap = nullptr;
		return false;
		SAFE_END();
	}

	static float GetMinimapLevelDistance()
	{
		if (_monoMiniMap == nullptr)
			return {};

		return _monoMiniMap->fields._areaMinDistance;
	}

	void InteractiveMap::DrawExternal()
	{
		if (IsMiniMapActive())
			DrawMinimapPoints();

        if (!IsMapActive())
            return;

		auto mapManager = GET_SINGLETON(MapManager);
		if (mapManager == nullptr)
			return;

		// Draw windows
		{
			std::lock_guard _rectGuard(_windowRectsMutex);
			
			_windowRects.clear();

			bool menuOpened = ImGui::Begin("Interactive map", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
			AddWindowRect();

			if (menuOpened)
			{
				DrawMenu();

				if (!m_SeparatedWindows)
				{
					ImGui::Spacing();
					DrawFilters(false);
				}
				ImGui::End();
			}

			if (m_SeparatedWindows)
			{
				bool filtersOpened = ImGui::Begin("Filters", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
				AddWindowRect();

				if (filtersOpened)
				{
					DrawFilters();
					ImGui::End();
				}
			}
		}

		if (!m_Enabled)
			return;

        DrawPoints();
	}

	static bool IsRectInScreen(const ImRect& rect, const ImVec2& screenSize)
	{
		return rect.Min.x < screenSize.x && rect.Min.y < screenSize.y &&
			rect.Max.x > 0 && rect.Max.y > 0;
	}
	
	static void RenderPointCircle(const ImVec2& position, ImTextureID textureID, float transparency, float radius, bool isCustom = false)
	{
		ImVec2 imageStartPos = position - radius;
		ImVec2 imageEndPos = position + radius;

		auto draw = ImGui::GetBackgroundDrawList();
		draw->AddCircleFilled(position, radius, ImColor(0.23f, 0.26f, 0.32f, transparency));

		if (textureID)
		{
			draw->AddImageRounded(textureID, imageStartPos + 2.0f, imageEndPos - 2.0f,
				ImVec2(0, 0), ImVec2(1, 1), ImColor(1.0f, 1.0f, 1.0f, transparency), radius);
		}

		draw->AddCircle(position, radius, isCustom ? ImColor(0.11f, 0.69f, 0.11f, transparency) : ImColor(0.91f, 0.68f, 0.36f, transparency));
	}

	void InteractiveMap::DrawPoint(const PointData& pointData, const ImVec2& screenPosition, float radius, float radiusSquared, ImTextureID texture, bool selectable)
	{
		if (pointData.completed && !m_ShowCompleted)
			return;

		float transparency = pointData.completed ? m_CompletePointTransparency : 1.0f;

		if (/* m_SelectedPoint == nullptr && */!selectable || m_HoveredPoint != nullptr)
		{
			RenderPointCircle(screenPosition, texture, transparency, radius, pointData.isCustom);
			return;
		}

		ImVec2 mousePos = ImGui::GetMousePos();
		ImVec2 diffSize = screenPosition - mousePos;
		if (diffSize.x * diffSize.x + diffSize.y * diffSize.y > radiusSquared)
		{
			RenderPointCircle(screenPosition, texture, transparency, radius, pointData.isCustom);
			return;
		}

		m_HoveredPoint = const_cast<PointData*>(&pointData);
		radius *= 1.2f;

		RenderPointCircle(screenPosition, texture, transparency, radius, pointData.isCustom);

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			if (pointData.completed)
				UncompletePoint(m_HoveredPoint);
			else
				CompletePoint(m_HoveredPoint);
		}	
	}

    void InteractiveMap::DrawPoints()
	{
		// static uint32_t _lastSceneID = 0;
		static const float relativeSizeX = 821.0f;

		// TODO: Remove
		auto draw = ImGui::GetBackgroundDrawList();
		std::string fpsString = fmt::format("{:.1f}/{:.1f}", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		draw->AddText(ImVec2(100, 100), ImColor(0, 0, 0), fpsString.c_str());
		// 

		auto sceneID = game::GetCurrentMapSceneID();
		if (m_ScenesData.count(sceneID) == 0)
			return;
		
		//if (sceneID != _lastSceneID)
		//{
		//	_lastSceneID = sceneID;
		//	RefreshValidPoints();
		//}

		ImVec2 screenSize = { static_cast<float>(app::Screen_get_width(nullptr, nullptr)),
			static_cast<float>(app::Screen_get_height(nullptr, nullptr)) };

		
		auto iconSize = (m_DynamicSize && s_MapViewRect.m_Width != 0.0f) ? m_IconSize * (relativeSizeX / s_MapViewRect.m_Width) : m_IconSize;
		auto radius = iconSize / 2;
		auto radiusSquared = radius * radius;

		std::lock_guard<std::mutex> _guard(m_PointMutex);
		// m_SelectedPoint = nullptr;
		m_HoveredPoint = nullptr;

		auto& labels = m_ScenesData[sceneID].labels;
		for (auto& [labelID, label] : labels)
		{
			if (!label.enabled->value())
				continue;

			auto image = ImageLoader::GetImage(m_ShowHDIcons ? "HD" + label.clearName : label.clearName);
			for (auto& [pointID, point] : label.points)
			{
				auto screenPosition = LevelToMapScreenPos(point.levelPosition);

				ImRect imageRect = { screenPosition - radius, screenPosition + radius };
				if (!IsRectInScreen(imageRect, screenSize))
					continue;

				//ImGui::PushID(&point);
				DrawPoint(point, screenPosition, radius, radiusSquared, image ? image->textureID : nullptr);
				//ImGui::PopID();
			}
		}
	}

	struct ImCircle
	{
		ImVec2 center;
		float radius;

		bool Contains(const ImCircle& b)
		{
			if (b.radius > radius)
				return false;

			auto diff = b.center - center;
			auto distanceSqrd = std::pow(diff.x, 2) + std::pow(diff.y, 2);
			auto radiusDiffSqrd = std::pow(radius - b.radius, 2);
			return radiusDiffSqrd > distanceSqrd;
		}
	};

	static ImCircle GetMinimapCircle()
	{
		if (_monoMiniMap == nullptr)
			return {};

		UPDATE_DELAY_VAR(ImCircle, _miniMapCircle, 2000);

		auto uiManager = GET_SINGLETON(UIManager_1);
		if (uiManager == nullptr || uiManager->fields._sceneCanvas == nullptr)
			return {};

		auto back = _monoMiniMap->fields._grpMapBack;
		if (back == nullptr)
			return {};

		auto mapPos = app::Transform_get_position(reinterpret_cast<app::Transform*>(back), nullptr);
		auto center = app::Camera_WorldToScreenPoint(uiManager->fields._uiCamera, mapPos, nullptr);
		center.y = app::Screen_get_height(nullptr, nullptr) - center.y;

		auto mapRect = app::RectTransform_get_rect(back, nullptr);
		float scaleFactor = app::Canvas_get_scaleFactor(uiManager->fields._sceneCanvas, nullptr);
		_miniMapCircle = {
			ImVec2(center.x, center.y),
			(mapRect.m_Width * scaleFactor) / 2
		};

		return _miniMapCircle;
	}

	static float GetMinimapRotation()
	{
		if (_monoMiniMap == nullptr)
			return {};

		auto back = _monoMiniMap->fields._grpMiniBackRotate;
		if (back == nullptr)
			return {};

		auto rotation = app::Transform_get_rotation(reinterpret_cast<app::Transform*>(back), nullptr);

		app::Quaternion__Boxed boxed = { nullptr, nullptr, rotation };
		return app::Quaternion_get_eulerAngles(&boxed, nullptr).z;
	}

	void InteractiveMap::DrawMinimapPoints()
	{
		// Found by hands. Only in Teyvat (3rd scene), need also test another scenes.
		static const float minimapAreaLevelRadius = 175.0f;
		constexpr float PI = 3.14159265f;

		auto sceneID = game::GetCurrentPlayerSceneID();
		if (m_ScenesData.count(sceneID) == 0)
			return;

		auto rotation = GetMinimapRotation();
		ImVec2 rotationMult = ImVec2(1.0f, 0.0f);
		if (rotation != 0)
		{
			auto rad =  ( (360.0f - rotation) * PI ) / 180.0f;
			rotationMult = { sin(rad), cos(rad) };
		}

		ImCircle minimapCircle = GetMinimapCircle();
		auto avatarLevelPos = game::EntityManager::instance().avatar()->levelPosition();
		auto scale = minimapCircle.radius / minimapAreaLevelRadius;
		
		auto iconRadius = m_MinimapIconSize / 2;

		auto& labels = m_ScenesData[sceneID].labels;
		for (auto& [labelID, label] : labels)
		{
			if (!label.enabled->value())
				continue;

			auto image = ImageLoader::GetImage(m_ShowHDIcons ? "HD" + label.clearName : label.clearName);
			for (auto& [pointID, point] : label.points)
			{
				ImVec2 positionDiff = { point.levelPosition.x - avatarLevelPos.x, avatarLevelPos.y - point.levelPosition.y };
				positionDiff = positionDiff * scale;
				if (rotation != 0.0f)
				{
					positionDiff = {
						positionDiff.x * rotationMult.y - positionDiff.y * rotationMult.x,
						positionDiff.x * rotationMult.x + positionDiff.y * rotationMult.y
					};
				}


				ImVec2 screenPos = minimapCircle.center + positionDiff;
				if (!minimapCircle.Contains({ screenPos, iconRadius }))
					continue;

				//ImGui::PushID(&point);
				DrawPoint(point, screenPos, iconRadius, 0.0f, image ? image->textureID : nullptr, false);
				//ImGui::PopID();
			}
		}
	}

	// Blocking interacts when cursor on window

	static ImVec2 _lastMousePosition = {};
	static bool MouseInIMapWindow()
	{
		std::lock_guard _rectGuard(_windowRectsMutex);

		for (auto& rect : _windowRects)
		{
			if (rect.Contains(_lastMousePosition))
				return true;
		}
		return false;
	}

	void InteractiveMap::InLevelMapPageContext_ZoomMap_Hook(app::InLevelMapPageContext* __this, float value, MethodInfo* method)
	{
		if (MouseInIMapWindow())
			return;

		return callOrigin(InLevelMapPageContext_ZoomMap_Hook, __this, value, method);
	}

	void InteractiveMap::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& cancelled)
	{
		if (!IsMapActive())
			return;

		POINT mPos;
		GetCursorPos(&mPos);
		ScreenToClient(hWnd, &mPos);
		ImVec2 cursorPos = { static_cast<float>(mPos.x), static_cast<float>(mPos.y) };
		_lastMousePosition = cursorPos;

		if (!MouseInIMapWindow())
			return;

		switch (uMsg)
		{
		case WM_MOUSEWHEEL:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
			cancelled = true;
			break;
		default:
			break;
		}
	}

	void InteractiveMap::OnKeyUp(short key, bool& cancelled)
	{
		if (m_CompleteNearestPoint.value().IsPressed(key))
		{
			auto& manager = game::EntityManager::instance();
			auto point = FindNearestPoint(manager.avatar()->levelPosition(), m_PointFindRange, m_CompleteOnlyViewed, false, game::GetCurrentPlayerSceneID());
			if (point)
				CompletePoint(point);
		}

		if (m_RevertLatestCompletion.value().IsPressed(key))
		{
			RevertLatestPointCompleting();
		}
	}

	std::vector<InteractiveMap::LabelData*> InteractiveMap::FindLabelsByClearName(const std::string& clearName)
	{
		std::vector<InteractiveMap::LabelData*> labels;
		for (auto& [sceneID, sceneData] : m_ScenesData)
		{
			if (sceneData.nameToLabel.count(clearName) > 0)
				labels.push_back(sceneData.nameToLabel[clearName]);
		}
		return labels;
	}

	void InteractiveMap::InitializeEntityFilter(game::IEntityFilter* filter, const std::string& clearName)
	{
		auto labels = FindLabelsByClearName(clearName);
		if (labels.size() == 0)
		{
			LOG_DEBUG("Not found filter for item '%s'", clearName.c_str());
			return;
		}

		for (auto& label : labels)
		{
			label->filter = filter;
		}		
	}

	void InteractiveMap::InitializeEntityFilters()
	{
#define INIT_FILTER(category, filterName) InitializeEntityFilter(&game::filters::##category##::##filterName, #filterName)

		INIT_FILTER(collection, Book);
		INIT_FILTER(collection, Viewpoint);
		INIT_FILTER(collection, RadiantSpincrystal);
		//INIT_FILTER(collection, BookPage);
		//INIT_FILTER(collection, QuestInteract);
		INIT_FILTER(chest, CommonChest);
		INIT_FILTER(chest, ExquisiteChest);
		INIT_FILTER(chest, PreciousChest);
		INIT_FILTER(chest, LuxuriousChest);
		INIT_FILTER(chest, RemarkableChest);
		INIT_FILTER(featured, Anemoculus);
		INIT_FILTER(featured, CrimsonAgate);
		INIT_FILTER(featured, Electroculus);
		//INIT_FILTER(featured, Electrogranum);
		INIT_FILTER(featured, Geoculus);
		INIT_FILTER(featured, Lumenspar);
		//INIT_FILTER(featured, KeySigil);
		//INIT_FILTER(featured, ShrineOfDepth);
		//INIT_FILTER(featured, TimeTrialChallenge);
		//INIT_FILTER(guide, CampfireTorch);
		//INIT_FILTER(guide, MysteriousCarvings);
		//INIT_FILTER(guide, PhaseGate);
		//INIT_FILTER(guide, Pot);
		//INIT_FILTER(guide, RuinBrazier);
		//INIT_FILTER(guide, Stormstone);
		//INIT_FILTER(living, BirdEgg);
		//INIT_FILTER(living, ButterflyWings);
		//INIT_FILTER(living, Crab);
		//INIT_FILTER(living, CrystalCore);
		//INIT_FILTER(living, Fish);
		//INIT_FILTER(living, Frog);
		//INIT_FILTER(living, LizardTail);
		//INIT_FILTER(living, LuminescentSpine);
		//INIT_FILTER(living, Onikabuto);
		//INIT_FILTER(living, Starconch);
		//INIT_FILTER(living, UnagiMeat);
		INIT_FILTER(mineral, AmethystLump);
		INIT_FILTER(mineral, ArchaicStone);
		INIT_FILTER(mineral, CorLapis);
		INIT_FILTER(mineral, CrystalChunk);
		INIT_FILTER(mineral, CrystalMarrow);
		INIT_FILTER(mineral, ElectroCrystal);
		INIT_FILTER(mineral, IronChunk);
		INIT_FILTER(mineral, NoctilucousJade);
		INIT_FILTER(mineral, MagicalCrystalChunk);
		INIT_FILTER(mineral, StarSilver);
		INIT_FILTER(mineral, WhiteIronChunk);
		//INIT_FILTER(monster, AbyssMage);
		//INIT_FILTER(monster, FatuiAgent);
		//INIT_FILTER(monster, FatuiCicinMage);
		//INIT_FILTER(monster, FatuiMirrorMaiden);
		//INIT_FILTER(monster, FatuiSkirmisher);
		//INIT_FILTER(monster, Geovishap);
		//INIT_FILTER(monster, GeovishapHatchling);
		//INIT_FILTER(monster, Hilichurl);
		//INIT_FILTER(monster, Mitachurl);
		//INIT_FILTER(monster, Nobushi);
		//INIT_FILTER(monster, RuinGuard);
		//INIT_FILTER(monster, RuinHunter);
		//INIT_FILTER(monster, RuinSentinel);
		//INIT_FILTER(monster, Samachurl);
		//INIT_FILTER(monster, Slime);
		//INIT_FILTER(monster, Specter);
		//INIT_FILTER(monster, TreasureHoarder);
		//INIT_FILTER(monster, UnusualHilichurl);
		//INIT_FILTER(monster, Whopperflower);
		//INIT_FILTER(monster, WolvesOfTheRift);
		INIT_FILTER(plant, AmakumoFruit);
		INIT_FILTER(plant, Apple);
		INIT_FILTER(plant, BambooShoot);
		INIT_FILTER(plant, Berry);
		INIT_FILTER(plant, CallaLily);
		INIT_FILTER(plant, Carrot);
		INIT_FILTER(plant, Cecilia);
		INIT_FILTER(plant, DandelionSeed);
		INIT_FILTER(plant, Dendrobium);
		INIT_FILTER(plant, FlamingFlowerStamen);
		INIT_FILTER(plant, FluorescentFungus);
		INIT_FILTER(plant, GlazeLily);
		INIT_FILTER(plant, Horsetail);
		INIT_FILTER(plant, JueyunChili);
		INIT_FILTER(plant, LavenderMelon);
		INIT_FILTER(plant, LotusHead);
		INIT_FILTER(plant, Matsutake);
		INIT_FILTER(plant, Mint);
		INIT_FILTER(plant, MistFlowerCorolla);
		INIT_FILTER(plant, Mushroom);
		INIT_FILTER(plant, NakuWeed);
		INIT_FILTER(plant, PhilanemoMushroom);
		INIT_FILTER(plant, Pinecone);
		INIT_FILTER(plant, Qingxin);
		INIT_FILTER(plant, Radish);
		INIT_FILTER(plant, SakuraBloom);
		INIT_FILTER(plant, SangoPearl);
		INIT_FILTER(plant, SeaGanoderma);
		INIT_FILTER(plant, Seagrass);
		INIT_FILTER(plant, SilkFlower);
		INIT_FILTER(plant, SmallLampGrass);
		INIT_FILTER(plant, Snapdragon);
		INIT_FILTER(plant, Sunsettia);
		INIT_FILTER(plant, SweetFlower);
		INIT_FILTER(plant, Valberry);
		INIT_FILTER(plant, Violetgrass);
		INIT_FILTER(plant, WindwheelAster);
		INIT_FILTER(plant, Wolfhook);
		//INIT_FILTER(puzzle, AncientRime);
		//INIT_FILTER(puzzle, BakeDanuki);
		//INIT_FILTER(puzzle, BloattyFloatty);
		//INIT_FILTER(puzzle, CubeDevices);
		//INIT_FILTER(puzzle, EightStoneTablets);
		//INIT_FILTER(puzzle, ElectricConduction);
		//INIT_FILTER(puzzle, ElectroSeelie);
		//INIT_FILTER(puzzle, ElementalMonument);
		//INIT_FILTER(puzzle, FloatingAnemoSlime);
		//INIT_FILTER(puzzle, Geogranum);
		//INIT_FILTER(puzzle, GeoPuzzle);
		//INIT_FILTER(puzzle, LargeRockPile);
		//INIT_FILTER(puzzle, LightUpTilePuzzle);
		//INIT_FILTER(puzzle, LightningStrikeProbe);
		//INIT_FILTER(puzzle, MistBubble);
		//INIT_FILTER(puzzle, PirateHelm);
		//INIT_FILTER(puzzle, PressurePlate);
		//INIT_FILTER(puzzle, SeelieLamp);
		//INIT_FILTER(puzzle, Seelie);
		//INIT_FILTER(puzzle, SmallRockPile);
		//INIT_FILTER(puzzle, StormBarrier);
		//INIT_FILTER(puzzle, SwordHilt);
		//INIT_FILTER(puzzle, TorchPuzzle);
		//INIT_FILTER(puzzle, UniqueRocks);
		//INIT_FILTER(puzzle, WindmillMechanism);

#undef  INIT_FILTER
	}

	void InteractiveMap::InitializeGatherDetectItems()
	{
#define INIT_DETECT_ITEM(name) \
			for (auto& label : FindLabelsByClearName(#name)) \
			{ \
				label->supportGatherDetect = true; \
			} \
		
		INIT_DETECT_ITEM(CommonChest);
		INIT_DETECT_ITEM(ExquisiteChest);
		INIT_DETECT_ITEM(PreciousChest);
		INIT_DETECT_ITEM(LuxuriousChest);
		INIT_DETECT_ITEM(RemarkableChest);

		INIT_DETECT_ITEM(Anemoculus);
		INIT_DETECT_ITEM(CrimsonAgate);
		INIT_DETECT_ITEM(Electroculus);
		INIT_DETECT_ITEM(Geoculus);
		INIT_DETECT_ITEM(Lumenspar);

#undef INIT_DETECT_ITEM
	}

}