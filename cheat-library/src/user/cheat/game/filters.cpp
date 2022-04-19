#include <pch-il2cpp.h>

#include "filters.h"

namespace cheat::game::filters
{
	namespace collection
	{
		SimpleFilter Book = { app::EntityType__Enum_1::GatherObject, "SkillObj_EmptyGadget" };
		SimpleFilter Viewpoint = { app::EntityType__Enum_1::ViewPoint, "SkillObj_EmptyGadget" };
		ChestFilter RadiantSpincrystal = ChestFilter(game::Chest::ItemType::BGM);
		ChestFilter BookPage = ChestFilter(game::Chest::ItemType::BookPage);
		ChestFilter QuestInteract = ChestFilter(game::Chest::ItemType::QuestInteract);
	}

	namespace chest
	{
		ChestFilter CommonChest = ChestFilter(game::Chest::ChestRarity::Common);
		ChestFilter ExquisiteChest = ChestFilter(game::Chest::ChestRarity::Exquisite);
		ChestFilter PreciousChest = ChestFilter(game::Chest::ChestRarity::Precious);
		ChestFilter LuxuriousChest = ChestFilter(game::Chest::ChestRarity::Luxurious);
		ChestFilter RemarkableChest = ChestFilter(game::Chest::ChestRarity::Remarkable);

		ChestFilter SLocked = ChestFilter(game::Chest::ChestState::Locked);
		ChestFilter SInLock = ChestFilter(game::Chest::ChestState::InRock);
		ChestFilter SFrozen = ChestFilter(game::Chest::ChestState::Frozen);
		ChestFilter SBramble = ChestFilter(game::Chest::ChestState::Bramble);
		ChestFilter STrap = ChestFilter(game::Chest::ChestState::Trap);
	}

	namespace featured
	{
		SimpleFilter Anemoculus = { app::EntityType__Enum_1::GatherObject, "WindCrystalShell" };
		SimpleFilter CrimsonAgate = { app::EntityType__Enum_1::GatherObject, "Prop_Essence" };
		SimpleFilter Electroculus = { app::EntityType__Enum_1::GatherObject, "Prop_ElectricCrystal" };
		SimpleFilter Electrogranum = { app::EntityType__Enum_1::Gadget, "ThunderSeedCreate" };
		SimpleFilter Geoculus = { app::EntityType__Enum_1::GatherObject, "RockCrystalShell" };
		SimpleFilter Lumenspar = { app::EntityType__Enum_1::GatherObject, "CelestiaSplinter" };
		SimpleFilter KeySigil = { app::EntityType__Enum_1::GatherObject, "RuneContent" };
		SimpleFilter ShrineOfDepth = { app::EntityType__Enum_1::Gadget, "Temple" };
		SimpleFilter TimeTrialChallenge = { app::EntityType__Enum_1::Field, "Challengestarter_" };
	}

	namespace guide
	{
		SimpleFilter CampfireTorch = { app::EntityType__Enum_1::Gadget, "_FireBasin" };
		SimpleFilter MysteriousCarvings = { app::EntityType__Enum_1::Gadget, "_ReginStatue" };
		SimpleFilter PhaseGate = { app::EntityType__Enum_1::Field, "_TeleportHighway" };
		SimpleFilter Pot = { app::EntityType__Enum_1::Gadget, "_Cooking_" };
		SimpleFilter RuinBrazier = { app::EntityType__Enum_1::Gadget, "_AncientHeatSource" };
		SimpleFilter Stormstone = { app::EntityType__Enum_1::Gadget, "_ReginLamp" };
	}

	namespace living
	{
		SimpleFilter BirdEgg = { app::EntityType__Enum_1::GatherObject, "BirdEgg" };
		SimpleFilter ButterflyWings = { app::EntityType__Enum_1::EnvAnimal, "Butterfly" };
		SimpleFilter Crab = { app::EntityType__Enum_1::EnvAnimal, "Crab" };
		SimpleFilter CrystalCore = { app::EntityType__Enum_1::EnvAnimal, "Wisp" };
		SimpleFilter Fish = { app::EntityType__Enum_1::EnvAnimal, "Fish" };
		SimpleFilter Frog = { app::EntityType__Enum_1::EnvAnimal, "Frog" };
		SimpleFilter LizardTail = { app::EntityType__Enum_1::EnvAnimal, "Lizard" };
		SimpleFilter LuminescentSpine = { app::EntityType__Enum_1::EnvAnimal, "FireFly" };
		SimpleFilter Onikabuto = { app::EntityType__Enum_1::GatherObject, "Electrohercules" };
		SimpleFilter Starconch = { app::EntityType__Enum_1::GatherObject, "_Shell" };
		SimpleFilter UnagiMeat = { app::EntityType__Enum_1::EnvAnimal, "_Eel_" };
	}

	namespace mineral
	{
		SimpleFilter AmethystLump = { app::EntityType__Enum_1::GatherObject, "_Thundercrystal" };
		SimpleFilter ArchaicStone = { app::EntityType__Enum_1::GatherObject, "AncientOre" };
		SimpleFilter CorLapis = { app::EntityType__Enum_1::GatherObject, "_ElementRock" };
		SimpleFilter CrystalChunk = { app::EntityType__Enum_1::GatherObject, "_OreCrystal" };
		SimpleFilter CrystalMarrow = { app::EntityType__Enum_1::GatherObject, "_Crystalizedmarrow" };
		SimpleFilter ElectroCrystal = { app::EntityType__Enum_1::GatherObject, "_OreElectricRock" };
		SimpleFilter IronChunk = { app::EntityType__Enum_1::GatherObject, "_OreStone" };
		SimpleFilter NoctilucousJade = { app::EntityType__Enum_1::GatherObject, "_OreNightBerth" };
		SimpleFilter MagicalCrystalChunk = { app::EntityType__Enum_1::GatherObject, "_OreMagicCrystal" };
		SimpleFilter StarSilver = { app::EntityType__Enum_1::GatherObject, "_OreMoonMeteor" };
		SimpleFilter WhiteIronChunk = { app::EntityType__Enum_1::GatherObject, "_OreMetal" };
	}

	namespace monster
	{
		SimpleFilter AbyssMage = { app::EntityType__Enum_1::Monster, "_Abyss" };
		SimpleFilter FatuiAgent = { app::EntityType__Enum_1::Monster, "_Fatuus" };
		SimpleFilter FatuiCicinMage = { app::EntityType__Enum_1::Monster, "_Fatuus_Summoner" };
		SimpleFilter FatuiMirrorMaiden = { app::EntityType__Enum_1::Monster, "_Fatuus_Maiden" };
		SimpleFilter FatuiSkirmisher = { app::EntityType__Enum_1::Monster, "_Skirmisher" };
		SimpleFilter Geovishap = { app::EntityType__Enum_1::Monster, "_Drake" };
		SimpleFilter GeovishapHatchling = { app::EntityType__Enum_1::Monster, "_Wyrm" };
		SimpleFilter Hilichurl = { app::EntityType__Enum_1::Monster, "_Hili" };
		SimpleFilter Mitachurl = { app::EntityType__Enum_1::Monster, "_Brute" };
		SimpleFilter Nobushi = { app::EntityType__Enum_1::Monster, "_Samurai" };
		SimpleFilter RuinGuard = { app::EntityType__Enum_1::Monster, "_Defender" };
		SimpleFilter RuinHunter = { app::EntityType__Enum_1::Monster, "_Formathr" };
		SimpleFilter RuinSentinel = { app::EntityType__Enum_1::Monster, std::vector<std::string> {"_Konungmathr", "_Apparatus" } };
		SimpleFilter Samachurl = { app::EntityType__Enum_1::Monster, "_Shaman" };
		SimpleFilter Slime = { app::EntityType__Enum_1::Monster, "_Slime" };
		SimpleFilter Specter = { app::EntityType__Enum_1::Monster, "_Sylph" };
		SimpleFilter TreasureHoarder = { app::EntityType__Enum_1::Monster, "_Thoarder" };
		SimpleFilter UnusualHilichurl = { app::EntityType__Enum_1::Monster, "_Hili_Wei" };
		SimpleFilter Whopperflower = { app::EntityType__Enum_1::Monster, "_Mimik" };
		SimpleFilter WolvesOfTheRift = { app::EntityType__Enum_1::Monster, "_Hound_Kanis" };
	}

	namespace plant
	{
		SimpleFilter AmakumoFruit = { app::EntityType__Enum_1::GatherObject, "_Electroseed" };
		SimpleFilter Apple = { app::EntityType__Enum_1::GatherObject, "_Drop_Plant_Apple" };
		SimpleFilter BambooShoot = { app::EntityType__Enum_1::GatherObject, "_Bambooshoot" };
		SimpleFilter Berry = { app::EntityType__Enum_1::GatherObject, "_Raspberry" };
		SimpleFilter CallaLily = { app::EntityType__Enum_1::GatherObject, "_Plant_Callas" };
		SimpleFilter Carrot = { app::EntityType__Enum_1::GatherObject, "_Plant_Carrot" };
		SimpleFilter Cecilia = { app::EntityType__Enum_1::GatherObject, "_Cecilia" };
		SimpleFilter DandelionSeed = { app::EntityType__Enum_1::GatherObject, "_Plant_Dandelion" };
		SimpleFilter Dendrobium = { app::EntityType__Enum_1::GatherObject, "_Blooddendrobe" };
		SimpleFilter FlamingFlowerStamen = { app::EntityType__Enum_1::GatherObject, "_Flower_FireFlower" };
		SimpleFilter FluorescentFungus = { app::EntityType__Enum_1::GatherObject, "_Lampmushroom" };
		SimpleFilter GlazeLily = { app::EntityType__Enum_1::GatherObject, "_GlazedLily" };
		SimpleFilter Horsetail = { app::EntityType__Enum_1::GatherObject, "_HorseTail" };
		SimpleFilter JueyunChili = { app::EntityType__Enum_1::GatherObject, "_UltimateChilli_Fruit" };
		SimpleFilter LavenderMelon = { app::EntityType__Enum_1::GatherObject, "_Akebia" };
		SimpleFilter LotusHead = { app::EntityType__Enum_1::GatherObject, "_Lotus_Drop" };
		SimpleFilter Matsutake = { app::EntityType__Enum_1::GatherObject, "_Food_Matsutake" };
		SimpleFilter Mint = { app::EntityType__Enum_1::GatherObject, "_Plant_Mint" };
		SimpleFilter MistFlowerCorolla = { app::EntityType__Enum_1::GatherObject, "_Flower_IceFlower" };
		SimpleFilter Mushroom = { app::EntityType__Enum_1::GatherObject, "_Plant_Mushroom" };
		SimpleFilter NakuWeed = { app::EntityType__Enum_1::GatherObject, "_Howlgrass" };
		SimpleFilter PhilanemoMushroom = { app::EntityType__Enum_1::GatherObject, "_WindmilHunter" };
		SimpleFilter Pinecone = { app::EntityType__Enum_1::GatherObject, "_Drop_Plant_Pine" };
		SimpleFilter Qingxin = { app::EntityType__Enum_1::GatherObject, "_QingXin" };
		SimpleFilter Radish = { app::EntityType__Enum_1::GatherObject, "_Plant_Radish" };
		SimpleFilter SakuraBloom = { app::EntityType__Enum_1::GatherObject, "_Cherrypetals" };
		SimpleFilter SangoPearl = { app::EntityType__Enum_1::GatherObject, "_Coralpearl" };
		SimpleFilter SeaGanoderma = { app::EntityType__Enum_1::GatherObject, "_Electroanemones" };
		SimpleFilter Seagrass = { app::EntityType__Enum_1::GatherObject, "_Seagrass" };
		SimpleFilter SilkFlower = { app::EntityType__Enum_1::GatherObject, "_NiChang_Drop" };
		SimpleFilter SmallLampGrass = { app::EntityType__Enum_1::Field, "_Plant_Fuchsia" };
		SimpleFilter Snapdragon = { app::EntityType__Enum_1::GatherObject, "_Plant_Snapdragon" };
		SimpleFilter Sunsettia = { app::EntityType__Enum_1::GatherObject, "_SunsetFruit" };
		SimpleFilter SweetFlower = { app::EntityType__Enum_1::GatherObject, "_Plant_Whiteballet" };
		SimpleFilter Valberry = { app::EntityType__Enum_1::GatherObject, "_DropingBerry_Gather" };
		SimpleFilter Violetgrass = { app::EntityType__Enum_1::GatherObject, "_GlazedGrass" };
		SimpleFilter WindwheelAster = { app::EntityType__Enum_1::GatherObject, "_WindmilDaisy" };
		SimpleFilter Wolfhook = { app::EntityType__Enum_1::GatherObject, "_GogoFruit" };
	}

	namespace puzzle
	{
		SimpleFilter AncientRime = { app::EntityType__Enum_1::Gadget, "_IceSolidBulk" };
		SimpleFilter BakeDanuki = { app::EntityType__Enum_1::Monster, "Animal_Inu_Tanuki_" };
		SimpleFilter BloattyFloatty = { app::EntityType__Enum_1::Field, "_Flower_PongPongTree_" };
		AdvancedFilter CubeDevices = { std::vector<app::EntityType__Enum_1> {app::EntityType__Enum_1::Gadget, app::EntityType__Enum_1::Platform }, std::vector<std::string> {"_ElecStone", "_ElecSwitch" }};
		SimpleFilter EightStoneTablets = { app::EntityType__Enum_1::Gadget, "_HistoryBoard" };
		SimpleFilter ElectricConduction = { app::EntityType__Enum_1::Gear, "_ElectricPowerSource" };
		SimpleFilter ElectroSeelie = { app::EntityType__Enum_1::Platform, "_ElectricSeelie" };
		SimpleFilter ElementalMonument = { app::EntityType__Enum_1::Gear, "_ElemTablet" };
		SimpleFilter FloatingAnemoSlime = { app::EntityType__Enum_1::Platform, "_WindSlime" };
		SimpleFilter Geogranum = { app::EntityType__Enum_1::Gadget, "_Property_Prop_RockFragment" };
		SimpleFilter GeoPuzzle = { app::EntityType__Enum_1::Field, "_Rockstraight_" };
		SimpleFilter LargeRockPile = { app::EntityType__Enum_1::Gadget, std::vector<std::string> {"_ElecRocks", "_StonePile_02" } };
		SimpleFilter LightUpTilePuzzle = { app::EntityType__Enum_1::Field, "_TwinStoryFloor" };
		SimpleFilter LightningStrikeProbe = { app::EntityType__Enum_1::Gadget, "_MagneticGear" };
		SimpleFilter MistBubble = { app::EntityType__Enum_1::Platform, "_Suspiciousbubbles" };
		SimpleFilter PirateHelm = { app::EntityType__Enum_1::Field, "_PirateHelm" };
		SimpleFilter PressurePlate = { app::EntityType__Enum_1::Field, "Gear_Gravity" };
		SimpleFilter SeelieLamp = { app::EntityType__Enum_1::Field, "Gear_SeeliaLamp" };
		SimpleFilter Seelie = { app::EntityType__Enum_1::Platform, "Gear_Seelie" };
		SimpleFilter SmallRockPile = { app::EntityType__Enum_1::Gadget, "_StonePile_01" };
		SimpleFilter StormBarrier = { app::EntityType__Enum_1::Field, "_WindField_PushField" };
		SimpleFilter SwordHilt = { app::EntityType__Enum_1::Field, "_WastedSword_" };
		SimpleFilter TorchPuzzle = { app::EntityType__Enum_1::Gadget, "_ImmortalFire" };
		SimpleFilter UniqueRocks = { app::EntityType__Enum_1::Gadget, "_Chalcedony" };
		SimpleFilter WindmillMechanism = { app::EntityType__Enum_1::Gear, "Gear_Windmill" };
	}

	namespace combined
	{
		SimpleFilter Oculies = featured::Anemoculus + featured::CrimsonAgate + featured::Electroculus + 
			featured::Geoculus + featured::Lumenspar + featured::KeySigil;
		SimpleFilter Chests = { app::EntityType__Enum_1::Chest };
		SimpleFilter Monsters = { app::EntityType__Enum_1::Monster };
	}
}