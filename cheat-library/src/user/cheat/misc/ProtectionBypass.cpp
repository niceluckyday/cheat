#include "pch-il2cpp.h"
#include "ProtectionBypass.h"

#include <imgui.h>
#include <common/util.h>
#include <helpers.h>
#include <gui/gui-util.h>
#include <common/HookManager.h>
#include <common/close-handle.h>

namespace cheat::feature 
{
	static app::Byte__Array* RecordUserData_Hook(int32_t nType)
	{
		auto& inst = ProtectionBypass::GetInstance();

		return inst.OnRecordUserData(nType);
	}

    ProtectionBypass::ProtectionBypass() : Feature(),
        NFF(m_Enabled, "Disable protection", "m_DisableMhyProt", "General", true),
		m_CorrectSignatures({})
    {
		HookManager::install(app::Unity_RecordUserData, RecordUserData_Hook);
    }

	void ProtectionBypass::Init()
	{
		for (int i = 0; i < 4; i++) {
			LOG_TRACE("Emulating call of RecordUserData with type %d", i);
			app::Application_RecordUserData(nullptr, i, nullptr);
		}

		if (m_Enabled) {
			LOG_TRACE("Trying to close mhyprot handle.");
			if (CloseHandleByName(L"\\Device\\mhyprot2"))
				LOG_INFO("Mhyprot2 handle successfuly closed. Happy hacking ^)");
			else
				LOG_ERROR("Failed closing mhyprot2 handle. Maybe dev updated anti-cheat.");
		}

		LOG_DEBUG("Initialized");
	}

    const FeatureGUIInfo& ProtectionBypass::GetGUIInfo() const
    {
        static const FeatureGUIInfo info { "", "Settings", false };
        return info;
    }

    void ProtectionBypass::DrawMain()
    {
		ConfigWidget(m_Enabled, 
			"Closing handle of mhyprot2.sys driver. (Changes will take effect after next launch)\n" \
            "It allows reading / writing process memory from another process.\n" \
            "For example, this allows Cheat engine read process.");
    }

    ProtectionBypass& ProtectionBypass::GetInstance()
    {
        static ProtectionBypass instance;
        return instance;
    }

	app::Byte__Array* ProtectionBypass::OnRecordUserData(int32_t nType)
	{
		if (m_CorrectSignatures.count(nType))
		{
			auto byteClass = app::GetIl2Classes()[0x25];

			auto& content = m_CorrectSignatures[nType];
			auto newArray = (app::Byte__Array*)il2cpp_array_new(byteClass, content.size());
			memmove_s(newArray->vector, content.size(), content.data(), content.size());

			return newArray;
		}

		app::Byte__Array* result = callOrigin(RecordUserData_Hook, nType);
		auto resultArray = ToUniArray(result, byte);

		auto length = resultArray->length();
		if (length == 0)
			return result;

		auto stringValue = std::string((char*)result->vector, length);
		m_CorrectSignatures[nType] = stringValue;

		LOG_DEBUG("Sniffed correct signature for type %d value '%s'", nType, stringValue.c_str());

		return result;
	}
}

