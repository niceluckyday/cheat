// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_IL2CPP_H
#define PCH_IL2CPP_H

// add headers that you want to pre-compile here
#include "il2cpp-appdata.h"

#include <iostream>
#include <filesystem>
#include <sstream>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <queue>
#include <optional>
#include <random>

#include <Windows.h>

#include <cheat-base/Logger.h>
#include <cheat-base/config/Config.h>
#include <cheat-base/util.h>
#include <cheat-base/cheat/CheatManager.h>
#include <cheat-base/cheat/Feature.h>
#include <cheat-base/cheat/IGameMisc.h>
#include <cheat-base/cheat/misc/Settings.h>
#include <cheat-base/PipeTransfer.h>
#include <cheat-base/HookManager.h>
#include <cheat-base/Patch.h>
#include <cheat-base/Event.h>
#include <cheat-base/render/gui-util.h>

#include <imgui.h>
#include <fmt/format.h>
#include <magic_enum.hpp>

#endif //PCH_IL2CPP_H