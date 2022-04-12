#include "pch-il2cpp.h"
#include "ESPSectionBase.h"

#include <cheat/esp/ESP.h>
#include <cheat/events.h>
namespace cheat::feature::esp
{
	ESPSectionBase::ESPSectionBase(std::string filterName) : m_Name(filterName)
	{
		cheat::events::KeyUpEvent += MY_METHOD_HANDLER(ESPSectionBase::OnKeyUp);
	}

	void ESPSectionBase::AddFilter(const std::string& name, IEntityFilter* filter)
	{
		m_Filters.push_back({ new config::field::ESPFilterField(name, name, m_Name), filter });
		auto& last = m_Filters.back();
		config::AddField(*last.first);
	}

	const cheat::FeatureGUIInfo& ESPSectionBase::GetGUIInfo() const
	{
		static const FeatureGUIInfo info { m_Name + " filters", "ESP", true};
		return info;
	}

	void ESPSectionBase::DrawMain()
	{
		for (auto& [field, filter] : m_Filters)
		{
			ImGui::PushID(field);
			DrawFilterField(*field);
			ImGui::PopID();
		}

		if (ImGui::TreeNode(this, "Hotkeys"))
		{
			for (auto& [field, filter] : m_Filters)
			{
				ImGui::PushID(field);

				auto& hotkey = field->valuePtr()->m_EnabledHotkey;
				if (InputHotkey(field->GetName().c_str(), &hotkey, true))
					field->Check();

				ImGui::PopID();
			}

			ImGui::TreePop();
		}
	}

	void ESPSectionBase::DrawFilterField(config::field::ESPFilterField& field)
	{
		auto& entry = *field.valuePtr();
		bool changed = false;

		if (ImGui::Checkbox("## FilterEnabled", &entry.m_Enabled))
			changed = true;

		ImGui::SameLine();
		
		bool pickerChanged = ImGui::ColorEdit4("## ColorPick", reinterpret_cast<float*>(&entry.m_Color));
		if (IsValueChanged(&entry, pickerChanged))
			changed = true;

		ImGui::SameLine();
		
		ImGui::Text("%s", field.GetName().c_str());

		if (changed)
			field.Check();
	}

	void ESPSectionBase::DrawExternal()
	{
		auto& esp = ESP::GetInstance();
		if (!esp.m_Enabled)
			return;

		for (auto& [field, filter] : m_Filters)
		{
			for (auto& entity : game::GetEntities())
			{
				if (!field->valuePtr()->m_Enabled || !filter->IsValid(entity))
					continue;

				switch (esp.m_DrawBoxMode)
				{
				case ESP::DrawMode::Box:
					DrawBox(entity, field->valuePtr()->m_Color);
					break;
				case ESP::DrawMode::Rectangle:
					DrawRect(entity, field->valuePtr()->m_Color);
					break;
				default:
					break;
				}

				if (esp.m_DrawLine)
					DrawLine(entity, field->valuePtr()->m_Color);
			}
		}
	}

	app::Camera* GetMainCamera()
	{
		auto cameraEntity = game::GetMainCameraEntity();
		if (cameraEntity == nullptr)
			return nullptr;

		auto camera = cameraEntity->fields._camera_k__BackingField;
		if (camera == nullptr)
			return nullptr;

		if (!app::Behaviour_get_isActiveAndEnabled(reinterpret_cast<app::Behaviour*>(camera), nullptr))
			return nullptr;

		return camera;
	}

	std::optional<app::Vector3> GetResolutionScale()
	{
		auto camera = GetMainCamera();
		if (camera == nullptr)
			return {};

		auto pixelWidth = app::Camera_get_pixelWidth(camera, nullptr);
		auto pixelHeight = app::Camera_get_pixelHeight(camera, nullptr);

		if (pixelWidth == 0 || pixelHeight == 0)
			return {};

		auto screenWidth = app::Screen_get_width(nullptr, nullptr);
		auto screenHeight = app::Screen_get_height(nullptr, nullptr);

		if (screenWidth == 0 || screenHeight == 0)
			return {};

		if (screenHeight == pixelHeight && screenWidth == pixelWidth)
			return {};

		app::Vector3 resolutionScale;
		resolutionScale.x = static_cast<float>(screenWidth) / static_cast<float>(pixelWidth);
		resolutionScale.y = static_cast<float>(screenHeight) / static_cast<float>(pixelHeight);
		resolutionScale.z = 1;

		return resolutionScale;
	}

	app::Vector3 WorldToScreenPosScalled(const app::Vector3& relPosition)
	{
		auto camera = GetMainCamera();
		if (camera == nullptr)
			return {};

		auto screenPos = app::Camera_WorldToScreenPoint(camera, relPosition, nullptr);
		auto resolutionScale = GetResolutionScale();

		if (resolutionScale)
		{
			screenPos.x *= resolutionScale->x;
			screenPos.y *= resolutionScale->y;
		}
		
		screenPos.y = app::Screen_get_height(nullptr, nullptr) - screenPos.y;
		return screenPos;
	}

	struct Rect
	{
		float xMin, xMax;
		float yMin, yMax;

		bool empty()
		{
			return xMin == 0 && xMax == 0 && yMin == 0 && yMax == 0;
		}
	};

	struct BoxScreen
	{
		ImVec2 lowerTopLeft;
		ImVec2 lowerTopRight;
		ImVec2 lowerBottomLeft;
		ImVec2 lowerBottomRight;

		ImVec2 upperTopLeft;
		ImVec2 upperTopRight;
		ImVec2 upperBottomLeft;
		ImVec2 upperBottomRight;
	};

#undef min
#undef max

	ImVec2 FromVec3(const app::Vector3& vec3)
	{
		return { vec3.x, vec3.y };
	}

	std::optional<BoxScreen> GetEntityScreenBox(app::BaseEntity* entity)
	{
		auto camera = GetMainCamera();
		if (camera == nullptr)
			return {};

		auto& esp = ESP::GetInstance();

		auto gameObject = app::BaseEntity_get_gameObject(entity, nullptr);

		app::Bounds bounds;
		if (gameObject == nullptr)
		{
			auto entityPosition = game::GetRelativePosition(entity);
			bounds = { entityPosition, { esp.m_MinWorldSize, esp.m_MinWorldSize, esp.m_MinWorldSize } };
		}
		else
		{
			bounds = app::Utils_1_GetBounds(nullptr, gameObject, nullptr);
			if (bounds.m_Extents.x < esp.m_MinWorldSize &&
				bounds.m_Extents.y < esp.m_MinWorldSize &&
				bounds.m_Extents.z < esp.m_MinWorldSize)
				bounds.m_Extents = { esp.m_MinWorldSize, esp.m_MinWorldSize, esp.m_MinWorldSize };
		}

		auto min = bounds.m_Center - bounds.m_Extents;
		auto max = bounds.m_Center + bounds.m_Extents;

		BoxScreen box;
		app::Vector3 temp;
#define BOX_FIELD_SET(field, px, py, pz) temp = app::Camera_WorldToScreenPoint(camera, { px, py, pz }, nullptr);\
			if (temp.z < 1) return {};\
			box.##field = FromVec3(temp);

		BOX_FIELD_SET(lowerTopLeft,     min.x, min.y, max.z);
		BOX_FIELD_SET(lowerTopRight,    max.x, min.y, max.z);
		BOX_FIELD_SET(lowerBottomLeft,  min.x, min.y, min.z);
		BOX_FIELD_SET(lowerBottomRight, max.x, min.y, min.z);

		BOX_FIELD_SET(upperTopLeft,     min.x, max.y, max.z);
		BOX_FIELD_SET(upperTopRight,    max.x, max.y, max.z);
		BOX_FIELD_SET(upperBottomLeft,  min.x, max.y, min.z);
		BOX_FIELD_SET(upperBottomRight, max.x, max.y, min.z);

#undef BOX_FIELD_SET

		return box;
	}

	void ScaleBoxScreen(BoxScreen& boxScreen)
	{
		auto resolutionScale = GetResolutionScale();

		if (resolutionScale)
		{

#define SCALE_FIELD(field) boxScreen.##field##.x *= resolutionScale->x; boxScreen.##field##.y *= resolutionScale->y

			SCALE_FIELD(lowerTopLeft);
			SCALE_FIELD(lowerTopRight);
			SCALE_FIELD(lowerBottomLeft);
			SCALE_FIELD(lowerBottomRight);

			SCALE_FIELD(upperTopLeft);
			SCALE_FIELD(upperTopRight);
			SCALE_FIELD(upperBottomLeft);
			SCALE_FIELD(upperBottomRight);

#undef SCALE_FIELD

		}

		auto screenHeight = app::Screen_get_height(nullptr, nullptr);

#define FIX_Y(field) boxScreen.##field##.y = screenHeight - boxScreen.##field##.y

		FIX_Y(lowerTopLeft);
		FIX_Y(lowerTopRight);
		FIX_Y(lowerBottomLeft);
		FIX_Y(lowerBottomRight);

		FIX_Y(upperTopLeft);
		FIX_Y(upperTopRight);
		FIX_Y(upperBottomLeft);
		FIX_Y(upperBottomRight);

#undef FIX_Y

	}

	Rect GetEntityScreenRect(app::BaseEntity* entity)
	{		

		auto box = GetEntityScreenBox(entity);
		if (!box)
			return {};

		Rect boxRect{};

		boxRect.xMin = std::min({ box->lowerTopLeft.x, box->lowerTopRight.x, box->lowerBottomLeft.x, box->lowerBottomRight.x,
			box->upperTopLeft.x, box->upperTopRight.x, box->upperBottomRight.x, box->upperBottomLeft.x });
		boxRect.xMax = std::max({ box->lowerTopLeft.x, box->lowerTopRight.x, box->lowerBottomLeft.x, box->lowerBottomRight.x,
			box->upperTopLeft.x, box->upperTopRight.x, box->upperBottomRight.x, box->upperBottomLeft.x });
		
		boxRect.yMin = std::max({ box->lowerTopLeft.y, box->lowerTopRight.y, box->lowerBottomLeft.y, box->lowerBottomRight.y,
			box->upperTopLeft.y, box->upperTopRight.y, box->upperBottomRight.y, box->upperBottomLeft.y });
		boxRect.yMax = std::min({ box->lowerTopLeft.y, box->lowerTopRight.y, box->lowerBottomLeft.y, box->lowerBottomRight.y,
			box->upperTopLeft.y, box->upperTopRight.y, box->upperBottomRight.y, box->upperBottomLeft.y });

		auto resolutionScale = GetResolutionScale();
		if (resolutionScale)
		{
			boxRect.xMin *= resolutionScale->x;
			boxRect.xMax *= resolutionScale->x;

			boxRect.yMin *= resolutionScale->y;
			boxRect.yMax *= resolutionScale->y;
		}

		auto screenHeight = app::Screen_get_height(nullptr, nullptr);
		boxRect.yMin = screenHeight - boxRect.yMin;
		boxRect.yMax = screenHeight - boxRect.yMax;
		return boxRect;
	}

	void ESPSectionBase::DrawRect(app::BaseEntity* entity, const ImColor& color)
	{
		auto entityRect = GetEntityScreenRect(entity);
		if (entityRect.empty())
			return;

		auto& esp = ESP::GetInstance();
		auto draw = ImGui::GetBackgroundDrawList();

		auto pMin = ImVec2(entityRect.xMin, entityRect.yMin);
		auto pMax = ImVec2(entityRect.xMax, entityRect.yMax);
		if (esp.m_Fill)
		{
			ImColor newColor = color;
			newColor.Value.w = 1.0f - esp.m_FillTransparency;
			draw->AddRectFilled(pMin, pMax, newColor);
		}
		draw->AddRect(pMin, pMax, color);
	}

	// Callow: This way to drawing is slower than native
	void ESPSectionBase::DrawBox(app::BaseEntity* entity, const ImColor& color)
	{
		auto box = GetEntityScreenBox(entity);
		if (!box)
			return;

		ScaleBoxScreen(*box);

		auto draw = ImGui::GetBackgroundDrawList();
		draw->AddQuad(box->lowerBottomLeft, box->lowerTopLeft, box->lowerTopRight, box->lowerBottomRight, color);
		draw->AddQuad(box->upperBottomLeft, box->upperTopLeft, box->upperTopRight, box->upperBottomRight, color);

		//draw->AddLine(box->lowerBottomLeft,  box->upperBottomLeft, color);
		//draw->AddLine(box->lowerTopLeft,     box->upperTopLeft,    color);
		//draw->AddLine(box->lowerTopRight,    box->upperTopRight,   color);
		//draw->AddLine(box->lowerBottomRight, box->upperBottomRight,color);
	}

	void ESPSectionBase::DrawLine(app::BaseEntity* entity, const ImColor& color)
	{

	}

	void ESPSectionBase::OnKeyUp(short key, bool& cancelled)
	{
		for (auto& [field, filter] : m_Filters)
		{
			auto& entry = *field->valuePtr();
			if (entry.m_EnabledHotkey.IsPressed(key))
			{
				entry.m_Enabled = !entry.m_Enabled;
				field->Check();
			}
		}
	}



}