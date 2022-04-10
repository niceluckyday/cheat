#include <pch.h>
#include "Settings.h"

#include <cheat-base/render/gui-util.h>

namespace cheat::feature 
{
    Settings::Settings() : Feature(),
        NF(m_MenuKey,    "Show cheat menu key", "General", Hotkey(VK_F1)),
		
		NF(m_StatusMove, "Move status window", "General", true),
		NF(m_StatusShow, "Show status window", "General", true),
		
		NF(m_InfoMove,   "Move info window", "General", true),
		NF(m_InfoShow,   "Show info window", "General", true),

		NF(m_ConsoleLogging, "Console logging", "General", true),
		NF(m_FileLogging,    "File logging",    "General", false)
    {

    }

    const FeatureGUIInfo& Settings::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "", "Settings", false };
        return info;
    }

    void Settings::DrawMain()
    {

		ConfigWidget(m_MenuKey, false,
			"Key to toggle this menu visibility. Cannot be empty.\nIf you forget this key, you can see it in config file.");

		BeginGroupPanel("Logging", ImVec2(-1, 0));
		{
			bool consoleChanged = ConfigWidget(m_ConsoleLogging,
				"Enable console for logging information. (Enabling will take effect after next launch)");
			if (consoleChanged && !m_ConsoleLogging) 
			{
				Logger::SetLevel(Logger::Level::None, Logger::LoggerType::ConsoleLogger);
			}

			bool fileLogging = ConfigWidget(m_FileLogging,
				"Enable file logging. (Enabling will take effect after next launch)\n" \
				"That's mean that in cheat directory will be created folder which will be contain file with logs.");
			if (fileLogging && !m_FileLogging) 
			{
				Logger::SetLevel(Logger::Level::None, Logger::LoggerType::FileLogger);
			}
		}
		EndGroupPanel();

		BeginGroupPanel("Status window", ImVec2(-1, 0));
		{
			ConfigWidget(m_StatusShow);
			ConfigWidget(m_StatusMove, "Give able to move 'Status' window.");
		}
		EndGroupPanel();

		BeginGroupPanel("Info window", ImVec2(-1, 0));
		{
			ConfigWidget(m_InfoShow);
			ConfigWidget(m_InfoMove, "Give able to move 'Info' window.");
		}
		EndGroupPanel();
    }

    Settings& Settings::GetInstance()
    {
        static Settings instance;
        return instance;
    }
}

