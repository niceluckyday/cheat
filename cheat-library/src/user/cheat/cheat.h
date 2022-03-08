#pragma once
#include <il2cpp-appdata.h>

void InitCheats();

void InitMapTPHooks();
void TeleportToPosition(app::Vector3 position, bool needCalculateHeight = false);

void InitPlayerCheats();
void InitDebugHooks();
void InitProtectionBypass();
void InitWorldCheats();