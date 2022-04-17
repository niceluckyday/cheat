#include "pch-il2cpp.h"
#include "ESP.h"

#include <helpers.h>
#include <algorithm>

#include <cheat/events.h>
#include <cheat/game/EntityManager.h>
#include "ESPRender.h"
#include <cheat/game/filters.h>

namespace cheat::feature 
{
	ESP::ESP() : Feature(),
		NF(m_Enabled, "ESP", "ESP", false),

        NF(m_DrawBoxMode, "Draw mode", "ESP", DrawMode::Box),
        NF(m_Fill, "Fill box/rentangle", "ESP", false),
        NF(m_FillTransparency, "Fill transparency", "ESP", 0.5f),

		NF(m_DrawLine, "Draw line", "ESP", false),
        NF(m_DrawDistance, "Draw distance", "ESP", false),
        NF(m_DrawName, "Draw name", "ESP", false),

        NF(m_FontSize, "Font size", "ESP", 12.0f),
        NF(m_FontColor, "Font color", "ESP", ImColor(255, 255, 255)),
		NF(m_ApplyGlobalFontColor, "Apply global font colors", "ESP", false),

        NF(m_MinSize, "Min in world size", "ESP", 0.5f),
		NF(m_Range, "Range", "ESP", 100.0f)
    {
		cheat::events::KeyUpEvent += MY_METHOD_HANDLER(ESP::OnKeyUp);
		InstallFilters();
	}


    const FeatureGUIInfo& ESP::GetGUIInfo() const
    {
        static const FeatureGUIInfo info { "", "ESP", false };
        return info;
    }

    void ESP::DrawMain()
    {
		BeginGroupPanel("General", ImVec2(-1, 0));

		ConfigWidget("ESP Enabled", m_Enabled, "Show filtered object through obstacles.");
        ConfigWidget(m_Range, 1.0f, 1.0f, 200.0f);
        
        ConfigWidget(m_DrawBoxMode, "Select the mode of box drawing.");
		ConfigWidget(m_Fill);
		ConfigWidget(m_FillTransparency, 0.01f, 0.0f, 1.0f, "Transparency of filled part.");

        ImGui::Spacing();
        ConfigWidget(m_DrawLine,     "Show line from character to object on screen.");
        ConfigWidget(m_DrawName,     "Draw name about object.");
        ConfigWidget(m_DrawDistance, "Draw distance about object.");

        ImGui::Spacing();
        ConfigWidget(m_FontSize, 0.05f, 1.0f, 100.0f, "Font size of name or distance.");
        ConfigWidget(m_FontColor, "Color of name or distance text font.");
		ConfigWidget(m_ApplyGlobalFontColor, "Override all color settings with above font color setting.\n"
			"Turn off to revert to custom settings.");

        ConfigWidget(m_MinSize, 0.05f, 0.1f, 200.0f, "Minimal object size in world.\n"
            "Some entities have not bounds or bounds is too small, this parameter help set minimal size of this type object.");
		
		EndGroupPanel();

		for (auto& [section, filters] : m_Sections)
		{
			DrawSection(section, filters);
		}
    }

    bool ESP::NeedStatusDraw() const
	{
        return m_Enabled;
    }

    void ESP::DrawStatus() 
    { 
        ImGui::Text("ESP [%.01fm|%s%s%s]", m_Range.value(), 
            m_DrawBoxMode ? "O" : "", 
            m_Fill ? "F" : "", 
            m_DrawLine ? "L" : "");
    }

    ESP& ESP::GetInstance()
    {
        static ESP instance;
        return instance;
    }

	void ESP::AddFilter(const std::string& section, const std::string& name, game::IEntityFilter* filter)
	{
		if (m_Sections.count(section) == 0)
			m_Sections[section] = {};

		auto& filters = m_Sections[section];
		filters.push_back({ new config::field::ESPItemField(name, name, section), filter });
		
		auto& last = filters.back();
		config::AddField(*last.first);
	}

	void ESP::DrawSection(const std::string& section, const Filters& filters)
	{
		BeginGroupPanel(section.c_str(), ImVec2(-1, 0));

		for (auto& [field, filter] : filters)
		{
			ImGui::PushID(field);
			DrawFilterField(*field);
			ImGui::PopID();
		}

		if (ImGui::TreeNode(this, "Hotkeys"))
		{
			for (auto& [field, filter] : filters)
			{
				ImGui::PushID(field);

				auto& hotkey = field->valuePtr()->m_EnabledHotkey;
				if (InputHotkey(field->GetName().c_str(), &hotkey, true))
					field->Check();

				ImGui::PopID();
			}

			ImGui::TreePop();
		}

		EndGroupPanel();
	}

	void ESP::DrawFilterField(config::field::ESPItemField& field)
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

	void ESP::DrawExternal()
	{
		auto draw = ImGui::GetBackgroundDrawList();

		std::string fpsString = fmt::format("{:.1f}/{:.1f}", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		draw->AddText(ImVec2(100, 100), ImColor(0, 0, 0), fpsString.c_str());

		auto& esp = ESP::GetInstance();
		if (!esp.m_Enabled)
			return;

		esp::render::PrepareFrame();

		auto& entityManager = game::EntityManager::instance();

		for (auto& entity : entityManager.entities())
		{
			if (entityManager.avatar()->distance(entity) > esp.m_Range)
				continue;

			for (auto& [section, filters] : m_Sections)
			{
				for (auto& [field, filter] : filters)
				{
					auto& entry = *field->valuePtr();
					if (!entry.m_Enabled || !m_FilterExecutor.ApplyFilter(entity, filter))
						continue;

					ImColor entityColor = entry.m_Color;
					esp::render::DrawEntity(entry.m_Name, entity, entityColor);
					break;
				}
			}
		}
	}

	void ESP::OnKeyUp(short key, bool& cancelled)
	{
		for (auto& [section, filters] : m_Sections)
		{
			for (auto& [field, filter] : filters)
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

	std::string SplitWords(const std::string& value)
	{
		std::stringstream outStream;
		std::stringstream inStream(value);

		char ch;
		inStream >> ch;
		outStream << ch;
		while (inStream >> ch)
		{
			if (isupper(ch))
				outStream << " ";
			outStream << ch;
		}
		return outStream.str();
	}

	std::string MakeCapital(std::string value)
	{
		if (islower(value[0]))
			value[0] = toupper(value[0]);
		return value;
	}

#define ADD_FILTER_FIELD(section, name) AddFilter(MakeCapital(#section), SplitWords(#name), &game::filters::##section##::##name##)
	void ESP::InstallFilters()
	{
		ADD_FILTER_FIELD(collection, Book);
		ADD_FILTER_FIELD(collection, Viewpoint);
		ADD_FILTER_FIELD(collection, RadiantSpincrystal);
		ADD_FILTER_FIELD(collection, BookPage);

		ADD_FILTER_FIELD(chest, Common);
		ADD_FILTER_FIELD(chest, Exquisite);
		ADD_FILTER_FIELD(chest, Precious);
		ADD_FILTER_FIELD(chest, Luxurious);
		ADD_FILTER_FIELD(chest, Remarkable);

		ADD_FILTER_FIELD(featured, Anemoculus);
		ADD_FILTER_FIELD(featured, CrimsonAgate);
		ADD_FILTER_FIELD(featured, Electroculus);
		ADD_FILTER_FIELD(featured, Electrogranum);
		ADD_FILTER_FIELD(featured, Geoculus);
		ADD_FILTER_FIELD(featured, ShrineOfDepth);
		ADD_FILTER_FIELD(featured, TimeTrial);

		ADD_FILTER_FIELD(guide, Campfire);
		ADD_FILTER_FIELD(guide, MysteriousCarvings);
		ADD_FILTER_FIELD(guide, PhaseGate);
		ADD_FILTER_FIELD(guide, Pot);
		ADD_FILTER_FIELD(guide, RuinBrazier);
		ADD_FILTER_FIELD(guide, Stormstone);

		ADD_FILTER_FIELD(living, BirdEgg);
		ADD_FILTER_FIELD(living, ButterflyWings);
		ADD_FILTER_FIELD(living, Crab);
		ADD_FILTER_FIELD(living, CrystalCore);
		ADD_FILTER_FIELD(living, Fish);
		ADD_FILTER_FIELD(living, Frog);
		ADD_FILTER_FIELD(living, LizardTail);
		ADD_FILTER_FIELD(living, LuminescentSpine);
		ADD_FILTER_FIELD(living, Onikabuto);
		ADD_FILTER_FIELD(living, Starconch);
		ADD_FILTER_FIELD(living, UnagiMeat);

		ADD_FILTER_FIELD(mineral, AmethystLump);
		ADD_FILTER_FIELD(mineral, ArchaicStone);
		ADD_FILTER_FIELD(mineral, CorLapis);
		ADD_FILTER_FIELD(mineral, CrystalChunk);
		ADD_FILTER_FIELD(mineral, CrystalMarrow);
		ADD_FILTER_FIELD(mineral, ElectroCrystal);
		ADD_FILTER_FIELD(mineral, IronChunk);
		ADD_FILTER_FIELD(mineral, NoctilucousJade);
		ADD_FILTER_FIELD(mineral, StarSilver);
		ADD_FILTER_FIELD(mineral, WhiteIronChunk);

		ADD_FILTER_FIELD(monster, AbyssMage);
		ADD_FILTER_FIELD(monster, FatuiAgent);
		ADD_FILTER_FIELD(monster, FatuiCicinMage);
		ADD_FILTER_FIELD(monster, FatuiMirrorMaiden);
		ADD_FILTER_FIELD(monster, FatuiSkirmisher);
		ADD_FILTER_FIELD(monster, Geovishap);
		ADD_FILTER_FIELD(monster, GeovishapHatchling);
		ADD_FILTER_FIELD(monster, Hilichurl);
		ADD_FILTER_FIELD(monster, Mitachurl);
		ADD_FILTER_FIELD(monster, Nobushi);
		ADD_FILTER_FIELD(monster, RuinGuard);
		ADD_FILTER_FIELD(monster, RuinHunter);
		ADD_FILTER_FIELD(monster, RuinSentinel);
		ADD_FILTER_FIELD(monster, Shamanchurl);
		ADD_FILTER_FIELD(monster, Slime);
		ADD_FILTER_FIELD(monster, Specter);
		ADD_FILTER_FIELD(monster, TreasureHoarder);
		ADD_FILTER_FIELD(monster, UnusualHilichurl);
		ADD_FILTER_FIELD(monster, Whopperflower);
		ADD_FILTER_FIELD(monster, WolvesOfTheRift);

		ADD_FILTER_FIELD(plant, AmakumoFruit);
		ADD_FILTER_FIELD(plant, Apple);
		ADD_FILTER_FIELD(plant, BambooShoot);
		ADD_FILTER_FIELD(plant, Berry);
		ADD_FILTER_FIELD(plant, CallaLily);
		ADD_FILTER_FIELD(plant, Carrot);
		ADD_FILTER_FIELD(plant, Cecilia);
		ADD_FILTER_FIELD(plant, DandelionSeed);
		ADD_FILTER_FIELD(plant, Dendrobium);
		ADD_FILTER_FIELD(plant, FlamingFlower);
		ADD_FILTER_FIELD(plant, FluorescentFungus);
		ADD_FILTER_FIELD(plant, GlazeLily);
		ADD_FILTER_FIELD(plant, Horsetail);
		ADD_FILTER_FIELD(plant, JueyunChili);
		ADD_FILTER_FIELD(plant, LavenderMelon);
		ADD_FILTER_FIELD(plant, LotusHead);
		ADD_FILTER_FIELD(plant, Matsutake);
		ADD_FILTER_FIELD(plant, Mint);
		ADD_FILTER_FIELD(plant, MistFlower);
		ADD_FILTER_FIELD(plant, Mushroom);
		ADD_FILTER_FIELD(plant, NakuWeed);
		ADD_FILTER_FIELD(plant, PhilanemoMushroom);
		ADD_FILTER_FIELD(plant, Pinecone);
		ADD_FILTER_FIELD(plant, Qingxin);
		ADD_FILTER_FIELD(plant, Radish);
		ADD_FILTER_FIELD(plant, SakuraBloom);
		ADD_FILTER_FIELD(plant, SangoPearl);
		ADD_FILTER_FIELD(plant, SeaGanoderma);
		ADD_FILTER_FIELD(plant, Seagrass);
		ADD_FILTER_FIELD(plant, SilkFlower);
		ADD_FILTER_FIELD(plant, SmallLampGrass);
		ADD_FILTER_FIELD(plant, Snapdragon);
		ADD_FILTER_FIELD(plant, Sunsettia);
		ADD_FILTER_FIELD(plant, SweetFlower);
		ADD_FILTER_FIELD(plant, Valberry);
		ADD_FILTER_FIELD(plant, Violetgrass);
		ADD_FILTER_FIELD(plant, WindwheelAster);
		ADD_FILTER_FIELD(plant, Wolfhook);

		ADD_FILTER_FIELD(puzzle, AncientRime);
		ADD_FILTER_FIELD(puzzle, BakeDanuki);
		ADD_FILTER_FIELD(puzzle, BloattyFloatty);
		ADD_FILTER_FIELD(puzzle, CubeDevices);
		ADD_FILTER_FIELD(puzzle, EightStoneTablets);
		ADD_FILTER_FIELD(puzzle, ElectricConduction);
		ADD_FILTER_FIELD(puzzle, ElectroSeelie);
		ADD_FILTER_FIELD(puzzle, ElementalMonument);
		ADD_FILTER_FIELD(puzzle, FloatingAnemoSlime);
		ADD_FILTER_FIELD(puzzle, Geogranum);
		ADD_FILTER_FIELD(puzzle, LargeRockPile);
		ADD_FILTER_FIELD(puzzle, LightUpTilePuzzle);
		ADD_FILTER_FIELD(puzzle, LightningStrikeProbe);
		ADD_FILTER_FIELD(puzzle, MistBubble);
		ADD_FILTER_FIELD(puzzle, PirateHelm);
		ADD_FILTER_FIELD(puzzle, PressurePlate);
		ADD_FILTER_FIELD(puzzle, SeelieLamp);
		ADD_FILTER_FIELD(puzzle, Seelie);
		ADD_FILTER_FIELD(puzzle, SmallRockPile);
		ADD_FILTER_FIELD(puzzle, StormBarrier);
		ADD_FILTER_FIELD(puzzle, SwordHilt);
		ADD_FILTER_FIELD(puzzle, TorchPuzzle);
		ADD_FILTER_FIELD(puzzle, UniqueRocks);
		ADD_FILTER_FIELD(puzzle, WindmillMechanism);
	}
#undef ADD_FILTER_FIELD
}

