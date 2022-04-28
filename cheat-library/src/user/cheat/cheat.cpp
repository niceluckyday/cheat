#include <pch-il2cpp.h>
#include "cheat.h"

#include <helpers.h>
#include <resource.h>

#include <cheat/events.h>
#include <cheat/GenshinMisc.h>

#include <cheat-base/cheat/misc/Settings.h>

#include <cheat/misc/ProtectionBypass.h>
#include <cheat/misc/Hotkeys.h>
#include <cheat/misc/Debug.h>

#ifdef _PACKET_SNIFFER
#include <cheat/misc/sniffer/PacketSniffer.h>
#endif

#include <cheat/player/GodMode.h>
#include <cheat/player/InfiniteStamina.h>
#include <cheat/player/NoCD.h>
#include <cheat/player/NoClip.h>
#include <cheat/player/RapidFire.h>

#include <cheat/world/AutoLoot.h>
#include <cheat/world/DialogSkip.h>
#include <cheat/world/DumbEnemies.h>
#include <cheat/world/ElementalSight.h>
#include <cheat/world/KillAura.h>
#include <cheat/world/MobVacuum.h>
#include <cheat/world/AutoTreeFarm.h>
#include <cheat/world/AutoDestroy.h>

#include <cheat/teleport/ChestTeleport.h>
#include <cheat/teleport/MapTeleport.h>
#include <cheat/teleport/OculiTeleport.h>
#include <cheat/teleport/CustomTeleports.h>

#include <cheat/esp/ESP.h>
#include <cheat/imap/InteractiveMap.h>

#include <cheat/world/AutoFish.h>

#include <cheat/ILPatternScanner.h>

namespace cheat 
{
	static void InstallEventHooks();

	void Init()
	{
		auto& protectionBypass = feature::ProtectionBypass::GetInstance();
		protectionBypass.Init();

		InstallEventHooks();

		CheatManager& manager = CheatManager::GetInstance();

#define FEAT_INST(name) &feature::##name##::GetInstance()
		manager.AddFeatures({
			&protectionBypass,
			FEAT_INST(Settings),
			FEAT_INST(Hotkeys),
			FEAT_INST(Debug),

#ifdef _PACKET_SNIFFER
			FEAT_INST(PacketSniffer),
#endif

			FEAT_INST(GodMode),
			FEAT_INST(InfiniteStamina),
			FEAT_INST(NoCD),
			FEAT_INST(NoClip),
			FEAT_INST(RapidFire),

			FEAT_INST(AutoLoot),
			FEAT_INST(AutoTreeFarm),
			FEAT_INST(AutoDestroy),
			FEAT_INST(DialogSkip),
			FEAT_INST(DumbEnemies),
			FEAT_INST(ElementalSight),
			FEAT_INST(KillAura),
			FEAT_INST(MobVacuum),

			FEAT_INST(ChestTeleport),
			FEAT_INST(OculiTeleport),
			FEAT_INST(MapTeleport),
			FEAT_INST(CustomTeleports),

			FEAT_INST(ESP),
			FEAT_INST(InteractiveMap),

			FEAT_INST(AutoFish)

			});
#undef FEAT_INST

		manager.SetModuleOrder({
			"Player",
			"World",
			"Teleport",
			"ESP",
			"Hotkeys",
			"Settings",
			"Debug"
			});

		LPBYTE pFontData = nullptr;
		DWORD dFontSize = 0;
		if (!ResourceLoader::LoadEx("ImGui_Font", RT_RCDATA, pFontData, dFontSize))
			LOG_WARNING("Failed to get font from resources.");

		manager.Init(pFontData, dFontSize, &GenshinMisc::GetInstance());
	}

	static void GameManager_Update_Hook(app::GameManager* __this, MethodInfo* method)
	{
		SAFE_BEGIN();
		events::GameUpdateEvent();
		SAFE_EEND();
		
		callOrigin(GameManager_Update_Hook, __this, method);
	}

	static void LevelSyncCombatPlugin_RequestSceneEntityMoveReq_Hook(app::BKFGGJFIIKC* __this, uint32_t entityId, app::MotionInfo* syncInfo,
		bool isReliable, uint32_t relseq, MethodInfo* method)
	{
		events::MoveSyncEvent(entityId, syncInfo);
		callOrigin(LevelSyncCombatPlugin_RequestSceneEntityMoveReq_Hook, __this, entityId, syncInfo, isReliable, relseq, method);
	}

	static void InstallEventHooks() 
	{
		HookManager::install(app::GameManager_Update, GameManager_Update_Hook);
		HookManager::install(app::LevelSyncCombatPlugin_RequestSceneEntityMoveReq, LevelSyncCombatPlugin_RequestSceneEntityMoveReq_Hook);
	}

}

