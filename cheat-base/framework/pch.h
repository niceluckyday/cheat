// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_set>
#include <queue>
#include <sstream>
#include <mutex>
#include <optional>

#include <Windows.h>

#include <imgui.h>
#include <imconfig.h>
#include <backends/imgui_impl_dx11.h>

#include <SimpleIni.h>
#include <fmt/format.h>
#include <detours.h>

#include <cheat-base/Logger.h>

#endif //PCH_H
