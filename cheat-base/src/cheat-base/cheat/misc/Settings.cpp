#include <pch.h>
#include "Settings.h"

#include <cheat-base/render/gui-util.h>

namespace cheat::feature 
{
    Settings::Settings() : Feature(),
        NF(m_MenuKey,    "Show Cheat Menu Key", "General", Hotkey(VK_F1)),
		
		NF(m_StatusMove, "Move Status Window", "General", true),
		NF(m_StatusShow, "Show Status Window", "General", true),
		
		NF(m_InfoMove,   "Move Info Window", "General", true),
		NF(m_InfoShow,   "Show Info Window", "General", true),
		
		NF(m_FpsMove, "Move FPS Indicator", "General", false),
		NF(m_FpsShow, "Show FPS Indicator", "General", true),

		NF(m_NotificationsShow, "Show Notifications", "General", true), 

		NF(m_ConsoleLogging, "Console Logging", "General", true),
		NF(m_FileLogging,    "File Logging",    "General", false),
		NF(m_HotkeysEnabled, "Hotkeys Enabled", "General", true)
    {

    }

    const FeatureGUIInfo& Settings::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "", "Settings", false };
        return info;
    }

	void Settings::DrawMain()
	{

		BeginGroupPanel("General", ImVec2(-1, 0));
		{
			ConfigWidget(m_MenuKey, false,
				"Key to toggle main menu visibility. Cannot be empty.\n"\
				"If you forget this key, you can see or set it in your config file.");
			ConfigWidget(m_HotkeysEnabled, "Enable hotkeys.");
		}
		EndGroupPanel();

		BeginGroupPanel("Logging", ImVec2(-1, 0));
		{
			bool consoleChanged = ConfigWidget(m_ConsoleLogging,
				"Enable console for logging information (changes will take effect after relaunch)");
			if (consoleChanged && !m_ConsoleLogging)
			{
				Logger::SetLevel(Logger::Level::None, Logger::LoggerType::ConsoleLogger);
			}

			bool fileLogging = ConfigWidget(m_FileLogging,
				"Enable file logging (changes will take effect after relaunch).\n" \
				"A folder in the app directory will be created for logs.");
			if (fileLogging && !m_FileLogging)
			{
				Logger::SetLevel(Logger::Level::None, Logger::LoggerType::FileLogger);
			}
		}
		EndGroupPanel();

		BeginGroupPanel("Status Window", ImVec2(-1, 0));
		{
			ConfigWidget(m_StatusShow);
			ConfigWidget(m_StatusMove, "Allow moving of 'Status' window.");
		}
		EndGroupPanel();

		BeginGroupPanel("Info Window", ImVec2(-1, 0));
		{
			ConfigWidget(m_InfoShow);
			ConfigWidget(m_InfoMove, "Allow moving of 'Info' window.");
		}
		EndGroupPanel();

		BeginGroupPanel("FPS indicator", ImVec2(-1, 0));
		{
			ConfigWidget(m_FpsShow);
			ConfigWidget(m_FpsMove, "Allow moving of 'FPS Indicator' window.");
		}
		EndGroupPanel();

		BeginGroupPanel("Show Notifications", ImVec2(-1, 0));
		{
			ConfigWidget(m_NotificationsShow, "Notifications on the bottom-right corner of the window will be displayed.");
		}
		EndGroupPanel();
	}

    Settings& Settings::GetInstance()
    {
        static Settings instance;
        return instance;
    }
}

