#include "pch-il2cpp.h"

#include "protection-bypass.h"

#include "util/HookManager.h"
#include "util/Config.h"

#include <string>
#include <util/close-handle.h>
#include <iostream>
#include <util/Logger.h>

static std::map<int32_t, std::string> correctSignatures;

app::Byte__Array* RecordUserData_Hook(int32_t nType) {

    if (correctSignatures.count(nType))
    {
        auto byteClass = app::GetIl2Classes()[0x25];

        auto content = correctSignatures[nType];
        auto newArray = (app::Byte__Array*)il2cpp_array_new(byteClass, content.size());
        memmove_s(newArray->vector, content.size(), content.data(), content.size());

        return newArray;
    }

    app::Byte__Array* result = callOrigin(RecordUserData_Hook, nType);
    auto length = app::Array_get_Length(reinterpret_cast<app::Array*>(result), nullptr);

    if (length == 0)
        return result;


    auto stringValue = std::string((char*)result->vector, length);
    correctSignatures[nType] = stringValue;

    LOG_DEBUG("Sniffed correct signature for type %d value '%s'", nType, stringValue.c_str());

    return result;
}


void InitProtectionBypass() {
    HookManager::install(app::Unity_RecordUserData, RecordUserData_Hook);
    LOG_DEBUG("Hooked UnityPlayer::RecordUserData. Origin at 0x%p", HookManager::getOrigin(RecordUserData_Hook));

    for (int i = 0; i < 4; i++) {
        LOG_TRACE("Emulating call of RecordUserData with type %d", i);
        app::Application_RecordUserData(nullptr, i, nullptr);
    }

    if (Config::cfgDisableMhyprot.GetValue()) {
        LOG_TRACE("Trying to close mhyprot handle.");
        if (CloseHandleByName(L"\\Device\\mhyprot2"))
            LOG_INFO("Mhyprot2 handle successfuly closed. Happy hacking ^)");
        else
            LOG_ERROR("Failed closing mhyprot2 handle. Maybe dev updated anti-cheat.");
    }
    
}